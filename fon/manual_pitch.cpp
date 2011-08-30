/* manual_pitch.cpp
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

void manual_pitch_init (ManPages me);
void manual_pitch_init (ManPages me) {

MAN_BEGIN (L"Create PitchTier...", L"ppgb", 20110101)
INTRO (L"A command in the @@New menu@ to create an empty @PitchTier object.")
NORMAL (L"The resulting object will have the specified name and time domain, but contain no pitch points. "
	"To add some points to it, use @@PitchTier: Add point...@.")
NORMAL (L"For an example, see @@Source-filter synthesis@.")
MAN_END

MAN_BEGIN (L"Harmonicity", L"ppgb", 20030610)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"A Harmonicity object represents the degree of acoustic periodicity, "
	"also called Harmonics-to-Noise Ratio (HNR). "
	"Harmonicity is expressed in dB: "
	"if 99\\%  of the energy of the signal is in the periodic part, and 1\\%  is noise, "
	"the HNR is 10*log10(99/1) = 20 dB. "
	"A HNR of 0 dB means that there is equal energy in the harmonics and in the noise.")
NORMAL (L"Harmonicity can be used as a measure for:")
LIST_ITEM (L"\\bu The signal-to-noise ratio of anything that generates a periodic signal.")
LIST_ITEM (L"\\bu Voice quality. For instance, a healthy speaker can produce a sustained [a] or [i] "
	"with a harmonicity of around 20 dB, and an [u] at around 40 dB; "
	"the difference comes from the high frequencies in [a] and [i], versus low frequencies in [u], "
	"resulting in a much higher sensitivity of HNR to jitter in [a] and [i] than in [u]. "
	"Hoarse speakers will have an [a] with a harmonicity much lower than 20 dB. "
	"We know of a pathological case where a speaker had an HNR of 40 dB for [i], "
	"because his voice let down above 2000 Hz.")
ENTRY (L"Harmonicity commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Sound: To Harmonicity (cc)...@: cross-correlation method (preferred).")
LIST_ITEM (L"\\bu @@Sound: To Harmonicity (ac)...@: autocorrelation method.")
MAN_END

MAN_BEGIN (L"Harmonicity: Formula...", L"ppgb", 20021206)
INTRO (L"A command for changing the data in all selected @Harmonicity objects.")
NORMAL (L"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (L"Harmonicity: Get maximum...", L"ppgb", 20030916)
INTRO (L"A @query to the selected @Harmonicity object.")
ENTRY (L"Return value")
NORMAL (L"the maximum value, expressed in dB.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the Harmonicity object is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Harmonicity: Get mean...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Harmonicity object.")
ENTRY (L"Return value")
NORMAL (L"the mean value, expressed in dB.")
ENTRY (L"Setting")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
ENTRY (L"Algorithm")
NORMAL (L"The mean harmonicity between the times %t__1_ and %t__2_ is defined as")
FORMULA (L"1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %dt %x(%t)")
NORMAL (L"where %x(%t) is the harmonicity (in dB) as a function of time. "
	"Frames in which the value is undefined (i.e. in silent intervals) "
	"are ignored. If all the frames are silent, the returned value is @undefined.")
MAN_END

MAN_BEGIN (L"Harmonicity: Get minimum...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Harmonicity object.")
ENTRY (L"Return value")
NORMAL (L"the minimum value, expressed in dB.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Harmonicity: Get standard deviation...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Harmonicity object.")
ENTRY (L"Return value")
NORMAL (L"the standard deviation, expressed in dB.")
ENTRY (L"Setting")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
ENTRY (L"Algorithm")
NORMAL (L"The standard deviation between the times %t__1_ and %t__2_ is defined as")
FORMULA (L"1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %dt (%x(%t) - %\\mu)^2")
NORMAL (L"where %x(%t) is the harmonicity (in dB) as a function of time, and %\\mu its mean. "
	"For our discrete Harmonicity object, the standard deviation is approximated by")
FORMULA (L"1/(%n-1) \\su__%i=%m..%m+%n-1_ (%x__%i_ - %\\mu)^2")
NORMAL (L"where %n is the number of frame centres between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN (L"Harmonicity: Get time of maximum...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Harmonicity object for the time associated with its maximum value.")
ENTRY (L"Return value")
NORMAL (L"the time (in seconds) associated with the maximum HNR value.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Harmonicity: Get time of minimum...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Harmonicity object.")
ENTRY (L"Return value")
NORMAL (L"the time (in seconds) associated with the minimum HNR value.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Harmonicity is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Harmonicity: Get value at time...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Harmonicity object to .")
ENTRY (L"Return value")
NORMAL (L"an estimate (in dB) of the value at a specified time. "
	"If this time is outside the time domain or outside the samples of the Harmonicity, the result is @undefined.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time at which the value is to be evaluated.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method, see @@vector value interpolation@. "
	"The standard is Cubic because of the usual nonlinearity (logarithm) in the computation of harmonicity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Harmonicity: Get value in frame...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Harmonicity object.")
ENTRY (L"Return value")
NORMAL (L"the value in a specified frame, expressed in dB. "
	"If the index is less than 1 or greater than the number of frames, the result is @undefined.")
ENTRY (L"Setting")
TAG (L"##Frame number")
DEFINITION (L"the frame whose value is to be looked up.")
MAN_END

MAN_BEGIN (L"Pitch", L"ppgb", 20101230)
INTRO (L"One of the @@types of objects@ in Praat. For tutorial information, see @@Intro 4. Pitch analysis@.")
NORMAL (L"A Pitch object represents periodicity candidates as a function of time. "
	"It does not mind whether this periodicity refers to acoustics, "
	"perception, or vocal-cord vibration. "
	"It is sampled into a number of %frames centred around equally spaced times.")
ENTRY (L"Pitch commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Sound: To Pitch...@: preferred method (autocorrelation).")
LIST_ITEM (L"\\bu @@Sound: To Pitch (ac)...@: autocorrelation method (all parameters).")
LIST_ITEM (L"\\bu @@Sound: To Pitch (cc)...@: cross-correlation method.")
NORMAL (L"Drawing:")
LIST_ITEM (L"\\bu @@Pitch: Draw...@")
NORMAL (L"Viewing and editing:")
LIST_ITEM (L"\\bu @PitchEditor")
NORMAL (L"Synthesis:")
LIST_ITEM (L"\\bu @@Pitch: To PointProcess@: create points in voiced intervals.")
LIST_ITEM (L"\\bu @@Sound & Pitch: To PointProcess (cc)@: near locations of high amplitude.")
LIST_ITEM (L"\\bu @@Sound & Pitch: To PointProcess (peaks)...@: near locations of high amplitude.")
NORMAL (L"Conversion:")
LIST_ITEM (L"\\bu @@Pitch: To PitchTier@: time-stamp voiced intervals.")
LIST_ITEM (L"\\bu @@Pitch & PointProcess: To PitchTier...@: interpolate values at specified times.")
ENTRY (L"Inside a Pitch object")
NORMAL (L"With @Inspect, you will see the following attributes:")
TAG (L"%x__%min_")
DEFINITION (L"start time, in seconds.")
TAG (L"%x__%max_")
DEFINITION (L"end time, in seconds.")
TAG (L"%n__%x_")
DEFINITION (L"the number of frames (\\>_ 1).")
TAG (L"%dx")
DEFINITION (L"time step = frame length = frame duration, in seconds.")
TAG (L"%x__1_")
DEFINITION (L"the time associated with the first frame, in seconds. "
	"This will usually be in the range [%xmin, %xmax]. "
	"The time associated with the last frame (i.e., %x__1_ + (%n__%x_ \\-- 1) %dx)) "
	"will also usually be in that range.")
TAG (L"%ceiling")
DEFINITION (L"a frequency above which a candidate is considered voiceless.")
TAG (L"%frame__%i_, %i = 1 ... %n__%x_")
DEFINITION (L"the frames (see below).")
ENTRY (L"Attributes of a pitch frame")
NORMAL (L"Each frame contains the following attributes:")
TAG (L"%nCandidates")
DEFINITION (L"the number of candidates in this frame (at least one: the `unvoiced' candidate).")
TAG (L"%candidate__%j_, %j = 1 ... %nCandidates")
DEFINITION (L"the information about each candidate (see below).")
ENTRY (L"Attributes of each candidate")
NORMAL (L"Each candidate contains the following attributes:")
TAG (L"%frequency")
DEFINITION (L"the candidate's frequency in Hz (for a voiced candidate), or 0 (for an unvoiced candidate).")
TAG (L"%strength")
DEFINITION (L"the degree of periodicity of this candidate (between 0 and 1).")
ENTRY (L"Interpretation")
NORMAL (L"The current pitch contour is determined by the path through all first candidates. "
	"If the first candidate of a certain frame has a frequency of 0, "
	"or a frequency above %ceiling, this frame is considered voiceless.")
MAN_END

MAN_BEGIN (L"Pitch: Draw...", L"ppgb", 19960910)
INTRO (L"A command for drawing the selected @Pitch objects into the @@Picture window@.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (seconds)")
DEFINITION (L"the time domain along the horizontal axis. "
	"If these are both zero, the time domain of the #Pitch itself is taken (autowindowing).")
TAG (L"##Minimum frequency (Hz)")
TAG (L"##Maximum frequency (Hz)")
DEFINITION (L"the frequency range along the vertical axis. "
	"##Maximum frequency# must be greater than ##Minimum frequency#.")
ENTRY (L"Behaviour")
NORMAL (L"In unvoiced frames, nothing will be drawn.")
NORMAL (L"In voiced frames, the pitch frequency associated with the frame "
	"is thought to represent the time midpoint of the frame, "
	"but frequencies will be drawn at all time points in the frame, as follows:")
LIST_ITEM (L"\\bu If two adjacent frames are both voiced, the frequency of the time points between the midpoints "
	"of the frames is linearly interpolated from both midpoints.")
LIST_ITEM (L"\\bu If a voiced frame is adjacent to another voiced frame on one side, "
	"and to a voiceless frame on the other side, "
	"the frequencies in the half-frame on the unvoiced side will be linearly extrapolated "
	"from the midpoints of the two voiced frames involved.")
LIST_ITEM (L"\\bu If a voiced frame is adjacent to two unvoiced frames, "
	"a horizontal line segment will be drawn at the frequency of the midpoint.")
MAN_END

/*
F0beg = Get value at time... tbeg Hertz Linear
while F0beg = undefined
   tbeg = tbeg + 0.01
   F0beg = Get value at time... tbeg Hertz Linear
endwhile
*/

