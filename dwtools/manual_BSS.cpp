/* manual_BSS.cpp
 *
 * Copyright (C) 2010-2019 David Weenink
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

/*
	djmw 20101227 Initial version
*/

#include "ManPagesM.h"

void manual_BSS (ManPages me);
void manual_BSS (ManPages me)
{
MAN_BEGIN (U"CrossCorrelationTable", U"djmw", 20170908)
INTRO (U"One of the types of objects in Praat. A ##CrossCorrelationTable# represents the cross-correlations between "
	"a number of signals. Cell [%i,%j] of a CrossCorrelationTable contains the cross-correlation between the %i-th "
	"and the %j-th signal. For example, the CrossCorrelationTable of an %n-channel sound is a %n\\xx%n table where "
	"the number in cell [%i,%j] is the cross-correlation of channel %i with channel %j (for a particular lag time %\\ta).")
NORMAL (U"A CrossCorrelationTable has a square matrix whose cells contain the cross-correlations between "
	"the signals and a centroid vector with the average value of each signal.")
ENTRY (U"Remarks")
NORMAL (U"Sometimes in the statistical literature, the cross-correlation between signals is also called "
	"\"covariance\". However, the only thing a @@Covariance@ has in common with a CrossCorrelationTable is that "
	"both are symmetric matrices. The differences between a CrossCorrelationTable and a Covariance are:")
TAG (U"1. A Covariance matrix is always positive-definite; for a cross-correlation table this is only guaranteed if "
	"the lag time %\\ta = 0.")
TAG (U"2. The elements %%c__ij_% in a Covariance always satisfy |%%c__ij_%/\\Vr(%%c__ii_%\\.c%%c__jj_%)| \\<_ 1; this is "
	"generally not the case for cross-correlations.")
MAN_END

MAN_BEGIN (U"CrossCorrelationTableList", U"djmw", 20101227)
INTRO (U"One of the types of objects in Praat. A CrossCorrelationTableList represents a collection of @@CrossCorrelationTable@ objects.")
MAN_END

MAN_BEGIN (U"CrossCorrelationTableList: Create test set...", U"djmw", 20110212)
INTRO (U"Create a collection of @@CrossCorrelationTable@s that are all derived from different diagonal matrices by the same transformation matrix.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("CrossCorrelationTableList: Create test set", 4)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Matrix dimension", "5")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Number of matrices", "20")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("First is positive-definite",1)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Sigma", "0.02")
)
TAG (U"##Matrix dimension")
DEFINITION (U"determines the size of the square matrix with cross-correlations.")
TAG (U"##Number of matrices")
DEFINITION (U"determines the number of matrices that have to be generated.")
TAG (U"##First is positive-definite")
DEFINITION (U"guarantees that the first matrix of the series is positive definite.")
TAG (U"##Sigma")
DEFINITION (U"the standard deviation of the noise that is added to each transformation matrix element. A value "
	"of zero makes all the cross-correlation matrices jointly diagonalizable. A value greater than zero "
	"makes each transformation matrix a little different and the collection not jointly "
	"diagonalizable anymore.")
ENTRY (U"Algorithm")
NORMAL (U"All the CrossCorrelationTable matrices are generated as #V\\'p\\.c#D__%k_\\.c #V, where #D__%k_ is a diagonal matrix "
	"with entries randomly choosen from the [-1,1] interval. The matrix #V is a \"random\" orthogonal matrix "
	"obtained from the singular value decomposition of a matrix #M = #U\\.c#D\\.c#V\\'p, where the cells of the "
	"matrix #M are random Gaussian numbers with mean 0 and standard deviation 1.")
NORMAL (U"If the first matrix has to be positive definite, the numbers on the diagonal of #D__1_ are randomly "
	"chosen from the [0.1,1] interval.")
MAN_END

MAN_BEGIN (U"Create simple MixingMatrix...", U"djmw", 20170908)
INTRO (U"Create a new @@MixingMatrix@.")
ENTRY (U"Settings")
TAG (U"##Number of inputs")
DEFINITION (U"defines the number of input channels, i.e. the number of columns in the matrix.")
TAG (U"##Number of outputs")
DEFINITION (U"defines the number of output channels, i.e. the number of rows in the matrix.")
TAG (U"##Mixing coefficients")
DEFINITION (U"define the coefficients. You input them row-wise. There have to be %%numberOfOutPuts% \\xx %%numberOfInputs% values. ")
MAN_END

