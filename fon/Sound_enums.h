/* Sound_enums.h
 *
 * Copyright (C) 1992-2002 Paul Boersma
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
 * pb 1997/09/25
 * pb 2002/07/16 GPL
 */

enum_begin (Sound_WINDOW, Rectangular)
	enum (Triangular)
	enum (Parabolic)
	enum (Hanning)
	enum (Hamming)
	enum (Gaussian1)
	enum (Gaussian2)
	enum (Gaussian3)
	enum (Gaussian4)
	enum (Gaussian5)
	enum (Kaiser1)
	enum (Kaiser2)
enum_end (Sound_WINDOW)

/* End of file Sound_enums.h */
