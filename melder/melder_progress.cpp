/* melder_progress.cpp
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

static void defaultProgress (double /*progress*/, conststring32 /*message*/) {
}

static void * defaultMonitor (double /*progress*/, conststring32 /*message*/) {
	return nullptr;
}

int MelderProgress::_depth = 0;

MelderProgress::ProgressProc MelderProgress::_p_progressProc = & defaultProgress;
MelderProgress::MonitorProc MelderProgress::_p_monitorProc = & defaultMonitor;

void Melder_progressOff () { MelderProgress::_depth --; }
void Melder_progressOn () { MelderProgress::_depth ++; }

void MelderProgress::_doProgress (double progress, conststring32 message) {
	if (! Melder_batch && MelderProgress::_depth >= 0 && Melder_debug != 14)
		MelderProgress::_p_progressProc (progress, message);
}

MelderString MelderProgress::_buffer;

void * MelderProgress::_doMonitor (double progress, conststring32 message) {
	if (! Melder_batch && MelderProgress::_depth >= 0) {
		void *result = MelderProgress::_p_monitorProc (progress, message);
		if (result)
			return result;
	}
	return progress <= 0.0 ? nullptr /* no Graphics */ : (void *) -1 /* any non-null pointer */;
}

void Melder_setProgressProc (MelderProgress::ProgressProc proc) {
	MelderProgress::_p_progressProc = ( proc ? proc : & defaultProgress );
}

void Melder_setMonitorProc (MelderProgress::MonitorProc proc) {
	MelderProgress::_p_monitorProc = ( proc ? proc : & defaultMonitor );
}

/* End of file melder_progress.cpp */
