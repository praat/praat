/* manual_spectrum.c
 *
 * Copyright (C) 1992-2004 Paul Boersma
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

#include "ManPagesM.h"

#include "Sound.h"

static void draw_SpectrumHann (Graphics g, double f1, double f2, int stop, int garnish) {
	double fmin = garnish == 1 ? 300 : 0, fmax = garnish == 1 ? 1300 : 4000, df = garnish == 1 ? 1 : 4;
	Sound me = Sound_create (fmin, fmax, (fmax - fmin) / df + 1, df, fmin);
	double w = 100, f1left = f1 - w, f1right = f1 + w, f2left = f2 - w, f2right = f2 + w, halfpibysmooth = NUMpi / (w + w);
	int i;
	if (! me) return;
	Graphics_setWindow (g, fmin, fmax, -0.1, 1.1);
	for (i = 1; i <= my nx; i ++) {
		double f = my x1 + (i - 1) * my dx;
		my z [1] [i] = f < f1left ? 0.0 : f < f1right ? ( f1 > 0.0 ? 0.5 - 0.5 * cos (halfpibysmooth * (f - f1left)) : 1.0 ) :
			f < f2left ? 1.0 : f < f2right ? ( f2 < fmax ? 0.5 + 0.5 * cos (halfpibysmooth * (f - f2left)) : 1.0 ) : 0.0;
	}
	if (stop)
		for (i = 1; i <= my nx; i ++)
			my z [1] [i] = 1.0 - my z [1] [i];
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textLeft (g, TRUE, "Intensity filter %H (%f)");
		Graphics_markLeft (g, 0, TRUE, TRUE, FALSE, NULL);
		Graphics_markLeft (g, 1, TRUE, TRUE, FALSE, NULL);
	}
	if (garnish == 1) {
		Graphics_textBottom (g, TRUE, "Frequency %f");
		Graphics_markBottom (g, f1left, FALSE, TRUE, TRUE, "%f__1_-%w");
		Graphics_markBottom (g, f1, FALSE, TRUE, TRUE, "%f__1_");
		Graphics_markBottom (g, f1right, FALSE, TRUE, TRUE, "%f__1_+%w");
		Graphics_markBottom (g, f2left, FALSE, TRUE, TRUE, "%f__2_-%w");
		Graphics_markBottom (g, f2, FALSE, TRUE, TRUE, "%f__2_");
		Graphics_markBottom (g, f2right, FALSE, TRUE, TRUE, "%f__2_+%w");
		Graphics_markRight (g, 1, FALSE, TRUE, FALSE, "0 dB");
		Graphics_markLeft (g, 0.5, TRUE, TRUE, TRUE, NULL);
		Graphics_markRight (g, 0.5, FALSE, TRUE, FALSE, "-6 dB");
	}
	if (garnish == 2) {
		Graphics_textBottom (g, TRUE, "Frequency %f (Hz)");
		Graphics_markBottom (g, 0, TRUE, TRUE, FALSE, NULL);
		Graphics_markBottom (g, 500, TRUE, TRUE, FALSE, NULL);
		Graphics_markBottom (g, 1000, TRUE, TRUE, FALSE, NULL);
		Graphics_markBottom (g, 2000, TRUE, TRUE, FALSE, NULL);
		Graphics_markBottom (g, 4000, TRUE, TRUE, FALSE, NULL);
	}
	Graphics_setColour (g, stop ? Graphics_BLUE : Graphics_RED);
	Sound_draw (me, g, 0, 0, -0.1, 1.1, FALSE, "curve");
	Graphics_setColour (g, Graphics_BLACK);
	forget (me);
}
static void draw_SpectrumPassHann (Graphics g) {
	draw_SpectrumHann (g, 500, 1000, 0, 1);
}
static void draw_SpectrumPassHann_decompose (Graphics g) {
	draw_SpectrumHann (g, 0, 500, 0, 2);
	draw_SpectrumHann (g, 500, 1000, 0, 0);
	draw_SpectrumHann (g, 1000, 2000, 0, 0);
	draw_SpectrumHann (g, 2000, 4000, 0, 0);
}
static void draw_SpectrumStopHann (Graphics g) { draw_SpectrumHann (g, 500, 1000, 1, 1); }
static void draw_SpectrumStopHann_decompose (Graphics g) {
	draw_SpectrumHann (g, 500, 1000, 0, 2);
	draw_SpectrumHann (g, 500, 1000, 1, 0);
}

void manual_spectrum_init (ManPages me);
void manual_spectrum_init (ManPages me) {

MAN_BEGIN ("Ltas", "ppgb", 20041122)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. "
	"#Ltas is short for Long-Term Average Spectrum.")
NORMAL ("An object of class Ltas represents the power spectral density as a function of frequency, "
	"expressed in dB/Hz relative to 2\\.c10^^-5^ Pa. ")
ENTRY ("Inside an Ltas object")
NORMAL ("With @Inspect, you will see the following attributes:")
TAG ("%x__%min_")
DEFINITION ("the bottom of the frequency domain, in Hertz. Usually 0.")
TAG ("%x__%max_")
DEFINITION ("the top of the frequency domain, in Hertz.")
TAG ("%n__%x_")
DEFINITION ("the number of frequency bins (\\>_ 1).")
TAG ("%dx")
DEFINITION ("the frequency step, or %%bin width%, in Hertz.")
TAG ("%x__1_")
DEFINITION ("the frequency associated with the first bin, in Hertz. "
	"Usually equals %dx / 2, because the first bin tends to start at 0 Hertz.")
TAG ("%z__1%i_, %i = 1 ... %n__%x_")
DEFINITION ("the power spectrum, expressed in dB. ")
MAN_END

MAN_BEGIN ("Ltas: Get bin number from frequency...", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return bin")
NORMAL ("the band number belonging to the specified frequency, expressed as a real number.")
ENTRY ("Example")
NORMAL ("If the Ltas has a bin width of 1000 Hz, and the lowest frequency is 0 Hz, "
	"the bin number associated with a frequency of 1800 Hz is 2.3.")
ENTRY ("Scripting")
NORMAL ("You can use this command to put the nearest bin centre into a script variable:")
CODE ("select Ltas hallo")
CODE ("bin = Get bin number from frequency... 1800")
CODE ("nearestBin = round (bin)")
NORMAL ("In this case, the value will not be written into the Info window. To round down or up, use")
CODE ("leftBin = floor (bin)")
CODE ("rightBin = ceiling (bin)")
MAN_END

MAN_BEGIN ("Ltas: Get bin width", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the with of a bin, expressed in Hertz.")
MAN_END

MAN_BEGIN ("Ltas: Get frequency from bin number...", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the frequency (in Hz) associated with a specified bin number.")
TAG ("%%Bin number")
DEFINITION ("the bin number whose frequency is sought.")
ENTRY ("Algorithm")
NORMAL ("the result is")
FORMULA ("%f__1_ + (%binNumber - 1) \\.c \\De%f")
NORMAL ("where %f__1_ is the frequency associated with the centre of the first bin, "
	"and \\De%f is the bin width.")
MAN_END

MAN_BEGIN ("Ltas: Get frequency of maximum...", "ppgb", 20030916)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the frequency (in Hertz) associated with the maximum energy density.")
ENTRY ("Arguments")
TAG ("%%From frequency% (s), %%To frequency% (s)")
DEFINITION ("the selected frequency domain. Values outside this domain are ignored. "
	"If %%To frequency% is not greater than %%From frequency%, "
	"the entire frequency domain of the Ltas object is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is None because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a Parabolic or Cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN ("Ltas: Get frequency of minimum...", "ppgb", 20030916)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the frequency (in Hertz) associated with the minimum energy density.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire frequency domain of the Ltas is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is None because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a Parabolic or Cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN ("Ltas: Get highest frequency", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the highest frequency, expressed in Hertz.")
MAN_END

MAN_BEGIN ("Ltas: Get lowest frequency", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object for its lowest frequency.")
ENTRY ("Return value")
NORMAL ("the lowest frequency, expressed in Hertz. It is usually 0 Hz.")
MAN_END

MAN_BEGIN ("Ltas: Get maximum...", "ppgb", 20030916)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the maximum value (in dB) within a specified frequency range.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire frequency domain of the Ltas is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is None because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a Parabolic or Cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN ("Ltas: Get mean...", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the mean value (in dB) within a specified frequency range.")
ENTRY ("Settings")
TAG ("%%From frequency% (s), %%To frequency% (s)")
DEFINITION ("the frequency range. Values outside this range are ignored. "
	"If %%To frequency% is not greater than %%From frequency%, the entire frequency domain of the Ltas is considered.")
ENTRY ("Algorithm")
NORMAL ("The mean value between the frequencies %f__1_ and %f__2_ is defined as")
FORMULA ("1/(%f__2_ - %f__1_)  \\in__%%f%1_^^%%f%2^ %df %x(%f)")
NORMAL ("where %x(%f) is the LTAS as a function of frequency, expressed in dB. "
	"For our discrete Ltas object, this mean is approximated by")
FORMULA ("1/%n \\su__%i=%m..%m+%n-1_ %x__%i_")
NORMAL ("where %n is the number of band centres between %f__1_ and %f__2_.")
MAN_END

MAN_BEGIN ("Ltas: Get minimum...", "ppgb", 20030916)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the minimum value (in dB) within a specified frequency range.")
ENTRY ("Arguments")
TAG ("%%From frequency% (s), %%To frequency% (s)")
DEFINITION ("the selected frequency domain. Values outside this domain are ignored. "
	"If %%To frequency% is not greater than %%From frequency%, "
	"the entire frequency domain of the Ltas object is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is None because of the usual large binning. "
	"If the Ltas was computed with @@Spectrum: To Ltas (1-to-1)@, "
	"a Parabolic or Cubic interpolation would be more appropriate.")
MAN_END

MAN_BEGIN ("Ltas: Get number of bins", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the total number of frequency bins.")
MAN_END

MAN_BEGIN ("Ltas: Get standard deviation...", "ppgb", 19991016)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the standard deviation (in dB) of the LTAS within a specified frequency domain.")
ENTRY ("Arguments")
TAG ("%%From frequency% (s), %%To frequency% (s)")
DEFINITION ("the frequency window. Values outside this domain are ignored. "
	"If %%To frequency% is not greater than %%From frequency%, the entire frequency domain of the Ltas is considered.")
ENTRY ("Algorithm")
NORMAL ("The standard deviation between the frequencies %f__1_ and %f__2_ is defined as")
FORMULA ("1/(%f__2_ - %f__1_)  \\in__%%f%1_^^%%f%2^ %df (%x(%f) - %\\mu)^2")
NORMAL ("where %x(%f) is the LTAS as a function of frequency, and %\\mu its mean. "
	"For our discrete Ltas object, the standard deviation is approximated by")
FORMULA ("1/(%n-1) \\su__%i=%m..%m+%n-1_ (%x__%i_ - %\\mu)^2")
NORMAL ("where %n is the number of band centres between %f__1_ and %f__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN ("Ltas: Get value at frequency...", "ppgb", 20030916)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the value (in dB) at a specified frequency. "
	"If %frequency is outside the bands of the Ltas, the result is 0.")
ENTRY ("Arguments")
TAG ("%Frequency (s)")
DEFINITION ("the time at which the value is to be evaluated.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method, see @@vector value interpolation@. "
	"The standard is None because binning is usually large.")
MAN_END

MAN_BEGIN ("Ltas: Get value in bin...", "ppgb", 20041122)
INTRO ("A @query to the selected @Ltas object.")
ENTRY ("Return value")
NORMAL ("the LTAS value (in dB) in a specified bin. "
	"If the bin number is less than 1 or greater than the number of bin, the result is 0; "
	"otherwise, it is %z [1] [%%bin number%].")
ENTRY ("Setting")
TAG ("%%Bin number")
DEFINITION ("the bin whose value is to be looked up.")
MAN_END

MAN_BEGIN ("Sound: To Spectrogram...", "ppgb", 20040330)
INTRO ("A command that creates a @Spectrogram from every selected @Sound object. "
	"It performs a %%short-term spectral analysis%, which means that for a number of time points in the Sound, "
	"Praat computes an approximation of the spectrum at that time. Each such spectrum is called an %%analysis frame%.")
NORMAL ("For tutorial information, see @@Intro 3. Spectral analysis@.")
ENTRY ("Arguments")
TAG ("%%Window length")
DEFINITION ("the duration of the analysis window. If this is 0.005 seconds, Praat uses for each frame "
	"the part of the sound that lies between 0.0025 seconds before and 0.0025 seconds after the centre of that frame "
	"(for Gaussian windows, Praat actually uses a bit more than that). "
	"The window length determines the %bandwidth of the spectral analysis, i.e. the width of the horizontal line "
	"in the spectrogram of a pure sine wave. "
	"For a Gaussian window, the -3 dB bandwidth is 2*sqrt(6*ln(2))/(\\pi*%%Window length%), "
	"or 1.2982804 / %%Window length%. "
	"To get a `broad-band' spectrogram (bandwidth 260 Hz), set %%Window length% to 5 milliseconds; "
	"to get a `narrow-band' spectrogram (bandwidth 43 Hz), set it to 30 milliseconds. "
	"The other window shapes give slightly different values.")
TAG ("%%Maximum frequency")
DEFINITION ("the maximum frequency subject to analysis, e.g. 5000 Hertz. "
	"If it is higher than the Nyquist frequency of the Sound (which is half its sampling frequency), "
	"some values in the result will be zero (and will be drawn in white by @@Spectrogram: Paint...@).")
TAG ("%%Time step")
DEFINITION ("the distance between the centres of subsequent frames, e.g. 0.002 seconds. "
	"This determines the number of frames of the resulting Spectrogram. For instance, if the Sound is 1 second long, "
	"and the time step is 2 milliseconds, the Spectrogram will consist of almost 500 frames "
	"(not %exactly 500, because no reliable spectrum can be measured near the beginning and end of the sound). "
	"See below for cases in which the time step of the resulting Spectrogram is different from what you supply here.")
TAG ("%%Frequency step")
DEFINITION ("the frequency resolution, e.g. 20 Hertz. "
	"This determines the number of frequency bands (%bins) of the resulting Spectrogram. For instance, if the %%Maximum frequency% "
	"is 5000 Hz, and the frequency step is 20 Hz, the Spectrogram will consist of 250 frequency bands. "
	"See below for cases in which the frequency step of the resulting Spectrogram is different from what you supply here.")
TAG ("%%Window shape")
DEFINITION ("determines the shape of the analysis window. "
	"You can choose from: Gaussian, Square (none, rectangular), Hamming (raised sine-squared), "
	"Bartlett (triangular), Welch (parabolic), and Hanning (sine-squared). "
	"The Gaussian window is superior, as it gives no sidelobes in your spectrogram; "
	"it analyzes a factor of 2 slower than the other window shapes, "
	"because the analysis is actually performed on twice as many samples per frame.")
NORMAL ("For purposes of computation speed, Praat may decide to change the time step and the frequency step. "
	"This is because the time step never needs to be smaller than 1/(8\\Vr\\pi) of the window length, "
	"and the frequency step never needs to be smaller than (\\Vr\\pi)/8 of the inverse of the window length. "
	"For instance, if the window length is 29 ms, the actual time step will be never be less than 29/(8\\Vr\\pi) = 2.045 ms. "
	"And if the window length is 5 ms, the actual frequency step will never be less than (\\Vr\\pi)/8/0.005 = 44.32 Hz.")
ENTRY ("Tests of the bandwidth")
NORMAL ("You can check the bandwidth formula with the following procedure:")
CODE ("! create a 1000-Hz sine wave, windowed by a 0.2-seconds Gaussian window.")
CODE ("Create Sound... gauss 0 1 22050 sin(2*pi*1000*x) * exp(-3*((x-0.5)/0.1)\\^ 2)")
CODE ("! compute its spectrum and look at its bandwidth")
CODE ("To Spectrum (fft)")
CODE ("Draw... 980 1020 20 80 yes")
CODE ("Marks bottom every... 1 2 yes yes yes")
CODE ("Marks left every... 1 2 yes yes yes")
CODE ("! now you should see a peak at 1000 Hz with a 3 dB bandwidth of 7 Hz (20 dB: 17 Hz)")
CODE ("! more precise: compute the position and width of the peak, and write them to the console")
CODE ("Formula... if x<980 or x>1020 then 0 else self fi")
CODE ("To Formant (peaks)... 20")
CODE ("Write to console")
CODE ("! now you should be able to read that a peak was found at 999.99982 Hz")
CODE ("! with a bandwidth of 6.497 Hz; the theory above predicted 6.491 Hz")
CODE ("")
CODE ("! The same, windowed by a 0.1-seconds Hamming window.")
CODE ("Create Sound... Hamming 0 1 22050 if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(0.54+0.46*cos(pi*(x-0.5)/0.1)) fi")
CODE ("To Spectrum (fft)")
CODE ("Formula... if x<970 or x>1030 then 0 else self fi")
CODE ("To Formant (peaks)... 20")
CODE ("Write to console")
CODE ("! peak at 999.99817 Hz, 3 dB bw 6.518 Hz, 20 dB bw 15 Hz, zero bw 20 Hz, sidelobe -42 dB")
CODE ("")
CODE ("! The same, windowed by a 0.1-seconds rectangular window.")
CODE ("Create Sound... rectangular 0 1 22050 if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x) fi")
CODE ("To Spectrum (fft)")
CODE ("Formula... if x<970 or x>1030 then 0 else self fi")
CODE ("To Formant (peaks)... 20")
CODE ("Write to console")
CODE ("! peak at 999.99506 Hz, 3 dB bw 4.440 Hz, 20 dB bw 27 Hz, zero bw 10 Hz, sidelobe -14 dB")
CODE ("")
CODE ("! The same, windowed by a 0.1-seconds Hanning window.")
CODE ("Create Sound... Hanning 0 1 22050 if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(0.5+0.5*cos(pi*(x-0.5)/0.1)) fi")
CODE ("To Spectrum (fft)")
CODE ("Formula... if x<970 or x>1030 then 0 else self fi")
CODE ("To Formant (peaks)... 20")
CODE ("Write to console")
CODE ("! peak at 999.99945 Hz, 3 dB bw 7.212 Hz, 20 dB bw 16 Hz, zero bw 20 Hz, sidelobe -31 dB")
CODE ("")
CODE ("! The same, windowed by a 0.1-seconds triangular window.")
CODE ("Create Sound... triangular 0 1 22050 if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(1-abs((x-0.5)/0.1)) fi")
CODE ("To Spectrum (fft)")
CODE ("Formula... if x<970 or x>1030 then 0 else self fi")
CODE ("To Formant (peaks)... 20")
CODE ("Write to console")
CODE ("! peak at 999.99933 Hz, 3 dB bw 6.384 Hz, 20 dB bw 15 Hz, zero bw 20 Hz, sidelobe -26 dB")
CODE ("")
CODE ("! The same, windowed by a 0.1-seconds parabolic window.")
CODE ("Create Sound... parabolic 0 1 22050 if x<0.4 or x>0.6 then 0 else sin(2*pi*1000*x)*(1-((x-0.5)/0.1)^2) fi")
CODE ("To Spectrum (fft)")
CODE ("Formula... if x<970 or x>1030 then 0 else self fi")
CODE ("To Formant (peaks)... 20")
CODE ("Write to console")
CODE ("! peak at 999.99921 Hz, 3 dB bw 5.786 Hz, 20 dB bw 12 Hz, zero bw 15 Hz, sidelobe -21 dB")
MAN_END

MAN_BEGIN ("Sound: To Ltas (pitch-corrected)...", "ppgb", 20051127)
INTRO ("A command available in the #Spectrum menu if you select one or more @Sound objects. "
	"It tries to compute an @Ltas of the spectral envelope of the voiced parts, "
	"correcting away the influence of F0 in a way that does not sacrifice frequency selectivity. "
	"The resulting Ltas is meant to reflect only the resonances (formants) in the vocal tract "
	"and the envelope of the glottal source spectrum.")
NORMAL ("The analysis method is described in @@Boersma & Kovacic (to appear)@.")
MAN_END

MAN_BEGIN ("Sound: To Spectrum...", "ppgb", 20041123)
INTRO ("A command that appears in the #Spectrum menu if you select one or more @Sound objects. "
	"It turns the selected Sound into a @Spectrum by an over-all spectral analysis, a %%Fourier transform%.")
ENTRY ("Setting")
TAG ("%Fast")
DEFINITION ("determines whether zeroes are appended to the sound such that the number of samples is a power of two. "
	"This can appreciably speed up the Fourier transform.")
ENTRY ("Mathematical procedure")
NORMAL ("For the Fourier transform, the P\\s{RAAT}-defined @@time domain@ of the @Sound is ignored. "
	"Instead, its time domain is considered to run from %t=0 to %t=%T, "
	"where %t=0 is supposed to be aligned with the first sample, "
	"and %T is the total duration of the samples, i.e. %%N%\\De%t, "
	"where %N is the number of samples and \\De%t is the @@sampling period@. "
	"Thus, the last sample lies at %t=%T-\\De%t.")
NORMAL ("For a sound %x(%t), defined for all times %t in the domain (0, %T), "
	"the complex spectrum %X(%f) for any frequency %f is the forward Fourier transform of %x(%t), with a negative exponent:")
FORMULA ("%X(%f) = \\in__0_^^%T^ %x(%t) %e^^-2%%\\piift%^ %dt")
NORMAL ("If the Sound is expressed in Pascal (Pa), the Spectrum is expressed in Pa\\.cs, or Pa/Hz. "
	"Since a @Spectrum object can only contain a finite number of frequency samples, "
	"it is only computed for frequencies that are multiples of \\De%f = 1/%T. "
	"The number of those frequencies is determined by the number of samples %N of the sound.")
NORMAL ("If %N is odd, there will be %N frequency samples. For instance, if the sound has 20,457 samples, "
	"the spectrum will be computed at the frequencies -10,228\\De%f, -10,227\\De%f, ..., -\\De%f, 0, +\\De%f, ..., "
	"+10,227\\De%f, +10,228\\De%f. If we suppose that a frequency sample represents a frequency bin with a width of \\De%f, "
	"we see that the frequency samples span adjacent frequency ranges, "
	"e.g. the first sample runs from -10,228.5\\De%f to -10,227.5\\De%f, the second from -10,227.5\\De%f to -10,226.5\\De%f. "
	"Together, the frequency samples span the frequency domain of the spectrum, "
	"which runs from -%F to +%F, where %F = 10,228.5\\De%f. "
	"We can see that this frequency equals one half of the @@sampling frequency@ of the original sound: "
	"%F = 10,228.5\\De%f = 10,228.5/%T = 10,228.5/(20,457\\De%t) = 0.5/\\De%t. This is the so-called @@Nyquist frequency@.")
NORMAL ("If %N is even, there will be %N+1 frequency samples. For instance, if the sound has 32,768 samples, "
	"the spectrum will be computed at the frequencies -16,384\\De%f, -16,383\\De%f, ..., -\\De%f, 0, +\\De%f, ..., "
	"+16,383\\De%f, +16,384\\De%f. Again, the frequency samples span adjacent frequency ranges, "
	"but the first and last samples are only half as wide as the rest, "
	"i.e. the first sample runs from -16,384\\De%f to -16,383.5\\De%f, the second from -16,383.5\\De%f to -16,382.5\\De%f, "
	"and the last from +16,383.5\\De%f to +16,384\\De%f. "
	"Together, the frequency samples again span the frequency domain of the spectrum, "
	"which runs from -%F to +%F, where %F = 16,384\\De%f = 0.5/\\De%t, the Nyquist frequency.")
ENTRY ("Storage")
NORMAL ("In a @Spectrum object, P\\s{RAAT} stores the real and imaginary parts of the complex spectrum separately. "
	"The real part is equal to the cosine transform:")
FORMULA ("re %X(%f) = \\in__0_^^%T^ %x(%t) cos (2%%\\pift%) %dt")
NORMAL ("The imaginary part is equal to the reverse of the sine transform:")
FORMULA ("im %X(%f) = - \\in__0_^^%T^ %x(%t) sin (2%%\\pift%) %dt")
NORMAL ("The complex spectrum can be reconstructed from the real and imaginary part as follows:")
FORMULA ("%X(%f) = re %X(%f) + %i im %X(%f)")
NORMAL ("Since the cosine is a symmetric function of %t and the sine is an antisymmetric function of %t, "
	"the complex spectrum for a negative frequency is the complex conjugate of the complex spectrum for the corresponding "
	"positive frequency:")
FORMULA ("%X(-%f) = re %X(-%f) + %i im %X(-%f) = re %X(%f) - %i im %X(%f) = %X^*(%f)")
NORMAL ("For purposes of storage, therefore, the negative frequencies are superfluous. "
	"For this reason, the Spectrum object stores re %X(%f) and im %X(%f) only for frequencies %f = 0, \\De%f, 2\\De%f... "
	"In the case of a sound with 20,457 samples, the Spectrum object contains the real part of %X(0) "
	"(its imaginary part is always zero), and the real and imaginary parts of %X(%f) "
	"for frequencies from \\De%f to 10,228\\De%f, which makes in total 1+2\\.c10,228 = 20,457 real values. "
	"In the case of a sound with 32,768 samples, the Spectrum object contains the real parts of %X(0) and %X(16,384\\De%f) "
	"(their imaginary parts are always zero), and the real and imaginary parts of %X(%f) "
	"for frequencies from \\De%f to 16,383\\De%f, which makes in total 2+2\\.c16,383 = 32,768 real values.")
NORMAL ("Since the negative frequencies have been removed, the frequency domain now runs from 0 to %F. "
	"This means that the first frequency bin is now only 0.5\\De%f wide (i.e. as wide as the last bin for even-%N spectra), "
	"which has consequences for computations of energies.")
ENTRY ("Behaviour")
NORMAL ("If you perform @@Spectrum: To Sound@ on the resulting Spectrum object, "
	"a Sound is created that is equal to the original Sound (or to the original Sound with appended zeroes).")
ENTRY ("Properties")
NORMAL ("The frequency integral over the squared Spectrum equals the time integral over the squared Sound:")
FORMULA ("\\in__-%F_^^+%F^ |%X(%f)|^2 %df = \\in__0_^%T |%x(%t)|^2 %dt")
NORMAL ("This is called %%Parceval's theorem%.")
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

MAN_BEGIN ("Spectrogram", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. For tutorial information, see @@Intro 3. Spectral analysis@.")
NORMAL ("A Spectrogram object represents  an acoustic time-frequency representation of a sound: "
	"the power spectral density %P (%f, %t), expressed in Pa^2/Hz. "
	"It is sampled into a number of points centred around equally spaced times %t__%i_ "
	"and frequencies %f__%j_.")
ENTRY ("Inside a Spectrogram")
NORMAL ("With @Inspect, you will see the following attributes:")
TAG ("%xmin")
DEFINITION ("start time, in seconds.")
TAG ("%xmax")
DEFINITION ("end time, in seconds.")
TAG ("%nx")
DEFINITION ("the number of times (\\>_ 1).")
TAG ("%dx")
DEFINITION ("time step, in seconds.")
TAG ("%x1")
DEFINITION ("the time associated with the first column, in seconds. "
	"This will usually be in the range [%xmin, %xmax]. "
	"The time associated with the last column (i.e., %x1 + (%nx \\-- 1) %dx)) "
	"will also usually be in that range.")
TAG ("%ymin")
DEFINITION ("lowest frequency, in Hertz. Normally 0.")
TAG ("%ymax")
DEFINITION ("highest frequency, in Hertz.")
TAG ("%ny")
DEFINITION ("the number of frequencies (\\>_ 1).")
TAG ("%dy")
DEFINITION ("frequency step, in Hertz.")
TAG ("%y1")
DEFINITION ("the frequency associated with the first row, in Hertz. Usually %dy / 2. "
	"The frequency associated with the last row (i.e., %y1 + (%ny \\-- 1) %dy)) "
	"will often be %ymax - %dy / 2.")
TAG ("%z__%ij_, %i = 1 ... %ny, %j = 1 ... %nx")
DEFINITION ("the power spectral density, in Pa^2/Hz. ")
MAN_END

MAN_BEGIN ("Spectrogram: Formula...", "ppgb", 20021206)
INTRO ("A command for changing the data in all selected @Spectrogram objects.")
NORMAL ("See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN ("Spectrogram: Paint...", "ppgb", 20030916)
INTRO ("A command to draw the selected @Spectrogram object(s) into the @@Picture window@ in shades of grey.")
ENTRY ("Arguments")
TAG ("%%From time%, %%To time% (seconds)")
DEFINITION ("the time domain along the %x axis.")
TAG ("%%From frequency%, %%To frequency% (Hertz)")
DEFINITION ("the frequency domain along the %y axis.")
TAG ("%%Dynamic range% (dB)")
DEFINITION ("The global maximum of the spectrogram (after preemphasis) will always be drawn in black; "
	"all values that are more than %%Dynamic range% dB below this maximum (after dynamic compression) "
	"will be drawn in white. Values in-between have appropriate shades of grey.")
TAG ("%%Preemphasis% (dB/octave)")
DEFINITION ("determines the steepness of a high-pass filter, "
	"i.e., how much the power of higher frequencies will be raised before drawing, as compared to lower frequencies. "
	"Since the spectral slope of human vowels is approximately -6 dB per octave, "
	"the standard value for this parameter is +6 dB per octave, "
	"so that the spectrum is flattened and the higher formants look as strong as the lower ones.")
TAG ("%%Dynamic compression")
DEFINITION ("determines how much stronger weak time frames should be made before drawing. "
	"Normally, this parameter is between 0 and 1. If it is 0, there is no dynamic compression. "
	"If it is 1, all time frames (vertical bands) will be drawn equally strong, "
	"i.e., all of them will contain frequencies that are drawn in black. "
	"If this parameter is 0.4 and the global maximum is at 80 dB, then a frame with a maximum at 20 dB "
	"(which will normally be drawn all white if the dynamic range is 50 dB), "
	"will be raised by 0.4 * (80 - 20) = 24 dB, "
	"so that its maximum will be seen at 44 dB (thus making this frame visible).")
MAN_END

MAN_BEGIN ("Spectrogram: To Spectrum (slice)...", "ppgb", 19961003)
INTRO ("A command to create a @Spectrum object from every selected @Spectrogram object.")
ENTRY ("Purpose")
NORMAL ("to extract the information contained in a Spectrogram at a certain time.")
ENTRY ("Algorithm")
NORMAL ("The Spectrum will be constructed from one frame of the Spectrogram, "
	"namely the frame whose centre is closed to the %time argument. ")
MAN_END

MAN_BEGIN ("Spectrum", "ppgb", 20041123)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. A Spectrum object represents "
	"the complex spectrum as a function of frequency. "
	"If the spectrum was created from a sound (which is expressed in units of Pascal), "
	"the complex values are expressed in units Pa/Hz (Pascal per Hertz). "
	"For detailed information, see @@Sound: To Spectrum...@.")
ENTRY ("Spectrum commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Sound: To Spectrum...")
NORMAL ("Queries:")
LIST_ITEM ("\\bu @@Spectrum: Get centre of gravity...")
LIST_ITEM ("\\bu @@Spectrum: Get standard deviation...")
LIST_ITEM ("\\bu @@Spectrum: Get skewness...")
LIST_ITEM ("\\bu @@Spectrum: Get kurtosis...")
LIST_ITEM ("\\bu @@Spectrum: Get central moment...")
NORMAL ("Modification:")
LIST_ITEM ("\\bu @@Spectrum: Filter (pass Hann band)...")
LIST_ITEM ("\\bu @@Spectrum: Filter (stop Hann band)...")
LIST_ITEM ("\\bu @@Formula...")
NORMAL ("Conversion:")
LIST_ITEM ("\\bu @@Spectrum: To Ltas (1-to-1)")
LIST_ITEM ("\\bu @@Spectrum: To Spectrogram")
NORMAL ("Synthesis:")
LIST_ITEM ("\\bu @@Spectrum: To Sound")
MAN_END

MAN_BEGIN ("Spectrum: Filter (pass Hann band)...", "ppgb", 20030916)
INTRO ("A command to modify every selected @Spectrum object.")
NORMAL ("The complex values in the #Spectrum are multiplied by real-valued sine shapes and straight lines, according to the following figure:")
PICTURE (5, 3, draw_SpectrumPassHann)
ENTRY ("Arguments")
TAG ("%%From frequency% (standard value: 500 Hz)")
DEFINITION ("the lower edge of the pass band (%f__1_ in the figure). The value zero is special: the filter then acts as a low-pass filter.")
TAG ("%%To frequency% (standard value: 1000 Hz)")
DEFINITION ("the upper edge of the pass band (%f__2_ in the figure). The value zero is special: the filter then acts as a high-pass filter.")
TAG ("%Smoothing (standard value: 100 Hz)")
DEFINITION ("the width of the region between pass and stop (%w in the figure).")
ENTRY ("Usage")
NORMAL ("Because of its symmetric Hann-like shape, the filter is especially useful for decomposing the Spectrum into consecutive bands. "
	"For instance, we can decompose the spectrum into the bands 0-500 Hz, 500-1000 Hz, 1000-2000 Hz, and 2000-\"0\" Hz:")
PICTURE (5, 3, draw_SpectrumPassHann_decompose)
NORMAL ("By adding the four bands together, we get the original spectrum again.")
NORMAL ("A complementary filter is described at @@Spectrum: Filter (stop Hann band)...@.")
NORMAL ("See the @Filtering tutorial for information on the need for smoothing and a comparative discussion of various filters.")
MAN_END

MAN_BEGIN ("Spectrum: Filter (stop Hann band)...", "ppgb", 20030916)
INTRO ("A command to modify every selected @Spectrum object.")
NORMAL ("The complex values in the #Spectrum are multiplied by real-valued sine shapes and straight lines, according to the following figure:")
PICTURE (5, 3, draw_SpectrumStopHann)
ENTRY ("Arguments")
TAG ("%%From frequency% (standard value: 500 Hz)")
DEFINITION ("the lower edge of the stop band (%f__1_ in the figure). The value zero is special: the filter then acts as a high-pass filter.")
TAG ("%%To frequency% (standard value: 1000 Hz)")
DEFINITION ("the upper edge of the stop band (%f__2_ in the figure). The value zero is special: the filter then acts as a low-pass filter.")
TAG ("%Smoothing (standard value: 100 Hz)")
DEFINITION ("the width of the region between stop and pass (%w in the figure).")
ENTRY ("Usage")
NORMAL ("This filter is the complement from the pass-band filter (@@Spectrum: Filter (pass Hann band)...@). "
	"For instance, we can decompose the spectrum into the above stop-band spectrum and a band from 500 to 1000 Hz:")
PICTURE (5, 3, draw_SpectrumStopHann_decompose)
NORMAL ("By adding the two spectra together, we get the original spectrum again.")
NORMAL ("See the @Filtering tutorial for information on the need for smoothing and a comparative discussion of various filters.")
MAN_END

MAN_BEGIN ("Spectrum: Formula...", "ppgb", 20021206)
INTRO ("A command for changing the data in all selected @Spectrum objects.")
NORMAL ("See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN ("Spectrum: Get central moment...", "ppgb", 20020323)
INTRO ("A command to query the selected @Spectrum object.")
NORMAL ("If the complex spectrum is given by %S(%f), the %%n%th central spectral moment is given by")
FORMULA ("\\in__0_^\\oo  (%f \\-- %f__%c_)^%n |%S(%f)|^%p %df")
NORMAL ("divided by the \"energy\"")
FORMULA ("\\in__0_^\\oo  |%S(%f)|^%p %df")
NORMAL ("In this formula, %f__%c_ is the spectral centre of gravity (see @@Spectrum: Get centre of gravity...@). "
	"Thus, the %%n%th central moment is the average of (%f \\-- %f__%c_)^%n over the entire frequency domain, "
	"weighted by |%S(%f)|^%p. For %p = 2, the weighting is done by the power spectrum, and for %p = 1, "
	"the weighting is done by the absolute spectrum. A value of %p = 2/3 has been seen as well.")
ENTRY ("Arguments")
TAG ("%Moment")
DEFINITION ("the number %n in the formulas above. A number of 3 gives you the third central spectral moment. "
	"It is not impossible to ask for fractional moments, e.g. %n = 1.5.")
TAG ("%Power")
DEFINITION ("the quantity %p in the formula above. Common values are 2, 1, or 2/3.")
ENTRY ("Usage")
NORMAL ("For %n = 1, the central moment should be zero, since the centre of gravity %f__%c_ is computed with "
	"the same %p. For %n = 2, you get the variance of the frequencies in the spectrum; the standard deviation "
	"of the frequency is the square root of this. For %n = 3, you get the non-normalized spectral skewness; "
	"to normalize it, you can divide by the 1.5 power of the second moment. For %n = 4, you get the "
	"non-normalized spectral kurtosis; to normalize it, you can divide by the square of the second moment "
	"and subtract 3. Praat can directly give you the quantities mentioned here:")
LIST_ITEM ("\\bu @@Spectrum: Get centre of gravity...")
LIST_ITEM ("\\bu @@Spectrum: Get standard deviation...")
LIST_ITEM ("\\bu @@Spectrum: Get skewness...")
LIST_ITEM ("\\bu @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN ("Spectrum: Get centre of gravity...", "ppgb", 20020323)
INTRO ("A command to query the selected @Spectrum object.")
NORMAL ("If the complex spectrum is given by %S(%f), where %f is the frequency, the %%centre of gravity% "
	"is given by")
FORMULA ("\\in__0_^\\oo  %f |%S(%f)|^%p %df")
NORMAL ("divided by the \"energy\"")
FORMULA ("\\in__0_^\\oo  |%S(%f)|^%p %df")
NORMAL ("Thus, the centre of gravity is the average of %f over the entire frequency domain, "
	"weighted by |%S(%f)|^%p. For %p = 2, the weighting is done by the power spectrum, and for %p = 1, "
	"the weighting is done by the absolute spectrum. A value of %p = 2/3 has been seen as well.")
ENTRY ("Argument")
TAG ("%Power")
DEFINITION ("the quantity %p in the formulas above. Common values are 2, 1, or 2/3.")
ENTRY ("Interpretation")
NORMAL ("The spectral centre of gravity is a measure for how high the frequencies in a spectrum are on average. "
	"For a sine wave with a frequency of 377 Hz, the centre of gravity is 377 Hz. You can easily check this "
	"in Praat by creating such a sine wave (@@Create Sound...@), then converting it to a Spectrum "
	"(@@Sound: To Spectrum...@), then querying the mean frequency. For a white noise sampled at 22050 Hz, "
	"the centre of gravity is 5512.5 Hz, i.e. one half of the @@Nyquist frequency@.")
ENTRY ("Related measures")
NORMAL ("The centre of gravity is used in the computation of spectral moments:")
LIST_ITEM ("\\bu @@Spectrum: Get central moment...")
LIST_ITEM ("\\bu @@Spectrum: Get standard deviation...")
LIST_ITEM ("\\bu @@Spectrum: Get skewness...")
LIST_ITEM ("\\bu @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN ("Spectrum: Get kurtosis...", "ppgb", 20020323)
INTRO ("A command to query the selected @Spectrum object.")
NORMAL ("The (normalized) kurtosis of a spectrum is the fourth central moment of this spectrum, "
	"divided by the square of the second central moment, minus 3. "
	"See @@Spectrum: Get central moment...@.")
ENTRY ("Argument")
TAG ("%Power")
DEFINITION ("the quantity %p in the formula for the centre of gravity and the second and fourth central moment. "
	"Common values are 2, 1, or 2/3.")
ENTRY ("Interpretation")
NORMAL ("The kurtosis is a measure for how much the shape of the spectrum around the "
	"%%centre of gravity% is different from a Gaussian shape. "
	"For a white noise, the kurtosis is Ð6/5.")
ENTRY ("Related measures")
LIST_ITEM ("\\bu @@Spectrum: Get centre of gravity...")
LIST_ITEM ("\\bu @@Spectrum: Get central moment...")
LIST_ITEM ("\\bu @@Spectrum: Get standard deviation...")
LIST_ITEM ("\\bu @@Spectrum: Get skewness...")
MAN_END

MAN_BEGIN ("Spectrum: Get skewness...", "ppgb", 20020323)
INTRO ("A command to query the selected @Spectrum object.")
NORMAL ("The (normalized) skewness of a spectrum is the third central moment of this spectrum, "
	"divided by the 1.5 power of the second central moment. "
	"See @@Spectrum: Get central moment...@.")
ENTRY ("Argument")
TAG ("%Power")
DEFINITION ("the quantity %p in the formula for the centre of gravity and the second and third central moment. "
	"Common values are 2, 1, or 2/3.")
ENTRY ("Interpretation")
NORMAL ("The skewness is a measure for how much the shape of the spectrum below the "
	"%%centre of gravity% is different from the shape above the mean frequency. "
	"For a white noise, the skewness is zero.")
ENTRY ("Related measures")
LIST_ITEM ("\\bu @@Spectrum: Get centre of gravity...")
LIST_ITEM ("\\bu @@Spectrum: Get central moment...")
LIST_ITEM ("\\bu @@Spectrum: Get standard deviation...")
LIST_ITEM ("\\bu @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN ("Spectrum: Get standard deviation...", "ppgb", 20020323)
INTRO ("A command to query the selected @Spectrum object.")
NORMAL ("The standard deviation of a spectrum is the square root of the second central moment of this spectrum. "
	"See @@Spectrum: Get central moment...@.")
ENTRY ("Argument")
TAG ("%Power")
DEFINITION ("the quantity %p in the formula for the centre of gravity and the second central moment. "
	"Common values are 2, 1, or 2/3.")
ENTRY ("Interpretation")
NORMAL ("The standard deviation is a measure for how much the frequencies in a spectrum can deviate from "
	"the %%centre of gravity%. "
	"For a sine wave, the standard deviation is zero. For a white noise, the standard deviation "
	"is the @@Nyquist frequency@ divided by \\Vr12.")
ENTRY ("Related measures")
LIST_ITEM ("\\bu @@Spectrum: Get centre of gravity...")
LIST_ITEM ("\\bu @@Spectrum: Get central moment...")
LIST_ITEM ("\\bu @@Spectrum: Get skewness...")
LIST_ITEM ("\\bu @@Spectrum: Get kurtosis...")
MAN_END

MAN_BEGIN ("Spectrum: To Ltas (1-to-1)", "ppgb", 19980327)
INTRO ("A command for converting each selected @Spectrum object into an @Ltas object without loss of frequency resolution.")
ENTRY ("Algorithm")
NORMAL ("Each band %b__%i_ in the Ltas is computed from a single frequency sample %s__%i_ in the Spectrum as follows:")
FORMULA ("%b__%i_ = 2 ((Re (%s__%i_))^2 + (Im (%s__%i_))^2) / 4.0\\.c10^^-10^")
NORMAL ("If the original Spectrum is expressible in Pa / Hz (sound pressure in air), the Ltas values "
	"are in \"dB/Hz\" relative to the auditory threshold at 1000 Hz (2\\.c10^^-5^ Pa).")
MAN_END

MAN_BEGIN ("Spectrum: To Sound", "ppgb", 200411123)
INTRO ("A command for creating a @Sound object from every selected @Spectrum object.")
ENTRY ("Mathematical procedure")
NORMAL ("The reverse of the Fourier transform described in @@Sound: To Spectrum...@. "
	"If the Spectrum is expressed in Pa/Hz, the Sound will be in Pascal. "
	"The frequency integral over the Sound equals the time integral over the Spectrum.")
ENTRY ("Behaviour")
NORMAL ("If you perform this command on a Spectrum object that was created earlier with @@Sound: To Spectrum...@, "
	"the resulting Sound is equal to the Sound that was input to ##Sound: To Spectrum...#.")
MAN_END

MAN_BEGIN ("Spectrum: To Spectrogram", "ppgb", 19961003)
INTRO ("A command to create a @Spectrogram object from every selected @Spectrum object.")
ENTRY ("Purpose")
NORMAL ("Format conversion.")
ENTRY ("Behaviour")
NORMAL ("The Spectrogram will have only one frame (time slice).")
ENTRY ("Algorithm")
NORMAL ("The values are computed as the sum of the squares of the real and imaginary parts of the Spectrum.")
MAN_END

MAN_BEGIN ("SpectrumEditor", "ppgb", 20030316)
INTRO ("One of the @editors in P\\s{RAAT}. It allows you to view, "
	"zoom, and play a @Spectrum object.")
NORMAL ("Clicking on one of the (maximally) 8 rectangles above or below the drawing area "
	"lets you play a @Sound that is synthesized from a band-filtered part of the @Spectrum. "
	"You can also copy the Spectrum, band-filtered with the @@frequency selection@, "
	"or the Sound synthesized from this Spectrum, to the list of objects.")
MAN_END

}

/* End of file manual_spectrum.c */
