#ifndef AIUI_COMMON_HDR_X2342Y3
#define AIUI_COMMON_HDR_X2342Y3

#if defined(WIN32)
#    ifndef AIUI_WINDOWS
#        define AIUI_WINDOWS
#    endif
#endif

#if defined(AIUI_WINDOWS)
#    if !defined(__MINGW32__)
typedef long ssize_t;
typedef unsigned long pid_t;
#    endif

#    undef AIUIEXPORT
#    if defined(AIUI_LIB_COMPILING)
#        define AIUI_DEPRECATED __declspec(deprecated)
#        define AIUIEXPORT      __declspec(dllexport)
#        define AIUIHIDDEN
#        define AIUIAPI     __stdcall
#        define AIUI_INLINE inline
#    else
#        define AIUI_DEPRECATED __declspec(deprecated)
#        define AIUIEXPORT
#        define AIUIHIDDEN
#        define AIUIAPI     __stdcall
#        define AIUI_INLINE inline
#    endif
#else
#    undef AIUIEXPORT
#    define AIUI_DEPRECATED __attribute__((deprecated))
#    define AIUIEXPORT      __attribute__((visibility("default")))
#    define AIUIHIDDEN      __attribute__((visibility("hidden")))
#    undef AIUIAPI
#    define AIUIAPI
#    define AIUI_INLINE __attribute__((always_inline))
#endif

#ifndef __cplusplus
#    define __AIUI_PREFIX__(x) AIUI_##x
#    define AIUI_PREFIX(x)     __AIUI_PREFIX__(x)
#else
#    define AIUI_PREFIX(x) x
#endif
#endif
