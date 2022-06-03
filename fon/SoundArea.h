#ifndef _SoundArea_h_
#define _SoundArea_h_
/* SoundArea.h
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

#include "FunctionArea.h"
#include "Sound.h"

Thing_define (SoundArea, FunctionArea) {
	virtual double v_minimumLegalY () { return undefined; }
	virtual double v_maximumLegalY () { return undefined; }
	virtual conststring32 v_rightTickUnits () { return U"Pa"; }

	double ymin, ymax, ycursor;

	void viewSoundAsWorldByWorld () const {
		our setViewport ();
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	}

	#include "SoundArea_prefs.h"
};

void SoundArea_draw (SoundArea me, Sound sound);

bool SoundArea_mouse (SoundArea me, Sound sound, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction);

inline void SoundArea_init (SoundArea me, FunctionEditor editor) {
	FunctionArea_init (me, editor);
	Melder_assert (isdefined (my instancePref_dataFreeMinimum()));
	Melder_assert (isdefined (my instancePref_dataFreeMaximum()));
}

inline autoSoundArea SoundArea_create (FunctionEditor editor) {
	autoSoundArea me = Thing_new (SoundArea);
	SoundArea_init (me.get(), editor);
	return me;
}

/* End of file SoundArea.h */
#endif
