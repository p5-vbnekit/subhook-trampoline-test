#include "foo.hpp"

#include <sstream>
#include <iostream>


SYMSPEC_EXTERN_C void SYMSPEC_API foo(int value) {
    try {
        ::std::ostringstream stream;
        stream << "foo: " << "value = " << value << ::std::endl;
        ::std::clog << stream.str() << ::std::flush;
    }

    catch(...) {}
}
