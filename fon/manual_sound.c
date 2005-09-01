/* manual_sound.c
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

static void draw_SoundDeepen_filter (Graphics g) {
	Sound s = Sound_createSimple (100, 10);
	int i;
	double alpha = sqrt (log (2.0));
	if (! s) return;
	Graphics_setWindow (g, 0, 100, 0, 1);
	for (i = 1; i <= s -> nx; i ++) {
		double alpha_f = alpha * (s -> x1 + (i - 1) * s -> dx);
		double slow = alpha_f / 3.0, fast = alpha_f / 30.0;
		s -> z [1] [i] = exp (- fast * fast) - exp (- slow * slow);
	}
	Graphics_drawInnerBox (g);
	Graphics_textBottom (g, TRUE, "Frequency %f (Hz)");
	Graphics_textLeft (g, TRUE, "Intensity filter %H (%f)");
	Graphics_markLeft (g, 0, TRUE, TRUE, FALSE, NULL);
	Graphics_markLeft (g, 0.5, TRUE, TRUE, TRUE, NULL);
	Graphics_markLeft (g, 1, TRUE, TRUE, FALSE, NULL);
	Graphics_markRight (g, 1, FALSE, TRUE, FALSE, "0 dB");
	Graphics_markRight (g, 0.5, FALSE, TRUE, FALSE, "-6 dB");
	Graphics_markBottom (g, 0, TRUE, TRUE, FALSE, NULL);
	Graphics_markBottom (g, 3, TRUE, TRUE, TRUE, NULL);
	Graphics_markBottom (g, 30, TRUE, TRUE, TRUE, NULL);
	Graphics_markBottom (g, 100, TRUE, TRUE, FALSE, NULL);
	Graphics_setColour (g, Graphics_RED);
	Sound_draw (s, g, 0, 0, 0, 1, FALSE, "curve");
	Graphics_setColour (g, Graphics_BLACK);
	forget (s);
}

static void draw_SoundDeepen_impulse (Graphics g) {
	Sound s = Sound_create (-0.2, 0.2, 1000, 4e-4, -0.1998);
	int i;
	double pibyalpha = NUMpi / sqrt (log (2.0)), twosqrtpitimespibyalpha = 2 * sqrt (NUMpi) * pibyalpha;
	if (! s) return;
	Graphics_setWindow (g, -0.2, 0.2, -100, 400);
	for (i = 1; i <= s -> nx; i ++) {
		double pibyalpha_f = pibyalpha * (s -> x1 + (i - 1) * s -> dx);
		double slow = pibyalpha_f * 3.0, fast = pibyalpha_f * 30.0;
		s -> z [1] [i] = twosqrtpitimespibyalpha * (30.0 * exp (- fast * fast) - 3.0 * exp (- slow * slow));
	}
	Graphics_drawInnerBox (g);
	Graphics_textBottom (g, TRUE, "Time %t (s)");
	Graphics_textLeft (g, TRUE, "Intensity impulse reponse %h (%t)");
	Graphics_markBottom (g, -0.2, TRUE, TRUE, FALSE, NULL);
	Graphics_markBottom (g, 0, TRUE, TRUE, TRUE, NULL);
	Graphics_markBottom (g, 0.2, TRUE, TRUE, FALSE, NULL);
	Graphics_markLeft (g, 0, TRUE, TRUE, TRUE, NULL);
	Graphics_setColour (g, Graphics_RED);
	Sound_draw (s, g, 0, 0, -100, 400, FALSE, "curve");
	Graphics_setColour (g, Graphics_BLACK);
	forget (s);
}

void manual_sound_init (ManPages me);
void manual_sound_init (ManPages me) {

MAN_BEGIN ("Create Sound...", "ppgb", 20040331)
INTRO ("A command in the @@New menu@ to create a @Sound with a specified duration and sampling frequency, "
	"filled with values from a formula.")
NORMAL ("See the @Formulas tutorial for explanations and examples.")
/*
Create Sound... blok 0 1 22050 if round(x*377*2) mod 2 then 0.9 else -0.9 fi

Je kunt dus heel veel maken met een scriptje:

form Blokgolf
   word Naam blok
   real Begintijd_(s) 0.0
   real Eindtijd_(s) 1.0
   positive Samplefrequentie_(Hz) 22050
   positive Frequentie_(Hz) 377
   real Amplitude 0.9
endform
Create Sound... 'Naam' Begintijd Eindtijd Samplefrequentie
   ... if round(x*'Frequentie'*2) mod 2 then 'Amplitude' else -'Amplitude' fi
*/
MAN_END

MAN_BEGIN ("Create Sound from tone complex...", "ppgb", 20021223)
INTRO ("A command in the @@New menu@ to create a @Sound as the sum of a number of sine waves "
	"with equidistant frequencies.")
ENTRY ("Arguments")
TAG ("%Name")
DEFINITION ("the name of the resulting Sound object.")
TAG ("%%Starting time%, %%End time% (s)")
DEFINITION ("the time domain of the resulting Sound.")
TAG ("%%Sampling frequency% (Hz)")
DEFINITION ("the sampling frequency of the resulting Sound.")
TAG ("%Phase")
DEFINITION ("determines whether the result is a sum of %sines or a sum of %cosines, "
	"i.e., whether the zero crossings or the maxima of the components are synchronized. "
	"This choice has little perceptual consequences.")
TAG ("%%Frequency step% (Hz)")
DEFINITION ("the distance between the components. In first approximation, "
	"this is the perceived fundamental frequency.")
TAG ("%%First frequency% (Hz)")
DEFINITION ("the lowest frequency component. If you supply a value of 0, "
	"%firstFrequency is taken equal to %frequencyStep.")
TAG ("%%Ceiling% (Hz)")
DEFINITION ("the frequency above which no components are used. If you supply a value of 0 "
	"or a value above the Sound's @@Nyquist frequency@, %ceiling is taken equal to "
	"the Nyquist frequency.")
TAG ("%%Number of components")
DEFINITION ("determines how many sinusoids are used. If you supply a value of 0 "
	"or a very high value, the maximum number of components is used, "
	"limited by %ceiling.")
ENTRY ("Example 1: a pulse train")
NORMAL ("A series of pulses at regular intervals, "
	"sampled after low-pass filtering at the Nyquist frequency, "
	"can be regarded as a sum of cosine waves. For instance, a 100-Hz pulse train, "
	"sampled at 22050 Hz, can be created with:")
CODE ("Create Sound from tone complex... train 0 1 22050 Cosine 100 0 0 0")
NORMAL ("Supplying the value 0 for %firstFrequency yields an unshifted harmonic complex.")
ENTRY ("Example 2: a shifted harmonic complex")
NORMAL ("Some experiments on human pitch perception (%%residue pitch%) use "
	"a number of sinusoidal components with harmonically "
	"related frequencies that are all shifted by a constant amount.")
NORMAL ("For instance, to get a sum of sine waves with frequencies 105 Hz, 205 Hz, and 305 Hz, "
	"you would use:")
