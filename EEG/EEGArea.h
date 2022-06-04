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
	EEG eeg;

	virtual conststring32 v_rightTickUnits () { return U"V"; }

	conststring32 v_getChannelName (integer /* channelNumber */)
		override;

	#include "EEGArea_prefs.h"
};

inline void EEGArea_init (EEGArea me, FunctionEditor editor) {
	FunctionArea_init (me, editor);
	Melder_assert (isdefined (my instancePref_dataFreeMinimum()));
	Melder_assert (isdefined (my instancePref_dataFreeMaximum()));
}

inline autoEEGArea EEGArea_create (FunctionEditor editor) {
	autoEEGArea me = Thing_new (EEGArea);
	EEGArea_init (me.get(), editor);
	return me;
}

/* End of file EEGArea.h */
#endif
