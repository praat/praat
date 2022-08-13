#ifndef _EEGAnalysisArea_h_
#define _EEGAnalysisArea_h_
/* EEGAnalysisArea.h
 *
 * Copyright (C) 2011-2018,2022 Paul Boersma
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

#include "SoundAnalysisArea.h"
#include "EEG.h"

Thing_define (EEGAnalysisArea, SoundAnalysisArea) {
	bool v_hasPitch ()
		override { return false; }
	bool v_hasIntensity ()
		override { return false; }
	bool v_hasFormants ()
		override { return false; }
	bool v_hasPulses ()
		override { return false; }

	#include "EEGAnalysisArea_prefs.h"
};
DEFINE_FunctionArea_create (EEGAnalysisArea, Sound)

/* End of file EEGAnalysisArea.h */
#endif
