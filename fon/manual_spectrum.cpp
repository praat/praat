/* manual_spectrum.cpp
 *
 * Copyright (C) 1992-2008,2010-2012,2014-2017,2019,2020 Paul Boersma
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

#include "ManPagesM.h"

#include "Sound.h"

enum class Decoration {
	None,
	Minimal,
	Fancy,
};

static void draw_SpectrumHann (Graphics g, double f1, double f2, bool stop, Decoration garnish) {
	try {
		const double fmin = ( garnish == Decoration::Fancy ? 300 : 0 );
		const double fmax = ( garnish == Decoration::Fancy ? 1300 : 4000 );
		const double df = ( garnish == Decoration::Fancy ? 1 : 4 );
		autoSound me = Sound_create (1, fmin, fmax, (long) floor ((fmax - fmin) / df) + 1, df, fmin);
		constexpr double w = 100.0;
		const double f1left = f1 - w, f1right = f1 + w, f2left = f2 - w, f2right = f2 + w;
		constexpr double halfpibysmooth = NUMpi / (w + w);
		Graphics_setWindow (g, fmin, fmax, -0.1, 1.1);
		for (integer i = 1; i <= my nx; i ++) {
			const double f = my x1 + (i - 1) * my dx;
			my z [1] [i] = (
				f < f1left ?
					0.0
				: f < f1right ?
					( f1 > 0.0 ? 0.5 - 0.5 * cos (halfpibysmooth * (f - f1left)) : 1.0 )
				: f < f2left ?
					1.0
				: f < f2right ?
					( f2 < fmax ? 0.5 + 0.5 * cos (halfpibysmooth * (f - f2left)) : 1.0 )
				:
					0.0
			);
		}
		if (stop)
			for (integer i = 1; i <= my nx; i ++)
				my z [1] [i] = 1.0 - my z [1] [i];
		if (garnish != Decoration::None) {
			Graphics_drawInnerBox (g);
			Graphics_textLeft (g, true, U"Amplitude filter %H (%f)");
			Graphics_markLeft (g, 0.0, true, true, false, nullptr);
			Graphics_markLeft (g, 1.0, true, true, false, nullptr);
		}
		if (garnish == Decoration::Minimal) {
			Graphics_textBottom (g, true, U"Frequency %f (Hz)");
			Graphics_markBottom (g, 0.0, true, true, false, nullptr);
			Graphics_markBottom (g, 500.0, true, true, false, nullptr);
			Graphics_markBottom (g, 1000.0, true, true, false, nullptr);
			Graphics_markBottom (g, 2000.0, true, true, false, nullptr);
			Graphics_markBottom (g, 4000.0, true, true, false, nullptr);
		}
		if (garnish == Decoration::Fancy) {
			Graphics_textBottom (g, true, U"Frequency %f");
			Graphics_markBottom (g, f1left, false, true, true, U"%f__1_-%w");
			Graphics_markBottom (g, f1, false, true, true, U"%f__1_");
			Graphics_markBottom (g, f1right, false, true, true, U"%f__1_+%w");
			Graphics_markBottom (g, f2left, false, true, true, U"%f__2_-%w");
			Graphics_markBottom (g, f2, false, true, true, U"%f__2_");
			Graphics_markBottom (g, f2right, false, true, true, U"%f__2_+%w");
			Graphics_markRight (g, 1.0, false, true, false, U"0 dB");
			Graphics_markLeft (g, 0.5, true, true, true, nullptr);
			Graphics_markRight (g, 0.5, false, true, false, U"-6 dB");
		}
		Graphics_setColour (g, stop ? Melder_BLUE : Melder_RED);
		Sound_draw (me.get(), g, 0.0, 0.0, -0.1, 1.1, false, U"curve");
		Graphics_setColour (g, Melder_BLACK);
	} catch (MelderError) {
		Melder_clearError ();
	}
}
static void draw_SpectrumPassHann (Graphics g) {
	draw_SpectrumHann (g, 500.0, 1000.0, false, Decoration::Fancy);
}
static void draw_SpectrumPassHann_decompose (Graphics g) {
	draw_SpectrumHann (g, 0.0, 500.0, false, Decoration::Minimal);
	draw_SpectrumHann (g, 500, 1000, false, Decoration::None);
	draw_SpectrumHann (g, 1000, 2000, false, Decoration::None);
	draw_SpectrumHann (g, 2000, 4000, false, Decoration::None);
}
static void draw_SpectrumStopHann (Graphics g) {
	draw_SpectrumHann (g, 500, 1000, true, Decoration::Fancy);
}
static void draw_SpectrumStopHann_decompose (Graphics g) {
	draw_SpectrumHann (g, 500.0, 1000.0, false, Decoration::Minimal);
	draw_SpectrumHann (g, 500.0, 1000.0, true, Decoration::None);
}

void manual_spectrum_init (ManPages me);
void manual_spectrum_init (ManPages me) {

MAN_BEGIN (U"Ltas", U"ppgb", 20070320)
INTRO (U"One of the @@types of objects@ in Praat. "
	"#Ltas is short for Long-Term Average Spectrum.")
NORMAL (U"An object of class Ltas represents the logarithmic @@power spectral density@ as a function of frequency, "
	"expressed in dB/Hz relative to 2·10^^-5^ Pa. ")
ENTRY (U"Inside an Ltas object")
NORMAL (U"With @Inspect, you will see the following attributes:")
TAG (U"%x__%min_")
DEFINITION (U"the bottom of the frequency domain, in hertz. Usually 0.")
TAG (U"%x__%max_")
DEFINITION (U"the top of the frequency domain, in hertz.")
TAG (U"%n__%x_")
DEFINITION (U"the number of frequency bins (≥ 1).")
TAG (U"%dx")
DEFINITION (U"the frequency step, or %%bin width%, in hertz.")
TAG (U"%x__1_")
DEFINITION (U"the frequency associated with the first bin, in hertz. "
	"Usually equals %dx / 2, because the first bin tends to start at 0 hertz.")
TAG (U"%z__1%i_, %i = 1 ... %n__%x_")
DEFINITION (U"the power spectral density, expressed in dB. ")
MAN_END

MAN_BEGIN (U"Ltas: Get bin number from frequency...", U"ppgb", 20140421)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return bin")
NORMAL (U"the band number belonging to the specified frequency, expressed as a real number.")
ENTRY (U"Example")
NORMAL (U"If the Ltas has a bin width of 1000 Hz, and the lowest frequency is 0 Hz, "
	"the bin number associated with a frequency of 1800 Hz is 2.3.")
ENTRY (U"Scripting")
NORMAL (U"You can use this command to put the nearest bin centre into a script variable:")
CODE (U"selectObject: \"Ltas hallo\"")
CODE (U"bin = Get bin number from frequency: 1800")
CODE (U"nearestBin = round (bin)")
NORMAL (U"In this case, the value will not be written into the Info window. To round down or up, use")
CODE (U"leftBin = floor (bin)")
CODE (U"rightBin = ceiling (bin)")
MAN_END

MAN_BEGIN (U"Ltas: Get bin width", U"ppgb", 20041122)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the with of a bin, expressed in hertz.")
MAN_END

MAN_BEGIN (U"Ltas: Get frequency from bin number...", U"ppgb", 20041122)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the frequency (in hertz) associated with a specified bin number.")
ENTRY (U"Setting")
TAG (U"##Bin number")
DEFINITION (U"the bin number whose frequency is sought.")
ENTRY (U"Algorithm")
NORMAL (U"the result is")
FORMULA (U"%f__1_ + (%binNumber - 1) · Δ%f")
NORMAL (U"where %f__1_ is the frequency associated with the centre of the first bin, "
	"and Δ%f is the bin width.")
MAN_END

MAN_BEGIN (U"Ltas: Get frequency of maximum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the frequency (in hertz) associated with the maximum energy density.")
ENTRY (U"Settings")
TAG (U"##From frequency (Hz)")
TAG (U"##To frequency (Hz)")
DEFINITION (U"the selected frequency domain. Values outside this domain are ignored. "
	"If ##To frequency# is not greater than ##From frequency#, "
	"the entire frequency domain of the Ltas object is considered.")
TAG (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is #none because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a #parabolic or #cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN (U"Ltas: Get frequency of minimum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the frequency (in hertz) associated with the minimum energy density.")
ENTRY (U"Settings")
TAG (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire frequency domain of the Ltas is considered.")
TAG (U"%%Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc) of the @@vector peak interpolation@. "
	"The standard is #none because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a #parabolic or #cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN (U"Ltas: Get highest frequency", U"ppgb", 20041122)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the highest frequency, expressed in hertz.")
MAN_END

MAN_BEGIN (U"Ltas: Get lowest frequency", U"ppgb", 20041122)
INTRO (U"A @query to the selected @Ltas object for its lowest frequency.")
ENTRY (U"Return value")
NORMAL (U"the lowest frequency, expressed in Hertz. It is usually 0 Hz.")
MAN_END

MAN_BEGIN (U"Ltas: Get maximum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the maximum value (in dB) within a specified frequency range.")
ENTRY (U"Settings")
TAG (U"##From frequency (Hz))")
TAG (U"##To frequency (Hz)")
DEFINITION (U"the selected frequency domain. Values outside this domain are ignored. "
	"If %%To frequency% is not greater than %%From frequency%, "
	"the entire frequency domain of the Ltas object is considered.")
TAG (U"%%Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is #none because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a #parabolic or #cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN (U"Ltas: Get mean...", U"ppgb", 20041122)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the mean value (in dB) within a specified frequency range.")
ENTRY (U"Settings")
TAG (U"##From frequency (Hz))")
TAG (U"##To frequency (Hz)")
DEFINITION (U"the frequency range. Values outside this range are ignored. "
	"If ##To frequency# is not greater than ##From frequency#, the entire frequency domain of the Ltas is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The mean value between the frequencies %f__1_ and %f__2_ is defined as")
FORMULA (U"1/(%f__2_ - %f__1_)  ∫__%%f%1_^^%%f%2^ %df %x(%f)")
NORMAL (U"where %x(%f) is the LTAS as a function of frequency, expressed in dB. "
	"For our discrete Ltas object, this mean is approximated by")
FORMULA (U"1/%n ∑__%i=%m..%m+%n-1_ %x__%i_")
NORMAL (U"where %n is the number of band centres between %f__1_ and %f__2_.")
MAN_END

MAN_BEGIN (U"Ltas: Get minimum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the minimum value (in dB) within a specified frequency range.")
ENTRY (U"Settings")
TAG (U"##From frequency (Hz))")
TAG (U"##To frequency (Hz)")
DEFINITION (U"the selected frequency domain. Values outside this domain are ignored. "
	"If ##To frequency# is not greater than ##From frequency#, "
	"the entire frequency domain of the Ltas object is considered.")
TAG (U"%%Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is #none because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a #parabolic or #cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN (U"Ltas: Get number of bins", U"ppgb", 20041122)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the total number of frequency bins.")
MAN_END

MAN_BEGIN (U"Ltas: Get standard deviation...", U"ppgb", 19991016)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the standard deviation (in dB) of the LTAS within a specified frequency domain.")
ENTRY (U"Settings")
TAG (U"##From frequency (Hz))")
TAG (U"##To frequency (Hz)")
DEFINITION (U"the frequency window. Values outside this domain are ignored. "
	"If ##To frequency# is not greater than ##From frequency#, the entire frequency domain of the Ltas is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The standard deviation between the frequencies %f__1_ and %f__2_ is defined as")
FORMULA (U"1/(%f__2_ – %f__1_)  ∫__%%f%1_^^%%f%2^ %df (%x(%f) – %μ)^2")
NORMAL (U"where %x(%f) is the LTAS as a function of frequency, and %μ its mean. "
	"For our discrete Ltas object, the standard deviation is approximated by")
FORMULA (U"1/(%n–1) ∑__%i=%m..%m+%n-1_ (%x__%i_ – %μ)^2")
NORMAL (U"where %n is the number of band centres between %f__1_ and %f__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN (U"Ltas: Get value at frequency...", U"ppgb", 20101228)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the value (in dB) at a specified frequency. "
	"If %frequency is outside the bands of the Ltas, the result is 0.")
ENTRY (U"Settings")
TAG (U"##Frequency (Hz)")
DEFINITION (U"the frequency at which the value is to be evaluated.")
TAG (U"%%Interpolation")
DEFINITION (U"the interpolation method, see @@vector value interpolation@. "
	"The standard is #None because binning is usually large.")
MAN_END

MAN_BEGIN (U"Ltas: Get value in bin...", U"ppgb", 20041122)
INTRO (U"A @query to the selected @Ltas object.")
ENTRY (U"Return value")
NORMAL (U"the LTAS value (in dB) in a specified bin. "
	"If the bin number is less than 1 or greater than the number of bin, the result is 0; "
	"otherwise, it is %z [1] [%%bin number%].")
ENTRY (U"Setting")
TAG (U"##Bin number")
DEFINITION (U"the bin whose value is to be looked up.")
MAN_END

MAN_BEGIN (U"Sound: To Spectrogram...", U"ppgb", 20170828)
INTRO (U"A command that creates a @Spectrogram from every selected @Sound object. "
	"It performs a %%short-term spectral analysis%, which means that for a number of time points in the Sound, "
	"Praat computes an approximation of the spectrum at that time. Each such spectrum is called an %%analysis frame%.")
NORMAL (U"For tutorial information, see @@Intro 3. Spectral analysis@.")
ENTRY (U"Settings")
TAG (U"##Window length (s)")
DEFINITION (U"the duration of the analysis window, in seconds. If this is 0.005 seconds, Praat uses for each frame "
	"the part of the sound that lies between 0.0025 seconds before and 0.0025 seconds after the centre of that frame "
	"(for Gaussian windows, Praat actually uses a bit more than that). "
	"The window length determines the %bandwidth of the spectral analysis, i.e. the width of the horizontal line "
	"in the spectrogram of a pure sine wave. "
	"For a Gaussian window, the -3 dB bandwidth is 2*sqrt(6*ln(2))/(%π*%%Window length%), "
	"or 1.2982804 / %%Window length%. "
	"To get a `broad-band' spectrogram (bandwidth 260 Hz), set %%Window length% to 5 milliseconds; "
	"to get a `narrow-band' spectrogram (bandwidth 43 Hz), set it to 30 milliseconds. "
	"The other window shapes give slightly different values.")
TAG (U"##Maximum frequency (Hz)")
DEFINITION (U"the maximum frequency subject to analysis, e.g. 5000 hertz. "
	"If it is higher than the Nyquist frequency of the Sound (which is half its sampling frequency), "
	"some values in the result will be zero (and will be drawn in white by @@Spectrogram: Paint...@).")
TAG (U"##Time step (s)")
DEFINITION (U"the distance between the centres of subsequent frames, e.g. 0.002 seconds. "
	"This determines the number of frames of the resulting Spectrogram. For instance, if the Sound is 1 second long, "
	"and the time step is 2 milliseconds, the Spectrogram will consist of almost 500 frames "
	"(not %exactly 500, because no reliable spectrum can be measured near the beginning and end of the sound). "
	"See below for cases in which the time step of the resulting Spectrogram is different from what you supply here.")
TAG (U"##Frequency step (Hz)")
DEFINITION (U"the frequency resolution, e.g. 20 Hertz. "
	"This determines the number of frequency bands (%bins) of the resulting Spectrogram. For instance, if the %%Maximum frequency% "
	"is 5000 Hz, and the frequency step is 20 Hz, the Spectrogram will consist of 250 frequency bands. "
	"See below for cases in which the frequency step of the resulting Spectrogram is different from what you supply here.")
TAG (U"##Window shape")
DEFINITION (U"determines the shape of the analysis window. "
	"You can choose from: #Gaussian, #Square (none, rectangular), #Hamming (raised sine-squared), "
	"#Bartlett (triangular), #Welch (parabolic), and #Hanning (sine-squared). "
	"The Gaussian window is superior, as it gives no sidelobes in your spectrogram; "
	"it analyzes a factor of 2 slower than the other window shapes, "
	"because the analysis is actually performed on twice as many samples per frame.")
NORMAL (U"For purposes of computation speed, Praat may decide to change the time step and the frequency step. "
	"This is because the time step never needs to be smaller than 1/(8√%π) of the window length, "
	"and the frequency step never needs to be smaller than (√%π)/8 of the inverse of the window length. "
	"For instance, if the window length is 29 ms, the actual time step will be never be less than 29/(8√%π) = 2.045 ms. "
	"And if the window length is 5 ms, the actual frequency step will never be less than (√%π)/8/0.005 = 44.31 Hz.")
ENTRY (U"Tests of the bandwidth")
NORMAL (U"You can check the bandwidth formula with the following procedure:")
CODE (U"! create a 1000-Hz sine wave, windowed by a 0.2-seconds Gaussian window.")
CODE (U"Create Sound from formula: \"gauss\", 1, 0.0, 1.0, 44100, \"sin(2*pi*1000*x) * exp(-3*((x-0.5)/0.1)\\^ 2)\"")
CODE (U"! compute its spectrum and look at its bandwidth")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Draw: 980, 1020, 20, 80, \"yes\"")
CODE (U"Marks bottom every: 1, 2, \"yes\", \"yes\", \"yes\"")
CODE (U"Marks left every: 1, 2, \"yes\", \"yes\", \"yes\"")
CODE (U"! now you should see a peak at 1000 Hz with a 3 dB bandwidth of 7 Hz (20 dB: 17 Hz)")
CODE (U"! more precise: compute the position and width of the peak, and write them to the console")
CODE (U"Formula: ~ if x<980 or x>1020 then 0 else self fi")
CODE (U"To Formant (peaks): 20")
CODE (U"Write to console")
CODE (U"! now you should be able to read that a peak was found at 999.99982 Hz")
CODE (U"! with a bandwidth of 6.497 Hz; the theory above predicted 6.491 Hz")
CODE (U"")
CODE (U"! The same, windowed by a 0.1-seconds Hamming window.")
CODE (U"Create Sound from formula: \"Hamming\", 1, 0.0, 1.0, 44100, ~ if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(0.54+0.46*cos(pi*(x-0.5)/0.1)) fi")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Formula: ~ if x<970 or x>1030 then 0 else self fi")
CODE (U"To Formant (peaks): 20")
CODE (U"Write to console")
CODE (U"! peak at 999.99817 Hz, 3 dB bw 6.518 Hz, 20 dB bw 15 Hz, zero bw 20 Hz, sidelobe -42 dB")
CODE (U"")
CODE (U"! The same, windowed by a 0.1-seconds rectangular window.")
CODE (U"Create Sound from formula: \"rectangular\", 1, 0.0, 1.0, 44100, ~ if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x) fi")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Formula: ~ if x<970 or x>1030 then 0 else self fi")
CODE (U"To Formant (peaks): 20")
CODE (U"Write to console")
CODE (U"! peak at 999.99506 Hz, 3 dB bw 4.440 Hz, 20 dB bw 27 Hz, zero bw 10 Hz, sidelobe -14 dB")
CODE (U"")
CODE (U"! The same, windowed by a 0.1-seconds Hanning window.")
CODE (U"Create Sound from formula: \"Hanning\", 1, 0.0, 1.0, 44100, ~ if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(0.5+0.5*cos(pi*(x-0.5)/0.1)) fi")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Formula: ~ if x<970 or x>1030 then 0 else self fi")
CODE (U"To Formant (peaks): 20")
CODE (U"Write to console")
CODE (U"! peak at 999.99945 Hz, 3 dB bw 7.212 Hz, 20 dB bw 16 Hz, zero bw 20 Hz, sidelobe -31 dB")
CODE (U"")
CODE (U"! The same, windowed by a 0.1-seconds triangular window.")
CODE (U"Create Sound from formula: \"triangular\", 1, 0.0, 1.0, 44100, ~ if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(1-abs((x-0.5)/0.1)) fi")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Formula: ~ if x<970 or x>1030 then 0 else self fi")
CODE (U"To Formant (peaks): 20")
CODE (U"Write to console")
CODE (U"! peak at 999.99933 Hz, 3 dB bw 6.384 Hz, 20 dB bw 15 Hz, zero bw 20 Hz, sidelobe -26 dB")
CODE (U"")
CODE (U"! The same, windowed by a 0.1-seconds parabolic window.")
CODE (U"Create Sound from formula: \"parabolic\", 1, 0.0, 1.0, 44100, ~ if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(1-((x-0.5)/0.1)^2) fi")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Formula: ~ if x<970 or x>1030 then 0 else self fi")
CODE (U"To Formant (peaks): 20")
CODE (U"Write to console")
CODE (U"! peak at 999.99921 Hz, 3 dB bw 5.786 Hz, 20 dB bw 12 Hz, zero bw 15 Hz, sidelobe -21 dB")
MAN_END

MAN_BEGIN (U"Sound: To Ltas (pitch-corrected)...", U"ppgb", 20061203)
INTRO (U"A command available in the #Spectrum menu if you select one or more @Sound objects. "
	"It tries to compute an @Ltas of the spectral envelope of the voiced parts, "
	"correcting away the influence of F0 in a way that does not sacrifice frequency selectivity. "
	"The resulting Ltas is meant to reflect only the resonances (formants) in the vocal tract "
	"and the envelope of the glottal source spectrum.")
NORMAL (U"The analysis method is described in @@Boersma & Kovacic (2006)@.")
MAN_END

MAN_BEGIN (U"Sound: To Spectrum...", U"ppgb", 20041123)
INTRO (U"A command that appears in the #Spectrum menu if you select one or more @Sound objects. "
	"It turns the selected Sound into a @Spectrum by an over-all spectral analysis, a %%Fourier transform%.")
ENTRY (U"Setting")
TAG (U"##Fast")
DEFINITION (U"determines whether zeroes are appended to the sound such that the number of samples is a power of two. "
	"This can appreciably speed up the Fourier transform.")
ENTRY (U"Mathematical procedure")
NORMAL (U"For the Fourier transform, the Praat-defined @@time domain@ of the @Sound is ignored. "
	"Instead, its time domain is considered to run from %t=0 to %t=%T, "
	"where %t=0 is supposed to be aligned with the first sample, "
	"and %T is the total duration of the samples, i.e. %%N%Δ%t, "
	"where %N is the number of samples and Δe%t is the @@sampling period@. "
	"Thus, the last sample lies at %t=%T–Δ%t.")
NORMAL (U"For a sound %x(%t), defined for all times %t in the domain (0, %T), "
	"the complex spectrum %X(%f) for any frequency %f is the forward Fourier transform of %x(%t), with a negative exponent:")
FORMULA (U"%X(%f) = ∫__0_^^%T^ %x(%t) %e^^-2%%πift%^ %dt")
NORMAL (U"If the Sound is expressed in Pascal (Pa), the Spectrum is expressed in Pa·s, or Pa/Hz. "
	"Since a @Spectrum object can only contain a finite number of frequency samples, "
	"it is only computed for frequencies that are multiples of Δ%f = 1/%T. "
	"The number of those frequencies is determined by the number of samples %N of the sound.")
NORMAL (U"If %N is odd, there will be %N frequency samples. For instance, if the sound has 20,457 samples, "
	"the spectrum will be computed at the frequencies -10,228Δ%f, -10,227Δ%f, ..., –Δ%f, 0, +Δ%f, ..., "
	"+10,227Δ%f, +10,228Δ%f. If we suppose that a frequency sample represents a frequency bin with a width of Δ%f, "
	"we see that the frequency samples span adjacent frequency ranges, "
	"e.g. the first sample runs from -10,228.5Δ%f to -10,227.5Δ%f, the second from -10,227.5Δ%f to -10,226.5Δ%f. "
	"Together, the frequency samples span the frequency domain of the spectrum, "
	"which runs from -%F to +%F, where %F = 10,228.5Δ%f. "
	"We can see that this frequency equals one half of the @@sampling frequency@ of the original sound: "
	"%F = 10,228.5Δ%f = 10,228.5/%T = 10,228.5/(20,457Δ%t) = 0.5/Δ%t. This is the so-called @@Nyquist frequency@.")
NORMAL (U"If %N is even, there will be %N+1 frequency samples. For instance, if the sound has 32,768 samples, "
	"the spectrum will be computed at the frequencies -16,384Δ%f, -16,383Δ%f, ..., -Δ%f, 0, +Δ%f, ..., "
	"+16,383Δ%f, +16,384Δ%f. Again, the frequency samples span adjacent frequency ranges, "
	"but the first and last samples are only half as wide as the rest, "
	"i.e. the first sample runs from -16,384Δ%f to -16,383.5Δ%f, the second from -16,383.5Δ%f to -16,382.5Δ%f, "
	"and the last from +16,383.5Δ%f to +16,384Δ%f. "
	"Together, the frequency samples again span the frequency domain of the spectrum, "
	"which runs from –%F to +%F, where %F = 16,384Δ%f = 0.5/Δ%t, the Nyquist frequency.")
ENTRY (U"Storage")
NORMAL (U"In a @Spectrum object, Praat stores the real and imaginary parts of the complex spectrum separately. "
	"The real part is equal to the cosine transform:")
FORMULA (U"re %X(%f) = ∫__0_^^%T^ %x(%t) cos (2%%πft%) %dt")
NORMAL (U"The imaginary part is equal to the reverse of the sine transform:")
FORMULA (U"im %X(%f) = – ∫__0_^^%T^ %x(%t) sin (2%%πft%) %dt")
NORMAL (U"The complex spectrum can be reconstructed from the real and imaginary part as follows:")
FORMULA (U"%X(%f) = re %X(%f) + %i im %X(%f)")
NORMAL (U"Since the cosine is a symmetric function of %t and the sine is an antisymmetric function of %t, "
	"the complex spectrum for a negative frequency is the complex conjugate of the complex spectrum for the corresponding "
	"positive frequency:")
FORMULA (U"%X(-%f) = re %X(-%f) + %i im %X(-%f) = re %X(%f) - %i im %X(%f) = %X^*(%f)")
NORMAL (U"For purposes of storage, therefore, the negative frequencies are superfluous. "
	"For this reason, the Spectrum object stores re %X(%f) and im %X(%f) only for frequencies %f = 0, Δ%f, 2Δ%f... "
	"In the case of a sound with 20,457 samples, the Spectrum object contains the real part of %X(0) "
	"(its imaginary part is always zero), and the real and imaginary parts of %X(%f) "
	"for frequencies from Δ%f to 10,228Δ%f, which makes in total 1+2·10,228 = 20,457 real values. "
	"In the case of a sound with 32,768 samples, the Spectrum object contains the real parts of %X(0) and %X(16,384Δ%f) "
	"(their imaginary parts are always zero), and the real and imaginary parts of %X(%f) "
	"for frequencies from Δ%f to 16,383Δ%f, which makes in total 2+2·16,383 = 32,768 real values.")
NORMAL (U"Since the negative frequencies have been removed, the frequency domain now runs from 0 to %F. "
	"This means that the first frequency bin is now only 0.5Δ%f wide (i.e. as wide as the last bin for even-%N spectra), "
	"which has consequences for computations of energies.")
ENTRY (U"Behaviour")
NORMAL (U"If you perform @@Spectrum: To Sound@ on the resulting Spectrum object, "
	"a Sound is created that is equal to the original Sound (or to the original Sound with appended zeroes).")
ENTRY (U"Properties")
NORMAL (U"The frequency integral over the squared Spectrum equals the time integral over the squared Sound:")
FORMULA (U"∫__-%F_^^+%F^ |%X(%f)|^2 %df = ∫__0_^%T |%x(%t)|^2 %dt")
NORMAL (U"This is called %%Parceval's theorem%.")
/*
Copy... square
Formula... self^2
To Spectrum... yes
Formula... self * exp (- (x/50) ^ 2)
To Sound
Rename... smoothIntensity

the result is a smoothed version of your squared sound.
Perhaps divide the original by its square root:

select Sound square
Formula... self / sqrt (Sound_smoothIntensity [])
Play

> I also have a question about whether this would remove the contour or
> just normalize the loudness?

it removes the contour. I tested it. It works.
*/
MAN_END