MAN_BEGIN (U"MixingMatrix", U"djmw", 20170907)
INTRO (U"One of the @@Types of objects|type of Objects@ in Praat. A ##MixingMatrix# shows a mapping of the channels of an input @Sound to the channels of an output Sound. A channel in the input sound is called an %%input% channel. Each output channel is a linear combination of input channels. ")
NORMAL (U"The mixing of input channels can be written as the matrix multiplication ##R=M\\.cS#. Here #S is the matrix that represents the input sound, with %numberOfInputs% rows and %%numberOfSamples% columns. Each row in #S corresponds to one input channel. #M is the %%numberOfOutputChannels%\\xx %%numberOfInputs% MixingMatrix and #R is the %%numberOfOutputChannels%\\xx %%numberOfSamples% matrix that is the result of the mixing.")
NORMAL (U"Row %i in the MixingMatrix #M therefore represents the weights %m__%ij_ of the different input channels %j in output channel %i, the number of rows of #M determines the number of output channels in the resulting #R. Column %j in #M represents the weight factors %m__%ij_ of input %j in the different output channels %i.")

ENTRY (U"Examples")
NORMAL (U"Given the following stereo Sound with a tone of 300 Hz in channel 1 and a tone of 600 Hz in channel two: ")
CODE (U"stereo = Create Sound from formula: \"s\", 2, 0, 1, 44100, \"sin(2*pi*row*300*x)\"")
TAG (U"Example 1")
CODE (U"mm1 = Create simple MixingMatrix: \"mm1\", 2, 1, \"1 0\"")
CODE (U"selectObject: mm1, stereo")
CODE (U"Mix")
DEFINITION (U"will produce a new %mono Sound object that shows a tone with a frequency of 300 Hz.")
DEFINITION (U"The example creates a Mixing matrix with one row and two columns. The resulting new Sound object will have only one channel which is the result of adding the two channels from the stereo sound with weights of 1.0 and 0.0, respectively.")  
TAG (U"Example 2")
CODE (U"mm2 = Create simple MixingMatrix: \"mm2\", 2, 1, \"0 1\"")
CODE (U"selectObject: mm2, stereo")
CODE (U"Mix")
DEFINITION (U"will produce a new mono Sound object that shows a tone with a frequency of 600 Hz.")
TAG (U"Example 3")
CODE (U"mm3 = Create simple MixingMatrix: \"mm3\", 2, 1, \"1 1\"")
CODE (U"selectObject: mm3, stereo")
CODE (U"Mix")
DEFINITION (U"will produce a new mono Sound object that shows a complex tone composed of frequencies 300 and 600 Hz. The amplitude of the output sound will be larger than 1")
TAG (U"Example 4")
CODE (U"mm4 = Create simple MixingMatrix: \"mm4\", 2, 2, \"1 0 1 0\"")
CODE (U"selectObject: mm4, stereo")
CODE (U"Mix")
DEFINITION (U"will produce a new stereo Sound object that shows a tone of frequency 300 Hz in both channels.")
TAG (U"Example 5")
CODE (U"mm5 = Create simple MixingMatrix: \"mm5\", 2, 1, \"0.5 0.5\"")
CODE (U"selectObject: mm5, stereo")
CODE (U"Mix")
DEFINITION (U"will produce a new mono Sound object that shows a complex tone composed of frequencies 300 and 600 Hz. The amplitudes of the output sound are now half the amplitude of the output sound of example 3.")
TAG (U"Example 6")
CODE (U"mono = Create Sound from formula: \"s\", 1, 0, 1, 44100, \"sin(2*pi*300*x)\"")
CODE (U"mm6 = Create simple MixingMatrix: \"mm6\", 1, 2, \"1 1\"")
CODE (U"selectObject: mm6, mono")
CODE (U"Mix")
DEFINITION (U"will produce from the mono input sound a stereo output sound that shows a tone of frequency 300 Hz in both channels.")
TAG (U"Example 7")
CODE (U"mm7 = Create simple MixingMatrix: \"mm7\", 2, 2, \"0 1 1 0\"")
CODE (U"selectObject: mm7, stereo")
CODE (U"Mix")
DEFINITION (U"will interchange the channels.")
TAG (U"Example 8")

