/* Sampled_extensions.c
 *
 * Copyright (C) 1993-2003 David Weenink
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
 djmw 20030815 GPL header
*/

#include "Sampled_extensions.h"

double Sampled_getAnalysisWidth (I)
{
	iam (Sampled);
	double duration_domain = my xmax - my xmin;
	double duration_sampling = my nx * my dx;
	return 2 * (my x1 - my xmin) - (duration_domain - duration_sampling);
}

/* End of file Sampled_extensions.c */
