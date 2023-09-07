/* CubeGrid.cpp
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

#include "CubeGrid.h"
#include "Graphics.h"

#include "oo_DESTROY.h"
#include "CubeGrid_def.h"
#include "oo_COPY.h"
#include "CubeGrid_def.h"
#include "oo_EQUAL.h"
#include "CubeGrid_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "CubeGrid_def.h"
#include "oo_WRITE_TEXT.h"
#include "CubeGrid_def.h"
#include "oo_READ_TEXT.h"
#include "CubeGrid_def.h"
#include "oo_WRITE_BINARY.h"
#include "CubeGrid_def.h"
#include "oo_READ_BINARY.h"
#include "CubeGrid_def.h"
#include "oo_DESCRIPTION.h"
#include "CubeGrid_def.h"

Thing_implement (CubePoint, Function, 0);

Thing_implement (CubeTier, Function, 0);

Thing_implement (CubeGrid, Function, 0);

autoCubePoint CubeGrid_average (CubeGrid me, integer tierNumber, double tmin, double tmax) {
	try {
		Melder_require (tierNumber >= 1, U"Tier number should be positive");
		Melder_require (tierNumber <= my tiers.size,
			U"Tier number (", tierNumber, U") should not be higher than the number of tiers (", my tiers.size, U").");
		CubeTier tier = my tiers.at [tierNumber];
		autoCubePoint you = Thing_new (CubePoint);
		your red   = 0.0;
		your green = 0.0;
		your blue  = 0.0;
		double numberOfSeconds = 0.0;
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			CubePoint inpoint = tier -> points.at [ipoint];
			if (inpoint -> xmax > tmin && inpoint -> xmin < tmax) {
				const double duration = inpoint -> xmax - inpoint -> xmin;
				your red   += inpoint -> red * duration;
				your green += inpoint -> green * duration;
				your blue  += inpoint -> blue * duration;
				numberOfSeconds += duration;
			}
		}
		if (numberOfSeconds == 0.0) {
			your red   = undefined;
			your green = undefined;
			your blue  = undefined;
		} else {
			your red   /= numberOfSeconds;
			your green /= numberOfSeconds;
			your blue  /= numberOfSeconds;
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": average not computed.");
	}
}

autoVEC CubeGrid_getAverages (CubeGrid me, integer tierNumber, double tmin, double tmax) {
	autoCubePoint point = CubeGrid_average (me, tierNumber, tmin, tmax);
	autoVEC result = raw_VEC (3);
	result [1] = point -> red;
	result [2] = point -> green;
	result [3] = point -> blue;
	return result;
}

void CubeGrid_paintInside (CubeGrid me, Graphics graphics, double tmin, double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	Graphics_setColour (graphics, Melder_WHITE);
	Graphics_setWindow (graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_fillRectangle (graphics, 0.0, 1.0, 0.0, 1.0);
	Graphics_setWindow (graphics, tmin, tmax, 0.0, my tiers.size);
	for (integer itier = 1; itier <= my tiers.size; itier ++) {
		CubeTier tier = my tiers.at [itier];
		const double ymin = my tiers.size - itier, ymax = ymin + 1.0;
		for (integer ipoint = 1; ipoint <= tier -> points.size; ipoint ++) {
			CubePoint point = tier -> points.at [ipoint];
			if (point -> xmax > tmin && point -> xmin < tmax) {
				const double xmin = Melder_clippedLeft (tmin, point -> xmin);
				const double xmax = Melder_clippedRight (point -> xmax, tmax);
				Graphics_setColour (graphics, MelderColour (point -> red, point -> green, point -> blue));
				Graphics_fillRectangle (graphics, xmin, xmax, ymin, ymax);
			}
		}
		Graphics_setColour (graphics, Melder_BLACK);
		if (itier > 1) {
			Graphics_setLineWidth (graphics, 1.0);
			Graphics_line (graphics, tmin, ymax, tmax, ymax);
		}
	}
	Graphics_setLineWidth (graphics, 1.0);
	Graphics_setColour (graphics, Melder_BLACK);
}

void CubeGrid_paint (CubeGrid me, Graphics g, double tmin, double tmax, bool garnish) {
	Graphics_setInner (g);
	CubeGrid_paintInside (me, g, tmin, tmax);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
	}
}

/* End of file CubeGrid.cpp */