MAN_BEGIN (L"Pitch: Interpolate", L"ppgb", 19990811)
INTRO (L"A command that converts every selected @Pitch object.")
/*
	myID = selected ("Pitch")
	tmin = Get start time
	tmax = Get end time
	numberOfFrames = Get number of frames
	timeStep = Get time step
	t1 = Get time of frame... 1
	ceiling = Get ceiling
	Create Pitch... interpolated tmin tmax numberOfFrames timeStep t1 ceiling 2
	for iframe to numberOfFrames
		select 'myID'
		f = Get frequency... iframe 1
		if f > 0.0 and f < ceiling
			# The frame is voiced? Copy the frequency.
			select Pitch interpolated
			Set frequency... iframe 1 f
		else
			# The frame is unvoiced? Interpolate the frequency.
			fleft = 0.0
			fright = 0.0
			# Find nearest voiced frame to the left.
			left = iframe - 1
			while left >= 1 and fleft = 0.0
				fleft = Get frequency... left 1
				if fleft >= ceiling
					fleft = 0.0
				endif
				left = left - 1
			endwhile
			# Find nearest voiced frame to the right.
			right = iframe + 1
			while right <= numberOfFrames and fright = 0.0
				fright = Get frequency... right 1
				if fright >= ceiling
					fright = 0.0
				endif
				right = right + 1
			endwhile
			if fleft <> 0 and fright <> 0
				select Pitch interpolated
				Set frequency... iframe 1 ((iframe - left) * fright + (right - iframe) * fleft) / (right - left)
			endif
		endif
		select Pitch interpolated
		Set strength... iframe 1 0.9
	endfor
	select Pitch interpolated
*/
MAN_END

