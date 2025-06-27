/* manual_pointProcess.cpp
 *
 * Copyright (C) 1992-2008,2010,2011,2014-2017,2019-2023,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ManPagesM.h"

void manual_pointProcess_init (ManPages me);
void manual_pointProcess_init (ManPages me) {

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"Create empty PointProcess..."
© Paul Boersma 2002-12-04

A command in the @@New menu@ to create an empty @PointProcess. The newly created object is put in the list of objects.

################################################################################
"Create Poisson process..."
© Paul Boersma 2004-10-05, 2023-05-31

A command to create a @PointProcess object that represents a Poisson process.
A Poisson process is a stationary point process with a fixed density %λ,
which means that there are, on the average, %λ events per second.

Settings
========

##Start time (s)
:	%t__%min_, the beginning of the time domain, in seconds.

##End time (s)
:	%t__%max_, the end of the time domain, in seconds.

##Density (Hz)
:	the average number of points per second (%\la).

Algorithm
=========

First, the number of points %N in the time domain is determined. Its expectation value is
~	%Λ = (%t__%max_ – %t__%min_) · %density

but its actual value is taken from the Poisson distribution:
~	%P(\# points=%N) = (%Λ^%N / %N!) %e^^–%Λ

Then, %N points are computed throughout the time domain, according to a uniform distribution:
~	%p(%t) = 1 / (%t__%max_ – %t__%min_)   for %t ∈ [%t__%min_, %t__%max_]
~	%p(%t) = 0   outside [%t__%min_, %t__%max_]

Example
=======

Suppose that it rains between %t__%min_ = 2.0 seconds and %t__%max_ = 5.0 seconds,
with an average of 4.0 drops per second expected to fall on my head.
To simulate this process, you can click (or script) the following commands:
{
	\#{Create Poisson process:} "rain", 2.0, 5.0, 4.0
	\@{PointProcess: ||Draw:} 0.0, 6.0, "no"
	\@{Draw inner box}
	\@{Marks bottom every:} 1, 1, "yes", "yes", "no"
	\@{Text bottom:} "yes", "Time (s)"
	\@{Text top:} "no", "##Three seconds of rain"
}
When you refresh this picture, e.g. by clicking on the “> 1” button and then on the “1 <” button,
you will see that the points lie at different time points each time.
This variation is due to the %stochasticity of the Poisson process:
the points occur at random places.

Also, the %number of points varies: on average, there will be 12 points,
but there can just as easily be 10 or 15 points.
As the rain shower lasts 3.0 seconds, the expected total number of drops on my head
is %λ = 3.0 seconds · 4.0 drops/second = 12.0 drops, but the actual number
of points is just as stochastic as their locations in time are.

Fast implementation
===================

One can simulate the number of points and their times as follows.
To show that our fast implementation does exactly the same as ##Create Poisson process...# does,
we first make sure that the points lie at reproducible time points:
{
	\#`{random_initializeWithSeedUnsafelyButPredictably} (1234567654321)
	\#{Create Poisson process:} "rain", 2.0, 5.0, 4.0
	\@{PointProcess: ||Draw:} 0.0, 6.0, "yes"
	\@{Text top:} "no", "##Three reproducible seconds of rain"
	\#`{random_initializeSafelyAndUnpredictably} ()
}
These are only 8 points, and their times will not change when you click “> 1” followed by “1 <”,
because Praat’s random generator is initialized to a fixed state,
determined by the arbitrary number 1234567654321 (you can use any number you like,
with different results depending on that number) in the first line of the script.

To replicate how these 8 numbers were created, we first replicate their count:
{
	\`{random_initializeWithSeedUnsafelyButPredictably} (1234567654321)
	n = \#`{randomPoisson} (12.0)
	\`{writeInfoLine}: n
}
We then replicate the actual times according to the algorithm above:
{
	times# = \#`{randomUniform#} (n, 2.0, 5.0)
	\`{writeInfoLine}: times#
}
In a PointProcess, these 8 points will be in sorted order:
{
	times# = \#`{sort#} (times#)
	\`{writeInfoLine}: times#
}
Here you can see, as in the picture, that the interval between the 4th and 5th point
is the largest, and the interval between the 5th and 6th point is the smallest.

We can add these eight points at one stroke to an empty PointProcess:
{
	\#@{Create empty PointProcess:} "rain3", 2.0, 5.0
	\#@{PointProcess: ||Add points:} times#
	\@{PointProcess: ||Draw:} 0.0, 6.0, "yes"
	\@{Text top:} "no", "##Three reproducible seconds of rain,
	... fast implementation"
}

Slow implementation
===================

We could also have generated the eight points one by one,
and added them immediately to an empty PointProcess:
{
	\`{random_initializeWithSeedUnsafelyButPredictably} (1234567654321)
	n = \`{randomPoisson} (12.0)
	\@{Create empty PointProcess:} "rain4", 2.0, 5.0
	for i to n
		time = \#`{randomUniform} (2.0, 5.0)
		\#@{PointProcess: ||Add point:} time
	endfor
	\`{random_initializeSafelyAndUnpredictably} ()
	\@{PointProcess: ||Draw:} 0.0, 6.0, "yes"
	\@{Text top:} "no", "##Three reproducible seconds of rain,
	... slow implementation"
}
This is slower than the fast implementation, because of two causes:
(1) the for-loop shown in the Praat script above
is slower than the for-loop in the C++ implementation of ##Add points...#.
(2) every call to ##Add point...# causes the new point to be inserted
into an existing array of points, so the complexity of the whole procedure
is %O(%N^2) (as the complexity of insertion is %O(%N)), while ##Add points...#
adds all new points to the end of the existing array (%O(%N · log %N)),
then sorts the new array just once, which is again %O(%N · log %N).

################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"PointEditor", U"ppgb", /*20110128 20220814*/ 20230608)
INTRO (U"One of the @Editors in Praat, for viewing and manipulating a @PointProcess object, "
	"which is optionally shown together with a @Sound object.")
