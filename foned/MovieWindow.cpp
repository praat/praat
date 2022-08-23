/* MovieWindow.cpp
 *
 * Copyright (C) 2011-2020,2022 Paul Boersma
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

#include "MovieWindow.h"

Thing_implement (MovieWindow, FunctionEditor, 0);

autoMovieWindow MovieWindow_create (conststring32 title, Movie movie) {
	try {
		autoMovieWindow me = Thing_new (MovieWindow);
		my videoArea() = MovieArea_create (true, nullptr, me.get());
		if (movie -> d_sound) {
			my soundArea() = SoundArea_create (false, nullptr, me.get());
			my soundAnalysisArea() = SoundAnalysisArea_create (false, nullptr, me.get());
		}
		FunctionEditor_init (me.get(), title, movie);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Movie window not created.");
	}
}

/* End of file MovieWindow.cpp */