MAN_BEGIN (U"Spectrogram", U"ppgb", 20070321)
INTRO (U"One of the @@types of objects@ in Praat. For tutorial information, see @@Intro 3. Spectral analysis@.")
NORMAL (U"A Spectrogram object represents  an acoustic time-frequency representation of a sound: "
	"the @@power spectral density@ %PSD (%f, %t), expressed in Pa^2/Hz. "
	"It is sampled into a number of points centred around equally spaced times %t__%i_ "
	"and frequencies %f__%j_.")
ENTRY (U"Inside a Spectrogram")
NORMAL (U"With @Inspect, you will see the following attributes:")
TAG (U"%xmin")
DEFINITION (U"start time, in seconds.")
TAG (U"%xmax")
DEFINITION (U"end time, in seconds.")
TAG (U"%nx")
DEFINITION (U"the number of times (≥ 1).")
TAG (U"%dx")
DEFINITION (U"time step, in seconds.")
TAG (U"%x1")
DEFINITION (U"the time associated with the first column, in seconds. "
	"This will usually be in the range [%xmin, %xmax]. "
	"The time associated with the last column (i.e., %x1 + (%nx – 1) %dx)) "
	"will also usually be in that range.")
TAG (U"%ymin")
DEFINITION (U"lowest frequency, in Hertz. Normally 0.")
TAG (U"%ymax")
DEFINITION (U"highest frequency, in Hertz.")
TAG (U"%ny")
DEFINITION (U"the number of frequencies (≥ 1).")
TAG (U"%dy")
DEFINITION (U"frequency step, in Hertz.")
TAG (U"%y1")
DEFINITION (U"the frequency associated with the first row, in Hertz. Usually %dy / 2. "
	"The frequency associated with the last row (i.e., %y1 + (%ny – 1) %dy)) "
	"will often be %ymax - %dy / 2.")
