/* Spectrum_to_Excitation.h
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

#include "Spectrum.h"
#include "Excitation.h"

Excitation Spectrum_to_Excitation (I, double df);
/*
	Postcondition:
		filtered with 10 ^ (1.581 + 0.75 * bark - 1.75 * sqrt (1 + bark * bark)))
*/

/* End of file Spectrum_to_Excitation.h */
