/* manual_sound.cpp
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
	Sound s = Sound_createSimple (1, 100, 10);
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
	Graphics_textBottom (g, TRUE, L"Frequency %f (Hz)");
	Graphics_textLeft (g, TRUE, L"Amplitude filter %H (%f)");
	Graphics_markLeft (g, 0, TRUE, TRUE, FALSE, NULL);
	Graphics_markLeft (g, 0.5, TRUE, TRUE, TRUE, NULL);
	Graphics_markLeft (g, 1, TRUE, TRUE, FALSE, NULL);
	Graphics_markRight (g, 1, FALSE, TRUE, FALSE, L"0 dB");
	Graphics_markRight (g, 0.5, FALSE, TRUE, FALSE, L"-6 dB");
	Graphics_markBottom (g, 0, TRUE, TRUE, FALSE, NULL);
	Graphics_markBottom (g, 3, TRUE, TRUE, TRUE, NULL);
	Graphics_markBottom (g, 30, TRUE, TRUE, TRUE, NULL);
	Graphics_markBottom (g, 100, TRUE, TRUE, FALSE, NULL);
	Graphics_setColour (g, Graphics_RED);
	Sound_draw (s, g, 0, 0, 0, 1, FALSE, L"curve");
	Graphics_setColour (g, Graphics_BLACK);
	forget (s);
}

static void draw_SoundDeepen_impulse (Graphics g) {
	Sound s = Sound_create (1, -0.2, 0.2, 1000, 4e-4, -0.1998);
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
	Graphics_textBottom (g, TRUE, L"Time %t (s)");
	Graphics_textLeft (g, TRUE, L"Intensity impulse reponse %h (%t)");
	Graphics_markBottom (g, -0.2, TRUE, TRUE, FALSE, NULL);
	Graphics_markBottom (g, 0, TRUE, TRUE, TRUE, NULL);
	Graphics_markBottom (g, 0.2, TRUE, TRUE, FALSE, NULL);
	Graphics_markLeft (g, 0, TRUE, TRUE, TRUE, NULL);
	Graphics_setColour (g, Graphics_RED);
	Sound_draw (s, g, 0, 0, -100, 400, FALSE, L"curve");
	Graphics_setColour (g, Graphics_BLACK);
	forget (s);
}

void manual_sound_init (ManPages me);
void manual_sound_init (ManPages me) {

MAN_BEGIN (L"Combine to stereo", L"ppgb", 20070129)
INTRO (L"To combine two mono @Sound objects into one single stereo Sound, "
	"select the two Sounds in the list and choose ##Combine to stereo# from the ##Combine sounds# menu. "
	"A new stereo Sound will appear in the list.")
NORMAL (L"Of the two original mono Sounds, the one that is higher in the list will become the left channel "
	"of the new stereo Sound, and the one that is lower in the list will become the right channel of the new Sound.")
MAN_END

MAN_BEGIN (L"Create Sound from formula...", L"ppgb", 20070225)
INTRO (L"A command in the @@New menu@ to create a @Sound with a specified duration and sampling frequency, "
	"filled with values from a formula.")
NORMAL (L"See the @Formulas tutorial for explanations and examples.")
/*
Create Sound from formula... blok Mono 0 1 44100 if round(x*377*2) mod 2 then 0.9 else -0.9 fi

Je kunt dus heel veel maken met een scriptje:

form Blokgolf
   word Naam blok
   real Begintijd_(s) 0.0
   real Eindtijd_(s) 1.0
   positive Samplefrequentie_(Hz) 22050
   positive Frequentie_(Hz) 377
   real Amplitude 0.9
endform
Create Sound from formula... 'Naam' Mono begintijd eindtijd samplefrequentie
   ... if round(x*frequentie*2) mod 2 then amplitude else -amplitude fi
*/
MAN_END

MAN_BEGIN (L"Create Sound from tone complex...", L"ppgb", 20060202)
INTRO (L"A command in the @@New menu@ to create a @Sound as the sum of a number of sine waves "
	"with equidistant frequencies.")
ENTRY (L"Settings")
TAG (L"##Name")
DEFINITION (L"the name of the resulting Sound object.")
TAG (L"##Start time (s)")
TAG (L"##End time (s)")
DEFINITION (L"the time domain of the resulting Sound.")
TAG (L"##Sampling frequency (Hz)")
DEFINITION (L"the sampling frequency of the resulting Sound.")
TAG (L"##Phase")
DEFINITION (L"determines whether the result is a sum of %sines or a sum of %cosines, "
	"i.e., whether the zero crossings or the maxima of the components are synchronized. "
	"This choice has little perceptual consequences.")
TAG (L"##Frequency step (Hz)")
DEFINITION (L"the distance between the components. In first approximation, "
	"this is the perceived fundamental frequency.")
TAG (L"##First frequency (Hz)")
DEFINITION (L"the lowest frequency component. If you supply a value of 0, "
	"##First frequency# is taken equal to ##Frequency step#.")
TAG (L"##Ceiling (Hz)")
DEFINITION (L"the frequency above which no components are used. If you supply a value of 0 "
	"or a value above the Sound's @@Nyquist frequency@, %ceiling is taken equal to "
	"the Nyquist frequency.")
TAG (L"##Number of components")
DEFINITION (L"determines how many sinusoids are used. If you supply a value of 0 "
	"or a very high value, the maximum number of components is used, "
	"limited by #Ceiling.")
ENTRY (L"Example 1: a pulse train")
NORMAL (L"A series of pulses at regular intervals, "
	"sampled after low-pass filtering at the Nyquist frequency, "
	"can be regarded as a sum of cosine waves. For instance, a 100-Hz pulse train, "
	"sampled at 22050 Hz, can be created with:")
CODE (L"Create Sound from tone complex... train 0 1 22050 Cosine 100 0 0 0")
NORMAL (L"Supplying the value 0 for %firstFrequency yields an unshifted harmonic complex.")
ENTRY (L"Example 2: a shifted harmonic complex")
NORMAL (L"Some experiments on human pitch perception (%%residue pitch%) use "
	"a number of sinusoidal components with harmonically "
	"related frequencies that are all shifted by a constant amount.")
NORMAL (L"For instance, to get a sum of sine waves with frequencies 105 Hz, 205 Hz, and 305 Hz, "
	"you would use:")
CODE (L"Create Sound from tone complex... train 0.3 1 22050 Sine 100 105 0 3")
NORMAL (L"or")
CODE (L"Create Sound from tone complex... train 0.3 1 22050 Sine 100 105 350 0")
NORMAL (L"whichever you prefer.")
NORMAL (L"Some of these experiments are described in @@Plomp (1967)@ and @@Patterson & Wightman (1976)@.")
ENTRY (L"Algorithm")
NORMAL (L"For the `sine' phase, the resulting Sound is given by the following formula:")
NORMAL (L"%x(%t) = \\su__%i=1..%numberOfComponents_ sin (2%\\pi\\.c(%firstFrequency + "
	"(%i\\--1)\\.c%frequencyStep)\\.c%t)")
ENTRY (L"More flexibility?")
NORMAL (L"Suppose you wanted to vary the relative strengths of the frequency components. "
	"You could achieve this by creating a Sound with the command discussed here, "
	"take its Fourier transform, run a formula on the resulting Spectrum, "
	"and take the inverse Fourier transform.")
NORMAL (L"A more general approach is described shortly.")
NORMAL (L"Suppose you need a sum of sine waves with frequencies 105, 205, 305, ..., 1905 Hz, "
	"and with relative amplitudes 1, 1/2, 1/3, ..., 1/19. You could build a script that computes "
	"the various components, and add them to each other as you go along. Instead of calling 19 "
	"scripts, however, you can achieve this with the following more general script:")
CODE (L"form Add waves with decreasing amplitudes")
CODE1 (L"natural Number_of_components 19")
CODE (L"endform")
CODE (L"\\#  Create a Matrix with frequency and amplitude information in each row:")
CODE (L"Create simple Matrix... freqAndGain number_of_components 2 0")
CODE (L"Formula... if col = 1 then row * 100 + 5 else 1 / row fi")
CODE (L"\\#  Create a large Matrix with all the component sine waves:")
CODE (L"Create Matrix... components 0 1 10000 1e-4 0.5e-4 1 number_of_components number_of_components 1 1 0")
CODE (L"Formula... Matrix_freqAndGain [2] * sin (2 * pi * Matrix_freqAndGain [1] * x)")
CODE (L"\\#  Integrate:")
CODE (L"Formula... self + self [row - 1, col]")
CODE (L"\\#  Publish last row:")
CODE (L"To Sound (slice)... number_of_components")
CODE (L"Scale amplitudes... 0.99")
MAN_END

MAN_BEGIN (L"Extract one channel...", L"ppgb", 20110129)
INTRO (L"To extract the left channel of an existing stereo @Sound as a new mono Sound, "
	"select that stereo Sound in the list and choose ##Extract one channel...# from the #Convert menu, "
	"Then set the #Channel to 1 and click OK. "
	"A new mono Sound will appear in the list. For the right channel, set #Channel to 2.")
ENTRY (L"Details")
NORMAL (L"The name of the new mono Sound will be based on the name of the original stereo Sound. "
	"For instance, if the original Sound is called #hello and you extract channel 2, the new Sound will be called #hello_ch2.")
NORMAL (L"This command also works if you select more than one stereo Sound. "
	"For each of them, Praat creates a new mono Sound.")
MAN_END

MAN_BEGIN (L"Read separate channels from sound file...", L"ppgb", 20111010)
INTRO (L"A command in the @@Open menu@ of the #Objects window. "
	"You use this if you want to get the channels of a @stereo (or multi-channel) sound file "
	"as two (or more) separate @Sound objects in the list. "
	"If the file name is hello.wav, Praat will name the channels hello_ch1, hello_ch2, and so on.")
MAN_END

MAN_BEGIN (L"Record mono Sound...", L"ppgb", 20021212)
INTRO (L"A command in the @@New menu@ to record a @Sound. Creates a @SoundRecorder window, "
	"except on very old Macintoshes with 8-bit audio, where it presents a native Macintosh sound-recorder window.")
MAN_END

MAN_BEGIN (L"Record stereo Sound...", L"ppgb", 20021212)
INTRO (L"A command in the @@New menu@ to record a @Sound. Creates a @SoundRecorder window.")
MAN_END

MAN_BEGIN (L"Sound", L"ppgb", 20110131)
INTRO (L"One of the @@types of objects@ in Praat. For tutorial information, see all of the @Intro.")
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Record mono Sound...@ (from microphone or line input, with the @SoundRecorder)")
LIST_ITEM (L"\\bu @@Record stereo Sound...@")
LIST_ITEM (L"\\bu @@Create Sound from formula...@")
LIST_ITEM (L"\\bu @@Create Sound from tone complex...")
LIST_ITEM (L"\\bu @@Create Sound from gammatone...")
LIST_ITEM (L"\\bu @@Create Sound from Shepard tone...")
NORMAL (L"Opening and saving:")
LIST_ITEM (L"\\bu @@Sound files")
NORMAL (L"You can also use the text and binary (real-valued) formats for Sounds, like for any other class:")
LIST_ITEM (L"\\bu @@Save as text file...")
LIST_ITEM (L"\\bu @@Save as binary file...")
NORMAL (L"Playing:")
LIST_ITEM (L"\\bu @@Sound: Play")
LIST_ITEM (L"\\bu @@PointProcess: Hum")
LIST_ITEM (L"\\bu @@PointProcess: Play")
NORMAL (L"Viewing and editing:")
LIST_ITEM (L"\\bu @SoundEditor, @ManipulationEditor, @TextGridEditor, @PointEditor, @PitchTierEditor, "
	"@SpectrumEditor")
