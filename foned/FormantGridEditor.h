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
#include "FormantGridArea.h"

Thing_define (FormantGridEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, FormantGridArea, formantGridArea);

	void v1_dataChanged (Editor sender) override {
		our structFunctionEditor :: v1_dataChanged (sender);
		our formantGridArea() -> _formantGrid = static_cast <FormantGrid> (our data());
		OrderedOf<structRealTier>* tiers =
				( our formantGridArea() -> editingBandwidths ? & our formantGridArea() -> _formantGrid -> bandwidths : & our formantGridArea() -> _formantGrid -> formants );
		RealTier tier = tiers->at [our formantGridArea() -> selectedFormant];
		our formantGridArea() -> functionChanged (tier);
	}
	void v_distributeAreas () override {
		our formantGridArea() -> setGlobalYRange_fraction (0.0, 1.0);
	}
	void v_play (double startTime, double endTime) override {
		FormantGridArea_playPart (our formantGridArea().get(), startTime, endTime, theFunctionEditor_playCallback, this);
	}
};

inline autoFormantGridEditor FormantGridEditor_create (conststring32 title, FormantGrid formantGrid) {
	try {
		autoFormantGridEditor me = Thing_new (FormantGridEditor);
		my formantGridArea() = FormantGridArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, formantGrid);
		return me;
	} catch (MelderError) {
		Melder_throw (U"FormantGrid window not created.");
	}
}

/* End of file FormantGridEditor.h */
#endif
