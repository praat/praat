/* praat_MultiSampledSpectrogram.cpp
 *
 * Copyright (C) 2021-2023 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "praatM.h"

#include "AnalyticSound.h"
#include "ConstantQSpectrograms.h"
#include "MultiSampledSpectrogram.h"
#include "Sound_and_MultiSampledSpectrogram.h"
#include "Spectrum_and_MultiSampledSpectrogram.h"

DIRECT (CONVERT_EACH_TO_ONE__AnalyticSound_toIntensity) {
	CONVERT_EACH_TO_ONE (AnalyticSound)
		autoIntensity result = AnalyticSound_to_Intensity (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__AnalyticSound_toSound) {
	CONVERT_EACH_TO_ONE (AnalyticSound)
		autoSound result = AnalyticSound_to_Sound (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (MODIFY_EACH_WEAK__MultiSampledSpectrogram_formula, U"MultiSampledSpectrogram: Formula", U"MultiSampledSpectrogram: Formula...") {
	FORMULA (formula, U"Formula", U"2 * self")
	OK
DO
	MODIFY_EACH_WEAK (MultiSampledSpectrogram)
		MultiSampledSpectrogram_formula (me, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (MODIFY_EACH_WEAK__MultiSampledSpectrogram_formula_part, U"MultiSampledSpectrogram: Formula (part)", U"MultiSampledSpectrogram: Formula...") {
	REAL (fromTime, U"From time", U"0.0")
	REAL (toTime, U"To time", U"0.0 (= all)")
	REAL (fromFrequency, U"From frequency (Hz)", U"100.0")
	REAL (toFrequency, U"To Frequency (Hz)", U"200.0")
	FORMULA (formula, U"Formula", U"2 * self")
	OK
DO
	MODIFY_EACH_WEAK (MultiSampledSpectrogram)
		MultiSampledSpectrogram_formula_part (me, fromTime, toTime, fromFrequency, toFrequency, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (GRAPHICS_EACH__ConstantQLog2FSpectrogram_paint, U"ConstantQLog2FSpectrogram: Paint", nullptr) {
	REAL (xmin, U"left Time range (s)", U"0.0")
	REAL (xmax, U"right Time range (s)", U"0.0 (= all)")
	REAL (ymin, U"left Frequency range (Hz)", U"0.0")
	REAL (ymax, U"right Frequency range (Hz)", U"0.0 (= auto)")
	POSITIVE (dBRange, U"Dynamic range (dB)", U"50.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (ConstantQLog2FSpectrogram)
		ConstantQLog2FSpectrogram_paint (me, GRAPHICS, xmin, xmax, ymin, ymax, dBRange, garnish);
	GRAPHICS_EACH_END
}

DIRECT (CONVERT_EACH_TO_ONE__MultiSampledSpectrogram_to_Sound) {
	CONVERT_EACH_TO_ONE (MultiSampledSpectrogram)
		autoSound result = MultiSampledSpectrogram_to_Sound (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__MultiSampledSpectrogram_to_Sound_frequencyBin, U"MultiSampledSpectrogram: To Sound (frequencyBin)", nullptr) {
	NATURAL (frequencyBinNumber, U"Frequency bin number", U"1")
	OK
DO
	CONVERT_EACH_TO_ONE (MultiSampledSpectrogram)
		autoSound result = MultiSampledSpectrogram_to_Sound_frequencyBin (me, frequencyBinNumber);
	CONVERT_EACH_TO_ONE_END (my name.get(), U"_",frequencyBinNumber)
}

DIRECT (CONVERT_EACH_TO_ONE__MultiSampledSpectrogram_to_Spectrum) {
	CONVERT_EACH_TO_ONE (MultiSampledSpectrogram)
		autoSpectrum result = MultiSampledSpectrogram_to_Spectrum (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__ConstantQLog2FSpectrogram_translateSpectrum, U"", nullptr) {
	REAL (fromTime, U"From time", U"0.0")
	REAL (toTime, U"To time", U"0.0 (= all)")
	REAL (fromFrequency, U"From frequency (Hz)", U"100.0")
	REAL (numberOfBins, U"Number of bins", U"5.0")	
	OK
DO
	CONVERT_EACH_TO_ONE (ConstantQLog2FSpectrogram)
		autoConstantQLog2FSpectrogram result = ConstantQLog2FSpectrogram_translateSpectrum (me, fromTime, toTime, fromFrequency, numberOfBins);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_ConstantQLog2FSpectrogram, U"Sound: To ConstantQLog2FSpectrogram", U"Sound: To ConstantQLog2FSpectrogram...") {
	POSITIVE (f1, U"Lowest frequency (Hz)", U"110.0 (=440*2^(-2))")
	REAL (fmax, U"Maximum frequency (Hz)", U"0.0 (= Nyquist)")
	NATURAL (numberOfFrequencyBinsPerOctave, U"Number of frequency bins / octave", U"24")
	POSITIVE (frequencyResolutionInBins, U"Frequency resolution (bins)", U"1.0")
	POSITIVE (timeOversamplingFactor, U"Time oversampling factor", U"1.0")
	CHOICE_ENUM (kSound_windowShape, filterShape,
			U"Filter shape", kSound_windowShape::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoConstantQLog2FSpectrogram result = Sound_to_ConstantQLog2FSpectrogram (me, f1, fmax, 
			numberOfFrequencyBinsPerOctave, frequencyResolutionInBins, timeOversamplingFactor, filterShape
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

DIRECT (CONVERT_EACH_TO_ONE__Sound_to_AnalyticSound) {
	CONVERT_EACH_TO_ONE (Sound)
		autoAnalyticSound result = Sound_to_AnalyticSound (me);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (GRAPHICS_EACH__GaborSpectrogram_paint, U"GaborSpectrogram: Paint", nullptr) {
	REAL (xmin, U"left Time range (s)", U"0.0")
	REAL (xmax, U"right Time range (s)", U"0.0 (= all)")
	REAL (ymin, U"left Frequency range (Hz)", U"0.0")
	REAL (ymax, U"right Frequency range (Hz)", U"0.0 (= auto)")
	POSITIVE (dBRange, U"Dynamic range (dB)", U"50.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (GaborSpectrogram)
		GaborSpectrogram_paint (me, GRAPHICS, xmin, xmax, ymin, ymax, dBRange, garnish);
	GRAPHICS_EACH_END
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_GaborSpectrogram, U"Sound: To GaborSpectrogram", U"Sound: To ConstantQLog2FSpectrogram...") {
	REAL (fmax, U"Maximum frequency (Hz)", U"0.0 (= Nyquist)")
	POSITIVE (filterBandwidth, U"Filter bandwidth (Hz)", U"40.0")
	POSITIVE (frequencyStep, U"Frequency step (hz)", U"20.0")
	POSITIVE (timeOversamplingFactor, U"Time oversampling factor", U"1.0")
	CHOICE_ENUM (kSound_windowShape, filterShape,
			U"Filter shape", kSound_windowShape::DEFAULT)
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoGaborSpectrogram result = Sound_to_GaborSpectrogram (me, fmax, filterBandwidth,
			frequencyStep, timeOversamplingFactor, filterShape
		);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

void praat_MultiSampledSpectrograms_generics (ClassInfo klas) {
	praat_addAction1 (klas, 0, U"Formula...", nullptr, 0,
			MODIFY_EACH_WEAK__MultiSampledSpectrogram_formula);
	praat_addAction1 (klas, 0, U"Formula (part)...", nullptr, 0, 
			MODIFY_EACH_WEAK__MultiSampledSpectrogram_formula_part);
	praat_addAction1 (klas, 0, U"To Sound", nullptr, 0,
			CONVERT_EACH_TO_ONE__MultiSampledSpectrogram_to_Sound);
	praat_addAction1 (klas, 0, U"To Sound (frequencyBin)...", nullptr, 0,
			CONVERT_EACH_TO_ONE__MultiSampledSpectrogram_to_Sound_frequencyBin);
	praat_addAction1 (klas, 0, U"To Spectrum", nullptr, 0,
			CONVERT_EACH_TO_ONE__MultiSampledSpectrogram_to_Spectrum);
}

void praat_MultiSampledSpectrogram_init ();
void praat_MultiSampledSpectrogram_init () {
	Thing_recognizeClassesByName (classAnalyticSound, classConstantQLog2FSpectrogram, classFrequencyBin,
		classGaborSpectrogram, classMultiSampledSpectrogram, nullptr);

	praat_addAction1 (classAnalyticSound, 0, U"To Intensity", nullptr, 0, 
			CONVERT_EACH_TO_ONE__AnalyticSound_toIntensity);
	praat_addAction1 (classAnalyticSound, 0, U"To Sound", nullptr, 0, 
			CONVERT_EACH_TO_ONE__AnalyticSound_toSound);
	
	praat_addAction1 (classConstantQLog2FSpectrogram, 0, U"Paint...", nullptr, 0, 
			GRAPHICS_EACH__ConstantQLog2FSpectrogram_paint);
	praat_MultiSampledSpectrograms_generics (classConstantQLog2FSpectrogram);
	praat_addAction1 (classConstantQLog2FSpectrogram, 0, U"Translate spectrum...", nullptr, 0,
			CONVERT_EACH_TO_ONE__ConstantQLog2FSpectrogram_translateSpectrum);
	
	praat_addAction1 (classSound, 0, U"To ConstantQLog2FSpectrogram...", U"To ComplexSpectrogram...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_ConstantQLog2FSpectrogram);
	praat_addAction1 (classSound, 0, U"To AnalyticSound", U"Resample...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_AnalyticSound);
	
	
	praat_addAction1 (classGaborSpectrogram, 0, U"Paint...", nullptr, 0, 
			GRAPHICS_EACH__GaborSpectrogram_paint);
	praat_MultiSampledSpectrograms_generics (classGaborSpectrogram);
	praat_addAction1 (classSound, 0, U"To GaborSpectrogram...", U"To ComplexSpectrogram...", GuiMenu_DEPTH_1 | GuiMenu_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_GaborSpectrogram);
}

/* End of file praat_MultiSampledSpectrogram.cpp */

