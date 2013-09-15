/* manual_LPC.c
 *
 * Copyright (C) 1994-2013 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020422 GPL + removed "C syntax" part of manpage
 djmw 20101009 Latest modification
*/

#include "ManPagesM.h"
#include "MFCC.h"

void manual_LPC_init (ManPages me);
void manual_LPC_init (ManPages me)
{

MAN_BEGIN (L"CC: Paint...", L"djmw", 20040407)
INTRO (L"A command to paint the cepstral coefficients in shades of grey.")
ENTRY (L"Settings")
TAG (L"##From coefficient#, ##To coefficient#")
DEFINITION (L"the range of coefficients that will be represented.")
MAN_END

MAN_BEGIN (L"CC: To DTW...", L"djmw", 19960918)
INTRO (L"You can choose this command after selecting 2 objects with cepstral "
	"coefficients (two @MFCC's or @LFCC's). "
	"With this command you perform dynamic time warping. ")
ENTRY (L"Algorithm")
NORMAL (L"First we calculate distances between cepstral coefficients: ")
LIST_ITEM (L"The distance between frame %i (from me) and %j (from thee) is:")
LIST_ITEM (L"    %wc \\.c %d1 + %wle \\.c %d2 + %wr \\.c %d3,")
LIST_ITEM (L"    where %wc, %wle & %wr are user-supplied weights and")
LIST_ITEM (L"      %d1 = \\su (%k=1..%nCoefficients; (%c__%ik_ - %c__%jk_)^2)")
LIST_ITEM (L"      %d2 = (c__%i0_ - c__%j0_)^2")
LIST_ITEM (L"      %d3 = \\su (%k=1..%nCoefficients; (%r__%ik_ - %r__%jk_)^2), with ")
LIST_ITEM (L"      %r__%ik_ the regression coefficient of the cepstral coefficients "
	"from the frames within a time span of %dtr seconds. "
	"c__%ij_ is %j-th cepstral coefficient in frame %i. ")
NORMAL (L"Next we find the optimum path through the distance matrix with a "
	"Viterbi-algorithm.")
MAN_END

MAN_BEGIN (L"CC: To Matrix", L"djmw", 20011123)
INTRO (L"Copies the cepstral coefficients of the selected @CC "
	"object to a newly created @Matrix object.")
ENTRY (L"Behaviour")
FORMULA (L"%z__%ji_ = %c__%ij_, with 1 \\<_ %i \\<_ %nx and "
	"1 \\<_ j \\<_ %numberOfCoefficients__%i_,")
NORMAL (L"where %z__%ji_ is the matrix element in row %j and column %i and "
	"%c__%ij_ is the %j-th cepstral coefficient in frame %i.")
MAN_END

MAN_BEGIN (L"PowerCepstrogram", L"djmw", 20130616)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
ENTRY (L"Description")
NORMAL (L"The PowerCepstrogram shows @@PowerCepstrum|cepstral slices@ as a function of time.")
MAN_END

MAN_BEGIN (L"PowerCepstrogram: To Table (peak prominence)...", L"djmw", 20130616)
INTRO (L"A command to create a table with @@PowerCepstrum: Get peak prominence...|cepstral peak prominence@ values.")
ENTRY (L"Settings")
SCRIPT (7, Manual_SETTINGS_WINDOW_HEIGHT (7), L""
	Manual_DRAW_SETTINGS_WINDOW ("PowerCepstrogram: To Table (peak prominence)", 7)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Peak search pitch range (Hz)", L"60.0", L"300.0")
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"Interpolation", L"None", 0)
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"", L"Parabolic", 0)
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"", L"Cubic", 1)
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"", L"Sinc70", 0)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Tilt line quefrency range (s)",L"0.001", L"0.0 (=end)")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU(L"Fit method", L"Robust")
)
NORMAL (L"The meaning of these settings is explained @@PowerCepstrum: Get peak prominence...|here@.")
MAN_END

MAN_BEGIN (L"PowerCepstrogram: Smooth...", L"djmw", 20130410)
INTRO (L"Smoothes the selected @PowerCepstrogram by averaging cepstra. The smoothed PowerCepstrogram is the result of two separate steps. "
	"In the first step, cepsta are averaged across time. In the second step, cepstra are averaged across quefrency.")
