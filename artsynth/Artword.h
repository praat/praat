#ifndef _Artword_h_
#define _Artword_h_
/* Artword.h
 *
 * Copyright (C) 1992-2005,2007,2009,2011,2015-2017 Paul Boersma
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

void Artword_setDefault (Artword me, kArt_muscle muscle);
/*
	Postconditions:
		my data [(int) muscle]. numberOfTargets == 2;
		my data [(int) muscle]. times [1] == 0.0;
		my data [(int) muscle]. times [2] == self -> totalTime;
		my data [(int) muscle]. targets [1] == 0.0;
		my data [(int) muscle]. targets [2] == 0.0;
		rest unchanged;	
*/

void Artword_setTarget (Artword me, kArt_muscle muscle, double time, double value);

double Artword_getTarget (Artword me, kArt_muscle muscle, double time);

void Artword_removeTarget (Artword me, kArt_muscle muscle, int16 targetNumber);
/*
	Function:
		remove one target from the target list of "muscle".
		If "iTarget" is the first or the last target in the list,
		only set the target to zero (begin and end targets remain).
	Preconditions:
		self != nullptr;
		muscle in enum class Art_MUSCLE;
		iTarget >= 1;
		iTarget <= self -> data [(int) muscle]. numberOfTargets;
	Postconditions:
		if (iTarget == 1)
			self -> data [(int) muscle]. targets [1] == 0.0;
		else if (iTarget == self -> data [(int) muscle]. numberOfTargets)
			self -> data [(int) muscle]. targets [iTarget] == 0.0;
		else
			self -> data [(int) muscle]. numberOfTargets == old self -> data [(int) muscle]. numberOfTargets - 1;
			for (i == iTarget..self -> data [(int) muscle]. numberOfTargets)
				self -> data [(int) muscle]. times [i] == old self -> data [(int) muscle]. times [i + 1];
				self -> data [(int) muscle]. targets [i] == old self -> data [(int) muscle]. targets [i + 1];
*/

void Artword_intoArt (Artword me, Art art, double time);
/*
	Function:
		Linear interpolation between targets, into an existing Art.
	Preconditions:
		me != nullptr;
		art != nullptr;
*/
	
void Artword_draw (Artword me, Graphics graphics, kArt_muscle muscle, bool garnish);

/* End of file Artword.h */
#endif
