/* SampledXY.cpp
 *
 * Copyright (C) 1992-2017,2019,2021,2023 Paul Boersma
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

/*
 * CHECK
	if (xmin > xmax || ymin > ymax) {
		Melder_throw (U"xmax should be greater than xmax and ymax should be greater than ymin.");
	}
	if (nx < 1 || ny < 1) {
		Melder_throw (U"nx and ny should be at least 1.");
	}
	if (dx <= 0 || dy <= 0) {
		Melder_throw (U"dx and dy should be positive.");
	}
}
*/

void SampledXY_init (SampledXY me,
	double xmin, double xmax, integer nx, double dx, double x1,
	double ymin, double ymax, integer ny, double dy, double y1)
{
	Sampled_init (me, xmin, xmax, nx, dx, x1);
	my ymin = ymin;
	my ymax = ymax;
	my ny = ny;
	my dy = dy;
	my y1 = y1;
}

integer SampledXY_getWindowSamplesY (
	const constSampledXY me,
	const double fromY,
	const double toY,
	integer *const iymin,
	integer *const iymax
) {
	const double riymin = 1.0 + Melder_roundUp   ((fromY - my y1) / my dy);
	const double riymax = 1.0 + Melder_roundDown ((toY - my y1) / my dy);   // could be above 32-bit LONG_MAX
	*iymin = ( riymin < 1.0 ? 1 : (integer) riymin );
	*iymax = ( riymax > double (my ny) ? my ny : (integer) riymax );
	if (*iymin > *iymax)
		return 0;
	return *iymax - *iymin + 1;
}

void SampledXY_unidirectionalAutowindowY (
	const constSampledXY me,
	double *const ymin,
	double *const ymax
) {
	if (*ymin >= *ymax) {
		*ymin = my ymin;
		*ymax = my ymax;
	}
}
void SampledXY_bidirectionalAutowindowY (
	const constSampledXY me,
	double *const y1,
	double *const y2
) {
	if (*y1 == *y2) {
		*y1 = my ymin;
		*y2 = my ymax;
	}
}

/* End of file SampledXY.cpp */
