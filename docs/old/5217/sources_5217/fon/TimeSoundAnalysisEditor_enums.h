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
 * pb 2007/12/05
 */

enums_begin (kTimeSoundAnalysisEditor_timeStepStrategy, 1)
	enums_add (kTimeSoundAnalysisEditor_timeStepStrategy, 1, AUTOMATIC, L"automatic")
	enums_add (kTimeSoundAnalysisEditor_timeStepStrategy, 2, FIXED, L"fixed")
	enums_add (kTimeSoundAnalysisEditor_timeStepStrategy, 3, VIEW_DEPENDENT, L"view-dependent")
	/* For reading old preferences files: */
	enums_alt (kTimeSoundAnalysisEditor_timeStepStrategy, AUTOMATIC, L"1")
	enums_alt (kTimeSoundAnalysisEditor_timeStepStrategy, FIXED, L"2")
	enums_alt (kTimeSoundAnalysisEditor_timeStepStrategy, VIEW_DEPENDENT, L"3")
enums_end (kTimeSoundAnalysisEditor_timeStepStrategy, 3, AUTOMATIC)

enums_begin (kTimeSoundAnalysisEditor_pitch_drawingMethod, 1)
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, 1, CURVE, L"curve")
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, 2, SPECKLE, L"speckles")
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod,    SPECKLE, L"speckle")
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, 3, AUTOMATIC, L"automatic")
	/* For reading old preferences files: */
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, CURVE, L"1")
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, SPECKLE, L"2")
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod, AUTOMATIC, L"3")
enums_end (kTimeSoundAnalysisEditor_pitch_drawingMethod, 3, AUTOMATIC)

enums_begin (kTimeSoundAnalysisEditor_pitch_analysisMethod, 1)
	enums_add (kTimeSoundAnalysisEditor_pitch_analysisMethod, 1, AUTOCORRELATION, L"autocorrelation")
	enums_add (kTimeSoundAnalysisEditor_pitch_analysisMethod, 2, CROSS_CORRELATION, L"cross-correlation")
	/* For reading old preferences files: */
	enums_alt (kTimeSoundAnalysisEditor_pitch_analysisMethod, AUTOCORRELATION, L"1")
	enums_alt (kTimeSoundAnalysisEditor_pitch_analysisMethod, CROSS_CORRELATION, L"2")
enums_end (kTimeSoundAnalysisEditor_pitch_analysisMethod, 2, AUTOCORRELATION)

enums_begin (kTimeSoundAnalysisEditor_intensity_averagingMethod, 0)
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 0, MEDIAN, L"median")   // BUG: this HAS to be zero!
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 1, MEAN_ENERGY, L"mean energy")
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 2, MEAN_SONES, L"mean sones")
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 3, MEAN_DB, L"mean dB")
	/* For reading old preferences files: */
	enums_alt (kTimeSoundAnalysisEditor_intensity_averagingMethod, MEDIAN, L"0")
	enums_alt (kTimeSoundAnalysisEditor_intensity_averagingMethod, MEAN_ENERGY, L"1")
	enums_alt (kTimeSoundAnalysisEditor_intensity_averagingMethod, MEAN_SONES, L"2")
	enums_alt (kTimeSoundAnalysisEditor_intensity_averagingMethod, MEAN_DB, L"3")
enums_end (kTimeSoundAnalysisEditor_intensity_averagingMethod, 3, MEAN_ENERGY)

enums_begin (kTimeSoundAnalysisEditor_formant_analysisMethod, 1)
	enums_add (kTimeSoundAnalysisEditor_formant_analysisMethod, 1, BURG, L"Burg")
	/* For reading old preferences files: */
	enums_alt (kTimeSoundAnalysisEditor_formant_analysisMethod, BURG, L"1")
enums_end (kTimeSoundAnalysisEditor_formant_analysisMethod, 1, BURG)

/* End of file TimeSoundAnalysisEditor_enums.h */