MAN_END

MAN_BEGIN (U"MixingMatrix: Multiply input channel...", U"djmw", 20170908)
INTRO (U"Multiply an input channel of the selected @@MixingMatrix@ by a value.")
ENTRY (U"Examples")
TAG (U"Increase the contribution of input channel 1 in each output channel by a factor of 2")
CODE (U"Multiply input channel: 1, 2")
TAG (U"Remove the contribution of channel 1 in each output channel")
CODE (U"Multiply input channel: 1, 0")
MAN_END

MAN_BEGIN (U"Sound: To CrossCorrelationTable...", U"djmw", 20110212)
INTRO (U"A command that creates a @@CrossCorrelationTable@ form every selected @@Sound@ object.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (2), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: To CrossCorrelationTable", 2)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range", "0.0", "10.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Lag time", "0.0")
)
TAG (U"##Time range (s)#,")
DEFINITION (U"determines the time range over which the table is calculated.")
TAG (U"##Lag time (s)#,")
DEFINITION (U"determines the lag time.")
ENTRY (U"Algorithm")
NORMAL (U"The cross-correlation between channel %i and channel %j for lag time \\ta is defined as the "
	"discretized #integral")
FORMULA (U"cross-corr (%c__%i_, %c__%j_) [%\\ta] \\=3 \\su__%t_ %c__%i_[%t] %c__%j_[%t+%\\ta] %%\\Det%,")
NORMAL (U"where %t and %t+%\\ta are discrete times and %%\\Det% is the @@sampling period@. ")
MAN_END

MAN_BEGIN (U"Sound: To Covariance (channels)...", U"djmw", 20120303)
INTRO (U"Detemines the @@Covariance|covariances@ between the channels of a selected @Sound.")
NORMAL (U"The covariance of a sound is determined by calculating the @@CrossCorrelationTable@ of a multichannel sound for a lag time equal to zero.")
MAN_END

MAN_BEGIN (U"Sound: To Sound (blind source separation)...", U"djmw", 20190811)
INTRO (U"Analyze the selected multi-channel sound into its independent components by an iterative method.")
NORMAL (U"The @@blind source separation@ method to find the independent components tries to simultaneously diagonalize a number of "
	"@@CrossCorrelationTable@s that are calculated from the multi-channel sound at different lag times.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: To Sound (blind source separation)", 6)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range (s)", "0.0", "10.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Number of cross-correlations", "20")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Lag times", "0.002")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum number of iterations", "100")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Tolerance", "0.001")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU("Diagonalization method", "ffdiag")
)
TAG (U"##Time range (s)")
DEFINITION (U"defines the time range over which the ##CrossCorrelationTable#s of the sound will be calculated.")
TAG (U"##Number of cross-correlations")
DEFINITION (U"defines the number of ##CrossCorrelationTable#s to be calculated.")
TAG (U"##Lag times")
DEFINITION (U"defines the lag time %\\ta__0_ for the ##CrossCorrelationTable#s. These tables "
	"are calculated at lag times %\\ta__k_=(%k - 1)%\\ta__0_, where %k runs from 1 to %%numberOfCrosscorrelations%.")
TAG (U"##Maximum number of iterations")
DEFINITION (U"defines a stopping criterion for the iteration. The iteration will stops when this number is reached.")
TAG (U"##Tolerance")
DEFINITION (U"defines another stopping criterion that depends on the method used.")
TAG (U"##Diagonalization method")
DEFINITION (U"defines the method to determine the independent components.")
ENTRY (U"Algorithm")
NORMAL (U"This method tries to decompose the sound according to the %%instantaneous% mixing model")
FORMULA (U"#Y=#A\\.c#X.")
NORMAL (U"In this model #Y is a matrix with the selected multi-channel sound, #A is a so-called "
	"%%mixing matrix% and #X is a matrix with the independent components. "
	"Essentially the model says that each channel in the multi-channel sound is a linear combination of the "
	"independent sound components in #X. "
	"If we would know the mixing matrix #A we could easily solve the model above for #X by standard means. "
	"However, if we don't know #A and we don't know #X, the decomposition of #Y is underdetermined.  This means there "
	"are an infinite number of possible combinations of #A and #X that result in the same #Y. ")
