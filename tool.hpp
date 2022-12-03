#include <ctime>
#include <stdio.h>
#include <iostream>
#include <string>
time_t getDate(const char* str); //获取指定时间的时间戳
time_t getDate(const char* str); //获取当前时间的时间戳

time_t getDate() {
	return time(0);
}

time_t getDate(const char* str)
{
	tm tm_;
	int year, month, day, hour, minute, second;
	sscanf_s(str, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;

	time_t t_ = mktime(&tm_); //已经减了8个时区
	return t_; //秒时间  
}