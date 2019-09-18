#include <cerrno>

#include <list>
#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <functional>
#include <system_error>

#include <subhook.h>

#include "foo.hpp"


struct Global final {
    Foo *trampoline;

    inline static auto & instance() noexcept(true) { static Global instance; return instance; }

private:
    Global() noexcept(true) = default;
    template <class ... T> Global(T && ...) = delete;
    template <class T> Global & operator = (T &&) = delete;
};

static void foo_replacement(int value) noexcept(true) {
    auto * const trampoline = Global::instance().trampoline;

    try {
        ::std::ostringstream stream;
        stream << "foo_replacement: " << "value = " << value << ", trampoline = " << reinterpret_cast<void *>(trampoline) << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
    }

    catch(...) {}

    if (static_cast<bool>(trampoline)) {
        ::std::ostringstream stream;
        stream << "foo_replacement: " << "executing trampoline [" << reinterpret_cast<void *>(trampoline) << "]..." << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;

        // WILL RAISE UNDEFINED BEHAVIOUR:
        // 1. under x86_64 with SUBHOOK_64BIT_OFFSET
        // 2. under x86 with size-optimized foo function build (gcc "-Os" flag)
        trampoline(1 + value);
    }
}

void routine() noexcept(false) {
    errno = 0;
    auto * const hook = ::subhook_new(reinterpret_cast<void *>(::foo), reinterpret_cast<void *>(::foo_replacement),
        ::SUBHOOK_64BIT_OFFSET /// !!!ON X86_64 TRAMPLOLINE CALL WITH THIS FLAG WILL RAISE UNDEFINED BEHAVIOUR!!! ///
        // it is not necessary if foo is statically linked.
        // but i need it, because owtherwise i have an error #75 (EOVERFLOW)
        // while i trying to hook x86_64 shared library.
        // & i hope, this flag will be ignored on x86. =)
    );
    auto errorCode = errno;

    struct Finalizer final {
        ::std::list<::std::function<void(void)>> actions;

        Finalizer() noexcept(true) = default;
        Finalizer(Finalizer &&) noexcept(true) = default;
        Finalizer & operator = (Finalizer &&) noexcept(true) = default;

        inline ~Finalizer() noexcept(true) {
            auto &&actions = ::std::move(this->actions);
            for (auto const &action: actions) if (static_cast<bool>(action)) { try { action(); } catch(...) {} }
        }

    private:
        Finalizer(Finalizer const &) = delete;
        Finalizer & operator = (Finalizer const &) = delete;
    };

    Finalizer finalizer;
    if (static_cast<bool>(hook)) finalizer.actions.push_front([hook] () { ::subhook_free(hook); });

    {
        ::std::ostringstream stream;
        stream << "main: hook = " << reinterpret_cast<void *>(hook) << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
    }

    if (! static_cast<bool>(hook)) {
        constexpr static auto const * const what = "failed to initialize hook";
        if (0 != errorCode) throw ::std::system_error{0 < errorCode ? +errorCode : -errorCode, ::std::generic_category(), what};
        throw ::std::runtime_error{what};
    }

    {
        ::std::ostringstream stream;
        stream << "main: executing foo..." << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
        foo(1);
    }

    {
        errorCode = ::subhook_install(hook);
        if (0 != errorCode) throw ::std::system_error{0 < errorCode ? +errorCode : -errorCode, ::std::generic_category(), "failed to install hook"};
        finalizer.actions.push_front([hook] () { ::subhook_remove(hook); });
        ::std::ostringstream stream;
        stream << "main: hook installed" << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
    }

    {
        ::std::ostringstream stream;
        stream << "main: executing foo..." << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
        foo(1);
    }

    {
        errno = 0; auto * const trampoline = ::subhook_get_trampoline(hook); errorCode = errno;
        ::std::ostringstream stream;
        stream << "main: trampoline = " << trampoline << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
        if (! static_cast<bool>(trampoline)) {
            constexpr static auto const * const what = "failed to initialize hook trampoline";
            if (0 != errorCode) throw ::std::system_error{0 < errorCode ? +errorCode : -errorCode, ::std::generic_category(), what};
            throw ::std::runtime_error{what};
        }

        Global::instance().trampoline = reinterpret_cast<::std::decay_t<decltype(Global::instance().trampoline)>>(trampoline);
    }

    {
        ::std::ostringstream stream;
        stream << "main: executing foo..." << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
        foo(1);
    }
}

int main(int, char **) {
    try { routine(); return 0; }

    catch(::std::system_error const &e) {
        try { ::std::cerr << "exception caught: " << e.what() << ::std::endl << ::std::flush; } catch(...) {}
        auto const code = e.code().value();
        if ((0 != code) && (e.code().category() == ::std::generic_category())) return code;
    }

    catch(::std::exception const &e) { try { ::std::cerr << "exception caught: " << e.what() << ::std::endl << ::std::flush; } catch(...) {} }
    catch(...) { try { ::std::cerr << "unknown exception caught" << ::std::endl << ::std::flush; } catch(...) {} }

    return 1;
}
