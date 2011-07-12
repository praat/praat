/* Proximity.c
 *
 * Copyright (C) 1993-2004 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020813 GPL header
 djmw 20040309 Removed assertion 'numberOfPoints> 0' in Proximity_init
*/

#include "Proximity.h"
#include "TableOfReal_extensions.h"

class_methods (Proximity, TableOfReal)
class_methods_end

void Proximity_init (I, long numberOfPoints)
{
	iam (Proximity);
	TableOfReal_init (me, numberOfPoints, numberOfPoints); therror
	TableOfReal_setSequentialRowLabels (me, 0, 0, NULL, 1, 1); therror
	TableOfReal_setSequentialColumnLabels (me, 0, 0, NULL, 1, 1); therror
}

/* End of file Proximity.c */