MAN_BEGIN (L"Pitch: Smooth...", L"ppgb", 19990811)
INTRO (L"A command that converts every selected @Pitch object.")
MAN_END

MAN_BEGIN (L"Pitch: To PitchTier", L"ppgb", 19960915)
INTRO (L"A command that converts a @Pitch object into a @PitchTier object.")
ENTRY (L"Algorithm")
NORMAL (L"The #PitchTier object will contain as many points as there were voiced frames in the #Pitch.")
NORMAL (L"The %time of each point is the time associated with the centre of the corresponding %frame "
	"of the #Pitch contour. The %frequency of the point is the pitch frequency associated in this frame "
	"with the current path through the candidates.")
MAN_END

MAN_BEGIN (L"Pitch: To PointProcess", L"ppgb", 19960917)
INTRO (L"A command that uses a @Pitch object to generate a @PointProcess.")
ENTRY (L"Purpose")
NORMAL (L"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (L"Algorithm")
LIST_ITEM (L"1. A @PitchTier is created with @@Pitch: To PitchTier@.")
LIST_ITEM (L"2. The algorithm of @@PitchTier: To PointProcess@ generates points along the entire time domain "
	"of the PitchTier.")
LIST_ITEM (L"3. The PitchTier is removed (it never appeared in the List of Objects).")
LIST_ITEM (L"4. The voiced/unvoiced information in the Pitch is used to remove all points "
	"that lie within voiceless frames.")
MAN_END

MAN_BEGIN (L"Pitch & PointProcess: To PitchTier...", L"ppgb", 20101230)
INTRO (L"A command that creates a @PitchTier object from one selected @Pitch and one selected @PointProcess object.")
ENTRY (L"Purpose")
NORMAL (L"to return the frequencies in the Pitch contour at the times specified by the PointProcess.")
ENTRY (L"Setting")
TAG (L"##Check voicing# (standard: on)")
DEFINITION (L"determines whether, if the time of a mark is not within a voiced frame, you will get a message like "
	"\"No periodicity at time %xxx.\", and no PitchTier is created. If this button is off, "
	"the resulting pitch frequency will be 0.0 Hz.")
ENTRY (L"Normal behaviour")
NORMAL (L"For all the times in the PointProcess, a pitch frequency is computed from the "
	"information in the Pitch, by linear interpolation.")
NORMAL (L"All the resulting time-frequency pairs are put in a new PitchTier object.")
NORMAL (L"The time domain of the resulting PitchTier is a union of the domains of the original Pitch "
	"and PointProcess functions.")
MAN_END

MAN_BEGIN (L"PitchEditor", L"ppgb", 20110808)
INTRO (L"One of the @Editors in Praat, for viewing and modifying a @Pitch object.")
ENTRY (L"What the Pitch editor shows")
NORMAL (L"In the window of the PitchEditor, you will see the following features:")
LIST_ITEM (L"\\bu Digits between 0 and 9 scattered all over the drawing area. Their locations represent "
	"the pitch %#candidates, of which there are several for every time frame. The digits themselves "
	"represent the goodness of a candidate, multiplied by ten. For instance, if you see a \"9\" "
	"at the location (1.23 seconds, 189 hertz), this means that in the time frame at 1.23 seconds, "
	"there is a pitch candidate with a value of 189 hertz, and its goodness is 0.9. "
	"The number 0.9 may be the relative height of an autocorrelation peak, a cross-correlation peak, "
	"or a spectral peak, depending on the method by which the Pitch object was computed.")
LIST_ITEM (L"\\bu A %#path of red disks. These disks represent the best path through the candidates, "
	"i.e. our best guess at what the pitch contour is. The path will usually have been determined "
	"by the %%path finder%, which was called by the pitch-extraction algorithm, and you can change "
	"the path manually. The path finder takes into account the goodness of each candidate, "
	"the intensity of the sound in the frame, voiced-unvoiced transitions, and frequency jumps. "
	"It also determines whether each frame is voiced or unvoiced.")
LIST_ITEM (L"\\bu A %%##voicelessness bar%# at the bottom of the drawing area. If there is no suitable "
	"pitch candidate in a frame, the frame is considered voiceless, which is shown as a blue rectangle "
	"in the voicelessness bar.")
LIST_ITEM (L"\\bu A line of digits between 0 and 9 along the top. These represent the relative intensity "
	"of the sound in each frame.")
ENTRY (L"Moving the marks")
NORMAL (L"To move the cursor hair or the beginning or end of the selection, "
	"use the @@time selection@ mechanism.")
ENTRY (L"Changing the path")
NORMAL (L"To change the path through the candidates manually, click on the candidates of your choice. "
	"The changes will immediately affect the Pitch object that you are editing. To make a voiced frame "
	"voiceless, click on the voicelessness bar.")
NORMAL (L"To change the path automatically, choose ##Path finder...# from the #Edit menu; "
	"this will have the same effect as filling in different values in the @@Sound: To Pitch (ac)...@ command window, "
	"but is much faster because the candidates do not have to be determined again.")
ENTRY (L"Resynthesis")
NORMAL (L"To hum any part of the pitch contour, click on one of the buttons "
	"below or above the data area (there can be 1 to 8 of these buttons), or use a #Play command from the #View menu.")
ENTRY (L"Changing the ceiling")
NORMAL (L"To change the ceiling, but not the path, choose ##Change ceiling...# from the #Edit menu; "
	"if the new ceiling is lower than the old ceiling, some formerly voiced frames may become unvoiced; "
	"if the new ceiling is higher than the old ceiling, some formerly unvoiced frames may become voiced.")
MAN_END

MAN_BEGIN (L"PitchTier", L"ppgb", 20101230)
INTRO (L"One of the @@types of objects@ in Praat. "
	"A PitchTier object represents a time-stamped pitch contour, "
	"i.e. it contains a number of (%time, %pitch) points, without voiced/unvoiced information. "
	"For instance, if your PitchTier contains two points, namely 150 Hz at a time of 0.5 seconds and "
	"200 Hz at a time of 1.5 seconds, then this is to be interpreted as a pitch contour that "
	"is constant at 150 Hz for all times before 0.5 seconds, constant at 200 Hz for all times after 1.5 seconds, "
	"and linearly interpolated for all times between 0.5 and 1.5 seconds (i.e. 170 Hz at 0.7 seconds, "
	"210 Hz at 1.1 seconds, and so on).")
NORMAL (L"PitchTier objects are used for two purposes: "
	"for manipulating the pitch curve of an existing sound "
	"(see @@Intro 8.1. Manipulation of pitch@) "
	"and for synthesizing a new sound "
	"(see @@Source-filter synthesis 1. Creating a source from pitch targets@, "
	"and for an example @@Source-filter synthesis 3. The ba-da continuum@).")
ENTRY (L"PitchTier commands")
NORMAL (L"Creation:")
LIST_ITEM (L"From scratch:")
LIST_ITEM (L"\\bu @@Create PitchTier...")
LIST_ITEM (L"\\bu @@PitchTier: Add point...")
LIST_ITEM (L"Copy from another object:")
LIST_ITEM (L"\\bu @@Pitch: To PitchTier@: trivial copying of voiced frames.")
LIST_ITEM (L"\\bu @@PointProcess: Up to PitchTier...@: single value at specified times.")
LIST_ITEM (L"\\bu @@Pitch & PointProcess: To PitchTier...@: copying interpolated values at specified points.")
LIST_ITEM (L"Synthesize from another object:")
LIST_ITEM (L"\\bu @@PointProcess: To PitchTier...@: periodicity analysis.")
LIST_ITEM (L"Extract from a @Manipulation object:")
LIST_ITEM (L"\\bu @@Manipulation: Extract pitch tier@")
NORMAL (L"Viewing and editing:")
LIST_ITEM (L"\\bu @PitchTierEditor: with or without a Sound.")
LIST_ITEM (L"\\bu @ManipulationEditor")
NORMAL (L"Conversion:")
LIST_ITEM (L"\\bu @@PitchTier: Down to PointProcess@: copy the times.")
NORMAL (L"Synthesis:")
LIST_ITEM (L"\\bu @@PitchTier: To PointProcess@: area-1 pulse generation (used in @@overlap-add@).")
LIST_ITEM (L"\\bu @@Manipulation: Replace pitch tier@")
NORMAL (L"Queries:")
LIST_ITEM (L"\\bu @@time domain")
LIST_ITEM (L"\\bu @@Get low index from time...")
LIST_ITEM (L"\\bu @@Get high index from time...")
LIST_ITEM (L"\\bu @@Get nearest index from time...")
LIST_ITEM (L"\\bu @@PitchTier: Get mean (curve)...")
LIST_ITEM (L"\\bu @@PitchTier: Get mean (points)...")
LIST_ITEM (L"\\bu @@PitchTier: Get standard deviation (curve)...")
LIST_ITEM (L"\\bu @@PitchTier: Get standard deviation (points)...")
LIST_ITEM (L"\\bu @@Get area...@: the number of periods")
NORMAL (L"Modification:")
LIST_ITEM (L"\\bu @@Remove point...")
LIST_ITEM (L"\\bu @@Remove point near...")
LIST_ITEM (L"\\bu @@Remove points between...")
LIST_ITEM (L"\\bu @@PitchTier: Add point...")
LIST_ITEM (L"\\bu @@PitchTier: Stylize...")
MAN_END

MAN_BEGIN (L"PitchTier: Add point...", L"ppgb", 20010410)
INTRO (L"A command to add a point to each selected @PitchTier.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time at which a point is to be added.")
TAG (L"##Pitch (Hz)")
DEFINITION (L"the pitch value of the requested new point.")
ENTRY (L"Behaviour")
NORMAL (L"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (L"PitchTier: Down to PointProcess", L"ppgb", 20010410)
INTRO (L"A command to degrade every selected @PitchTier to a @PointProcess.")
ENTRY (L"Behaviour")
NORMAL (L"The times of all the pitch points are trivially copied, and so is the time domain. The pitch information is lost.")
MAN_END

MAN_BEGIN (L"PitchTier: Get mean (curve)...", L"ppgb", 20010821)
INTRO (L"A @query to the selected @PitchTier object.")
ENTRY (L"Return value")
NORMAL (L"the mean of the curve within a specified time window.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the time window, in seconds. Values outside this window are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
ENTRY (L"Algorithm")
NORMAL (L"The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the area under the curve is")
FORMULA (L"1/2 (%f__1_ + %f__2_) (%t__2_ \\-- %t__1_)")
NORMAL (L"The mean is the sum of these values divided by %toTime \\-- %fromTime.")
NORMAL (L"For a PitchTier that was created from a @Pitch object, this command gives the same result as "
	"##Get mean....# for the original Pitch object (but remember that the median, "
	"as available for Pitch objects, is more robust).")
NORMAL (L"To get the mean in the entire curve, i.e. weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get mean (points)...@ instead.")
MAN_END

MAN_BEGIN (L"PitchTier: Get mean (points)...", L"ppgb", 20010821)
INTRO (L"A @query to the selected @PitchTier object.")
ENTRY (L"Return value")
NORMAL (L"the mean of the points within a specified time window.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the time window, in seconds. Values outside this window are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
NORMAL (L"To get the mean in the entire curve, i.e. weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get mean (curve)...@ instead.")
MAN_END

MAN_BEGIN (L"PitchTier: Get standard deviation (curve)...", L"ppgb", 20010821)
INTRO (L"A @query to the selected @PitchTier object.")
ENTRY (L"Return value")
NORMAL (L"the standard deviation in the curve within a specified time window.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
ENTRY (L"Algorithm")
NORMAL (L"The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the variance-multiplied-by-time is")
FORMULA (L"[ 1/4 (%f__1_ + %f__2_)^2 + 1/12 (%f__1_ \\-- %f__2_)^2 ] (%t__2_ \\-- %t__1_)")
NORMAL (L"The standard deviation is the square root of: the sum of these values divided by %toTime \\-- %fromTime.")
NORMAL (L"To get the standard deviation in the points only, i.e. not weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get standard deviation (points)...@ instead.")
MAN_END

MAN_BEGIN (L"PitchTier: Get standard deviation (points)...", L"ppgb", 20010821)
INTRO (L"A @query to the selected @PitchTier object.")
ENTRY (L"Return value")
NORMAL (L"the standard deviation in the points within a specified time window.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
NORMAL (L"For a PitchTier that was created from a @Pitch object, this command gives the same result as "
	"##Get standard deviation....# for the original Pitch object (but remember that variation measures "
	"based on quantiles, as available for Pitch objects, are more robust).")
NORMAL (L"To get the standard deviation in the entire curve, i.e. weighted by the durations of the line pieces, "
	"Use @@PitchTier: Get standard deviation (curve)...@ instead.")
MAN_END

MAN_BEGIN (L"PitchTier: Stylize...", L"ppgb", 20050831)
INTRO (L"A command that modifies the selected @PitchTier object "
	"(or the pitch curve in the @ManipulationEditor).")
ENTRY (L"Purpose")
NORMAL (L"to end up with a much simplified pitch curve.")
ENTRY (L"Settings")
TAG (L"##Frequency resolution# (standard: 2.0 semitones)")
DEFINITION (L"the minimum amount by which every remaining pitch point will lie "
	"above or below the line that connects its two neigbours.")
ENTRY (L"Algorithm")
LIST_ITEM (L"1. Look up the pitch point that is closest to the straight line "
	"that connects its two neighbouring points.")
LIST_ITEM (L"2. If this pitch point is further away from that straight line "
	"than ##Frequency resolution#, we are finished: the curve cannot be stylized any further.")
LIST_ITEM (L"3. If we are not finished, the pitch point we found in step 1 is removed.")
LIST_ITEM (L"4. Go back to step 1.")
MAN_END

MAN_BEGIN (L"PitchTier: To PointProcess", L"ppgb", 19960915)
INTRO (L"A command that uses a @PitchTier object to generate a @PointProcess.")
ENTRY (L"Purpose")
NORMAL (L"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (L"Algorithm")
NORMAL (L"Points are generated along the entire time domain of the #PitchTier, "
	"because there is no voiced/unvoiced information. The area between two adjacent points "
	"under the linearly interpolated pitch contour, is always 1.")
MAN_END

MAN_BEGIN (L"PitchTierEditor", L"ppgb", 20110128)
INTRO (L"One of the @Editors in Praat, for viewing and manipulating a @PitchTier object, "
	"which is optionally shown together with a @Sound object.")
ENTRY (L"Objects")
NORMAL (L"The editor shows:")
LIST_ITEM (L"\\bu The @Sound, if you selected a Sound object together with the PitchTier object "
	"before you clicked ##View & Edit#.")
LIST_ITEM (L"\\bu The @PitchTier: blue points connected with blue lines.")
ENTRY (L"Playing")
NORMAL (L"To play (a part of) the %resynthesized sound: "
	"@click on any of the 8 buttons below and above the drawing area, or choose a Play command from the View menu.")
NORMAL (L"To play the %original sound instead, use @@Shift-click@.")
ENTRY (L"Adding a point")
NORMAL (L"@Click at the desired time location, and choose ##Add point at cursor# or type ##Command-P#.")
ENTRY (L"Removing points")
NORMAL (L"To remove one or more pitch points, "
	"make a @@time selection@ and choose ##Remove point(s)# from the ##Point# menu. "
	"If there is no selection, the point nearest to the cursor is removed.")
MAN_END

MAN_BEGIN (L"PointProcess: To PitchTier...", L"ppgb", 19970402)
INTRO (L"A command to compute a @PitchTier from a @PointProcess.")
ENTRY (L"Setting")
TAG (L"##Maximum interval (s)")
DEFINITION (L"the maximum duration of a period; intervals longer than this are considered voiceless.")
ENTRY (L"Algorithm")
NORMAL (L"A pitch point is constructed between each consecutive pair of points in the #PointProcess, "
	"if these are more than %maximumInterval apart. "
	"The associated pitch value will be the inverse of the duration of the interval between the two points.")
MAN_END

MAN_BEGIN (L"Sound: To Harmonicity (ac)...", L"ppgb", 20030916)
INTRO (L"A command that creates a @Harmonicity object from every selected @Sound object.")
ENTRY (L"Purpose")
NORMAL (L"to perform a short-term HNR analysis.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm performs an acoustic periodicity detection on the basis of an "
	"accurate autocorrelation method, as described in @@Boersma (1993)@.")
ENTRY (L"Settings")
TAG (L"%%Time step% (standard value: 0.01 s)")
DEFINITION (L"the measurement interval (frame duration), in seconds.")
TAG (L"%%Minimum pitch% (standard value: 75 Hz)")
DEFINITION (L"determines the length of the analysis window.")
TAG (L"%%Silence threshold% (standard value: 0.1)")
DEFINITION (L"frames that do not contain amplitudes above this threshold (relative to the global maximum amplitude), "
	"are considered silent.")
TAG (L"%%Number of periods per window% (standard value: 4.5)")
DEFINITION (L"4.5 is best for speech: "
	"HNR values up to 37 dB are guaranteed to be detected reliably; "
	"6 periods per window raises this figure to more than 60 dB, "
	"but the algorithm becomes more sensitive to dynamic changes in the signal.")
ENTRY (L"Usage")
NORMAL (L"You will normally use @@Sound: To Harmonicity (cc)...@ instead of this command, "
	"because that has a much better time resolution (though its sensitivity is 60, not 80 dB).")
MAN_END

MAN_BEGIN (L"Sound: To Harmonicity (cc)...", L"ppgb", 19961003)
INTRO (L"A command that creates a @Harmonicity object from every selected @Sound object.")
ENTRY (L"Purpose")
NORMAL (L"to perform a short-term HNR analysis.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm performs an acoustic periodicity detection on the basis of a "
	"forward cross-correlation analysis. "
	"For information on the settings, see @@Sound: To Harmonicity (ac)...@.")
MAN_END

MAN_BEGIN (L"Sound: To Pitch...", L"ppgb", 20030916)
INTRO (L"A command that creates a @Pitch object from every selected @Sound object.")
ENTRY (L"Purpose")
NORMAL (L"to perform a pitch analysis, optimized for speech.")
ENTRY (L"Settings")
NORMAL (L"The settings that control the recruitment of the candidates are:")
TAG (L"##Time step (s)# (standard value: 0.0)")
DEFINITION (L"the measurement interval (frame duration), in seconds. If you supply 0, "
	"Praat will use a time step of 0.75 / (%%pitch floor%), e.g. 0.01 seconds if the pitch floor is 75 Hz; "
	"in this example, Praat computes 100 pitch values per second.")
TAG (L"##Pitch floor (Hz)# (standard value: 75 Hz)")
DEFINITION (L"candidates below this frequency will not be recruited. "
	"This parameter determines the length of the analysis window: it will be 3 longest periods long, "
	"i.e., if the pitch floor is 75 Hz, the window will be 3/75 = 0.04 seconds long.")
NORMAL (L"Note that if you set the time step to zero, the analysis windows for consecutive measurements "
	"will overlap appreciably: Praat will always compute 4 pitch values "
	"within one window length, i.e., the degree of %oversampling is 4.")
NORMAL (L"A post-processing algorithm seeks the cheapest path through the candidates. "
	"The argument that determines the cheapest path is:")
TAG (L"##Pitch ceiling (Hz)# (standard value: 600 Hz)")
DEFINITION (L"candidates above this frequency will be ignored.")
ENTRY (L"Algorithm")
NORMAL (L"This is the algorithm described at @@Sound: To Pitch (ac)...@, "
	"with all the parameters not mentioned above set to their standard values.")
MAN_END

MAN_BEGIN (L"Sound: To Pitch (ac)...", L"ppgb", 20030916)
INTRO (L"A command that creates a @Pitch object from every selected @Sound object.")
ENTRY (L"Purpose")
NORMAL (L"to perform a pitch analysis based on an autocorrelation method.")
ENTRY (L"Usage")
NORMAL (L"Normally, you will instead use @@Sound: To Pitch...@, which uses the same method. "
	"The command described here is mainly for experimenting with the parameters, "
	"or for the analysis of non-speech signals, which may require different "
	"standard settings of the parameters.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm performs an acoustic periodicity detection on the basis of an "
	"accurate autocorrelation method, as described in @@Boersma (1993)@. "
	"This method is more accurate, noise-resistant, and robust, than methods based on cepstrum or combs, "
	"or the original autocorrelation methods. The reason why other methods were invented, "
	"was the failure to recognize the fact that if you want to estimate a signal's short-term autocorrelation "
	"function on the basis of a windowed signal, you should divide the autocorrelation function of the "
	"windowed signal by the autocorrelation function of the window:")
NORMAL (L"%r__%x_ (%\\ta) \\~~ %r__%xw_ (%\\ta) / %r__%w_ (%\\ta)")
ENTRY (L"Settings")
NORMAL (L"The settings that control the recruitment of the candidates are:")
TAG (L"##Time step (s)# (standard value: 0.0)")
DEFINITION (L"the measurement interval (frame duration), in seconds. If you supply 0, "
	"Praat will use a time step of 0.75 / (%%pitch floor%), e.g. 0.01 seconds if the pitch floor is 75 Hz; "
	"in this example, Praat computes 100 pitch values per second.")
TAG (L"##Pitch floor (Hz)# (standard value: 75 Hz)")
DEFINITION (L"candidates below this frequency will not be recruited. "
	"This parameter determines the effective length of the analysis window: it will be 3 longest periods long, "
	"i.e., if the pitch floor is 75 Hz, the window will be effectively 3/75 = 0.04 seconds long.")
NORMAL (L"Note that if you set the time step to zero, the analysis windows for consecutive measurements "
	"will overlap appreciably: Praat will always compute 4 pitch values "
	"within one window length, i.e., the degree of %oversampling is 4.")
TAG (L"##Very accurate# (standard value: %off)")
DEFINITION (L"if %off, the window is a Hanning window with a physical length of  3 / (%%pitch floor%). "
	"If %on, the window is a Gaussian window with a physical length of  6 / (%%pitch floor%), "
	"i.e. twice the effective length.")
NORMAL (L"A post-processing algorithm seeks the cheapest path through the candidates. "
	"The settings that determine the cheapest path are:")
TAG (L"##Pitch ceiling (Hz)# (standard value: 600 Hz)")
DEFINITION (L"candidates above this frequency will be ignored.")
TAG (L"##Silence threshold# (standard value: 0.03)")
DEFINITION (L"frames that do not contain amplitudes above this threshold (relative to the global maximum amplitude), "
	"are probably silent.")
TAG (L"##Voicing threshold# (standard value: 0.45)")
DEFINITION (L"the strength of the unvoiced candidate, relative to the maximum possible autocorrelation. "
	"To increase the number of unvoiced decisions, increase this value.")
TAG (L"##Octave cost# (standard value: 0.01 per octave)")
DEFINITION (L"degree of favouring of high-frequency candidates, relative to the maximum possible autocorrelation. "
	"This is necessary because even (or: especially) in the case of a perfectly periodic signal, "
	"all undertones of %F__0_ are equally strong candidates as %F__0_ itself. "
	"To more strongly favour recruitment of high-frequency candidates, increase this value.")
TAG (L"##Octave-jump cost# (standard value: 0.35)")
DEFINITION (L"degree of disfavouring of pitch changes, relative to the maximum possible autocorrelation. "
	"To decrease the number of large frequency jumps, increase this value. In contrast with what is described "
	"in the article, this value will be corrected for the time step: multiply by 0.01 s / %TimeStep to get "
	"the value in the way it is used in the formulas in the article.")
TAG (L"##Voiced / unvoiced cost# (standard value: 0.14)")
DEFINITION (L"degree of disfavouring of voiced/unvoiced transitions, relative to the maximum possible autocorrelation. "
	"To decrease the number of voiced/unvoiced transitions, increase this value. In contrast with what is described "
	"in the article, this value will be corrected for the time step: multiply by 0.01 s / %TimeStep to get "
	"the value in the way it is used in the formulas in the article.")
MAN_END

MAN_BEGIN (L"Sound: To Pitch (cc)...", L"ppgb", 20030916)
INTRO (L"A command that creates a @Pitch object from every selected @Sound object.")
ENTRY (L"Purpose")
NORMAL (L"to perform a pitch analysis based on a cross-correlation method.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm performs an acoustic periodicity detection on the basis of a "
	"forward cross-correlation analysis.")
ENTRY (L"Settings")
TAG (L"##Time step (s)# (standard value: 0.0)")
DEFINITION (L"the measurement interval (frame duration), in seconds. If you supply 0, "
	"Praat will use a time step of 0.25 / (%%pitch floor%), e.g. 0.00333333 seconds if the pitch floor is 75 Hz; "
	"in this example, Praat computes 300 pitch values per second.")
TAG (L"##Pitch floor (Hz)# (standard value: 75 Hz)")
DEFINITION (L"candidates below this frequency will not be recruited. "
	"This parameter determines the length of the analysis window: it will be 1 longest period long, "
	"i.e., if the pitch floor is 75 Hz, the window will be 1/75 = 0.01333333 seconds long.")
NORMAL (L"Note that if you set the time step to zero, the analysis windows for consecutive measurements "
	"will overlap appreciably: Praat will always compute 4 pitch values "
	"within one window length, i.e., the degree of %oversampling is 4.")
NORMAL (L"The other settings are the same as for @@Sound: To Pitch (ac)...@.")
ENTRY (L"Usage")
NORMAL (L"The preferred method for speech is @@Sound: To Pitch...@. "
	"The command described here is mainly for experimenting, "
	"or for applications where you need short time windows.")
MAN_END

MAN_BEGIN (L"Sound: To PointProcess (periodic, cc)...", L"ppgb", 20030309)
INTRO (L"A command that analyses the selected @Sound objects, and creates @PointProcess objects.")
NORMAL (L"This command combines the actions of @@Sound: To Pitch (ac)...@ and @@Sound & Pitch: To PointProcess (cc)@.")
MAN_END

MAN_BEGIN (L"Sound: To PointProcess (periodic, peaks)...", L"ppgb", 20030309)
INTRO (L"A command that analyses the selected @Sound objects, and creates @PointProcess objects.")
NORMAL (L"This command combines the actions of @@Sound: To Pitch (ac)...@ and @@Sound & Pitch: To PointProcess (peaks)...@.")
MAN_END

MAN_BEGIN (L"Sound & Pitch: To PointProcess (cc)", L"ppgb", 19980322)
INTRO (L"A command to create a @PointProcess from the selected @Sound and @Pitch objects.")
ENTRY (L"Purpose")
NORMAL (L"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (L"Algorithm")
NORMAL (L"The voiced intervals are determined on the basis of the voiced/unvoiced decisions in the @Pitch object. "
	"For every voiced interval, a number of %points (or glottal pulses) is found as follows:")
LIST_ITEM (L"1. The first point %t__1_ is the absolute extremum of the amplitude of the #Sound, "
	"between %t__%mid_ \\-- %T__0_ / 2 and %t__%mid_ + %T__0_ / 2, where %t__%mid_ is the midpoint of the interval, "
	"and %T__0_ is the period at %t__%mid_, as can be interpolated from the #Pitch contour.")
LIST_ITEM (L"2. From this point, we recursively search for points %t__%i_ to the left "
	"until we reach the left edge of the interval. These points must be located between "
	"%t__%i\\--1_ \\-- 1.2 %T__0_(%t__%i\\--1_) and %t__%i\\--1_ \\-- 0.8 %T__0_(%t__%i\\--1_), "
	"and the cross-correlation of the amplitude "
	"in its environment [%t__%i_ \\-- %T__0_(%t__%i_) / 2; %t__%i_ + %T__0_(%t__%i_) / 2] "
	"with the amplitude of the environment of the existing point %t__%i\\--1_ must be maximal "
	"(we use parabolic interpolation between samples of the correlation function).")
LIST_ITEM (L"3. The same is done to the right of %t__1_.")
LIST_ITEM (L"4. Though the voiced/unvoiced decision is initially taken by the #Pitch contour, "
	"points are removed if their correlation value is less than 0.3; "
	"furthermore, one extra point may be added at the edge of the voiced interval "
	"if its correlation value is greater than 0.7.")
MAN_END

MAN_BEGIN (L"Sound & Pitch: To PointProcess (peaks)...", L"ppgb", 20070722)
INTRO (L"A command to create a @PointProcess from the selected @Sound and @Pitch objects.")
ENTRY (L"Purpose")
NORMAL (L"to interpret an acoustic periodicity contour as the frequency of an underlying point process "
	"(such as the sequence of glottal closures in vocal-fold vibration).")
ENTRY (L"Algorithm")
NORMAL (L"The voiced intervals are determined on the basis of the voiced/unvoiced decisions in the @Pitch object. "
	"For every voiced interval, a number of %points (or glottal pulses) is found as follows:")
LIST_ITEM (L"1. The first point %t__1_ is the absolute extremum (or the maximum, or the minimum, depending on "
	"your %%Include maxima% and %%Include minima% settings) of the amplitude of the #Sound, "
	"between %t__%mid_ \\-- %T__0_ / 2 and %t__%mid_ + %T__0_ / 2, where %t__%mid_ is the midpoint of the interval, "
	"and %T__0_ is the period at %t__%mid_, as can be interpolated from the #Pitch contour.")
LIST_ITEM (L"2. From this point, we recursively search for points %t__%i_ to the left "
	"until we reach the left edge of the interval. These points are the absolute extrema (or the maxima, or the minima) "
	"between the times "
	"%t__%i\\--1_ \\-- 1.2 %T__0_(%t__%i\\--1_) and %t__%i\\--1_ \\-- 0.8 %T__0_(%t__%i\\--1_).")
LIST_ITEM (L"3. The same is done to the right of %t__1_.")
NORMAL (L"The periods that are found in this way are much more variable than those found by "
	"@@Sound & Pitch: To PointProcess (cc)@, and therefore less useful for analysis and subsequent @@overlap-add@ synthesis.")
MAN_END

}

/* End of file manual_pitch.cpp */
