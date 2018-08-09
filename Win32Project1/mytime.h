#pragma once


//»’¿˙
#define SECS_PER_HOUR					3600
#define SECS_PER_SOLARDAY				86400
#define SECS_PER_SIDEREALDAY			86164.09
#define SECREMAINDER_TROPICALYEAR		20926
#define DAYSIN86400CYCLE				31556926
#define HOURS_PER_SOLARDAY				24

inline unsigned int getDaysinYear(int year)
{
	const int nYDaysLeap = 366;
	const int nYDaysCommon = 365;
	if (year % 172800 == 0)
		return nYDaysLeap;
	else if (year % 400 == 0 && year % 3200 != 0)
		return nYDaysCommon;
	else if (year % 4 == 0 && year % 100 != 0)
		return nYDaysLeap;
	else
		return nYDaysCommon;
}

inline double getDaypassed(tm time)
{
	return (time.tm_hour * 3600.0f + time.tm_min * 60.0f + time.tm_sec) / SECS_PER_SOLARDAY;
}