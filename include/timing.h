#ifndef TIMING_H
#define TIMING_H

#ifdef _WIN32

#define CLOCK_REALTIME 0
#include <windows.h>

struct timespec
{
	long tv_sec;
	long tv_nsec;
};

#define exp7           10000000LL     //1E+7     //C-file part
#define exp9         1000000000LL     //1E+9
#define w2ux 116444736000000000LL     //1.jan1601 to 1.jan1970

void unix_time(struct timespec *spec)
{
	__int64 wintime;
	GetSystemTimeAsFileTime((FILETIME*)&wintime); 
	wintime -= w2ux;
	spec->tv_sec  = wintime / exp7;                 
	spec->tv_nsec = wintime % exp7 *100;
}

int clock_gettime(int, timespec *spec)
{
	static struct timespec startspec;
	static double ticks2nano;
	static __int64 startticks, tps = 0;
	__int64 tmp, curticks;

	// A system can possibly change the freq.
	QueryPerformanceFrequency((LARGE_INTEGER*)&tmp);
	if (tps != tmp)
	{
		tps = tmp;
		QueryPerformanceCounter((LARGE_INTEGER*)&startticks);
		unix_time(&startspec);
		ticks2nano = (double)exp9 / tps;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&curticks); curticks -=startticks;
	spec->tv_sec = startspec.tv_sec + (curticks / tps);
	spec->tv_nsec = startspec.tv_nsec + (double)(curticks % tps) * ticks2nano;

	if (!(spec->tv_nsec < exp9))
	{
		spec->tv_sec++;
		spec->tv_nsec -=exp9;
	}

	return 0;
}

#else

#include <time.h>

#endif // _WIN32

// Get the timer value in milliseconds
static void get_time(uint64_t* ret)
{
	volatile struct timespec val;
	clock_gettime(CLOCK_REALTIME, (struct timespec*)&val);
	*ret = val.tv_nsec / 1e6 + 1000 * val.tv_sec;
}

#endif // TIMING_H

