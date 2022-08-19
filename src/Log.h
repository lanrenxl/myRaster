#pragma once

#include <memory> // for use shared_ptr<>

#include <spdlog/spdlog.h>

class Log
{
public:
    static void Init();
    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;//for engine use
};

// Core log macros
#define RT_TRACE(...)    ::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RT_INFO(...)     ::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RT_WARN(...)     ::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RT_ERROR(...)    ::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RT_CRITICAL(...) ::Log::GetCoreLogger()->critical(__VA_ARGS__)