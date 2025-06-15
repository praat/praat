/* Artword_Speaker.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
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

/*
 * pb 1999/02/07
 * pb 2002/07/16 GPL
 */

#include "Art_Speaker.h"
#include "Artword_Speaker.h"
#ifdef sgi
	int sginap (long ticks);
#endif

void Artword_Speaker_draw (Artword artword, Speaker speaker, Graphics g, int numberOfSteps) {
	int i;
	double oldLineWidth = Graphics_inqLineWidth (g);
	Art art = Art_create ();
	for (i = 0; i <= numberOfSteps; i ++) {
		Artword_intoArt (artword, art, i * artword -> totalTime / numberOfSteps);
		Graphics_setLineWidth (g, 2 + i + i);
		Art_Speaker_draw (art, speaker, g);
	}
	forget (art);
	Graphics_setLineWidth (g, oldLineWidth);
}

void Artword_Speaker_movie (Artword artword, Speaker speaker, Graphics g) {
	double tim;
	#if defined (sgi)
		double timeStep = 1.0 / 60;
	#else
		double timeStep = 0.00001;
	#endif
	Art art = Art_create ();
	for (tim = 0.0; tim < artword -> totalTime; tim += timeStep) {
		Artword_intoArt (artword, art, tim);
		Graphics_setViewport (g, 0, 1, 0, 1);
		Graphics_clearWs (g);
		Art_Speaker_draw (art, speaker, g);
		Graphics_flushWs (g);
		#ifdef sgi
			sginap (1);
		#endif
	}
}

/* End of file Artword_Speaker.c */
