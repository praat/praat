/* Tube.c
 *
 * Copyright (C) 1994-2003 David Weenink
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
 djmw 20030613 Creation
 djmw 20061212 Changed info to Melder_writeLine<x> format.
*/

#include "Tube.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "Tube_def.h"
#include "oo_COPY.h"
#include "Tube_def.h"
#include "oo_EQUAL.h"
#include "Tube_def.h"
#include "oo_WRITE_ASCII.h"
#include "Tube_def.h"
#include "oo_WRITE_BINARY.h"
#include "Tube_def.h"
#include "oo_READ_ASCII.h"
#include "Tube_def.h"
#include "oo_READ_BINARY.h"
#include "Tube_def.h"
#include "oo_DESCRIPTION.h"
#include "Tube_def.h"

static void classTube_info (I)
{
	iam (Tube);
	classData -> info (me);
	MelderInfo_writeLine5 ("Time domain: ", Melder_double (my xmin), " to ", Melder_double (my xmax),
		" (s).");
	MelderInfo_writeLine2 ("maximum number of segments: ", Melder_integer (my maxnSegments));
	MelderInfo_writeLine2 ("Number of frames: ", Melder_integer (my nx));
	MelderInfo_writeLine3 ("Time step: ", Melder_double (my dx), " (s).");
	MelderInfo_writeLine3 ("First frame at: ", Melder_double (my x1), " (s).");
}

class_methods (Tube, Sampled)
	class_method_local (Tube, destroy)
	class_method_local (Tube, equal)
	class_method_local (Tube, copy)
	class_method_local (Tube, info)
	class_method_local (Tube, readAscii)
	class_method_local (Tube, readBinary)
	class_method_local (Tube, writeAscii)
	class_method_local (Tube, writeBinary)
	class_method_local (Tube, description)
class_methods_end

int Tube_Frame_init (Tube_Frame me, long nSegments, double length)
{
	my nSegments = nSegments;
	my length = length;
	return nSegments > 0 && ((my c = NUMdvector (1, nSegments)) != NULL);
}


void Tube_Frame_free (Tube_Frame me)
{
	Tube_Frame_destroy (me);
}

/* Gray & Markel (1979), LPTRN */
int Tube_Frames_rc_into_area (Tube_Frame me, Tube_Frame thee)
{
	long i;
	double s = 0.0001; /* 1.0 cm^2 at glottis */
	double *rc = my c, *area = thy c;
	
	if (my nSegments > thy nSegments) return 0;
	
	for (i = my nSegments; i > 0; i--)
	{
		s *= (1.0 + rc[i]) / (1.0 - rc[i]);
		area[i] = s;
	}
	
	return 1;
}

static void Tube_setLengths (I, double length)
{
	iam (Tube);
	long i;
	for (i = 1; i <= my nx; i++)
	{
		Tube_Frame f = & my frame[i];
		if (f) f -> length = length;
	}
}

int Tube_init (I, double tmin, double tmax, long nt, double dt, double t1,
	long maxnSegments, double defaultLength)
{
	iam (Tube);
	my maxnSegments = maxnSegments;
	if (Sampled_init (me, tmin, tmax, nt, dt, t1) &&
		(my frame = NUMstructvector (Tube_Frame, 1, nt)))
	{
		Tube_setLengths (me, defaultLength);
		return 1;
	}
	return 0;
}

class_methods (Area, Tube)
class_methods_end

int Area_init (Area me, double tmin, double tmax, long nt, double dt, double t1,
	long maxnSegments, double defaultLength)
{
	return Tube_init (me, tmin, tmax, nt, dt, t1, maxnSegments, defaultLength);
}
		
Area Area_create (double tmin, double tmax, long nt, double dt, double t1,
	long maxnSegments, double defaultLength)
{
	Area me = new (Area);
	
	if (! me || 
		! Area_init (me, tmin, tmax, nt, dt, t1, maxnSegments, defaultLength)) forget (me);
	return me;
}

class_methods (RC, Tube)
class_methods_end

int RC_init (RC me, double tmin, double tmax, long nt, double dt, double t1,
	long maxnSegments, double defaultLength)
{	
	return Tube_init (me, tmin, tmax, nt, dt, t1, maxnSegments, defaultLength);
}

RC RC_create (double tmin, double tmax, long nt, double dt, double t1,
	long maxnCoefficients, double defaultLength)
{
	RC me = new (RC);
	
	if (! me || 
		! RC_init (me, tmin, tmax, nt, dt, t1, maxnCoefficients, defaultLength)) forget (me);
	return me;
}
		
/* End of file Tube.c */
