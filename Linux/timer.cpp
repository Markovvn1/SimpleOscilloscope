#include "timer.hpp"

#include <time.h>

long long getCTMillisecond()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	return (long long)(time.tv_sec) * 1000 + time.tv_nsec / 1000000;
}

long long getCTMicrosecond()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	return (long long)(time.tv_sec) * 1000000 + time.tv_nsec / 1000;
}

long long getCTNanosecond()
{
	timespec time;
	clock_gettime(CLOCK_REALTIME, &time);
	return (long long)(time.tv_sec) * 1000000000 + time.tv_nsec;
}
