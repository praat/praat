#ifndef _AmplitudeTierEditor_h_
#define _AmplitudeTierEditor_h_
/* AmplitudeTierEditor.h
 *
 * Copyright (C) 2003-2005,2007,2009-2012,2014-2018,2020-2022 Paul Boersma
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

#include "RealTierEditor.h"
#include "AmplitudeTierArea.h"
#include "Sound.h"

Thing_define (AmplitudeTierEditor, RealTierEditor) {
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_play (double fromTime, double toTime)
		override;
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable AmplitudeTier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
		if (our soundArea())
			FunctionArea_drawLegend (our soundArea().get(),
				FunctionArea_legend_WAVEFORM U" %%non-modifiable copy of sound",
				DataGui_defaultForegroundColour (our soundArea().get(), false)
			);
	}
};

autoAmplitudeTierEditor AmplitudeTierEditor_create (conststring32 title,
	AmplitudeTier amplitude,   // the main data
	Sound sound   // may be null
);

/* End of file AmplitudeTierEditor.h */
#endif