TAG (U"%z__%ij_, %i = 1 ... %ny, %j = 1 ... %nx")
DEFINITION (U"the power spectral density, in Pa^2/Hz. ")
MAN_END

MAN_BEGIN (U"Spectrogram: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Spectrogram objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Spectrogram: Paint...", U"ppgb", 20030916)
INTRO (U"A command to draw the selected @Spectrogram object(s) into the @@Picture window@ in shades of grey.")
ENTRY (U"Settings")
TAG (U"##From time (s)")
TAG (U"##To time (s)")
DEFINITION (U"the time domain along the %x axis.")
TAG (U"##From frequency (Hz)")
TAG (U"##To frequency (Hz)")
DEFINITION (U"the frequency domain along the %y axis.")
TAG (U"##Dynamic range (dB)")
DEFINITION (U"The global maximum of the spectrogram (after preemphasis) will always be drawn in black; "
	"all values that are more than %%Dynamic range% dB below this maximum (after dynamic compression) "
	"will be drawn in white. Values in-between have appropriate shades of grey.")
TAG (U"##Preemphasis (dB/octave)")
DEFINITION (U"determines the steepness of a high-pass filter, "
	"i.e., how much the power of higher frequencies will be raised before drawing, as compared to lower frequencies. "
	"Since the spectral slope of human vowels is approximately -6 dB per octave, "
	"the standard value for this parameter is +6 dB per octave, "
	"so that the spectrum is flattened and the higher formants look as strong as the lower ones.")
