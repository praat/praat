/* manual_LPC.c
 *
 * Copyright (C) 1994-2003 David Weenink
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
 djmw 20040407 Latest modification
*/

#include "ManPagesM.h"
#include "MFCC.h"

void manual_LPC_init (ManPages me);
void manual_LPC_init (ManPages me)
{

MAN_BEGIN ("CC: Paint...", "djmw", 20040407)
INTRO ("A command to paint the cepstral coefficients in shades of grey.")
ENTRY ("ss")
TAG ("%%From coefficient, To coefficient")
DEFINITION ("the range of coefficients that will be represented.")
MAN_END

MAN_BEGIN ("CC: To DTW...", "djmw", 19960918)
INTRO ("You can choose this command after selecting 2 objects with cepstral "
	"coefficients (two @MFCC's or @LFCC's). "
	"With this command you perform dynamic time warping. ")
ENTRY ("Algorithm")
NORMAL ("First we calculate distances between cepstral coefficients: ")
LIST_ITEM ("The distance between frame %i (from me) and %j (from thee) is:")
LIST_ITEM ("    %wc \\.c %d1 + %wle \\.c %d2 + %wr \\.c %d3,")
LIST_ITEM ("    where %wc, %wle & %wr are user-supplied weights and")
LIST_ITEM ("      %d1 = \\su (%k=1..%nCoefficients; (%c__%ik_ - %c__%jk_)^2)")
LIST_ITEM ("      %d2 = (c__%i0_ - c__%j0_)^2")
LIST_ITEM ("      %d3 = \\su (%k=1..%nCoefficients; (%r__%ik_ - %r__%jk_)^2), with ")
LIST_ITEM ("      %r__%ik_ the regression coefficient of the cepstral coefficients "
	"from the frames within a time span of %dtr seconds. "
	"c__%ij_ is %j-th cepstral coefficient in frame %i. ")
NORMAL ("Next we find the optimum path through the distance matrix with a "
	"Viterbi-algorithm.")
MAN_END

MAN_BEGIN ("CC: To Matrix", "djmw", 20011123)
INTRO ("Copies the cepstral coefficients of the selected @CC "
	"object to a newly created @Matrix object.")
ENTRY ("Behaviour")
FORMULA ("%z__%ji_ = %c__%ij_, with 1 \\<_ %i \\<_ %nx and "
	"1 \\<_ j \\<_ %numberOfCoefficients__%i_,")
NORMAL ("where %z__%ji_ is the matrix element in row %j and column %i and "
	"%c__%ij_ is the %j-th cepstral coefficient in frame %i.")
MAN_END

MAN_BEGIN ("Cepstrum", "djmw", 20010219)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
ENTRY ("Description")
NORMAL ("An object of type Cepstrum represents the complex cepstrum.")
MAN_END

MAN_BEGIN ("LFCC", "djmw", 20040421)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type LFCC represents cepstral "
	"coefficients on a linear frequency scale as a function of time. "
	"The coefficients are represented in frames with constant sampling "
	"period.")
ENTRY ("#LFCC commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@LPC: To LFCC...")
MAN_END

MAN_BEGIN ("LFCC: To LPC...", "djmw", 20040407)
INTRO ("You can choose this command after selecting 1 or more @LFCC's.")
ENTRY ("Settings")
TAG ("%%Number of coefficients%")
DEFINITION ("the desired number of linear predictive coefficients.")
ENTRY ("Behaviour")
NORMAL ("The transformation from cepstral coefficients to %a-coefficients "
	"as described in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN ("LPC", "djmw", 19990610)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type LPC represents filter coefficients as a function of time. "
	"The coefficients are represented in frames with constant sampling period.")
