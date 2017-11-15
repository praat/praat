/* melder_time.cpp
 *
 * Copyright (C) 1992-2008,2011,2014-2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"

#if defined (macintosh) || defined (UNIX)
	#include <time.h>
	#include <sys/time.h>
	#include <unistd.h>
#elif defined (_WIN32)
	#include <windows.h>
#endif

/*
#include <assert.h>
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

uint64_t GetPIDTimeInNanoseconds(void)
{
    uint64_t        start;
    uint64_t        end;
    uint64_t        elapsed;
    uint64_t        elapsedNano;
    static mach_timebase_info_data_t    sTimebaseInfo;

    // Start the clock.

    start = mach_absolute_time();

    // Call getpid. This will produce inaccurate results because 
    // we're only making a single system call. For more accurate 
    // results you should call getpid multiple times and average 
    // the results.

    (void) getpid();

    // Stop the clock.

    end = mach_absolute_time();

    // Calculate the duration.

    elapsed = end - start;

    // Convert to nanoseconds.

    // If this is the first time we've run, get the timebase.
    // We can use denom == 0 to indicate that sTimebaseInfo is 
    // uninitialised because it makes no sense to have a zero 
    // denominator is a fraction.

    if ( sTimebaseInfo.denom == 0 ) {
        (void) mach_timebase_info(&sTimebaseInfo);
    }

    // Do the maths. We hope that the multiplication doesn't 
    // overflow; the price you pay for working in fixed point.

    elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;

    return elapsedNano;
}
*/

double Melder_clock () {
	#if defined (macintosh) || defined (UNIX)
		/*
		 * The clock counts the number of seconds elapsed since 1969.
		 */
		struct timeval timeVal;
		struct timezone timeZone;
		gettimeofday (& timeVal, & timeZone);
		return timeVal. tv_sec + 1e-6 * timeVal. tv_usec;
	#elif defined (_WIN32)
		/*
		 * The clock counts the number of ticks since system start-up.
		 */
		static double clockFrequency = -1.0;   // we can use a static, because the clock frequency does not change while the computer is running
		if (clockFrequency == -1.0) {   // not initialized?
			LARGE_INTEGER clockFrequency_longlong;
			QueryPerformanceFrequency (& clockFrequency_longlong);   // returns 0 if the system does not have a performance counter
			clockFrequency = (double) clockFrequency_longlong.QuadPart;   // the compiler has to support 64-bit integers
		}
		if (clockFrequency == 0.0) {   // this will be true if the system does not have a performance counter
			return GetTickCount () / 1000.0;   // fallback: only millisecond resolution, and potentially jumpy
		}
		LARGE_INTEGER clockCount;
		QueryPerformanceCounter (& clockCount);
		return (double) clockCount.QuadPart / clockFrequency;
	#else
		return 0;
	#endif
}

double Melder_stopwatch () {
	static double lastTime;
	double now = Melder_clock ();
	double timeElapsed = lastTime == 0 ? -1.0 : now - lastTime;
	//Melder_casual ("%ld %ld %ld %lf %lf", now, lastTime, now - lastTime, (now - lastTime) / (double) CLOCKS_PER_SEC, timeElapsed);
	lastTime = now;
	return timeElapsed;
}

void Melder_sleep (double duration) {
	if (duration <= 0.0) return;   // already past end time
	#if defined (_WIN32)
		Sleep (duration * 1e3);
	#elif defined (macintosh) || defined (UNIX)
		unsigned int seconds = (unsigned int) duration;
		unsigned int microseconds = (unsigned int) ((duration - seconds) * 1e6);
		if (seconds > 0) sleep (seconds);
		if (microseconds > 0) usleep (microseconds);
	#endif
}

/* End of file melder_time.cpp */
