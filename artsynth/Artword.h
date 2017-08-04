#ifndef _Artword_h_
#define _Artword_h_
/* Artword.h
 *
 * Copyright (C) 1992-2011,2015,2016,2017 Paul Boersma
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

#include "Articulation.h"
#include "Graphics.h"

#include "Artword_def.h"

autoArtword Artword_create (double totalTime);

void Artword_setDefault (Artword me, int feature);
/*
	Postconditions:
		my data [feature]. numberOfTargets == 2;
		my data [feature]. times [1] == 0.0;
		my data [feature]. times [2] == self -> totalTime;
		my data [feature]. targets [1] == 0.0;
		my data [feature]. targets [2] == 0.0;
		rest unchanged;	
*/

void Artword_setTarget (Artword me, int feature, double time, double value);

double Artword_getTarget (Artword me, int feature, double time);

void Artword_removeTarget (Artword me, int feature, int16 targetNumber);
/*
	Function:
		remove one target from the target list of "feature".
		If "iTarget" is the first or the last target in the list,
		only set the target to zero (begin and end targets remain).
	Preconditions:
		self != nullptr;
		feature in enum Art_MUSCLE;
		iTarget >= 1;
		iTarget <= self -> data [feature]. numberOfTargets;
	Postconditions:
		if (iTarget == 1)
			self -> data [feature]. targets [1] == 0.0;
		else if (iTarget == self -> data [feature]. numberOfTargets)
			self -> data [feature]. targets [iTarget] == 0.0;
		else
			self -> data [feature]. numberOfTargets == old self -> data [feature]. numberOfTargets - 1;
			for (i == iTarget..self -> data [feature]. numberOfTargets)
				self -> data [feature]. times [i] == old self -> data [feature]. times [i + 1];
				self -> data [feature]. targets [i] == old self -> data [feature]. targets [i + 1];	
*/

void Artword_intoArt (Artword me, Art art, double time);
/*
	Function:
		Linear interpolation between targets, into an existing Art.
	Preconditions:
		me != nullptr;
		art != nullptr;
*/
	
void Artword_draw (Artword me, Graphics graphics, int feature, bool garnish);

/* End of file Artword.h */
#endif
