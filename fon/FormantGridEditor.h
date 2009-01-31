#ifndef _FormantGridEditor_h_
#define _FormantGridEditor_h_
/* FormantGridEditor.h
 *
 * Copyright (C) 2008-2009 Paul Boersma & David Weenink
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
 * pb 2009/01/23
 */

#ifndef _FunctionEditor_h_
	#include "FunctionEditor.h"
#endif
#ifndef _FormantGrid_h_
	#include "FormantGrid.h"
#endif

struct FormantGridEditor_Play {
	double samplingFrequency;
};
struct FormantGridEditor_Source {
	struct { double tStart, f0Start, tMid, f0Mid, tEnd, f0End; } pitch;
	struct { double adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2; } phonation;
};

#define FormantGridEditor__parents(Klas) FunctionEditor__parents(Klas) Thing_inherit (Klas, FunctionEditor)
Thing_declare1 (FormantGridEditor);

#define FormantGridEditor__members(Klas) FunctionEditor__members(Klas) \
	bool editingBandwidths; \
	long selectedFormant; \
	double formantFloor, formantCeiling, bandwidthFloor, bandwidthCeiling, ycursor; \
	struct FormantGridEditor_Play play; \
	struct FormantGridEditor_Source source;
#define FormantGridEditor__methods(Klas) FunctionEditor__methods(Klas) \
	bool hasSourceMenu;
Thing_declare2 (FormantGridEditor, FunctionEditor);

int FormantGridEditor_init (FormantGridEditor me, Widget parent, const wchar_t *title, FormantGrid data);

FormantGridEditor FormantGridEditor_create (Widget parent, const wchar_t *title, FormantGrid data);

void FormantGridEditor_prefs (void);

/* End of file FormantGridEditor.h */
#endif
