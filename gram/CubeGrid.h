#ifndef _CubeGrid_h_
#define _CubeGrid_h_
/* CubeGrid.h
 *
 * Copyright (C) 2023 Paul Boersma
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

#include "CubeGrid_def.h"

autoCubePoint CubeGrid_average (CubeGrid me, integer tierNumber, double tmin, double tmax);
autoVEC CubeGrid_getAverages (CubeGrid me, integer tierNumber, double tmin, double tmax);

void CubeGrid_paintInside (CubeGrid me, Graphics graphics, double tmin, double tmax);
void CubeGrid_paint (CubeGrid me, Graphics g, double tmin, double tmax, bool garnish);

/* End of file CubeGrid.h */
#endif

