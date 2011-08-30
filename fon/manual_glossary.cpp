/* manual_glossary.cpp
 *
 * Copyright (C) 1992-2008 Paul Boersma
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

#include "manual_exampleSound.h"

static void draw_TimeDomain_Sound (Graphics g) {
	Sound_draw (manual_exampleSound (), g, 0, 0, 0, 0, TRUE, L"curve");
}
static void draw_TimeDomain_Pitch (Graphics g) {
	Pitch_draw (manual_examplePitch (), g, 0, 0, 200.0, 500.0, TRUE, Pitch_speckle_NO, kPitch_unit_HERTZ);
}

void manual_glossary_init (ManPages me);
void manual_glossary_init (ManPages me) {

MAN_BEGIN (L"aliasing", L"ppgb", 20040331)
INTRO (L"Aliasing (Du. %vouwvervorming) is the phenomenon of the ambiguity "
	"of a sampled signal.")
ENTRY (L"Example")
NORMAL (L"With a sampling frequency of 10 kHz, a sine wave with a frequency of 3 kHz "
	"receives the same representation as a sine wave with a frequency of 7 kHz, "
	"13 kHz, or 17 kHz, and so on. If the sampled signal is meant to represent a "
	"continuous spectral range starting at 0 Hz "
	"(which is the most common case for speech recordings), "
	"all these tones are likely to be interpreted as 3 kHz tones after sampling.")
NORMAL (L"To remedy this unwanted situation, the signal is usually low-pass filtered "
	"with a cut-off frequency just below 5 kHz, prior to sampling.")
MAN_END

MAN_BEGIN (L"Click", L"ppgb", 19960913)
INTRO (L"One of the ways to control @Editors.")
ENTRY (L"How to click")
LIST_ITEM (L"1. Position the mouse above the object that you want to click.")
LIST_ITEM (L"2. Press and release the (left) mouse button.")
NORMAL (L"See also @@Shift-click@.")
ENTRY (L"Usage in the Praat program")
NORMAL (L"Clicking on an object is used for selecting this object while deselecting "
	"all previously selected objects; clicking is also used for moving a cursor hair.")
MAN_END

MAN_BEGIN (L"constant extrapolation", L"ppgb", 20080425)
INTRO (L"- the interpretation of values in tiers before the first point or after the last point.")
ENTRY (L"Example")
NORMAL (L"The following is a @PitchTier with three points:")
SCRIPT (4, 3, L""
	"Create PitchTier... tier 0 0.5\n"
	"Add point... 0.10 170\n"
	"Add point... 0.20 180\n"
	"Add point... 0.45 110\n"
	"Draw... 0 0 50 250 yes\n"
	"Remove\n"
	"One mark left... 100 yes yes yes\n"
	"One mark left... 150 yes yes yes\n"
	"One mark left... 200 yes yes yes\n"
	"One mark bottom... 0.1 yes yes yes\n"
	"One mark bottom... 0.2 yes yes yes\n"
	"One mark bottom... 0.3 yes yes yes\n"
	"One mark bottom... 0.4 yes yes yes\n"
)
NORMAL (L"Between 0.10 and 0.20 seconds, the pitch rises from 170 to 180 Hz, "
	"and between 0.20 and 0.45 seconds it falls from 180 to 110 Hz. "
	"This is @@linear interpolation@.")
NORMAL (L"But what happens before 0.10 seconds? There, the pitch is just 170 Hz, "
	"i.e. the value of the first pitch point. This is %%constant extrapolation% to the left.")
NORMAL (L"Likewise, after 0.45 seconds the pitch is just 110 Hz, "
	"i.e. the value of the last pitch point. This is constant extrapolation to the right.")
ENTRY (L"Constant extrapolation in Praat")
NORMAL (L"Praat uses constant extrapolation in all tiers and grids with values at time points "
	"(@PitchTier, @IntensityTier, @DurationTier, @AmplitudeTier, @FormantGrid).")
MAN_END

MAN_BEGIN (L"Drag", L"ppgb", 19960913)
INTRO (L"Dragging is one of the ways to control @Editors.")
ENTRY (L"How to drag")
LIST_ITEM (L"1. Position the mouse above the object that you want to drag.")
LIST_ITEM (L"2. Press the (left) mouse button.")
LIST_ITEM (L"3. Keeping the mouse button pressed, move the mouse across the window. "
	"A shadow of the object will follow.")
LIST_ITEM (L"4. Release the mouse button when it is above the location where you want your object "
	"to be moved (the %drop %site). If the drop site makes any sense, "
	"the object will move there.")
NORMAL (L"See also @@Shift-drag@.")
ENTRY (L"Usage in the Praat program")
NORMAL (L"Dragging is used for manipulating the time and value of one or more marks, targets, or boundaries:")
MAN_END

MAN_BEGIN (L"end time", L"ppgb", 20060714)
INTRO (L"- the end of the @@time domain@ (see there).")
MAN_END

MAN_BEGIN (L"Fast Fourier Transform", L"ppgb", 20041123)
INTRO (L"An algorithm for fast computation of the Fourier transform of a sampled signal. "
	"It involves increasing the number of samples %N to the next-highest power of two, "
	"and the computation time scales as %N log %N.")
NORMAL (L"In Praat, the Fast Fourier Transform is used:")
LIST_ITEM (L"1. For the Fourier transform of an entire sound: @@Sound: To Spectrum...@ with #yes for the #Fast setting, "
	"and @@Spectrum: To Sound@.")
LIST_ITEM (L"2. For the Fourier transform of consecutive frames in a sound. See @@Sound: To Spectrogram...@.")
LIST_ITEM (L"3. For the fast computation of correlations, e.g. in @@Sound: To Pitch (ac)...@.")
MAN_END

MAN_BEGIN (L"FFT", L"ppgb", 20011121)
INTRO (L"An abbreviation for @@Fast Fourier Transform@.")
MAN_END

MAN_BEGIN (L"frequency", L"ppgb", 20030314)
INTRO (L"Frequency is how often something happens in a certain time, for instance "
	"the number of times the Praat home page www.praat.org is visited every day.")
NORMAL (L"In Praat, frequency is the number of vibration cycles per second. "
	"Although one can sometimes see the abbreviation %cps, Praat always uses Hz (short for %hertz), "
	"which means the same.")
NORMAL (L"Unfortunately, there are two very distinct kinds of vibrations in speech analysis. "
	"For pitch, frequency is the number of glottal cycles per second, and for spectral analysis, "
	"frequency is the number of sine wave cycles per second. Quite some bit of the training of an "
	"acoustic phonetician goes into the understanding of the difference between the ideas behind F0 and F1, "
	"and many years can be spent on understanding the influence they have on each other in production, "
	"acoustics, perception, or measurement...")
NORMAL (L"In order to prevent confusion, Praat always requires frequency to be expressed in Hz. "
	"So if you want to supply a sampling frequency of 20 kiloHertz (20 kHz), you fill in 20000 or 2e4 or 20e3. "
	"If you want to switch off pre-emphasis in some spectral analysis, you supply 1 GigaHertz (GHz) "
	"for its \"from-frequency\", by typing 1e9.")
NORMAL (L"In Praat editor windows, frequency usually runs from bottom to top, since time already has to run "
	"from left to right. This goes for spectrograms, pitch contours, and formant contours. "
	"In spectral slices, frequency runs from left to right, since these have no time axis.")
MAN_END

MAN_BEGIN (L"linear interpolation", L"ppgb", 20080426)
INTRO (L"- the interpretation of values in tiers between the first point and the last point.")
ENTRY (L"Example")
NORMAL (L"The following is a @PitchTier with three points:")
SCRIPT (4, 3, L""
	"Create PitchTier... tier 0 0.5\n"
	"Add point... 0.10 170\n"
	"Add point... 0.20 180\n"
	"Add point... 0.45 110\n"
	"Draw... 0 0 50 250 yes\n"
	"Remove\n"
	"One mark left... 100 yes yes yes\n"
	"One mark left... 150 yes yes yes\n"
	"One mark left... 200 yes yes yes\n"
	"One mark bottom... 0.1 yes yes yes\n"
	"One mark bottom... 0.2 yes yes yes\n"
	"One mark bottom... 0.3 yes yes yes\n"
	"One mark bottom... 0.4 yes yes yes\n"
)
NORMAL (L"Between 0.10 and 0.20 seconds, the pitch rises linearly from 170 to 180 Hz, "
	"and between 0.20 and 0.45 seconds it falls linearly from 180 to 110 Hz. "
	"This is %%linear interpolation%: at all times between two adjacent points, "
	"the pitch values follow the straight line that connects the two points.")
NORMAL (L"(Before 0.10 seconds and after 0.45 seconds there is @@constant extrapolation@.")
ENTRY (L"Linear interpolation in Praat")
NORMAL (L"Praat uses linear interpolation in all tiers and grids with values at time points "
	"(@PitchTier, @IntensityTier, @DurationTier, @AmplitudeTier, @FormantGrid).")
MAN_END

MAN_BEGIN (L"Nyquist frequency", L"ppgb", 20040331)
INTRO (L"The %%Nyquist frequency% is the bandwidth of a sampled signal, "
	"and is equal to half the sampling frequency of that signal. "
	"If the sampled signal should represent a continuous spectral range starting "
	"at 0 Hz (which is the most common case for speech recordings), "
	"the Nyquist frequency is the highest frequency that the sampled signal "
	"can unambiguously represent.")
ENTRY (L"Example")
NORMAL (L"If a speech signal is sampled at 22050 Hz, the highest frequency that we can "
	"expect to be present in the sampled signal is 11025 Hz. "
	"This means that to heed this expectation, we should run the continuous signal "
	"through a low-pass filter with a cut-off frequency below 11025 Hz; "
	"otherwise, we would experience the phenomenon of @aliasing.")
NORMAL (L"Of course, with a sampling frequency of 22050 Hz we could also represent a signal "
	"band-limited between, say, 40000 Hz and 51025 Hz, but this seems less useful "
	"in speech research.")
MAN_END

MAN_BEGIN (L"objects", L"ppgb", 20030528)
INTRO (L"The things that contain the data in the Praat program.")
NORMAL (L"The objects are visible in the @@List of Objects@ in the @@Object window@.")
NORMAL (L"Most objects are contained in memory: they are %not files! "
	"Therefore, you may want to save them with one of the commands from the @@Save menu@ "
	"before you @Quit. "
	"Exceptions are the LongSound objects in the Praat program and the file-based dictionaries "
	"in the ALS program.")
NORMAL (L"You can create an object by choosing a command from the @@New menu@ "
	"or from the @@Open menu@.")
NORMAL (L"When you select one or more objects, you can perform on them the actions that are shown "
	"in the @@dynamic menu@, on the ##fixed buttons# below the list of objects, "
	"or in the @@Save menu@. You can choose hidden actions with the help of the @ButtonEditor.")
MAN_END

MAN_BEGIN (L"overlap-add", L"ppgb", 20070816)
INTRO (L"A method for manipulating the pitch and duration of an acoustic speech signal. "
	"This method was realized by @@Moulines & Charpentier (1990)@, "
	"who called it Time-Domain Pitch-Synchronous Overlap-and-Add (TD-PSOLA). "
	"Nowadays, PSOLA\\tm and TD-PSOLA\\tm are trademarks owned by France Telecom^\\re.")
ENTRY (L"Overlap-add synthesis")
NORMAL (L"When a @Sound is created from a @Manipulation object, the following steps are performed:")
LIST_ITEM (L"1. From the @PitchTier, new points are generated along the entire time domain, "
	"with the method of @@PitchTier: To PointProcess@.")
LIST_ITEM (L"2. The period information in the original pulses (available in the Manipulation object) "
	"is used to remove from the new pulses all points that lie within voiceless intervals "
	"(i.e., places where the distance between adjacent points in the original pulses is greater than 20 ms).")
LIST_ITEM (L"3. The voiceless parts are copied from the source Sound to the target Sound, "
	"re-using some parts if the local duration is greater than 1.")
LIST_ITEM (L"4. For each target point, we look up the nearest source point. A piece of the source Sound, "
	"centred around the source point, is copied to the target Sound at a location determined by "
	"the target point, using a bell-shaped window whose left-hand half-length is the minimum "
	"of the left-hand periods adjacent to the source and target points "
	"(and analogously for the right-hand half-length).")
MAN_END

MAN_BEGIN (L"pitch floor", L"ppgb", 20060914)
INTRO (L"The pitch floor is the bottom of the pitch range in the Sound window (see the @Intro). "
	"The standard setting is 75 hertz; pitch values below this pitch floor will not be computed or shown by Praat.")
NORMAL (L"To change the pitch floor, choose @@Pitch settings...@.")
MAN_END

MAN_BEGIN (L"power spectral density", L"ppgb", 20101026)
INTRO (L"The average power in a sound during a certain time range and in a certain frequency range, expressed in Pa^2/Hz.")
ENTRY (L"Mathematical definition")
NORMAL (L"The %%complex spectrum% of a sound %x(%t) in the time range (%t__1_, %t__2_) is")
FORMULA (L"%X(%f) \\=3 \\in__%%t%1_^^%%t%2^ %x(%t) e^^-2%\\pi%ift^ %dt")
NORMAL (L"for any frequency %f in the two-sided frequency domain (-%F, +%F). "
	"If %x(%t) is expressed in units of Pascal, %X(%f) is expressed in units of Pa/Hz. "
	"In Praat, this complex spectrum is the quantity stored in a @Spectrum.")
NORMAL (L"From the complex spectrum we can compute the %%one-sided power spectral density% in Pa^2/Hz as")
FORMULA (L"PSD(%f) \\=3 2|%X(%f)|^2 / (%t__2_ - %t__1_)")
NORMAL (L"where the factor 2 is due to adding the contributions from positive and negative frequencies. "
	"In Praat, this power spectral density is the quantity stored in a @Spectrogram.")
NORMAL (L"The PSD divides up the total power of the sound. "
	"To see this, we integrate it over its entire one-sided frequency domain (0, %F):")
FORMULA (L"\\in__0_^^%F^ PSD(%f) %df = \\in__0_^^%F^ 2|%X(%f)|^2/(%t__2_-%t__1_) %df =")
FORMULA (L"= 1/(%t__2_-%t__1_) \\in__-%F_^^+%F^ |%X(%f)|^2 %df = 1/(%t__2_-%t__1_) \\in__%%t%1_^^%%t%2^ |%x(%t)|^2 %dt")
NORMAL (L"where the last step uses %%Parceval's theorem%. "
	"The result is precisely the average power of the sound in the time range (%t__1_, %t__2_).")
ENTRY (L"The logarithmic power spectral density")
NORMAL (L"It is often useful to express the power spectral density in dB relative to %%P__ref_% = 2\\.c10^^-5^ Pa:")
FORMULA (L"PSD__dB_(%f) = 10 log__10_ { PSD(%f) / %%P__ref_%^2 }")
NORMAL (L"Since the argument of the logarithm is in units of Hz^^-1^, this spectral measure can loosely be said "
	"to be in units of `dB/Hz'. In Praat, this logarithmic power spectral density is the quantity stored in an @Ltas; "
	"it is also the quantity shown in pictures of a @Spectrum and a @Spectrogram.")
MAN_END

MAN_BEGIN (L"quantile algorithm", L"ppgb", 19980101)
INTRO (L"An algorithm to compute the specified quantile of a sorted array of real numbers.")
NORMAL (L"The %n\\%  %quantile of a continuous real-valued distribution is the value below which %n\\%  of the values "
	"is expected to lie. If we are given an array of real numbers that we want to interpret as having been drawn from a "
	"distribution, we can %estimate the quantiles of the underlying distribution.")
ENTRY (L"1. The median")
NORMAL (L"The %median is a special case of a quantile: it is the 50\\%  quantile. It is usually estimated as follows: "
	"from an odd number of values, take the middle value; form an even number, take the average of "
	"the two midmost values. For instance, if our values are 15, 20, and 32, the median is 20; "
	"if our values are 15, 20, 32, and 60, the median is 26.")
NORMAL (L"This estimate is direction-independent: if we multiply all values by -1 (i.e., they become -60, -32, -20, and -15), "
	"the median is also multiplied by -1 (it becomes -26).")
ENTRY (L"2. Percentiles?")
NORMAL (L"The %%n%th %percentile of a set of values is usually defined as the highest attested value "
	"for which at most %n\\%  of all attested values are less or equal. "
	"For instance, if our values are 15, 20, 32, and 60, the 30th percentile is 15. Here is an extensive list:")
LIST_ITEM (L"\tPercentile number\tValue")
LIST_ITEM (L"\t0\t-")
LIST_ITEM (L"\t10\t-")
LIST_ITEM (L"\t20\t-")
LIST_ITEM (L"\t30\t15")
LIST_ITEM (L"\t40\t15")
LIST_ITEM (L"\t50\t20")
LIST_ITEM (L"\t60\t20")
LIST_ITEM (L"\t70\t20")
LIST_ITEM (L"\t80\t32")
LIST_ITEM (L"\t90\t32")
LIST_ITEM (L"\t100\t60")
NORMAL (L"However, this procedure does not yield an estimate of the quantiles of the underlying distribution. "
	"For instance, the estimate is direction-dependent: if we multiply all values by -1, the 50th percentile "
	"becomes -32 instead of -20, and the 70th percentile becomes -32 instead of the expected -15, "
	"which is minus the 30th percentile of the original data set.")
ENTRY (L"3. Unbiased quantiles")
NORMAL (L"To get a better estimate of the quantiles of the underlying distribution, the interpolation that we used "
	"to determine the median, is generalized to %any quantile.")
NORMAL (L"We assume that the attested values 15, 20, 32, and 60 each take up one quarter of the \"quantile space\". "
	"These four values are in the middles of those quarters, so they are at the 0.125, 0.375, 0.625, and 0.875 quantiles.")
NORMAL (L"Quantiles in between 0.125 and 0.875 are evaluated by linear interpolation: the 0.25, 0.50, and 0.75 quantiles "
	"are 17.5, 26, and 46, respectively. Note that the 0.50 quantile is the median. The 0.40 quantile, for example, "
	"is estimated as 20 + (32 - 20)\\.c(0.40 - 0.375)/(0.625 - 0.375) = 21.2.")
NORMAL (L"Quantiles between 0 and 0.125 or between 0.875 and 1 are evaluated by linear extrapolation from the "
	"lowest or highest pair of values: the 0\\%  quantile is estimated as 15 - 1/2 (20 - 15) = 12.5, "
	"and the 100\\%  quantile is estimated as 60 + 1/2 (60 - 32) = 74. The 0.10 quantile is estimated as "
	"12.5 + (15 - 12.5)\\.c(0.10 - 0.0)/(0.125 - 0.0) = 14.5.")
NORMAL (L"Note that the estimated values for the very low or high quantiles can lie outside the range of attested values. "
	"In fact, the computed 0\\%  and 100\\%  quantiles are thought to be estimates of the minimum and maximum values "
	"of the distribution. For uniform distributions, these estimates are reasonable; for a normal distribution, of course, "
	"the 0\\%  and 100\\%  quantiles are meaningless.")
MAN_END

MAN_BEGIN (L"sampling frequency", L"ppgb", 20040415)
INTRO (L"The sampling frequency (or %%sample rate%) is the number of samples per second in a @Sound. "
	"For example: if the sampling frequency is 44100 hertz, a recording with a duration of 60 seconds "
	"will contain 2,646,000 samples.")
NORMAL (L"Usual values for the sampling frequency are 44100 Hz (CD quality) and 22050 Hz "
	"(just enough for speech, since speech does not contain relevant frequencies above 11025 Hz; see @aliasing).")
NORMAL (L"To get the sampling frequency of a selected #Sound, click #Info or choose @@Get sampling frequency@.")
MAN_END

MAN_BEGIN (L"sampling period", L"ppgb", 20040415)
INTRO (L"The sampling period is the time difference between two consecutive samples in a @Sound. "
	"It is the inverse of the @@sampling frequency@. For example: if the sampling frequency "
	"is 44100 Hz, the sampling period is 1/44100 = 2.2675736961451248e-05 seconds: "
	"the samples are spaced approximately 23 microseconds apart.")
NORMAL (L"To get the sampling period of a selected #Sound, click #Info or choose @@Get sampling period@.")
MAN_END

MAN_BEGIN (L"Shift-drag", L"ppgb", 19980823)
INTRO (L"Shift-dragging is one of the ways to control @Editors.")
ENTRY (L"How to Shift-drag")
LIST_ITEM (L"1. Position the mouse above any of the objects that you want to drag "
	"(the objects were probably selected first).")
LIST_ITEM (L"2. Press a Shift key.")
LIST_ITEM (L"3. Press the (left) mouse button.")
LIST_ITEM (L"4. Keeping the mouse button pressed, move the mouse across the window. "
	"A shadow of the objects will follow.")
LIST_ITEM (L"5. Release the mouse button when it is above the location where you want your objects "
	"to be moved. If this %%drop site% makes any sense, the objects will move there.")
ENTRY (L"Usage in the Praat program")
NORMAL (L"While plain @@drag@ging is used for moving objects that were selected first by clicking, "
	"##Shift-dragging# is used for manipulating the times and values of more marks, targets, "
	"or boundaries simultaneously:")
MAN_END

MAN_BEGIN (L"Shift-click", L"ppgb", 19960913)
INTRO (L"One of the ways to control @Editors.")
ENTRY (L"How to Shift-click")
LIST_ITEM (L"1. Position the mouse above the object that you want to Shift-click.")
LIST_ITEM (L"2. Press a Shift key.")
LIST_ITEM (L"3. Press and release the (left) mouse button.")
ENTRY (L"Usage in the Praat program")
NORMAL (L"Whereas plain @@click@ing is used for selecting only one object while deselecting "
	"all previously selected objects, ##Shift-click# is used for selecting a mark, target, "
	"or boundary, %without deselecting the previously selected objects:")
MAN_END

MAN_BEGIN (L"sound pressure calibration", L"ppgb", 20041119)
NORMAL (L"Your computer's sound-recording software returns integer values between -32768 and 32767. "
	"Praat divides them by 32768 before putting them into a Sound object, "
	"so that the values in the Sound objects are always between -1 and +1.")
NORMAL (L"The Praat program considers these numbers to be air pressures in units of Pascal.")
NORMAL (L"These values in Pascal are probably not the actual true air pressures that went into the microphone. "
	"To determine what the actual air pressures were, you have to record an extra sound with a known air pressure, "
	"with the same recording gain as the rest of your recordings. It is best if this extra sound is something "
	"like a white noise, so that its @@sound pressure level@ can be determined reliably. A sine wave of 400 Hz, "
	"for instance, is less suitable, since its sound pressure level at your microphone can depend very strongly "
	"on the precise position and direction of the loudspeaker and on the position of other objects (like you, "
	"the experimenter) in the room (differences of 10 dB are no exception).")
NORMAL (L"For example, suppose that you record some speech on a CD recorder or with Praat's SoundRecorder. "
	"Immediately before or after this recording, you also record a white noise produced by a noise generator. "
	"With a dB meter (Linear or C setting), located at the same position as the microphone of the recording, "
	"you measure that the Sound Pressure Level of this noise is 76.5 dB. "
	"When you later open the recorded noise in Praat's sound window, Praat tells you that its average intensity "
	"(switch on ##Show intensity#) is 68.6 dB. You then know that you have to add 7.9 dB to intensities measured in Praat "
	"to get at the true sound pressure level. Thus, if your speech contains a long [a:] whose average intensity "
	"is measured in the Sound window as 71.1 dB, its true sound pressure level must be 79.0 dB.")
NORMAL (L"In this example, you can make Praat's sound window show the true sound pressures in Pascal "
	"and true sound pressure levels in dB, if you multiply the sound with a factor of 10^^(7.9/20)^, "
	"which you can do by selecting the Sound and choosing ##Multiply...# from the #Modify menu, "
	"then supplying $$10\\^ (7.9/20)$ as the multiplication factor.")
MAN_END

MAN_BEGIN (L"sound pressure level", L"ppgb", 20041124)
NORMAL (L"Air pressures are measured in Pascal (Pa), which are Newtons per square metre (N/m^2). "
	"The ambient air pressure is about 100,000 Pa, and the lungs modify this in phonation "
	"only by a small amount, namely 200 to 1000 Pa (= 2 to 10 cm H__2_O). "
	"Outside your body, the air pressures caused by your speech are much smaller again, "
	"namely 0.01 to 1 Pa at 1 metre from your lips. These are comparable to the values that you "
	"see for a typical speech recording in Praat's sound window, "
	"although these numbers in Praat can be interpreted as true air pressures only if you perform "
	"a @@sound pressure calibration@ (including the multiplication).")
NORMAL (L"A normative human ear can detect a root-mean-squared air pressure "
	"as small as 0.00002 Pa, for a sine wave with a frequency of 1000 Hz. "
	"The %%sound pressure level% can be expressed in dB relative to this normative threshold:")
FORMULA (L"SPL = 10 log__10_ { 1/(%t__2_-%t__1_) \\in__t1_^^t2^ %x^2(%t) %dt / (2\\.c10^^-5^ Pa)^2 }")
NORMAL (L"where %x(%t) is the sound pressure in Pa as a function of time, "
	"and %t__1_ and %t__2_ are the times between which the energy (squared air pressure) is averaged. "
	"For a calibrated sound, Praat's sound window will show you the SPL as a function of time, "
	"if you switch on ##Show intensity# and set its ##averaging method# to #energy (with ##Intensity settings...#).")
MAN_END

MAN_BEGIN (L"spectro-temporal representation", L"ppgb", 20030314)
INTRO (L"A representation (of a sound signal, for instance) as some sort of intensity as a function "
	"of @time and @frequency. In Praat, we have the @Spectrogram, which is acoustic energy density as a "
	"function of time in seconds and frequency in Hz, and the @Cochleagram, which is basilar membrane "
	"excitation as a function of time in seconds and frequency in Bark.")
NORMAL (L"For tutorial information, see @@Intro 3.1. Viewing a spectrogram@.")
MAN_END

MAN_BEGIN (L"start time", L"ppgb", 20060714)
INTRO (L"- the beginning of the @@time domain@ (see there).")
MAN_END

MAN_BEGIN (L"stereo", L"ppgb", 20110131)
INTRO (L"A stereo sound is a sound consisting of a left and a right channel. "
	"Praat can read and write both mono and stereo sounds.")
ENTRY (L"Reading a stereo sound from disk")
NORMAL (L"If you read a @Sound into Praat from disk with @@Read from file...@, "
	"and that sound file is a stereo file, the resulting Sound object will be a stereo Sound.")
NORMAL (L"If you want to look at the left or right channel separately, "
	"you can use @@Extract one channel...@.")
ENTRY (L"Saving a stereo sound to disk")
NORMAL (L"If you choose @@Save as WAV file...@, and the sound is a stereo sound, "
	"then the sound file will be a stereo file. "
	"If you have two mono sounds and you want to combine them into one stereo sound, "
	"you can use @@Combine to stereo@.")
MAN_END

MAN_BEGIN (L"time", L"ppgb", 20110128)
INTRO (L"In normal life, time is how late the watch says it is.")
NORMAL (L"In Praat, this definition is largely irrelevant. "
	"Sound files rarely tell us the absolute time of recording. "
	"So when you read a sound file into Praat and click ##View & Edit#, you will see "
	"that the Sound starts at a time of 0 seconds, and if its duration is 3.5 seconds, "
	"you will see that the Sound finishes at a time of 3.5 seconds.")
NORMAL (L"Besides sounds, many other types of objects in Praat have a time scale as well: "
	"spectrograms, pitch contours, formant contours, point processes, and so on. None of these "
	"are required to have a time domain that starts at 0 seconds. In the Sound editor window, for example, "
	"you can select the part that runs from 1.4. to 1.7 seconds, and \"extract\" it to the Objects window "
	"while \"preserving the times\". The resulting Sound object will have a start time of 1.4 seconds "
	"and an end time of 1.7 seconds, as you can see when you click ##View & Edit#. "
	"Spectrograms and pitch contours that you create from this sound will also have a time domain "
	"from 1.4 to 1.7 seconds. This time domain is preserved if you save these objects to a text file "
	"or to a binary file and read them into Praat again later. Only if you save the Sound object "
	"to an audio file (WAV, AIFF), the time information is not preserved in that file; "
	"if you read such an audio file into Praat again, the time domain of the new Sound object "
	"will run from 0 to 0.3 seconds.")
NORMAL (L"In order to prevent confusion, Praat always requires times to be expressed in seconds. "
	"So if you want to supply a window length of 5 milliseconds (5 ms), you fill in 0.005 or 5e-3. "
	"For 83.2 microseconds (83.2 \\mus), you say 0.0000832, or better 83.2e-6 or 8.32e-5.")
NORMAL (L"On a clock, time runs around in circles. In Praat's editor windows, time runs from left to right. "
	"You can often see only a part of the time scale in the window. "
	"To see another part, you %scroll backward or forward.")
MAN_END

MAN_BEGIN (L"time domain", L"ppgb", 20110128)
INTRO (L"This manual page assumes that you have read the @Intro.")
NORMAL (L"Many objects in Praat are %%functions of time%. Examples are: "
	"@Sound, @Pitch, @Spectrogram, @Formant, @Intensity, @TextGrid, "
	"@PitchTier, @DurationTier, @Harmonicity, @PointProcess.")
NORMAL (L"In Praat, these functions have a contiguous ##time domain#, i.e. "
	"a single time stretch with a @@start time@ and an @@end time@. "
	"The @@total duration@ of such a function is the difference between the start time "
	"and the end time. There are up to five ways to see the time domain "
	"of an object.")
ENTRY (L"The time domain in editor windows")
NORMAL (L"If you select an object that is a function of time and click ##View & Edit#, an editor window "
	"will appear on the screen. The rectangle at the bottom will show the start time, "
	"the end time, and the total duration.")
ENTRY (L"The time domain in the picture window")
NORMAL (L"If you select an object that is a function of time and choose one of the #Draw commands, "
	"the window that pops up will invite you to supply a time range. If you keep this time range "
	"at its standard setting (from 0.0 to 0.0 seconds), Praat will draw the object for the whole time domain "
	"and print the start time and the end time below the horizontal axis (if #Garnish is on):")
PICTURE (5, 2.5, draw_TimeDomain_Sound)
PICTURE (5, 2.5, draw_TimeDomain_Pitch)
ENTRY (L"The time domain in the Info window")
NORMAL (L"If you select an object that is a function of time and click #Info, "
	"the Info window will tell you the start time, the end time, and the total duration (among other properties of the object).")
ENTRY (L"Time domain query commands")
NORMAL (L"If you select an object that is a function of time, "
	"the following three commands will become available in the #Query menu:")
LIST_ITEM (L"##Get start time")
LIST_ITEM (L"##Get end time")
LIST_ITEM (L"##Get total duration")
NORMAL (L"If you choose one of these commands, the Info window will tell you the result, "
	"expressed in seconds. These commands are most useful in a Praat script. Example:")
CODE (L"select Pitch hello")
CODE (L"startTime = Get start time")
CODE (L"endTime = Get end time")
CODE (L"centreTime = (startTime + endTime) / 2")
CODE (L"echo This Pitch runs from 'startTime' to 'endTime' seconds,")
CODE (L"printline and the centre of its time domain is at 'centreTime' seconds.")
ENTRY (L"Details for hackers")
NORMAL (L"If you select an object that is a function of time and you click @Inspect, "
	"you can see how the time domain information is stored in the object: "
	"the start time is the object's #xmin attribute and the end time is its #xmax attribute. "
	"The total duration is not stored in the object, "
	"because it can easily be computed as #xmax minus #xmin.")
MAN_END

MAN_BEGIN (L"time selection", L"ppgb", 20050924)
INTRO (L"The ways to select a part of the time domain in some @editors in Praat, "
	"namely those that contain a function of time.")
NORMAL (L"The ##time selection# is used for selecting the time interval that will be played, "
	"copied, cut, modified, or questioned:")
ENTRY (L"1. How to make a selection")
NORMAL (L"The easiest way is to @drag the mouse across the part that you want to select. "
	"This is analogous to how selection works in a text processor.")
ENTRY (L"2. How to extend or shrink a selection")
NORMAL (L"The easiest way is to click with the Shift key pressed. "
	"The nearest edge of the selection will move to the time position where you clicked. "
	"This is analogous to how extending a selection works in a text processor.")
NORMAL (L"For instance, if the currently selected time interval runs from 2 to 5 seconds, "
	"and you shift-click at a time position of 4 seconds, the end of the selection will move "
	"from 5 to 4 seconds, thus shrinking the selection.")
NORMAL (L"You can also shift-drag, i.e. hold the Shift key and the mouse button down while moving the mouse.")
ENTRY (L"3. Other tricks")
NORMAL (L"To select a specific time stretch or collapse the selection to its start or end, use the Select menu.")
MAN_END

MAN_BEGIN (L"total duration", L"ppgb", 20040505)
INTRO (L"- the extent of the @@time domain@ (see there).")
MAN_END

MAN_BEGIN (L"vector peak interpolation", L"ppgb", 20010410)
INTRO (L"An algorithm for finding a maximum or a minimum in a sampled signal.")
ENTRY (L"Overview")
NORMAL (L"The signal is described with the sequence %y__%i_, %i = 1...%n, where %n is the number of samples. "
	"Each sample %i is associated with an %x value (typically, time) given by %x__%i_ = %x__1_ + (%i - 1) %dx, "
	"where %dx is the sample period.")
NORMAL (L"The maximum is looked for in two kinds of locations:")
LIST_ITEM (L"1. At the left and right edge, i.e. at %i = 1 and at %i = %n.")
LIST_ITEM (L"2. At or %near all local maxima, i.e. at or %near those %i that satisfy %y__%i-1_ < %y__%i_ \\<_ %y__%i+1_.")
NORMAL (L"The greatest of the following values, therefore, will be the maximum:")
LIST_ITEM (L"1. %y__1_.")
LIST_ITEM (L"2. The local maxima, which are at or %near %y__%i_, where %y__%i-1_ < %y__%i_ \\<_ %y__%i+1_.")
LIST_ITEM (L"3. %y__%n_.")
NORMAL (L"We will now see what %near means. The precision of the result depends on the %%interpolation method% of this algorithm.")
ENTRY (L"1. Lowest precision: round to sample")
NORMAL (L"If the interpolation method is None, the local maxima are %at the samples %m that satisfy %y__%m-1_ < %y__%m_ \\<_ %y__%m+1_. "
	"Thus, their %x values are at %x__%m_ = %x__1_ + (%m - 1) %dx, and their %y values are %y__%m_.")
NORMAL (L"This kind of precision is appropriate for an unordered sequence of values %y__%i_: the result is simply the greatest available value.")
ENTRY (L"2. Middle precision: parabolic interpolation")
NORMAL (L"If the interpolation method is Parabolic, the algorithm uses one point on each side of every local maximum %y__%m_ "
	"to estimate the location and value of the local maximum. Because a Taylor expansion shows that any smooth curve "
	"can be approximated as a parabola in the vicinity of any local maximum, the location %x__%max_ and value %y__%max_ can be found "
	"with the following procedure:")
FORMULA (L"%dy \\=3 1/2 (%y__%m+1_ - %y__%m-1_)")
FORMULA (L"%d^2%y \\=3 2 %y__%m_ - %y__%m-1_ - %y__%m+1_")
FORMULA (L"%m\\'p \\=3 %m + %dy/%d^2%y")
FORMULA (L"%x__%max_ = %x__1_ + (%m\\'p - 1) %dx")
FORMULA (L"%y__%max_ = %y__%m_ + 1/2 %dy^2 / %d^2%y")
NORMAL (L"This kind of precision is appropriate if %y is considered a smooth function of %x, as in:")
LIST_ITEM (L"@@Formant: Get minimum...")
LIST_ITEM (L"@@Formant: Get time of minimum...")
LIST_ITEM (L"@@Formant: Get maximum...")
LIST_ITEM (L"@@Formant: Get time of maximum...")
LIST_ITEM (L"@@Intensity: Get minimum...")
LIST_ITEM (L"@@Intensity: Get time of minimum...")
LIST_ITEM (L"@@Intensity: Get maximum...")
LIST_ITEM (L"@@Intensity: Get time of maximum...")
ENTRY (L"3. Higher precision: cubic interpolation")
NORMAL (L"If the interpolation method is Cubic, the interpolation is performed over four points (see @@vector value interpolation@). "
	"The results are similar to those of the parabolic interpolation method, but you can use it (or sinc interpolation) if you want the result of a "
	"command like ##Get maximum...# to be equal to the result of a sequence of commands like "
	"##Get time of maximum...# and ##Get value at time...#.")
ENTRY (L"4. Highest precision: sinc interpolation")
NORMAL (L"If the interpolation method is Sinc70 or Sinc700, the algorithm assumes that the signal "
	"is a sum of sinc functions, so that a number of points (namely, 70 or 700) on each side of the initial guess %m must be taken into account "
	"(see @@vector value interpolation@). The algorithm finds the maximum of this continuous function by Brent's method (see @@Press et al. (1992)@).")
NORMAL (L"This method is appropriate for signals that result from sampling a continuous signal after it has been low-pass filtered "
	"at the @@Nyquist frequency@. See:")
LIST_ITEM (L"@@Sound: Get minimum...")
LIST_ITEM (L"@@Sound: Get time of minimum...")
LIST_ITEM (L"@@Sound: Get maximum...")
LIST_ITEM (L"@@Sound: Get time of maximum...")
LIST_ITEM (L"@@Sound: Get absolute extremum...")
MAN_END

MAN_BEGIN (L"vector value interpolation", L"ppgb", 19980104)
INTRO (L"An algorithm for estimating the value of a sampled signal at a specified location.")
ENTRY (L"Overview")
NORMAL (L"The signal is described with the sequence %y__%i_, %i = 1...%n, where %n is the number of samples. "
	"Each sample %i is associated with an %x location (typically, time) given by %x__%i_ = %x__1_ + (%i - 1) %dx, "
	"where %dx is the sample period, so that the real-valued sample number associated with a given time %x is")
FORMULA (L"%s = (%x - %x__1_) / %dx + 1")
NORMAL (L"If the resulting %s is an integer number, the %y value must be %y__%s_. Otherwise, the estimated %y value %y(%s) must be interpolated "
	"from nearby values of %y. The precision of the result depends on the %%interpolation method% of this algorithm.")
ENTRY (L"1. Lowest precision: round to sample")
NORMAL (L"If the interpolation method is Nearest, we take the value of the nearest point:")
FORMULA (L"%near \\=3 round (%s)")
FORMULA (L"%y(%s) \\~~ %y__%near_")
ENTRY (L"2. Middle precision: linear interpolation")
NORMAL (L"If you know or assume that the function that underlies your points is continuous, the \"rounding\" interpolation would be poor, "
	"because the rounded value would abruptly change at the centres between the sample points.")
NORMAL (L"For a linear interpolation, therefore, we use the attested values on both sides (%left and %right) of %s:")
FORMULA (L"%s__%l_ \\=3 floor (%s) ;   %s__%r_ \\=3 %s__%l_ + 1")
FORMULA (L"%y(%s) \\~~ %y__%l_ + (%s - %s__%l_) \\.c (%y__%r_ - %y__%l_)")
NORMAL (L"where #floor (%x) computes the greatest integer not greater than %x. This interpolation is continuous.")
ENTRY (L"3. Higher precision: cubic interpolation")
NORMAL (L"If you know or assume that the function that underlies your points is %smooth, i.e. its derivative is defined for every %x, "
	"linear interpolation would probably be poor, because the derivative of the interpolated function would abruptly change at every "
	"sample point.")
NORMAL (L"The next higher interpolation (Cubic), therefore, is differentiable at sample points. To enforce this, we define the "
	"derivatives %y\\'p__%l_ and %y\\'p__%r_ at the left and right sample points on the basis of %their immediate neighbours "
	"(i.e., the algorithm needs four sample points), perhaps by a parabolic interpolation through these three points. "
	"A parabolic interpolation has the advantage that the extrema will be computed correctly if "
	"the underlying function can be approximated by a parabola near its extremes (see @@vector peak interpolation@).")
NORMAL (L"Because the derivative of a parabolic function is a linear function of %x, "
	"the derivatives at the left and right sample points are simply estimated as")
FORMULA (L"%y\\'p__%l_ \\~~ (%y__%r_ - %y__%l-1_) / 2 ;   %y\\'p__%r_ \\~~ (%y__%r+1_ - %y__%l_) / 2")
NORMAL (L"Now that we know %y__%l_, %y__%r_, %y\\'p__%l_, and %y\\'p__%r_, we can fit these values with a "
	"third-degree (%cubic) polynomial:")
FORMULA (L"%As__%l_^3 + %Bs__%l_^2 + %Cs__%l_ + %D = %y__%l_")
FORMULA (L"%As__%r_^3 + %Bs__%r_^2 + %Cs__%r_ + %D = %y__%r_")
FORMULA (L"3%As__%l_^2 + 2%Bs__%l_ + %C = %y\\'p__%l_")
FORMULA (L"3%As__%r_^2 + 2%Bs__%r_ + %C = %y\\'p__%r_")
NORMAL (L"If we shift the %x axis to the left sample point, we can reduce the four equations to")
FORMULA (L"%D = %y__%l_")
FORMULA (L"%A + %B + %C + %D = %y__%r_")
FORMULA (L"%C = %y\\'p__%l_")
FORMULA (L"3%A + 2%B + %C = %y\\'p__%r_")
NORMAL (L"so that the interpolated value %y(%s) at any point %s between %s__%l_ and %s__%r_ is estimated as")
FORMULA (L"(%y\\'p__%r_ + %y\\'p__%l_ - 2%y__%r_ + 2%y__%l_) %\\fi__%l_^3 + "
	"(3%y__%r_ - 3%y__%l_ - 2%y\\'p__%l_ - %y\\'p__%r_) %\\fi__%l_^2 + %y\\'p__%l_ %\\fi__%l_ + %y__%l_")
NORMAL (L"where %\\fi__%l_ \\=3 %s - %s__%l_. Some rearrangement gives")
FORMULA (L"%y(%s) \\~~ %y__%l_ %\\fi__%r_ + %y__%r_ %\\fi__%l_ +")
FORMULA (L"- %\\fi__%l_ %\\fi__%r_ [1/2 (%y\\'p__%r_ - %y\\'p__%l_) + (%\\fi__%l_ - 1/2) "
	"(%y\\'p__%l_ + %y\\'p__%r_ - 2(%y__%r_ - %y__%l_))]")
NORMAL (L"where %\\fi__%r_ \\=3 1 - %\\fi__%l_. From this formula we see:")
LIST_ITEM (L"1. The first two terms define the linear interpolation.")
LIST_ITEM (L"2. If the underlying function is linear, so that %y\\'p__%l_ equals %y\\'p__%r_ and both equal %y__%r_ - %y__%l_, "
	"the higher-degree terms are zero.")
LIST_ITEM (L"3. If %y\\'p__%l_ + %y\\'p__%r_ equals 2(%y__%r_ - %y__%l_), the third-degree term is zero, "
	"so that the interpolated function is parabolic.")
LIST_ITEM (L"4. At the left and right points, one of the %\\fi is 0 and the other is 1, so that at these boundary points, "
	"%y is computed with exact precision.")
ENTRY (L"4. Highest precision: sinc interpolation")
NORMAL (L"If the interpolation method is Sinc70 or Sinc700, the algorithm assumes that the signal "
	"is a sum of sinc functions, so that a number of points (the %%interpolation depth%: 70 or 700) on each side of %s must be taken into account.")
NORMAL (L"Because the interpolation depth must be finite, the sum of sinc functions is multiplied by a Hanning window:")
FORMULA (L"%s__%l_ \\=3 floor (%s);   %s__%r_ \\=3 %s__%l_ + 1")
FORMULA (L"%\\fi__%l_ \\=3 %s - %s__%l_;   %\\fi__%r_ \\=3 1 - %\\fi__%l_")
FORMULA (L"%y(%s) \\~~ \\su__%i=1...%N_ %y__%r-%i_ sinc (%\\pi(%\\fi__%l_+%i-1)) (1/2 + 1/2 cos (%\\pi(%\\fi__%l_+%i-1)/(%\\fi__%l_+%N))) +")
FORMULA (L"+ \\su__%i=1...%N_ %y__%l+%i_ sinc (%\\pi(%\\fi__%r_+%i-1)) (1/2 + 1/2 cos (%\\pi(%\\fi__%r_+%i-1)/(%\\fi__%r_+%N)))")
NORMAL (L"where the sinc function is defined as")
FORMULA (L"sinc (0) \\=3 1;   sinc (%x) \\=3 sin %x / %x   for %x \\=/ 0")
NORMAL (L"If %s is less than the interpolation depth or greater than %n + 1 minus the interpolation depth, "
	"the depth is reduced accordingly.")
NORMAL (L"This method is appropriate for signals that result from sampling a continuous signal after it has been low-pass filtered "
	"at the @@Nyquist frequency@. See:")
LIST_ITEM (L"@@Sound: Get value at time...")
MAN_END

MAN_BEGIN (L"waveform", L"ppgb", 20060914)
INTRO (L"The waveform is the sound pressure as a function of time, or: the microphone output as a function of time. "
	"In Praat, the waveform is shown in the top half of the Sound window (see the @Intro).")
MAN_END
}

/* End of file manual_glossary.cpp */
