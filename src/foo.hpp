#pragma once

#include <type_traits>
#include "symspec.h"


SYMSPEC_EXTERN_C void SYMSPEC_API foo(int);
using Foo = ::std::remove_pointer_t<::std::decay_t<decltype(foo)>>;