ENTRY ("LPC commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Sound: To LPC (autocorrelation)...@")
LIST_ITEM ("\\bu @@Sound: To LPC (covariance)...@")
LIST_ITEM ("\\bu @@Sound: To LPC (burg)...@")
LIST_ITEM ("\\bu @@Sound: To LPC (marple)...@")
ENTRY ("Conversion")
LIST_ITEM ("\\bu @@LPC: To LFCC...|To LFCC...@")
LIST_ITEM ("\\bu @@LPC: To Spectrogram...|To Spectrogram...@")
LIST_ITEM ("\\bu @@LPC: To Spectrum (slice)...|To Spectrum (slice)...@")
LIST_ITEM ("\\bu @@LPC: To Polynomial (slice)...|To Polynomial (slice)...@")
MAN_END

MAN_BEGIN ("LPC: Draw gain...", "djmw", 20040407)
INTRO ("You can choose this command after selecting 1 or more @LPC objects.")
ENTRY ("Settings")
TAG ("%%From time (seconds), To time (seconds)")
DEFINITION ("the time domain along the %x-axis.")
TAG ("%%Minimum gain, Maximum gain")
DEFINITION ("the range for the %y-axis.")
TAG ("%Garnish")
DEFINITION ("determines whether to draw a bounding box and axis labels.")
ENTRY ("Behaviour")
NORMAL ("Gain will be drawn as a function of time (gain also equals the prediction error "
	"energy). ")
MAN_END

MAN_BEGIN ("LPC: Draw poles...", "djmw", 20040407)
INTRO ("You can choose this command after selecting 1 or more @LPC objects.")
ENTRY ("Settings")
TAG ("%Time")
DEFINITION ("the time of the nearest frame.")
ENTRY ("Behaviour")
NORMAL ( "The @@Roots|roots@ of the @@LPC: To Polynomial (slice)...|linear prediction "
	"polynomial@, constructed from the coefficients "
	"of the analysis frame, will be drawn in the complex plane.")
MAN_END

MAN_BEGIN ("LPC: To LFCC...", "djmw", 20040407)
INTRO ("You can choose this command after selecting 1 or more @LPC objects.")
ENTRY ("Behaviour")
NORMAL ("The transformation from a-coefficients to cepstral coefficients "
	"as described in @@Markel & Gray (1976)@.")
ENTRY ("Settings")
TAG ("%%Number of coefficients%")
DEFINITION ("the desired number of cepstral coefficients.")
MAN_END

MAN_BEGIN ("LPC: To Matrix", "djmw", 20011123)
INTRO ("Copies the linear prediction coefficients of the selected @LPC "
	"object to a newly created @Matrix object.")
ENTRY ("Behaviour")
FORMULA ("%z__%ji_ = %a__%ij_, with 1 \\<_ %i \\<_ %nx and "
	"1 \\<_ j \\<_ %nCoefficients__%i_,")
NORMAL ("where %z__%ji_ is the matrix element in row %j and column %i and "
	"%a__%ij_ is the %j-th linear prediction coefficient in frame %i.")
MAN_END

MAN_BEGIN ("LPC: To Formant", "djmw", 19970123)
INTRO ("You can choose this command after selecting 1 or more @LPC objects.")
ENTRY ("Behaviour")
NORMAL ("For each LPC_Frame, the zeros of the linear prediction polynomial are extracted. "
	"Zeros that are outside the unit circle are reflected into it. "
	"Next, formant frequencies and bandwidths are calculated from all the roots that have the "
	"imaginary part positive, i.e., that lie in the upper half of the unit circle. "
	"Formant frequencies smaller than 50 Hz or larger than (%Nyquist_frequency - 50) are discarded. "
	"The remaining frequencies and bandwidths are sorted "
	"and copied to the Formant_Frame. Finally, the %gain field of the LPC is copied to the %intensity "
	"field of the Formant_Frame.")
ENTRY ("Algorithm")
NORMAL ("The root finder is Laguerre's method followed by root polishing, see @@Press "
	"et al. (1992)@.")
ENTRY ("Warning")
LIST_ITEM ("\\bu The formant values can be very inaccurate if you did not resample the Sound "
	"before the LPC-analysis (consult the @@Source-filter synthesis@ tutorial).") 
LIST_ITEM ("\\bu The results of the root finder may not always be accurate when more than 30 "
	"roots have to be found.")