ENTRY (L"Settings")
TAG (L"##Time averaging window (s)")
DEFINITION (L"determines how many frames will used in the first step, averaging across time. The user-supplied value will be divided "
	"by the Cepstrograms's time step value (its %dx). If %%numberOfFramesToAverage%, the result of the division, turns out to be one or less, no averaging across time is performed. "
	"If %%numberOfFramesToAverage% is larger than one and is even, one will be added. "
	"Each new cepstral frame will be the average of %numberOfFramesToAverage frames of the input Cepstrogram. "
	"For example, if %numberOfFramesToAverage turns out to be 5, then the %j-th new cepstral frame is the result of averaging the 5 frames with indices %j\\--2 , %j\\--1, %j, %j+1 and %j+2 for all frames %j=3..%%numberOfFrames%\\--2, i.e. besides frame %j, the 2 frames on either side are used in the averaging. The %numberOfFramesToAverage has to be uneven to allow for this symmetric behaviour. ")
TAG (L"##Quefrency averaging window (s)")
DEFINITION (L"determines how many quefrency bins will be used for the averaging across quefrency step. The number of bins used in this step "
	"is the result of the division of the user-supplied value by the quefrency step value (the Cepstrogram's %dy). "
	"If the result turns out to be one or less, no averaging across quefrencies is performed. If the resulting value is even, one will be added. "
	"If, for example, the result happens to be 3 then the value in quefrency bin %k will be the average value of the values in quefrency bins "
	"%k\\--1, %k and %k+1. ")
ENTRY (L"Note")
NORMAL (L"The following commands should reproduce the smoothing described in the @@Hillenbrand & Houde (1996)@ article, where they use a 20 ms "
	"(10 frame) time smoothing and a 1 ms (10 bin) quefrency smoothing. ")
CODE (L"selectObject (\"Sound xxx\")")
CODE (L"do (\"To PowerCepstrogram...\", 0.041, 0.002, 5000.0)")
CODE (L"do (\"Smooth...\", 0.02, 0.001)")
MAN_END

MAN_BEGIN (L"Cepstrum", L"djmw", 20130616)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
ENTRY (L"Description")
NORMAL (L"A Cepstrum is the log spectrum of the log power spectrum.")
MAN_END

MAN_BEGIN (L"PowerCepstrum", L"djmw", 20130616)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
ENTRY (L"Description")
NORMAL (L"A PowerCepstrum is the log power spectrum of the log power spectrum. The vertical scale will show the amplitude expressed in dB. The horizontal scale shows %%quefrency% in units of seconds.")
MAN_END

MAN_BEGIN (L"PowerCepstrum: Get peak prominence...", L"djmw", 20130616)
INTRO (L"Calculates the cepstral peak prominence measure (CPP) as defined by @@Hillenbrand et al. (1994)@")
NORMAL (L"The CPP measure is the difference in amplitude between the cepstral peak and the corresponding value on the regression "
	"line that is directly below the peak (i.e., the predicted magnitude for the quefrency at the cepstral peak). "
	"The CPP measure represents how far the cepstral peak emerges from the cepstrum background. ")
ENTRY (L"Settings")
SCRIPT (7, Manual_SETTINGS_WINDOW_HEIGHT (7), L""
	Manual_DRAW_SETTINGS_WINDOW (L"PowerCepstrum: Get peak prominence", 7)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Search peak in pitch range (s)", L"60.0", L"333.3")
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"Interpolation", L"None", 0)
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"", L"Parabolic", 0)
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"", L"Cubic", 1)
	Manual_DRAW_SETTINGS_WINDOW_RADIO (L"", L"Sinc70", 0)
	Manual_DRAW_SETTINGS_WINDOW_RANGE (L"Tilt line quefrency range (s)", L"0.001", L"0.0 (=end)")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU (L"Fit method", L"Robust")
)
TAG (L"##Search peak in pitch range")
DEFINITION (L"determine the limits of the quefrency range where a peak is searched for. The lower quefrency is determined as "
	"1 / %%pitchCeiling% and this value is in general more critical than "
	"the value of the upper quefrency which equals 1 / %%pitchFloor%. A %%pitchCeiling% of 300 Hz will correspond to a lower quefrency of 1/300\\~~0.0033 seconds.")
TAG (L"##Interpolation")
DEFINITION (L"determines how the @@vector peak interpolation|amplitude of a peak is determined@.")
TAG (L"##Tilt line quefrency range")
DEFINITION (L"the quefrency range for which the amplitudes (in dB) will be modelled by a straight line. "
	"The lower value for this range in the Hillenbrand article was chosen as 0.001 s "
	"in order to reduce the effect of the low quefrency data on the straight line fit. In our analysis this value is not so critical "
	"as we use a more robust straight line fit.")
TAG (L"##Fit method")
DEFINITION (L"the default method is @@theil regression|Theil's robust line fit@. However, to be compatible with the past, a standard least squares line fit has also  been implemented.")
ENTRY (L"Note")
NORMAL (L"The CPP value does not depend on the reference value used in the dB calculation of the power cepstrum.")
MAN_END

