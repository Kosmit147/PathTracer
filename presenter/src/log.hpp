#pragma once

#include <spdlog/spdlog.h>

#define PT_TRACE(...) ::spdlog::trace(__VA_ARGS__)
#define PT_DEBUG(...) ::spdlog::debug(__VA_ARGS__)
#define PT_INFO(...) ::spdlog::info(__VA_ARGS__)
#define PT_WARN(...) ::spdlog::warn(__VA_ARGS__)
#define PT_ERROR(...) ::spdlog::error(__VA_ARGS__)
#define PT_CRITICAL(...) ::spdlog::critical(__VA_ARGS__)
