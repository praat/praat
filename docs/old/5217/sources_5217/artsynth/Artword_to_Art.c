/* Artword_to_Art.c
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
 * pb 1993/12/02
 * pb 2002/07/16 GPL
 */

#include "Artword_to_Art.h"

Art Artword_to_Art (I, double tim)
{
	iam (Artword);
	Art thee = Art_create ();
	if (! thee) return NULL;
	Artword_intoArt (me, thee, tim);
	return thee;
}

/* End of file Artword_to_Art.c */
