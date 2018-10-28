/* Art_Speaker.h
 *
 * Copyright (C) 1992-2005,2011,2016-2018 Paul Boersma
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
#include "Speaker.h"
#include "Graphics.h"

void Art_Speaker_toVocalTract (Art art, Speaker speaker,
	double intX [], double intY [], double extX [], double extY [],
	double *out_bodyX, double *out_bodyY);
/*
	Function:
		compute key places of the supralaryngeal vocal tract.
	Preconditions:
		index intX [1..13];
		index intY [1..13];
		index extX [1..9];
		index extY [1..9];
	Postconditions:
		int [1..6] is anterior larynx, hyoid, and tongue root.
		int [6..7] is the arc of the tongue body.
		int [7..13] is tongue blade, lower teeth, and lower lip.
		ext [1..5] is posterior larynx, back pharynx wall, and velic.
		ext [5..6] is the arc of the velum and palate.
		ext [6..9] is the gums, upper teeth and upper lip.
*/

void Art_Speaker_draw (Art art, Speaker speaker, Graphics g);
void Art_Speaker_fillInnerContour (Art art, Speaker speaker, Graphics g);

void Art_Speaker_meshVocalTract (Art art, Speaker speaker,
	double xi [], double yi [], double xe [], double ye [],
	double xmm [], double ymm [], bool closed []);

void Art_Speaker_drawMesh (Art art, Speaker speaker, Graphics g);

/* End of file Art_Speaker.h */
