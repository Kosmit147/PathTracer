#pragma once

#include <cstdlib>
#include <iostream>
#include <source_location>

// TODO: Add a compile option to toggle asserts.
#define PT_ASSERT(...)                                                                                                 \
    do                                                                                                                 \
    {                                                                                                                  \
        if ((__VA_ARGS__)) [[likely]]                                                                                  \
        {}                                                                                                             \
        else [[unlikely]]                                                                                              \
        {                                                                                                              \
            std::source_location source_location = std::source_location::current();                                    \
            std::cerr << source_location.file_name() << '(' << source_location.line() << ':'                           \
                      << source_location.column() << ") `" << source_location.function_name()                          \
                      << "`:\nAssertion failed: (" << #__VA_ARGS__ << ")\n";                                           \
            std::abort();                                                                                              \
        }                                                                                                              \
    } while (false)