NORMAL (U"One approach to solve the equation above is to make assumptions about the statistical properties "
	"of the components in the matrix #X: it turns out that a sufficient assumption is to assume that the "
	"components in #X at each time instant are %%statistically independent%. This is not an unrealistic "
	"assumption in many cases, although in practice it need not be exactly the case. Another assumption is "
	"that the mixing matrix is constant, which means that the mixing conditions did not change during the recoding of the sound." )
NORMAL (U"The theory says that statistically independent signals are not correlated (although the reverse "
	"is not always true: signals that are not correlated don't have to be statistically independent). "
	"The methods implemented here all follow this lead as follows. If we calculate the @@CrossCorrelationTable@ "
	"for the left and the right side signals of the equation above, then, "
	"for the multi-channel sound #Y this will result in a cross-correlation matrix #C. For the right side we "
	"obtain #A\\.c#D\\.c#A\\'p, where #D is a diagonal matrix because all the cross-correlations between "
	"different independent components are zero by definition. This results in the following identity: ")
FORMULA (U"#C(\\ta)=#A\\.c#D(\\ta)\\.c#A\\'p, for all values of the lag time \\ta.")
NORMAL (U"This equation says that, given the model, the cross-correlation matrix can be diagonalized for "
	"all values of the lag time %%by the same transformation matrix% #A.")
NORMAL (U"If we calculate the cross-correlation matrices for a number of different lag times, say 20, we "
	"then have to obtain the matrix #A that diagonalizes them all. Unfortunately there is no closed form solution "
	"that diagonalizes more than two matrices at the same time and we have to resort to iterative "
	"algorithms for joint diagonalization. ")
NORMAL (U"Two of these algorithms are the ##qdiag# method as described in @@Vollgraf & Obermayer (2006)@ "
	"and the ##ffdiag# method as described in @@Ziehe et al. (2004)@. ")
NORMAL (U"Unfortunately the convergence criteria of these two algorithms cannot easily be compared as "
	"the criterion for the ##ffdiag# algorithm is the relative change of the square root of the sum of the "
	"squared off-diagonal "
	"elements of the transformed cross-correlation matrices and the criterion for ##qdiag# is the largest "
	"change in the eigenvectors norm during an iteration.")
ENTRY (U"Example")
NORMAL (U"We start by creating a speech synthesizer that need to create two sounds. We will mix the two sounds and finally our blind source separation software will try to undo our mixing by extracting the two original sounds as well as possible from the two mixtures.")
CODE (U"synth = Create SpeechSynthesizer: \"English (Great Britain)\", \"Female1\"")
CODE (U"s1 = To Sound: \"This is some text\", \"no\"")
NORMAL (U"The first speech sound was created from the text \"This is some text\" at a speed of 175 words per minute.")
CODE (U"selectObject: synth")
CODE (U"Speech output settings: 44100, 0.01, 1.2, 1.0, 145, \"IPA\"")
CODE (U"Estimate speech rate from speech: \"no\"")
CODE (U"s2 = To Sound.: \"Abracadabra, abra\", \"no\"")
NORMAL (U"The second sound \"Abracadabra, abra\" was synthesized at 145 words per minute with a somewhat larger pitch excursion (80) than the previous sound (50).")
CODE (U"plusObject: s1")
CODE (U"stereo = Combine to stereo")
NORMAL (U"We combine the two separate sounds into one stereo sound because our blind source separation works on multichannel sounds only.")
CODE (U"mm = Create simple MixingMatrix: \"mm\", 2, 2, \"1.0 2.0 2.0 1.0\"")
NORMAL (U"A two by two MixingMatrix is created.")
CODE (U"plusObject: stereo")
CODE (U"Mix")
NORMAL (U"The last command, Mix, creates a new two-channel sound where each channel is a linear mixture of the two "
    "channels in the stereo sound, i.e. channel 1 is the sum of s1 and s2 with mixture strengths of 1 and 2, respectively. "
    "The second channel is also the sum of s1 and s2 but now with mixture strengths 2 and 1, respectively.")