ENTRY (U"Objects")
NORMAL (U"The editor shows:")
LIST_ITEM (U"• The @Sound, if you selected a Sound object together with the PointProcess object "
	"before you clicked ##View & Edit#.")
LIST_ITEM (U"• The @PointProcess; vertical blue lines represent the points.")
ENTRY (U"Playing")
NORMAL (U"To play (a part of) the %resynthesized sound (pulse train): "
	"@click any of the 8 buttons below and above the drawing area, or choose a command from the Play menu.")
NORMAL (U"To play the %original sound instead, use @@Shift-click@.")
ENTRY (U"Adding a point")
NORMAL (U"@Click at the desired time location, and choose \"Add point at cursor\" or type @@Keyboard shortcuts|Command-P@.")
ENTRY (U"Removing points")
NORMAL (U"To remove one or more points, "
	"make a @@time selection@ and choose ##Remove point(s)# from the ##Point# menu. "
	"If there is no selection, the point nearest to the cursor is removed.")
MAN_END

MAN_BEGIN (U"PointProcess", U"ppgb", 20110128)   // 20230531
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A PointProcess object represents a %%point process%, "
	"which is a sequence of %points %t__%i_ in time, defined on a domain [%t__%min_, %t__%max_]. "
	"The index %i runs from 1 to the number of points. The points are sorted by time, i.e. %t__%i+1_ > %t__%i_.")
