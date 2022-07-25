#ifndef _TextGridEditor_h_
#define _TextGridEditor_h_
/* TextGridEditor.h
 *
 * Copyright (C) 1992-2005,2007-2022 Paul Boersma
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

#include "AnyTextGridEditor.h"

Thing_define (TextGridEditor, AnyTextGridEditor) {
	void v1_dataChanged () override {
		our TextGridEditor_Parent :: v1_dataChanged ();
		our textGridArea() -> functionChanged ((Function) our data());
		if (our soundArea)
			our soundArea -> functionChanged (nullptr);   // BUG: this function has not actually changed
		if (our soundAnalysisArea)
			our soundAnalysisArea -> functionChanged (our soundArea -> function());
	}
};

autoTextGridEditor TextGridEditor_create (conststring32 title, TextGrid grid,
	SampledXY sound,   // either a Sound or a LongSound, or null
	SpellingChecker spellingChecker,
	conststring32 callbackSocket
);

/* End of file TextGridEditor.h */
#endif