TAG (U"##Dynamic compression")
DEFINITION (U"determines how much stronger weak time frames should be made before drawing. "
	"Normally, this parameter is between 0 and 1. If it is 0, there is no dynamic compression. "
	"If it is 1, all time frames (vertical bands) will be drawn equally strong, "
	"i.e., all of them will contain frequencies that are drawn in black. "
	"If this parameter is 0.4 and the global maximum is at 80 dB, then a frame with a maximum at 20 dB "
	"(which will normally be drawn all white if the dynamic range is 50 dB), "
	"will be raised by 0.4 * (80 - 20) = 24 dB, "
	"so that its maximum will be seen at 44 dB (thus making this frame visible).")
MAN_END

MAN_BEGIN (U"Spectrogram: To Spectrum (slice)...", U"ppgb", 19961003)
INTRO (U"A command to create a @Spectrum object from every selected @Spectrogram object.")
ENTRY (U"Purpose")
NORMAL (U"to extract the information contained in a Spectrogram at a certain time.")
ENTRY (U"Algorithm")
NORMAL (U"The Spectrum will be constructed from one frame of the Spectrogram, "
	"namely the frame whose centre is closed to the %time argument. ")
MAN_END

MAN_BEGIN (U"Spectrum", U"ppgb", 20041123)
INTRO (U"One of the @@types of objects@ in Praat. A Spectrum object represents "
	"the complex spectrum as a function of frequency. "
	"If the spectrum was created from a sound (which is expressed in units of Pascal), "
	"the complex values are expressed in units Pa/Hz (Pascal per Hertz). "
	"For detailed information, see @@Sound: To Spectrum...@.")