MAN_END

MAN_BEGIN ("LPC: To Polynomial (slice)...", "djmw", 20040407)
INTRO ("A command that creates a Polynomial object from each selected @LPC object.")
ENTRY ("Settings")
TAG ("%Time (s)")
DEFINITION ("defines the LPC frame whose coefficents will be selected.")
ENTRY ("Behaviour")
NORMAL ("The linear prediction coefficients %a__1..%n_ of the selected LPC "
	"frame will be copied to polynomial coefficients %c__1..%n+1_ as follows:")
FORMULA ("%c__%i_ = %a__%n\\--%i+1_, ")
FORMULA ("%c__%n+1_ = 1")
MAN_END

MAN_BEGIN ("LPC: To Spectrum (slice)...", "djmw", 20040407)
INTRO ("You can choose this command after selecting 1 or more @LPC objects.")
ENTRY ("Settings")
TAG ("%%Time (seconds)")
DEFINITION ("the time at which the Spectrum must be calculated.")
TAG ("%%Minimum frequency resolution (Hz)")
DEFINITION ("successive frequencies in the @Spectrum "
	"will be maximally this distance apart.")
TAG ("%%Bandwidth reduction (Hz)")
DEFINITION ("formants with small bandwidths show up very well as peaks in the spectrum because the poles "
	"lie close to the contour along which the spectrum is computed (the unit circle in the z-plane). "
	"Peak enhancement can be realized by computing the spectrum in the z-plane along a contour of radius "
	"%r = exp (\\-- %\\pi \\.c %BandwidthReduction / %samplingFrequency). "
	"This technique is also called off-axis spectrum computation.")
TAG ("%%De-emphasis frequency (Hz)")
DEFINITION ("Performs de-emphasis when frequency is in the interval (0, @@Nyquist frequency@)")
ENTRY ("Algorithm")
NORMAL ("The Spectrum at time %t will be calculated from the %nearest LPC_Frame according to:")
FORMULA ("Spectrum (%f) = \\Vr(%gain\\.c%T/%df) / (1 + \\su__%k=1..%numberOfCoefficients_ %a__%k_%z^^\\--%k^),")
NORMAL ("where %T is the sampling period and %z = exp (\\--2 %\\pi %i %f %T) and %df is the distance in Hz "
	"between two successive components in the Spectrum.")
LIST_ITEM ("1. Allocate a large enough buffer[1..%nfft] to perform an fft analysis.")
LIST_ITEM ("2. Make the first value of the buffer 1 and copy the prediction coefficients #a into "
	"the buffer. This results in buffer values: (1, %a__1_, ..., %a__%numberOfCoefficients_, 0, ..., 0).")
LIST_ITEM ("3. If %deEmphasisFrequency is in the interval (0, nyquistFrequency) then \"multiply\" "
	"the buffer with (1 - %b %z^^\\--1^), where %b = exp (\\-- %\\pi %deEmphasisFrequency %T). "
	"This results in buffer values: (1, %a__1_\\--%b, %a__2_\\--%b\\.c%a__1_, ..., "
	"%a__%numberOfCoefficients_\\--%b\\.c%a__%numberOfCoefficients\\--1_, "
	"\\--%b\\.c%a__%numberOfCoefficients_, 0, ..., 0). Note that the number of values in the buffer that differ from 0 "
	"has increased by one.")
LIST_ITEM ("4. If %bandwidthReduction > 0 then multiply corresponding values in the buffer by %g^^%i\\--1^ where "
	"%g = exp (2%\\pi %bandwidthReduction %T / %nfft), and %i is the position index in the buffer. "
	"%i runs from 1 to %numberOfCoefficients+1+%t, where %t equals 1 when de-emphasis was performed, "
	"else 0.")
LIST_ITEM ("5. Calculate the fft spectrum of the buffer with the coefficients. This results in complex "
	"amplitudes (%a__%j_,%b__%j_), %j=1..%nfft/2+1.")
