#ifndef _EEGArea_h_
#define _EEGArea_h_
/* EEGArea.h
 *
 * Copyright (C) 2022 Paul Boersma
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

#include "SoundArea.h"
#include "EEG.h"

Thing_define (EEGArea, SoundArea) {
	/*
		Accessors.
	*/
	EEG borrowedEEG;

	conststring32 v_getChannelName (integer channelNumber) override {
		Melder_assert (our borrowedEEG);
		return our borrowedEEG -> channelNames [channelNumber].get();
	}

	#include "EEGArea_prefs.h"
};
DEFINE_FunctionArea_create (EEGArea, Sound)

/* End of file EEGArea.h */
#endif
