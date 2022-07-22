#ifndef _SoundEditor_h_
#define _SoundEditor_h_
/* SoundEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2014-2016,2018,2020,2022 Paul Boersma
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

#include "TimeSoundAnalysisEditor.h"

Thing_define (SoundEditor, TimeSoundAnalysisEditor) {
	double maxBuffer;

	void v1_dataChanged () override {
		SoundEditor_Parent :: v1_dataChanged ();
		Thing_cast (SampledXY, soundOrLongSound, our data());
		our soundArea -> functionChanged (soundOrLongSound);
		our soundAnalysisArea -> functionChanged (soundOrLongSound);
	}
	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_distributeAreas ()
		override;
	void v_draw ()
		override;
	void v_play (double tmin, double tmax)
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double xWC, double yWC)
		override;
};

void SoundEditor_init (SoundEditor me, autoSoundArea soundArea,
	conststring32 title,
	SampledXY data
);

autoSoundEditor SoundEditor_create (
	conststring32 title,
	SampledXY data   // either a Sound or a LongSound
);

/* End of file SoundEditor.h */
#endif
