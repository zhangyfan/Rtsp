#ifndef _RWALG_MACRO_H__
#define _RWALG_MACRO_H__

#ifdef __cplusplus
    #define RWALG_API_BEGIN extern "C" {
    #define RWALG_API_END }
#else
    #define RWALG_API_BEGIN
    #define RWALG_API_END
#endif

#if RWALG_LIBRARY
    #if defined(_WIN32)
        #if RWALG_DLLEXPORT
            #define RWALG_API __declspec(dllexport)
        #else
            #define RWALG_API __declspec(dllimport)
        #endif
    #else
        #if RWALG_DLLEXPORT
            #define RWALG_API __attribute__((visibility("default")))
        #else
            #define RWALG_API
        #endif
    #endif
#else
    #define RWALG_API
#endif

#ifdef __GNUC__
    #define RWALG_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
    #define RWALG_DEPRECATED __declspec(deprecated)
#else
    #define RWALG_DEPRECATED
#endif

#endif /*_RWALG_MACRO_H__*/