NORMAL (L"Queries:")
LIST_ITEM (L"structure:")
LIST_ITEM1 (L"\\bu @@time domain")
LIST_ITEM1 (L"\\bu @@Get number of samples")
LIST_ITEM1 (L"\\bu @@Get sampling period")
LIST_ITEM1 (L"\\bu @@Get sampling frequency")
LIST_ITEM1 (L"\\bu @@Get time from sample number...")
LIST_ITEM1 (L"\\bu @@Get sample number from time...")
LIST_ITEM (L"content:")
LIST_ITEM1 (L"\\bu @@Sound: Get value at time...")
LIST_ITEM1 (L"\\bu @@Sound: Get value at sample number...")
LIST_ITEM (L"shape:")
LIST_ITEM1 (L"\\bu @@Sound: Get minimum...")
LIST_ITEM1 (L"\\bu @@Sound: Get time of minimum...")
LIST_ITEM1 (L"\\bu @@Sound: Get maximum...")
LIST_ITEM1 (L"\\bu @@Sound: Get time of maximum...")
LIST_ITEM1 (L"\\bu @@Sound: Get absolute extremum...")
LIST_ITEM1 (L"\\bu @@Sound: Get nearest zero crossing...")
LIST_ITEM (L"statistics:")
LIST_ITEM1 (L"\\bu @@Sound: Get mean...")
LIST_ITEM1 (L"\\bu @@Sound: Get root-mean-square...")
LIST_ITEM1 (L"\\bu @@Sound: Get standard deviation...")
LIST_ITEM (L"energy:")
LIST_ITEM1 (L"\\bu @@Sound: Get energy...")
LIST_ITEM1 (L"\\bu @@Sound: Get power...")
LIST_ITEM (L"in air:")
LIST_ITEM1 (L"\\bu @@Sound: Get energy in air")
LIST_ITEM1 (L"\\bu @@Sound: Get power in air")
LIST_ITEM1 (L"\\bu @@Sound: Get intensity (dB)")
NORMAL (L"Modification:")
LIST_ITEM (L"\\bu @@Matrix: Formula...")
LIST_ITEM (L"\\bu @@Sound: Set value at sample number...")
LIST_ITEM (L"\\bu @@Sound: Filter with one formant (in-line)...")
LIST_ITEM (L"\\bu @@Sound: Pre-emphasize (in-line)...")
LIST_ITEM (L"\\bu @@Sound: De-emphasize (in-line)...")
NORMAL (L"Annotation (see @@Intro 7. Annotation@):")
LIST_ITEM (L"\\bu @@Sound: To TextGrid...")
NORMAL (L"Periodicity analysis:")
LIST_ITEM (L"\\bu @@Sound: To Pitch...")
LIST_ITEM (L"\\bu @@Sound: To Pitch (ac)...")
LIST_ITEM (L"\\bu @@Sound: To Pitch (cc)...")
LIST_ITEM (L"\\bu @@Sound: To Pitch (shs)...")
LIST_ITEM (L"\\bu @@Sound: To Harmonicity (ac)...")
LIST_ITEM (L"\\bu @@Sound: To Harmonicity (cc)...")
LIST_ITEM (L"\\bu @@Sound: To PointProcess (periodic, cc)...")
LIST_ITEM (L"\\bu @@Sound: To PointProcess (periodic, peaks)...")
LIST_ITEM (L"\\bu @@Sound & Pitch: To PointProcess (cc)")
LIST_ITEM (L"\\bu @@Sound & Pitch: To PointProcess (peaks)...")
LIST_ITEM (L"\\bu @@Sound: To Intensity...")
NORMAL (L"Spectral analysis:")
LIST_ITEM (L"\\bu @@Sound: To Spectrum...")
LIST_ITEM (L"\\bu @@Sound: To Spectrogram...")
LIST_ITEM (L"\\bu @@Sound: To Formant (burg)...")
LIST_ITEM (L"\\bu @@Sound: To Formant (sl)...")
LIST_ITEM (L"\\bu @@Sound: LPC analysis")
LIST_ITEM1 (L"\\bu @@Sound: To LPC (autocorrelation)...")
LIST_ITEM1 (L"\\bu @@Sound: To LPC (covariance)...")
LIST_ITEM1 (L"\\bu @@Sound: To LPC (burg)...")
LIST_ITEM1 (L"\\bu @@Sound: To LPC (marple)...")
NORMAL (L"Filtering (see @Filtering tutorial):")
LIST_ITEM (L"\\bu @@Sound: Filter (pass Hann band)...")
LIST_ITEM (L"\\bu @@Sound: Filter (stop Hann band)...")
LIST_ITEM (L"\\bu @@Sound: Filter (formula)...")
LIST_ITEM (L"\\bu @@Sound: Filter (one formant)...")
LIST_ITEM (L"\\bu @@Sound: Filter (pre-emphasis)...")
LIST_ITEM (L"\\bu @@Sound: Filter (de-emphasis)...")
LIST_ITEM (L"\\bu @@LPC & Sound: Filter...")
LIST_ITEM (L"\\bu @@LPC & Sound: Filter (inverse)")
LIST_ITEM (L"\\bu @@Sound & Formant: Filter")
LIST_ITEM (L"\\bu @@Sound & FormantGrid: Filter")
NORMAL (L"Conversion:")
LIST_ITEM (L"\\bu @@Sound: Resample...")
NORMAL (L"Enhancement:")
LIST_ITEM (L"\\bu @@Sound: Lengthen (overlap-add)...@: lengthen by a constant factor")
LIST_ITEM (L"\\bu @@Sound: Deepen band modulation...@: strenghten intensity modulations in each critical band")
NORMAL (L"Combination:")
LIST_ITEM (L"\\bu @@Sounds: Convolve...")
LIST_ITEM (L"\\bu @@Sounds: Cross-correlate...")
LIST_ITEM (L"\\bu @@Sound: Autocorrelate...")
LIST_ITEM (L"\\bu @@Sounds: Concatenate")
NORMAL (L"Synthesis")
LIST_ITEM (L"\\bu @@Source-filter synthesis@ tutorial")
LIST_ITEM (L"\\bu @Manipulation (@@overlap-add@ etc.)")
LIST_ITEM (L"\\bu @@Spectrum: To Sound")
LIST_ITEM (L"\\bu ##Pitch: To Sound...")
LIST_ITEM (L"\\bu @@PointProcess: To Sound (pulse train)...")
LIST_ITEM (L"\\bu @@PointProcess: To Sound (hum)...")
LIST_ITEM (L"\\bu ##Pitch & PointProcess: To Sound...")
LIST_ITEM (L"\\bu @@Articulatory synthesis@ tutorial")
LIST_ITEM (L"\\bu @@Artword & Speaker: To Sound...")
ENTRY (L"Inside a Sound")
NORMAL (L"With @Inspect, you will see that a Sound contains the following data:")
TAG (L"%x__%min_")
DEFINITION (L"start time, in seconds.")
TAG (L"%x__%max_ > %x__%min_")
DEFINITION (L"end time, in seconds.")
TAG (L"%n__%x_")
DEFINITION (L"the number of samples (\\>_ 1).")
TAG (L"%dx")
DEFINITION (L"sample period, in seconds. The inverse of the sampling frequency (in Hz).")
TAG (L"%x__1_")
DEFINITION (L"the time associated with the first sample (in seconds). "
	"This will normally be in the range [%xmin, %xmax]. "
	"The time associated with the last sample (i.e., %x__1_ + (%nx \\-- 1) %dx)) "
	"will also normally be in that range. Mostly, the sound starts at %t = 0 seconds "
	"and %x__1_ = %dx / 2. Also, usually, %x__%max_ = %n__%x_ %dx.")
TAG (L"%z [1] [1..%n__%x_]")
DEFINITION (L"the amplitude of the sound (stored as single-precision floating-point numbers). "
	"For the most common applications (playing and file I-O), Praat assumes that "
	"the amplitude is greater than -1 and less than +1. "
	"For some applications (modelling of the inner ear; "
	"articulatory synthesis), Praat assumes that the amplitude is expressed in Pascal units. "
	"If these interpretations are combined, we see that the maximum peak amplitude of "
	"a calibrated sound is 1 Pascal; for a sine wave, this means 91 dB SPL.")
ENTRY (L"Limitations")
NORMAL (L"Since the Sound object completely resides in memory, its size is limited "
	"to the amount of RAM in your computer. For sounds longer than a few minutes, "
	"you could use the @LongSound object instead, which you can view in the @LongSoundEditor.")
MAN_END

MAN_BEGIN (L"Sound: De-emphasize (in-line)...", L"ppgb", 20030309)
INTRO (L"A command to change the spectral slope of every selected @Sound object.")
NORMAL (L"The reverse of @@Sound: Pre-emphasize (in-line)...@. For an example, see @@Source-filter synthesis@.")
NORMAL (L"This is the in-line version of @@Sound: Filter (de-emphasis)...@, "
	"i.e., it does not create a new Sound object but modifies an existing object.")
ENTRY (L"Setting")
TAG (L"##From frequency (Hz)")
DEFINITION (L"the frequency %F above which the spectral slope will decrease by 6 dB/octave.")
ENTRY (L"Algorithm")
NORMAL (L"The de-emphasis factor %\\al is computed as")
FORMULA (L"%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL (L"where \\De%t is the sampling period of the sound. Every sample %x__%i_ of the sound, "
	"except %x__1_, is then changed, going up from the second sample:")
FORMULA (L"%x__%i_ = %x__%i_ + %\\al %x__%i-1_")
MAN_END

MAN_BEGIN (L"Sound: Deepen band modulation...", L"ppgb", 20101026)
INTRO (L"A command to enhance the fast spectral changes, like %F__2_ movements, in each selected @Sound object.")
ENTRY (L"Settings")
TAG (L"##Enhancement (dB)")
DEFINITION (L"the maximum increase in the level within each critical band. The standard value is 20 dB.")
TAG (L"##From frequency (Hz)")
DEFINITION (L"the lowest frequency that shall be manipulated. The bottom frequency of the first critical band that is to be enhanced. "
	"The standard value is 300 Hertz.")
TAG (L"##To frequency (Hz)")
DEFINITION (L"the highest frequency that shall be manipulated (the last critical band may be narrower than the others). The standard value is 8000 Hz.")
TAG (L"##Slow modulation (Hz)")
DEFINITION (L"the frequency %f__%slow_ below which the intensity modulations in the bands should not be expanded. The standard value is 3 Hz.")
TAG (L"##Fast modulation (Hz)")
DEFINITION (L"the frequency %f__%fast_ above which the intensity modulations in the bands should not be expanded. The standard value is 30 Hz.")
TAG (L"##Band smoothing (Hz)")
DEFINITION (L"the degree of overlap of each band into its adjacent bands. Prevents %ringing. The standard value is 100 Hz.")
ENTRY (L"Algorithm")
NORMAL (L"This algorithm was inspired by @@Nagarajan, Wang, Merzenich, Schreiner, Johnston, Jenkins, Miller & Tallal (1998)@, "
	"but not identical to it. Now follows the description.")
