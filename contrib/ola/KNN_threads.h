#ifndef _KNN_threads_h_
#define _KNN_threads_h_

/* KNN_threads.h
 *
 * Copyright (C) 2009 Ola Söder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * os 20090123 First version
 * pb 2011/03/08 C++
 */

/////////////////////////////////////////////////////
//                                                 //
/////////////////////////////////////////////////////

// Error codes
enum KNN_thread_status
{
    KNN_THREAD_OK,
    KNN_THREAD_ERROR
};

// Query the number of available CPUs
int KNN_getNumberOfCPUs(void);

// Distribute the work specified by 
// (*function) and input over nthreads 
// threads
void * KNN_threadDistribution
(
    void * (* function) (void *), 
    void ** input, 
    int nthreads
);

// Test threading
void KNN_threadTest(void);

// Test threading aux
void * KNN_threadTestAux
(
    void * dummy
);

/* End of file KNN_threads.h */
#endif