ENTRY (U"Spectrum commands")
NORMAL (U"Creation:")
LIST_ITEM (U"• @@Sound: To Spectrum...")
NORMAL (U"Queries:")
LIST_ITEM (U"• @@Spectrum: Get centre of gravity...")
LIST_ITEM (U"• @@Spectrum: Get standard deviation...")
LIST_ITEM (U"• @@Spectrum: Get skewness...")
LIST_ITEM (U"• @@Spectrum: Get kurtosis...")
LIST_ITEM (U"• @@Spectrum: Get central moment...")
NORMAL (U"Modification:")
LIST_ITEM (U"• @@Spectrum: Filter (pass Hann band)...")
LIST_ITEM (U"• @@Spectrum: Filter (stop Hann band)...")
LIST_ITEM (U"• @@Formula...")
NORMAL (U"Conversion:")
LIST_ITEM (U"• @@Spectrum: To Ltas (1-to-1)")
LIST_ITEM (U"• @@Spectrum: To Spectrogram")
NORMAL (U"Synthesis:")
LIST_ITEM (U"• @@Spectrum: To Sound")
MAN_END

MAN_BEGIN (U"Spectrum: Filter (pass Hann band)...", U"ppgb", 20030916)
INTRO (U"A command to modify every selected @Spectrum object.")
NORMAL (U"The complex values in the #Spectrum are multiplied by real-valued sine shapes and straight lines, according to the following figure:")
PICTURE (5, 3, draw_SpectrumPassHann)
ENTRY (U"Settings")
TAG (U"##From frequency (Hz)# (standard value: 500 Hz)")
DEFINITION (U"the lower edge of the pass band (%f__1_ in the figure). The value zero is special: the filter then acts as a low-pass filter.")
TAG (U"##To frequency (Hz)# (standard value: 1000 Hz)")
DEFINITION (U"the upper edge of the pass band (%f__2_ in the figure). The value zero is special: the filter then acts as a high-pass filter.")
TAG (U"##Smoothing (Hz)# (standard value: 100 Hz)")
DEFINITION (U"the width of the region between pass and stop (%w in the figure).")
ENTRY (U"Usage")
NORMAL (U"Because of its symmetric Hann-like shape, the filter is especially useful for decomposing the Spectrum into consecutive bands. "
	"For instance, we can decompose the spectrum into the bands 0-500 Hz, 500-1000 Hz, 1000-2000 Hz, and 2000-\"0\" Hz:")
