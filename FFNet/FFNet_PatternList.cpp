/* FFNet_PatternList.cpp
 *
 * Copyright (C) 1997-2018 David Weenink
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
 djmw 19950113
 djmw 20020712 GPL header
*/

#include "FFNet_PatternList.h"

void FFNet_PatternList_drawActivation (FFNet me, PatternList pattern, Graphics g, integer index) {
	if (index < 1 || index > pattern->ny) 
		return;
	FFNet_propagate (me, pattern -> z.row (index), nullptr);
	FFNet_drawActivation (me, g);
}

/* End of file FFNet_PatternList.cpp */
