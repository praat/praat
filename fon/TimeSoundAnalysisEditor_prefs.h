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

Prefs_begin (TimeSoundAnalysisEditor)

	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, longestAnalysis,                 1, U"10.0")   // seconds
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, timeStepStrategy,                1, kTimeSoundAnalysisEditor_timeStepStrategy, DEFAULT)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, fixedTimeStep,                   1, U"0.01")   // seconds
	InstancePrefs_addInteger (TimeSoundAnalysisEditor, numberOfTimeStepsPerView,        1, U"100")
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, spectrogram_show,                1, true)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_viewFrom,            2, U"0.0")   // Hz
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_viewTo,              2, U"5000.0")   // Hz
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_windowLength,        2, U"0.005")   // seconds
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_dynamicRange,        2, U"70.0")   // dB
	InstancePrefs_addInteger (TimeSoundAnalysisEditor, spectrogram_timeSteps,           2, U"1000")
	InstancePrefs_addInteger (TimeSoundAnalysisEditor, spectrogram_frequencySteps,      2, U"250")
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, spectrogram_method,              2, kSound_to_Spectrogram_method, DEFAULT)
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, spectrogram_windowShape,         2, kSound_to_Spectrogram_windowShape, DEFAULT)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, spectrogram_autoscaling,         2, true)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_maximum,             2, U"100.0")   // dB/Hz
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_preemphasis,         2, U"6.0")   // dB/octave
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, spectrogram_dynamicCompression,  2, U"0.0")   // 0..1
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, spectrogram_picture_garnish,     1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_show,                      1, true)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_floor,                     1, U"75.0")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_ceiling,                   1, U"500.0")
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, pitch_unit,                      1, kPitch_unit, DEFAULT)
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, pitch_drawingMethod,             1, kTimeSoundAnalysisEditor_pitch_drawingMethod, DEFAULT)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_viewFrom,                  1, U"0.0 (= auto)")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_viewTo,                    1, U"0.0 (= auto)")
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, pitch_method,                    1, kTimeSoundAnalysisEditor_pitch_analysisMethod, DEFAULT)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_veryAccurate,              1, false)
	InstancePrefs_addInteger (TimeSoundAnalysisEditor, pitch_maximumNumberOfCandidates, 1, U"15")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_silenceThreshold,          1, U"0.03")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_voicingThreshold,          1, U"0.45")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_octaveCost,                1, U"0.01")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_octaveJumpCost,            1, U"0.35")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pitch_voicedUnvoicedCost,        1, U"0.14")
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_picture_speckle,           1, false)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, pitch_picture_garnish,           1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, intensity_show,                  1, false)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, intensity_viewFrom,              1, U"50.0")   // dB
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, intensity_viewTo,                1, U"100.0")   // dB
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, intensity_averagingMethod,       1, kTimeSoundAnalysisEditor_intensity_averagingMethod, DEFAULT)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, intensity_subtractMeanPressure,  1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, intensity_picture_garnish,       1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, formant_show,                    1, false)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_ceiling,                 1, U"5500.0")   // Hz
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_numberOfFormants,        1, U"5.0")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_windowLength,            1, U"0.025")   // seconds
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_dynamicRange,            1, U"30.0")   // dB
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_dotSize,                 1, U"1.0")   // mm
	InstancePrefs_addEnum    (TimeSoundAnalysisEditor, formant_method,                  1, kTimeSoundAnalysisEditor_formant_analysisMethod, BURG)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, formant_preemphasisFrom,         1, U"50.0")   // Hz
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, formant_picture_garnish,         1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, pulses_show,                     1, false)
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pulses_maximumPeriodFactor,      1, U"1.3")
	InstancePrefs_addDouble  (TimeSoundAnalysisEditor, pulses_maximumAmplitudeFactor,   1, U"1.6")
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, pulses_picture_garnish,          1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, log1_toInfoWindow,               1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, log1_toLogFile,                  1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, log2_toInfoWindow,               1, true)
	InstancePrefs_addBool    (TimeSoundAnalysisEditor, log2_toLogFile,                  1, true)
	#if defined (macintosh)
		InstancePrefs_addString (TimeSoundAnalysisEditor, log1_fileName,               1, U"~/Desktop/Pitch Log")
		InstancePrefs_addString (TimeSoundAnalysisEditor, log2_fileName,               1, U"~/Desktop/Formant Log")
		InstancePrefs_addString (TimeSoundAnalysisEditor, logScript3,                  1, U"~/Desktop/Log script 3")
		InstancePrefs_addString (TimeSoundAnalysisEditor, logScript4,                  1, U"~/Desktop/Log script 4")
	#elif defined (WIN32)
		InstancePrefs_addString (TimeSoundAnalysisEditor, log1_fileName,               1, U"C:\\WINDOWS\\DESKTOP\\Pitch Log.txt")
		InstancePrefs_addString (TimeSoundAnalysisEditor, log2_fileName,               1, U"C:\\WINDOWS\\DESKTOP\\Formant Log.txt")
		InstancePrefs_addString (TimeSoundAnalysisEditor, logScript3,                  1, U"C:\\WINDOWS\\DESKTOP\\Log script 3.praat")
		InstancePrefs_addString (TimeSoundAnalysisEditor, logScript4,                  1, U"C:\\WINDOWS\\DESKTOP\\Log script 4.praat")
	#else
		InstancePrefs_addString (TimeSoundAnalysisEditor, log1_fileName,               1, U"~/pitch_log")
		InstancePrefs_addString (TimeSoundAnalysisEditor, log2_fileName,               1, U"~/formant_log")
		InstancePrefs_addString (TimeSoundAnalysisEditor, logScript3,                  1, U"~/log_script3")
		InstancePrefs_addString (TimeSoundAnalysisEditor, logScript4,                  1, U"~/log_script4")
	#endif
	InstancePrefs_addString  (TimeSoundAnalysisEditor, log1_format,                     1, U"Time 'time:6' seconds, pitch 'f0:2' Hz")
	InstancePrefs_addString  (TimeSoundAnalysisEditor, log2_format,                     1, U"'t1:4''tab$''t2:4''tab$''f1:0''tab$''f2:0''tab$''f3:0'")

Prefs_end (TimeSoundAnalysisEditor)

/* End of file TimeSoundAnalysisEditor_prefs.h */