PICTURE (5, 3, draw_SpectrumPassHann_decompose)
NORMAL (U"By adding the four bands together, we get the original spectrum again.")
NORMAL (U"A complementary filter is described at @@Spectrum: Filter (stop Hann band)...@.")
NORMAL (U"See the @Filtering tutorial for information on the need for smoothing and a comparative discussion of various filters.")
MAN_END

MAN_BEGIN (U"Spectrum: Filter (stop Hann band)...", U"ppgb", 20030916)
INTRO (U"A command to modify every selected @Spectrum object.")
NORMAL (U"The complex values in the #Spectrum are multiplied by real-valued sine shapes and straight lines, according to the following figure:")
PICTURE (5, 3, draw_SpectrumStopHann)
ENTRY (U"Settings")
TAG (U"##From frequency (Hz)# (standard value: 500 Hz)")
DEFINITION (U"the lower edge of the stop band (%f__1_ in the figure). The value zero is special: the filter then acts as a high-pass filter.")
TAG (U"##To frequency (Hz)# (standard value: 1000 Hz)")
DEFINITION (U"the upper edge of the stop band (%f__2_ in the figure). The value zero is special: the filter then acts as a low-pass filter.")
TAG (U"##Smoothing (Hz)# (standard value: 100 Hz)")
DEFINITION (U"the width of the region between stop and pass (%w in the figure).")
ENTRY (U"Usage")
NORMAL (U"This filter is the complement from the pass-band filter (@@Spectrum: Filter (pass Hann band)...@). "
	"For instance, we can decompose the spectrum into the above stop-band spectrum and a band from 500 to 1000 Hz:")
