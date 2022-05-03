#pragma once

// Allow spdlog to print wstrings

#ifndef SPDLOG_WCHAR_TO_UTF8_SUPPORT
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT 
#endif // !SPDLOG_WCHAR_TO_UTF8_SUPPORT

#pragma warning(push)
#pragma warning(disable:26495) 
#pragma warning(disable:26498) 
#pragma warning(disable:26812) 
#pragma warning(disable:26800) 
#pragma warning(disable:6285) 
#pragma warning(disable:26437)
#pragma warning(disable:26451) 
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

class Log
{
public:

	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:

	static std::shared_ptr<spdlog::logger> s_Logger;

};

inline std::shared_ptr<spdlog::logger> Log::s_Logger;

inline void Log::Init()
{
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_Logger = spdlog::stdout_color_mt("Logger");
	s_Logger->set_level(spdlog::level::info);
}

// Core Log Macros.
#define LOG_FATAL(...)		Log::GetLogger()->fatal(__VA_ARGS__)
#define LOG_ERROR(...)		Log::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...)		Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...)		Log::GetLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...)		Log::GetLogger()->trace(__VA_ARGS__)

