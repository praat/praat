/* Spectrum_to_Excitation.h
 *
 * Copyright (C) 1992-2011,2014,2015 Paul Boersma
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

#include "Spectrum.h"
#include "Excitation.h"

autoExcitation Spectrum_to_Excitation (Spectrum me, double df);
/*
	Postcondition:
		filtered with 10 ^ (1.581 + 0.75 * bark - 1.75 * sqrt (1 + bark * bark)))
*/

/* End of file Spectrum_to_Excitation.h */
