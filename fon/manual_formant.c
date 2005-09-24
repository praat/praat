/* manual_formant.c
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

void manual_formant_init (ManPages me);
void manual_formant_init (ManPages me) {

MAN_BEGIN ("Create FormantTier...", "ppgb", 20021204)
INTRO ("A command in the @@New menu@ to create an empty @FormantTier object.")
NORMAL ("The resulting object will have the specified name and time domain, but contain no formant points. "
	"To add some points to it, use @@FormantTier: Add point...@.")
NORMAL ("For an example, see @@Source-filter synthesis@.")
MAN_END

MAN_BEGIN ("Formant", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. "
	"A Formant object represents spectral structure as a function of time: a %%formant contour%. "
	"Unlike the time-stamped @FormantTier object, it is sampled into a number of %frames centred around equally spaced times, "
	"Each frame contains frequency and bandwidth information about several formants.")
ENTRY ("Inside a Formant object")
NORMAL ("With @Inspect, you will see the following attributes:")
TAG ("%xmin")
DEFINITION ("starting time, in seconds.")
TAG ("%xmax")
DEFINITION ("end time, in seconds.")
TAG ("%nx")
DEFINITION ("the number of frames (\\>_ 1).")
TAG ("%dx")
DEFINITION ("time step = frame length = frame duration, in seconds.")
TAG ("%x1")
DEFINITION ("the time associated with the first frame, in seconds. "
	"This will usually be in the range [%xmin, %xmax]. "
	"The time associated with the last frame (i.e., %x1 + (%nx \\-- 1) %dx)) "
	"will also usually be in that range.")
TAG ("%frame__%i_, %i = 1 ... %nx")
DEFINITION ("the frames (see below).")
ENTRY ("Attributes of a formant frame")
NORMAL ("Each %frame__%i_ contains the following attributes:")
TAG ("%intensity")
DEFINITION ("an indication of the maximum intensity (a squared sound amplitude) in this frame.")
TAG ("%nFormants")
DEFINITION ("the number of formants in this frame (usually between 2 and 6).")
TAG ("%formant__%j_, %j = 1 ... %nFormants")
DEFINITION ("the information about each formant (see below).")
ENTRY ("Attributes of each formant")
NORMAL ("Each %formant__%j_ contains the following attributes:")
TAG ("%frequency")
DEFINITION ("the formant's centre frequency (in Hz).")
TAG ("%bandwidth")
DEFINITION ("the formant's bandwidth (in Hz).")
ENTRY ("See also")
NORMAL ("##Linear Prediction#")
MAN_END

MAN_BEGIN ("Formant: Down to FormantTier", "ppgb", 19980101)
INTRO ("A command for converting each selected @Formant object into a @FormantTier object.")
NORMAL ("The resulting FormantTier contains a point for each original frame. The number of formants "
	"in the result is limited to 10. The intensity information is lost.")
MAN_END

MAN_BEGIN ("Formant: Speckle...", "ppgb", 20030916)
INTRO ("A command to draw the selected @Formant objects to the @@Picture window@.")
ENTRY ("Behaviour")
NORMAL ("Every formant value is drawn as a small circle, filled with the current colour.")
ENTRY ("Arguments")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the time domain of the drawing. If %%To time% is not greater than %%From time%, "
	"the entire formant contour is drawn.")
TAG ("%%Maximum frequency% (Hz)")
DEFINITION ("the height of the %y axis. For speech, 5000 Hz is a usual value.")
TAG ("%%Dynamic range% (dB)")
DEFINITION ("determines the signal intensity (as stored in each formant frame) below which "
	"no formants will be drawn. If zero, all formants will be drawn. The standard value is 30 dB, "
	"which would mean that formants in frames with intensities less than the maximum intensity minus 30 dB will not be drawn.")
TAG ("%%Garnish")
DEFINITION ("determines whether axes, numbers, and texts (\"Time\", \"Formant frequency\") will be drawn in the margins around the picture. "
	"Turn this button off if you prefer to garnish your picture by yourself with the @Margins menu.")
MAN_END

MAN_BEGIN ("Formant: Draw tracks...", "ppgb", 19980321)
INTRO ("A command to draw the selected @Formant objects to the @@Picture window@.")
ENTRY ("Behaviour")
NORMAL ("Every formant value is drawn as one or two short line segments, connected, if possible, "
	"with a line segment of the corresponding formant values in the adjacent frames.")
ENTRY ("Arguments")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the time domain of the drawing. If %%To time% is not greater than %%From time%, "
	"the entire formant contour is drawn.")
TAG ("%%Maximum frequency% (Hz)")
DEFINITION ("the height of the %y axis. For speech, 5000 Hz is a usual value.")
TAG ("%%Garnish")
DEFINITION ("determines whether axes, numbers, and texts (\"Time\", \"Formant frequency\") will be drawn in the margins around the picture. "
	"Turn this button off if you prefer to garnish your picture by yourself with the @Margins menu.")
ENTRY ("Usage")
NORMAL ("Unlike @@Formant: Speckle...@, this procedure assumes that e.g. the second formant in frame %i "
	"has something to do with the second formant in frame %i+1. To make this assumption more plausible, "
	"use @@Formant: Track...@ first.")
MAN_END

MAN_BEGIN ("Formant: Formula (bandwidths)...", "ppgb", 19980323)
INTRO ("A command to modify each selected @Formant object with a specified formula.")
NORMAL ("For what you can do with formulas, see @@Matrix: Formula...@. "
	"The %%i%th row contains the values of the %%i%th bandwidth contour.")
NORMAL ("See @@Formant: Formula (frequencies)...@ for more information.")
MAN_END

MAN_BEGIN ("Formant: Formula (frequencies)...", "ppgb", 19981221)
INTRO ("A command to modify each selected @Formant object with a specified formula.")
NORMAL ("For what you can do with formulas, see @@Matrix: Formula...@. "
	"The %%i%th row contains the values of the %%i%th frequency contour.")
NORMAL ("For an example, see @@Source-filter synthesis@.")
MAN_END

MAN_BEGIN ("Formant: Get bandwidth at time...", "ppgb", 19991016)
INTRO ("A @query to the selected @Formant object for the bandwidth of the specified formant at the specified time.")
ENTRY ("Arguments")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%Time% (s)")
DEFINITION ("the time at which to evaluate the bandwidth.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
ENTRY ("Return value")
NORMAL ("the estimated bandwidth in Hertz or Bark. "
	"If %Time is not within half a frame width of any frame centre, or "
	"If the specified %%Formant number% is greater than the number of formants in the frame, "
	"the return value is @undefined; "
	"otherwise, the formant is considered to belong to the frame whose centre is nearest to the specified time.")
ENTRY ("Algorithm")
NORMAL ("If possible (i.e. if the adjacent frame has enough formants), "
	"a linear interpolation is performed between the centre of the frame and the centre of the adjacent frame. "
	"With Bark units, the Hertz-to-Bark transformation is performed "
	"on the two frequencies %F \\+_ 1/2 %B (after interpolation), and the result is the difference between these two values")
MAN_END

MAN_BEGIN ("Formant: Get maximum...", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the maximum value of the specified formant.")
ENTRY ("Return value")
NORMAL ("the maximum, in Hertz or Bark.")
ENTRY ("Arguments")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the formant contour is considered.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
TAG ("%Interpolation")
DEFINITION ("the interpolation method (none or parabolic). See @@vector peak interpolation@.")
MAN_END

MAN_BEGIN ("Formant: Get mean...", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the mean value of the specified formant.")
ENTRY ("Return value")
NORMAL ("the mean, in Hertz or Bark.")
ENTRY ("Arguments")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the formant contour is considered.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
ENTRY ("Scripting")
NORMAL ("You can use this command to put the mean into a script variable:")
CODE ("select Formant hallo")
CODE ("mean = Get mean... 2 0 0 Hertz")
NORMAL ("In this case, the value will not be written into the Info window.")
MAN_END

MAN_BEGIN ("Formant: Get minimum...", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the minimum value of the specified formant.")
ENTRY ("Return value")
NORMAL ("the minimum, in Hertz or Bark.")
ENTRY ("Arguments")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the formant contour is considered.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
TAG ("%Interpolation")
DEFINITION ("the interpolation method (none or parabolic). See @@vector peak interpolation@.")
MAN_END

MAN_BEGIN ("Formant: Get number of formants", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the number of formants in a specified frame.")
ENTRY ("Argument")
TAG ("%%Frame number")
DEFINITION ("the frame number whose formant count is sought.")
ENTRY ("Return value")
NORMAL ("the number of formants.")
MAN_END

MAN_BEGIN ("Formant: Get quantile...", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for an estimation of the specified quantile of the distribution "
	"that underlies the attested values of the specified formant.")
ENTRY ("Return value")
NORMAL ("the quantile, in Hertz or Bark.")
ENTRY ("Arguments")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the formant contour is considered.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
TAG ("%Quantile")
DEFINITION ("the fraction (between 0 and 1) of the values of the underlying distribution expected to lie below the result. "
	"For instance, if %Quantile is 0.10, the algorithm estimates the formant value below which 10\\%  of all formant values "
	"are expected to lie. To get an estimate of the %median of the underlying distribution, specify a quantile of 0.50.")
ENTRY ("Algorithm")
NORMAL ("First, the available formant values within the selected time domain are collected in an array. "
	"This array is then sorted and the @@quantile algorithm@ is performed. With Bark units, "
	"the Hertz-to-Bark transformation is performed before the quantile algorithm.")
MAN_END

MAN_BEGIN ("Formant: Get standard deviation", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the standard deviation of the attested values of the specified formant "
	"within a specified time domain.")
ENTRY ("Return value")
NORMAL ("the standard deviation, in Hertz or Bark.")
ENTRY ("Arguments")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the formant contour is considered.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
MAN_END

MAN_BEGIN ("Formant: Get time of maximum...", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the time associated with the maximum value "
	"of a specified formant within a specified time domain.")
ENTRY ("Argument")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored, except for interpolation. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the formant contour is considered.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
TAG ("%Interpolation")
DEFINITION ("the interpolation method (None or Parabolic). See @@vector peak interpolation@.")
ENTRY ("Return value")
NORMAL ("the time expressed in seconds. If no relevant formants are found, the value is @undefined.")
MAN_END

MAN_BEGIN ("Formant: Get time of minimum...", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the time associated with the minimum value "
	"of a specified formant within a specified time domain.")
ENTRY ("Argument")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored, except for interpolation. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the formant contour is considered.")
TAG ("%Units")
DEFINITION ("the units of the result (Hertz or Bark).")
TAG ("%Interpolation")
DEFINITION ("the interpolation method (None or Parabolic). See @@vector peak interpolation@.")
ENTRY ("Return value")
NORMAL ("the time expressed in seconds. If no relevant formants are found, the value is @undefined.")
MAN_END

MAN_BEGIN ("Formant: Get value at time...", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Formant object for the frequency of the specified formant at the specified time.")
ENTRY ("Arguments")
TAG ("%%Formant number")
DEFINITION ("the ordinal number of the formant, counting up from 0 Hz. Specify \"2\" for %F__2_ etc.")
TAG ("%%Time% (s)")
DEFINITION ("the time at which to evaluate the formant frequency.")
TAG ("%Units)")
DEFINITION ("the units of the result (Hertz or Bark).")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method, see @@vector value interpolation@. "
	"Always Linear.")
ENTRY ("Return value")
NORMAL ("the bandwidth in Hertz or Bark. "
	"If %Time is not within half a frame width of any frame centre, or "
	"if the specified %%Formant number% is greater than the number of formants in the frame, "
	"the value is @undefined; "
	"otherwise, the formant is considered to belong to the frame whose centre is nearest to the specified time.")
ENTRY ("Algorithm")
NORMAL ("If possible (i.e. if the adjacent frame has enough formants), "
	"a linear interpolation is performed between the centre of the frame and the centre of the adjacent frame. "
	"With Bark units, the Hertz-to-Bark transformation is performed before interpolation.")
MAN_END

MAN_BEGIN ("Formant: Track...", "ppgb", 20020308)
INTRO ("A command to extract a specified number of formant tracks from each selected @Formant object. "
	"The tracks represent the cheapest paths through the measured formant values in consecutive frames.")
ENTRY ("How to use")
NORMAL ("In order to be capable of producing three tracks (i.e. F1, F2, and F3), there must be at least "
	"three formant candidates in %every frame of the Formant object. The typical use of "
	"this command, therefore, is to analyse five formants with @@Sound: To Formant (burg)...@ "
	"and then use the tracking command to extract three tracks.")
ENTRY ("When to use, when not")
NORMAL ("This command only makes sense if the whole of the formant contour makes sense. "
	"For speech, formant contours make sense only for vowels and the like. "
	"During some consonants, the Formant object may have fewer than three formant values, "
	"and trying to create three tracks through them will fail. "
	"You will typically use this command for the contours in diphthongs, if at all.")
ENTRY ("Arguments")
NORMAL ("To be able to interpret the arguments, you should know that the aim of the procedure is "
	"to minimize the sum of the %costs associated with the three tracks.")
TAG ("%%Number of tracks")
DEFINITION ("the number of formant tracks that the procedure must find. If this number is 3, "
	"the procedure will try to find tracks for F1, F2, and F3; if the Formant object contains "
	"a frame with less than three formants, the tracking procedure will fail.")
TAG ("%%Reference F1% (Hertz)")
DEFINITION ("the preferred value near which the first track wants to be. "
	"For average (i.e. adult female) speakers, this value will be around "
	"the average F1 for vowels of female speakers, i.e. 550 Hz.")
TAG ("%%Reference F2% (Hertz)")
DEFINITION ("the preferred value near which the second track wants to be. "
	"A good value will be around "
	"the average F2 for vowels of female speakers, i.e. 1650 Hz.")
TAG ("%%Reference F3% (Hertz)")
DEFINITION ("the preferred value near which the third track wants to be. "
	"A good value will be around "
	"the average F3 for vowels of female speakers, i.e. 2750 Hz. "
	"This argument will be ignored if you choose to have fewer than three tracks, "
	"i.e., if you are only interested in F1 and F2.")
TAG ("%%Reference F4% (Hertz)")
DEFINITION ("the preferred value near which the fourth track wants to be. "
	"A good value may be around 3850 Hz, but you will usually not want to "
	"track F4, because traditional formant lore tends to ignore it "
	"(however inappropriate this may be for the vowel [i]), "
	"and because Formant objects often contain not more than three formant values "
	"in some frames. So you will not usually specify a higher %%Number of tracks% than 3, "
	"and in that case, this argument will be ignored.")
TAG ("%%Reference F5% (Hertz)")
DEFINITION ("the preferred value near which the five track wants to be. "
	"In the unlikely case that you want five tracks, "
	"a good value may be around 4950 Hz.")
TAG ("%%Frequency cost% (per kiloHertz)")
DEFINITION ("the local cost of having a formant value in your track that deviates "
	"from the reference value. For instance, if a candidate (i.e. any formant in a frame "
	"of the Formant object) has a formant frequency of 800 Hz, "
	"and the %%Frequency cost% is 1.0/kHz, "
	"the cost of putting this formant in the first track is 0.250, "
	"because the distance to the reference F1 of 550 Hz is 250 Hz. "
	"The cost of putting the formant in the second track would be 0.850 "
	"(= (1.650 kHz - 0.600 kHz) \\.c 1.0/kHz), so we see that the procedure "
	"locally favours the inclusion of the 800 Hz candidate into the F1 track. "
	"But the next two cost factors may override this local preference.")
TAG ("%%Bandwidth cost%")
DEFINITION ("the local cost of having a bandwidth, relative to the formant frequency. "
	"For instance, if a candidate has a formant frequency of 400 Hz and "
	"a bandwidth of 80 Hz, and the %%Bandwidth cost% is 1.0, "
	"the cost of having this formant in any track is (80/400) \\.c 1.0 = 0.200. "
	"So we see that the procedure locally favours the inclusion of candidates "
	"with low relative bandwidths.")
TAG ("%%Transition cost% (per octave)")
DEFINITION ("the cost of having two different consecutive formant values in a track. "
	"For instance, if a proposed track through the candidates has two consecutive formant "
	"values of 300 Hz and 424 Hz, and the %%Transition cost% is 1.0/octave, "
	"the cost of having this large frequency jump is (0.5 octave) \\.c (1.0/octave) = 0.500.")
ENTRY ("Algorithm")
NORMAL ("This command uses a Viterbi algorithm with multiple planes. For instance, if the selected Formant object "
	"contains up to five formants per frame, and you request three tracks, the Viterbi algorithm will have to choose "
	"between ten candidates (the number of combinations of three out of five) for each frame.")
NORMAL ("The formula for the cost of e.g. track 3, with proposed values %F__2%i_ (%i = 1...%N, "
	"where %N is the number of frames) is:")
FORMULA ("\\su__%i=1..%N_ %frequencyCost\\.c\\|f%F__3%i_ \\-- %referenceF3\\|f/1000 +")
FORMULA ("+ \\su__%i=1..%N_ %bandWidthCost\\.c%B__3%i_/%F__3%i_ +")
FORMULA ("+ \\su__%i=1..%N-1_ %transitionCost\\.c\\|flog__2_(%F__3%i_/%F__3,%i+1_)\\|f")
NORMAL ("Analogous formulas compute the cost of track 1 and track 2. "
	"The procedure will assign those candidates to the three tracks that minimize "
	"the sum of three track costs.")
MAN_END

MAN_BEGIN ("FormantTier", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("A FormantTier object represents spectral structure as a function of time: a %%formant contour%. "
	"Unlike the evenly sampled @Formant object, it consists of a number of formant %points (or %targets), sorted by time. "
	"Each point contains contains several formant/bandwidth pairs.")
NORMAL ("For examples, see @@Source-filter synthesis@.")
ENTRY ("FormantTier commands")
NORMAL ("Creation:")
LIST_ITEM ("From scratch:")
LIST_ITEM ("\\bu @@Create FormantTier...")
LIST_ITEM ("\\bu @@FormantTier: Add point...")
LIST_ITEM ("Copy from another object:")
LIST_ITEM ("\\bu @@Formant: Down to FormantTier@: trivial copying of frames to points.")
NORMAL ("Conversion:")
LIST_ITEM ("\\bu @@FormantTier: Down to PointProcess@: copy times.")
NORMAL ("Synthesis:")
LIST_ITEM ("\\bu @@Sound & FormantTier: Filter@: see @@Source-filter synthesis@.")
NORMAL ("Queries:")
LIST_ITEM ("\\bu @@Get low index from time...")
LIST_ITEM ("\\bu @@Get high index from time...")
LIST_ITEM ("\\bu @@Get nearest index from time...")
NORMAL ("Modification:")
LIST_ITEM ("\\bu @@Remove point...")
LIST_ITEM ("\\bu @@Remove point near...")
LIST_ITEM ("\\bu @@Remove points between...")
LIST_ITEM ("\\bu @@FormantTier: Add point...")
ENTRY ("FormantTier attributes")
NORMAL ("With @Inspect, you will see the following attribute:")
TAG ("%points")
DEFINITION ("when you open this, you see the %size (the number of points) and a series of points (%item [%i], %i = 1...%n).")
ENTRY ("Attributes of a formant point")
NORMAL ("Each point contains the following attributes:")
TAG ("%time")
DEFINITION ("the time associated with this formant target, in seconds. Also used to sort the targets.")
TAG ("%numberOfFormants")
DEFINITION ("the number of formant/bandwidth pairs (never more than 10).")
TAG ("%formant [0..%numberOfFormants-1]")
DEFINITION ("the formant frequencies in Hertz. ##Caution:# the frequency of the %%i%th formant is in %formant [%i-1]!")
TAG ("%bandwidth [0..%numberOfFormants-1]")
DEFINITION ("the formant bandwidths in Hertz. ##Caution:# the bandwidth of the %%i%th formant is in %bandwidth [%i-1]!")
MAN_END

MAN_BEGIN ("FormantTier: Add point...", "ppgb", 19981221)
INTRO ("A command to add a point to each selected @FormantTier.")
NORMAL ("For examples, see @@Source-filter synthesis@.")
ENTRY ("Arguments")
TAG ("%Time (s)")
DEFINITION ("the time at which a point is to be added.")
TAG ("%%Frequencies and bandwidths (Hz)")
DEFINITION ("the frequency and bandwidth values of the requested new point. To get three formants at "
	"500, 1500, and 2500 Hz with bandwidths of 50, 100, and 150 Hz, respectively, "
	"you specify \"500 50 1500 100 2500 150\".")
ENTRY ("Behaviour")
NORMAL ("The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN ("FormantTier: Down to PointProcess", "ppgb", 20010410)
INTRO ("A command to degrade every selected @FormantTier to a @PointProcess.")
ENTRY ("Behaviour")
NORMAL ("The times of all the formant points are trivially copied, and so is the time domain. "
	"The formant information is lost.")
MAN_END

MAN_BEGIN ("Sound: To Formant (burg)...", "ppgb", 20031003)
INTRO ("A command that creates a @Formant object from every selected @Sound object. "
	"It performs a short-term spectral analysis, approximating the spectrum of each "
	"analysis frame by a number of formants.")
ENTRY ("Arguments")
TAG ("%%Time step% (seconds)")
DEFINITION ("the time between the centres of consecutive analysis frames. If the sound is 2 seconds long, "
	"and the time step is 0.01 seconds, there will be approximately 200 analysis frames. "
	"The actual number is somewhat lower (usually 195), because we cannot measure very well "
	"near the edges. If you set the time step to 0.0 (the standard), Praat will use a time step "
	"that is equal to 25 percent of the analysis window length (see below).")
TAG ("%%Maximum number of formants%")
DEFINITION ("for most analyses of human speech, you will want to extract 5 formants per frame. "
	"This, in combination with the %%Maximum formant% argument, is the only way "
	"in which this procedure will give you results compatible with how people "
	"tend to interpret formants for vowels, i.e. in terms of "
	"vowel height (F1) and vowel place (F2).")
TAG ("%%Maximum formant% (Hertz)")
DEFINITION ("the ceiling of the formant search range. It is crucial that you set this argument to a value suitable for your speaker. "
	"The standard value of 5500 Hz is suitable for an average adult female. For a male, use 5000 Hz; "
	"if you use 5500 Hz for an adult male, you may end up with too few formants in the low frequency region, "
	"e.g. analysing an [u] as having a single formant near 500 Hz whereas you want two formants at 300 and 600 Hz. "
	"For a young child, use a value much higher than 5500 Hz, for instance 8000 Hz (experiment with it on steady vowels).")
TAG ("%%Window length% (seconds)")
DEFINITION ("the effective duration of the analysis window. The actual length is twice this value, "
	"because Praat uses a Gaussian-like analysis window with sidelobes below -120 dB. "
	"For instance, if the %%Window length% is 0.025 seconds, the actual Gaussian window duration "
	"is 0.050 seconds. This window has values below 4\\%  outside the central 0.025 seconds, "
	"and its frequency resolution (-3 dB point) is 1.298 / (0.025 s) = 51.9 Hz, as computed "
	"with the formula given at @@Sound: To Spectrogram...@. This is comparable to the bandwidth "
	"of a Hamming window of 0.025 seconds, which is 1.303 / (0.025 s) = 52.1 Hz, "
	"but that window (which is the window most often used in other analysis programs) "
	"has three spectral lobes of about -42 dB on each side.")
TAG ("%%Pre-emphasis from% (Hertz)")
DEFINITION ("the +3 dB point for an inverted low-pass filter with a slope of +6 dB/octave. "
	"If this value is 50 Hz, then frequencies below 50 Hz are not enhanced, "
	"frequencies around 100 Hz are amplified by 6 dB, frequencies around 200 Hz are amplified by 12 dB, "
	"and so forth. The point of this is that vowel spectra tend to fall by 6 dB per octave; "
	"the pre-emphasis creates a flatter spectrum, which is better for formant analysis "
	"because we want our formants to match the local peaks, not the global spectral slope. "
	"See the @@source-filter synthesis@ tutorial for a technical explanation, "
	"and @@Sound: Pre-emphasize (in-line)...@ for the algorithm.")
ENTRY ("Algorithm")
NORMAL ("The sound will be resampled to a sampling frequency of twice the value of %%Maximum formant%, "
	"with the algorithm described at @@Sound: Resample...@. "
	"After this, pre-emphasis is applied with the algorithm described at @@Sound: Pre-emphasize (in-line)...@. "
	"For each analysis window, Praat applies a Gaussian-like window, "
	"and computes the LPC coefficients with the algorithm by Burg, as given by @@Childers (1978)@ "
	"and @@Press et al. (1992)@.")
NORMAL ("This algorithm can initially find formants at very low or high frequencies. "
	"In order for you to be able to identify the traditional F1 and F2, "
	"all formants below 50 Hz and all formants above %%Maximum formant% minus 50 Hz, "
	"are removed. If you don't want this, you may experiment with @@Sound: To Formant (keep all)...@ instead. "
	"If you prefer an algorithm that always yields the requested number of formants, nicely distributed "
	"across the frequency domain, you may try the otherwise rather unreliable Split-Levinson procedure @@Sound: To Formant (sl)...@.")
MAN_END

MAN_BEGIN ("Sound: To Formant (keep all)...", "ppgb", 20000210)
INTRO ("A command that creates a @Formant object from every selected @Sound object. Not recommended for general use.")
ENTRY ("Purpose")
NORMAL ("to perform a short-term spectral analysis, approximating the spectrum of each frame by a number of formants.")
ENTRY ("Arguments")
NORMAL ("The same as with @@Sound: To Formant (burg)...@.")
ENTRY ("Algorithm")
NORMAL ("The same as with @@Sound: To Formant (burg)...@. In contrast with that command, "
	"however, all formant values are kept, even those below 50 Hz and those above %%Maximum formant% minus 50 Hz. "
	"Although this makes the identification of the traditional F1 and F2 more difficult, "
	"this might give better results in resynthesis (see @@Sound & Formant: Filter@), but it usually generates funny values instead.")
MAN_END

MAN_BEGIN ("Sound: To Formant (sl)...", "ppgb", 20021215)
INTRO ("A command that creates a @Formant object from every selected @Sound object. Not recommended for general use.")
ENTRY ("Purpose")
NORMAL ("to perform a short-term spectral analysis, approximating the spectrum of each frame by a number of formants.")
ENTRY ("Arguments")
NORMAL ("The same as with @@Sound: To Formant (burg)...@.")
ENTRY ("Algorithm")
NORMAL ("The algorithm is based on the implementation of the `Split Levinson' algorithm by @@Willems (1986)@. "
	"This algorithm will always find the requested number of formants in every frame, even if they do not exist. "
	"The standard routine (@@Sound: To Formant (burg)...@) yields much more reliable formant values, though it is more sensitive "
	"to the %%Maximum formant% argument.")
NORMAL ("Because of the general funny behaviour of the Split-Levinson algorithm, we did not bother to implement an analysis "
	"of the bandwidths. They are all set arbitrarily to 50 Hz.")
MAN_END

MAN_BEGIN ("Sound & Formant: Filter", "ppgb", 19991119)
INTRO ("A command to create a new Sound from the selected @Sound and @Formant objects.")
NORMAL ("For examples, see @@Source-filter synthesis@.")
NORMAL ("The resulting Sound is scaled so that its maximum absolute amplitude is 0.99. "
	"If you don't want this, use @@Sound & Formant: Filter (no scale)@ instead.")
MAN_END

MAN_BEGIN ("Sound & Formant: Filter (no scale)", "ppgb", 19991119)
INTRO ("A command to create a new Sound from the selected @Sound and @Formant objects.")
NORMAL ("For examples, see @@Source-filter synthesis@.")
NORMAL ("Unlike what happens in @@Sound & Formant: Filter@, "
	"the resulting Sound is not scaled. This allows generation of a series "
	"of signals with controlled relative intensities.")
MAN_END

MAN_BEGIN ("Sound & FormantTier: Filter", "ppgb", 19991120)
INTRO ("A command to create a new Sound from the selected @Sound and @FormantTier objects.")
NORMAL ("For examples, see @@Source-filter synthesis@.")
NORMAL ("The resulting Sound is scaled so that its maximum absolute amplitude is 0.99. "
	"If you don't want this, use @@Sound & FormantTier: Filter (no scale)@ instead.")
MAN_END

MAN_BEGIN ("Sound & FormantTier: Filter (no scale)", "ppgb", 19991119)
INTRO ("A command to create a new Sound from the selected @Sound and @FormantTier objects.")
NORMAL ("For examples, see @@Source-filter synthesis@.")
NORMAL ("Unlike what happens in @@Sound & FormantTier: Filter@, "
	"the resulting Sound is not scaled. This allows generation of a series "
	"of signals with controlled relative intensities.")
MAN_END

}

/* End of file manual_formant.c */
