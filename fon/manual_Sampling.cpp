/* manual_Sampling.cpp
 *
 * Copyright (C) 1992-2005,2007,2011,2014-2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

#include "Sound.h"

void manual_Sampling_init (ManPages me);
void manual_Sampling_init (ManPages me) {

MAN_BEGIN (U"Get sampling period", U"ppgb", 20140421)
INTRO (U"A command that becomes available in the #Query menu if you select a @Sound object.")
NORMAL (U"The Info window will tell you the @@sampling period@ in seconds.")
ENTRY (U"Usage")
NORMAL (U"You will not often choose this command with the mouse, "
	"since the sampling period is included in the information that you get "
	"by clicking the #Info button. This command is probably more useful in a Praat script:")
CODE (U"selectObject: \"Sound hello\"")
CODE (U"samplingPeriod = Get sampling period")
ENTRY (U"Details for hackers")
NORMAL (U"With @Inspect, you can see how the sampling period is stored in a #Sound object: it is the #dx attribute.")
MAN_END

MAN_BEGIN (U"Get sampling frequency", U"ppgb", 20140421)
INTRO (U"A command that becomes available in the #Query menu if you select a @Sound object.")
NORMAL (U"The Info window will tell you the @@sampling frequency@ in hertz.")
ENTRY (U"Usage")
NORMAL (U"You will not often choose this command with the mouse, "
	"since the sampling frequency is included in the information that you get "
	"by clicking the #Info button. This command is probably more useful in a Praat script:")
CODE (U"selectObject: \"Sound hello\"")
CODE (U"samplingFrequency = Get sampling frequency")
ENTRY (U"Algorithm")
NORMAL (U"The sampling frequency is defined as 1 / (\\De%t), where \\De%t is the @@sampling period@. "
	"See @@Get sampling period@.")
MAN_END

MAN_BEGIN (U"Get time from sample number...", U"ppgb", 20040420)
INTRO (U"A command that becomes available in the #Query menu if you select a @Sound or @LongSound object.")
NORMAL (U"The Info window will tell you the time (in seconds) associated with the sample number that you specify.")
ENTRY (U"Setting")
TAG (U"##Sample number")
DEFINITION (U"the sample number whose time is sought.")
ENTRY (U"Algorithm")
NORMAL (U"the result is")
FORMULA (U"%t__1_ + (%sample_number - 1) \\.c \\De%t")
NORMAL (U"where %t__1_ is the time associated with the first sample, and \\De%t is the sampling period.")
ENTRY (U"Details for hackers")
NORMAL (U"If you select a Sound or LongSound and click @Inspect, "
	"you can see how the relation between sample numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN (U"Get sample number from time...", U"ppgb", 20140421)
INTRO (U"A command that becomes available in the #Query menu if you select a @Sound or @LongSound object.")
NORMAL (U"The Info window will tell you the sample number belonging to the time that you specify. "
	"The result is presented as a real number.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time (in seconds) for which you want to know the sample number.")
ENTRY (U"Example")
NORMAL (U"If the sound has a sampling frequency of 10 kHz, the sample number associated with a time of 0.1 seconds "
	"will usually be 1000.5.")
ENTRY (U"Scripting")
NORMAL (U"You can use this command to put the nearest sample number into a script variable:")
CODE (U"selectObject: \"Sound hallo\"")
CODE (U"sampleNumber = Get sample number from time... 0.1")
CODE (U"nearestSample = round (sampleNumber)")
NORMAL (U"In this case, the value will not be written into the Info window. To round down or up, use")
CODE (U"leftSample = floor (sampleNumber)")
CODE (U"rightSample = ceiling (sampleNumber)")
ENTRY (U"Algorithm")
NORMAL (U"the result is")
FORMULA (U"1 + (%time \\-- %t__1_) / \\De%t")
NORMAL (U"where %t__1_ is the time associated with the first sample, and \\De%t is the sampling period.")
ENTRY (U"Details for hackers")
NORMAL (U"If you select a Sound or LongSound and click @Inspect, "
	"you can see how the relation between sample numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN (U"Get number of samples", U"ppgb", 20040420)
INTRO (U"A command that becomes available in the #Query menu if you select a @Sound or @LongSound object.")
NORMAL (U"The Info window will tell you the total number of time samples in this object.")
ENTRY (U"Example")
NORMAL (U"If the sampling frequency is 44100 hertz, a recording with a duration of 60 seconds "
	"will contain 2,646,000 samples.")
ENTRY (U"Details for hackers")
NORMAL (U"If you select a Sound or LongSound and click @Inspect, "
	"you can see how the number of samples is stored in the object: it is the #nx attribute.")
MAN_END

MAN_BEGIN (U"Get time from frame number...", U"ppgb", 20040420)
INTRO (U"A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL (U"The Info window will tell you the time associated with the frame number that you specify.")
ENTRY (U"Setting")
TAG (U"##Frame number")
DEFINITION (U"the frame number whose time is sought.")
ENTRY (U"Algorithm")
NORMAL (U"the result is")
FORMULA (U"%t__1_ + (%frame_number - 1) \\.c \\De%t")
NORMAL (U"where %t__1_ is the time associated with the centre of the first frame, and \\De%t is the time step.")
ENTRY (U"Details for hackers")
NORMAL (U"If you select one of the above objects and click @Inspect, "
	"you can see how the relation between frame numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN (U"Get frame number from time...", U"ppgb", 20140421)
INTRO (U"A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL (U"The Info window will tell you the frame number belonging to the time that you specify. "
	"The result is presented as a real number.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time (in seconds) for which you want to know the frame number.")
ENTRY (U"Example")
NORMAL (U"If the Pitch object has a time step of 10 ms, and the first frame is centred around 18 ms, "
	"the frame number associated with a time of 0.1 seconds is 9.2.")
ENTRY (U"Scripting")
NORMAL (U"You can use this command to put the nearest frame centre into a script variable:")
CODE (U"selectObject: \"Pitch hallo\"")
CODE (U"frame = Get frame from time... 0.1")
CODE (U"nearestFrame = round (frame)")
NORMAL (U"In this case, the value will not be written into the Info window. To round down or up, use")
CODE (U"leftFrame = floor (frame)")
CODE (U"rightFrame = ceiling (frame)")
ENTRY (U"Algorithm")
NORMAL (U"the result is")
FORMULA (U"1 + (%time \\-- %t__1_) / \\De%t")
NORMAL (U"where %t__1_ is the time associated with the centre of the first frame, "
	"and \\De%t is the time step.")
ENTRY (U"Details for hackers")
NORMAL (U"If you select one of the above objects and click @Inspect, "
	"you can see how the relation between frame numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN (U"Get time step", U"ppgb", 20040420)
INTRO (U"A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL (U"The Info window will tell you the time difference between consecutive frames, e.g. "
	"the time difference between consecutive formant circles in the sound editor window.")
ENTRY (U"Details for hackers")
NORMAL (U"If you select one of the above objects and click @Inspect, "
	"you can see how the time step is stored in the object: it is the #dx attribute.")
MAN_END

MAN_BEGIN (U"Get number of frames", U"ppgb", 20040420)
INTRO (U"A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL (U"The Info window will tell you the total number of time frames in the object.")
ENTRY (U"Details for hackers")
NORMAL (U"If you select one of the above objects and click @Inspect, "
	"you can see how the number of frames is stored in the object: it is the #nx attribute.")
MAN_END

}

/* End of file manual_Sampling.cpp */
