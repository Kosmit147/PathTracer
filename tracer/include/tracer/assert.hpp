#pragma once

#include <cstdlib>
#include <iostream>
#include <source_location>

#if defined(PT_DEBUG_BREAKS)

    #if defined(_MSC_VER)

        #define TRACER_DEBUG_BREAK __debugbreak()

    #elif defined(__GNUC__)

        #define TRACER_DEBUG_BREAK __builtin_trap()

    #elif defined(__clang__)

        #define TRACER_DEBUG_BREAK __builtin_debugtrap()

    #endif

#else

    #define TRACER_DEBUG_BREAK ((void)(0))

#endif

#define TRACER_ASSERT_IMPL(...)                                                                                        \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((__VA_ARGS__)) [[likely]]                                                                                  \
        {}                                                                                                             \
        else [[unlikely]]                                                                                              \
        {                                                                                                              \
            auto source_location = std::source_location::current();                                                    \
            std::cerr << source_location.file_name() << '(' << source_location.line() << ':'                           \
                      << source_location.column() << ") `" << source_location.function_name()                          \
                      << "`:\nAssertion failed: (" << #__VA_ARGS__ << ")\n";                                           \
            TRACER_DEBUG_BREAK;                                                                                        \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (false)

#if defined(PT_ASSERTS)

    #define TRACER_ASSERT(...) TRACER_ASSERT_IMPL(__VA_ARGS__)

#else

    #define TRACER_ASSERT(...) ((void)(0))

#endif

#define TRACER_RUNTIME_ASSERT(...) TRACER_ASSERT_IMPL(__VA_ARGS__)