PICTURE (5, 3, draw_SpectrumStopHann_decompose)
NORMAL (U"By adding the two spectra together, we get the original spectrum again.")
NORMAL (U"See the @Filtering tutorial for information on the need for smoothing and a comparative discussion of various filters.")
MAN_END

MAN_BEGIN (U"Spectrum: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Spectrum objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Spectrum: Get central moment...", U"ppgb", 20020323)
INTRO (U"A command to query the selected @Spectrum object.")
NORMAL (U"If the complex spectrum is given by %S(%f), the %%n%th central spectral moment is given by")
FORMULA (U"∫__0_^∞  (%f – %f__%c_)^%n |%S(%f)|^%p %df")
NORMAL (U"divided by the \"energy\"")
FORMULA (U"∫__0_^∞  |%S(%f)|^%p %df")
NORMAL (U"In this formula, %f__%c_ is the spectral centre of gravity (see @@Spectrum: Get centre of gravity...@). "
	"Thus, the %%n%th central moment is the average of (%f – %f__%c_)^%n over the entire frequency domain, "
	"weighted by |%S(%f)|^%p. For %p = 2, the weighting is done by the power spectrum, and for %p = 1, "
	"the weighting is done by the absolute spectrum. A value of %p = 2/3 has been seen as well.")
ENTRY (U"Settings")
TAG (U"##Moment")
DEFINITION (U"the number %n in the formulas above. A number of 3 gives you the third central spectral moment. "
	"It is not impossible to ask for fractional moments, e.g. %n = 1.5.")
TAG (U"##Power")
DEFINITION (U"the quantity %p in the formula above. Common values are 2, 1, or 2/3.")
ENTRY (U"Usage")
NORMAL (U"For %n = 1, the central moment should be zero, since the centre of gravity %f__%c_ is computed with "
	"the same %p. For %n = 2, you get the variance of the frequencies in the spectrum; the standard deviation "
	"of the frequency is the square root of this. For %n = 3, you get the non-normalized spectral skewness; "
	"to normalize it, you can divide by the 1.5 power of the second moment. For %n = 4, you get the "
	"non-normalized spectral kurtosis; to normalize it, you can divide by the square of the second moment "
	"and subtract 3. Praat can directly give you the quantities mentioned here:")
