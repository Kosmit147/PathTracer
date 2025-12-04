#pragma once

#include <spdlog/spdlog.h>

#define PRESENTER_TRACE(...) ::spdlog::trace(__VA_ARGS__)
#define PRESENTER_DEBUG(...) ::spdlog::debug(__VA_ARGS__)
#define PRESENTER_INFO(...) ::spdlog::info(__VA_ARGS__)
#define PRESENTER_WARN(...) ::spdlog::warn(__VA_ARGS__)
#define PRESENTER_ERROR(...) ::spdlog::error(__VA_ARGS__)
#define PRESENTER_CRITICAL(...) ::spdlog::critical(__VA_ARGS__)
