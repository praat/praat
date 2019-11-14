/* Configuration_and_Procrustes.c
 *
 * Copyright (C) 1993-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020424 GPL
 djmw 20020704 Changed header NUMclasses.h to SVD.h
 djmw 20041022 Added orthogonal argument to Configurations_to_Procrustes.
*/

#include "Configuration_and_Procrustes.h"
#include "NUM2.h"

autoProcrustes Configurations_to_Procrustes (Configuration me, Configuration thee, bool orthogonal) {
	try {
		Melder_require (my numberOfRows == thy numberOfRows && my numberOfColumns == thy numberOfColumns,
			U"Configurations must have the same number of points and the same dimension.");

		autoProcrustes p = Procrustes_create (my numberOfColumns);
		NUMprocrustes (my data.get(), thy data.get(), & p -> r, ( orthogonal ? nullptr : & p -> t ), ( orthogonal ? nullptr : & p -> s) );
		return p;
	} catch (MelderError) {
		Melder_throw (U"Procrustes from two Configurations not created.");
	}
}

/* End of file Configuration_and_Procrustes.c */
