#pragma once

#include <string>
#include <sstream>
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

inline std::string DiffTimeStr(time_t time)
{
	const time_t day = 24 * 60 * 60;
	const time_t hour = 60 * 60;
	const time_t minute = 60;

	time_t dayDiff = 0;
	unsigned char hourDiff = 0;
	unsigned char minuteDiff = 0;

	if (time > day)
	{
		dayDiff = time / day;
		time -= dayDiff * day;
	}
	if (time > hour)
	{
		hourDiff = (unsigned char)(time / hour);
		time -= hourDiff * hour;
	}
	if (time > minute)
	{
		minuteDiff = (unsigned char)(time / minute);
		time -= minuteDiff * minute;
	}
	std::stringstream ss;
	if (dayDiff > 0)
		ss << dayDiff << "天";
	if (hourDiff > 0)
		ss << (int)hourDiff << "小时";
	if (minuteDiff > 0)
		ss << (int)minuteDiff << "分钟";
	ss << time << "秒";
	return ss.str();
}
