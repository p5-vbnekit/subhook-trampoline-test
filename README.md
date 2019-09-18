# subhook-trampoline-test
This program demonstrates undefined behavior of subhook trampoline calls.

## 1. under x86_64 with SUBHOOK_64BIT_OFFSET
I need it, because owtherwise i have an error #75 (EOVERFLOW) while i trying to hook x86_64 shared library.

## 2. under x86 with size-optimized foo function build
If the object with the target function is built with size optimization (`gcc -Os` | `cmake -DCMAKE_BUILD_TYPE=MinSizeRel`).