CODE ("Create Sound from tone complex... train 0.3 1 22050 Sine 100 105 0 3")
NORMAL ("or")
CODE ("Create Sound from tone complex... train 0.3 1 22050 Sine 100 105 350 0")
NORMAL ("whichever you prefer.")
NORMAL ("Some of these experiments are described in @@Plomp (1967)@ and @@Patterson & Wightman (1976)@.")
ENTRY ("Algorithm")
NORMAL ("For the `sine' phase, the resulting Sound is given by the following formula:")
NORMAL ("%x(%t) = \\su__%i=1..%numberOfComponents_ sin (2%\\pi\\.c(%firstFrequency + "
	"(%i\\--1)\\.c%frequencyStep)\\.c%t)")
ENTRY ("More flexibility?")
NORMAL ("Suppose you wanted to vary the relative strengths of the frequency components. "
	"You could achieve this by creating a Sound with the command discussed here, "
	"take its Fourier transform, run a formula on the resulting Spectrum, "
	"and take the inverse Fourier transform.")
NORMAL ("A more general approach is described shortly.")
NORMAL ("Suppose you need a sum of sine waves with frequencies 105, 205, 305, ..., 1905 Hz, "
	"and with relative amplitudes 1, 1/2, 1/3, ..., 1/19. You could build a script that computes "
	"the various components, and add them to each other as you go along. Instead of calling 19 "
	"scripts, however, you can achieve this with the following more general script:")
CODE ("form Add waves with decreasing amplitudes")
CODE1 ("Number_of_components 19")
CODE ("endform")
CODE ("\\#  Create a Matrix with frequency and amplitude information in each row:")
CODE ("Create simple Matrix... freqAndGain number_of_components 2 0")
CODE ("Formula... if col = 1 then row * 100 + 5 else 1 / row fi")
CODE ("\\#  Create a large Matrix with all the component sine waves:")
CODE ("Create Matrix... components 0 1 10000 1e-4 0.5e-4 1 number_of_components number_of_components 1 1 0")
CODE ("Formula... Matrix_freqAndGain [2] * sin (2 * pi * Matrix_freqAndGain [1] * x)")
CODE ("\\#  Integrate:")
CODE ("Formula... self + self [row - 1, col]")
CODE ("\\#  Publish last row:")
CODE ("To Sound (slice)... number_of_components")
CODE ("Scale amplitudes... 0.99")
MAN_END

MAN_BEGIN ("Record mono Sound...", "ppgb", 20021212)
INTRO ("A command in the @@New menu@ to record a @Sound. Creates a @SoundRecorder window, "
	"except on very old Macintoshes with 8-bit audio, where it presents a native Macintosh sound-recorder window.")
MAN_END

MAN_BEGIN ("Record stereo Sound...", "ppgb", 20021212)
INTRO ("A command in the @@New menu@ to record a @Sound. Creates a @SoundRecorder window.")
MAN_END

MAN_BEGIN ("Sound", "ppgb", 20041123)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. For tutorial information, see all of the @Intro.")
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Record mono Sound...@ (from microphone or line input, with the @SoundRecorder)")
LIST_ITEM ("\\bu @@Record stereo Sound...@")
LIST_ITEM ("\\bu @@Create Sound...@ (from a formula)")
LIST_ITEM ("\\bu @@Create Sound from tone complex...")
LIST_ITEM ("\\bu @@Create Sound from gamma-tone...")
LIST_ITEM ("\\bu @@Create Sound from Shepard tone...")
NORMAL ("Reading and writing:")
LIST_ITEM ("\\bu @@Sound files")
NORMAL ("You can also use the text and binary (real-valued) formats for Sounds, like for any other class:")
LIST_ITEM ("\\bu @@Write to text file...")
LIST_ITEM ("\\bu @@Write to binary file...")
NORMAL ("Playing:")
LIST_ITEM ("\\bu @@Sound: Play")
LIST_ITEM ("\\bu @@PointProcess: Hum")
LIST_ITEM ("\\bu @@PointProcess: Play")
NORMAL ("Viewing and editing:")
LIST_ITEM ("\\bu @SoundEditor, @ManipulationEditor, @TextGridEditor, @PointEditor, @PitchTierEditor, "
	"@SpectrumEditor")
