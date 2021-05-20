/* praat_MultiSampledSpectrogram.cpp
 *
 * Copyright (C) 2021 David Weenink
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

#include "praat_MultiSampledSpectrogram.h"
#include "Sound_and_ConstantQLogFSpectrogram.h"
#include "Spectrum_and_MultiSampledSpectrogram.h"

FORM (MODIFY_EACH_WEAK__MultiSampledSpectrogram_formula, U"MultiSampledSpectrogram: Formula", U"MultiSampledSpectrogram: Formula...") {
	FORMULA (formula, U"Formula:", U"2 * self")
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
	REAL (toFrequency, U"To Frequncy (Hz)", U"200.0")
	FORMULA (formula, U"Formula:", U"2 * self")
	OK
DO
	MODIFY_EACH_WEAK (MultiSampledSpectrogram)
		MultiSampledSpectrogram_formula_part (me, fromTime, toTime, fromFrequency, toFrequency, formula, interpreter);
	MODIFY_EACH_WEAK_END
}

FORM (GRAPHICS_EACH__ConstantQLogFSpectrogram_paint, U"ConstantQLogFSpectrogram: Paint", nullptr) {
	REAL (xmin, U"left Time range (s)", U"0.0")
	REAL (xmax, U"right Time range (s)", U"0.0 (=all)")
	REAL (ymin, U"left Frequency range (Hz)", U"0.0")
	REAL (ymax, U"right Frequency range (Hz)", U"0.0 (=auto)")
	POSITIVE (dBRange, U"Dynamic range (dB)", U"50.0")
	BOOLEAN (garnish, U"Garnish", true);
	OK
DO
	GRAPHICS_EACH (ConstantQLogFSpectrogram)
		ConstantQLogFSpectrogram_paint (me, GRAPHICS, xmin, xmax, ymin, ymax, dBRange, garnish);
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

FORM (CONVERT_EACH_TO_ONE__ConstantQLogFSpectrogram_translateSpectrum, U"", nullptr) {
	REAL (fromTime, U"From time", U"0.0")
	REAL (toTime, U"To time", U"0.0 (= all)")
	REAL (fromFrequency, U"From frequency (Hz)", U"100.0")
	REAL (numberOfBins, U"Number of bins", U"5.0")	
	OK
DO
	CONVERT_EACH_TO_ONE (ConstantQLogFSpectrogram)
		autoConstantQLogFSpectrogram result = ConstantQLogFSpectrogram_translateSpectrum (me, fromTime, toTime, fromFrequency, numberOfBins);
	CONVERT_EACH_TO_ONE_END (my name.get())
}

FORM (CONVERT_EACH_TO_ONE__Sound_to_ConstantQLogFSpectrogram, U"Sound: To ConstantQLogFSpectrogram", U"Sound: To ConstantQLogFSpectrogram...") {
	POSITIVE (f1, U"Lowest frequency (Hz)", U"110.0 (=440*2^(-2))")
	REAL (fmax, U"Maximum frequency (Hz)", U"0.0 (=nyquist)")
	NATURAL (numberOfFrequencyBinsPerOctave, U"Number of frequency bins / octave", U"24")
	POSITIVE (frequencyResolutionInBins, U"Freqency resolution (bins)", U"1.0")
	POSITIVE (timeOversamplingFactor, U"Time oversampling factor", U"1.0")
	OK
DO
	CONVERT_EACH_TO_ONE (Sound)
		autoConstantQLogFSpectrogram result = Sound_to_ConstantQLogFSpectrogram (me, f1, fmax, 
			numberOfFrequencyBinsPerOctave, frequencyResolutionInBins, timeOversamplingFactor
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
}

void praat_MultiSampledSpectrogram_init () {
	Thing_recognizeClassesByName (classAnalyticSound, classConstantQLogFSpectrogram, classFrequencyBin,
		classMultiSampledSpectrogram, nullptr);

	praat_addAction1 (classConstantQLogFSpectrogram, 0, U"Paint...", nullptr, 0, 
			GRAPHICS_EACH__ConstantQLogFSpectrogram_paint);
	praat_MultiSampledSpectrograms_generics (classConstantQLogFSpectrogram);
	praat_addAction1 (classConstantQLogFSpectrogram, 0, U"Translate spectrum...", nullptr, 0,
			CONVERT_EACH_TO_ONE__ConstantQLogFSpectrogram_translateSpectrum);
	
	praat_addAction1 (classSound, 0, U"To ConstantQLogFSpectrogram...", U"To ComplexSpectrogram...", praat_DEPTH_1 + praat_HIDDEN,
			CONVERT_EACH_TO_ONE__Sound_to_ConstantQLogFSpectrogram);
}

/* End of file praat_MultiSampledSpectrogram.cpp */

