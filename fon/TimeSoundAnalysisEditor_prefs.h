/* TimeSoundAnalysisEditor_prefs.h
 *
 * Copyright (C) 2013,2015-2017,2020,2022 Paul Boersma
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

EditorPrefs_begin (TimeSoundAnalysisEditor)

	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, longestAnalysis,                 1, U"10.0")   // seconds
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, timeStepStrategy,                1, kTimeSoundAnalysisEditor_timeStepStrategy, DEFAULT)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, fixedTimeStep,                   1, U"0.01")   // seconds
	EditorInstancePrefs_addInteger (TimeSoundAnalysisEditor, numberOfTimeStepsPerView,        1, U"100")
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, spectrogram_show,                1, true)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_viewFrom,            2, U"0.0")   // Hz
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_viewTo,              2, U"5000.0")   // Hz
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_windowLength,        2, U"0.005")   // seconds
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_dynamicRange,        2, U"70.0")   // dB
	EditorInstancePrefs_addInteger (TimeSoundAnalysisEditor, spectrogram_timeSteps,           2, U"1000")
	EditorInstancePrefs_addInteger (TimeSoundAnalysisEditor, spectrogram_frequencySteps,      2, U"250")
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, spectrogram_method,              2, kSound_to_Spectrogram_method, DEFAULT)
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, spectrogram_windowShape,         2, kSound_to_Spectrogram_windowShape, DEFAULT)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, spectrogram_autoscaling,         2, true)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_maximum,             2, U"100.0")   // dB/Hz
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_preemphasis,         2, U"6.0")   // dB/octave
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_dynamicCompression,  2, U"0.0")   // 0..1
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, spectrogram_picture_garnish,     1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_show,                      1, true)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_floor,                     1, U"75.0")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_ceiling,                   1, U"500.0")
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, pitch_unit,                      1, kPitch_unit, DEFAULT)
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, pitch_drawingMethod,             1, kTimeSoundAnalysisEditor_pitch_drawingMethod, DEFAULT)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_viewFrom,                  1, U"0.0 (= auto)")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_viewTo,                    1, U"0.0 (= auto)")
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, pitch_method,                    1, kTimeSoundAnalysisEditor_pitch_analysisMethod, DEFAULT)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_veryAccurate,              1, false)
	EditorInstancePrefs_addInteger (TimeSoundAnalysisEditor, pitch_maximumNumberOfCandidates, 1, U"15")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_silenceThreshold,          1, U"0.03")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_voicingThreshold,          1, U"0.45")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_octaveCost,                1, U"0.01")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_octaveJumpCost,            1, U"0.35")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_voicedUnvoicedCost,        1, U"0.14")
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_picture_speckle,           1, false)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_picture_garnish,           1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, intensity_show,                  1, false)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, intensity_viewFrom,              1, U"50.0")   // dB
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, intensity_viewTo,                1, U"100.0")   // dB
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, intensity_averagingMethod,       1, kTimeSoundAnalysisEditor_intensity_averagingMethod, DEFAULT)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, intensity_subtractMeanPressure,  1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, intensity_picture_garnish,       1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, formant_show,                    1, false)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_ceiling,                 1, U"5500.0")   // Hz
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_numberOfFormants,        1, U"5.0")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_windowLength,            1, U"0.025")   // seconds
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_dynamicRange,            1, U"30.0")   // dB
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_dotSize,                 1, U"1.0")   // mm
	EditorInstancePrefs_addEnum    (TimeSoundAnalysisEditor, formant_method,                  1, kTimeSoundAnalysisEditor_formant_analysisMethod, BURG)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_preemphasisFrom,         1, U"50.0")   // Hz
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, formant_picture_garnish,         1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, pulses_show,                     1, false)
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pulses_maximumPeriodFactor,      1, U"1.3")
	EditorInstancePrefs_addDouble  (TimeSoundAnalysisEditor, pulses_maximumAmplitudeFactor,   1, U"1.6")
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, pulses_picture_garnish,          1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, log1_toInfoWindow,               1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, log1_toLogFile,                  1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, log2_toInfoWindow,               1, true)
	EditorInstancePrefs_addBool    (TimeSoundAnalysisEditor, log2_toLogFile,                  1, true)
	#if defined (macintosh)
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, log1_fileName,               1, U"~/Desktop/Pitch Log")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, log2_fileName,               1, U"~/Desktop/Formant Log")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, logScript3,                  1, U"~/Desktop/Log script 3")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, logScript4,                  1, U"~/Desktop/Log script 4")
	#elif defined (WIN32)
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, log1_fileName,               1, U"C:\\WINDOWS\\DESKTOP\\Pitch Log.txt")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, log2_fileName,               1, U"C:\\WINDOWS\\DESKTOP\\Formant Log.txt")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, logScript3,                  1, U"C:\\WINDOWS\\DESKTOP\\Log script 3.praat")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, logScript4,                  1, U"C:\\WINDOWS\\DESKTOP\\Log script 4.praat")
	#else
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, log1_fileName,               1, U"~/pitch_log")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, log2_fileName,               1, U"~/formant_log")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, logScript3,                  1, U"~/log_script3")
		EditorInstancePrefs_addString (TimeSoundAnalysisEditor, logScript4,                  1, U"~/log_script4")
	#endif
	EditorInstancePrefs_addString  (TimeSoundAnalysisEditor, log1_format,                     1, U"Time 'time:6' seconds, pitch 'f0:2' Hz")
	EditorInstancePrefs_addString  (TimeSoundAnalysisEditor, log2_format,                     1, U"'t1:4''tab$''t2:4''tab$''f1:0''tab$''f2:0''tab$''f3:0'")

EditorPrefs_end (TimeSoundAnalysisEditor)

/* End of file TimeSoundAnalysisEditor_prefs.h */
