/* Distance.c
 *
 * Copyright (C) 1993-2002 David Weenink
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
*/

#include "Distance.h"

class_methods (Distance, Proximity)
class_methods_end

Distance Distance_create (long numberOfPoints)
{
	Distance me = new (Distance);
	if (! me || ! Proximity_init (me, numberOfPoints)) forget (me);
	return me;
}

void Distance_drawDendogram (Distance me, Graphics g, int method)
{
	(void) me;
	(void) g;
	(void) method;
}
