#ifndef _OptimalCeilingTierEditor_h_
#define _OptimalCeilingTierEditor_h_
/* OptimalCeilingTierEditor.h
 *
 * Copyright (C) 2015 David Weenink, 2017,2020-2022 Paul Boersma
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
#include "OptimalCeilingTierArea.h"
#include "Sound.h"

Thing_define (OptimalCeilingTierEditor, RealTierEditor) {
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime)
		override;
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable OptimalCeilingTier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
		if (our soundArea())
			FunctionArea_drawLegend (our soundArea().get(),
				FunctionArea_legend_WAVEFORM U" %%non-modifiable copy of sound",
				DataGui_defaultForegroundColour (our soundArea().get(), false)
			);
	}
};

autoOptimalCeilingTierEditor OptimalCeilingTierEditor_create (conststring32 title,
	OptimalCeilingTier optimalCeilingTier,   // main data
	Sound sound   // may be null
);

/* End of file OptimalCeilingTierEditor.h */
#endif
