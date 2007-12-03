/* TimeSoundAnalysisEditor_enums.h
 *
 * Copyright (C) 2007 Paul Boersma
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
 * pb 2007/12/02
 */

enums_begin (kTimeSoundAnalysisEditor_pitch_drawingMethod, 1)
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, CURVE, L"curve", 1)
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, CURVE, L"Curve")
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, SPECKLE, L"speckles", 2)
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, SPECKLE, L"Speckles")
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, SPECKLE, L"speckle")
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, SPECKLE, L"Speckle")
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, AUTOMATIC, L"automatic", 3)
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, AUTOMATIC, L"Automatic")
enums_end (kTimeSoundAnalysisEditor_pitch_drawingMethod, AUTOMATIC, 3)

/* End of file TimeSoundAnalysisEditor_enums.h */
