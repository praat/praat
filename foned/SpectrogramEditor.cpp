/* SpectrogramEditor.cpp
 *
 * Copyright (C) 1992-2005,2007-2012,2014-2022 Paul Boersma
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

#include "SpectrogramEditor.h"

Thing_implement (SpectrogramEditor, FunctionEditor, 0);

autoSpectrogramEditor SpectrogramEditor_create (conststring32 title, Spectrogram spectrogram) {
	try {
		autoSpectrogramEditor me = Thing_new (SpectrogramEditor);
		my spectrogramArea() = SpectrogramArea_create (true, nullptr, me.get());
		FunctionEditor_init (me.get(), title, spectrogram);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrogram window not created.");
	}
}

/* End of file SpectrogramEditor.cpp */
