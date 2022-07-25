/* TextGridEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "TextGridEditor.h"

Thing_implement (TextGridEditor, AnyTextGridEditor, 0);

autoTextGridEditor TextGridEditor_create (conststring32 title, TextGrid textGrid,
	SampledXY optionalSoundOrLongSound, SpellingChecker spellingChecker, conststring32 callbackSocket)
{
	try {
		autoTextGridEditor me = Thing_new (TextGridEditor);
		my textGridArea() = TextGridArea_create (true, nullptr, me.get());
		if (optionalSoundOrLongSound) {
			if (Thing_isa (optionalSoundOrLongSound, classSound))
				my soundArea() = SoundArea_create (false, static_cast <Sound> (optionalSoundOrLongSound), me.get());
			else
				my soundArea() = LongSoundArea_create (false, static_cast <LongSound> (optionalSoundOrLongSound), me.get());
			my soundAnalysisArea() = SoundAnalysisArea_create (false, nullptr, me.get());
			my textGridArea() -> borrowedSoundArea = my soundArea().get();
			my textGridArea() -> borrowedSoundAnalysisArea = my soundAnalysisArea().get();
		}
		AnyTextGridEditor_init (me.get(), title, textGrid, spellingChecker, callbackSocket);
		return me;
	} catch (MelderError) {
		Melder_throw (U"TextGrid window not created.");
	}
}

/* End of file TextGridEditor.cpp */