NORMAL ("Queries:")
LIST_ITEM ("structure:")
LIST_ITEM1 ("\\bu @@time domain")
LIST_ITEM1 ("\\bu @@Get number of samples")
LIST_ITEM1 ("\\bu @@Get sampling period")
LIST_ITEM1 ("\\bu @@Get sampling frequency")
LIST_ITEM1 ("\\bu @@Get time from sample number...")
LIST_ITEM1 ("\\bu @@Get sample number from time...")
LIST_ITEM ("content:")
LIST_ITEM1 ("\\bu @@Sound: Get value at time...")
LIST_ITEM1 ("\\bu @@Sound: Get value at sample number...")
LIST_ITEM ("shape:")
LIST_ITEM1 ("\\bu @@Sound: Get minimum...")
LIST_ITEM1 ("\\bu @@Sound: Get time of minimum...")
LIST_ITEM1 ("\\bu @@Sound: Get maximum...")
LIST_ITEM1 ("\\bu @@Sound: Get time of maximum...")
LIST_ITEM1 ("\\bu @@Sound: Get absolute extremum...")
LIST_ITEM1 ("\\bu @@Sound: Get nearest zero crossing...")
LIST_ITEM ("statistics:")
LIST_ITEM1 ("\\bu @@Sound: Get mean...")
LIST_ITEM1 ("\\bu @@Sound: Get root-mean-square...")
LIST_ITEM1 ("\\bu @@Sound: Get standard deviation...")
LIST_ITEM ("energy:")
LIST_ITEM1 ("\\bu @@Sound: Get energy...")
LIST_ITEM1 ("\\bu @@Sound: Get power...")
LIST_ITEM ("in air:")
LIST_ITEM1 ("\\bu @@Sound: Get energy in air")
LIST_ITEM1 ("\\bu @@Sound: Get power in air")
LIST_ITEM1 ("\\bu @@Sound: Get intensity (dB)")
NORMAL ("Modification:")
LIST_ITEM ("\\bu @@Matrix: Formula...")
LIST_ITEM ("\\bu @@Sound: Set value at sample number...")
LIST_ITEM ("\\bu @@Sound: Filter with one formant (in-line)...")
LIST_ITEM ("\\bu @@Sound: Pre-emphasize (in-line)...")
LIST_ITEM ("\\bu @@Sound: De-emphasize (in-line)...")
NORMAL ("Annotation (see @@Intro 7. Annotation@):")
LIST_ITEM ("\\bu @@Sound: To TextGrid...")
NORMAL ("Periodicity analysis:")
LIST_ITEM ("\\bu @@Sound: To Pitch...")
LIST_ITEM ("\\bu @@Sound: To Pitch (ac)...")
LIST_ITEM ("\\bu @@Sound: To Pitch (cc)...")
LIST_ITEM ("\\bu @@Sound: To Pitch (shs)...")
LIST_ITEM ("\\bu @@Sound: To Harmonicity (ac)...")
LIST_ITEM ("\\bu @@Sound: To Harmonicity (cc)...")
LIST_ITEM ("\\bu @@Sound: To PointProcess (periodic, cc)...")
LIST_ITEM ("\\bu @@Sound: To PointProcess (periodic, peaks)...")
LIST_ITEM ("\\bu @@Sound & Pitch: To PointProcess (cc)")
LIST_ITEM ("\\bu @@Sound & Pitch: To PointProcess (peaks)...")
LIST_ITEM ("\\bu @@Sound: To Intensity...")
NORMAL ("Spectral analysis:")
LIST_ITEM ("\\bu @@Sound: To Spectrum...")
LIST_ITEM ("\\bu @@Sound: To Spectrogram...")
LIST_ITEM ("\\bu @@Sound: To Formant (burg)...")
LIST_ITEM ("\\bu @@Sound: To Formant (sl)...")
LIST_ITEM ("\\bu @@Sound: LPC analysis")
LIST_ITEM1 ("\\bu @@Sound: To LPC (autocorrelation)...")
LIST_ITEM1 ("\\bu @@Sound: To LPC (covariance)...")
LIST_ITEM1 ("\\bu @@Sound: To LPC (burg)...")
LIST_ITEM1 ("\\bu @@Sound: To LPC (marple)...")
NORMAL ("Filtering (see @Filtering tutorial):")
LIST_ITEM ("\\bu @@Sound: Filter (pass Hann band)...")
LIST_ITEM ("\\bu @@Sound: Filter (stop Hann band)...")
LIST_ITEM ("\\bu @@Sound: Filter (formula)...")
LIST_ITEM ("\\bu @@Sound: Filter (one formant)...")
LIST_ITEM ("\\bu @@Sound: Filter (pre-emphasis)...")
LIST_ITEM ("\\bu @@Sound: Filter (de-emphasis)...")
LIST_ITEM ("\\bu @@LPC & Sound: Filter...")
LIST_ITEM ("\\bu @@LPC & Sound: Filter (inverse)")
LIST_ITEM ("\\bu @@Sound & Formant: Filter")
LIST_ITEM ("\\bu @@Sound & FormantTier: Filter")
NORMAL ("Conversion:")
LIST_ITEM ("\\bu @@Sound: Resample...")
NORMAL ("Enhancement:")
LIST_ITEM ("\\bu @@Sound: Lengthen (PSOLA)...@: lengthen by a constant factor")
LIST_ITEM ("\\bu @@Sound: Deepen band modulation...@: strenghten intensity modulations in each critical band")
NORMAL ("Combination:")
LIST_ITEM ("\\bu @@Sounds: Convolve")
LIST_ITEM ("\\bu @@Sounds: Concatenate")
NORMAL ("Synthesis")
LIST_ITEM ("\\bu @@Source-filter synthesis@ tutorial")
LIST_ITEM ("\\bu @Manipulation (@PSOLA etc.)")
LIST_ITEM ("\\bu @@Spectrum: To Sound")
LIST_ITEM ("\\bu ##Pitch: To Sound...")
LIST_ITEM ("\\bu @@PointProcess: To Sound (pulse train)...")
LIST_ITEM ("\\bu @@PointProcess: To Sound (hum)...")
LIST_ITEM ("\\bu ##Pitch & PointProcess: To Sound...")
LIST_ITEM ("\\bu @@Articulatory synthesis@ tutorial")
LIST_ITEM ("\\bu @@Artword & Speaker: To Sound...")
ENTRY ("Inside a Sound")
NORMAL ("With @Inspect, you will see that a Sound contains the following data:")
TAG ("%x__%min_")
DEFINITION ("starting time, in seconds.")
TAG ("%x__%max_ > %x__%min_")
DEFINITION ("end time, in seconds.")
TAG ("%n__%x_")
DEFINITION ("the number of samples (\\>_ 1).")
TAG ("%dx")
DEFINITION ("sample period, in seconds. The inverse of the sampling frequency (in Hz).")
TAG ("%x__1_")
DEFINITION ("the time associated with the first sample (in seconds). "
	"This will normally be in the range [%xmin, %xmax]. "
	"The time associated with the last sample (i.e., %x__1_ + (%nx \\-- 1) %dx)) "
	"will also normally be in that range. Mostly, the sound starts at %t = 0 seconds "
	"and %x__1_ = %dx / 2. Also, usually, %x__%max_ = %n__%x_ %dx.")
TAG ("%z [1] [1..%n__%x_]")
DEFINITION ("the amplitude of the sound (stored as single-precision floating-point numbers). "
	"For the most common applications (playing and file I-O), Praat assumes that "
	"the amplitude is greater than -1 and less than +1. "
	"For some applications (modelling of the inner ear; "
	"articulatory synthesis), Praat assumes that the amplitude is expressed in Pascal units. "
	"If these interpretations are combined, we see that the maximum peak amplitude of "
	"a calibrated sound is 1 Pascal; for a sine wave, this means 91 dB SPL.")
ENTRY ("Limitations")
NORMAL ("Since the Sound object completely resides in memory, its size is limited "
	"to the amount of RAM in your computer. For sounds longer than a few minutes, "
	"you could use the @LongSound object instead, which you can view in the @LongSoundEditor.")
MAN_END

MAN_BEGIN ("Sound: De-emphasize (in-line)...", "ppgb", 20030309)
INTRO ("A command to change the spectral slope of every selected @Sound object.")
NORMAL ("The reverse of @@Sound: Pre-emphasize (in-line)...@. For an example, see @@Source-filter synthesis@.")
NORMAL ("This is the in-line version of @@Sound: Filter (de-emphasis)...@, "
	"i.e., it does not create a new Sound object but modifies an existing object.")
ENTRY ("Argument")
TAG ("%%From frequency% (Hz)")
DEFINITION ("the frequency %F above which the spectral slope will decrease by 6 dB/octave.")
ENTRY ("Algorithm")
NORMAL ("The de-emphasis factor %\\al is computed as")
FORMULA ("%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL ("where \\De%t is the sampling period of the sound. Every sample %x__%i_ of the sound, "
	"except %x__1_, is then changed, going up from the second sample:")
FORMULA ("%x__%i_ = %x__%i_ + %\\al %x__%i-1_")
MAN_END

MAN_BEGIN ("Sound: Deepen band modulation...", "ppgb", 20030916)
INTRO ("A command to enhance the fast spectral changes, like %F__2_ movements, in each selected @Sound object.")
ENTRY ("Arguments")
TAG ("%Enhancement (dB)")
DEFINITION ("the maximum increase in the level within each critical band. The standard value is 20 dB.")
TAG ("%%From frequency% (Hz)")
DEFINITION ("the lowest frequency that shall be manipulated. The bottom frequency of the first critical band that is to be enhanced. "
	"The standard value is 300 Hertz.")
TAG ("%%To frequency% (Hz)")
DEFINITION ("the highest frequency that shall be manipulated (the last critical band may be narrower than the others). The standard value is 8000 Hz.")
TAG ("%%Slow modulation% (Hz)")
DEFINITION ("the frequency %f__%slow_ below which the intensity modulations in the bands should not be expanded. The standard value is 3 Hz.")
TAG ("%%Fast modulation% (Hz)")
DEFINITION ("the frequency %f__%fast_ above which the intensity modulations in the bands should not be expanded. The standard value is 30 Hz.")
TAG ("%%Band smoothing% (Hz)")
DEFINITION ("the degree of overlap of each band into its adjacent bands. Prevents %ringing. The standard value is 100 Hz.")
ENTRY ("Algorithm")
NORMAL ("Suppose we have the standard settings of the arguments. The resulting sound will composed of the unfiltered part of the original sound, "
	"plus all manipulated bands.")
