#pragma once

#include <format>
#include "Utils/StringUtils.h"

class LogViewport;

enum class LogLevelType
{
	Log = 0,
	Warring,
	Error
};

struct LogCommand
{
	LogLevelType mLevel;
	std::string mStr;
};

/**
 * 특정 스레드에서 랜더 스레드에 로그 기록을 밀어넣어주는 Logger 클래스
 */
class Logger
{
public:
	template<typename... Args>
	void LogMsg(const std::string& format, Args&&... args);
	template<typename... Args>
	void LogMsg(const std::wstring& format, Args&&... args);
	template<typename... Args>
	void LogMsg(LogLevelType type, const std::string& format, Args&&... args);
	template<typename... Args>
	void LogMsg(LogLevelType type, const std::wstring& format, Args&&... args);

public:
	// 뷰포트에 신규 데이터 밀어넣기
	void PrintLogs(std::shared_ptr<LogViewport> viewport) const;

private:
	std::vector<LogCommand> _mAccLogs;
};

template<typename ...Args>
inline void Logger::LogMsg(const std::string& format, Args && ...args)
{
	LogMsg(LogLevelType::Log, format, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::LogMsg(const std::wstring& format, Args && ...args)
{
	LogMsg(LogLevelType::Log, format, std::forward<Args>(args)...);
}

template<typename ...Args>
inline void Logger::LogMsg(LogLevelType type, const std::string& format, Args && ...args)
{
#ifdef _DEBUG
	std::string log = std::format(format, std::forward<Args>(args)...);
	log.append('\n');

	_mAccLogs.push_back({ type, log });
#endif
}

template<typename ...Args>
inline void Logger::LogMsg(LogLevelType type, const std::wstring& format, Args && ...args)
{
#ifdef _DEBUG
	std::wstring log = std::format(format, std::forward<Args>(args)...);
	log.append(L'\n');

	_mAccLogs.push_back({ type, ConvertWStringToUtf8(log) });
#endif
}
