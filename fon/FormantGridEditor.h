#ifndef _FormantGridEditor_h_
#define _FormantGridEditor_h_
/* FormantGridEditor.h
 *
 * Copyright (C) 2008-2011,2012 Paul Boersma & David Weenink
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

#include "FunctionEditor.h"
#include "FormantGrid.h"

struct FormantGridEditor_Play {
	double samplingFrequency;
};
struct FormantGridEditor_Source {
	struct { double tStart, f0Start, tMid, f0Mid, tEnd, f0End; } pitch;
	struct { double adaptFactor, maximumPeriod, openPhase, collisionPhase, power1, power2; } phonation;
};

Thing_define (FormantGridEditor, FunctionEditor) {
	// new data:
	public:
		bool editingBandwidths;
		long selectedFormant;
		double formantFloor, formantCeiling, bandwidthFloor, bandwidthCeiling, ycursor;
		struct FormantGridEditor_Play play;
		struct FormantGridEditor_Source source;
	// overridden methods:
		virtual void v_createMenus ();
		virtual void v_draw ();
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
		virtual void v_play (double tmin, double tmax);
	// new methods:
		virtual bool v_hasSourceMenu () { return true; }
};

void FormantGridEditor_init (FormantGridEditor me, const wchar_t *title, FormantGrid data);

FormantGridEditor FormantGridEditor_create (const wchar_t *title, FormantGrid data);

void FormantGridEditor_prefs (void);

/* End of file FormantGridEditor.h */
#endif
