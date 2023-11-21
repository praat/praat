/* SoundAnalysisArea_prefs.h
 *
 * Copyright (C) 2013,2015-2017,2020,2022,2023 Paul Boersma
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

Prefs_begin (SoundAnalysisArea)

	InstancePrefs_addDouble  (SoundAnalysisArea, longestAnalysis,                 1, U"10.0")   // seconds
	InstancePrefs_addEnum    (SoundAnalysisArea, timeStepStrategy,                1, kSoundAnalysisArea_timeStepStrategy, DEFAULT)
	InstancePrefs_addDouble  (SoundAnalysisArea, fixedTimeStep,                   1, U"0.01")   // seconds
	InstancePrefs_addInteger (SoundAnalysisArea, numberOfTimeStepsPerView,        1, U"100")
	InstancePrefs_addBool    (SoundAnalysisArea, spectrogram_show,                1, true)
	InstancePrefs_addDouble  (SoundAnalysisArea, spectrogram_viewFrom,            2, U"0.0")   // Hz
	InstancePrefs_addDouble  (SoundAnalysisArea, spectrogram_viewTo,              2, U"5000.0")   // Hz
	InstancePrefs_addDouble  (SoundAnalysisArea, spectrogram_windowLength,        2, U"0.005")   // seconds
	InstancePrefs_addDouble  (SoundAnalysisArea, spectrogram_dynamicRange,        2, U"70.0")   // dB
	InstancePrefs_addInteger (SoundAnalysisArea, spectrogram_timeSteps,           2, U"1000")
	InstancePrefs_addInteger (SoundAnalysisArea, spectrogram_frequencySteps,      2, U"250")
	InstancePrefs_addEnum    (SoundAnalysisArea, spectrogram_method,              2, kSound_to_Spectrogram_method, DEFAULT)
	InstancePrefs_addEnum    (SoundAnalysisArea, spectrogram_windowShape,         2, kSound_to_Spectrogram_windowShape, DEFAULT)
	InstancePrefs_addBool    (SoundAnalysisArea, spectrogram_autoscaling,         2, true)
	InstancePrefs_addDouble  (SoundAnalysisArea, spectrogram_maximum,             2, U"100.0")   // dB/Hz
	InstancePrefs_addDouble  (SoundAnalysisArea, spectrogram_preemphasis,         2, U"6.0")   // dB/octave
	InstancePrefs_addDouble  (SoundAnalysisArea, spectrogram_dynamicCompression,  2, U"0.0")   // 0..1
	InstancePrefs_addBool    (SoundAnalysisArea, spectrogram_picture_garnish,     1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, pitch_show,                      1, true)
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_floor,                     2, U"50.0")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_ceiling,                   2, U"800.0")
	InstancePrefs_addEnum    (SoundAnalysisArea, pitch_unit,                      2, kPitch_unit, HERTZ_LOGARITHMIC)
	InstancePrefs_addEnum    (SoundAnalysisArea, pitch_drawingMethod,             1, kSoundAnalysisArea_pitch_drawingMethod, DEFAULT)
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_viewFrom,                  2, U"0.0 (= auto)")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_viewTo,                    2, U"0.0 (= auto)")
	InstancePrefs_addEnum    (SoundAnalysisArea, pitch_method,                    2, kSoundAnalysisArea_pitch_analysisMethod, DEFAULT)
	InstancePrefs_addBool    (SoundAnalysisArea, pitch_rawAcCc_veryAccurate,              1, false)
	InstancePrefs_addInteger (SoundAnalysisArea, pitch_rawAcCc_maximumNumberOfCandidates, 1, U"15")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_rawAcCc_silenceThreshold,          1, U"0.03")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_rawAcCc_voicingThreshold,          1, U"0.45")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_rawAcCc_octaveCost,                1, U"0.01")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_rawAcCc_octaveJumpCost,            1, U"0.35")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_rawAcCc_voicedUnvoicedCost,        1, U"0.14")
	InstancePrefs_addBool    (SoundAnalysisArea, pitch_filteredAcCc_veryAccurate,              1, false)
	InstancePrefs_addInteger (SoundAnalysisArea, pitch_filteredAcCc_maximumNumberOfCandidates, 1, U"15")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_filteredAcCc_attenuationAtCeiling,      1, U"0.03")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_filteredAcCc_silenceThreshold,          1, U"0.09")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_filteredAcCc_voicingThreshold,          1, U"0.50")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_filteredAcCc_octaveCost,                1, U"0.055")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_filteredAcCc_octaveJumpCost,            1, U"0.35")
	InstancePrefs_addDouble  (SoundAnalysisArea, pitch_filteredAcCc_voicedUnvoicedCost,        1, U"0.14")
	InstancePrefs_addBool    (SoundAnalysisArea, pitch_picture_speckle,           1, false)
	InstancePrefs_addBool    (SoundAnalysisArea, pitch_picture_garnish,           1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, intensity_show,                  1, false)
	InstancePrefs_addDouble  (SoundAnalysisArea, intensity_viewFrom,              1, U"50.0")   // dB
	InstancePrefs_addDouble  (SoundAnalysisArea, intensity_viewTo,                1, U"100.0")   // dB
	InstancePrefs_addEnum    (SoundAnalysisArea, intensity_averagingMethod,       1, kSoundAnalysisArea_intensity_averagingMethod, DEFAULT)
	InstancePrefs_addBool    (SoundAnalysisArea, intensity_subtractMeanPressure,  1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, intensity_picture_garnish,       1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, formant_show,                    1, false)
	InstancePrefs_addDouble  (SoundAnalysisArea, formant_ceiling,                 1, U"5500.0")   // Hz
	InstancePrefs_addDouble  (SoundAnalysisArea, formant_numberOfFormants,        1, U"5.0")
	InstancePrefs_addDouble  (SoundAnalysisArea, formant_windowLength,            1, U"0.025")   // seconds
	InstancePrefs_addDouble  (SoundAnalysisArea, formant_dynamicRange,            1, U"30.0")   // dB
	InstancePrefs_addDouble  (SoundAnalysisArea, formant_dotSize,                 1, U"1.0")   // mm
	InstancePrefs_addEnum    (SoundAnalysisArea, formant_method,                  1, kSoundAnalysisArea_formant_analysisMethod, BURG)
	InstancePrefs_addDouble  (SoundAnalysisArea, formant_preemphasisFrom,         1, U"50.0")   // Hz
	InstancePrefs_addBool    (SoundAnalysisArea, formant_picture_garnish,         1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, pulses_show,                     1, false)
	InstancePrefs_addDouble  (SoundAnalysisArea, pulses_maximumPeriodFactor,      1, U"1.3")
	InstancePrefs_addDouble  (SoundAnalysisArea, pulses_maximumAmplitudeFactor,   1, U"1.6")
	InstancePrefs_addBool    (SoundAnalysisArea, pulses_picture_garnish,          1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, log1_toInfoWindow,               1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, log1_toLogFile,                  1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, log2_toInfoWindow,               1, true)
	InstancePrefs_addBool    (SoundAnalysisArea, log2_toLogFile,                  1, true)
	InstancePrefs_addString  (SoundAnalysisArea, log1_fileName,                   1, U"~/Desktop/pitchLog.txt")
	InstancePrefs_addString  (SoundAnalysisArea, log2_fileName,                   1, U"~/Desktop/formantLog.txt")
	InstancePrefs_addString  (SoundAnalysisArea, logScript3,                      1, U"~/Desktop/log3.praat")
	InstancePrefs_addString  (SoundAnalysisArea, logScript4,                      1, U"~/Desktop/log4.praat")
	InstancePrefs_addString  (SoundAnalysisArea, log1_format,                     1, U"Time 'time:6' seconds, pitch 'f0:2' Hz")
	InstancePrefs_addString  (SoundAnalysisArea, log2_format,                     1, U"'t1:4''tab$''t2:4''tab$''f1:0''tab$''f2:0''tab$''f3:0'")

Prefs_end (SoundAnalysisArea)

/* End of file SoundAnalysisArea_prefs.h */
