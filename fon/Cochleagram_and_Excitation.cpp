/* Cochleagram_and_Excitation.cpp
 *
 * Copyright (C) 1992-2011,2015,2017 Paul Boersma
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

/*
 * pb 1993/12/20
 * pb 2002/07/16 GPL
 * pb 2011/05/05 type
 * pb 2011/05/09 C++
 */

#include "Cochleagram_and_Excitation.h"

autoExcitation Cochleagram_to_Excitation (Cochleagram me, double t) {
	try {
		integer column = Matrix_xToNearestColumn (me, t);
		if (column < 1) column = 1;
		if (column > my nx) column = my nx;
		autoExcitation thee = Excitation_create (my dy, my ny);
		for (integer ifreq = 1; ifreq <= my ny; ifreq ++)
			thy z [1] [ifreq] = my z [ifreq] [column];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": slice at time ", t, U" seconds not extracted to Excitation.");
	}
}

/* End of file Cochleagram_and_Excitation.cpp */
