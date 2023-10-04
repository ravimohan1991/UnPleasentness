#pragma once

#include "windows.h"
#include "iostream"

/*
 *  FUNCTION: log_add(const char *fmt, ...)
 *
 *  PURPOSE: log information to a file with timestamps.
 *
 *  COMMENTS:
 *
 */
void log_add(const char* fmt, ...)
{
	va_list va_alist;
	static char logbuf[1024];
	if (fmt == NULL)
	{
		return;
	}

	//FILE* fp;
	struct tm* current_tm;
	time_t current_time;
	time(&current_time);
	current_tm = localtime(&current_time);

	sprintf(logbuf, "%02d:%02d:%02d - ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);
	va_start(va_alist, fmt);
	_vsnprintf(logbuf + strlen(logbuf), sizeof(logbuf) - strlen(logbuf), fmt, va_alist);
	va_end(va_alist);

	printf("%s\n", logbuf);
}