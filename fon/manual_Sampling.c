/* manual_Sampling.c
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

#include "Sound.h"

void manual_Sampling_init (ManPages me);
void manual_Sampling_init (ManPages me) {

MAN_BEGIN ("Get sampling period", "ppgb", 20040420)
INTRO ("A command that becomes available in the #Query menu if you select a @Sound object.")
NORMAL ("The Info window will tell you the @@sampling period@ in seconds.")
ENTRY ("Usage")
NORMAL ("You will not often choose this command with the mouse, "
	"since the sampling period is included in the information that you get "
	"by clicking the #Info button. This command is probably more useful in a Praat script:")
CODE ("select Sound hello")
CODE ("samplingPeriod = Get sampling period")
ENTRY ("Details for hackers")
NORMAL ("With @Inspect, you can see how the sampling period is stored in a #Sound object: it is the #dx attribute.")
MAN_END

MAN_BEGIN ("Get sampling frequency", "ppgb", 20040420)
INTRO ("A command that becomes available in the #Query menu if you select a @Sound object.")
NORMAL ("The Info window will tell you the @@sampling frequency@ in Hertz.")
ENTRY ("Usage")
NORMAL ("You will not often choose this command with the mouse, "
	"since the sampling frequency is included in the information that you get "
	"by clicking the #Info button. This command is probably more useful in a Praat script:")
CODE ("select Sound hello")
CODE ("samplingFrequency = Get sampling frequency")
ENTRY ("Algorithm")
NORMAL ("The sampling frequency is defined as 1 / (\\De%t), where \\De%t is the @@sampling period@. "
	"See @@Get sampling period@.")
MAN_END

MAN_BEGIN ("Get time from sample number...", "ppgb", 20040420)
INTRO ("A command that becomes available in the #Query menu if you select a @Sound or @LongSound object.")
NORMAL ("The Info window will tell you the time (in seconds) associated with the sample number that you specify.")
ENTRY ("Setting")
TAG ("%%Sample number")
DEFINITION ("the sample number whose time is sought.")
ENTRY ("Algorithm")
NORMAL ("the result is")
FORMULA ("%t__1_ + (%sample_number - 1) \\.c \\De%t")
NORMAL ("where %t__1_ is the time associated with the first sample, and \\De%t is the sampling period.")
ENTRY ("Details for hackers")
NORMAL ("If you select a Sound or LongSound and click @Inspect, "
	"you can see how the relation between sample numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN ("Get sample number from time...", "ppgb", 20040505)
INTRO ("A command that becomes available in the #Query menu if you select a @Sound or @LongSound object.")
NORMAL ("The Info window will tell you the sample number belonging to the time that you specify. "
	"The result is presented as a real number.")
ENTRY ("Setting")
TAG ("%%Time (seconds)")
DEFINITION ("the time for which you want to know the sample number.")
ENTRY ("Example")
NORMAL ("If the sound has a sampling frequency of 10 kHz, the sample number associated with a time of 0.1 seconds "
	"will usually be 1000.5.")
ENTRY ("Scripting")
NORMAL ("You can use this command to put the nearest sample number into a script variable:")
CODE ("select Sound hallo")
CODE ("sampleNumber = Get sample number from time... 0.1")
CODE ("nearestSample = round (sampleNumber)")
NORMAL ("In this case, the value will not be written into the Info window. To round down or up, use")
CODE ("leftSample = floor (sampleNumber)")
CODE ("rightSample = ceiling (sampleNumber)")
ENTRY ("Algorithm")
NORMAL ("the result is")
FORMULA ("1 + (%time \\-- %t__1_) / \\De%t")
NORMAL ("where %t__1_ is the time associated with the first sample, and \\De%t is the sampling period.")
ENTRY ("Details for hackers")
NORMAL ("If you select a Sound or LongSound and click @Inspect, "
	"you can see how the relation between sample numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN ("Get number of samples", "ppgb", 20040420)
INTRO ("A command that becomes available in the #Query menu if you select a @Sound or @LongSound object.")
NORMAL ("The Info window will tell you the total number of time samples in this object.")
ENTRY ("Example")
NORMAL ("If the sampling frequency is 44100 Hertz, a recording with a duration of 60 seconds "
	"will contain 2,646,000 samples.")
ENTRY ("Details for hackers")
NORMAL ("If you select a Sound or LongSound and click @Inspect, "
	"you can see how the number of samples is stored in the object: it is the #nx attribute.")
MAN_END

MAN_BEGIN ("Get time from frame number...", "ppgb", 20040420)
INTRO ("A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL ("The Info window will tell you the time associated with the frame number that you specify.")
ENTRY ("Setting")
TAG ("%%Frame number")
DEFINITION ("the frame number whose time is sought.")
ENTRY ("Algorithm")
NORMAL ("the result is")
FORMULA ("%t__1_ + (%frame_number - 1) \\.c \\De%t")
NORMAL ("where %t__1_ is the time associated with the centre of the first frame, and \\De%t is the time step.")
ENTRY ("Details for hackers")
NORMAL ("If you select one of the above objects and click @Inspect, "
	"you can see how the relation between frame numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN ("Get frame number from time...", "ppgb", 20040505)
INTRO ("A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL ("The Info window will tell you the frame number belonging to the time that you specify. "
	"The result is presented as a real number.")
ENTRY ("Setting")
TAG ("%%Time (seconds)")
DEFINITION ("the time for which you want to know the frame number.")
ENTRY ("Example")
NORMAL ("If the Pitch object has a time step of 10 ms, and the first frame is centred around 18 ms, "
	"the frame number associated with a time of 0.1 seconds is 9.2.")
ENTRY ("Scripting")
NORMAL ("You can use this command to put the nearest frame centre into a script variable:")
CODE ("select Pitch hallo")
CODE ("frame = Get frame from time... 0.1")
CODE ("nearestFrame = round (frame)")
NORMAL ("In this case, the value will not be written into the Info window. To round down or up, use")
CODE ("leftFrame = floor (frame)")
CODE ("rightFrame = ceiling (frame)")
ENTRY ("Algorithm")
NORMAL ("the result is")
FORMULA ("1 + (%time \\-- %t__1_) / \\De%t")
NORMAL ("where %t__1_ is the time associated with the centre of the first frame, "
	"and \\De%t is the time step.")
ENTRY ("Details for hackers")
NORMAL ("If you select one of the above objects and click @Inspect, "
	"you can see how the relation between frame numbers and times is stored in the object: "
	"%t__1_ is the #x1 attribute, and \\De%t is the #dx attribute.")
MAN_END

MAN_BEGIN ("Get time step", "ppgb", 20040420)
INTRO ("A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL ("The Info window will tell you the time difference between consecutive frames, e.g. "
	"the time difference between consecutive formant circles in the sound editor window.")
ENTRY ("Details for hackers")
NORMAL ("If you select one of the above objects and click @Inspect, "
	"you can see how the time step is stored in the object: it is the #dx attribute.")
MAN_END

MAN_BEGIN ("Get number of frames", "ppgb", 20040420)
INTRO ("A command that becomes available in the #Query menu if you select a sound-analysis object that is a function of time "
	"and that is evenly sampled in time (@Pitch, @Formant, @Intensity, @Harmonicity).")
NORMAL ("The Info window will tell you the total number of time frames in the object.")
ENTRY ("Details for hackers")
NORMAL ("If you select one of the above objects and click @Inspect, "
	"you can see how the number of frames is stored in the object: it is the #nx attribute.")
MAN_END

}

/* End of file manual_Sampling.c */
