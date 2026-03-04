#pragma once

#include "Graphics/Widget/VirtualWindowContent.h"
#include "Tool/Logger.h"

#include "Utils/Thread/SceneJobQueue.h"

class LogPrinter : public SceneJobQueue
{
public:
	void Print(std::vector<LogCommand> commands, LogLevelType level)
	{
		for (const auto& command : commands)
		{
			_mLogs.push_back(command);
			if (_mLogs.size() > _mMaxLogCount)
			{
				_mLogs.pop_front();
			}
		}

		if (_mCurrentLevel != level)
		{
			_mCurrentLevel = level;

			// 전부 리드로우
		}
		else
		{
			// 추가 드로우
		}
	}

private:
	LogLevelType _mCurrentLevel = LogLevelType::Log;
	const uint32 _mMaxLogCount = 100;
	std::deque<LogCommand> _mLogs;
};

class LogVirtualWindow : public VirtualWindowContent
{
	GEN_REFLECTION(LogVirtualWindow)

public:
	const std::shared_ptr<LogPrinter> GetPrinter() const
	{
		return _mPrinter;
	}

private:
	std::shared_ptr<LogPrinter> _mPrinter;
};

