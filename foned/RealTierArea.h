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

	double ymin, ymax, ycursor;   // BUG: this should be in a cache
public:   // BUG: should be "protected" (now public because it is sometimes used as a message)
	virtual void v_updateScaling () {
		TRACE trace(U"");
		/*
			Computes ymin, ymax and ycursor on the basis of the data.
		*/
		Melder_assert (isdefined (our instancePref_dataFreeMinimum()));
		Melder_assert (isdefined (our instancePref_dataFreeMaximum()));
		our ymin = our instancePref_dataFreeMinimum();
		our ymax = our instancePref_dataFreeMaximum();
		if (our realTier() && our realTier() -> points.size > 0) {
			Melder_assert (! (our v_maximumLegalY() < our v_minimumLegalY()));   // NaN-safe
			const double minimumValue = Melder_clipped (our v_minimumLegalY(), RealTier_getMinimumValue (our realTier()), our v_maximumLegalY());
			const double maximumValue = Melder_clipped (our v_minimumLegalY(), RealTier_getMaximumValue (our realTier()), our v_maximumLegalY());
			Melder_clipRight (& our ymin, minimumValue);
			Melder_clipLeft (maximumValue, & our ymax);
		}
		if (our ycursor <= our ymin || our ycursor >= our ymax)
			our ycursor = 0.382 * our ymin + 0.618 * our ymax;
	}
protected:
	void v_computeAuxiliaryData () override {
		our RealTierArea_Parent :: v_computeAuxiliaryData ();
		our v_updateScaling ();
	}

public:
	virtual conststring32 v_rightTickUnits () { return U""; }

	double anchorTime = undefined, anchorY;
	bool anchorIsInFreePart, anchorIsNearPoint;   // only in cb_mouse
	bool draggingSelection;
	double dt = 0.0, dy = 0.0;
	integer firstSelected, lastSelected;

	void viewRealTierAsWorldByWorld () const {
		FunctionArea_setViewport (this);
		Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), our ymin, our ymax);
	}

	#include "RealTierArea_prefs.h"
};

void RealTierArea_addPointAt (RealTierArea me, double time, double desiredY);

void RealTierArea_removePoints (RealTierArea me);

void RealTierArea_addPointAtCursor (RealTierArea me);

void RealTierArea_draw (RealTierArea me);

void RealTierArea_drawWhileDragging (RealTierArea me);

bool RealTierArea_mouse (RealTierArea me, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction);

DEFINE_FunctionArea_create (RealTierArea, RealTier)

/* End of file RealTierArea.h */
#endif
