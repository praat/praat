#ifndef _RealTierArea_h_
#define _RealTierArea_h_
/* RealTierArea.h
 *
 * Copyright (C) 1992-2005,2007-2012,2015-2018,2020-2022 Paul Boersma
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
#include "RealTier.h"

Thing_define (RealTierArea, FunctionArea) {
	RealTier realTier() { return static_cast <RealTier> (our function()); }

	virtual double v_minimumLegalY () { return undefined; }
	virtual double v_maximumLegalY () { return undefined; }
	virtual conststring32 v_rightTickUnits () { return U""; }

	double ymin, ymax, ycursor;
	double anchorTime = undefined, anchorY;
	bool anchorIsInFreePart, anchorIsNearPoint;   // only in cb_mouse
	bool draggingSelection;
	double dt = 0.0, dy = 0.0;
	integer firstSelected, lastSelected;

	void viewRealTierAsWorldByWorld () const {
		our setViewport ();
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	}

	#include "RealTierArea_prefs.h"
};

void RealTierArea_addPointAt (RealTierArea me, double time, double desiredY);

void RealTierArea_removePoints (RealTierArea me);

void RealTierArea_addPointAtCursor (RealTierArea me);

void RealTierArea_updateScaling (RealTierArea me);

void RealTierArea_draw (RealTierArea me);

void RealTierArea_drawWhileDragging (RealTierArea me);

bool RealTierArea_mouse (RealTierArea me, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction);

inline void RealTierArea_init (RealTierArea me, FunctionEditor editor, RealTier realTier) {
	FunctionArea_init (me, editor, realTier);
	Melder_assert (isdefined (my instancePref_dataFreeMinimum()));
	Melder_assert (isdefined (my instancePref_dataFreeMaximum()));
	RealTierArea_updateScaling (me);
	my ycursor = 0.382 * my ymin + 0.618 * my ymax;
}

inline autoRealTierArea RealTierArea_create (FunctionEditor editor, RealTier realTier) {
	autoRealTierArea me = Thing_new (RealTierArea);
	RealTierArea_init (me.get(), editor, realTier);
	return me;
}

/* End of file RealTierArea.h */
#endif