LIST_ITEM ("6. Calculate the LPC Spectrum by taking the inverse of the fft spectrum, i.e., each complex "
	"amplitude becomes (%a__%j_,%b__%j_)^^\\--1^ = (%a__%j_,\\--%b__%j_) / (%a__%j_^2 + %b__%j_^2)")
LIST_ITEM ("7. Multiply all values with the scale factor \\Vr(%gain\\.c%T/%df).")
MAN_END

MAN_BEGIN ("LPC: To Spectrogram...", "djmw", 20040407)
INTRO ("You can choose this command after selecting 1 or more @LPC objects.")
ENTRY ("Settings")
TAG ("%%Minimum frequency resolution (Hz)")
DEFINITION ("successive frequencies in the Spectrum will be maximally this distance apart")
TAG ("%%Bandwidth reduction (Hz)")
DEFINITION ("formants with small bandwidths show up very well as darker regions in the spectrogram "
	"because the poles lie close to the contour along which a spectrum is computed (the unit circle "
	"in the z-plane). "
	"Peak enhancement can be realized by computing a spectrum in the z-plane along a contour of radius "
	"%r = exp (\\-- %\\pi \\.c %BandwidthReduction / %samplingFrequency).")
TAG ("%%De-emphasis frequency (Hz)")
DEFINITION ("Performs de-emphasis when value is in the interval (0, @@Nyquist frequency@)")
ENTRY ("Algorithm")
NORMAL ("For each LPC_Frame the corresponding Spectrum will be calculated according to the algorithm "
	"explained in @@LPC: To Spectrum (slice)...@. "
	"For each frequency the power, i.e., the square of the complex values, will be stored in the "
	"corresponding area in the Spectrogram.")
MAN_END

MAN_BEGIN ("LPC: To VocalTract (slice)...", "djmw", 20040407)
INTRO ("You can choose this command after selecting 1 or more @LPC objects.")
ENTRY ("Settings")
TAG ("%Time")
DEFINITION ("the time of the nearest frame.")
TAG ("%Length")
DEFINITION ("the length of the vocal tract.")
TAG ("%%Length according to Wakita")
DEFINITION ("the length of the vocal tract is calculated according "
	"to the algorithm as described in @@Wakita (1977)@.")
ENTRY ("Behaviour")
NORMAL (" A new @VocalTract area function is calculated from the prediction coefficients in the frame.")
MAN_END

MAN_BEGIN ("LPC & Sound: Filter...", "djmw", 20040407)
INTRO ("A command that creates a new Sound object from one @Sound and one @LPC "
	"object which have been selected together.")
ENTRY ("Settings")
TAG ("%%Use LPC gain%")
DEFINITION ("Determines whether the gain from the LPC is used in the synthesis.")
ENTRY ("Behaviour")
NORMAL ("Filters the selected Sound by the selected LPC-filter.")
NORMAL ("When the LPC-gain is used the samples in the new Sound will be "
	"multiplied with the square root of the corresponding LPC-gain value.")
NORMAL ("In #Z-domain notation: #O(%z) = #H(%z) \\.c #E(%z), where "
	"#E(%z) is the selected filter input Sound, #H(%z) the selected LPC filter, "
	"and, #O(%z) the filter output (the new Sound that will appear in the List of objects).")
MAN_END

MAN_BEGIN ("LPC & Sound: Filter (inverse)", "djmw", 19970126)
INTRO ("A command that creates a new Sound object from one @Sound and one @LPC "
	"object which have been selected together.")
ENTRY ("Behaviour")
NORMAL ("Given a filter (the selected LPC) and its output (the selected Sound), "
	"its input is reconstructed (the new Sound that will appear in the List of objects).")
NORMAL ("In Z-domain notation: #E(%z) = #O(%z) / #H(%z), where "
	"#O(%z) is the filter output Sound, #H(%z) the LPC filter, and, #E(%z) the filter "
	"input Sound. (Selecting this newly generated Sound and the LPC, choosing the option "
	"`Filter...' generates a Sound that is identical to the Sound that originated "
	"the LPC.)")
MAN_END

