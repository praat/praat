/* melder_time.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2008/01/30 moved from Melder_audio.c
 * pb 2008/01/31 Win: use QueryPerformanceCounter
 * pb 2008/03/18 moved Melder_stopwatch here and made it call Melder_clock
 * pb 2011/04/05 C++
 */

#include "melder.h"

#if defined (macintosh) || defined (UNIX)
	#include <sys/time.h>
#elif defined (_WIN32)
	#include <windows.h>
#endif

double Melder_clock (void) {
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

double Melder_stopwatch (void) {
	static double lastTime;
	double now = Melder_clock ();
	double timeElapsed = lastTime == 0 ? -1.0 : now - lastTime;
	//Melder_casual ("%ld %ld %ld %lf %lf", now, lastTime, now - lastTime, (now - lastTime) / (double) CLOCKS_PER_SEC, timeElapsed);
	lastTime = now;
	return timeElapsed;
}

/* End of file melder_time.cpp */