MAN_BEGIN (L"PowerCepstrum: Draw tilt line...", L"djmw", 20130616)
INTRO (L"Draws the line that models the backgound of the power cepstrum.")
MAN_END

MAN_BEGIN (L"Formant & Spectrogram: To IntensityTier...", L"djmw", 20130109)
INTRO (L"Determines the formant intensities from the selected Spectrogram.")
NORMAL (L"The intensities at the frequencies of the selected formant are copied from the corresponding positions in the spectrogram. "
	"If the selected formant doesn't exist in a particular frame, then a large negative value (< -200 dB) is substituted instead. "
	"Because the values in the spectrogram are expressed in Pa^^2^/Hz, the units in the intensity tier are in dB/Hz. ")
MAN_END

MAN_BEGIN (L"LFCC", L"djmw", 20040421)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type LFCC represents cepstral "
	"coefficients on a linear frequency scale as a function of time. "
	"The coefficients are represented in frames with constant sampling "
	"period.")
ENTRY (L"#LFCC commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@LPC: To LFCC...")
MAN_END

MAN_BEGIN (L"LFCC: To LPC...", L"djmw", 20040407)
INTRO (L"You can choose this command after selecting 1 or more @LFCC's.")
ENTRY (L"Settings")
TAG (L"##Number of coefficients")
DEFINITION (L"the desired number of linear predictive coefficients.")
ENTRY (L"Behaviour")
NORMAL (L"The transformation from cepstral coefficients to %a-coefficients "
	"as described in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN (L"LPC", L"djmw", 19990610)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type LPC represents filter coefficients as a function of time. "
	"The coefficients are represented in frames with constant sampling period.")
ENTRY (L"LPC commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Sound: To LPC (autocorrelation)...@")
LIST_ITEM (L"\\bu @@Sound: To LPC (covariance)...@")
LIST_ITEM (L"\\bu @@Sound: To LPC (burg)...@")
LIST_ITEM (L"\\bu @@Sound: To LPC (marple)...@")
ENTRY (L"Conversion")
LIST_ITEM (L"\\bu @@LPC: To LFCC...|To LFCC...@")
LIST_ITEM (L"\\bu @@LPC: To Spectrogram...|To Spectrogram...@")
LIST_ITEM (L"\\bu @@LPC: To Spectrum (slice)...|To Spectrum (slice)...@")
LIST_ITEM (L"\\bu @@LPC: To Polynomial (slice)...|To Polynomial (slice)...@")
MAN_END