MAN_BEGIN ("MFCC", "djmw", 20010411)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type MFCC represents mel frequency cepstral coefficients "
	"as a function of time. The coefficients are represented in frames "
	"with constant sampling period.")
ENTRY ("MFCC commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Sound: To MFCC...@")
LIST_ITEM ("\\bu @@MelFilter: To MFCC...@")
MAN_END

MAN_BEGIN ("MFCC: To MelFilter...", "djmw", 20040407)
INTRO ("A command to reconstruct @MelFilter objects  from the selected @MFCC "
	"objects .")
ENTRY ("Settings")
TAG ("%%From coefficient, To coefficient")
DEFINITION ("the range of coefficients that will be used in the reconstruction.")
ENTRY ("Details")
NORMAL ("The output of the triangular filters in a mel filter bank will be "
	"synthezised by applying the inverse cosine transform:")
FORMULA ("%P__%j_ = 2/N (%c__0_/2 + \\Si__%j=%from_^^%to^ %c__%i_ cos "
	"(%j\\pi/%N (%i-0.5)))),")
NORMAL ("where %N represents the number of filters and %c__%i_ the %i-th "
	"cepstral coefficient.")
MAN_END

MAN_BEGIN ("Sound: LPC analysis", "djmw", 19970126)
INTRO ("You can perform this analysis by selecting one or more @Sound objects and "
	"choosing the appropriate command to generate an @LPC.")
NORMAL ("The acronym LPC stands for Linear Predictive Coding.")
NORMAL ("In the LPC analysis one tries to predict %x__%n_ on the basis of the %p previous samples,")
FORMULA ("%x\\'p__%n_ = \\su %a__%k_ %x__%%n-k%_")
NORMAL ("then {%a__1_, %a__2_, ..., %a__%p_} can be chosen to minimize the prediction power %%Q__p_% where")
FORMULA ("%%Q__p_% = E[ |%x__%n_ - %x\\'p__%n_|^2].")
NORMAL ("Several different algorithms exist for minimizing %%Q__p_%:")
LIST_ITEM ("\\bu @@Sound: To LPC (autocorrelation)...|To LPC (autocorrelation)...@")
LIST_ITEM ("\\bu @@Sound: To LPC (covariance)...|To LPC (covariance)...@")
LIST_ITEM ("\\bu @@Sound: To LPC (burg)...|To LPC (burg)...@")
LIST_ITEM ("\\bu @@Sound: To LPC (marple)...|To LPC (marple)...@")
MAN_END

