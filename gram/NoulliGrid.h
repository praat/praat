#ifndef _NoulliGrid_h_
#define _NoulliGrid_h_
/* NoulliGrid.h
 *
 * Copyright (C) 2018,2020 Paul Boersma
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

#include "Function.h"
#include "Collection.h"

#include "NoulliGrid_def.h"

integer NoulliPoint_getWinningCategory (NoulliPoint me);

autoNoulliPoint NoulliGrid_average (NoulliGrid me, integer tierNumber, double tmin, double tmax);

autoVEC NoulliGrid_getAverageProbabilities (NoulliGrid me, integer tierNumber, double tmin, double tmax);

/* End of file NoulliGrid.h */
#endif

