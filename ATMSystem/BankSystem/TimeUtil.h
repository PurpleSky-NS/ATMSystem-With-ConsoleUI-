#pragma once

#include <string>
#include <ctime>

inline time_t CurrentTimeStape()
{
	return time(nullptr);
}

inline std::string CurrentTimeStr()
{
	time_t currentTime = time(nullptr);
	char tmBuf[32];
	strftime(tmBuf, sizeof(tmBuf), "%Y-%m-%d %H:%M:%S", localtime(&currentTime));
	return tmBuf;
}

inline std::string TimeStr(time_t time)
{
	char tmBuf[32];
	strftime(tmBuf, sizeof(tmBuf), "%Y-%m-%d %H:%M:%S", localtime(&time));
	return tmBuf;
}