#define Sound_to_LPC_COMMON_HELP(method) \
INTRO ("With this command you create a new @LPC from every selected @Sound, " \
	"using " #method " method.") \
ENTRY ("Warning") \
NORMAL ("You are advised not to use this command for formant analysis. " \
	"For formant analysis, instead use @@Sound: To Formant (burg)...@, " \
	"which also works via LPC (linear predictive coding). This is because " \
	"##Sound: To Formant (burg)...# lets you specify a maximum frequency, " \
	"whereas the ##To LPC# commands automatically use the @@Nyquist frequency@ " \
	"as their maximum frequency. If you do use one of the ##To LPC# commands " \
	"for formant analysis, you may therefore want to downsample the sound first. " \
	"For instance, if you want five formants below 5500 Hz but your Sound has a sampling frequency " \
	"of 44100 Hz, you have to downsample the sound to 11000 Hz with the @@Sound: Resample...@ command. " \
	"After that, you can use the ##To LPC# commands, with a prediction order of 10 or 11.") \
ENTRY ("Settings") \
TAG ("%%Prediction order%") \
DEFINITION ("the number of linear prediction coefficients, also called the %%number of poles%. " \
	"Choose this number at least twice as large as the number of spectral peaks that you want " \
	"to detect.") \
TAG ("%%Analysis window duration% (s)") \
DEFINITION ("the effective duration of each analysis frame, in seconds.") \
TAG ("%%Time step% (s)") \
DEFINITION ("the time step between two consecutive analysis frames.") \
TAG ("%%Pre-emphasis frequency% (Hz)") \
DEFINITION ("a +6dB / octave filtering will be applied above this frequency. " \
	"A pre-emphasis frequency of 48.47 Hz for a signal with a sampling frequency of 10 kHz " \
	"approximately corresponds to a value of %a = 0.97 for the filter %y__%n_ = %x__%n_ - %a \\.c %x__%n-1_. " \
	"The relation between %a and the pre-emphasis frequency is: " \
	"%a = exp (\\--2\\.c%\\pi\\.c%preemphasisFrequency/%samplingFrequency). " \
	"If you do not want pre-emphasis, choose a frequency greater than the @@Nyquist frequency@.")

MAN_BEGIN ("Sound: To LPC (autocorrelation)...", "David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("the autocorrelation")
ENTRY ("Algorithm")
NORMAL ("The autocorrelation algorithm is decribed in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN ("Sound: To LPC (covariance)...", "David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("the covariance")
ENTRY ("Algorithm")
NORMAL ("The covariance algorithm is decribed in @@Markel & Gray (1976)@.")
MAN_END

MAN_BEGIN ("Sound: To LPC (marple)...", "djmw", 19970126)
Sound_to_LPC_COMMON_HELP ("Marple's")
TAG ("%%Tolerance 1")
DEFINITION ("stop the iteration when %E(%m) / %E(0) < %%Tolerance 1%, where %E(%m) is the "
	"prediction error for order %m.")
TAG ("%%Tolerance 2")
DEFINITION ("stop the iteration when (%E(%m) - %E(%m-1)) / %E(%m-1) < %%Tolerance 2.")
ENTRY ("Algorithm")
NORMAL ("The algorithm is described in @@Marple (1980)@.")
MAN_END

MAN_BEGIN ("Sound: To LPC (burg)...", "David Weenink & Paul Boersma", 20040407)
Sound_to_LPC_COMMON_HELP ("Burg's")
ENTRY ("Algorithm")
NORMAL ("Burg's algorithm is described in @@Anderson (1978)@")
MAN_END

MAN_BEGIN ("Sound: To MFCC...", "djmw", 20010410)
INTRO ("A command that creates a @MFCC object from every selected @Sound "
	"object.")
NORMAL ("The analysis proceeds in two steps:")
LIST_ITEM ("1.  We perform a filter bank analysis on a mel frequency scale "
	"(see @@Sound: To MelFilter...@ for details).")
LIST_ITEM ("2.  We convert the filter values to mel frequency cepstral "
	"coefficients (see @@MelFilter: To MFCC...@ for details).")
MAN_END

MAN_BEGIN ("Anderson (1978)","djmw", 20030701)
NORMAL ("N. Anderson (1978), \"On the calculation of filter coefficients for "
	"maximum entropy spectral analysis\", in Childers, %%Modern Spectrum Analysis%, "
	"IEEE Press, 252-255.")
MAN_END

MAN_BEGIN ("Markel & Gray (1976)", "djmw", 19980114)
NORMAL ("J.D. Markel & A.H. Gray, Jr. (1976), %%Linear Prediction of Speech%, "
	"Springer Verlag, Berlin.")
MAN_END

MAN_BEGIN ("Marple (1980)", "djmw", 19980114)
NORMAL ("L. Marple (1980), \" A new autoregressive spectrum analysis algorithm\", "
	"%%IEEE Trans. on ASSP% #28, 441-454.")
MAN_END

MAN_BEGIN ("Wakita (1977)", "djmw", 19980114)
NORMAL ("H. Wakita (1977), \"Normalization of vowels by vocal-tract "
	"length and its application to vowel identification\", %%IEEE Trans. on ASSP% "
	"#25, 183-192.")
MAN_END

}

/*
 BUGS:
 19980217 djmw LPC_and_Sound_filter ilast was not always defined.
 19980322 djmw Sound_into_LPC_Frame_auto did not return (nCoefficients == 0) when (r[1] == 0).
*/

/* End of file manual_LPC.c */