CODE (U"To Sound (blind source separation): 0.1, 1, 20, 0.0002, 100, 0.001, \"ffdiag\"")
NORMAL (U"The two channels in the new sound that results from this command contain a reasonable approximation of "
    "the two originating sounds.")
NORMAL (U"In the top panel the two speech sounds \"This is some text\" and \"abracadabra, abra\". "
    "The middle panel shows the two mixed sounds while the lower panel shows the two sounds after unmixing.")
SCRIPT (6, 6, U" "
	"syn = Create SpeechSynthesizer: \"English (Great Britain)\", \"Female1\"\n"
	"s1 = To Sound: \"This is some text\", \"no\"\n"
    "selectObject: syn\n"
	"Speech output settings: 44100, 0.01, 1.2, 1.0, 145, \"IPA\"\n"
	"Estimate speech rate from speech: \"no\"\n"
	"s2 = To Sound: \"abracadabra, abra\", \"no\"\n"
    "plusObject: s1\n"
	"stereo = Combine to stereo\n"
	"Select inner viewport: 1, 6, 0.1, 1.9\n"
	"Draw: 0, 0, 0, 0, \"no\", \"Curve\"\n"
	"Draw inner box\n"
	"mm = Create simple MixingMatrix: \"mm\", 2, 2, \"1.0 2.0 2.0 1.0\"\n"
    "plusObject: stereo\n"
	"mixed = Mix\n"
	"Select inner viewport: 1, 6, 2.1, 3.9\n"
	"Draw: 0, 0, 0, 0, \"no\", \"Curve\"\n"
	"Draw inner box\n"
	"unmixed = To Sound (bss): 0.1, 1, 20, 0.00021, 100, 0.001, \"ffdiag\"\n"
	"Select inner viewport: 1, 6, 4.1, 5.9\n"
	"Draw: 0, 0, 0, 0, \"no\", \"Curve\"\n"
	"Draw inner box\n"
	"removeObject: unmixed, syn, stereo, s1, s2, mixed, mm\n"
)
NORMAL (U"The first two panels will not change between different sessions of praat. The last panel, which shows "
    "the result of the blind source separation, i.e. unmixing, will not always be the same because of two things. In the first place the unmixing always starts with an initialisation with random values of the parameters that "
    "we have to determine for the blind source separation. Therefore the iteration sequence will never be the same and the final outcomes might differ. In the second place, as was explained in the @@blind source separation@ manual, the unmixing is only "
    "unique up to a scale factor and a permutation. Therefore the channels in the unmixed sound do not necessarily correspond to the corresponding channel in our \"original\" stereo sound.")
NORMAL (U"The complete script:")
CODE (U"syn = Create SpeechSynthesizer: \"English (Great Britain)\", \"Female1\"")
CODE (U"s1 = To Sound: \"This is some text\", \"no\"")
CODE (U"selectObject: syn")
CODE (U"Speech output settings: 44100, 0.01, 1.2, 1.0, 145, \"IPA\"")
CODE (U"Estimate speech rate from speech: \"no\"")
CODE (U"s2 = To Sound: \"abracadabra, abra\", \"no\"")
CODE (U"plusObject: s1")
CODE (U"stereo = Combine to stereo")
CODE (U"Select inner viewport: 1, 6, 0.1, 1.9")
CODE (U"Draw: 0, 0, 0, 0, \"no\", \"Curve\"")
CODE (U"Draw inner box")
CODE (U"mm = Create simple MixingMatrix: \"mm\", 2, 2, \"1.0 2.0 2.0 1.0\"")
CODE (U"plusObject: stereo")
CODE (U"mixed = Mix")
CODE (U"Select inner viewport: 1, 6, 2.1, 3.9")
CODE (U"Draw: 0, 0, 0, 0, \"no\", \"Curve\"")
CODE (U"Draw inner box")
CODE (U"unmixed = To Sound (bss): 0.1, 1, 20, 0.00021, 100, 0.001, \"ffdiag\"")
CODE (U"Select inner viewport: 1, 6, 4.1, 5.9")
CODE (U"Draw: 0, 0, 0, 0, \"no\", \"Curve\"")
CODE (U"Draw inner box")
CODE (U"removeObject: unmixed, syn, stereo, s1, s2, mixed, mm")
MAN_END