NORMAL ("First, the resulting sound becomes the original sound, stop-band filtered between 300 and 8000 Hz: "
	"after a forward Fourier transform, all values in the @Spectrum at frequencies between 0 and 200 Hz and "
	"between 8100 Hz and the Nyquist frequency of the sound are retained unchanged. "
	"The spectral values at frequencies between 400 and 7900 Hz are set to zero. "
	"Between 200 and 400 Hz and between 7900 and 8100 Hz, the values are multiplied by a raised sine, "
	"so as to give a smooth transition without ringing in the time domain (the raised sine also allows us to view "
	"the spectrum as a sum of spectral bands). Finally, a backward Fourier transform gives us the filtered sound.")
NORMAL ("The remaining part of the spectrum is divided into %%critical bands%, i.e. frequency bands one Bark wide. "
	"For instance, the first critical band run from 300 to 406 Hz, the second from 406 to 520 Hz, and so on. "
	"Each critical band is converted to a pass-band filtered sound by means of the backward Fourier transform.")
NORMAL ("Each filtered sound will be manipulated, and the resulting manipulated sounds are added to the stop-band filtered sound "
	"we created earlier. If the manipulation is the identity transformation, the resulting sound will be equal to the original sound. "
	"But, of course, the manipulation does something different. Here are the steps.")
NORMAL ("First, we compute the local intensity of the filtered sound %x (%t):")
FORMULA ("%intensity (%t) = 10 log__10_ (%x^2 (%t) + 10^^-6^)")
NORMAL ("This intensity is subjected to a forward Fourier transform. In the frequency domain, we administer a band filter. "
	"We want to enhance the intensity modulation in the range between 3 and 30 Hz. "
	"We can achieve this by comparing the very smooth intensity contour, low-pass filtered at %f__%slow_ = 3 Hz, "
	"with the intensity contour that has enough temporal resolution to see the place-discriminating %F__2_ movements, "
	"which is low-pass filtered at %f__%fast_ = 30 Hz. In the frequency domain, the filter is")
FORMULA ("%H (%f) = exp (- (%\\al%f / %f__%fast_)^2) - exp (- (%\\al%f / %f__%slow_)^2)")
NORMAL ("where %\\al equals \\Vrln 2 \\~~ 1 / 1.2011224, so that %H (%f) has its -6 dB points at %f__%slow_ and %f__%fast_:")
PICTURE (5, 3, draw_SoundDeepen_filter)
NORMAL ("Now, why do we use such a flat filter? Because a steep filter would show ringing effects in the time domain, "
	"dividing the sound into 30-ms chunks. If our filter is a sum of exponentials in the frequency domain, it will also "
	"be a sum of exponentials in the time domain. The backward Fourier transform of the frequency response %H (%f) "
	"is the impulse response %h (%t). It is given by")
FORMULA ("%h (%t) = 2%\\pi\\Vr%\\pi %f__%fast_/%\\al exp (-(%\\pi%t%f__%fast_/%\\al)^2) - "
	"2%\\pi\\Vr%\\pi %f__%slow_/%\\al exp (-(%\\pi%t%f__%slow_/%\\al)^2)")
NORMAL ("This impulse response behaves well:")
PICTURE (5, 3, draw_SoundDeepen_impulse)
NORMAL ("We see that any short intensity peak will be enhanced, and that this enhancement will suppress the intensity "
	"around 30 milliseconds from the peak. Non-Gaussian frequency-domain filters would have given several maxima and minima "
	"in the impulse response, clearly an undesirable phenomenon.")
NORMAL ("After the filtered band is subjected to a backward Fourier transform, we convert it into power again:")
FORMULA ("%power (%t) = 10^^%filtered / 2^")
NORMAL ("The relative enhancement has a maximum that is smoothly related to the basilar place:")
FORMULA ("%ceiling = 1 + (10^^%enhancement / 20^ - 1) \\.c (1/2 - 1/2 cos (%\\pi %f__%midbark_ / 13))")
NORMAL ("where %f__%midbark_ is the mid frequency of the band. Clipping is implemented as")
FORMULA ("%factor (%t) = 1 / (1 / %power (%t) + 1 / %ceiling)")
NORMAL ("Finally, the original filtered sound %x (%t), multiplied by this factor, is added to the output.")
/*
form Consonantversterker
	positive Enhancement_(dB) 20
	choice Algoritme: 3
		button (1) Luide stukken spreiden in de tijd
		button (2) Versterken bij beweging
		button (3) Intensiteitscontour filteren
	comment Al deze algoritmen werken binnen alle frequentiebanden!
endform
smooth = 100
low = 300
high = 8000
maximumFactor = 10^(Enhancement/20)

Copy... original

# Remove filter bands.
To Spectrum (fft)
Formula... if x<'low'-'smooth' then self else
... if x<'low'+'smooth' then self*(0.5+0.5*cos(pi*(x-('low'-'smooth'))/(2*'smooth'))) else
... if x<'high'-'smooth' then 0 else
... if x<'high'+'smooth' then self*(0.5-0.5*cos(pi*(x-('high'-'smooth'))/(2*'smooth'))) else
... self fi fi fi fi
To Sound (fft)
Rename... enhanced

min = low
while min < high
	mid_bark = hertzToBark (min) + 0.5
	max = round (barkToHertz (hertzToBark (min) + 1))
	if max > high
		max = high
	endif
	call enhance_one_band
	min = max
endwhile

select Spectrum original
plus Sound original
Remove

select Sound enhanced
Rename... 'Algoritme'_'Enhancement'
Scale... 0.99

procedure enhance_one_band
	select Sound original
	To Spectrum (fft)
	Formula... if x<'min'-'smooth'then 0 else
	... if x<'min'+'smooth' then self*(0.5-0.5*cos(pi*(x-('min'-'smooth'))/(2*'smooth'))) else
	... if x<'max'-'smooth' then self else
	... if x<'max'+'smooth' then self*(0.5+0.5*cos(pi*(x-('max'-'smooth'))/(2*'smooth'))) else
	... 0 fi fi fi fi
	Rename... band
	To Sound (fft)
if Algoritme = 1
	To Intensity... 120 0.01
	select Intensity band
	Erase all
	Viewport... 0 6 0 4
	Draw... 0 5 0 80 yes
	Down to Matrix
	Rename... factor
	Formula... if Intensity_band [col-2] > self then Intensity_band [col-2] else self fi
	Formula... if Intensity_band [col-1] > self then Intensity_band [col-1] else self fi
	Formula... if Intensity_band [col+1] > self then Intensity_band [col+1] else self fi
	Formula... if Intensity_band [col+2] > self then Intensity_band [col+2] else self fi
	Red
	Draw rows... 0 5 0 0 0 80
	Black
	Formula... self - Intensity_band []
	Formula... if self > 'Enhancement' then 'Enhancement' else self fi
	Formula... 10^(self/20)
	Viewport... 0 6 4 8
	Draw rows... 0 5 0 0 0 Enhancement
	Draw inner box
	Text top... yes Band from 'min' to 'max' Hertz
	Marks bottom... 2 yes yes no
	Marks left every... 1 5 yes yes yes
	select Intensity band
	Remove
elsif Algoritme = 2
	To Intensity... 120 0.01
	# Enhance contour
	Erase all
	Viewport... 0 6 0 4
	Draw... 0 5 0 80 yes
	Down to Matrix
	Rename... factor
	Formula... if col = 1 or col = ncol then 1 else
		... (abs (Intensity_band [col-1] - Intensity_band []) +
		... abs (Intensity_band [col+1] - Intensity_band []))*0.5 fi
	Formula... if self < 1 then 1 else if self > 'maximumFactor' then 'maximumFactor' else self fi fi
	Red
	Draw rows... 0 5 0 0 1 maximumFactor
	Black
	Text top... yes Band from 'min' to 'max' Hertz
	Marks right every... 1 1 yes yes yes
	select Intensity band
	Remove
elsif Algoritme = 3
	Copy... intensity
	Formula... self^2+1e-6
Formula... 10*log10(self)
	To Spectrum (fft)
	Rename... intensityfilt
	Formula... self*exp(-(x/35)^2)-self*exp(-(x/3.5)^2); if x<3 or x>30 then 0 else self fi;
	To Sound (fft)
	Erase all
	Viewport... 0 6 0 4
	Text top... no Band van 'min' tot 'max' Hertz
	Draw... 0 5 -10 10 yes
	Down to Matrix
	Rename... factor
	Formula... 10^(Sound_intensityfilt[]/2)
ceiling = 1+(maximumFactor -1)*(0.5-0.5*cos(pi*mid_bark/13))
	Formula... 1/(1/self+1/'ceiling')
	Viewport... 0 6 4 8
	Draw rows... 0 5 0 0 0 ceiling
	Draw inner box
	Marks bottom... 2 yes yes no
	Marks left every... 1 1 yes yes yes
	select Sound intensity
	plus Spectrum intensityfilt
	plus Sound intensityfilt
	Remove
endif
	select Sound band
	Formula... self*Matrix_factor()

	select Sound enhanced
	Formula... self+Sound_band[]

	select Matrix factor
	plus Spectrum band
	plus Sound band
	Remove
endproc
*/
MAN_END

