/* Vector_extensions.cpp
 *
 * Copyright (C) 2022 David Weenink
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

#include "Sampled.h"
#include "Vector_extensions.h"

#include "enums_getText.h"
#include "Vector_extensions_enums.h"
#include "enums_getValue.h"
#include "Vector_extensions_enums.h"

double Vector_getNearestLevelCrossing (Vector me, integer channel, double position, double level, kVectorSearchDirection searchDirection) {
	const double *amplitude = & my z [channel] [0];
	const integer leftSample = Sampled_xToLowIndex (me, position);
	if (leftSample > my nx)
		return undefined;
	const integer rightSample = leftSample + 1;
	
	auto interpolateLinear = [&] (integer i1) -> double {
		const integer i2 = i1 + 1;
		const double x1 = Sampled_indexToX (me, i1); // x2 = x1 + dx
		const double y1 = my z [channel] [i1], y2 = my z [channel] [i2];
		/*
			y = x1 + (x2 - x1) * (y1 - level) / (y1 - y2) = x1 + dx * (y1 - level) / (y1 - y2)
		*/
		return x1 + my dx * (y1 - level) / (y1 - y2);   // y1 <> y2!
	};
	/*
		Are we already at a level crossing?
	*/
	if (leftSample >= 1 && rightSample <= my nx &&
			(amplitude [leftSample] >= level) != (amplitude [rightSample] >= level)) 
	{
		const double crossing = interpolateLinear (leftSample);
		return searchDirection == kVectorSearchDirection::LEFT ?
			( crossing <= position ? crossing : undefined ) :
			( crossing >= position ? crossing : undefined );
	}
	
	double leftCrossing = undefined;
	if (searchDirection == kVectorSearchDirection::LEFT || searchDirection == kVectorSearchDirection::NEAREST) {
		for (integer ileft = leftSample - 1; ileft >= 1; ileft --)
			if ((amplitude [ileft] >= level) != (amplitude [ileft + 1] >= level)) {
				leftCrossing = interpolateLinear (ileft);
				break;
			}
		if (searchDirection == kVectorSearchDirection::LEFT)
			return leftCrossing;
	}
	
	if (rightSample < 1)
		return undefined;
	double rightCrossing = undefined;
	if (searchDirection == kVectorSearchDirection::RIGHT || searchDirection == kVectorSearchDirection::NEAREST) {
		for (integer iright = rightSample + 1; iright <= my nx; iright ++)
			if ((amplitude [iright] >= level) != (amplitude [iright - 1] >= level)) {
				rightCrossing = interpolateLinear (iright - 1);
				break;
			}
		if (searchDirection == kVectorSearchDirection::RIGHT)
			return rightCrossing;
	}

	return
		isdefined (leftCrossing) && isdefined (rightCrossing) ?
				( position - leftCrossing < rightCrossing - position ? leftCrossing : rightCrossing )
		: isdefined (leftCrossing) ? leftCrossing
		: isdefined (rightCrossing) ? rightCrossing
		: undefined;
}

/* End of file Vector_extensions.cpp */
