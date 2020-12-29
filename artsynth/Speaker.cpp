/* Speaker.cpp
 *
 * Copyright (C) 1992-2005,2007,2011,2012,2015-2018,2020 Paul Boersma
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

#include "Speaker.h"

#include "oo_DESTROY.h"
#include "Speaker_def.h"
#include "oo_COPY.h"
#include "Speaker_def.h"
#include "oo_EQUAL.h"
#include "Speaker_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Speaker_def.h"
#include "oo_WRITE_TEXT.h"
#include "Speaker_def.h"
#include "oo_READ_TEXT.h"
#include "Speaker_def.h"
#include "oo_WRITE_BINARY.h"
#include "Speaker_def.h"
#include "oo_READ_BINARY.h"
#include "Speaker_def.h"
#include "oo_DESCRIPTION.h"
#include "Speaker_def.h"

Thing_implement (Speaker, Daata, 0);

autoSpeaker Speaker_create (conststring32 kindOfSpeaker, int16 numberOfVocalCordMasses) {
	autoSpeaker me = Thing_new (Speaker);

	/* Supralaryngeal dimensions are taken from P. Mermelstein (1973):		*/
	/*    "Articulatory model for the study of speech production",		*/
	/*    Journal of the Acoustical Society of America 53,1070 - 1082.		*/
	/* That was a male speaker, so we need scaling for other speakers:		*/

	double scaling;
	if (Melder_equ_firstCharacterCaseInsensitive (kindOfSpeaker, U"male")) my relativeSize = 1.1;
	else if (Melder_equ_firstCharacterCaseInsensitive (kindOfSpeaker, U"child")) my relativeSize = 0.7;
	else my relativeSize = 1.0;
	scaling = my relativeSize;

	/* Laryngeal system. Data for male speaker from Ishizaka and Flanagan.	*/

	if (Melder_equ_firstCharacterCaseInsensitive (kindOfSpeaker, U"female")) {
		my lowerCord.thickness = 1.4e-3;   // dx, in metres
		my upperCord.thickness = 0.7e-3;
		my cord.length = 10e-3;
		my lowerCord.mass = 0.02e-3;   // kilograms
		my upperCord.mass = 0.01e-3;
		my lowerCord.k1 = 10;   // Newtons per metre
		my upperCord.k1 = 4;
	} else if (Melder_equ_firstCharacterCaseInsensitive (kindOfSpeaker, U"male")) {
		my lowerCord.thickness = 2.0e-3;   // dx, in metres
		my upperCord.thickness = 1.0e-3;
		my cord.length = 18e-3;
		my lowerCord.mass = 0.1e-3;   // kilograms
		my upperCord.mass = 0.05e-3;
		my lowerCord.k1 = 12;   // Newtons per metre
		my upperCord.k1 = 4;
	} else /* "child" */ {
		my lowerCord.thickness = 0.7e-3;   // dx, in metres
		my upperCord.thickness = 0.3e-3;
		my cord.length = 6e-3;
		my lowerCord.mass = 0.003e-3;   // kilograms
		my upperCord.mass = 0.002e-3;
		my lowerCord.k1 = 6;   // Newtons per metre
		my upperCord.k1 = 2;
	}
	my cord.numberOfMasses = numberOfVocalCordMasses;
	if (numberOfVocalCordMasses == 1) {
		my lowerCord.thickness += my upperCord.thickness;
		my lowerCord.mass += my upperCord.mass;
		my lowerCord.k1 += my upperCord.k1;
	}

	/* Supralaryngeal system. Data from Mermelstein. */

	my velum.x = -0.031 * scaling;
	my velum.y = 0.023 * scaling;
	my velum.a = atan2 (my velum.y, my velum.x);
	my palate.radius = sqrt (my velum.x * my velum.x + my velum.y * my velum.y);
	my tip.length = 0.034 * scaling;
	my neutralBodyDistance = 0.086 * scaling;
	my alveoli.x = 0.024 * scaling;
	my alveoli.y = 0.0302 * scaling;
	my alveoli.a = atan2 (my alveoli.y, my alveoli.x);
	my teethCavity.dx1 = -0.009 * scaling;
	my teethCavity.dx2 = -0.004 * scaling;
	my teethCavity.dy = -0.011 * scaling;
	my lowerTeeth.a = -0.30;   // radians
	my lowerTeeth.r = 0.113 * scaling;   // metres
	my upperTeeth.x = 0.036 * scaling;
	my upperTeeth.y = 0.026 * scaling;
	my lowerLip.dx = 0.010 * scaling;
	my lowerLip.dy = -0.004 * scaling;
	my upperLip.dx = 0.010 * scaling;
	my upperLip.dy = 0.004 * scaling;

	my nose.Dx = 0.007 * scaling;
	my nose.Dz = 0.014 * scaling;
	my nose.weq = raw_VEC (14);
	my nose.weq [1] = 0.018 * scaling;
	my nose.weq [2] = 0.016 * scaling;
	my nose.weq [3] = 0.014 * scaling;
	my nose.weq [4] = 0.020 * scaling;
	my nose.weq [5] = 0.023 * scaling;
	my nose.weq [6] = 0.020 * scaling;
	my nose.weq [7] = 0.035 * scaling;
	my nose.weq [8] = 0.035 * scaling;
	my nose.weq [9] = 0.030 * scaling;
	my nose.weq [10] = 0.022 * scaling;
	my nose.weq [11] = 0.016 * scaling;
	my nose.weq [12] = 0.010 * scaling;
	my nose.weq [13] = 0.012 * scaling;
	my nose.weq [14] = 0.013 * scaling;

	return me;
}

/* End of file Speaker.cpp */
