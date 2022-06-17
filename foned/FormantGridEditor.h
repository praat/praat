#ifndef _FormantGridEditor_h_
#define _FormantGridEditor_h_
/* FormantGridEditor.h
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

#include "FunctionEditor.h"
#include "FormantGrid.h"
#include "RealTierArea.h"

Thing_define (FormantGridArea, RealTierArea) {
	FormantGrid _formantGrid;
	FormantGrid formantGrid() { return our _formantGrid; }

	bool editingBandwidths;
	integer selectedFormant;

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

	double v_minimumLegalY ()
		override { return 0.0; }
	conststring32 v_rightTickUnits ()
		override { return U" Hz"; }

	#include "FormantGridArea_prefs.h"
};
inline void FormantGridArea_init (FormantGridArea me, FunctionEditor editor, bool makeCopy, bool editable) {
	my editingBandwidths = false;
	my selectedFormant = 1;
	RealTierArea_init (me, editor, nullptr, makeCopy, editable);
	my setGlobalYRange_fraction (0.0, 1.0);
}

Thing_define (FormantGridEditor, FunctionEditor) {
	autoFormantGridArea formantGridArea;
	void v_dataChanged () override {
		our structFunctionEditor :: v_dataChanged ();
		our formantGridArea -> _formantGrid = static_cast <FormantGrid> (our data);
		OrderedOf<structRealTier>* tiers =
				( our formantGridArea -> editingBandwidths ? & our formantGridArea -> _formantGrid -> bandwidths : & our formantGridArea -> _formantGrid -> formants );
		RealTier tier = tiers->at [our formantGridArea -> selectedFormant];
		our formantGridArea -> functionChanged (tier);
	}

	GuiMenuItem d_bandwidthsToggle;

	void v_createMenus ()
		override;
	void v_distributeAreas ()
		override;
	void v_draw ()
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double globalY_fraction)
		override;
	void v_play (double startTime, double endTime)
		override;

	virtual bool v_hasSourceMenu () { return true; }

	#include "FormantGridEditor_prefs.h"
};

Thing_define (FormantGridEditor_FormantGridArea, FormantGridArea) {
};

autoFormantGridEditor FormantGridEditor_create (conststring32 title, FormantGrid formantGrid);

/* End of file FormantGridEditor.h */
#endif
