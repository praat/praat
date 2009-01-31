#ifndef _SpectrumEditor_h_
#define _SpectrumEditor_h_
/* SpectrumEditor.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/06/10
 */

#ifndef _FunctionEditor_h_
	#include "FunctionEditor.h"
#endif
#ifndef _Spectrum_h_
	#include "Spectrum.h"
#endif

#define SpectrumEditor__parents(Klas) FunctionEditor__parents(Klas) Thing_inherit (Klas, FunctionEditor)
Thing_declare1 (SpectrumEditor);

#define SpectrumEditor__members(Klas) FunctionEditor__members(Klas) \
	double minimum, maximum, cursorHeight; \
	double bandSmoothing, dynamicRange; \
	Widget publishBandButton, publishSoundButton;
#define SpectrumEditor__methods(Klas) FunctionEditor__methods(Klas)
Thing_declare2 (SpectrumEditor, FunctionEditor);

SpectrumEditor SpectrumEditor_create (Widget parent, const wchar_t *title, Any data);

void SpectrumEditor_prefs (void);

/* End of file SpectrumEditor.h */
#endif