MAN_BEGIN (L"LPC: Draw gain...", L"djmw", 20040407)
INTRO (L"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (L"Settings")
TAG (L"##From time (s)#, ##To time (seconds)#")
DEFINITION (L"the time domain along the %x-axis.")
TAG (L"##Minimum gain#, ##Maximum gain#")
DEFINITION (L"the range for the %y-axis.")
TAG (L"##Garnish")
DEFINITION (L"determines whether to draw a bounding box and axis labels.")
ENTRY (L"Behaviour")
NORMAL (L"Gain will be drawn as a function of time (gain also equals the prediction error "
	"energy). ")
MAN_END

MAN_BEGIN (L"LPC: Draw poles...", L"djmw", 20040407)
INTRO (L"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time of the nearest frame.")
ENTRY (L"Behaviour")
NORMAL (L"The @@Roots|roots@ of the @@LPC: To Polynomial (slice)...|linear prediction "
	"polynomial@, constructed from the coefficients "
	"of the analysis frame, will be drawn in the complex plane.")
MAN_END

MAN_BEGIN (L"LPC: To LFCC...", L"djmw", 20040407)
INTRO (L"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (L"Behaviour")
NORMAL (L"The transformation from a-coefficients to cepstral coefficients "
	"as described in @@Markel & Gray (1976)@.")
ENTRY (L"Settings")
TAG (L"%%Number of coefficients%")
DEFINITION (L"the desired number of cepstral coefficients.")
MAN_END

MAN_BEGIN (L"LPC: To Matrix", L"djmw", 20011123)
INTRO (L"Copies the linear prediction coefficients of the selected @LPC "
	"object to a newly created @Matrix object.")
ENTRY (L"Behaviour")
FORMULA (L"%z__%ji_ = %a__%ij_, with 1 \\<_ %i \\<_ %nx and "
	"1 \\<_ j \\<_ %nCoefficients__%i_,")
NORMAL (L"where %z__%ji_ is the matrix element in row %j and column %i and "
	"%a__%ij_ is the %j-th linear prediction coefficient in frame %i.")
MAN_END

MAN_BEGIN (L"LPC: To Formant", L"djmw", 19970123)
INTRO (L"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (L"Behaviour")
NORMAL (L"For each LPC_Frame, the zeros of the linear prediction polynomial are extracted. "
	"Zeros that are outside the unit circle are reflected into it. "
	"Next, formant frequencies and bandwidths are calculated from all the roots that have the "
	"imaginary part positive, i.e., that lie in the upper half of the unit circle. "
	"Formant frequencies smaller than 50 Hz or larger than (%Nyquist_frequency - 50) are discarded. "
	"The remaining frequencies and bandwidths are sorted "
	"and copied to the Formant_Frame. Finally, the %gain field of the LPC is copied to the %intensity "
	"field of the Formant_Frame.")
ENTRY (L"Algorithm")
NORMAL (L"The root finder is Laguerre's method followed by root polishing, see @@Press "
	"et al. (1992)@.")
ENTRY (L"Warning")
LIST_ITEM (L"\\bu The formant values can be very inaccurate if you did not resample the Sound "
	"before the LPC-analysis (consult the @@Source-filter synthesis@ tutorial).")
LIST_ITEM (L"\\bu The results of the root finder may not always be accurate when more than 30 "
	"roots have to be found.")
MAN_END

MAN_BEGIN (L"LPC: To Polynomial (slice)...", L"djmw", 20040407)
INTRO (L"A command that creates a Polynomial object from each selected @LPC object.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"defines the LPC frame whose coefficents will be selected.")
ENTRY (L"Behaviour")
NORMAL (L"The linear prediction coefficients %a__1..%n_ of the selected LPC "
	"frame will be copied to polynomial coefficients %c__1..%n+1_ as follows:")
FORMULA (L"%c__%i_ = %a__%n\\--%i+1_, ")
FORMULA (L"%c__%n+1_ = 1")
MAN_END

MAN_BEGIN (L"LPC: To Spectrum (slice)...", L"djmw", 20071120)
INTRO (L"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time at which the Spectrum must be calculated.")
TAG (L"##Minimum frequency resolution (Hz)")
DEFINITION (L"successive frequencies in the @Spectrum "
	"will be maximally this distance apart.")
TAG (L"##Bandwidth reduction (Hz)")
DEFINITION (L"formants with small bandwidths show up very well as peaks in the spectrum because the poles "
	"lie close to the contour along which the spectrum is computed (the unit circle in the z-plane). "
	"Peak enhancement can be realized by computing the spectrum in the z-plane along a contour of radius "
	"%r = exp (\\-- %\\pi \\.c %bandwidthReduction / %samplingFrequency). "
	"This technique is also called off-axis spectrum computation. "
	"Negative values evaluate the spectrum on a contour %outside the unit circle and therefore result in a "
	"flattened  spectrum.")
TAG (L"##De-emphasis frequency (Hz)")
DEFINITION (L"Performs de-emphasis when frequency is in the interval (0, @@Nyquist frequency@)")
ENTRY (L"Algorithm")
NORMAL (L"The Spectrum at time %t will be calculated from the %nearest LPC_Frame according to:")
FORMULA (L"Spectrum (%f) = \\Vr(%gain\\.c%T/%df) / (1 + \\su__%k=1..%numberOfCoefficients_ %a__%k_%z^^\\--%k^),")
NORMAL (L"where %T is the sampling period and %z = exp (\\--2 %\\pi %i %f %T) and %df is the distance in Hz "
	"between two successive components in the Spectrum.")
LIST_ITEM (L"1. Allocate a large enough buffer[1..%nfft] to perform an FFT analysis.")
LIST_ITEM (L"2. Make the first value of the buffer 1 and copy the prediction coefficients #a into "
	"the buffer. This results in buffer values: (1, %a__1_, ..., %a__%numberOfCoefficients_, 0, ..., 0).")
LIST_ITEM (L"3. If ##De-emphasis frequency# is in the interval (0, %nyquistFrequency) then \"multiply\" "
	"the buffer with (1 - %b %z^^\\--1^), where %b = exp (\\-- %\\pi %deEmphasisFrequency %T). "
	"This results in buffer values: (1, %a__1_\\--%b, %a__2_\\--%b\\.c%a__1_, ..., "
	"%a__%numberOfCoefficients_\\--%b\\.c%a__%numberOfCoefficients\\--1_, "
	"\\--%b\\.c%a__%numberOfCoefficients_, 0, ..., 0). Note that the number of values in the buffer that differ from 0 "
	"has increased by one.")
LIST_ITEM (L"4. If ##Bandwidth reduction# is greater than 0 then multiply corresponding values in the buffer by %g^^%i\\--1^ where "
	"%g = exp (2%\\pi %bandwidthReduction %T / %nfft), and %i is the position index in the buffer. "
	"%i runs from 1 to %numberOfCoefficients+1+%t, where %t equals 1 when de-emphasis was performed, "
	"else 0.")
LIST_ITEM (L"5. Calculate the FFT spectrum of the buffer with the coefficients. This results in complex "
	"amplitudes (%a__%j_,%b__%j_), %j=1..%nfft/2+1.")
LIST_ITEM (L"6. Calculate the LPC Spectrum by taking the inverse of the FFT spectrum, i.e., each complex "
	"amplitude becomes (%a__%j_,%b__%j_)^^\\--1^ = (%a__%j_,\\--%b__%j_) / (%a__%j_^2 + %b__%j_^2)")
LIST_ITEM (L"7. Multiply all values with the scale factor \\Vr(%gain\\.c%T/%df).")
MAN_END

MAN_BEGIN (L"LPC: To Spectrogram...", L"djmw", 20040407)
INTRO (L"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (L"Settings")
TAG (L"##Minimum frequency resolution (Hz)")
DEFINITION (L"successive frequencies in the Spectrum will be maximally this distance apart")
TAG (L"##Bandwidth reduction (Hz)")
DEFINITION (L"formants with small bandwidths show up very well as darker regions in the spectrogram "
	"because the poles lie close to the contour along which a spectrum is computed (the unit circle "
	"in the z-plane). "
	"Peak enhancement can be realized by computing a spectrum in the z-plane along a contour of radius "
	"%r = exp (\\-- %\\pi \\.c %bandwidthReduction / %samplingFrequency).")
TAG (L"##De-emphasis frequency (Hz)")
DEFINITION (L"Performs de-emphasis when value is in the interval (0, @@Nyquist frequency@)")
ENTRY (L"Algorithm")
NORMAL (L"For each LPC_Frame the corresponding Spectrum will be calculated according to the algorithm "
	"explained in @@LPC: To Spectrum (slice)...@. "
	"For each frequency the power, i.e., the square of the complex values, will be stored in the "
	"corresponding area in the Spectrogram.")
MAN_END

MAN_BEGIN (L"LPC: To VocalTract (slice)...", L"djmw", 20050615)
INTRO (L"You can choose this command after selecting 1 or more @LPC objects.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time of the nearest frame, in seconds.")
TAG (L"##Length (m)")
DEFINITION (L"the length of the vocal tract, in metres.")
/*
TAG (L"##Compute length according to Wakita")
DEFINITION (L"the length of the vocal tract is calculated according "
	"to the algorithm as described in @@Wakita (1977)@.")
ENTRY (L"Behaviour")
NORMAL (L"A new @VocalTract area function is calculated from the prediction coefficients in the frame. ")
ENTRY (L"Warning")
NORMAL (L"If ##Compute length according to Wakita# is on, the optimal length is searched for in the range from 0.1 m to "
	"0.25 m. This length calculation is extremely sensitive to the number of and the positions of the formants "
	"with respect to the @@Nyquist frequency@. For example, there is a large difference "
	"between the vocal tract length estimates if the highest formant is just below or just above the "
	"Nyquist frequency. "
	"The algorithm is not very reliable in vocal tract length estimation and we do not recommend using it. " )*/
MAN_END

MAN_BEGIN (L"LPC & Sound: Filter...", L"djmw", 20040407)
INTRO (L"A command that creates a new Sound object from one @Sound and one @LPC "
	"object which have been selected together.")
ENTRY (L"Settings")
TAG (L"##Use LPC gain")
DEFINITION (L"Determines whether the gain from the LPC is used in the synthesis.")
ENTRY (L"Behaviour")
NORMAL (L"Filters the selected Sound by the selected LPC-filter.")
NORMAL (L"When the LPC-gain is used the samples in the new Sound will be "
	"multiplied with the square root of the corresponding LPC-gain value.")
NORMAL (L"In #Z-domain notation: #O(%z) = #H(%z) \\.c #E(%z), where "
	"#E(%z) is the selected filter input Sound, #H(%z) the selected LPC filter, "
	"and, #O(%z) the filter output (the new Sound that will appear in the List of objects).")
MAN_END

MAN_BEGIN (L"LPC & Sound: Filter with filter at time...", L"djmw", 20101009)
INTRO (L"Filters the selected @Sound with a static filter that is formed by the filter coefficients "
	"from only one @LPC frame.")
ENTRY (L"Settings")
TAG (L"##Channel")
DEFINITION (L"determines the sound channel to be filtered.")
TAG (L"##Use filter at time (s)")
DEFINITION (L"determines which LPC frame will be chosen to filter the sound. ")
MAN_END

MAN_BEGIN (L"LPC & Sound: Filter (inverse)", L"djmw", 19970126)
INTRO (L"A command that creates a new Sound object from one @Sound and one @LPC "
	"object which have been selected together.")
ENTRY (L"Behaviour")
NORMAL (L"Given a filter (the selected LPC) and its output (the selected Sound), "
	"its input is reconstructed (the new Sound that will appear in the List of objects).")
NORMAL (L"In Z-domain notation: #E(%z) = #O(%z) / #H(%z), where "
	"#O(%z) is the filter output Sound, #H(%z) the LPC filter, and, #E(%z) the filter "
	"input Sound. (Selecting this newly generated Sound and the LPC, choosing the option "
	"`Filter...' generates a Sound that is identical to the Sound that originated "
	"the LPC.)")
MAN_END

MAN_BEGIN (L"LPC & Sound: Filter (inverse) with filter at time...", L"djmw", 20101009)
INTRO (L"%%Inverse% filters the selected @Sound with a static inverse filter that is formed by the filter coefficients "
	"from only one @LPC frame.")
ENTRY (L"Settings")
TAG (L"##Channel")
DEFINITION (L"determines the sound channel to be filtered.")
TAG (L"##Use filter at time (s)")
DEFINITION (L"determines which LPC frame will be chosen to inverse filter the sound. ")
MAN_END

MAN_BEGIN (L"MFCC", L"djmw", 20010411)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type MFCC represents mel frequency cepstral coefficients "
	"as a function of time. The coefficients are represented in frames "
	"with constant sampling period.")
ENTRY (L"MFCC commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Sound: To MFCC...@")
LIST_ITEM (L"\\bu @@MelFilter: To MFCC...@")
MAN_END

MAN_BEGIN (L"MFCC: To MelFilter...", L"djmw", 20130221)
INTRO (L"A command to reconstruct @MelFilter objects  from the selected @MFCC "
	"objects .")
ENTRY (L"Settings")
TAG (L"##From coefficient#, ##To coefficient#")
DEFINITION (L"the range of coefficients that will be used in the reconstruction.")
ENTRY (L"Details")
NORMAL (L"The output of the triangular filters in a mel filter bank will be "
	"reconstructed by applying the inverse cosine transform:")
FORMULA (L"%P__%j_ = 2/N (%c__0_/2 + \\Si__%k=1_^^%N-1^ %c__%k_ cos (\\pi%k(%j-0.5)/%N))),")
NORMAL (L"where %N represents the number of filters, %j runs from 1 to %N, and coefficients %c__%k_ with %k less than "
	"%%fromCoefficient% and %k larger than %%toCoefficient% take zero values in the evaluation.")
MAN_END

MAN_BEGIN (L"Sound: To PowerCepstrogram...", L"djmw", 20130616)
INTRO (L"A command that creates a @@PowerCepstrogram@ from every selected @@Sound@.")
ENTRY (L"Settings")
TAG (L"##Pitch floor (Hz)")
DEFINITION (L"determines the effective length of the analysis window: it will be 3 longest periods long, i.e. if the pitch floor is 60 Hz, the window will be 3/60 = 0.05 seconds long.")
TAG (L"##Time step (s)")
TAG (L"##Maximum frequency (Hz)")
TAG (L"##Pre-emphasis from (Hz)")
MAN_END

MAN_BEGIN (L"Sound: To Formant (robust)...", L"djmw", 20111027)
INTRO (L"A command that creates a @@Formant@ object from every selected @@Sound@. ")
ENTRY (L"Settings")
NORMAL (L"The settings for ##Time step (s)#, ##Maximum number of formants#, ##Maximum formant (Hz), "
	"##Window length (s)# and ##Pre emphasis from (Hz)# are as in @@Sound: To Formant (burg)...@. "
	" The following settings determine aspects of the iterative formant frequency refinement.")
TAG (L"%%Number of std. dev.%,")
DEFINITION (L"determines the number of standard deviation from where selective weighing of samples starts. ")
TAG (L"%%Maximum number of iterations%,")
DEFINITION (L"determines the maximum number of iterations allowed in the refinement step.")
TAG (L"%%Tolerance%,")
DEFINITION (L"detemines another stop ctriterion for the refinement step. If the relative change in variance "
	"between successive iterations is less then this value, iteration stops. Iteration stops whenever "
	"one of the two defined stop criteria is reached.")
ENTRY (L"Algorithm")
NORMAL (L"First the sound is downsampled to twice the maximum formant frequency. Next the LPC coefficients are determined by the autocorrelation method. Finally, in an iterative procedure as described by @@Lee (1988)@ the formant frequencies and bandwidths are refined by selectively weighting of samples values.")
MAN_END

MAN_BEGIN (L"Sound: LPC analysis", L"djmw", 19970126)
INTRO (L"You can perform this analysis by selecting one or more @Sound objects and "
	"choosing the appropriate command to generate an @LPC.")
NORMAL (L"The acronym LPC stands for Linear Predictive Coding.")
NORMAL (L"In the LPC analysis one tries to predict %x__%n_ on the basis of the %p previous samples,")
FORMULA (L"%x\\'p__%n_ = \\su %a__%k_ %x__%%n-k%_")
NORMAL (L"then {%a__1_, %a__2_, ..., %a__%p_} can be chosen to minimize the prediction power %%Q__p_% where")
FORMULA (L"%%Q__p_% = E[ |%x__%n_ - %x\\'p__%n_|^2].")
NORMAL (L"Several different algorithms exist for minimizing %%Q__p_%:")
LIST_ITEM (L"\\bu @@Sound: To LPC (autocorrelation)...|To LPC (autocorrelation)...@")
LIST_ITEM (L"\\bu @@Sound: To LPC (covariance)...|To LPC (covariance)...@")
LIST_ITEM (L"\\bu @@Sound: To LPC (burg)...|To LPC (burg)...@")
LIST_ITEM (L"\\bu @@Sound: To LPC (marple)...|To LPC (marple)...@")
MAN_END

#define Sound_to_LPC_COMMON_HELP(method) \
INTRO (L"With this command you create a new @LPC from every selected @Sound, " \
	"using " #method " method.") \
ENTRY (L"Warning") \
NORMAL (L"You are advised not to use this command for formant analysis. " \
	"For formant analysis, instead use @@Sound: To Formant (burg)...@, " \
	"which also works via LPC (linear predictive coding). This is because " \
	"##Sound: To Formant (burg)...# lets you specify a maximum frequency, " \
	"whereas the ##To LPC# commands automatically use the @@Nyquist frequency@ " \
	"as their maximum frequency. If you do use one of the ##To LPC# commands " \
	"for formant analysis, you may therefore want to downsample the sound first. " \
	"For instance, if you want five formants below 5500 Hz but your Sound has a sampling frequency " \
	"of 44100 Hz, you have to downsample the sound to 11000 Hz with the @@Sound: Resample...@ command. " \
	"After that, you can use the ##To LPC# commands, with a prediction order of 10 or 11.") \
ENTRY (L"Settings") \
TAG (L"##Prediction order#") \
DEFINITION (L"the number of linear prediction coefficients, also called the %%number of poles%. " \
	"Choose this number at least twice as large as the number of spectral peaks that you want " \
	"to detect.") \
TAG (L"##Analysis window duration (s)") \
DEFINITION (L"the effective duration of each analysis frame, in seconds.") \
TAG (L"##Time step (s)") \
DEFINITION (L"the time step between two consecutive analysis frames.") \
TAG (L"##Pre-emphasis frequency (Hz)") \
DEFINITION (L"a +6dB / octave filtering will be applied above this frequency. " \
	"A pre-emphasis frequency of 48.47 Hz for a signal with a sampling frequency of 10 kHz " \
	"approximately corresponds to a value of %a = 0.97 for the filter %y__%n_ = %x__%n_ - %a \\.c %x__%n-1_. " \
	"The relation between %a and the pre-emphasis frequency is: " \
	"%a = exp (\\--2\\.c%\\pi\\.c%preemphasisFrequency/%samplingFrequency). " \
	"If you do not want pre-emphasis, choose a frequency greater than the @@Nyquist frequency@.")

MAN_BEGIN (L"Sound: To LPC (autocorrelation)...", L"David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("the autocorrelation")
ENTRY (L"Algorithm")
NORMAL (L"The autocorrelation algorithm is decribed in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN (L"Sound: To LPC (covariance)...", L"David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("the covariance")
ENTRY (L"Algorithm")
NORMAL (L"The covariance algorithm is decribed in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN (L"Sound: To LPC (marple)...", L"djmw", 19970126)
Sound_to_LPC_COMMON_HELP ("Marple's")
TAG (L"##Tolerance 1")
DEFINITION (L"stop the iteration when %E(%m) / %E(0) < %%Tolerance 1%, where %E(%m) is the "
	"prediction error for order %m.")
TAG (L"##Tolerance 2")
DEFINITION (L"stop the iteration when (%E(%m) - %E(%m-1)) / %E(%m-1) < %%Tolerance 2.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm is described in @@Marple (1980)@.")
MAN_END

MAN_BEGIN (L"Sound: To LPC (burg)...", L"David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("Burg's")
ENTRY (L"Algorithm")
NORMAL (L"Burg's algorithm is described in @@Anderson (1978)@")
MAN_END

MAN_BEGIN (L"Sound: To MFCC...", L"djmw", 20010410)
INTRO (L"A command that creates a @MFCC object from every selected @Sound "
	"object.")
NORMAL (L"The analysis proceeds in two steps:")
LIST_ITEM (L"1.  We perform a filter bank analysis on a mel frequency scale "
	"(see @@Sound: To MelFilter...@ for details).")
LIST_ITEM (L"2.  We convert the filter values to mel frequency cepstral "
	"coefficients (see @@MelFilter: To MFCC...@ for details).")
MAN_END

MAN_BEGIN (L"VocalTractTier", L"djmw", 20120423)
INTRO (L"One of the @@types of objects@ in Praat. A VocalTractTier objects contains a number of (%time, %VocalTract) points, where a @@VocalTract@ represents the area function of the vocal tract expressed as m^^2^, running from the glottis to the lips.")
MAN_END

MAN_BEGIN (L"theil regression", L"djmw", 20130710)
NORMAL (L"a robust linear regression method, first proposed by @@Theil (1950)@. The slope of the regression line is estimated as "
	"the median of all pairwise slopes between each pair of points in the data set. Because this number of pairs increases quadratically "
	"with the number of data points, we have implemented a somewhat less computationally intensive procedure, the %%incomplete% theil regression. In the incomplete method we first split the data set of %N data points (%x__%i_, %y__%i_), %i = 1..%N, in two equal sets "
	"of size %N/2 and then calculate %N/2 slopes as ")
FORMULA (L"%m__%i_ = (%y__%N/2+%i_ - %y__%i_) / (%x__%N/2+%i_ - %x__%i_), for %i = 1..%N/2.")
NORMAL (L"The regression slope %m is calculated as the median of these %N/2 values %m__%i_.")
NORMAL (L"Given the slope %m, the offset %b is calculated as the median of the %N values %b__%i_= %y__%i_ - %m\\.c%x__%i_.")
NORMAL (L"The theil regression has a breakdown point of 29.3\\% , which means that it can tolerate arbitrary corruption of up to 29.3% of the input data-points without degradation of its accuracy")
MAN_END

MAN_BEGIN (L"Anderson (1978)", L"djmw", 20030701)
NORMAL (L"N. Anderson (1978): \"On the calculation of filter coefficients for "
	"maximum entropy spectral analysis.\" In Childers: %%Modern Spectrum Analysis%, "
	"IEEE Press: 252\\--255.")
MAN_END

MAN_BEGIN (L"Hillenbrand et al. (1994)", L"djmw", 20121017)
NORMAL (L"J. Hillenbrand, R.A. Cleveland & R.L. Erickson (1994): \"Acoustic correlates of breathy vocal quality\", %%Journal of speech and hearing research% #37: 769\\--778.")
MAN_END

MAN_BEGIN (L"Hillenbrand & Houde (1996)", L"djmw", 20121203)
NORMAL (L"J. Hillenbrand & R.A. Houde (1996): \"Acoustic correlates of breathy vocal quality: Dysphonic voices and continuous speech\", %%Journal of speech and hearing research% #39: 311\\--321.")

MAN_END

MAN_BEGIN (L"Lee (1988)", L"djmw", 20111027)
NORMAL (L"C.-H. Lee (1988): \"On Robust Linear Prediction of Speech.\", %%IEEE Trans. on ASSP% #36: 642\\--649.")
MAN_END

MAN_BEGIN (L"Markel & Gray (1976)", L"djmw", 19980114)
NORMAL (L"J.D. Markel & A.H. Gray, Jr. (1976): %%Linear Prediction of Speech.% "
	"Springer Verlag, Berlin.")
MAN_END


MAN_BEGIN (L"Marple (1980)", L"djmw", 19980114)
NORMAL (L"L. Marple (1980): \"A new autoregressive spectrum analysis algorithm.\" "
	"%%IEEE Trans. on ASSP% #28, 441\\--454.")
MAN_END

MAN_BEGIN (L"Theil (1950)", L"djmw", 20121118)
NORMAL (L"H. Theil (1950): \"A rank-invariant method of linear and polynomial regression analysis\", "
	"%%Proceedings of Koninklijke Nederlandse Akademie van Wetenschappen% ##A.53#: 1397\\--1412.")
MAN_END

MAN_BEGIN (L"Wakita (1977)", L"djmw", 19980114)
NORMAL (L"H. Wakita (1977): \"Normalization of vowels by vocal-tract "
	"length and its application to vowel identification.\" %%IEEE Trans. on ASSP% "
	"#25: 183\\--192.")
MAN_END

}

/*
 BUGS:
 19980217 djmw LPC_and_Sound_filter ilast was not always defined.
 19980322 djmw Sound_into_LPC_Frame_auto did not return (nCoefficients == 0) when (r[1] == 0).
*/

/* End of file manual_LPC.c */
