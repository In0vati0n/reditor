/**
 * 终端渲染相关逻辑
 *
 * Author: In0vati0n
 * Date: 2021/08/08
 */

#ifndef __RE_COMMON__
#define __RE_COMMON__

#define RE_LUA_GLOBAL_NAME "reditor"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
    //define something for Windows (32-bit and 64-bit, this part is common)
    #define RE_PLATFORM_WINDOWS
    #ifdef _WIN64
        #define RE_PLATFORM_WINDOWS_64
    #else
        #define RE_PLATFORM_WINDOWS_32
    #endif
#elif __APPLE__
    #define RE_PLATFORM_APPLE
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        #define RE_PLATFORM_IPHONE_SIMULATOR
    #elif TARGET_OS_IPHONE
        #define RE_PLATFORM_IPHONE
    #elif TARGET_OS_MAC
        #define RE_PLATFORM_MACOS
    #else
        #error "Unknown Apple platform"
    #endif
#elif __linux__
    #define RE_PLATFORM_LINUX
#elif __unix__ // all unices not caught above
    #define RE_PLATFORM_UNIX
#elif defined(_POSIX_VERSION)
    #define RE_PLATFORM_POSIX
#else
    #error "Unknown compiler"
#endif

// DLL
#ifdef RE_PLATFORM_WINDOWS
    #ifdef RE_DYNAMIC_LINK
        #ifdef RE_BUILD
            #define RE_API __declspec(dllexport)
        #else
            #define RE_API __declspec(dllimport)
        #endif
    #else
    #endif
#else
    #define RE_API extern
#endif

#endif // __RE_COMMON__