LIST_ITEM (U"• @@Spectrum: Get centre of gravity...")
LIST_ITEM (U"• @@Spectrum: Get standard deviation...")
LIST_ITEM (U"• @@Spectrum: Get skewness...")
LIST_ITEM (U"• @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN (U"Spectrum: Get centre of gravity...", U"ppgb", 20070225)
INTRO (U"A command to query the selected @Spectrum object.")
NORMAL (U"If the complex spectrum is given by %S(%f), where %f is the frequency, the %%centre of gravity% "
	"is given by")
FORMULA (U"∫__0_^∞  %f |%S(%f)|^%p %df")
NORMAL (U"divided by the \"energy\"")
FORMULA (U"∫__0_^∞  |%S(%f)|^%p %df")
NORMAL (U"Thus, the centre of gravity is the average of %f over the entire frequency domain, "
	"weighted by |%S(%f)|^%p. For %p = 2, the weighting is done by the power spectrum, and for %p = 1, "
	"the weighting is done by the absolute spectrum. A value of %p = 2/3 has been seen as well.")
ENTRY (U"Setting")
TAG (U"##Power")
DEFINITION (U"the quantity %p in the formulas above. Common values are 2, 1, or 2/3.")
ENTRY (U"Interpretation")
NORMAL (U"The spectral centre of gravity is a measure for how high the frequencies in a spectrum are on average. "
	"For a sine wave with a frequency of 377 Hz, the centre of gravity is 377 Hz. You can easily check this "
	"in Praat by creating such a sine wave (@@Create Sound from formula...@), then converting it to a Spectrum "
	"(@@Sound: To Spectrum...@), then querying the mean frequency. For a white noise sampled at 22050 Hz, "
	"the centre of gravity is 5512.5 Hz, i.e. one half of the @@Nyquist frequency@.")
ENTRY (U"Related measures")
NORMAL (U"The centre of gravity is used in the computation of spectral moments:")
LIST_ITEM (U"• @@Spectrum: Get central moment...")
LIST_ITEM (U"• @@Spectrum: Get standard deviation...")
LIST_ITEM (U"• @@Spectrum: Get skewness...")
LIST_ITEM (U"• @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN (U"Spectrum: Get kurtosis...", U"ppgb", 20020323)
INTRO (U"A command to query the selected @Spectrum object.")
NORMAL (U"The (normalized) kurtosis of a spectrum is the fourth central moment of this spectrum, "
	"divided by the square of the second central moment, minus 3. "
	"See @@Spectrum: Get central moment...@.")
ENTRY (U"Setting")
TAG (U"##Power")
DEFINITION (U"the quantity %p in the formula for the centre of gravity and the second and fourth central moment. "
	"Common values are 2, 1, or 2/3.")
ENTRY (U"Interpretation")
NORMAL (U"The kurtosis is a measure for how much the shape of the spectrum around the "
	"%%centre of gravity% is different from a Gaussian shape. "
	"For a white noise, the kurtosis is -6/5.")
ENTRY (U"Related measures")
LIST_ITEM (U"• @@Spectrum: Get centre of gravity...")
LIST_ITEM (U"• @@Spectrum: Get central moment...")
LIST_ITEM (U"• @@Spectrum: Get standard deviation...")
LIST_ITEM (U"• @@Spectrum: Get skewness...")
MAN_END

MAN_BEGIN (U"Spectrum: Get skewness...", U"ppgb", 20020323)
INTRO (U"A command to query the selected @Spectrum object.")
NORMAL (U"The (normalized) skewness of a spectrum is the third central moment of this spectrum, "
	"divided by the 1.5 power of the second central moment. "
	"See @@Spectrum: Get central moment...@.")
ENTRY (U"Setting")
TAG (U"##Power")
DEFINITION (U"the quantity %p in the formula for the centre of gravity and the second and third central moment. "
	"Common values are 2, 1, or 2/3.")
ENTRY (U"Interpretation")
NORMAL (U"The skewness is a measure for how much the shape of the spectrum below the "
	"%%centre of gravity% is different from the shape above the mean frequency. "
	"For a white noise, the skewness is zero.")
ENTRY (U"Related measures")
LIST_ITEM (U"• @@Spectrum: Get centre of gravity...")
LIST_ITEM (U"• @@Spectrum: Get central moment...")
LIST_ITEM (U"• @@Spectrum: Get standard deviation...")
LIST_ITEM (U"• @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN (U"Spectrum: Get standard deviation...", U"ppgb", 20020323)
INTRO (U"A command to query the selected @Spectrum object.")
NORMAL (U"The standard deviation of a spectrum is the square root of the second central moment of this spectrum. "
	"See @@Spectrum: Get central moment...@.")
ENTRY (U"Setting")
TAG (U"##Power")
DEFINITION (U"the quantity %p in the formula for the centre of gravity and the second central moment. "
	"Common values are 2, 1, or 2/3.")
ENTRY (U"Interpretation")
NORMAL (U"The standard deviation is a measure for how much the frequencies in a spectrum can deviate from "
	"the %%centre of gravity%. "
	"For a sine wave, the standard deviation is zero. For a white noise, the standard deviation "
	"is the @@Nyquist frequency@ divided by √12.")
ENTRY (U"Related measures")
LIST_ITEM (U"• @@Spectrum: Get centre of gravity...")
LIST_ITEM (U"• @@Spectrum: Get central moment...")
LIST_ITEM (U"• @@Spectrum: Get skewness...")
LIST_ITEM (U"• @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN (U"Spectrum: To Ltas (1-to-1)", U"ppgb", 20141001)
INTRO (U"A command for converting each selected @Spectrum object into an @Ltas object without loss of frequency resolution.")
ENTRY (U"Algorithm")
NORMAL (U"Each band %b__%i_ in the Ltas is computed from a single frequency sample %s__%i_ in the Spectrum as follows:")
FORMULA (U"%b__%i_ = 2 ((re (%s__%i_))^2 + (im (%s__%i_))^2) / 4.0·10^^-10^")
NORMAL (U"If the original Spectrum is expressible in Pa / Hz (sound pressure in air), the Ltas values "
	"are in \"dB/Hz\" relative to the auditory threshold at 1000 Hz (2·10^^-5^ Pa).")
MAN_END

MAN_BEGIN (U"Spectrum: To Sound", U"ppgb", 200411123)
INTRO (U"A command for creating a @Sound object from every selected @Spectrum object.")
ENTRY (U"Mathematical procedure")
NORMAL (U"The reverse of the Fourier transform described in @@Sound: To Spectrum...@. "
	"If the Spectrum is expressed in Pa/Hz, the Sound will be in Pascal. "
	"The frequency integral over the Sound equals the time integral over the Spectrum.")
ENTRY (U"Behaviour")
NORMAL (U"If you perform this command on a Spectrum object that was created earlier with @@Sound: To Spectrum...@, "
	"the resulting Sound is equal to the Sound that was input to ##Sound: To Spectrum...#.")
MAN_END

MAN_BEGIN (U"Spectrum: To Spectrogram", U"ppgb", 19961003)
INTRO (U"A command to create a @Spectrogram object from every selected @Spectrum object.")
ENTRY (U"Purpose")
NORMAL (U"Format conversion.")
ENTRY (U"Behaviour")
NORMAL (U"The Spectrogram will have only one frame (time slice).")
ENTRY (U"Algorithm")
NORMAL (U"The values are computed as the sum of the squares of the real and imaginary parts of the Spectrum.")
MAN_END

MAN_BEGIN (U"SpectrumEditor", U"ppgb", 20030316)
INTRO (U"One of the @editors in Praat. It allows you to view, "
	"zoom, and play a @Spectrum object.")
NORMAL (U"Clicking on one of the (maximally) 8 rectangles above or below the drawing area "
	"lets you play a @Sound that is synthesized from a band-filtered part of the @Spectrum. "
	"You can also copy the Spectrum, band-filtered with the @@frequency selection@, "
	"or the Sound synthesized from this Spectrum, to the list of objects.")
MAN_END

}

/* End of file manual_spectrum.cpp */
