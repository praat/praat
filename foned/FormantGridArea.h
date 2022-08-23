#ifndef _FormantGridArea_h_
#define _FormantGridArea_h_
/* FormantGridArea.h
 *
 * Copyright (C) 2008-2018,2020,2022 Paul Boersma & David Weenink
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

#include "FormantGrid.h"
#include "RealTierArea.h"

Thing_define (FormantGridArea, RealTierArea) {
	FormantGrid _formantGrid;
	FormantGrid formantGrid() { return our _formantGrid; }

	bool editingBandwidths = false;
	integer selectedFormant = 1;
	GuiMenuItem d_bandwidthsToggle;

	void v_updateScaling () override {
		if (our editingBandwidths) {
			Melder_assert (isdefined (our instancePref_bandwidthFloor()));
			Melder_assert (isdefined (our instancePref_bandwidthCeiling()));
			our ymin = our instancePref_bandwidthFloor();
			our ymax = our instancePref_bandwidthCeiling();
		} else {
			Melder_assert (isdefined (our instancePref_formantFloor()));
			Melder_assert (isdefined (our instancePref_formantCeiling()));
			our ymin = our instancePref_formantFloor();
			our ymax = our instancePref_formantCeiling();
		}
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
	conststring32 v_menuTitle ()
		override { return U"FormantGrid"; }
	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }
	void v_drawInside ()
		override;
	void v_createMenus ()
		override;

	virtual bool v_hasSourceMenu () { return true; }

	#include "FormantGridArea_prefs.h"
};

DEFINE_FunctionArea_create (FormantGridArea, FormantGrid)

void FormantGridArea_playPart (FormantGridArea me, double startTime, double endTime, Sound_PlayCallback playCallback, Thing playBoss);

/* End of file FormantGridArea.h */
#endif
