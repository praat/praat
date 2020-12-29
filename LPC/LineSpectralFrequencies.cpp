/* LineSpectralFrequencies.cpp
 *
 * Copyright (C) 2016-2020 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20160421  Initial version
*/

#include "LineSpectralFrequencies.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_COPY.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_EQUAL.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_WRITE_TEXT.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_WRITE_BINARY.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_READ_TEXT.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_READ_BINARY.h"
#include "LineSpectralFrequencies_def.h"
#include "oo_DESCRIPTION.h"
#include "LineSpectralFrequencies_def.h"

Thing_implement (LineSpectralFrequencies, Sampled, 1);

void structLineSpectralFrequencies :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain: ", xmin, U" to ", xmax, U" (s).");
	MelderInfo_writeLine (U"Number of frequencies: ", maximumNumberOfFrequencies);
	MelderInfo_writeLine (U"Number of frames: ", nx);
	MelderInfo_writeLine (U"Time step: ", dx, U" (s).");
	MelderInfo_writeLine (U"First frame at: ", x1, U" (s).");
}

void LineSpectralFrequencies_Frame_init (LineSpectralFrequencies_Frame me, integer numberOfFrequencies) {
	my frequencies = zero_VEC (numberOfFrequencies);
	my numberOfFrequencies = numberOfFrequencies;
}

void LineSpectralFrequencies_init (LineSpectralFrequencies me, double tmin, double tmax, integer nt, double dt, double t1, integer numberOfFrequencies, double maximumFrequency) {
	my maximumFrequency = maximumFrequency;
	my maximumNumberOfFrequencies = numberOfFrequencies;
	Sampled_init (me, tmin, tmax, nt, dt, t1);
	my d_frames = newvectorzero <structLineSpectralFrequencies_Frame> (nt);
}

autoLineSpectralFrequencies LineSpectralFrequencies_create (double tmin, double tmax, integer nt, double dt, double t1, integer numberOfFrequencies, double maximumFrequency) {
	try {
		autoLineSpectralFrequencies me = Thing_new (LineSpectralFrequencies);
		LineSpectralFrequencies_init (me.get(), tmin, tmax, nt, dt, t1, numberOfFrequencies, maximumFrequency);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LineSpectralFrequencies not created.");
	}
}

void LineSpectralFrequencies_drawFrequencies (LineSpectralFrequencies me, Graphics g, double tmin, double tmax, double fmin, double fmax, bool garnish) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	if (fmax <= fmin) {
		const integer numberOfSelected = itmax - itmin + 1;
		autoVEC f1 = raw_VEC (numberOfSelected);
		autoVEC f2 = raw_VEC (numberOfSelected);
		for (integer iframe = itmin; iframe <= itmax; iframe ++) {
			f1 [iframe - itmin + 1] = my d_frames [iframe]. frequencies [1];
			f2 [iframe - itmin + 1] = my d_frames [iframe]. frequencies [my d_frames [iframe] . numberOfFrequencies];
		}
		double f1max, f2min;
		NUMextrema (f1.get(), & fmin, & f1max);
		NUMextrema (f2.get(), & f2min, & fmax);
	}
	if (fmax == fmin) {
		fmin = 0.0;
		fmax += 0.5;
	}

	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const LineSpectralFrequencies_Frame lsf = & my d_frames [iframe];
		const double x = Sampled_indexToX (me, iframe);
		for (integer ifreq = 1; ifreq <= lsf -> numberOfFrequencies; ifreq ++) {
			const double y = lsf -> frequencies [ifreq];
			if (y >= fmin && y <= fmax)
				Graphics_speckle (g, x, y);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (seconds)");
		Graphics_textLeft (g, true, U"Frequency (Hz)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

autoMatrix LineSpectralFrequencies_downto_Matrix (LineSpectralFrequencies me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, 0.5 + my maximumNumberOfFrequencies, my maximumNumberOfFrequencies, 1.0, 1.0);
		for (integer j = 1; j <= my nx; j ++) {
			const LineSpectralFrequencies_Frame lsf = & my d_frames [j];
			thy z . column (j) .part (1, lsf -> numberOfFrequencies) <<= lsf -> frequencies.get();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Matrix with linear prediction coefficients created.");
	}
}

/* End of file LineSpectralFrequencies.cpp */
