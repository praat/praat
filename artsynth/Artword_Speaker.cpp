/* Artword_Speaker.cpp
 *
 * Copyright (C) 1992-2005,2011,2015-2017,2019,2020 Paul Boersma
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

#include "Art_Speaker.h"
#include "Artword_Speaker.h"

void Artword_Speaker_draw (Artword artword, Speaker speaker, Graphics g, int numberOfSteps) {
	const double oldLineWidth = Graphics_inqLineWidth (g);
	autoArt art = Art_create ();
	for (int i = 0; i <= numberOfSteps; i ++) {
		Artword_intoArt (artword, art.get(), i * artword -> totalTime / numberOfSteps);
		Graphics_setLineWidth (g, 2 + i + i);
		Art_Speaker_draw (art.get(), speaker, g);
	}
	Graphics_setLineWidth (g, oldLineWidth);
}

void Artword_Speaker_playMovie (Artword artword, Speaker speaker, Graphics graphics) {
	constexpr double timeStep = 0.03;
	autoArt art = Art_create ();
	for (double tim = 0.0; tim < artword -> totalTime; tim += timeStep) {
		Artword_intoArt (artword, art.get(), tim);
		Graphics_beginMovieFrame (graphics, & Melder_WHITE);
		Graphics_setWindow (graphics, 0.0, 1.0, 0.0, 1.0);
		Art_Speaker_draw (art.get(), speaker, graphics);
		Graphics_endMovieFrame (graphics, timeStep);
	}
}

/* End of file Artword_Speaker.cpp */
