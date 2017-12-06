/*
 * $Id: pa_unix_util.c 1097 2006-08-26 08:27:53Z rossb $
 * Portable Audio I/O Library
 * UNIX platform-specific support functions
 *
 * Based on the Open Source API proposed by Ross Bencina
 * Copyright (c) 1999-2000 Ross Bencina
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

/** @file
 @ingroup unix_src
*/
 
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <string.h> /* For memset */
#include <math.h>
#include <errno.h>

#include "pa_util.h"
//#include "pa_mac_util.h"

/*
   Track memory allocations to avoid leaks.
 */

#if PA_TRACK_MEMORY
static int numAllocations_ = 0;
#endif


void *PaUtil_AllocateMemory( long size )
{
    void *result = malloc( size );

#if PA_TRACK_MEMORY
    if( result != NULL ) numAllocations_ += 1;
#endif
    return result;
}


void PaUtil_FreeMemory( void *block )
{
    if( block != NULL )
    {
        free( block );
#if PA_TRACK_MEMORY
        numAllocations_ -= 1;
#endif

    }
}


int PaUtil_CountCurrentlyAllocatedBlocks( void )
{
#if PA_TRACK_MEMORY
    return numAllocations_;
#else
    return 0;
#endif
}


void Pa_Sleep( long msec )
{
#ifdef HAVE_NANOSLEEP
    struct timespec req = {0}, rem = {0};
    PaTime time = msec / 1.e3;
    req.tv_sec = (time_t)time;
    assert(time - req.tv_sec < 1.0);
    req.tv_nsec = (long)((time - req.tv_sec) * 1.e9);
    nanosleep(&req, &rem);
    /* XXX: Try sleeping the remaining time (contained in rem) if interrupted by a signal? */
#else
    while( msec > 999 )     /* For OpenBSD and IRIX, argument */
        {                   /* to usleep must be < 1000000.   */
        usleep( 999000 );
        msec -= 999;
        }
    usleep( msec * 1000 );
#endif
}

/*            *** NOT USED YET: ***
static int usePerformanceCounter_;
static double microsecondsPerTick_;
*/

void PaUtil_InitializeClock( void )
{
    /* TODO */
}


PaTime PaUtil_GetTime( void )
{
#ifdef HAVE_CLOCK_GETTIME
    struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
    return (PaTime)(tp.tv_sec + tp.tv_nsec / 1.e9);
#else
    struct timeval tv;
    gettimeofday( &tv, NULL );
    return (PaTime) tv.tv_usec / 1000000. + tv.tv_sec;
#endif
}
