/* Cochleagram_and_Excitation.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 1993/12/20
 * pb 2002/07/16 GPL
 * pb 2011/05/05 type
 * pb 2011/05/09 C++
 */

#include "Cochleagram_and_Excitation.h"

Excitation Cochleagram_to_Excitation (I, double t)
{
	iam (Cochleagram);
	long column = Matrix_xToNearestColumn (me, t), ifreq;
	Excitation thee;
	if (column < 1) column = 1;
	if (column > my nx) column = my nx;
	thee = Excitation_create (my dy, my ny);
	if (! thee) return NULL;
	for (ifreq = 1; ifreq <= my ny; ifreq ++)
		thy z [1] [ifreq] = my z [ifreq] [column];
	return thee;
}

/* End of file Cochleagram_and_Excitation.cpp */
