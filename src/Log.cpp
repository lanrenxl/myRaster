#include "Log.h"
#include <spdlog/sinks/stdout_color_sinks.h>//stdout_color_mt接口需要

std::shared_ptr<spdlog::logger> Log::s_CoreLogger;

void Log::Init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");

    s_CoreLogger = spdlog::stdout_color_mt("Lanrenl");
    s_CoreLogger->set_level(spdlog::level::trace);
    RT_INFO("Engine start");
}