ENTRY (U"PointProcess commands")
NORMAL (U"Creation from scratch:")
LIST_ITEM (U"• @@Create empty PointProcess...@")
LIST_ITEM (U"• @@Create Poisson process...@")
NORMAL (U"Creation of a pulse train from a pitch contour:")
LIST_ITEM (U"• @@PitchTier: To PointProcess@: area-1 along entire time domain.")
LIST_ITEM (U"• @@Pitch: To PointProcess@: same, but excludes voiceless intervals.")
LIST_ITEM (U"• @@Sound & Pitch: To PointProcess (cc)@: \"pitch-synchronous\": near locations of high amplitude.")
LIST_ITEM (U"• @@Sound & Pitch: To PointProcess (peaks)...@: \"pitch-synchronous\": near locations of high amplitude.")
LIST_ITEM (U"• @@Sound: To PointProcess (periodic, cc)...@: near locations of high amplitude.")
LIST_ITEM (U"• @@Sound: To PointProcess (periodic, peaks)...@: near locations of high amplitude.")
NORMAL (U"Creation from converting another object:")
LIST_ITEM (U"• ##Matrix: To PointProcess")
LIST_ITEM (U"• @@PitchTier: Down to PointProcess@")
LIST_ITEM (U"• @@IntensityTier: Down to PointProcess@")
NORMAL (U"Hearing:")
LIST_ITEM (U"• @@PointProcess: Play@: pulse train.")
LIST_ITEM (U"• @@PointProcess: Hum@: pulse train with formants.")
NORMAL (U"Drawing:")
LIST_ITEM (U"• @@PointProcess: Draw...@")
NORMAL (U"Editing:")
LIST_ITEM (U"• ##PointProcess: View & Edit#: invokes a @PointEditor.")
LIST_ITEM (U"• ##PointProcess & Sound: View & Edit#: invokes a @PointEditor.")
LIST_ITEM (U"• Inside a @ManipulationEditor.")
NORMAL (U"Queries:")
LIST_ITEM (U"• @@PointProcess: Get jitter (local)...@: periodic jitter.")
LIST_ITEM (U"• @@PointProcess: Get jitter (local, absolute)...@: periodic jitter.")
LIST_ITEM (U"• @@PointProcess: Get jitter (rap)...@: periodic jitter.")
LIST_ITEM (U"• @@PointProcess: Get jitter (ppq5)...@: periodic jitter.")
LIST_ITEM (U"• @@PointProcess: Get jitter (ddp)...@: periodic jitter.")
LIST_ITEM (U"• @@PointProcess: Get low index...@: index of nearest point not after specified time.")
LIST_ITEM (U"• @@PointProcess: Get high index...@: index of nearest point not before specified time.")
LIST_ITEM (U"• @@PointProcess: Get nearest index...@: index of point nearest to specified time.")
LIST_ITEM (U"• @@PointProcess: Get interval...@: duration of interval around specified time.")
NORMAL (U"Set calculations:")
LIST_ITEM (U"• @@PointProcesses: Union@: the union of two point processes.")
LIST_ITEM (U"• @@PointProcesses: Intersection@: the intersection of two point processes.")
LIST_ITEM (U"• @@PointProcesses: Difference@: the difference of two point processes.")
NORMAL (U"Modification:")
LIST_ITEM (U"• @@PointProcess: Add point...@: at a specified time.")
LIST_ITEM (U"• @@PointProcess: Add points...@: at specified times.")
LIST_ITEM (U"• @@PointProcess: Remove point...@: at specified index.")
LIST_ITEM (U"• @@PointProcess: Remove point near...@: near specified time.")
LIST_ITEM (U"• @@PointProcess: Remove points...@: between specified indices.")
LIST_ITEM (U"• @@PointProcess: Remove points between...@: between specified times.")
NORMAL (U"Analysis:")
LIST_ITEM (U"• @@PointProcess: To PitchTier...@: pitch values in interval centres.")
LIST_ITEM (U"• ##PointProcess & Sound: To Manipulation")
NORMAL (U"Synthesis:")
LIST_ITEM (U"• @@PointProcess: To Sound (pulse train)...@")
LIST_ITEM (U"• @@PointProcess: To Sound (hum)...@")
NORMAL (U"Conversion:")
LIST_ITEM (U"• ##PointProcess: To Matrix")
LIST_ITEM (U"• @@PointProcess: Up to TextGrid...")
LIST_ITEM (U"• @@PointProcess: Up to PitchTier...")
LIST_ITEM (U"• @@PointProcess: Up to IntensityTier...")
MAN_END

MAN_BEGIN (U"PointProcess: Add point...", U"ppgb", 20010410)
INTRO (U"A command to add a point to each selected @PointProcess.")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time at which a point is to be added.")
ENTRY (U"Behaviour")
NORMAL (U"The point process is modified so that it contains the new point. "
	"If a point at the specified time was already present in the point process, nothing happens.")
MAN_END

MAN_BEGIN (U"PointProcess: Add points...", U"ppgb", 20230531)
INTRO (U"A command to add multiple points to each selected @PointProcess.")
ENTRY (U"Setting")
TERM (U"##Times (s)")
DEFINITION (U"the times at which points are to be added.")
ENTRY (U"Behaviour")
NORMAL (U"The point process is modified so that it contains the new points. "
	"If a point at any specified time was already present in the point process, that point is still added.")
MAN_END

MAN_BEGIN (U"PointProcesses: Difference", U"ppgb", 20021212)
INTRO (U"A command to compute the difference of two selected @PointProcess objects.")
ENTRY (U"Behaviour")
NORMAL (U"The resulting #PointProcess will contain only those points of the first selected original point process "
	"that do not occur in the second.")
NORMAL (U"The time domain of the resulting point process is equal to the time domain of the first original point process.")
MAN_END

MAN_BEGIN (U"PointProcess: Draw...", U"ppgb", 20021212)
INTRO (U"A command to draw every selected @PointProcess into the @@Picture window@.")
MAN_END

