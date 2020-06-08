/* TimeSoundAnalysisEditor_prefs.h
 *
 * Copyright (C) 2013,2015 Paul Boersma
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

prefs_begin (TimeSoundAnalysisEditor)

	prefs_add_double_with_data  (TimeSoundAnalysisEditor, longestAnalysis,                 1, U"10.0")   // seconds
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, timeStepStrategy,                1, kTimeSoundAnalysisEditor_timeStepStrategy, DEFAULT)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, fixedTimeStep,                   1, U"0.01")   // seconds
	prefs_add_integer_with_data (TimeSoundAnalysisEditor, numberOfTimeStepsPerView,        1, U"100")
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, spectrogram_show,                1, true)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, spectrogram_viewFrom,            2, U"0.0")   // Hz
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, spectrogram_viewTo,              2, U"5000.0")   // Hz
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, spectrogram_windowLength,        2, U"0.005")   // seconds
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, spectrogram_dynamicRange,        2, U"70.0")   // dB
	prefs_add_integer_with_data (TimeSoundAnalysisEditor, spectrogram_timeSteps,           2, U"1000")
	prefs_add_integer_with_data (TimeSoundAnalysisEditor, spectrogram_frequencySteps,      2, U"250")
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, spectrogram_method,              2, kSound_to_Spectrogram_method, DEFAULT)
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, spectrogram_windowShape,         2, kSound_to_Spectrogram_windowShape, DEFAULT)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, spectrogram_autoscaling,         2, true)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, spectrogram_maximum,             2, U"100.0")   // dB/Hz
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, spectrogram_preemphasis,         2, U"6.0")   // dB/octave
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, spectrogram_dynamicCompression,  2, U"0.0")   // 0..1
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, spectrogram_picture_garnish,     1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, pitch_show,                      1, true)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_floor,                     1, U"75.0")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_ceiling,                   1, U"500.0")
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, pitch_unit,                      1, kPitch_unit, DEFAULT)
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, pitch_drawingMethod,             1, kTimeSoundAnalysisEditor_pitch_drawingMethod, DEFAULT)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_viewFrom,                  1, U"0.0 (= auto)")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_viewTo,                    1, U"0.0 (= auto)")
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, pitch_method,                    1, kTimeSoundAnalysisEditor_pitch_analysisMethod, DEFAULT)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, pitch_veryAccurate,              1, false)
	prefs_add_integer_with_data (TimeSoundAnalysisEditor, pitch_maximumNumberOfCandidates, 1, U"15")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_silenceThreshold,          1, U"0.03")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_voicingThreshold,          1, U"0.45")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_octaveCost,                1, U"0.01")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_octaveJumpCost,            1, U"0.35")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pitch_voicedUnvoicedCost,        1, U"0.14")
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, pitch_picture_speckle,           1, false)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, pitch_picture_garnish,           1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, intensity_show,                  1, false)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, intensity_viewFrom,              1, U"50.0")   // dB
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, intensity_viewTo,                1, U"100.0")   // dB
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, intensity_averagingMethod,       1, kTimeSoundAnalysisEditor_intensity_averagingMethod, DEFAULT)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, intensity_subtractMeanPressure,  1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, intensity_picture_garnish,       1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, formant_show,                    1, false)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, formant_ceiling,                 1, U"5500.0")   // Hz
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, formant_numberOfFormants,        1, U"5.0")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, formant_windowLength,            1, U"0.025")   // seconds
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, formant_dynamicRange,            1, U"30.0")   // dB
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, formant_dotSize,                 1, U"1.0")   // mm
	prefs_add_enum_with_data    (TimeSoundAnalysisEditor, formant_method,                  1, kTimeSoundAnalysisEditor_formant_analysisMethod, BURG)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, formant_preemphasisFrom,         1, U"50.0")   // Hz
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, formant_picture_garnish,         1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, pulses_show,                     1, false)
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pulses_maximumPeriodFactor,      1, U"1.3")
	prefs_add_double_with_data  (TimeSoundAnalysisEditor, pulses_maximumAmplitudeFactor,   1, U"1.6")
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, pulses_picture_garnish,          1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, log1_toInfoWindow,               1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, log1_toLogFile,                  1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, log2_toInfoWindow,               1, true)
	prefs_add_bool_with_data    (TimeSoundAnalysisEditor, log2_toLogFile,                  1, true)
	#if defined (macintosh)
		prefs_add_string_with_data (TimeSoundAnalysisEditor, log1_fileName,               1, U"~/Desktop/Pitch Log")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, log2_fileName,               1, U"~/Desktop/Formant Log")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, logScript3,                  1, U"~/Desktop/Log script 3")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, logScript4,                  1, U"~/Desktop/Log script 4")
	#elif defined (WIN32)
		prefs_add_string_with_data (TimeSoundAnalysisEditor, log1_fileName,               1, U"C:\\WINDOWS\\DESKTOP\\Pitch Log.txt")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, log2_fileName,               1, U"C:\\WINDOWS\\DESKTOP\\Formant Log.txt")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, logScript3,                  1, U"C:\\WINDOWS\\DESKTOP\\Log script 3.praat")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, logScript4,                  1, U"C:\\WINDOWS\\DESKTOP\\Log script 4.praat")
	#else
		prefs_add_string_with_data (TimeSoundAnalysisEditor, log1_fileName,               1, U"~/pitch_log")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, log2_fileName,               1, U"~/formant_log")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, logScript3,                  1, U"~/log_script3")
		prefs_add_string_with_data (TimeSoundAnalysisEditor, logScript4,                  1, U"~/log_script4")
	#endif
	prefs_add_string_with_data  (TimeSoundAnalysisEditor, log1_format,                     1, U"Time 'time:6' seconds, pitch 'f0:2' Hz")
	prefs_add_string_with_data  (TimeSoundAnalysisEditor, log2_format,                     1, U"'t1:4''tab$''t2:4''tab$''f1:0''tab$''f2:0''tab$''f3:0'")

prefs_end (TimeSoundAnalysisEditor)

/* End of file TimeSoundAnalysisEditor_prefs.h */
