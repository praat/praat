#ifndef _PointEditor_h_
#define _PointEditor_h_
/* PointEditor.h
 *
 * Copyright (C) 1992-2011,2012,2015,2016,2018,2022 Paul Boersma
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
#include "PointArea.h"
#include "SoundArea.h"
#include "PointProcess_and_Sound.h"

Thing_define (PointEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, PointArea, pointArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)

	void v1_dataChanged (Editor sender) override {
		PointEditor_Parent :: v1_dataChanged (sender);
		our pointArea() -> functionChanged (static_cast <PointProcess> (our data()));
		if (our soundArea())
			our soundArea() -> functionChanged (nullptr);
	}
	void v_distributeAreas () override {
		our pointArea() -> setGlobalYRange_fraction (0.0, 1.0);
		if (our soundArea())
			our soundArea() -> setGlobalYRange_fraction (0.0, 1.0);
	}
	void v_draw () override {
		if (our soundArea())
			FunctionArea_drawTwo (our soundArea().get(), our pointArea().get());
		else
			FunctionArea_drawOne (our pointArea().get());
	}
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime) override {
		if (our soundArea())
			Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
		else
			PointProcess_playPart (our pointArea() -> pointProcess(), startTime, endTime, theFunctionEditor_playCallback, this);
	}
};

autoPointEditor PointEditor_create (conststring32 title, PointProcess point, Sound optionalCopyOfSound);

/* End of file PointEditor.h */
#endif
