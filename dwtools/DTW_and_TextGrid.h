#ifndef _DTW_and_TextGrid_h_
#define _DTW_and_TextGrid_h_

/* DTW_and_TextGrid.h
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20060906
 djmw 20120328 Latest modification
*/

#include "DTW.h"
#include "TextGrid.h"
#include "Table.h"

TextTier DTW_and_TextTier_to_TextTier (DTW me, TextTier thee, double precision);
IntervalTier DTW_and_IntervalTier_to_IntervalTier (DTW me, IntervalTier thee, double precision);
TextGrid DTW_and_TextGrid_to_TextGrid (DTW me, TextGrid thee, double precision);
/*
	Purpose: Create the new TextGrid with all times determined by the DTW.
*/

Table DTW_and_IntervalTier_to_Table (DTW me, IntervalTier thee, double precision);

#endif /* _DTW_and_TextGrid_h_ */
