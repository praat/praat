/* Artword_Speaker_Sound.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Artword_Speaker_Sound.h"
#include "Artword_Speaker.h"
#include "Art_Speaker.h"

struct playInfo {
	Artword artword;
	Speaker speaker;
	Graphics graphics;
};

static int playCallback (void *playClosure, int phase, double tmin, double tmax, double t) {
	struct playInfo *me = (struct playInfo *) playClosure;
	static Art art;
	(void) phase;
	(void) tmin;
	(void) tmax;
	if (! art) art = Art_create ();
	Artword_intoArt (my artword, art, t);
	Graphics_clearWs (my graphics);
	Art_Speaker_draw (art, my speaker, my graphics);
	return 1;
}

void Artword_Speaker_Sound_movie (Artword artword, Speaker speaker, Sound sound, Graphics graphics) {
	try {
		static struct playInfo info;   // must be static!!!
		info. artword = artword;
		info. speaker = speaker;
		info. graphics = graphics;
		autoSound mySound = sound ? NULL : Sound_createSimple (1, artword -> totalTime, 44100);
		Sound_play (sound ? sound : mySound.peek(), playCallback, & info);
	} catch (MelderError) {
		Melder_throw (artword, " & ", speaker, ": movie not played.");
	}
}

/* End of file Artword_Speaker_Sound.cpp */