MAN_BEGIN (U"PointProcess: Get high index...", U"ppgb", 20021212)
INTRO (U"A @@Query submenu|query@ to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the index of the nearest point at or after the specified time, "
	"0 if the point process contains no points, "
	"or a number higher than the number of points if the specified time is after the last point.")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time from which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (U"PointProcess: Get interval...", U"ppgb", 20021212)
INTRO (U"A @@Query submenu|query@ to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the duration of the interval around a specified time. "
	"if the point process contains no points or if the specified time falls before the first point "
	"or not before the last point, the value is @undefined. Otherwise, the result is the distance between "
	"the nearest points to the left and to the right of the specified time. "
	"If the point process happens to contain a point at exactly the specified time, "
	"the duration of the interval following this point is returned.")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time around which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (local)...", U"ppgb", 20221202)
INTRO (U"A command that becomes available in the @@Query submenu@ when you select a @PointProcess object.")
NORMAL (U"This command will write into the Info window "
	"the %%local jitter%, which is the average absolute difference between consecutive intervals, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (U"As %jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (U"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (local)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TERM (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TERM (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TERM (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this setting to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TERM (U"##Maximum period factor")
DEFINITION (U"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (U"2. Usage")
NORMAL (U"The local jitter can be used as a measure of voice quality; "
	"it is the most common jitter measurement and is usually expressed as a percentage. See @@Voice 2. Jitter@.")
ENTRY (U"3. Algorithm")
NORMAL (U"(In the following the term %absolute means two different things: (1) the absolute (i.e. non-negative) value of a real number, "
	"and (2) the opposite of %relative.)")
NORMAL (U"The local jitter is defined as the relative mean absolute "
	"second-order difference of the point process (= the first-order difference of the interval process), as follows.")
NORMAL (U"First, we define the absolute (non-relative) local jitter (in seconds) as the mean absolute (non-negative) "
	"difference of consecutive intervals:")
EQUATION (U"%jitter(seconds) = ∑__%i=2_^^%N^ |%T__%i_ - %T__%i-1_| / (%N - 1)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - %T__%i-1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 2, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
EQUATION (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute local jitter.")
NORMAL (U"Finally, we compute the (relative) local jitter as")
EQUATION (U"%jitter = %jitter(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is a value between 0 and 2, or between 0 and 200 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (local, absolute)...", U"ppgb", 20221202)
INTRO (U"A command that becomes available in the @@Query submenu@ when you select a @PointProcess object.")
NORMAL (U"This command will write into the Info window "
	"the %%absolute local jitter%, which is the average absolute difference between consecutive intervals, "
	"in seconds (an interval is the time between two consecutive points).")
NORMAL (U"As %jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (U"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (local, absolute)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TERM (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TERM (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TERM (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TERM (U"##Maximum period factor")
DEFINITION (U"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (U"2. Usage")
NORMAL (U"The local jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
ENTRY (U"3. Algorithm")
NORMAL (U"The absolute local jitter is defined as the absolute (i.e. non-relative) mean absolute (i.e. non-negative) "
	"second-order difference of the point process (= the first-order difference of the interval process), as follows.")
NORMAL (U"The absolute local jitter (in seconds) is the mean absolute (non-negative) "
	"difference of consecutive intervals:")
EQUATION (U"%jitter(seconds) = ∑__%i=2_^^%N^ |%T__%i_ - %T__%i-1_| / (%N - 1)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - %T__%i-1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 2, the result of the command is @undefined).")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (rap)...", U"ppgb", 20221202)
INTRO (U"A command that becomes available in the @@Query submenu@ when you select a @PointProcess object.")
NORMAL (U"This command will write into the Info window the %%Relative Average Perturbation% (RAP), "
	"a jitter measure defined as the average absolute difference between an interval and the average of it and its two neighbours, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (U"As jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (U"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (rap)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TERM (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TERM (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TERM (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TERM (U"##Maximum period factor")
DEFINITION (U"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (U"2. Usage")
NORMAL (U"The RAP can be used as a measure of voice quality; "
	"it is the second most common jitter measurement (after @@PointProcess: Get jitter (local)...|local jitter@). See @@Voice 2. Jitter@.")
ENTRY (U"3. Algorithm")
NORMAL (U"Relative Average Perturbation is defined in terms of three consecutive intervals, as follows.")
NORMAL (U"First, we define the absolute (i.e. non-relative) Average Perturbation (in seconds):")
EQUATION (U"%absAP(seconds) = ∑__%i=2_^^%N-1^ |%T__%i_ - (%T__%i-1_ + %T__%i_ + %T__%i+1_) / 3| / (%N - 2)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ or %T__%i+1_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - (%T__%i-1_ + %T__%i_ + %T__%i+1_) / 3| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 3, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
EQUATION (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute average perturbation.")
NORMAL (U"Finally, we compute the Relative Average Perturbation as")
EQUATION (U"%RAP = %absAP(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is a value between 0 and 2, or between 0 and 200 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (ppq5)...", U"ppgb", 20221202)
INTRO (U"A command that becomes available in the @@Query submenu@ when you select a @PointProcess object.")
NORMAL (U"This command will write into the Info window the %%five-point Period Perturbation Quotient%, "
	"a jitter measure defined as the average absolute difference between an interval and the average of it and its four closest neighbours, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (U"As jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (U"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (rap)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TERM (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TERM (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TERM (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TERM (U"##Maximum period factor")
DEFINITION (U"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (U"2. Usage")
NORMAL (U"The jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
ENTRY (U"3. Algorithm")
NORMAL (U"The five-point Period Perturbation Quotient (PPQ5) is defined in terms of five consecutive intervals, as follows.")
NORMAL (U"First, we define the absolute (i.e. non-relative) PPQ5 (in seconds):")
EQUATION (U"%absPPQ5(seconds) = ∑__%i=3_^^%N-2^ |%T__%i_ - (%T__%i-2_ + %T__%i-1_ + %T__%i_ + %T__%i+1_ + %T__%i+2_) / 5| / (%N - 4)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-2_ or %T__%i-1_ or %T__%i_ or %T__%i+1_ or %T__%i+2_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-2_/%T__%i-1_ or %T__%i-1_/%T__%i-2_ or %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ or %T__%i+2_/%T__%i+1_ or %T__%i+1_/%T__%i+2_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - (%T__%i-2_ + %T__%i-1_ + %T__%i_ + %T__%i+1_ + %T__%i+2_) / 5| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 5, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
EQUATION (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute PPQ5.")
NORMAL (U"Finally, we compute the five-point Period Perturbation Quotient as")
EQUATION (U"%PPQ5 = %PPQ5(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is a value between 0 and 4, or between 0 and 400 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (ddp)...", U"ppgb", 20221202)
INTRO (U"A command that becomes available in the @@Query submenu@ when you select a @PointProcess object.")
NORMAL (U"This command will write into the Info window the %%Difference of Differences of Periods%, "
	"a jitter measure defined as the average absolute difference between the consecutives differences between consecutive intervals, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (U"As jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (U"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (rap)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TERM (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TERM (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TERM (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TERM (U"##Maximum period factor")
DEFINITION (U"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (U"2. Usage")
NORMAL (U"The jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
ENTRY (U"3. Algorithm")
NORMAL (U"(In the following the term %absolute means two different things: (1) the absolute (i.e. non-negative) value of a real number, "
	"and (2) the opposite of %relative.)")
NORMAL (U"DDP is defined as the relative mean absolute (i.e. non-negative) "
	"third-order difference of the point process (= the second-order difference of the interval process), as follows.")
NORMAL (U"First, we define the absolute (i.e. non-relative) Average Perturbation (in seconds) as one third of the mean absolute (non-negative) "
	"difference of difference of consecutive intervals:")
EQUATION (U"%absDDP(seconds) = ∑__%i=2_^^%N-1^ |(%T__%i+1_ - %T__%i_) - (%T__%i_ - %T__%i-1_)| / (%N - 2)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ or %T__%i+1_ is not between ###Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term |2%T__%i_ - %T__%i-1_ - %T__%i+1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 3, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
EQUATION (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of DDP.")
NORMAL (U"Finally, we compute DDP as")
EQUATION (U"%DDP = %absDDP(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is exactly 3 times the @@PointProcess: Get jitter (rap)...|RAP@ jitter measurement: "
	"a value between 0 and 6, or between 0 and 600 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get low index...", U"ppgb", 20021212)
INTRO (U"A @@Query submenu|query@ to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the index of the nearest point before or at the specified time, "
	"or 0 if the point process contains no points or the specified time is before the first point.")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time from which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (U"PointProcess: Get nearest index...", U"ppgb", 20021212)
INTRO (U"A @@Query submenu|query@ to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the index of the point nearest to the specified time, "
	"or 0 if the point process contains no points.")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time around which a point is looked for, in seconds.")
/*
form Get nearest raising zero
   real Time_(s) 0.5
endform
To PointProcess (zeroes)... yes no
index = Get nearest index... Time
if index = 0
   # Geen punten gevonden: default-actie.
   time = 'Time'
else
   time = Get time from index... index
endif
echo 'time'
*/
MAN_END

MAN_BEGIN (U"PointProcess: Hum", U"ppgb", 19970330)
INTRO (U"A command to hear a @PointProcess.")
ENTRY (U"Algorithm")
NORMAL (U"A @Sound is created with the algorithm described at @@PointProcess: To Sound (hum)...@.")
NORMAL (U"This sound is then played.")
MAN_END

MAN_BEGIN (U"PointProcesses: Intersection", U"ppgb", 20021212)
INTRO (U"A command to merge two selected @PointProcess objects into one.")
ENTRY (U"Behaviour")
NORMAL (U"The resulting #PointProcess will contain only those points that occur in both original point processes.")
NORMAL (U"The time domain of the resulting point process is the intersection of the time domains of the original point processes.")
MAN_END

MAN_BEGIN (U"PointProcess: Play", U"ppgb", 19970330)
INTRO (U"A command to hear a @PointProcess.")
ENTRY (U"Algorithm")
NORMAL (U"A @Sound is created with the algorithm described at @@PointProcess: To Sound (pulse train)...@.")
NORMAL (U"This sound is then played.")
MAN_END

MAN_BEGIN (U"PointProcess: Remove point...", U"ppgb", 20021212)
INTRO (U"A command to remove a point from every selected @PointProcess.")
ENTRY (U"Setting")
TERM (U"##Index")
DEFINITION (U"the index of the point that is to be removed.")
ENTRY (U"Behaviour")
NORMAL (U"Does nothing if %index is less than 1 or greater than the number of points %nt in the point process. "
	"Otherwise, one point is removed (e.g., if %index is 3, the third point is removed), and the other points stay the same.")
MAN_END

MAN_BEGIN (U"PointProcess: Remove point near...", U"ppgb", 20021212)
INTRO (U"A command to remove a point from every selected @PointProcess.")
ENTRY (U"Setting")
TERM (U"##Time (s)")
DEFINITION (U"the time (in seconds) around which a point is to be removed.")
ENTRY (U"Behaviour")
NORMAL (U"Does nothing if there are no points in the point process. "
	"Otherwise, the point nearest to %time is removed, and the other points stay the same.")
MAN_END

MAN_BEGIN (U"PointProcess: Remove points...", U"ppgb", 20021212)
INTRO (U"A command to remove a range of points from every selected @PointProcess.")
ENTRY (U"Settings")
TERM (U"##From index (≥ 1)")
DEFINITION (U"the first index of the range of points that are to be removed.")
TERM (U"##To index")
DEFINITION (U"the last index of the range of points that are to be removed.")
ENTRY (U"Behaviour")
NORMAL (U"All points that originally fell in the range [%fromIndex, %toIndex] are removed, and the other points stay the same.")
MAN_END

MAN_BEGIN (U"PointProcess: Remove points between...", U"ppgb", 20021212)
INTRO (U"A command to remove a range of points from every selected @PointProcess.")
ENTRY (U"Settings")
TERM (U"##From time (s)")
DEFINITION (U"the start of the domain from which all points are to be removed.")
TERM (U"##To time (s)")
DEFINITION (U"the end of the domain from which all points are to be removed.")
ENTRY (U"Behaviour")
NORMAL (U"All points that originally fell in the domain [%fromTime, %toTime], including the edges, are removed, "
	"and the other points stay the same.")
MAN_END

MAN_BEGIN (U"PointProcess: To Sound (hum)...", U"ppgb", 19970330)
INTRO (U"A command to convert every selected @PointProcess into a @Sound.")
ENTRY (U"Algorithm")
NORMAL (U"A @Sound is created with the algorithm described at @@PointProcess: To Sound (pulse train)...@. "
	"This sound is then run through a sequence of second-order filters that represent five formants.")
MAN_END

MAN_BEGIN (U"PointProcess: To Sound (phonation)...", U"ppgb", 20070225)
INTRO (U"A command to convert every selected @PointProcess into a @Sound.")
ENTRY (U"Algorithm")
NORMAL (U"A glottal waveform is generated at every point in the point process. "
	"Its shape depends on the settings %power1 and %power2 according to the formula")
EQUATION (U"%U(%x) = %x^^%power1^ - %x^^%power2^")
NORMAL (U"where %x is a normalized time that runs from 0 to 1 and %U(%x) is the normalized glottal flow in arbitrary units (the real unit is m^3/s). "
	"If %power1 = 2.0 and %power2 = 3.0, the glottal flow shape is that proposed by @@Rosenberg (1971)@, "
	"upon which for instance the Klatt synthesizer is based (@@Klatt & Klatt (1990)@):")
SCRIPT (4.5, 3,
	U"Select outer viewport... 0 4.5 -0.4 3\n"
	"Axes... 0 1 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (x^2-x^3)*6\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
NORMAL (U"If %power1 = 3.0 and %power2 = 4.0, the glottal flow shape starts somewhat smoother, "
	"reflecting the idea that the glottis opens like a zipper:")
SCRIPT (4.5, 3,
	U"Axes... 0 1 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (x^3-x^4)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
NORMAL (U"For the generation of speech sounds, we do not take the glottal flow itself, "
	"but rather its derivative (this takes into account the influence of radiation at the lips). "
	"The glottal flow derivative is given by")
EQUATION (U"%dU(%x)/%dx = %power1 %x^^(%power1-1)^ - %power2 %x^^(%power2-1)^")
NORMAL (U"The flow derivative clearly shows the influence of the smoothing mentioned above. "
	"The unsmoothed curve, with %power1 = 2.0 and %power2 = 3.0, looks like:")
SCRIPT (4.5, 4,
	U"Axes... 0 1 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (2*x-3*x^2)*6\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL (U"Unlike the unsmoothed curve, the smoothed curve, with %power1 = 3.0 and %power2 = 4.0, starts out horizontally:")
SCRIPT (4.5, 4,
	U"Axes... 0 1 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (3*x^2-4*x^3)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL (U"Another setting is the %%open phase%. If it is 0.70, the glottis will be open during 70 percent of a period. "
	"Suppose that the PointProcess has a pulse at time 0, at time 1, at time 2, and so on. The pulses at times 1 and 2 will then be turned "
	"into glottal flows starting at times 0.30 and 1.30:")
SCRIPT (4.5, 2.5,
	U"Axes... 0 2 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes yes\n"
	"One mark bottom... 2 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 0.3 2 0\n"
	"Draw function... 0.3 1.3 300 if x<1 then (((x-0.3)/0.7)^3-((x-0.3)/0.7)^4)*8 else 0 fi\n"
	"Draw function... 1.3 2 300 (((x-1.3)/0.7)^3-((x-1.3)/0.7)^4)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
SCRIPT (4.5, 2.5,
	U"Axes... 0 2 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes yes\n"
	"One mark bottom... 2 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 0.3 2 0\n"
	"Draw function... 0.3 1.3 300 if x<1 then (3*((x-0.3)/0.7)^2-4*((x-0.3)/0.7)^3)*8 else 0 fi\n"
	"Draw function... 1.3 2 300 (3*((x-1.3)/0.7)^2-4*((x-1.3)/0.7)^3)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL (U"The final setting that influences the shape of the glottal flow is the %%collision phase%. "
	"If it is 0.03, for instance, the glottal flow derivative will not go abruptly to 0 at a pulse, "
	"but will instead decay by a factor of %e (≈ 2.7183) every 3 percent of a period. "
	"In order to keep the glottal flow curve smooth (and the derivative continuous), "
	"the basic shape discussed above has to be shifted slightly to the right and truncated "
	"at the time of the pulse, to be replaced there with the exponential decay curve; "
	"this also makes sure that the average of the derivative stays zero, as it was above "
	"(i.e. the area under the positive part of the curve equals the area above the negative part). "
	"This is what the curves look like if %power1 = 3.0, %power2 = 4.0, %openPhase = 0.70 and %collisionPhase = 0.03:")
SCRIPT (4.5, 2.5,
	U"Axes... 0 2 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes yes\n"
	"One mark bottom... 2 yes yes no\n"
	"Draw inner box\n"
	"xo = 0.32646\n"
	"g1 = 0.269422\n"
	"Draw function... 0 xo 300 g1 * exp(-x/0.03)\n"
	"Draw function... xo 1 300 (((x-xo)/0.7)^3-((x-xo)/0.7)^4)*8 + g1 * exp(-x/0.03)\n"
	"Draw function... 1 1+xo 300 g1 * exp(-(x-1)/0.03)\n"
	"Draw function... 1+xo 2 300 (((x-1-xo)/0.7)^3-((x-1-xo)/0.7)^4)*8 + g1 * exp(-(x-1)/0.03)\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
SCRIPT (4.5, 2.5,
	U"Axes... 0 2 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes yes\n"
	"One mark bottom... 2 yes yes no\n"
	"Draw inner box\n"
	"xo = 0.32646\n"
	"g1 = -8.980736 * 0.7\n"
	"Draw function... 0 xo 300 g1 * exp(-x/0.03)\n"
	"Draw function... xo 1 300 (3*((x-xo)/0.7)^2-4*((x-xo)/0.7)^3)*8 + g1 * exp(-x/0.03)\n"
	"Draw function... 1 1+xo 300 g1 * exp(-(x-1)/0.03)\n"
	"Draw function... 1+xo 2 300 (3*((x-1-xo)/0.7)^2-4*((x-1-xo)/0.7)^3)*8 + g1 * exp(-(x-1)/0.03)\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL (U"These curves have moved 2.646 percent of a period to the right. At time 1, "
	"the glottal flow curve turns from a convex polynomial into a concave exponential, "
	"and the derivative still has its minimum there.")
ENTRY (U"Settings")
TERM (U"##Sampling frequency (Hz)")
DEFINITION (U"the sampling frequency of the resulting Sound object, e.g. 44100 hertz.")
TERM (U"##Adaptation factor")
DEFINITION (U"the factor by which a pulse height will be multiplied if the pulse time is not within "
	"##Maximum period# from the previous pulse, and by which a pulse height will again be multiplied "
	"if the previous pulse time is not within ##Maximum period# from the pre-previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TERM (U"##Maximum period (s)")
DEFINITION (U"the minimal period that will be considered a silence, e.g. 0.05 seconds. "
	"Example: if ##Adaptation factor# is 0.6, and ##Adaptation time# is 0.02 s, "
	"then the heights of the first two pulses after silences of at least 20 ms "
	"will be multiplied by 0.36 and 0.6, respectively.")
MAN_END

MAN_BEGIN (U"PointProcess: To Sound (pulse train)...", U"ppgb", 20070225)
INTRO (U"A command to convert every selected @PointProcess into a @Sound.")
ENTRY (U"Algorithm")
NORMAL (U"A pulse is generated at every point in the point process. This pulse is filtered at the Nyquist frequency "
	"of the resulting #Sound by converting it into a sampled #sinc function.")
ENTRY (U"Settings")
TERM (U"##Sampling frequency (Hz)")
DEFINITION (U"the sampling frequency of the resulting Sound object, e.g. 44100 hertz.")
TERM (U"##Adaptation factor")
DEFINITION (U"the factor by which a pulse height will be multiplied if the pulse time is not within "
	"##Adaptation time# from the pre-previous pulse, and by which a pulse height will again be multiplied "
	"if the pulse time is not within ##Adaptation time# from the previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TERM (U"##Adaptation time (s)")
DEFINITION (U"the minimal period that will be considered a silence, e.g. 0.05 seconds.")
TERM (U"##Interpolation depth")
DEFINITION (U"the extent of the sinc function to the left and to the right of the peak, e.g. 2000 samples.")
NORMAL (U"Example: if ##Adaptation factor# is 0.6, and ##Adaptation time# is 0.02 s, "
	"then the heights of the first two pulses after silences of at least 20 ms "
	"will be multiplied by 0.36 and 0.6, respectively.")
MAN_END

MAN_BEGIN (U"PointProcesses: Union", U"ppgb", 20021212)
INTRO (U"A command to merge two selected @PointProcess objects into one.")
ENTRY (U"Behaviour")
NORMAL (U"The resulting #PointProcess will contain all the points of the two original point processes, sorted by time. "
	"Points that occur in both original point processes, will occur only once in the resulting point process.")
NORMAL (U"The time domain of the resulting point process is the union of the time domains of the original point processes.")
MAN_END

MAN_BEGIN (U"PointProcess: Up to IntensityTier...", U"ppgb", 19970329)
INTRO (U"A command to promote every selected @PointProcess to an @IntensityTier.")
ENTRY (U"Setting")
TERM (U"##Intensity (dB)")
DEFINITION (U"the intensity that will be associated with every point.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the points are trivially copied, and so is the time domain. "
	"The intensity information will be the same for every point.")
MAN_END

MAN_BEGIN (U"PointProcess: Up to PitchTier...", U"ppgb", 19970329)
INTRO (U"A command to promote every selected @PointProcess to a @PitchTier.")
ENTRY (U"Setting")
TERM (U"##Frequency (Hz)")
DEFINITION (U"the pitch frequency that will be associated with every point.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the points are trivially copied, and so is the time domain. "
	"The pitch information will be the same for every point.")
MAN_END


}

/* End of file manual_pointProcess.cpp */
