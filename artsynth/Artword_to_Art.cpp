/* Artword_to_Art.cpp
 *
 * Copyright (C) 1992-2005,2011,2015-2017 Paul Boersma
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

#include "Artword_to_Art.h"

autoArt Artword_to_Art (Artword me, double tim) {
	try {
		autoArt thee = Art_create ();
		Artword_intoArt (me, thee.get(), tim);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Articulation.");
	}
}

/* End of file Artword_to_Art.cpp */