MAN_BEGIN (U"Sound: To Sound (whiten channels)...", U"djmw", 20120303)
INTRO (U"Transforms the channels of the selected @Sound linearly to make them white, i.e. the new channels will be uncorrelated and their variances equal unity.")
ENTRY (U"Settings")
TAG (U"##Variance fraction to keep#,")
DEFINITION (U"determines, indirectly, how many channels the final sound will have.")
ENTRY (U"Algorithm")
NORMAL (U"We start by determining the @@Sound: To Covariance (channels)...|covariance@ of the selected sound. "
   "Next a @@Principal component analysis|principal component analysis@ determines the eigenvalues and eigenvectors of the covariance matrix. The settings of the variance fraction to keep determines how many eigenvalues and eigenvectors we use for the whitening. This number, %p, will also be equal to the number of channels of the resulting whitened sound.")
NORMAL (U"In mathematical terms. For an %n-channel sound, if #E is the matrix with the eigenvectors and #D=diag (%d__1_, %d__2_,..., %d__n_) is the diagonal matrix with the "
    "eigenvalues of the covariance matrix, then the whitening matrix is #W = #E#D^^-1/2^#E\\'p, where #D^^-1/2^=diag (%d__1_^^-1/2^, ..., %d__p_^^-1/2^, 0, ..., 0). Only the %p most important eigenvalues have been retained, where %p was determined as the smallest integer for which (%d__1_+%d__2_+...%d__%p_)/(%d__1_+%d__2_+ ... + %d__%n_) >= %%varianceFractionToKeep%.")
NORMAL (U"The resulting sound samples of the whitened sound, %w__%ij_, are then calculated from the samples of the "
    "original sound, %s__%kj_, as %w__%ij_ = \\Si__%k_ W__%ik_ %s__%kj_, where 1 \\<_ %i \\<_%p, 1 \\<_ %j \\<_ numberOfSamples and 1 \\<_ %k \\<_ %n.")
MAN_END

MAN_BEGIN (U"blind source separation", U"djmw", 20120907)
INTRO (U"Blind source separation (BSS) is a technique for estimating individual source components from their mixtures "
	"at multiple sensors. It is called %blind because we don't use any other information besides the mixtures. ")
NORMAL (U"For example, imagine a room with a number of persons present and a number of microphones for recording. "
	"When one or more persons are speaking at the same time, each microphone registers a different %mixture of individual speaker's audio signals. It is the task of BSS to untangle these mixtures into their sources, i.e. the individual speaker's audio signals. "
	"In general, this is a difficult problem because of several complicating factors. ")
LIST_ITEM (U"\\bu Different locations of speakers and microphones in the room: the individual speaker's audio signals do not reach all microphones at the same time. ")
LIST_ITEM (U"\\bu Room acoustics: the signal that reaches a microphone is composed of the signal that %directly travels to the microphone and parts that come from room reverberations and echos. ")
LIST_ITEM (U"\\bu Varying distances to microphones: one or more speakers might be moving. This makes the mixing time dependent.")
NORMAL (U"If the number of sensors is %larger than the number of sources we speak of an %overdetermined problem. If the number of sensors and the number of sources are %equal we speak of a %determined problem. The more difficult problem is the %underdetermined one where the number of sensors is %less than the number of sources.")
ENTRY (U"Typology of mixtures")
NORMAL (U"In general two different types of mixtures are considered in the literature: %%instantaneous "
	"mixtures% and %%convolutive mixtures%. ")
TAG (U"%%Instantaneous mixtures%")
DEFINITION (U"where the mixing is instantaneous, corresponds to the model #Y=#A\\.c#X. In this model #Y is a matrix with the recorded microphone sounds, #A is a so-called "
	"%%mixing matrix% and #X is a matrix with the independent source signals. "
	"Essentially the model says that the signal that each microphone records is a (possibly different) linear combination of the %same source signals.  "
	"If we would know the mixing matrix #A we could easily solve the model above for #X by standard means. "
	"However, in general we don't know #A and #X and there are an infinite number of possible decompositions for #Y. The problem is however solvable by making some (mild) assumptions about #A and #X. ")
