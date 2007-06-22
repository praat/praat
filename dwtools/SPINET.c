/* SPINET.c
 *
 * Copyright (C) 1993-2006 David Weenink
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
 djmw 20020813 GPL header
 djmw 20061212 Changed info to Melder_writeLine<x> format.
*/

#include "SPINET.h"
#include "Sound_extensions.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SPINET_def.h"
#include "oo_COPY.h"
#include "SPINET_def.h"
#include "oo_EQUAL.h"
#include "SPINET_def.h"
#include "oo_WRITE_TEXT.h"
#include "SPINET_def.h"
#include "oo_WRITE_BINARY.h"
#include "SPINET_def.h"
#include "oo_READ_TEXT.h"
#include "SPINET_def.h"
#include "oo_READ_BINARY.h"
#include "SPINET_def.h"
#include "oo_DESCRIPTION.h"
#include "SPINET_def.h"

static void info (I)
{
	iam (SPINET); double miny, maxy, mins, maxs;
	
	classData -> info (me);
 	if (! Sampled2_getWindowExtrema_d (me, my y, 1, my nx, 1, my ny, &miny, &maxy) ||
 		! Sampled2_getWindowExtrema_d (me, my s, 1, my nx, 1, my ny, &mins, &maxs)) return;
	MelderInfo_writeLine2 ("Minimum power: ", Melder_double (miny));
	MelderInfo_writeLine2 ("Maximum power: ", Melder_double (maxy));
	MelderInfo_writeLine2 ("Minimum power rectified: ", Melder_double (mins));
	MelderInfo_writeLine2 ("Maximum powerrectified: ", Melder_double (maxs));
}

class_methods (SPINET, Sampled2)
	class_method_local (SPINET, destroy)
	class_method_local (SPINET, equal)
	class_method_local (SPINET, copy)
	class_method_local (SPINET, readText)
	class_method_local (SPINET, readBinary)
	class_method_local (SPINET, writeText)
	class_method_local (SPINET, writeBinary)
	class_method_local (SPINET, description)
	class_method (info)
class_methods_end


SPINET SPINET_create (double tmin, double tmax, long nt, double dt, double t1,
	 double minimumFrequency, double maximumFrequency, long nFilters,
	 double excitationErbProportion, double inhibitionErbProportion)
{
	SPINET me = new (SPINET);
	double minErb = NUMhertzToErb (minimumFrequency);
	double maxErb = NUMhertzToErb (maximumFrequency);
	double dErb = (maxErb - minErb) / nFilters;
	if (! me || ! Sampled2_init (me, tmin, tmax, nt, dt, t1,
			minErb - dErb / 2, maxErb + dErb / 2, nFilters, dErb, minErb) ||
		! (my y = NUMdmatrix (1, nFilters, 1, nt)) ||
		! (my s = NUMdmatrix (1, nFilters, 1, nt))) { forget (me); return NULL; }
	my gamma = 4;
	my excitationErbProportion = excitationErbProportion;
	my inhibitionErbProportion = inhibitionErbProportion;
	return me;
}

void SPINET_spectralRepresentation (SPINET me, Graphics g, double fromTime, double toTime,
	double fromErb, double toErb, double minimum, double maximum, int enhanced,
	int garnish)
{
	long i, j; double **z = enhanced ? my s : my y;
	Matrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1,
		my ymin, my ymax, my ny, my dy, my y1);
	if (! thee) return;
	for (j=1; j <= my ny; j++) for (i=1; i <= my nx; i++)
		thy z[j][i] = z[j][i]; /* > 0 ? 10 * log10 (z[i][j] / 2e-5) : 0;*/
	Matrix_paintCells (thee, g, fromTime, toTime, fromErb, toErb, minimum, maximum);
	if (garnish)
	{
		Graphics_drawInnerBox(g);
		Graphics_textBottom (g, 1, "Time (s)");
		Graphics_marksBottom( g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, "Frequency (ERB)");
		Graphics_marksLeft( g, 2, 1, 1, 0);
		Graphics_textTop (g, 0, enhanced ? "Cooperative interaction output" : 
			"Gammatone filterbank output");
	}
	forget (thee);
}

void SPINET_drawSpectrum (SPINET me, Graphics g, double time, double fromErb, double toErb,
	double minimum, double maximum, int enhanced, int garnish)
{
	long i, ifmin, ifmax, icol = Sampled2_xToColumn (me, time);
	double **z = enhanced ? my s : my y; float *spec;
	if (icol < 1 || icol > my nx) return;
	if (toErb <= fromErb) { fromErb = my ymin; toErb = my ymax; }
	if (! Sampled2_getWindowSamplesY (me, fromErb, toErb, &ifmin, &ifmax) ||
		! (spec = NUMfvector (1, my ny))) return;
		
	for (i=1; i <= my ny; i++) spec[i] = z[i][icol];
	if (maximum <= minimum) NUMfvector_extrema (spec, ifmin, ifmax, &minimum, &maximum);
	if (maximum <= minimum) { minimum -= 1; maximum += 1; }
	for (i=ifmin; i <= ifmax; i++)
		if (spec[i] > maximum) spec[i] = maximum;
		else if (spec[i] < minimum) spec[i] = minimum;
	Graphics_setInner (g);
	Graphics_setWindow (g, fromErb, toErb, minimum, maximum);
	Graphics_function (g, spec, ifmin, ifmax,
		Sampled2_rowToY (me, ifmin), Sampled2_rowToY (me, ifmax));
	Graphics_unsetInner (g);
	if (garnish)
	{
		Graphics_drawInnerBox(g);
		Graphics_textBottom (g, 1, "Frequency (ERB)");
		Graphics_marksBottom( g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, "strength");
		Graphics_marksLeft( g, 2, 1, 1, 0);
	}
	NUMfvector_free (spec, 1);
}

/* End of file SPINET.c */
