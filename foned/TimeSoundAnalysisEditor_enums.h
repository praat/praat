/* TimeSoundAnalysisEditor_enums.h
 *
 * Copyright (C) 2007,2013,2015,2017 Paul Boersma
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

enums_begin (kTimeSoundAnalysisEditor_timeStepStrategy, 1)
	enums_add (kTimeSoundAnalysisEditor_timeStepStrategy, 1, AUTOMATIC, U"automatic")
	enums_add (kTimeSoundAnalysisEditor_timeStepStrategy, 2, FIXED_, U"fixed")
	enums_add (kTimeSoundAnalysisEditor_timeStepStrategy, 3, VIEW_DEPENDENT, U"view-dependent")
enums_end (kTimeSoundAnalysisEditor_timeStepStrategy, 3, AUTOMATIC)

enums_begin (kTimeSoundAnalysisEditor_pitch_drawingMethod, 1)
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, 1, CURVE, U"curve")
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, 2, SPECKLE, U"speckles")
	enums_alt (kTimeSoundAnalysisEditor_pitch_drawingMethod,    SPECKLE, U"speckle")
	enums_add (kTimeSoundAnalysisEditor_pitch_drawingMethod, 3, AUTOMATIC, U"automatic")
enums_end (kTimeSoundAnalysisEditor_pitch_drawingMethod, 3, AUTOMATIC)

enums_begin (kTimeSoundAnalysisEditor_pitch_analysisMethod, 1)
	enums_add (kTimeSoundAnalysisEditor_pitch_analysisMethod, 1, AUTOCORRELATION, U"autocorrelation")
	enums_add (kTimeSoundAnalysisEditor_pitch_analysisMethod, 2, CROSS_CORRELATION, U"cross-correlation")
enums_end (kTimeSoundAnalysisEditor_pitch_analysisMethod, 2, AUTOCORRELATION)

enums_begin (kTimeSoundAnalysisEditor_intensity_averagingMethod, 0)
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 0, MEDIAN, U"median")   // BUG: this HAS to be zero!
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 1, MEAN_ENERGY, U"mean energy")
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 2, MEAN_SONES, U"mean sones")
	enums_add (kTimeSoundAnalysisEditor_intensity_averagingMethod, 3, MEAN_DB, U"mean dB")
enums_end (kTimeSoundAnalysisEditor_intensity_averagingMethod, 3, MEAN_ENERGY)

enums_begin (kTimeSoundAnalysisEditor_formant_analysisMethod, 1)
	enums_add (kTimeSoundAnalysisEditor_formant_analysisMethod, 1, BURG, U"Burg")
enums_end (kTimeSoundAnalysisEditor_formant_analysisMethod, 1, BURG)

/* End of file TimeSoundAnalysisEditor_enums.h */