NORMAL (L"Suppose the settings have their standard values. The resulting sound will composed of the unfiltered part of the original sound, "
	"plus all manipulated bands.")
NORMAL (L"First, the resulting sound becomes the original sound, stop-band filtered between 300 and 8000 Hz: "
	"after a forward Fourier transform, all values in the @Spectrum at frequencies between 0 and 200 Hz and "
	"between 8100 Hz and the Nyquist frequency of the sound are retained unchanged. "
	"The spectral values at frequencies between 400 and 7900 Hz are set to zero. "
	"Between 200 and 400 Hz and between 7900 and 8100 Hz, the values are multiplied by a raised sine, "
	"so as to give a smooth transition without ringing in the time domain (the raised sine also allows us to view "
	"the spectrum as a sum of spectral bands). Finally, a backward Fourier transform gives us the filtered sound.")
NORMAL (L"The remaining part of the spectrum is divided into %%critical bands%, i.e. frequency bands one Bark wide. "
	"For instance, the first critical band run from 300 to 406 Hz, the second from 406 to 520 Hz, and so on. "
	"Each critical band is converted to a pass-band filtered sound by means of the backward Fourier transform.")
NORMAL (L"Each filtered sound will be manipulated, and the resulting manipulated sounds are added to the stop-band filtered sound "
	"we created earlier. If the manipulation is the identity transformation, the resulting sound will be equal to the original sound. "
	"But, of course, the manipulation does something different. Here are the steps.")
NORMAL (L"First, we compute the local intensity of the filtered sound %x (%t):")
FORMULA (L"%intensity (%t) = 10 log__10_ (%x^2 (%t) + 10^^-6^)")
NORMAL (L"This intensity is subjected to a forward Fourier transform. In the frequency domain, we administer a band filter. "
	"We want to enhance the intensity modulation in the range between 3 and 30 Hz. "
	"We can achieve this by comparing the very smooth intensity contour, low-pass filtered at %f__%slow_ = 3 Hz, "
	"with the intensity contour that has enough temporal resolution to see the place-discriminating %F__2_ movements, "
	"which is low-pass filtered at %f__%fast_ = 30 Hz. In the frequency domain, the filter is")
FORMULA (L"%H (%f) = exp (- (%\\al%f / %f__%fast_)^2) - exp (- (%\\al%f / %f__%slow_)^2)")
NORMAL (L"where %\\al equals \\Vrln 2 \\~~ 1 / 1.2011224, so that %H (%f) has its -6 dB points at %f__%slow_ and %f__%fast_:")
PICTURE (5, 3, draw_SoundDeepen_filter)
NORMAL (L"Now, why do we use such a flat filter? Because a steep filter would show ringing effects in the time domain, "
	"dividing the sound into 30-ms chunks. If our filter is a sum of exponentials in the frequency domain, it will also "
	"be a sum of exponentials in the time domain. The backward Fourier transform of the frequency response %H (%f) "
	"is the impulse response %h (%t). It is given by")
FORMULA (L"%h (%t) = 2%\\pi\\Vr%\\pi %f__%fast_/%\\al exp (-(%\\pi%t%f__%fast_/%\\al)^2) - "
	"2%\\pi\\Vr%\\pi %f__%slow_/%\\al exp (-(%\\pi%t%f__%slow_/%\\al)^2)")
NORMAL (L"This impulse response behaves well:")
PICTURE (5, 3, draw_SoundDeepen_impulse)
NORMAL (L"We see that any short intensity peak will be enhanced, and that this enhancement will suppress the intensity "
	"around 30 milliseconds from the peak. Non-Gaussian frequency-domain filters would have given several maxima and minima "
	"in the impulse response, clearly an undesirable phenomenon.")
NORMAL (L"After the filtered band is subjected to a backward Fourier transform, we convert it into power again:")
FORMULA (L"%power (%t) = 10^^%filtered / 2^")
NORMAL (L"The relative enhancement has a maximum that is smoothly related to the basilar place:")
FORMULA (L"%ceiling = 1 + (10^^%enhancement / 20^ - 1) \\.c (1/2 - 1/2 cos (%\\pi %f__%midbark_ / 13))")
NORMAL (L"where %f__%midbark_ is the mid frequency of the band. Clipping is implemented as")
FORMULA (L"%factor (%t) = 1 / (1 / %power (%t) + 1 / %ceiling)")
NORMAL (L"Finally, the original filtered sound %x (%t), multiplied by this factor, is added to the output.")
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

