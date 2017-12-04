#ifndef _TableOfReal_and_SVD_h_
#define _TableOfReal_and_SVD_h_
/* TableOfReal_and_SVD.h
 *
 * Copyright (C) 1993-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20021009 GPL header
 djmw 20110397 Latest modification
*/

#include "SVD.h"
#include "TableOfReal.h"

autoSVD TableOfReal_to_SVD (TableOfReal me);

autoGSVD TablesOfReal_to_GSVD (TableOfReal me, TableOfReal thee);

autoTableOfReal SVD_to_TableOfReal (SVD me, integer from, integer to);

autoTableOfReal SVD_extractLeftSingularVectors (SVD me);

autoTableOfReal SVD_extractRightSingularVectors (SVD me);

autoTableOfReal SVD_extractSingularValues (SVD me);

#endif // _TableOfReal_and_SVD_h_
