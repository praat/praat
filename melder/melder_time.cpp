/* melder_time.cpp
 *
 * Copyright (C) 1992-2008,2011,2014-2018,2020 Paul Boersma
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

#include <time.h>
#if defined (macintosh) || defined (UNIX)
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

uint64 GetPIDTimeInNanoseconds(void)
{
    uint64        start;
    uint64        end;
    uint64        elapsed;
    uint64        elapsedNano;
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

#include <chrono>

double Melder_clock () {
	using namespace std::chrono;
	auto timePoint = high_resolution_clock::now ();
	auto duration = timePoint. time_since_epoch ();
	return duration. count () * double (high_resolution_clock::period::num) / double (high_resolution_clock::period::den);
}

double Melder_stopwatch () {
	static double lastTime;
	double now = Melder_clock ();
	double timeElapsed = ( lastTime == 0 ? -1.0 : now - lastTime );
	//Melder_casual ("%ld %ld %ld %lf %lf", now, lastTime, now - lastTime, (now - lastTime) / (double) CLOCKS_PER_SEC, timeElapsed);
	lastTime = now;
	return timeElapsed;
}

void Melder_sleep (double duration) {
	if (duration <= 0.0)
		return;   // already past end time
	#if defined (_WIN32)
		Sleep (duration * 1e3);
	#elif defined (macintosh) || defined (UNIX)
		unsigned int seconds = (unsigned int) duration;
		unsigned int microseconds = (unsigned int) ((duration - seconds) * 1e6);
		if (seconds > 0)
			sleep (seconds);
		if (microseconds > 0)
			usleep (microseconds);
	#endif
}

autostring32 STRdate () {
	time_t today = time (nullptr);
	autostring32 date = Melder_8to32 (ctime (& today));
	mutablestring32 newline = str32chr (date.get(), U'\n');
	if (newline)
		*newline = U'\0';
	return date;
}

/* End of file melder_time.cpp */
