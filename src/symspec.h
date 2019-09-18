#pragma once

#ifndef SYMSPEC_EXTERN
#define SYMSPEC_EXTERN extern
#endif

#ifndef SYMSPEC_EXTERN_C
#ifdef __cplusplus
#define SYMSPEC_EXTERN_C SYMSPEC_EXTERN "C"
#else
#define SYMSPEC_EXTERN_C SYMSPEC_EXTERN
#endif
#endif

#ifndef SYMSPEC_CDECL
#if (defined _MSC_VER) || (defined __MINGW32__)
#define SYMSPEC_CDECL __cdecl
#elif defined __GNUC__
#define SYMSPEC_CDECL __attribute__((cdecl))
#else
#error "Unsupported compiler"
#endif
#endif

#ifndef SYMSPEC_WINAPI
#ifdef _WIN32
#define SYMSPEC_WINAPI WINAPI
#else
#define SYMSPEC_WINAPI
#endif
#endif

#ifndef SYMSPEC_EXPORT
#ifdef STATIC
#define SYMSPEC_EXPORT
#else
#ifdef _MSC_VER
#define SYMSPEC_EXPORT __declspec(dllexport)
#elif defined __MINGW32__
#define SYMSPEC_EXPORT __declspec(dllexport)
#elif defined __GNUC__
#define SYMSPEC_EXPORT __attribute__((visibility("default")))
#else
#error "Unsupported compiler"
#endif
#endif
#endif

#ifndef SYMSPEC_IMPORT
#ifdef STATIC
#define SYMSPEC_IMPORT
#else
#ifdef _MSC_VER
#define SYMSPEC_IMPORT __declspec(dllimport)
#elif defined __MINGW32__
#define SYMSPEC_IMPORT __declspec(dllimport)
#elif defined __GNUC__
#define SYMSPEC_IMPORT
#else
#error "Unsupported compiler"
#endif
#endif
#endif

#ifndef SYMSPEC_API
#if defined FOO_IMPLEMENTATION
#define SYMSPEC_API SYMSPEC_EXPORT
#else
#define SYMSPEC_API SYMSPEC_IMPORT
#endif
#endif
