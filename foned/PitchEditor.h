#ifndef _PitchEditor_h_
#define _PitchEditor_h_
/* PitchEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2015,2016,2018,2020,2022 Paul Boersma
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
#include "PitchArea.h"
#include "Pitch_to_Sound.h"

Thing_define (PitchEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, PitchArea, pitchArea)

	void v1_dataChanged (Editor sender) override {
		PitchEditor_Parent :: v1_dataChanged (sender);
		our pitchArea() -> functionChanged (static_cast <Pitch> (our data()));
	}
	void v_distributeAreas () override {
		our pitchArea() -> setGlobalYRange_fraction (0.0, 1.0);
	}
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime) override {
		Pitch_hum (our pitchArea() -> pitch(), startTime, endTime, theFunctionEditor_playCallback, this);   // BUG: why no callback?
	}
};

autoPitchEditor PitchEditor_create (conststring32 title, Pitch pitch);

/* End of file PitchEditor.h */
#endif
