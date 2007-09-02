#ifndef _FunctionEditor_SoundAnalysis_h_
#define _FunctionEditor_SoundAnalysis_h_
/* FunctionEditor_SoundAnalysis.h
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
 * pb 2007/09/01
 */

#ifndef _FunctionEditor_h_
	#include "FunctionEditor.h"
#endif
#ifndef _Spectrogram_h_
	#include "Spectrogram.h"
#endif

void FunctionEditor_SoundAnalysis_prefs (void);

void FunctionEditor_SoundAnalysis_forget (I);

void FunctionEditor_SoundAnalysis_draw (I);
void FunctionEditor_SoundAnalysis_drawPulses (I);

void FunctionEditor_SoundAnalysis_viewMenus (I);
void FunctionEditor_SoundAnalysis_selectionQueries (I);
void FunctionEditor_SoundAnalysis_addMenus (I);

void FunctionEditor_SoundAnalysis_init (I);

void FunctionEditor_SoundAnalysis_computeSpectrogram (I);
void FunctionEditor_SoundAnalysis_computePitch (I);
void FunctionEditor_SoundAnalysis_computeIntensity (I);
void FunctionEditor_SoundAnalysis_computeFormants (I);
void FunctionEditor_SoundAnalysis_computePulses (I);

/* End of file FunctionEditor_SoundAnalysis.h */
#endif