MAN_BEGIN (L"Sound: Filter (de-emphasis)...", L"ppgb", 20030309)
INTRO (L"A command to filter every selected @Sound object. The resulting Sound object has a lower spectral slope.")
NORMAL (L"The reverse of @@Sound: Filter (pre-emphasis)...@. For an example, see @@Source-filter synthesis@.")
ENTRY (L"Setting")
TAG (L"##From frequency (Hz)")
DEFINITION (L"the frequency %F above which the spectral slope will decrease by 6 dB/octave.")
ENTRY (L"Algorithm")
NORMAL (L"The de-emphasis factor %\\al is computed as")
FORMULA (L"%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL (L"where \\De%t is the sampling period of the sound. The new sound %y is then computed recursively as:")
FORMULA (L"%y__1_ = %x__1_")
FORMULA (L"%y__%i_ = %x__%i_ + %\\al %y__%i-1_")
MAN_END

MAN_BEGIN (L"Sound: Filter (formula)...", L"ppgb", 20041123)
INTRO (L"A command to convert every selected @Sound object into a filtered sound.")
NORMAL (L"The filtering is done in the frequency domain. This command is equivalent to the following sequence:")
LIST_ITEM (L"1. @@Sound: To Spectrum...@ yes")
LIST_ITEM (L"2. @@Matrix: Formula...")
LIST_ITEM (L"3. @@Spectrum: To Sound")
NORMAL (L"For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL (L"The example formula is the following:")
CODE (L"if x<500 or x>1000 then 0 else self fi; rectangular band")
NORMAL (L"This formula represents a rectangular pass band between 500 Hz and 1000 Hz (%x is the frequency). "
	"Rectangular bands are %not recommended, since they may lead to an appreciable amount of %#ringing in the time domain. "
	"The transition between stop and pass band should be smooth, as e.g. in @@Sound: Filter (pass Hann band)...@.")
MAN_END

MAN_BEGIN (L"Sound: Filter (one formant)...", L"ppgb", 20030309)
INTRO (L"A command to filter every selected @Sound object, with a single formant of a specified frequency and bandwidth.")
ENTRY (L"Algorithm")
NORMAL (L"Two recursive filter coefficients are computed as follows:")
FORMULA (L"%p = \\--2 exp (\\--%\\pi %bandwidth %dt) cos (2%\\pi %frequency %dt)")
FORMULA (L"%q = exp (\\--2%\\pi %bandwidth %dt)")
NORMAL (L"where %dt is the sample period. The new signal %y is then computed from the old signal %x and itself as")
FORMULA (L"%y__1_ := %x__1_")
FORMULA (L"%y__2_ := %x__2_ \\-- %p %y__1_")
FORMULA (L"\\At%n \\>_ 3:   %y__n_ := %x__n_ \\-- %p %y__%n-1_ \\-- %q %y__%n-2_")
NORMAL (L"After filtering, the sound %y is scaled so that its absolute extremum is 0.9.")
NORMAL (L"For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL (L"This filter has an in-line version: @@Sound: Filter with one formant (in-line)...@.")
MAN_END

MAN_BEGIN (L"Sound: Filter with one formant (in-line)...", L"ppgb", 20030309)
INTRO (L"A command to filter every selected @Sound object in-line, with a single formant of a specified frequency and bandwidth.")
NORMAL (L"This is the in-line version of @@Sound: Filter (one formant)...@, "
	"i.e. it does not create a new Sound object but modifies the selected object.")
MAN_END

MAN_BEGIN (L"Sound: Filter (pass Hann band)...", L"ppgb", 20041123)
INTRO (L"A command to convert every selected @Sound object into a filtered sound.")
NORMAL (L"The filtering is done in the frequency domain. This command is equivalent to the following sequence:")
LIST_ITEM (L"1. @@Sound: To Spectrum...")
LIST_ITEM (L"2. @@Spectrum: Filter (pass Hann band)...")
LIST_ITEM (L"3. @@Spectrum: To Sound")
NORMAL (L"For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL (L"For a complementary filter, see @@Sound: Filter (stop Hann band)...@.")
MAN_END

MAN_BEGIN (L"Sound: Filter (stop Hann band)...", L"ppgb", 20041123)
INTRO (L"A command to convert every selected @Sound object into a filtered sound.")
NORMAL (L"The filtering is done in the frequency domain. This command is equivalent to the following sequence:")
LIST_ITEM (L"1. @@Sound: To Spectrum...")
LIST_ITEM (L"2. @@Spectrum: Filter (stop Hann band)...")
LIST_ITEM (L"3. @@Spectrum: To Sound")
NORMAL (L"For a comparative discussion of various filtering methods, see the @Filtering tutorial.")
NORMAL (L"For a complementary filter, see @@Sound: Filter (pass Hann band)...@.")
MAN_END

MAN_BEGIN (L"Sound: Formula...", L"ppgb", 20021206)
INTRO (L"A command for changing the data in all selected @Sound objects.")
NORMAL (L"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (L"Sound: Get absolute extremum...", L"ppgb", 20040711)
INTRO (L"A @query to the selected @Sound object.")
ENTRY (L"Return value")
NORMAL (L"the absolute extremum (in Pascal) within a specified time window.")
ENTRY (L"Settings")
TAG (L"%%Time range% (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG (L"%%Interpolation")
DEFINITION (L"the interpolation method (None, Parabolic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN (L"Sound: Get energy...", L"ppgb", 20070129)
INTRO (L"A @query to the selected @Sound object.")
ENTRY (L"Return value")
NORMAL (L"the energy. If the unit of sound amplitude is Pa (Pascal), the unit of energy will be Pa^2\\.cs.")
ENTRY (L"Setting")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY (L"Algorithm")
NORMAL (L"The energy is defined as")
FORMULA (L"\\in__%%t%1_^^%%t%2^ %x^2(%t) %dt")
NORMAL (L"where %x(%t) is the amplitude of the sound. For stereo sounds, it is")
FORMULA (L"\\in__%%t%1_^^%%t%2^ (%x__1_^2(%t) + %x__2_^2(%t))/2 %dt")
NORMAL (L"where %x__1_(%t) and %x__2_(%t) are the two channels; this definition ensures that "
	"if you convert a mono sound to a stereo sound, the energy will stay the same.")
ENTRY (L"See also")
NORMAL (L"For an interpretation of the energy as the sound energy in air, see @@Sound: Get energy in air@. "
	"For the power, see @@Sound: Get power...@.")
MAN_END

MAN_BEGIN (L"Sound: Get energy in air", L"ppgb", 20070129)
INTRO (L"A @query to the selected @Sound object.")
ENTRY (L"Return value")
NORMAL (L"The energy in air, expressed in Joule/m^2.")
ENTRY (L"Algorithm")
NORMAL (L"The energy of a sound in air is defined as")
FORMULA (L"1 / (%\\roc) \\in %x^2(%t) %dt")
NORMAL (L"where %x(%t) is the sound pressure in units of Pa (Pascal), %\\ro is the air density "
	"(apx. 1.14 kg/m^3), and %c is the velocity of sound in air (apx. 353 m/s). "
	"For how stereo sounds are handled, see @@Sound: Get energy...@.")
ENTRY (L"See also")
NORMAL (L"For an air-independent interpretation of the energy, see @@Sound: Get energy...@. "
	"For the power, see @@Sound: Get power in air@.")
MAN_END

MAN_BEGIN (L"Sound: Get intensity (dB)", L"ppgb", 20070129)
INTRO (L"A @query to the selected @Sound object.")
ENTRY (L"Return value")
NORMAL (L"the intensity in air, expressed in dB relative to the auditory threshold.")
ENTRY (L"Algorithm")
NORMAL (L"The intensity of a sound in air is defined as")
FORMULA (L"10 log__10_ { 1 / (%T %P__0_^2) \\in%dt %x^2(%t) }")
NORMAL (L"where %x(%t) is the sound pressure in units of Pa (Pascal), %T is the duration of the sound, "
	"and %P__0_ = 2\\.c10^^-5^ Pa is the auditory threshold pressure. "
	"For how stereo sounds are handled, see @@Sound: Get energy...@.")
ENTRY (L"See also")
NORMAL (L"For the intensity in Watt/m^2, see @@Sound: Get power in air@. For an auditory intensity, "
	"see @@Excitation: Get loudness@.")
MAN_END

MAN_BEGIN (L"Sound: Get maximum...", L"ppgb", 20041123)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the maximum amplitude (sound pressure in Pascal) within a specified time window.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is #Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN (L"Sound: Get mean...", L"ppgb", 20041123)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the mean amplitude (sound pressure in Pascal) within a specified time range.")
ENTRY (L"Setting")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY (L"Mathematical definition")
NORMAL (L"The mean amplitude between the times %t__1_ and %t__2_ is defined as")
FORMULA (L"1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %x(%t) %dt")
NORMAL (L"where %x(%t) is the amplitude of the sound in Pa.")
MAN_END

MAN_BEGIN (L"Sound: Get minimum...", L"ppgb", 20041123)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the minimum amplitude (sound pressure in Pascal) within a specified time window.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG (L"%%Interpolation")
DEFINITION (L"the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN (L"Sound: Get nearest zero crossing...", L"ppgb", 20041123)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the time associated with the zero crossing nearest to a specified time point. "
	"It is @undefined if there are no zero crossings or if the specified time is outside the time domain of the sound. "
	"Linear interpolation is used between sample points.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time for which you want to get the time of the nearest zero crossing.")
MAN_END

MAN_BEGIN (L"Sound: Get power...", L"ppgb", 20070129)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the power within a specified time window. "
	"If the unit of sound amplitude is Pa (Pascal), the unit of power will be Pa^2.")
ENTRY (L"Setting")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY (L"Mathematical definition")
NORMAL (L"The power is defined as")
FORMULA (L"1/(%t__2_-%t__1_)  \\in__%%t%1_^^%%t%2^ %x^2(%t) %dt")
NORMAL (L"where %x(%t) is the amplitude of the sound. "
	"For how stereo sounds are handled, see @@Sound: Get energy...@.")
ENTRY (L"See also")
NORMAL (L"For an interpretation of the power as the sound power in air, see @@Sound: Get power in air@. "
	"For the total energy, see @@Sound: Get energy...@.")
MAN_END

MAN_BEGIN (L"Sound: Get power in air", L"ppgb", 20070129)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the power in air, expressed in Watt/m^2.")
ENTRY (L"Mathematical definition")
NORMAL (L"The power of a sound in air is defined as")
FORMULA (L"1 / (%\\rocT) \\in %x^2(%t) %dt")
NORMAL (L"where %x(%t) is the sound pressure in units of Pa (Pascal), %\\ro is the air density "
	"(apx. 1.14 kg/m^3), %c is the velocity of sound in air (apx. 353 m/s), and %T is the duration of the sound. "
	"For how stereo sounds are handled, see @@Sound: Get energy...@.")
NORMAL (L"For an air-independent interpretation of the power, see @@Sound: Get power...@. "
	"For the energy, see @@Sound: Get energy in air@. For the intensity in dB, "
	"see @@Sound: Get intensity (dB)@.")
MAN_END

MAN_BEGIN (L"Sound: Get root-mean-square...", L"ppgb", 20070129)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the root-mean-square (rms) value of the sound pressure, expressed in Pascal.")
ENTRY (L"Setting")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY (L"Mathematical definition")
NORMAL (L"The root-mean-square value is defined as")
FORMULA (L"\\Vr { 1/(%t__2_-%t__1_)  \\in__%%t%1_^^%%t%2^ %x^2(%t) %dt }")
NORMAL (L"where %x(%t) is the amplitude of the sound. "
	"For how stereo sounds are handled, see @@Sound: Get energy...@.")
MAN_END

MAN_BEGIN (L"Sound: Get standard deviation...", L"ppgb", 20041123)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the standard deviation (in Pascal) of the sound pressure within a specified window. "
	"If the sound contains less than 2 samples, the value is @undefined.")
ENTRY (L"Setting")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
ENTRY (L"Mathematical definition")
NORMAL (L"The standard deviation is defined as")
FORMULA (L"1/(%t__2_-%t__1_)  \\in%__%%t%1_^^%%t%2^ (%x(%t) - %\\mu)^2 %dt")
NORMAL (L"where %x(%t) is the amplitude of the sound, and %\\mu is its mean. "
	"For our discrete Sound object, the standard deviation is approximated by")
FORMULA (L"1/(%n-1) \\su__%i=%m..%m+%n-1_ (%x__%i_ - %\\mu)^2")
NORMAL (L"where %n is the number of sample centres between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN (L"Sound: Get time of maximum...", L"ppgb", 20041123)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the time (in seconds) associated with the maximum pressure in a specified time range.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is #Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN (L"Sound: Get time of minimum...", L"ppgb", 20041107)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the time (in seconds) associated with the minimum pressure in a specified time range.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the sound is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is #Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN (L"Sound: Get value at sample number...", L"ppgb", 20040420)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show the amplitude (sound pressure in Pascal) at a specified sample number. "
	"If the sample number is less than 1 or greater than the number of samples, the result is @undefined.")
ENTRY (L"Setting")
TAG (L"##Sample number")
DEFINITION (L"the sample number at which the value is to be evaluated.")
MAN_END

MAN_BEGIN (L"Sound: Get value at time...", L"ppgb", 20030916)
INTRO (L"A command available in the #Query menu if you select a @Sound object. "
	"The Info window will show an estimate of the amplitude (sound pressure in Pascal) at a specified time. "
	"If that time is outside the samples of the Sound, the result is equal to the value of the nearest sample; "
	"otherwise, the result is an interpolated value.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time at which the value is to be evaluated.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method, see @@vector value interpolation@. "
	"The standard is #Sinc70 because a Sound object is normally a sampled band-limited signal, "
	"which can be seen as a sum of sinc functions.")
MAN_END

MAN_BEGIN (L"Sound: Lengthen (overlap-add)...", L"ppgb", 20030916)
INTRO (L"A command to convert each selected @Sound object into a longer new @Sound object.")
ENTRY (L"Settings")
TAG (L"##Minimum frequency (Hz)")
DEFINITION (L"the minimum pitch used in the periodicity analysis. The standard value is 75 Hz. For the voice of a young child, set this to 150 Hz."
	"The shortest voiceless interval in the decomposition is taken as 1.5 divided by %%minimum frequency%.")
TAG (L"##Maximum frequency (Hz)")
DEFINITION (L"the maximum pitch used in the periodicity analysis. The standard value is 600 Hz. For an adult male voice, set this to 300 Hz.")
TAG (L"##Factor")
DEFINITION (L"the factor with which the sound will be lengthened. The standard value is 1.5. If you take a value less than 1, "
	"the resulting sound will be shorter than the original. A value larger than 3 will not work.")
ENTRY (L"Algorithm")
NORMAL (L"@@overlap-add@.")
MAN_END

MAN_BEGIN (L"audio control panel", L"ppgb", 20050822)
INTRO (L"Your system's way of controlling where sounds will be played, and how loud.")
NORMAL (L"On Windows, double-click the loudspeaker icon in the Start bar. "
	"On MacOS X, go to ##System Preferences#, then to #Sound, then to ##Output Volume# "
	"(you can put a loudspeaker icon in OSX's menu bar here). "
	"On HP-UX, try the ##Use internal loudspeaker...# preference in Praat's #Preferences menu.")
MAN_END

MAN_BEGIN (L"Sound: Play", L"ppgb", 20050822)
INTRO (L"A command to play @Sound objects.")
ENTRY (L"Availability")
NORMAL (L"You can choose this command after selecting one or more Sounds.")
ENTRY (L"Purpose")
NORMAL (L"To play the selected Sounds through the internal or external loudspeakers, "
	"the headphones, or the analog or digital outputs of your computer.")
ENTRY (L"Behaviour")
NORMAL (L"All of the Sounds selected are played, "
	"in the order in which they appear in the list. If the sampling frequency of the Sound does not match any of "
	"the system's sampling frequencies, a fast but inaccurate conversion is performed via linear interpolation.")
ENTRY (L"Usage")
NORMAL (L"The output level and the choice of the output device(s) depend on the settings "
	"in your @@audio control panel@.")
MAN_END

MAN_BEGIN (L"Sound: Filter (pre-emphasis)...", L"ppgb", 20030309)
INTRO (L"A command to filter each selected @Sound object. The resulting Sound object has a higher spectral slope.")
NORMAL (L"The reverse of @@Sound: Filter (de-emphasis)...@.")
ENTRY (L"Setting")
TAG (L"##From frequency (Hz)")
DEFINITION (L"the frequency %F above which the spectral slope will increase by 6 dB/octave.")
ENTRY (L"Algorithm")
NORMAL (L"The pre-emphasis factor %\\al is computed as")
FORMULA (L"%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL (L"where \\De%t is the sampling period of the sound. The new sound %y is then computed as:")
FORMULA (L"%y__%i_ = %x__%i_ - %\\al %x__%i-1_")
MAN_END

MAN_BEGIN (L"Sound: Pre-emphasize (in-line)...", L"ppgb", 20030309)
INTRO (L"A command to change the spectral slope of every selected @Sound object.")
NORMAL (L"The reverse of @@Sound: De-emphasize (in-line)...@.")
NORMAL (L"This is the in-line version of @@Sound: Filter (pre-emphasis)...@, "
	"i.e., it does not create a new Sound object but modifies an existing object.")
ENTRY (L"Algorithm")
NORMAL (L"The pre-emphasis factor %\\al is computed as")
FORMULA (L"%\\al = exp (-2 %\\pi %F \\De%t)")
NORMAL (L"where \\De%t is the sampling period of the sound. Every sample %x__%i_ of the sound, "
	"except %x__1_, is then changed, going down from the last sample:")
FORMULA (L"%x__%i_ = %x__%i_ - %\\al %x__%i-1_")
MAN_END

MAN_BEGIN (L"Sound: Resample...", L"ppgb", 20040330)
INTRO (L"A command that creates new @Sound objects from the selected Sounds.")
ENTRY (L"Purpose")
NORMAL (L"High-precision resampling from any sampling frequency to any other sampling frequency.")
ENTRY (L"Settings")
TAG (L"##Sampling frequency (Hz)")
DEFINITION (L"the new sampling frequency, in hertz.")
TAG (L"##Precision")
DEFINITION (L"the depth of the interpolation, in samples (standard is 50). "
	"This determines the quality of the interpolation used in resampling.")
ENTRY (L"Algorithm")
NORMAL (L"If #Precision is 1, the method is linear interpolation, which is inaccurate but fast.")
/* BUG */
NORMAL (L"If #Precision is greater than 1, the method is sin(%x)/%x (\"%sinc\") interpolation, "
	"with a depth equal to #Precision. "
	"For higher #Precision, the algorithm is slower but more accurate.")
NORMAL (L"If ##Sampling frequency# is less than the sampling frequency of the selected sound, "
	"an anti-aliasing low-pass filtering is performed prior to resampling.")
ENTRY (L"Behaviour")
NORMAL (L"A new Sound will appear in the list of objects, "
	"bearing the same name as the original Sound, followed by the sampling frequency. "
	"For instance, the Sound \"hallo\" will give a new Sound \"hallo_10000\".")
MAN_END

MAN_BEGIN (L"Sound: Set value at sample number...", L"ppgb", 20040420)
INTRO (L"A command to change a specified sample of the selected @Sound object.")
ENTRY (L"Settings")
TAG (L"##Sample number")
DEFINITION (L"the sample whose value is to be changed. Specify any value between 1 and the number of samples in the Sound. "
	"If you specify a value outside that range, you will get an error message.")
TAG (L"##New value")
DEFINITION (L"the value that is to be put into the specified sample.")
ENTRY (L"Scripting")
NORMAL (L"Example:")
CODE (L"select Sound hallo")
CODE (L"Set value at sample number... 100 1/2")
NORMAL (L"This sets the value of the 100th sample to 0.5.")
MAN_END

MAN_BEGIN (L"SoundEditor", L"ppgb", 20100404)
INTRO (L"An @@Editors|Editor@ for viewing and editing a @Sound object. "
	"Most of the functions of this editor are described in the @Intro.")
ENTRY (L"The markers")
NORMAL (L"To set the cursor, use the left mouse button. A short horizontal line will also be shown at the left edge (in cyan); "
	"the extension of this line crosses the cursor line at the sound's function value (seen as a cyan dot). "
	"This function value (shown at the left in cyan) is the sinc-interpolated value, and is generally different from the "
	"value that you would expect when looking at the linearly interpolated version of the sampled sound.")
NORMAL (L"To select a part of the time domain, use the @@time selection@ mechanism.")
ENTRY (L"Playing")
NORMAL (L"To play any part of the sound, click on one of the rectangles "
	"below or above the sound window (there can be 1 to 8 of these rectangles), "
	"or choose a Play command from the View menu.")
ENTRY (L"Publishing")
NORMAL (L"To perform analyses on the selection, or save it to a file, "
	"create an independent Sound as a copy of the selection, "
	"by clicking on the button that will copy the selection to the List of Objects; "
	"the resulting Sound will be called \"Sound untitled\".")
ENTRY (L"Editing")
LIST_ITEM (L"\\bu Cut: cut the selection to the clipboard, for later pasting into the same or another Sound.")
LIST_ITEM (L"\\bu Copy selection to Sound clipboard: copy the selection to the clipboard, for later pasting into the same or another Sound.")
LIST_ITEM (L"\\bu Paste after selection: paste the clipboard to the cursor or after the selection.")
LIST_ITEM (L"\\bu Set selection to zero: set the selected samples to zero.")
LIST_ITEM (L"\\bu Reverse selection: reverse the selected part of the sound.")
NORMAL (L"You can undo these commands with Undo (Command-Z).")
ENTRY (L"The Group button")
NORMAL (L"To synchronize a SoundEditor window with other windows that show a time signal, "
	"push the Group button in all the windows that you want to synchronize. "
	"You cannot Cut from or Paste into a synchronized SoundEditor window.")
MAN_END

MAN_BEGIN (L"SoundRecorder", L"ppgb", 20110129)
INTRO (L"With the Praat SoundRecorder window you can record a mono or stereo sound "
	"for subsequent viewing and analysis in Praat. "
	"The SoundRecorder appears on your screen if you choose @@Record mono Sound...@ or @@Record stereo Sound...@ "
	"from the @@New menu@.")
NORMAL (L"Depending on your system, the SoundRecorder window may allow you to choose "
	"the sampling frequency, the input gain, and the input device (microphone, line, or digital). "
	"The sound input level is monitored continuously with one or two meters. "
	"The resulting sound has 16 bits per sample, like sounds on an audio CD.")
ENTRY (L"Usage")
NORMAL (L"To record the sound, use the Record and Stop buttons in the SoundRecorder window. "
	"Click ##Save to list# to copy the recorded sound to the object window "
	"(or ##Save left channel to list# or ##Save right channel to list# "
	"to copy the left or right channel if you have a stereo sound). "
	"The name of the resulting Sound object will be taken from the text field next to the button clicked.")
ENTRY (L"Size of the recording buffer")
NORMAL (L"The size of the recording buffer determines how many seconds of sound you can record. "
	"For instance, if the recording buffer is 20 megabytes (the standard value), you can record 220 seconds in stereo "
	"(440 seconds in mono) at a sampling frequency of 22050 Hz, "
	"or 110 seconds in stereo (220 seconds in mono) at a sampling frequency of 44100 Hz. "
	"You can change the size of the recording buffer "
	"with ##Sound input prefs...# from the Preferences menu.")
NORMAL (L"If you recorded a very long sound, it is probable that you cannot copy it to the list of objects. "
	"In such a case, you can still write the sound to disk with one of the #Save commands in the #File menu. "
	"You can then open such a long sound file in Praat with @@Open long sound file...@ from the Open menu.")
#ifdef macintosh
ENTRY (L"Recording sounds on MacOS X")
NORMAL (L"You can record from the combined microphone / line input. On some computers, these are separate.")
NORMAL (L"Note that in MacOS X you cannot record from the internal CD. This is because the system provides you with something better. "
	"If you open the CD in the Finder, you will see the audio tracks as AIFC files! "
	"To open these audio tracks in Praat, use @@Read from file...@ or @@Open long sound file...@.")
#endif
#ifdef _WIN32
ENTRY (L"Recording sounds in Windows")
NORMAL (L"In Windows, you can choose your input device with the help of the recording mixer that is supplied by Windows or comes with "
	"your sound card. There will usually be a loudspeaker icon in the Start bar; double-click it, and you will see the %playing mixer "
	"(if there is no loudspeaker icon, go to ##Control Panels#, then ##Sounds and Audio Devices#, then #Volume, then #Advanced; "
	"do not blame us for how Windows XP works). In the playing mixer, choose #Properties from the #Option menu, then click #Recording, "
	"then #OK. You are now in the %recording mixer.")
NORMAL (L"You can watch the input level only while recording.")
#endif
ENTRY (L"The File menu")
NORMAL (L"If your computer has little memory, a very long recorded sound can be too big to be copied to the list of objects. "
	"Fortunately, the File menu contains commands to save the recording "
	"to a sound file on disk, so that you will never have to lose your recording.")
ENTRY (L"Sound pressure calibration")
NORMAL (L"Your computer's sound-recording software returns integer values between -32768 and 32767. "
	"Praat divides them by 32768 before putting them into a Sound object, "
	"so that the values in the Sound objects are always between -1 and +1.")
NORMAL (L"The Praat program considers these numbers to be air pressures in units of Pascal, "
	"but they are probably not the actual true air pressures that went into the microphone. "
	"For how to obtain the true air pressures, perform a @@sound pressure calibration@.")
MAN_END

MAN_BEGIN (L"Sounds: Concatenate", L"ppgb", 20110211)
INTRO (L"A command to concatenate all selected @Sound objects into a single large Sound.")
NORMAL (L"All sounds must have equal sampling frequencies and equal numbers of channels. "
	"They are concatenated in the order in which they appear in the list of objects (not in the order in which you select them; remember: What You See Is What You Get).")
ENTRY (L"How to concatenate directly to a file")
NORMAL (L"If the resulting sound does not fit into memory, use one of the "
	"commands in the @@Save menu@. See @@How to concatenate sound files@.")
ENTRY (L"See also")
NORMAL (L"If you want the sounds to fade into each other smoothly, choose @@Sounds: Concatenate with overlap...@ instead.")
MAN_END

MAN_BEGIN (L"Sounds: Concatenate with overlap...", L"ppgb", 20110211)
INTRO (L"A command to concatenate all selected @Sound objects into a single large Sound, with smooth cross-fading between the sounds.")
NORMAL (L"All sounds must have equal sampling frequencies and equal numbers of channels. "
	"They are concatenated in the order in which they appear in the list of objects (not in the order in which you select them; remember: What You See Is What You Get).")
ENTRY (L"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (1), L""
	Manual_DRAW_SETTINGS_WINDOW ("Sounds: Concatenate with overlap", 1)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Overlap time (s)", "0.01")
)
TAG (L"##Overlap time (s)")
DEFINITION (L"the time by which any two adjacent sounds will come to overlap, "
	"i.e. the time during which the earlier sound fades out and the later sound fades in.")
ENTRY (L"Procedure")
NORMAL (L"Suppose we start with the following two sounds. They are both 0.1 seconds long. "
	"The first sound is a sine wave with a frequency of 100 Hz, the second a sine wave with a frequency of 230 Hz:")
SCRIPT (5.0, 3, L""
	"Create Sound from formula... sine100 1 0 0.1 10000 0.9*sin(2*pi*100*x)\n"
	"Draw... 0 0 -1 1 yes Curve\n"
	"Remove")
SCRIPT (5.0, 3, L""
	"Create Sound from formula... sine230 1 0 0.1 10000 0.9*sin(2*pi*230*x)\n"
	"Draw... 0 0 -1 1 yes Curve\n"
	"Remove")
NORMAL (L"If the overlap time is 0.01 seconds, the concatenation of these two sounds will produce a Sound with a duration of 0.19 seconds, "
	"which is the sum of the durations of the two sounds, minus the overlap time.")
NORMAL (L"The concatenation works in the following way. "
	"The last 0.01 seconds of the first sound is multiplied by a falling raised cosine (the second half of a Hann window, see the first red curve), "
	"and the first 0.01 seconds of the second sound is multiplied by a rising raised cosine (the first half of a Hann window, see the second red curve):")
SCRIPT (6.7, 5, L""
	"Create Sound from formula... sine 1 0 0.1 10000 0.9\n"
	"Formula (part)... 0.09 0.1 1 1 self*(0.5-0.5*cos(pi*(xmax-x)/0.01))\n"
	"Select inner viewport... 0.5 3.5 0.5 2.5\n"
	"Red\n"
	"Draw... 0 0 -1 1 no Curve\n"
	"Formula... self*sin(2*pi*100*x)\n"
	"Black\n"
	"Draw... 0 0 -1 1 no Curve\n"
	"Draw inner box\n"
	"One mark top... 0 yes yes no\n"
	"One mark top... 0.09 yes yes yes\n"
	"One mark top... 0.1 yes yes no\n"
	"Text top... no Time (s)\n"
	"One mark left... -1 yes yes no\n"
	"One mark left... 0 yes yes yes\n"
	"One mark left... 1 yes yes no\n"
	"Formula... 0.9\n"
	"Formula (part)... 0 0.01 1 1 self*(0.5-0.5*cos(pi*x/0.01))\n"
	"Select inner viewport... 3.2 6.2 2.5 4.5\n"
	"Red\n"
	"Draw... 0 0 -1 1 no Curve\n"
	"Formula... self*sin(2*pi*230*x)\n"
	"Black\n"
	"Draw... 0 0 -1 1 no Curve\n"
	"Draw inner box\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 0.01 yes yes yes\n"
	"One mark bottom... 0.1 yes yes no\n"
	"Text bottom... no Time (s)\n"
	"One mark right... -1 yes yes no\n"
	"One mark right... 0 yes yes yes\n"
	"One mark right... 1 yes yes no\n"
	"Remove\n"
)
NORMAL (L"This figure shows how the two sounds are windowed (faded out and in), as well as how they will overlap.")
NORMAL (L"Finally, the two windowed (\"cross-faded\") sounds are added to each other:")
SCRIPT (6.7, 3, L""
	"sine100 = Create Sound from formula... sine100 1 0 0.1 10000 0.9*sin(2*pi*100*x)\n"
	"sine230 = Create Sound from formula... sine100 1 0 0.1 10000 0.9*sin(2*pi*230*x)\n"
	"plus sine100\n"
	"Concatenate with overlap... 0.01\n"
	"Draw... 0 0 -1 1 yes Curve\n"
	"One mark bottom... 0.09 yes yes yes\n"
	"One mark bottom... 0.1 yes yes yes\n"
	"plus sine100\n"
	"plus sine230\n"
	"Remove\n"
)
NORMAL (L"This example showed how it works if you concatenate two sounds; "
	"if you concatenate three sounds, there will be two overlaps, and so on.")
MAN_END

MAN_BEGIN (L"Sounds: Convolve...", L"ppgb & djmw", 20100404)
INTRO (L"A command available in the #Combine menu when you select two @Sound objects. "
	"This command convolves two selected @Sound objects with each other. "
	"As a result, a new Sound will appear in the list of objects; "
	"this new Sound is the %convolution of the two original Sounds.")
ENTRY (L"Settings")
TAG (L"##Amplitude scaling")
DEFINITION (L"Here you can choose between the `principled' options #integral, #sum, and #normalize, which are explained in 1, 2 and 3 below. "
	"There is also a `pragmatic' option, namely ##peak 0.99#, which scales the resulting sound in such a way "
	"that its absolute peak becomes 0.99, so that the sound tends to be clearly audible without distortion when you play it "
	"(see @@Sound: Scale peak...@).")
TAG (L"##Signal outside time domain is...")
DEFINITION (L"Here you can choose whether outside their time domains the sounds are considered to be #zero "
	"(the standard value), or #similar to the sounds within the time domains. "
	"This is explained in 4 below.")
ENTRY (L"1. Convolution as an integral")
NORMAL (L"The convolution %f*%g of two continuous time signals %f(%t) and %g(%t) is defined as the #integral")
FORMULA (L"(%f*%g) (%t) \\=3 \\in %f(%\\ta) %g(%t-%\\ta) %d%\\ta")
NORMAL (L"If %f and %g are sampled signals (as Sounds are in Praat), with the same @@sampling period@ %%\\Det%, "
	"the definition is discretized as")
FORMULA (L"(%f*%g) [%t] \\=3 \\su__%\\ta_ %f[%\\ta] %g[%t-%\\ta] %%\\Det%")
NORMAL (L"where %\\ta and %t-%\\ta are the discrete times at which %f and %g are defined, respectively.")
NORMAL (L"Convolution is a commutative operation, i.e. %g*%f equals %f*%g. "
	"This means that the order in which you put the two Sounds in the object list does not matter: you get the same result either way.")
ENTRY (L"2. Convolution as a sum")
NORMAL (L"You can see in the formula above that if both input Sounds are expressed in units of Pa, "
	"the resulting Sound should ideally be expressed in Pa^^2^s. "
	"Nevertheless, Praat will express it in Pa, because Sounds cannot be expressed otherwise.")
NORMAL (L"This basically means that it is impossible to get the amplitude of the resulting Sound correct for all purposes. "
	"For this reason, Praat considers a different definition of convolution as well, namely as the #sum")
FORMULA (L"(%f*%g) [%t] \\=3 \\su__\\ta_ %f[%\\ta] %g[%t-%\\ta]")
NORMAL (L"The sum definition is appropriate if you want to filter a pulse train with a finite-impulse-response filter "
	"and expect the amplitudes of each resulting period to be equal to the amplitude of the filter. Thus, the pulse train")
SCRIPT (5, 3,
	L"Create Sound from formula... peaks mono 0 0.6 1000 x*(col mod 100 = 0)\n"
	"Draw... 0 0 0 0.6 yes curve\n"
	"Remove"
)
NORMAL (L"(made with ##@@Create Sound from formula...@ peaks mono 0 0.6 1000 x*(col mod 100 = 0)#), "
	"convolved with the `leaky integrator' filter")
SCRIPT (5, 3,
	L"Create Sound from formula... leak mono 0 1 1000 exp(-x/0.1)\n"
	"Draw... 0 0 0 1 yes curve\n"
	"Remove"
)
NORMAL (L"(made with ##@@Create Sound from formula...@ leak mono 0 1 1000 exp(-x/0.1)#), "
	"yields the convolution")
SCRIPT (5, 3,
	L"Create Sound from formula... peaks mono 0 0.6 1000 x*(col mod 100 = 0)\n"
	"Create Sound from formula... leak mono 0 1 1000 exp(-x/0.1)\n"
	"plus Sound peaks\n"
	"Convolve... sum zero\n"
	"Draw... 0 0 0 0.8523 yes curve\n"
	"plus Sound peaks\n"
	"plus Sound leak\n"
	"Remove"
)
NORMAL (L"The difference between the integral and sum definitions is that in the sum definition "
	"the resulting sound is divided by %%\\Det%.")
ENTRY (L"3. Normalized convolution")
NORMAL (L"The %%normalized convolution% is defined as")
FORMULA (L"(normalized %f*%g) (%t) \\=3 \\in %f(%\\ta) %g(%t-%\\ta) %d%\\ta "
	"/ \\Vr (\\in %f^^2^(%\\ta) %%d\\ta% \\in %g^^2^(%\\ta) %%d\\ta%)")
ENTRY (L"4. Shape scaling")
NORMAL (L"The boundaries of the integral in 1 are -\\oo and +\\oo. "
	"However, %f and %g are Sound objects in Praat and therefore have finite time domains. "
	"If %f runs from %t__1_ to %t__2_ and is assumed to be #zero before %t__1_ and after %t__2_, and "
	"%g runs from %t__3_ to %t__4_ and is assumed to be zero outside that domain, "
	"then the convolution will be zero before %t__1_ + %t__3_ and after %t__2_ + %t__4_, "
	"while between %t__1_ + %t__3_ and %t__2_ + %t__4_ it is")
FORMULA (L"(%f*%g) (%t) = \\in__%%t%1_^^%%t%2^ %f(%\\ta) %g(%t-%\\ta) %d%\\ta")
NORMAL (L"In this formula, the argument of %f runs from %t__1_ to %t__2_, "
	"but the argument of %g runs from (%t__1_ + %t__3_) - %t__2_ to (%t__2_ + %t__4_) - %t__1_, "
	"i.e. from %t__3_ - (%t__2_ - %t__1_) to %t__4_ + (%t__2_ - %t__1_). "
	"This means that the integration is performed over two equal stretches of time during which %g must be taken zero, "
	"namely a time stretch before %t__3_ and a time stretch after %t__4_, both of duration %t__2_ - %t__1_ "
	"(equivalent equations can be formulated that rely on two stretches of %t__4_ - %t__3_ of zeroes in %f rather than in %g, "
	"or on a stretch of %t__2_ - %t__1_ of zeroes in %g and a stretch of %t__4_ - %t__3_ of zeroes in %f.")
NORMAL (L"If you consider the sounds outside their time domains as #similar to what they are within their time domains, instead of #zero, "
	"the discretized formula in 1 should be based on the average over the jointly defined values of %f[%\\ta] and %g[%t-%\\ta], "
	"without counting any multiplications of values outside the time domains. "
	"Suppose that %f is defined on the time domain [0, 1.2] with the value of 1 everywhere, "
	"and %g is defined on the time domain [0, 3] with the value 1 everywhere. "
	"Their convolution under the assumption that they are #zero elsewhere is then")
SCRIPT (5, 3,
	L"Create Sound from formula... short mono 0 1.2 1000 1\n"
	"Create Sound from formula... long mono 0 3 1000 1\n"
	"plus Sound short\n"
	"Convolve... integral zero\n"
	"Draw... 0 0 0 1.5 yes curve\n"
	"One mark left... 1.2 yes yes yes\n"
	"plus Sound short\n"
	"plus Sound long\n"
	"Remove"
)
NORMAL (L"but under the assumption that the sounds are #similar (i.e. 1) elsewhere, their convolution should be")
SCRIPT (5, 3,
	L"Create Sound from formula... short mono 0 1.2 1000 1\n"
	"Create Sound from formula... long mono 0 3 1000 1\n"
	"plus Sound short\n"
	"Convolve... integral similar\n"
	"Draw... 0 0 0 1.5 yes curve\n"
	"One mark left... 1.2 yes yes yes\n"
	"plus Sound short\n"
	"plus Sound long\n"
	"Remove"
)
NORMAL (L"i.e. a constant value of 1.2. This is what you get by choosing the #similar option; "
	"if %f is shorter than %g, the first and last parts of the convolution will be divided by a straight line of duration %t__2_ - %t__1_ "
	"to compensate for the fact that the convolution has been computed over fewer values of %f and %g there.")
ENTRY (L"5. Behaviour")
NORMAL (L"The start time of the resulting Sound will be the sum of the start times of the original Sounds, "
	"the end time of the resulting Sound will be the sum of the end times of the original Sounds, "
	"the time of the first sample of the resulting Sound will be the sum of the first samples of the original Sounds, "
	"the time of the last sample of the resulting Sound will be the sum of the last samples of the original Sounds, "
	"and the number of samples in the resulting Sound will be the sum of the numbers of samples of the original Sounds minus 1.")
ENTRY (L"6. Behaviour for stereo and other multi-channel sounds")
NORMAL (L"You can convolve e.g. a 10-channel sound either with another 10-channel sound or with a 1-channel (mono) sound.")
NORMAL (L"If both Sounds have more than one channel, the two Sounds have to have the same number of channels; "
	"each channel of the resulting Sound is then computed as the convolution of the corresponding channels "
	"of the original Sounds. For instance, if you convolve two 10-channel sounds, "
	"the resulting sound will have 10 channels, and its 9th channel will be the convolution of the 9th channels "
	"of the two original sounds.")
NORMAL (L"If one of the original Sounds has multiple channels and the other Sound has only one channel, "
	"the resulting Sound will have multiple channels; each of these is computed as the convolution of "
	"the corresponding channel of the multiple-channel original and the single channel of the single-channel original. "
	"For instance, if you convolve a 10-channel sound with a mono sound, "
	"the resulting sound will have 10 channels, and its 9th channel will be the convolution of the mono sound "
	"with the 9th channel of the original 10-channel sound.")
NORMAL (L"The amplitude scaling factor will be the same for all channels, so that the relative amplitude of the channels "
	"will be preserved in the resulting sound. For the #normalize scaling, for instance, the norm of %f in the formula above "
	"is taken over all channels of %f. For the ##peak 0.99# scaling, the resulting sound will typically have an absolute peak "
	"of 0.99 in only one channel, and lower absolute peaks in the other channels.")
ENTRY (L"7. Algorithm")
NORMAL (L"The computation makes use of the fact that convolution in the time domain corresponds to multiplication in the frequency domain: "
	"we first pad %f with a stretch of %t__4_ - %t__3_ of zeroes and %g with a stretch of %t__2_ - %t__1_ of zeroes (see 4 above), "
	"so that both sounds obtain a duration of (%t__2_ - %t__1_) + (%t__4_ - %t__3_); "
	"we then calculate the spectra of the two zero-padded sounds by Fourier transformation, "
	"then multiply the two spectra with each other, "
	"and finally Fourier-transform the result of this multiplication back to the time domain; "
	"the result will again have a duration of (%t__2_ - %t__1_) + (%t__4_ - %t__3_).")
MAN_END

MAN_BEGIN (L"Sounds: Cross-correlate...", L"djmw & ppgb", 20100404)
INTRO (L"A command available in the #Combine menu when you select two @Sound objects. "
	"This command cross-correlates two selected @Sound objects with each other. "
	"As a result, a new Sound will appear in the list of objects; "
	"this new Sound is the %cross-correlation of the two original Sounds.")
ENTRY (L"Settings")
TAG (L"##Amplitude scaling")
DEFINITION (L"Here you can choose between the `principled' options #integral, #sum, and #normalize, which are explained in 1, 2 and 3 below. "
	"There is also a `pragmatic' option, namely ##peak 0.99#, which scales the resulting sound in such a way "
	"that its absolute peak becomes 0.99, so that the sound tends to be clearly audible without distortion when you play it "
	"(see @@Sound: Scale peak...@).")
TAG (L"##Signal outside time domain is...")
DEFINITION (L"Here you can choose whether outside their time domains the sounds are considered to be #zero "
	"(the standard value), or #similar to the sounds within the time domains. "
	"This is explained in 4 below.")
ENTRY (L"1. Cross-correlation as an integral")
NORMAL (L"The cross-correlation of two continuous time signals %f(%t) and %g(%t) is a function of the lag time %\\ta, "
	"and defined as the #integral")
FORMULA (L"cross-corr (%f, %g) (%\\ta) \\=3 \\in %f(%t) %g(%t+%\\ta) %dt")
NORMAL (L"If %f and %g are sampled signals (as Sounds are in Praat), with the same @@sampling period@ %%\\Det%, "
	"the definition is discretized as")
FORMULA (L"cross-corr (%f, %g) [%\\ta] \\=3 \\su__%t_ %f[%t] %g[%t+%\\ta] %%\\Det%")
NORMAL (L"where %\\ta and %t+%\\ta are the discrete times at which %f and %g are defined, respectively.")
NORMAL (L"Cross-correlation is not a commutative operation, i.e. cross-corr (%g, %f) equals the time reversal of cross-corr (%f, %g). "
	"This means that the order in which you put the two Sounds in the object list does matter: "
	"the two results are each other's time reversals.")
ENTRY (L"2. Cross-correlation as a sum")
NORMAL (L"You can see in the formula above that if both input Sounds are expressed in units of Pa, "
	"the resulting Sound should ideally be expressed in Pa^^2^s. "
	"Nevertheless, Praat will express it in Pa, because Sounds cannot be expressed otherwise.")
NORMAL (L"This basically means that it is impossible to get the amplitude of the resulting Sound correct for all purposes. "
	"For this reason, Praat considers a different definition of cross-correlation as well, namely as the #sum")
FORMULA (L"cross-corr (%f, %g) [%\\ta] \\=3 \\su__%t_ %f[%t] %g[%t+%\\ta]")
NORMAL (L"The difference between the integral and sum definitions is that in the sum definition "
	"the resulting sound is divided by %%\\Det%.")
ENTRY (L"3. Normalized cross-correlation")
NORMAL (L"The %%normalized cross-correlation% is defined as")
FORMULA (L"norm-cross-corr (%f, %g) (%\\ta) \\=3 \\in %f(%t) %g(%t+%\\ta) %d%t "
	"/ \\Vr (\\in %f^^2^(%t) %%dt% \\in %g^^2^(%t) %%dt%)")
ENTRY (L"4. Shape scaling")
NORMAL (L"The boundaries of the integral in 1 are -\\oo and +\\oo. "
	"However, %f and %g are Sound objects in Praat and therefore have finite time domains. "
	"If %f runs from %t__1_ to %t__2_ and is assumed to be #zero before %t__1_ and after %t__2_, and "
	"%g runs from %t__3_ to %t__4_ and is assumed to be zero outside that domain, "
	"then the cross-correlation will be zero before %t__3_ - %t__2_ and after %t__4_ - %t__1_, "
	"while between %t__3_ - %t__2_ and %t__4_ - %t__1_ it is")
FORMULA (L"cross-corr (%f, %g) (%\\ta) = \\in__%%t%1_^^%%t%2^ %f(%t) %g(%t+%\\ta) %d%t")
NORMAL (L"In this formula, the argument of %f runs from %t__1_ to %t__2_, "
	"but the argument of %g runs from %t__1_ + (%t__3_ - %t__2_) to %t__2_ + (%t__4_ - %t__1_), "
	"i.e. from %t__3_ - (%t__2_ - %t__1_) to %t__4_ + (%t__2_ - %t__1_). "
	"This means that the integration is performed over two equal stretches of time during which %g must be taken zero, "
	"namely a time stretch before %t__3_ and a time stretch after %t__4_, both of duration %t__2_ - %t__1_ "
	"(equivalent equations can be formulated that rely on two stretches of %t__4_ - %t__3_ of zeroes in %f rather than in %g, "
	"or on a stretch of %t__2_ - %t__1_ of zeroes in %g and a stretch of %t__4_ - %t__3_ of zeroes in %f.")
NORMAL (L"If you consider the sounds outside their time domains as #similar to what they are within their time domains, instead of #zero, "
	"the discretized formula in 1 should be based on the average over the jointly defined values of %f[%\\ta] and %g[%t-%\\ta], "
	"without counting any multiplications of values outside the time domains. "
	"Suppose that %f is defined on the time domain [0, 1.2] with the value of 1 everywhere, "
	"and %g is defined on the time domain [0, 3] with the value 1 everywhere. "
	"Their cross-correlation under the assumption that they are #zero elsewhere is then")
SCRIPT (5, 3,
	L"Create Sound from formula... short mono 0 1.2 1000 1\n"
	"Create Sound from formula... long mono 0 3 1000 1\n"
	"plus Sound short\n"
	"Cross-correlate... integral zero\n"
	"Draw... 0 0 0 1.5 yes curve\n"
	"One mark left... 1.2 yes yes yes\n"
	"plus Sound short\n"
	"plus Sound long\n"
	"Remove"
)
NORMAL (L"but under the assumption that the sounds are #similar (i.e. 1) elsewhere, their cross-correlation should be")
SCRIPT (5, 3,
	L"Create Sound from formula... short mono 0 1.2 1000 1\n"
	"Create Sound from formula... long mono 0 3 1000 1\n"
	"plus Sound short\n"
	"Cross-correlate... integral similar\n"
	"Draw... 0 0 0 1.5 yes curve\n"
	"One mark left... 1.2 yes yes yes\n"
	"plus Sound short\n"
	"plus Sound long\n"
	"Remove"
)
NORMAL (L"i.e. a constant value of 1.2. This is what you get by choosing the #similar option; "
	"if %f is shorter than %g, the first and last parts of the cross-correlation will be divided by a straight line of duration %t__2_ - %t__1_ "
	"to compensate for the fact that the cross-correlation has been computed over fewer values of %f and %g there.")
ENTRY (L"5. Behaviour")
NORMAL (L"The start time of the resulting Sound will be the start time of %f minus the end time of %g, "
	"the end time of the resulting Sound will be the end time of %f minus the start time of %g, "
	"the time of the first sample of the resulting Sound will be the first sample of %f minus the last sample of %g, "
	"the time of the last sample of the resulting Sound will be the last sample of %f minus the first sample of %g, "
	"and the number of samples in the resulting Sound will be the sum of the numbers of samples of %f and %g minus 1.")
ENTRY (L"6. Behaviour for stereo and other multi-channel sounds")
NORMAL (L"You can cross-correlate e.g. a 10-channel sound either with another 10-channel sound or with a 1-channel (mono) sound.")
NORMAL (L"If both Sounds have more than one channel, the two Sounds have to have the same number of channels; "
	"each channel of the resulting Sound is then computed as the cross-correlation of the corresponding channels "
	"of the original Sounds. For instance, if you cross-correlate two 10-channel sounds, "
	"the resulting sound will have 10 channels, and its 9th channel will be the cross-correlation of the 9th channels "
	"of the two original sounds.")
NORMAL (L"If one of the original Sounds has multiple channels and the other Sound has only one channel, "
	"the resulting Sound will have multiple channels; each of these is computed as the cross-correlation of "
	"the corresponding channel of the multiple-channel original and the single channel of the single-channel original. "
	"For instance, if you cross-correlate a 10-channel sound with a mono sound, "
	"the resulting sound will have 10 channels, and its 9th channel will be the cross-correlation of the mono sound "
	"with the 9th channel of the original 10-channel sound.")
NORMAL (L"The amplitude scaling factor will be the same for all channels, so that the relative amplitude of the channels "
	"will be preserved in the resulting sound. For the #normalize scaling, for instance, the norm of %f in the formula above "
	"is taken over all channels of %f. For the ##peak 0.99# scaling, the resulting sound will typically have an absolute peak "
	"of 0.99 in only one channel, and lower absolute peaks in the other channels.")
ENTRY (L"7. Algorithm")
NORMAL (L"The computation makes use of the fact that cross-correlation in the time domain corresponds to multiplication "
	"of the time-reversal of %f with %g in the frequency domain: "
	"we first pad %f with a stretch of %t__4_ - %t__3_ of zeroes and %g with a stretch of %t__2_ - %t__1_ of zeroes (see 4 above), "
	"so that both sounds obtain a duration of (%t__2_ - %t__1_) + (%t__4_ - %t__3_); "
	"we then calculate the spectra of the two zero-padded sounds by Fourier transformation, "
	"then multiply the complex conjugate of the spectrum of %f with the spectrum of %g, "
	"and finally Fourier-transform the result of this multiplication back to the time domain; "
	"the result will again have a duration of (%t__2_ - %t__1_) + (%t__4_ - %t__3_).")
MAN_END

MAN_BEGIN (L"Sound: Autocorrelate...", L"djmw & ppgb", 20100404)
INTRO (L"A command available in the #Periodicity menu when you select one or more @Sound objects. "
	"This command autocorrelates the selected @Sound object. "
	"As a result, a new Sound will appear in the list of objects; "
	"this new Sound is the %autocorrelation of the original Sound.")
ENTRY (L"Settings")
TAG (L"##Amplitude scaling")
DEFINITION (L"Here you can choose between the `principled' options #integral, #sum, and #normalize, which are explained in 1, 2 and 3 below. "
	"There is also a `pragmatic' option, namely ##peak 0.99#, which scales the resulting sound in such a way "
	"that its absolute peak becomes 0.99, so that the sound tends to be clearly audible without distortion when you play it "
	"(see @@Sound: Scale peak...@).")
TAG (L"##Signal outside time domain is...")
DEFINITION (L"Here you can choose whether outside its time domain the sound is considered to be #zero "
	"(the standard value), or #similar to the sound within the time domain. "
	"This is explained in 4 below.")
//DEFINITION (L"Here you can choose whether outside its time domain the sound is considered to be #zero "
//	"(the standard value), or #similar to the sound within the time domain, or #periodic, i.e. that "
//	"outside the time domain the waveforms is a repetition of the waveform within the time domain. "
//	"This is explained in 4 below.")
ENTRY (L"1. Autocorrelation as an integral")
NORMAL (L"The autocorrelation of a continuous time signal %f(%t) is a function of the lag time %\\ta, "
	"and defined as the #integral")
FORMULA (L"%R__%f_ (%\\ta) \\=3 \\in %f(%t) %f(%t+%\\ta) %dt")
NORMAL (L"If %f is a sampled signal (as Sounds are in Praat), with @@sampling period@ %%\\Det%, "
	"the definition is discretized as")
FORMULA (L"%R__%f_ [%\\ta] \\=3 \\su__%t_ %f[%t] %f[%t+%\\ta] %%\\Det%")
NORMAL (L"where %\\ta and %t+%\\ta are the discrete times at which %f is defined.")
NORMAL (L"The autocorrelation is symmetric: %R__%f_ (-%\\ta) = %R__%f_ (%\\ta).")
ENTRY (L"2. Autocorrelation as a sum")
NORMAL (L"You can see in the formula above that if the input Sound is expressed in units of Pa, "
	"the resulting Sound should ideally be expressed in Pa^^2^s. "
	"Nevertheless, Praat will express it in Pa, because Sounds cannot be expressed otherwise.")
NORMAL (L"This basically means that it is impossible to get the amplitude of the resulting Sound correct for all purposes. "
	"For this reason, Praat considers a different definition of autocorrelation as well, namely as the #sum")
FORMULA (L"%R__%f_ [%\\ta] \\=3 \\su__%t_ %f[%t] %g[%t+%\\ta]")
NORMAL (L"The difference between the integral and sum definitions is that in the sum definition "
	"the resulting sound is divided by %%\\Det%.")
ENTRY (L"3. Normalized autocorrelation")
NORMAL (L"The %%normalized autocorrelation% is defined as")
FORMULA (L"norm-autocorr (%f) (%\\ta) \\=3 \\in %f(%t) %f(%t+%\\ta) %d%t / \\in %f^^2^(%t) %%dt%")
ENTRY (L"4. Shape scaling")
NORMAL (L"The boundaries of the integral in 1 are -\\oo and +\\oo. "
	"However, %f is a Sound object in Praat and therefore has a finite time domain. "
	"If %f runs from %t__1_ to %t__2_ and is assumed to be #zero before %t__1_ and after %t__2_, "
	"then the autocorrelation will be zero before %t__1_ - %t__2_ and after %t__2_ - %t__1_, "
	"while between %t__1_ - %t__2_ and %t__2_ - %t__1_ it is")
FORMULA (L"%R__%f_ (%\\ta) = \\in__%%t%1_^^%%t%2^ %f(%t) %f(%t+%\\ta) %d%t")
NORMAL (L"In this formula, the argument of the first %f runs from %t__1_ to %t__2_, "
	"but the argument of the second %f runs from %t__1_ + (%t__1_ - %t__2_) to %t__2_ + (%t__2_ - %t__1_), "
	"i.e. from %t__1_ - (%t__2_ - %t__1_) to %t__2_ + (%t__2_ - %t__1_). "
	"This means that the integration is performed over two equal stretches of time during which %f must be taken zero, "
	"namely a time stretch before %t__1_ and a time stretch after %t__2_, both of duration %t__2_ - %t__1_.")
NORMAL (L"If you consider the sound outside its time domains as #similar to what it is within its time domain, instead of #zero, "
	"the discretized formula in 1 should be based on the average over the jointly defined values of %f[%\\ta] and %f[%t-%\\ta], "
	"without counting any multiplications of values outside the time domain. "
	"Suppose that %f is defined on the time domain [0, 1.2] with the value of 1 everywhere. "
	"Its autocorrelation under the assumption that it is #zero elsewhere is then")
SCRIPT (5, 3,
	L"Create Sound from formula... sound mono 0 1.2 1000 1\n"
	"Autocorrelate... integral zero\n"
	"Draw... 0 0 0 1.5 yes curve\n"
	"One mark left... 1.2 yes yes yes\n"
	"plus Sound sound\n"
	"Remove"
)
NORMAL (L"but under the assumption that the sound is #similar (i.e. 1) elsewhere, its autocorrelation should be")
SCRIPT (5, 3,
	L"Create Sound from formula... sound mono 0 1.2 1000 1\n"
	"Autocorrelate... integral similar\n"
	"Draw... 0 0 0 1.5 yes curve\n"
	"One mark left... 1.2 yes yes yes\n"
	"plus Sound sound\n"
	"Remove"
)
NORMAL (L"i.e. a constant value of 1.2. This is what you get by choosing the #similar option; "
	"the autocorrelation will be divided by a triangular function "
	"to compensate for the fact that the autocorrelation has been computed over fewer values closer to the edges; "
	"this procedure is followed in all autocorrelation-based pitch computations in Praat (see @@Sound: To Pitch...@). "
	"For examples, see @@Boersma (1993)@.")
ENTRY (L"5. Behaviour")
NORMAL (L"The start time of the resulting Sound will be the start time of %f minus the end time of %f, "
	"the end time of the resulting Sound will be the end time of %f minus the start time of %f, "
	"the time of the first sample of the resulting Sound will be the first sample of %f minus the last sample of %f, "
	"the time of the last sample of the resulting Sound will be the last sample of %f minus the first sample of %f, "
	"and the number of samples in the resulting Sound will be twice the number of samples of %f, minus 1.")
ENTRY (L"6. Behaviour for stereo and other multi-channel sounds")
NORMAL (L"If the selected Sound has more than one channel, each channel of the resulting Sound is computed "
	"as the cross-correlation of the corresponding channel "
	"of the original Sound. For instance, if you autocorrelate a 10-channel sound, "
	"the resulting sound will again have 10 channels, and its 9th channel will be the autocorrelation of the 9th channel "
	"of the original sound.")
NORMAL (L"The amplitude scaling factor will be the same for all channels, so that the relative amplitude of the channels "
	"will be preserved in the resulting sound. For the #normalize scaling, for instance, the squared norm of %f in the formula above "
	"is taken over all channels of %f. For the ##peak 0.99# scaling, the resulting sound will typically have an absolute peak "
	"of 0.99 in only one channel, and lower absolute peaks in the other channels.")
ENTRY (L"Algorithm")
NORMAL (L"The autocorrelation is calculated as the @@Sounds: Cross-correlate...|cross-correlation@ of a sound with itself.")
MAN_END

MAN_BEGIN (L"Sound: Scale intensity...", L"ppgb", 20120614)
INTRO (L"A command available in the #Modify menu when you select one or more @Sound objects. "
	"With this command you multiply the amplitude of each Sound in such a way that its average (i.e. root-mean-square) intensity becomes "
	"the ##new average intensity# that you specify (see Settings).")
ENTRY (L"Settings")
TAG (L"##New average intensity (dB SPL)")
DEFINITION (L"the new average intensity of the Sound. The standard value is 70 dB SPL. This means that the root-mean-square amplitude "
	"of the sound will come to lie 70 dB above the assumed auditory threshold of 0.00002 Pa. Please check whether as a result a peak in the sound "
	"does not get below -1 Pa or above +1 Pa. If that happens, the sound will be clipped when played, which will be audible as distortion; "
	"in thast case you may want to set this number to 60 dB or even lower.")
MAN_END

MAN_BEGIN (L"Sound: Scale peak...", L"ppgb", 20100328)
INTRO (L"A command available in the #Modify menu when you select one or more @Sound objects. "
	"With this command you multiply the amplitude of each Sound in such a way that its absolute peak becomes "
	"the ##new absolute peak# that you specify (see Settings).")
ENTRY (L"Settings")
TAG (L"##New absolute peak")
DEFINITION (L"the new absolute peak of the Sound. The standard value is 0.99: this maximizes the audibility of the Sound "
	"(sounds with lower amplitude are weaker) without distorting it "
	"(sounds with absolute peaks above 1 are clipped when they are played).")
ENTRY (L"Examples")
NORMAL (L"The absolute peak of the following sound is 0.033:")
SCRIPT (6.0, 3.0,
	L"Create Sound from formula... 033 mono 0 0.1 10000 -0.02*(sin(2*pi*205*x)+sin(2*pi*5*x))+0.007\n"
	"Draw... 0 0 -0.033 0.0247 yes curve\n"
	"Remove"
)
NORMAL (L"This sound will play rather weakly. To make it louder, you can do ##Scale peak...# with a ##new absolute peak# of 0.99. "
	"Praat will then multiply the waveform by 30, changing the sound to the following:")
SCRIPT (6.0, 3.0,
	L"Create Sound from formula... 033 mono 0 0.1 10000 -0.6*(sin(2*pi*205*x)+sin(2*pi*5*x))+0.21\n"
	"Draw... 0 0 -0.99 0.741 yes curve\n"
	"Remove"
)
NORMAL (L"The absolute peak is now 0.99, and the sound will play loudly.")
NORMAL (L"The reverse is also possible. The absolute peak of the following sound is 19.8:")
SCRIPT (6.0, 3.0,
	L"Create Sound from formula... 033 mono 0 0.1 10000 12*(sin(2*pi*305*x)+sin(2*pi*5*x))-4.2\n"
	"Draw... 0 0 -15.28 19.8 yes curve\n"
	"Remove"
)
NORMAL (L"This sound will not play correctly: all samples with an amplitude outside the [-1;+1] range will be clipped to -1 or +1. "
	"To make this sound nicer to play, you can again do ##Scale peak...# with a ##new absolute peak# of 0.99. "
	"Praat will then divide the waveform by 20, changing the sound to the following:")
SCRIPT (6.0, 3.0,
	L"Create Sound from formula... 033 mono 0 0.1 10000 0.6*(sin(2*pi*305*x)+sin(2*pi*5*x))-0.21\n"
	"Draw... 0 0 -0.764 0.99 yes curve\n"
	"Remove"
)
NORMAL (L"The absolute peak is now 0.99, and the sound will play without distortion.")
MAN_END

}

/* End of file manual_sound.cpp */
