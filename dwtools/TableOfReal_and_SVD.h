#ifndef _TableOfReal_and_SVD_h_
#define TableOfReal_and_SVD_h_
/* TableOfReal_and_SVD.h
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20021009 GPL header
 djmw 20110397 Latest modification
*/

#include "SVD.h"
#include "TableOfReal.h"

SVD TableOfReal_to_SVD (TableOfReal me);
GSVD TablesOfReal_to_GSVD (TableOfReal me, TableOfReal thee);

TableOfReal SVD_to_TableOfReal (SVD me, long from, long to);
TableOfReal SVD_extractLeftSingularVectors (SVD me);
TableOfReal SVD_extractRightSingularVectors (SVD me);
TableOfReal SVD_extractSingularValues (SVD me);

#endif // _TableOfReal_and_SVD_h_