/*
Sound: Filter (freq. domain):
>> Nog iets anders. Ik heb het een tijdje geleden met jou gehad over 'troep'
>> die met jouw eenvoudige filtertechniek in geluidsfiles terechtkomt. In de
>> attachment zit een (synthetische) spraakuiting met hier en daar een (echte)
>> stilte erin. Filter die maar eens met een 'telefoonfilter'(alles 0 voor
>> x<300 or x>3400). Hier in Nijmegen krijg ik er dan een hele vieze zoemtoon
>> in, die ook duidelijk te zien is in de stilten van het signaal.
>
>Je bedoelt met de formule if x<300 or x>3400 then 0 else self fi ?

Ja


>De top van de 300-Hz-ringing is bij mij -40 dB bij 0.54 seconde,
>en -46 dB bij 1.89 seconde. Hoort dat niet?
*/

MAN_BEGIN ("Sound: Filter (de-emphasis)...", "ppgb", 20030309)
INTRO ("A command to filter every selected @Sound object. The resulting Sound object has a lower spectral slope.")
NORMAL ("The reverse of @@Sound: Filter (pre-emphasis)...@. For an example, see @@Source-filter synthesis@.")
ENTRY ("Argument")
TAG ("%%From frequency% (Hz)")
DEFINITION ("the frequency %F above which the spectral slope will decrease by 6 dB/octave.")
ENTRY ("Algorithm")
NORMAL ("The de-emphasis factor %\\al is computed as")
FORMULA ("%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL ("where \\De%t is the sampling period of the sound. The new sound %y is then computed recursively as:")
FORMULA ("%y__1_ = %x__1_")
FORMULA ("%y__%i_ = %x__%i_ + %\\al %y__%i-1_")
MAN_END

MAN_BEGIN ("Sound: Filter (formula)...", "ppgb", 20041123)
INTRO ("A command to convert every selected @Sound object into a filtered sound.")
NORMAL ("The filtering is done in the frequency domain. This command is equivalent to the following sequence:")
LIST_ITEM ("1. @@Sound: To Spectrum...@ yes")
LIST_ITEM ("2. @@Matrix: Formula...")
LIST_ITEM ("3. @@Spectrum: To Sound")
NORMAL ("For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL ("The example formula is the following:")
CODE ("if x<500 or x>1000 then 0 else self fi; rectangular band")
NORMAL ("This formula represents a rectangular pass band between 500 Hz and 1000 Hz (%x is the frequency). "
	"Rectangular bands are %not recommended, since they may lead to an appreciable amount of %#ringing in the time domain. "
	"The transition between stop and pass band should be smooth, as e.g. in @@Sound: Filter (pass Hann band)...@.")
MAN_END

MAN_BEGIN ("Sound: Filter (one formant)...", "ppgb", 20030309)
INTRO ("A command to filter every selected @Sound object, with a single formant of a specified frequency and bandwidth.")
ENTRY ("Algorithm")
NORMAL ("Two recursive filter coefficients are computed as follows:")
FORMULA ("%p = \\--2 exp (\\--%\\pi %bandwidth %dt) cos (2%\\pi %frequency %dt)")
FORMULA ("%q = exp (\\--2%\\pi %bandwidth %dt)")
NORMAL ("where %dt is the sample period. The new signal %y is then computed from the old signal %x and itself as")
FORMULA ("%y__1_ := %x__1_")
FORMULA ("%y__2_ := %x__2_ \\-- %p %y__1_")
FORMULA ("\\At%n \\>_ 3:   %y__n_ := %x__n_ \\-- %p %y__%n-1_ \\-- %q %y__%n-2_")
NORMAL ("After filtering, the sound %y is scaled so that its absolute extremum is 0.9.")
NORMAL ("For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL ("This filter has an in-line version: @@Sound: Filter with one formant (in-line)...@.")
MAN_END

MAN_BEGIN ("Sound: Filter with one formant (in-line)...", "ppgb", 20030309)
INTRO ("A command to filter every selected @Sound object in-line, with a single formant of a specified frequency and bandwidth.")
NORMAL ("This is the in-line version of @@Sound: Filter (one formant)...@, "
	"i.e. it does not create a new Sound object but modifies the selected object.")
MAN_END

MAN_BEGIN ("Sound: Filter (pass Hann band)...", "ppgb", 20041123)
INTRO ("A command to convert every selected @Sound object into a filtered sound.")
NORMAL ("The filtering is done in the frequency domain. This command is equivalent to the following sequence:")
LIST_ITEM ("1. @@Sound: To Spectrum...")
LIST_ITEM ("2. @@Spectrum: Filter (pass Hann band)...")
LIST_ITEM ("3. @@Spectrum: To Sound")
NORMAL ("For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL ("For a complementary filter, see @@Sound: Filter (stop Hann band)...@.")
MAN_END

MAN_BEGIN ("Sound: Filter (stop Hann band)...", "ppgb", 20041123)
INTRO ("A command to convert every selected @Sound object into a filtered sound.")
NORMAL ("The filtering is done in the frequency domain. This command is equivalent to the following sequence:")
LIST_ITEM ("1. @@Sound: To Spectrum...")
LIST_ITEM ("2. @@Spectrum: Filter (stop Hann band)...")
LIST_ITEM ("3. @@Spectrum: To Sound")
NORMAL ("For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL ("For a complementary filter, see @@Sound: Filter (pass Hann band)...@.")
MAN_END

MAN_BEGIN ("Sound: Formula...", "ppgb", 20021206)
INTRO ("A command for changing the data in all selected @Sound objects.")
NORMAL ("See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN ("Sound: Get absolute extremum...", "ppgb", 20040711)
INTRO ("A @query to the selected @Sound object.")
ENTRY ("Return value")
NORMAL ("the absolute extremum (in Pascal) within a specified time window.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN ("Sound: Get energy...", "ppgb", 20041107)
INTRO ("A @query to the selected @Sound object.")
ENTRY ("Return value")
NORMAL ("the energy. If the unit of sound amplitude is Pa (Pascal), the unit of energy will be Pa^2\\.cs.")
ENTRY ("Setting")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY ("Algorithm")
NORMAL ("The energy is defined as")
FORMULA ("\\in__%%t%1_^^%%t%2^ %x^2(%t) %dt")
NORMAL ("where %x(%t) is the amplitude of the sound.")
ENTRY ("See also")
NORMAL ("For an interpretation of the energy as the sound energy in air, see @@Sound: Get energy in air@. "
	"For the power, see @@Sound: Get power...@.")
MAN_END

MAN_BEGIN ("Sound: Get energy in air", "ppgb", 20041107)
INTRO ("A @query to the selected @Sound object.")
ENTRY ("Return value")
NORMAL ("The energy in air, expressed in Joule/m^2.")
ENTRY ("Algorithm")
NORMAL ("The energy of a sound in air is defined as")
FORMULA ("1 / (%\\roc) \\in %x^2(%t) %dt")
NORMAL ("where %x(%t) is the sound pressure in units of Pa (Pascal), %\\ro is the air density "
	"(apx. 1.14 kg/m^3), and %c is the velocity of sound in air (apx. 353 m/s).")
ENTRY ("See also")
NORMAL ("For an air-independent interpretation of the energy, see @@Sound: Get energy...@. "
	"For the power, see @@Sound: Get power in air@.")
MAN_END

MAN_BEGIN ("Sound: Get intensity (dB)", "ppgb", 20041107)
INTRO ("A @query to the selected @Sound object.")
ENTRY ("Return value")
NORMAL ("the intensity in air, expressed in dB relative to the auditory threshold.")
ENTRY ("Algorithm")
NORMAL ("The intensity of a sound in air is defined as")
FORMULA ("10 log__10_ { 1 / (%T %P__0_^2) \\in%dt %x^2(%t) }")
NORMAL ("where %x(%t) is the sound pressure in units of Pa (Pascal), %T is the duration of the sound, "
	"and %P__0_ = 2\\.c10^^-5^ Pa is the auditory threshold pressure.")
ENTRY ("See also")
NORMAL ("For the intensity in Watt/m^2, see @@Sound: Get power in air@. For an auditory intensity, "
	"see @@Excitation: Get loudness@.")
MAN_END

MAN_BEGIN ("Sound: Get maximum...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the maximum amplitude (sound pressure in Pascal) within a specified time window.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN ("Sound: Get mean...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the mean amplitude (sound pressure in Pascal) within a specified time range.")
ENTRY ("Setting")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY ("Mathematical definition")
NORMAL ("The mean amplitude between the times %t__1_ and %t__2_ is defined as")
FORMULA ("1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %x(%t) %dt")
NORMAL ("where %x(%t) is the amplitude of the sound in Pa.")
MAN_END

MAN_BEGIN ("Sound: Get minimum...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the minimum amplitude (sound pressure in Pascal) within a specified time window.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN ("Sound: Get nearest zero crossing...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the time associated with the zero crossing nearest to a specified time point. "
	"It is @undefined if there are no zero crossings or if the specified time is outside the time domain of the sound. "
	"Linear interpolation is used between sample points.")
ENTRY ("Setting")
TAG ("%Time (s)")
DEFINITION ("the time for which you want to get the time of the nearest zero crossing.")
MAN_END

MAN_BEGIN ("Sound: Get power...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the power within a specified time window. "
	"If the unit of sound amplitude is Pa (Pascal), the unit of power will be Pa^2.")
ENTRY ("Setting")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY ("Mathematical definition")
NORMAL ("The power is defined as")
FORMULA ("1/(%t__2_-%t__1_)  \\in__%%t%1_^^%%t%2^ %x^2(%t) %dt")
NORMAL ("where %x(%t) is the amplitude of the sound.")
ENTRY ("See also")
NORMAL ("For an interpretation of the power as the sound power in air, see @@Sound: Get power in air@. "
	"For the total energy, see @@Sound: Get energy...@.")
MAN_END

MAN_BEGIN ("Sound: Get power in air", "ppgb", 20041107)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the power in air, expressed in Watt/m^2.")
ENTRY ("Mathematical definition")
NORMAL ("The power of a sound in air is defined as")
FORMULA ("1 / (%\\rocT) \\in %x^2(%t) %dt")
NORMAL ("where %x(%t) is the sound pressure in units of Pa (Pascal), %\\ro is the air density "
	"(apx. 1.14 kg/m^3), %c is the velocity of sound in air (apx. 353 m/s), and %T is the duration of the sound.")
NORMAL ("For an air-independent interpretation of the power, see @@Sound: Get power...@. "
	"For the energy, see @@Sound: Get energy in air@. For the intensity in dB, "
	"see @@Sound: Get intensity (dB)@.")
MAN_END

MAN_BEGIN ("Sound: Get root-mean-square...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the root-mean-square (rms) value of the sound pressure, expressed in Pascal.")
ENTRY ("Setting")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY ("Mathematical definition")
NORMAL ("The root-mean-square value is defined as")
FORMULA ("\\Vr { 1/(%t__2_-%t__1_)  \\in__%%t%1_^^%%t%2^ %x^2(%t) %dt }")
NORMAL ("where %x(%t) is the amplitude of the sound.")
MAN_END

MAN_BEGIN ("Sound: Get standard deviation...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the standard deviation (in Pascal) of the sound pressure within a specified window. "
	"If the sound contains less than 2 samples, the value is @undefined.")
ENTRY ("Setting")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY ("Mathematical definition")
NORMAL ("The standard deviation is defined as")
FORMULA ("1/(%t__2_-%t__1_)  \\in%__%%t%1_^^%%t%2^ (%x(%t) - %\\mu)^2 %dt")
NORMAL ("where %x(%t) is the amplitude of the sound, and %\\mu is its mean. "
	"For our discrete Sound object, the standard deviation is approximated by")
FORMULA ("1/(%n-1) \\su__%i=%m..%m+%n-1_ (%x__%i_ - %\\mu)^2")
NORMAL ("where %n is the number of sample centres between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN ("Sound: Get time of maximum...", "ppgb", 20041123)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the time (in seconds) associated with the maximum pressure in a specified time range.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN ("Sound: Get time of minimum...", "ppgb", 20041107)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the time (in seconds) associated with the minimum pressure in a specified time range.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN ("Sound: Get value at sample number...", "ppgb", 20040420)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the amplitude (sound pressure in Pascal) at a specified sample number. "
	"If the sample number is less than 1 or greater than the number of samples, the result is @undefined.")
ENTRY ("Setting")
TAG ("%%Sample number")
DEFINITION ("the sample number at which the value is to be evaluated.")
MAN_END

MAN_BEGIN ("Sound: Get value at time...", "ppgb", 20030916)
INTRO ("A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show an estimate of the amplitude (sound pressure in Pascal) at a specified time. "
	"If that time is outside the samples of the Sound, the result is equal to the value of the nearest sample; "
	"otherwise, the result is an interpolated value.")
ENTRY ("Settings")
TAG ("%Time (s)")
DEFINITION ("the time at which the value is to be evaluated.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method, see @@vector value interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN ("Sound: Lengthen (PSOLA)...", "ppgb", 20030916)
INTRO ("A command to convert each selected @Sound object into a longer new @Sound object.")
ENTRY ("Arguments")
TAG ("%%Minimum frequency% (Hz)")
DEFINITION ("the minimum pitch used in the periodicity analysis. The standard value is 75 Hz. For the voice of a young child, set this to 150 Hz."
	"The shortest voiceless interval in the PSOLA decomposition is taken as "
	"1.5 divided by %%minimum frequency%.")
TAG ("%%Maximum frequency% (Hz)")
DEFINITION ("the maximum pitch used in the periodicity analysis. The standard value is 600 Hz. For an adult male voice, set this to 300 Hz.")
TAG ("%Factor")
DEFINITION ("the factor with which the sound will be lengthened. The standard value is 1.5. If you take a value less than 1, "
	"the resulting sound will be shorter than the original. A value larger than 3 will not work.")
ENTRY ("Algorithm")
NORMAL ("Pitch-synchronous overlap-and-add.")
MAN_END

MAN_BEGIN ("audio control panel", "ppgb", 20050822)
INTRO ("Your system's way of controlling where sounds will be played, and how loud.")
NORMAL ("On Windows, double-click the loudspeaker icon in the Start bar. "
	"On MacOS X, go to System Preferences, then to Sound, then to Output Volume "
	"(you can put a loudspeaker icon in OSX's menu bar here). "
	"On HP-UX, try the ##Use internal loudspeaker...# preference in Praat's Preferences menu.")
MAN_END

MAN_BEGIN ("Sound: Play", "ppgb", 20050822)
INTRO ("A command to play @Sound objects.")
ENTRY ("Availability")
NORMAL ("You can choose this command after selecting one or more Sounds.")
ENTRY ("Purpose")
NORMAL ("To play the selected Sounds through the internal or external loudspeakers, "
	"the headphones, or the analog or digital outputs of your computer.")
ENTRY ("Behaviour")
NORMAL ("All of the Sounds selected are played, "
	"in the order in which they appear in the list. If the sampling frequency of the Sound does not match any of "
	"the system's sampling frequencies, a fast but inaccurate conversion is performed via linear interpolation.")
ENTRY ("Usage")
NORMAL ("The output level and the choice of the output device(s) depend on the settings "
	"in your @@audio control panel@.")
MAN_END

MAN_BEGIN ("Sound: Filter (pre-emphasis)...", "ppgb", 20030309)
INTRO ("A command to filter each selected @Sound object. The resulting Sound object has a higher spectral slope.")
NORMAL ("The reverse of @@Sound: Filter (de-emphasis)...@.")
ENTRY ("Argument")
TAG ("%%From frequency% (Hz)")
DEFINITION ("the frequency %F above which the spectral slope will increase by 6 dB/octave.")
ENTRY ("Algorithm")
NORMAL ("The pre-emphasis factor %\\al is computed as")
FORMULA ("%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL ("where \\De%t is the sampling period of the sound. The new sound %y is then computed as:")
FORMULA ("%y__%i_ = %x__%i_ - %\\al %x__%i-1_")
MAN_END

MAN_BEGIN ("Sound: Pre-emphasize (in-line)...", "ppgb", 20030309)
INTRO ("A command to change the spectral slope of every selected @Sound object.")
NORMAL ("The reverse of @@Sound: De-emphasize (in-line)...@.")
NORMAL ("This is the in-line version of @@Sound: Filter (pre-emphasis)...@, "
	"i.e., it does not create a new Sound object but modifies an existing object.")
ENTRY ("Algorithm")
NORMAL ("The pre-emphasis factor %\\al is computed as")
FORMULA ("%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL ("where \\De%t is the sampling period of the sound. Every sample %x__%i_ of the sound, "
	"except %x__1_, is then changed, going down from the last sample:")
FORMULA ("%x__%i_ = %x__%i_ - %\\al %x__%i-1_")
MAN_END

MAN_BEGIN ("Sound: Resample...", "ppgb", 20040330)
INTRO ("A command that creates new @Sound objects from the selected Sounds.")
ENTRY ("Purpose")
NORMAL ("High-precision resampling from any sampling frequency to any other sampling frequency.")
ENTRY ("Arguments")
TAG ("%%Sampling frequency")
DEFINITION ("the new sampling frequency, in Hertz.")
TAG ("%Precision")
DEFINITION ("the depth of the interpolation, in samples (standard is 50). "
	"This determines the quality of the interpolation used in resampling.")
ENTRY ("Algorithm")
NORMAL ("If %Precision is 1, the method is linear interpolation, which is inaccurate but fast.")
/* BUG */
NORMAL ("If %Precision is greater than 1, the method is sin(%x)/%x (\"%sinc\") interpolation, "
	"with a depth equal to %Precision. "
	"For higher %Precision, the algorithm is slower but more accurate.")
NORMAL ("If %%Sampling frequency% is less than the sampling frequency of the selected sound, "
	"an anti-aliasing low-pass filtering is performed prior to resampling.")
ENTRY ("Behaviour")
NORMAL ("A new Sound will appear in the list of objects, "
	"bearing the same name as the original Sound, followed by the sampling frequency. "
	"For instance, the Sound \"hallo\" will give a new Sound \"hallo_10000\".")
MAN_END

MAN_BEGIN ("Sound: Set value at sample number...", "ppgb", 20040420)
INTRO ("A command to change a specified sample of the selected @Sound object.")
ENTRY ("Settings")
TAG ("%%Sample number")
DEFINITION ("the sample whose value is to be changed. Specify any value between 1 and the number of samples in the Sound. "
	"If you specify a value outside that range, you will get an error message.")
TAG ("%%New value")
DEFINITION ("the value that is to be put into the specified sample.")
ENTRY ("Scripting")
NORMAL ("Example:")
CODE ("select Sound hallo")
CODE ("Set value at sample number... 100 1/2")
NORMAL ("This sets the value of the 100th sample to 0.5.")
MAN_END

MAN_BEGIN ("SoundEditor", "ppgb", 20030312)
INTRO ("An @@Editors|Editor@ for viewing and editing a @Sound object. "
	"Most of the functions of this editor are described in the @Intro.")
ENTRY ("The markers")
NORMAL ("To set the cursor, use the left mouse button. A horizontal line will also be shown; "
	"this line crosses the cursor line at the sound's function value. "
	"This function value is the sinc-interpolated value, and is generally different from the "
	"value that you would expect when looking at the linearly interpolated version of the sampled sound.")
NORMAL ("To select a part of the time domain, use the @@time selection@ mechanism.")
ENTRY ("Playing")
NORMAL ("To play any part of the sound, click on one of the rectangles "
	"below or above the sound window (there can be 1 to 8 of these rectangles), "
	"or choose a Play command from the View menu.")
ENTRY ("Publishing")
NORMAL ("To perform analyses on the selection, or save it to a file, "
	"create an independent Sound as a copy of the selection, "
	"by clicking on the button that will copy the selection to the List of Objects; "
	"the resulting Sound will be called \"Sound untitled\".")
ENTRY ("Editing")
LIST_ITEM ("\\bu Cut: cut the selection to the clipboard.")
LIST_ITEM ("\\bu Copy: copy the selection to the clipboard.")
LIST_ITEM ("\\bu Paste: paste the clipboard to the cursor.")
LIST_ITEM ("\\bu Zero: set the selected samples to zero.")
LIST_ITEM ("\\bu Reverse: reverse the selected part of the sound.")
NORMAL ("You can undo these commands with Undo (Command-Z).")
ENTRY ("The Group button")
NORMAL ("To synchronize a SoundEditor window with other windows that show a time signal, "
	"push the Group button in all the windows that you want to synchronize. "
	"You cannot Cut from or Paste into a synchronized SoundEditor window.")
MAN_END

MAN_BEGIN ("SoundRecorder", "ppgb", 20050822)
INTRO ("With the Praat SoundRecorder window you can record a mono or stereo sound "
	"for subsequent viewing and analysis in Praat. "
	"The SoundRecorder appears on your screen if you choose @@Record mono Sound...@ or @@Record stereo Sound...@ "
	"from the @@New menu@.")
NORMAL ("Depending on your system, the SoundRecorder window may allow you to choose "
	"the sampling frequency, the input gain, and the input device (microphone, line, or digital). "
	"The sound input level is monitored continuously with one or two meters. "
	"The resulting sound has 16 bits per sample, like sounds on an audio CD.")
ENTRY ("Usage")
NORMAL ("To record the sound, use the Record and Stop buttons in the SoundRecorder window. "
	"Click ##Save to list# to copy the recorded sound to the object window "
	"(or ##Save left channel to list# or ##Save right channel to list# "
	"to copy the left or right channel if you have a stereo sound). "
	"The name of the resulting Sound object will be taken from the text field next to the button clicked.")
ENTRY ("Size of the recording buffer")
NORMAL ("The size of the recording buffer determines how many seconds of sound you can record. "
	"For instance, if the recording buffer is 20 megabytes (the standard value), you can record 220 seconds in stereo "
	"(440 seconds in mono) at a sampling frequency of 22050 Hz, "
	"or 110 seconds in stereo (220 seconds in mono) at a sampling frequency of 44100 Hz. "
	"You can change the size of the recording buffer "
	"with ##Sound input prefs...# from the Preferences menu.")
#if defined (macintosh) && ! defined (__MACH__)
NORMAL ("On Macintosh System 8 or 9 with virtual memory switched off, however, "
	"the buffer always fills the available temporary memory, i.e. the size of the largest unused block "
	"reported by \"About this computer\" in the Finder; to maximize this, quit some applications other than P\\s{RAAT}.")
#endif
NORMAL ("If you recorded a very long sound, it is probable that you cannot copy it to the list of objects. "
	"In such a case, you can still write the sound to disk with one of the Write commands in the File menu. "
	"You can then open such a long sound file in P\\s{RAAT} with @@Open long sound file...@ from the Read menu.")
#ifdef sgi
ENTRY ("Recording sounds on SGI")
NORMAL ("On SGI (Indigo, Indy, O2), you can record from microphone, line input, or digital input. "
	"Changes in most of the audio settings (sampling frequency, gain, input device) are reflected immediately in your system's Audio Control Panel; "
	"you can also change the settings from the Audio Control Panel: the SoundRecorder window will follow suit.")
#endif
#ifdef macintosh
#ifdef __MACH__
ENTRY ("Recording sounds on MacOS X")
NORMAL ("You can record from the combined microphone / line input. On some computers, these are separate.")
NORMAL ("Note that in MacOS X you cannot record from the internal CD. This is because the system provides you with something better. "
	"If you open the CD in the Finder, you will see the audio tracks as AIFC files! "
	"To open these audio tracks in P\\s{RAAT}, use @@Read from file...@ or @@Open long sound file...@.")
#else
ENTRY ("Recording sounds on Macintosh")
NORMAL ("You can record from the combined microphone / line input (on some computers, these are separate), "
	"or from the internal CD. From MacOS 8.1 on, changes in your choice of input device are reflected "
	"immediately in your Monitors & Sound control panel; "
	"you can also change the settings from this control panel: the SoundRecorder window will follow suit.")
#endif
#endif
#ifdef _WIN32
ENTRY ("Recording sounds in Windows")
NORMAL ("In Windows, you can choose your input device with the help of the recording mixer that is supplied by Windows or comes with "
	"your sound card. There will usually be a loudspeaker icon in the Start bar; double-click it, and you will see the %playing mixer "
	"(if there is no loudspeaker icon, go to ##Control Panels#, then ##Sounds and Audio Devices#, then #Volume, then #Advanced; "
	"do not blame us for how Windows XP works). In the playing mixer, choose #Properties from the #Option menu, then click #Recording, "
	"then #OK. You are now in the %recording mixer.")
NORMAL ("You can watch the input level only while recording.")
#endif
ENTRY ("The File menu")
NORMAL ("A very long recorded sound can be too big to copy it to the list of objects. "
	"Fortunately, the File menu contains commands to save the recording (or just the left or right channel) "
	"to a sound file on disk, so that you will never have to lose your recording.")
ENTRY ("Sound pressure calibration")
NORMAL ("Your computer's sound-recording software returns integer values between -32768 and 32767. "
	"Praat divides them by 32768 before putting them into a Sound object, "
	"so that the values in the Sound objects are always between -1 and +1.")
NORMAL ("The Praat program considers these numbers to be air pressures in units of Pascal, "
	"but they are probably not the actual true air pressures that went into the microphone. "
	"For how to obtain the true air pressures, perform a @@sound pressure calibration@.")
MAN_END

MAN_BEGIN ("Sounds: Concatenate", "ppgb", 20000123)
INTRO ("A command to concatenate all selected @Sound objects into a single large Sound.")
NORMAL ("All sounds must have equal sampling frequencies. "
	"They are concatenated in the order that they appear in the list of objects.")
ENTRY ("How to concatenate directly to a file")
NORMAL ("If the resulting sound does not fit into memory, use one of the "
	"commands in the @@Write menu@. See @@How to concatenate sound files@.")
MAN_END

MAN_BEGIN ("Sounds: Convolve", "ppgb", 19980323)
INTRO ("A command to convolve two @Sound objects with each other.")
NORMAL ("The convolution of two time signals %A(%t) and %B(%t) is defined as")
FORMULA ("(%A * %B) (%t) \\=3 \\in %A(%\\ta) %B(%t-%\\ta) %d%\\ta")
NORMAL ("Convolution is commutative, i.e. the convolution of %A and %B equals the convolution of %B and %A.")
ENTRY ("Algorithm")
NORMAL ("Since convolution in the time domain amounts to multiplication in the frequency domain, "
	"both sounds are FFT-ed, the resulting spectra are multiplied, and the resulting product spectrum "
	"is FFT-ed back to give the convoluted sound.")
MAN_END

}

/* End of file manual_sound.c */
