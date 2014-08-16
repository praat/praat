/* SampledXY.cpp
 *
 * Copyright (C) 1992-2012,2013,2014 Paul Boersma
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

#include "SampledXY.h"

#include "oo_DESTROY.h"
#include "SampledXY_def.h"
#include "oo_COPY.h"
#include "SampledXY_def.h"
#include "oo_EQUAL.h"
#include "SampledXY_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SampledXY_def.h"
#include "oo_WRITE_TEXT.h"
#include "SampledXY_def.h"
#include "oo_READ_TEXT.h"
#include "SampledXY_def.h"
#include "oo_WRITE_BINARY.h"
#include "SampledXY_def.h"
#include "oo_READ_BINARY.h"
#include "SampledXY_def.h"
#include "oo_DESCRIPTION.h"
#include "SampledXY_def.h"

Thing_implement (SampledXY, Sampled, 0);

void SampledXY_init (SampledXY me,
	double xmin, double xmax, long nx, double dx, double x1,
	double ymin, double ymax, long ny, double dy, double y1)
{
	Sampled_init (me, xmin, xmax, nx, dx, x1);
	my ymin = ymin;
	my ymax = ymax;
	my ny = ny;
	my dy = dy;
	my y1 = y1;
}

long SampledXY_getWindowSamplesY (SampledXY me, double fromY, double toY, long *iymin, long *iymax) {
	double riymin = 1.0 + ceil ((fromY - my y1) / my dy);
	double riymax = 1.0 + floor ((toY - my y1) / my dy);   // could be above 32-bit LONG_MAX
	*iymin = riymin < 1.0 ? 1 : (long) riymin;
	*iymax = riymax > (double) my ny ? my ny : (long) riymax;
	if (*iymin > *iymax) return 0;
	return *iymax - *iymin + 1;
}

/* End of file SampledXY.cpp */
