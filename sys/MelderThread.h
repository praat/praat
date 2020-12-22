#ifndef _MelderThread_h_
#define _MelderThread_h_
/* MelderThread.h
 *
 * Copyright (C) 2014-2018,2020 Paul Boersma
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
#include <thread>

inline integer MelderThread_getNumberOfProcessors () {
	return uinteger_to_integer (std::thread::hardware_concurrency ());
}

template <class T> void MelderThread_run (void (*func) (T *), autoSomeThing <T> *args, integer numberOfThreads) {
	uinteger unsignedNumberOfThreads = integer_to_uinteger (numberOfThreads);
	if (unsignedNumberOfThreads == 1) {
		func (args [0].get());
	} else {
		std::vector <std::thread> thread (unsignedNumberOfThreads);
		try {
			for (uinteger ithread = 1; ithread < unsignedNumberOfThreads; ithread ++)
				thread [ithread - 1] = std::thread (func, args [ithread - 1].get());
			func (args [unsignedNumberOfThreads - 1].get());
		} catch (MelderError) {
			for (uinteger ithread = 1; ithread < unsignedNumberOfThreads; ithread ++)
				if (thread [ithread - 1]. joinable ())
					thread [ithread - 1]. join ();
			throw;
		}
		for (uinteger ithread = 1; ithread < unsignedNumberOfThreads; ithread ++)
			thread [ithread - 1]. join ();
	}
}

/* End of file MelderThread.h */
#endif