TAG (U"%%Convolutive mixtures%")
DEFINITION (U"are mixtures where the mixing is of convolutive nature, i.e. the model is ")
FORMULA (U"%%y__i_ (n)% = \\Si__%j_^^%d^\\Si__%\\ta_^^M__%ij_-1^ %%h__ij_(\\ta)x__j_(n-\\ta) + N__i_(n)%, for %i=1..m.")
DEFINITION (U"Here %%y__i_ (n) is the %n-th sample of the %i-th microphone signal, %m is the number of microphones, %%h__ij_(\\ta)% is the multi-input multi-output linear filter with the source-microphone impulse responses that characterize the propagation of the sound in the room and %%N__i_% is a noise source. This model is typically much harder to solve than the previous one because of the %%h__ij_(\\ta)% filter term that can have thousands of coefficients. For example, the typical @@reverberation time@ of a room is approximately 0.3 s which corresponds to 2400 samples, i.e. filter coefficients, for an 8 kHz sampled sound.")
ENTRY (U"Solving the blind source separation for instantaneous mixtures")
NORMAL (U"Various techniques exist for solving the blind source separation problem for %instantaneous mixtures. Very popular ones make make use of second order statistics (SOS) by trying to "
	"simultaneously diagonalize a large number of cross-correlation matrices. Other techniques like independent component analysis use higher order statistics (HOS) to find the independent components, i.e. the sources.")
NORMAL (U"Given the decomposition problem #Y=#A\\.c#X, we can see that the solution is determined "
	"only upto a permutation and a scaling of the components. This is called the %%indeterminancy "
	"problem% of BSS. This can be seen as follows: given a permutation matrix #P, i.e. a matrix which "
	"contains only zeros except for one 1 in every row and column, and a diagonal scaling matrix #D, any "
	"scaling and permutation of the independent components #X__%n_=(#D\\.c#P)\\.c#X can be compensated "
	"by the reversed scaling of the mixing matrix #A__%n_=#A\\.c(#D\\.c#P)^^-1^ because #A\\.c(#D\\.c#P)^^-1^\\.c(#D\\.c#P)\\.c#X = #A\\.c#X = #Y. ")
ENTRY (U"Solving the blind source separation for convolutive mixtures")
NORMAL (U"Solutions for %convolutive mixture problems are much harder to achieve. "
	"One normally starts by transforming the problem to the frequency domain where the "
	"convolution is turned into a multiplication. The problem then translates into a separate "
	"%%instantaneous% mixing problem for %%each% frequency in the frequency domain. It is here that "
	"the indeterminacy problem hits us because it is not clear beforehand how to combine the "
	"independent components of each frequency bin.")
MAN_END

MAN_BEGIN (U"reverberation time", U"djmw", 20110107)
NORMAL (U"Reverberation is the persistence of sound in a room after the sound source has silenced. ")
NORMAL (U"The %%reverberation time% is normally defined as the time required for the persistence of a direct sound "
    "to decay by 60 dB after the direct sound has silenced. Sometimes this dB level is indicated with a subscript "
    "and the reverberation time is given by the symbol %T__60_. "
	"The reverberation time depends mainly on a room's volume and area and on the absorption at the walls. Generally absorption is frequency dependent and therefore the reverberation time of a room varies with frequency. ")
MAN_END

MAN_BEGIN (U"Vollgraf & Obermayer (2006)", U"djmw", 20110105)
NORMAL (U"Roland Vollgraf & Klaus Obermayer (2006): \"Quadratic optimization for simultaneous matrix "
	"diagonalization.\" %%IEEE Transactions On Signal Processing% #54: 3270\\--3278.")
MAN_END

MAN_BEGIN (U"Ziehe et al. (2004)", U"djmw", 20110105)
NORMAL (U"Andreas Ziehe, Pavel Laskov, Guido Nolte & Klaus-Robert M\\u\"ller (2004): \"A fast algorithm for joint "
	"diagonalization with non-orthogonal transformations and its application to blind source separation\", "
	"%%Journal of Machine Learning Research% #5: 777\\--800.")
MAN_END

}

/* End of file manual_BSS.cpp */

