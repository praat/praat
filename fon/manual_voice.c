/* manual_Voice.c
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

void manual_voice_init (ManPages me);
void manual_voice_init (ManPages me) {

MAN_BEGIN ("Voice", "ppgb", 20040616)
INTRO ("This tutorial describes how you can do voice analysis with P\\s{RAAT}. "
	"To understand this tutorial, you have to be familiar with the @Intro, "
	"which describes the more general features of the @SoundEditor window.")
NORMAL ("Most of P\\s{RAAT}'s voice analysis methods start from the glottal pulses that are visible in the @SoundEditor window "
	"as blue vertical lines through the waveform. If you do not see these lines, "
	"choose @@Show pulses@ from the #Pulses menu. If your sound is long, you may have to zoom in "
	"in order to see the separate pulses. You may notice that for some sounds, the time location of the pulses "
	"can vary when you zoom or scroll. This is because only the visible part of the sound is used for the analysis. "
	"The measurement results will also vary slightly when you zoom or scroll.")
NORMAL ("The Pulse menu contains the command @@Voice report@, which will show in the Info window "
	"the results of many voice measurements for the visible part of the selection "
	"(or for the visible part of the whole sound, if there is a cursor instead of a selection or if the selection is not visible).")
NORMAL ("In general, you will want to be careful about the pitch range. The standard range is 75\\--600 Hertz, "
	"but take a range of e.g. 50\\--200 Hertz for pathological male voices if that is the typical range. "
	"The `advanced' pitch settings like %%silence threshold% and %%octave jump cost% can stay at their standard values.")
LIST_ITEM ("@@Voice 1. Voice breaks@")
LIST_ITEM ("@@Voice 2. Jitter@")
LIST_ITEM ("@@Voice 3. Shimmer@")
LIST_ITEM ("@@Voice 4. Additive noise@ (HNR, harmonicity)")
LIST_ITEM ("@@Voice 5. Comparison with other programs@")
MAN_END

MAN_BEGIN ("Voice 1. Voice breaks", "ppgb", 20030916)
INTRO ("Normal voices can easily maintain phonation for some time when saying [a]. "
	"Some pathological voices have trouble with it. This can be measured in P\\s{RAAT} "
	"in two ways.")
ENTRY ("Fraction of locally unvoiced pitch frames")
NORMAL ("This is the fraction of pitch frames that are analysed as unvoiced (MDVP calls it DUV). If the pitch floor is 75 Hz, "
	"your Sound editor window will contain pitch measurements that are 0.01 seconds apart, so that if you select one second, "
	"there will be 100 pitch frames. If 86 of these are locally voiced, the Fraction will be 14 percent.")
NORMAL ("The usual pitch analysis contains a %%path finder% that searches for a smooth path through the local "
	"pitch candidates. This path finder is temporarily switched off to determine the fraction of locally unvoiced frames. "
	"A frame is regarded as %locally unvoiced if it has a voicing strength below the %%voicing threshold% (whose standard value is 0.45), "
	"or a local peak below the %%silence threshold% (whose standard value is 0.03).")
NORMAL ("In the voice report, the fraction of unvoiced frames will be reported as follows:")
CODE ("Fraction of locally unvoiced frames: 14.000\\%  (14/100)")
NORMAL ("The numbers between parentheses are the number of unvoiced frames and the total number of frames, "
	"respectively (in MDVP, these are called NUV and SEG, respectively).")
NORMAL ("The normative value for the fraction of unvoiced frames is 0, i.e., normal healthy voices should "
	"have no trouble maintaining voicing during a sustained vowel. Every non-zero value can be considered "
	"a sign of pathology (like a common cold). "
	"Naturally, you will not select the leading and trailing silences when measuring this parameter.")
ENTRY ("Number of voice breaks")
NORMAL ("The number of distances between consecutive pulses that are longer than 1.25 divided by the pitch floor. "
	"Thus, if the pitch floor is 75 Hz, all inter-pulse intervals longer than 16.6667 milliseconds are regarded as voice breaks.")
ENTRY ("Degree of voice breaks")
NORMAL ("This is the total duration of the breaks between the voiced parts of the signal, "
	"divided by the total duration of the analysed part of the signal (MDVP calls it DVB). Since silences at the beginning "
	"and the end of the signal are not considered breaks, you will probably not want to select these silences "
	"when measuring this parameter.")
NORMAL ("In the voice report, the degree of voice breaks will be reported like this:")
CODE ("Degree of voice breaks: 29.529\\%  (1.163061 s / 3.938685 s)")
NORMAL ("The numbers between parentheses are the total duration of the voice breaks and the duration of the analysed part of the signal, "
	"respectively.")
MAN_END

MAN_BEGIN ("Voice 2. Jitter", "ppgb", 20030531)
NORMAL ("You can measure jitter in the Sound editor window, after choosing @@Show pulses@ from the #Pulses menu. "
	"You will see blue lines that can be thought of as representing the glottal closures. "
	"Use the Pulse menu to get the jitter in the selected part. You typically perform jitter measurements only on "
	"long sustained vowels. The voice report gives five kinds of jitter measurements.")
ENTRY ("Jitter (local)")
NORMAL ("This is the average absolute difference between consecutive periods, divided by the average period. "
	"MDVP calls this parameter %Jitt, and gives 1.040\\%  as a threshold for pathology.")
ENTRY ("Jitter (local, absolute)")
NORMAL ("This is the average absolute difference between consecutive periods. "
	"MDVP calls this parameter %Jita, and gives 83.200 \\mus as a threshold for pathology.")
ENTRY ("Jitter (rap)")
NORMAL ("This is the Relative Average Perturbation, "
	"the average absolute difference between a period and the average of it and its two neighbours, divided by the average period. "
	"MDVP gives 0.680\\%  as a threshold for pathology.")
ENTRY ("Jitter (ppq5)")
NORMAL ("This is the five-point Period Perturbation Quotient, "
	"the average absolute difference between a period and the average of it and its four closest neighbours, divided by the average period. "
	"MDVP calls this parameter %PPQ, and gives 0.840\\%  as a threshold for pathology.")
ENTRY ("Jitter (ddp)")
NORMAL ("This is the average absolute difference between consecutive differences between consecutive periods, divided by the average period. "
	"This is P\\s{RAAT}'s original ##Get jitter#. The value is three times RAP.")
MAN_END

MAN_BEGIN ("Voice 3. Shimmer", "ppgb", 20030521)
NORMAL ("You can measure shimmer in the Sound editor window, after choosing @@Show pulses@ from the #Pulses menu. "
	"You will see blue lines that can be thought of as representing the glottal closures. "
	"Use the Pulse menu to get the shimmer in the selected part. You typically perform shimmer measurements only on "
	"long sustained vowels. The voice report gives six kinds of shimmer measurements.")
ENTRY ("Shimmer (local)")
NORMAL ("This is the average absolute difference between the amplitudes of consecutive periods, divided by the average amplitude. "
	"MDVP calls this parameter %Shim, and gives 3.810\\%  as a threshold for pathology.")
ENTRY ("Shimmer (local, dB)")
NORMAL ("This is the average absolute base-10 logarithm of the difference between the amplitudes of consecutive periods, multiplied by 20. "
	"MDVP calls this parameter %ShdB, and gives 0.350 dB as a threshold for pathology.")
ENTRY ("Shimmer (apq3)")
NORMAL ("This is the three-point Amplitude Perturbation Quotient, "
	"the average absolute difference between the amplitude of a period and the average of the amplitudes of its neighbours, "
	"divided by the average amplitude.")
ENTRY ("Shimmer (apq5)")
NORMAL ("This is the five-point Amplitude Perturbation Quotient, "
	"the average absolute difference between the amplitude of a period and the average of the amplitudes of it and its four closest neighbours, "
	"divided by the average amplitude.")
ENTRY ("Shimmer (apq11)")
NORMAL ("This is the 11-point Amplitude Perturbation Quotient, "
	"the average absolute difference between the amplitude of a period and the average of the amplitudes of it and its ten closest neighbours, "
	"divided by the average amplitude. "
	"MDVP calls this parameter %APQ, and gives 3.070\\%  as a threshold for pathology.")
ENTRY ("Shimmer (ddp)")
NORMAL ("This is the average absolute difference between consecutive differences between the amplitudes of consecutive periods. "
	"This is P\\s{RAAT}'s original ##Get shimmer#. The value is three times APQ3.")
MAN_END

MAN_BEGIN ("Voice 4. Additive noise", "ppgb", 20021206)
NORMAL ("For a signal that can be assumed periodic (i.e., a sustained vowel), "
	"the signal-to-noise ratio equals the harmonics-to-noise ratio, which you get can get "
	"by selecting a Sound and choosing one of the \"To Harmonicity...\" commands from the Periodicity menu "
	"(for the algorithm, see @@Sound: To Harmonicity (ac)...@ or @@Sound: To Harmonicity (cc)...@). "
	"These are the world's most sensitive HNR measurements (up to 90 dB). "
	"For more information, see the @Harmonicity manual page.")
MAN_END

MAN_BEGIN ("Voice 5. Comparison with other programs", "ppgb", 20030916)
INTRO ("Voicing, jitter, and shimmer measurements made by P\\s{RAAT} cannot always be compared directly with those made by other "
	"programs such as MDVP. The causes are the voicing decision strategy and the accuracy of period and peak determination.")
ENTRY ("Voicing decision strategy")
NORMAL ("Different programs use very different methods for deciding whether an irregular part of the signal is voiced or not. "
	"A comparison of @@Boersma (1993)@ for P\\s{RAAT} and @@Deliyski (1993)@ for MDVP leads to the "
	"following considerations. Both P\\s{RAAT} and MDVP use an autocorrelation method for "
	"pitch analysis, but MDVP quantizes the amplitudes into the values -1, 0, and +1 before computing "
	"the autocorrelation, whereas P\\s{RAAT} uses the original amplitude. Also, P\\s{RAAT} corrects "
	"the autocorrelation function by dividing it by the autocorrelation function of the window, "
	"unlike any other program. Lastly, P\\s{RAAT} uses sinc interpolation to compute an accurate "
	"estimate of the height of the autocorrelation peaks, unlike any other program. "
	"All three of these differences (and there are more) influence the measurement of the height "
	"of the autocorrelation peak at 1/F0. This height is generally taken as a criterion for voicing: "
	"if it is more than the %%voicing threshold% (which you can change with @@Pitch settings...@, "
	"the frame is considered voiced, otherwise voiceless. "
	"In P\\s{RAAT}, the standard voicing threshold is 0.45, in MDVP it is 0.29, which suggests that MDVP "
	"tends to regard more frames as voiced than P\\s{RAAT}. But the difference between these two numbers "
	"may partly be explained by MDVP's failure to correct the autocorrelation function and by MDVP's "
	"failure to do an accurate sinc interpolation: both of these failures cause "
	"the measured height of the peak at 1/F0 (in MDVP) to be lower than the real height, as explained by @@Boersma (1993)@.")
ENTRY ("Peak determination")
NORMAL ("")
MAN_END

MAN_BEGIN ("Voice report", "ppgb", 20030316)
INTRO ("A command in the #Pulses menu that will write to the Info window an extensive report "
	"about many voice parameters. See the @Voice tutorial.")
MAN_END
}

/* End of file manual_Voice.c */
