/* KNN_threads.c
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
 */

/////////////////////////////////////////////////////
//                                                 //
/////////////////////////////////////////////////////

#include <stdlib.h>
#include "KNN.h"
#include "KNN_threads.h"
#include "OlaP.h"

// Threading disabled
/*
// Linux... 
#if defined (__linux__)
    #define __USE_GNU
    #include <sched.h>
// BSD-style OSes
#elif defined (__APPLE__)    
    #include <sys/types.h>
    #include <sys/sysctl.h>
#endif
// Non-pthreaders, Windows
#ifdef _WIN32
    #include <windows.h>
// The rest of the pack
#else
    #include <pthread.h>
#endif
*/


/////////////////////////////////////////////////////
// KNN_getNumberOfCPUs                             //
/////////////////////////////////////////////////////

int KNN_getNumberOfCPUs(void)
{
    return(1);
    
    // Threading disabled
    /*
    int ncpus = 0;
// Linux... 
#if defined (__linux__)
    cpu_set_t * cpuset = (cpu_set_t *) malloc(sizeof(cpu_set_t));
    if(cpuset)
    {
        sched_getaffinity(0, sizeof(cpu_set_t), cpuset);
        for(int cpu = 0; cpu <= CPU_SETSIZE; ++cpu) 
            if(CPU_ISSET(cpu, cpuset))
                ++ncpus;
        free(cpuset);
    }
    Melder_assert(ncpus >= 1);
// BSD-style OSes
#elif defined (__APPLE__)
    int mib[2];
    size_t len = sizeof(ncpus);
            
    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    sysctl(mib, 2, &ncpus, &len, 0, 0);
// Non-pthreaders, Windows
#elif defined (_WIN32)
    typedef BOOL (WINAPI *LPFN_GLPI)
        (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
  
    BOOL done, rc;
    DWORD returnLength, byteOffset;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer, ptr;
    LPFN_GLPI Glpi;

    Glpi = (LPFN_GLPI) GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation");

    if(!Glpi) 
        return(1);
    
    done = FALSE;
    buffer = NULL;
    returnLength = 0;

    while(!done) 
    {
        rc = Glpi(buffer, &returnLength);
        if(!rc) 
        {
            if(GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if(buffer) 
                    free(buffer);
                return (1);
            } 
            else done = TRUE;
        }
        byteOffset = 0;
        ptr=buffer;
        while (byteOffset < returnLength) 
        {
            switch (ptr->Relationship) 
            {
                case RelationProcessorCore:
                        ++ncpus;
                default:
            }
            byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
            ++ptr;
        }
    }
    free(buffer);
    ncpus = 1;
// Unknown OS
#else 
    ncpus = 1;
#endif
    Melder_assert(ncpus >= 1);
    return(ncpus);
    */
}



/////////////////////////////////////////////////////
// KNN_threadDistribution                          //
/////////////////////////////////////////////////////

void * KNN_threadDistribution
(   
    void * (* function) (void *), 
    void ** input, 
    int nthreads
)

{
    Melder_assert(function && input && nthreads > 0);
    if(!(function && input && nthreads > 0))
    {
		KNN_thread_status * error = (KNN_thread_status *) malloc(sizeof(enum KNN_thread_status));
        if(error)
            *error = KNN_THREAD_ERROR;
        return((void *) error);
    }

    if(nthreads > 1)
    {
#ifdef _WIN32
        // Threading disabled
        /*
        HANDLE hHandle[nthreads];
        for(int i = 0; i < nthreds; ++)
            hHandle[i] = CreateThread(NULL, 0, function, input[i], 0, NULL);    
        while(nthreads--)
            WaitForSingleObject(hHandle[nthreds], INFINITE);
        */
#else
        // Threading disabled
        /*
        pthread_t thread_ids[nthreads];
        for(int i = 0; i < nthreads; ++i)
            pthread_create(&thread_ids[i], NULL, function, input[i]);
        while(nthreads--)
            pthread_join(thread_ids[nthreads], NULL);
        return(NULL);
        */
#endif
	}
	void *result = function (input [0]);
	return result;
}



/////////////////////////////////////////////////////
// KNN_threadTest                                  //
/////////////////////////////////////////////////////

void KNN_threadTest(void)
{
    void * dummy[KNN_getNumberOfCPUs()];
    KNN_threadDistribution(KNN_threadTestAux, (void **) &dummy, KNN_getNumberOfCPUs());
}



/////////////////////////////////////////////////////
// KNN_threadTestAux                               //
/////////////////////////////////////////////////////

void * KNN_threadTestAux(void * dummy)
{
    dummy = NULL; // dummy assignment to avoid compiler warnings;

    for(int i = 0; i < 50000; ++i)
        for(int i = 0; i < 50000; ++i) ;

    return(NULL);
}


