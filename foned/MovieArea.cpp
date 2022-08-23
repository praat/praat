/* MovieArea.cpp
 *
 * Copyright (C) 2011-2020,2022 Paul Boersma
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

#include "MovieArea.h"

Thing_implement (MovieArea, FunctionArea, 0);

void structMovieArea :: v_drawInside () {
	Graphics_setWindow (our graphics(), our startWindow(), our endWindow(), 0.0, 1.0);
	const integer firstFrame = Melder_clippedLeft (1_integer, Sampled_xToNearestIndex (our movie(), our startWindow()));
	const integer lastFrame = Melder_clippedRight (Sampled_xToNearestIndex (our movie(), our endWindow()), our movie() -> nx);
	for (integer iframe = firstFrame; iframe <= lastFrame; iframe ++) {
		const double time = Sampled_indexToX (our movie(), iframe);
		const double timeLeft = Melder_clippedLeft (our startWindow(), time - 0.5 * movie() -> dx);
		const double timeRight = Melder_clippedRight (time + 0.5 * our movie() -> dx, our endWindow());
		Movie_paintOneImageInside (our movie(), our graphics(), iframe, timeLeft, timeRight, 0.0, 1.0);
	}
}

/* End of file MovieArea.cpp */
