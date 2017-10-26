#ifndef _MelderThread_h_
#define _MelderThread_h_
/* MelderThread.h
 *
 * Copyright (C) 2014-2017 Paul Boersma
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

#include <vector>
#include "Thing.h"

#if defined (_WIN32)
	#define USE_WINTHREADS 1
	#define USE_PTHREADS 0
	#define USE_CPPTHREADS 0
#elif defined (macintosh)
	#define USE_WINTHREADS 0
	#define USE_PTHREADS 1
	#define USE_CPPTHREADS 0
#else
	#define USE_WINTHREADS 0
	#define USE_PTHREADS 1
	#define USE_CPPTHREADS 0
#endif

#if USE_WINTHREADS
	#include <windows.h>
	#include <process.h>
	#define MelderThread_MUTEX(_mutex)  static CRITICAL_SECTION _mutex
	#define MelderThread_MUTEX_INIT(_mutex)  InitializeCriticalSection (& _mutex)
	#define MelderThread_LOCK(_mutex)  EnterCriticalSection (& _mutex)
	#define MelderThread_UNLOCK(_mutex)  LeaveCriticalSection (& _mutex)
	#define MelderThread_RETURN_TYPE  DWORD WINAPI
	#define MelderThread_RETURN  return 0;
#elif USE_PTHREADS
	#include <pthread.h>
	#define MelderThread_MUTEX(_mutex)  static pthread_mutex_t _mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER
	#define MelderThread_MUTEX_INIT(_mutex)  (void) 0
	#define MelderThread_LOCK(_mutex)  pthread_mutex_lock (& _mutex)
	#define MelderThread_UNLOCK(_mutex)  pthread_mutex_unlock (& _mutex)
	#define MelderThread_RETURN_TYPE  void *
	#define MelderThread_RETURN  return nullptr;
#elif USE_CPPTHREADS
	#include <mutex>
	#include <thread>
	#define MelderThread_MUTEX(_mutex)  static std::mutex _mutex
	#define MelderThread_MUTEX_INIT(_mutex)  (void) 0
	#define MelderThread_LOCK(_mutex)  std::lock_guard <std::mutex> lock (_mutex)
	#define MelderThread_UNLOCK(_mutex)  (void) 0
	#define MelderThread_RETURN_TYPE  void
	#define MelderThread_RETURN  return;
#else
	/* No threads. Make single-threaded. */
	#define MelderThread_MUTEX(_mutex)  static int _mutex
	#define MelderThread_MUTEX_INIT(_mutex)  (void) 0
	#define MelderThread_LOCK(_mutex)  (void) 0
	#define MelderThread_UNLOCK(_mutex)  (void) 0
	#define MelderThread_RETURN_TYPE  void
	#define MelderThread_RETURN  return;
#endif

#if 0
	/* For debugging of lock code only. */
	#define MelderThread_MUTEX(_mutex)  static volatile int _mutex
	#define MelderThread_MUTEX_INIT(_mutex)  _mutex = 0
	#define MelderThread_LOCK(_mutex)  do { while (_mutex) ; _mutex = 1; } while (0)
	#define MelderThread_UNLOCK(_mutex)  _mutex = 0
#endif

inline static int MelderThread_getNumberOfProcessors () {
	#if USE_WINTHREADS
		return 8;
	#elif USE_PTHREADS
		return 8;
	#elif USE_CPPTHREADS
		return std::thread::hardware_concurrency ();
	#else
		return 1;
	#endif
}

#if USE_WINTHREADS
	template <class T> void MelderThread_run (DWORD (WINAPI *func) (T *), _Thing_auto <T> *args, int numberOfThreads) {
		if (numberOfThreads == 1) {
			func (args [0].get());
		} else {
			std::vector <HANDLE> threads (numberOfThreads);
			try {
				for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
					threads [ithread - 1] = CreateThread (nullptr, 0,
						(DWORD (WINAPI *)(void *)) func, (void *) args [ithread - 1].get(), 0, nullptr);
				}
				func (args [numberOfThreads - 1].get());
			} catch (MelderError) {
				for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
					WaitForSingleObject (threads [ithread - 1], INFINITE);
					CloseHandle (threads [ithread - 1]);
				}
				throw;
			}
			for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
				WaitForSingleObject (threads [ithread - 1], INFINITE);
				CloseHandle (threads [ithread - 1]);
			}
		}
	}
#elif USE_PTHREADS
	template <class T> void MelderThread_run (void * (*func) (T *), _Thing_auto <T> *args, int numberOfThreads) {
		if (numberOfThreads == 1) {
			func (args [0].get());
		} else {
			std::vector <pthread_t> threads ((size_t) numberOfThreads);
			try {
				for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
					(void) pthread_create (& threads [(size_t) ithread - 1],
						nullptr, (void*(*)(void *)) func, (void *) args [ithread - 1].get());
				}
				func (args [numberOfThreads - 1].get());
			} catch (MelderError) {
				for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
					pthread_join (threads [(size_t) ithread - 1], nullptr);
				}
				throw;
			}
			for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
				pthread_join (threads [(size_t) ithread - 1], nullptr);
			}
		}
	}
#elif USE_CPPTHREADS
	template <class T> void MelderThread_run (void * (*func) (T *), _Thing_auto <T> *args, int numberOfThreads) {
		if (numberOfThreads == 1) {
			func (args [0].get());
		} else {
			std::vector <std::thread> thread (numberOfThreads);
			try {
				for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
					thread [ithread - 1] = std::thread (func, args [ithread - 1].get());
				}
				func (args [numberOfThreads - 1].get());
			} catch (MelderError) {
				for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
					if (thread [ithread - 1]. joinable ())
						thread [ithread - 1]. join ();
				}
				throw;
			}
			for (int ithread = 1; ithread < numberOfThreads; ithread ++) {
				thread [ithread - 1]. join ();
			}
		}
	}
#else
	template <class T> void MelderThread_run (void (*func) (T *), _Thing_auto <T> *args, int numberOfThreads) {
		func (args [0].get());
	}
#endif

#endif
/* End of file MelderThread.h */
