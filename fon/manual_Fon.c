/* manual_Fon.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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

static void draw_CreateDurationTier (Graphics g) {
	static float x [] = { 0, 0.3, 0.6, 0.7, 0.9 }, y [] = { 1, 1, 2.3, 1, 1 };
	Graphics_setWindow (g, 0, 0.9, 0, 2.5);
	Graphics_drawInnerBox (g);
	Graphics_marksLeftEvery (g, 1, 1, TRUE, TRUE, FALSE);
	Graphics_marksBottomEvery (g, 1, 0.1, TRUE, TRUE, FALSE);
	Graphics_setInner (g);
	Graphics_setColour (g, Graphics_RED);
	Graphics_polyline (g, 5, x, y);
	Graphics_setColour (g, Graphics_BLACK);
	Graphics_unsetInner (g);
	Graphics_textBottom (g, TRUE, "Time (s)");
	Graphics_textLeft (g, TRUE, "Relative duration");
}

void manual_Fon_init (ManPages me);
void manual_Fon_init (ManPages me) {

{ extern void manual_Sampling_init (ManPages me); manual_Sampling_init (me); }
{ extern void manual_sound_init (ManPages me); manual_sound_init (me); }
{ extern void manual_pitch_init (ManPages me); manual_pitch_init (me); }
{ extern void manual_spectrum_init (ManPages me); manual_spectrum_init (me); }
{ extern void manual_formant_init (ManPages me); manual_formant_init (me); }
{ extern void manual_annotation_init (ManPages me); manual_annotation_init (me); }

MAN_BEGIN ("Get high index from time...", "ppgb", 20030216)
INTRO ("A @query to ask the selected tier object "
	"(@DurationTier, @FormantTier, @IntensityTier, @PitchTier, @TextTier) "
	"which point is nearest to, but no earlier than, the specified time.")
ENTRY ("Argument")
TAG ("%Time (s)")
DEFINITION ("the time from which you want to get the point index.")
ENTRY ("Return value")
NORMAL ("This query returns the index of the point with the lowest time greater than or equal to %time."
	"It is @undefined if there are no points. "
	"It is the number of points plus 1 (offright) if the specified time is greater than the time of the last point.")
MAN_END

MAN_BEGIN ("Get low index from time...", "ppgb", 20030216)
INTRO ("A @query to ask the selected tier object "
	"(@DurationTier, @FormantTier, @IntensityTier, @PitchTier, @TextTier) "
	"which point is nearest to, but no later than, the specified time.")
ENTRY ("Argument")
TAG ("%Time (s)")
DEFINITION ("the time from which you want to get the point index.")
ENTRY ("Return value")
NORMAL ("This query returns the index of the point with the highest time less than or equal to %time."
	"It is @undefined if there are no points. "
	"It is 0 (offleft) if the specified time is less than the time of the first point.")
MAN_END

MAN_BEGIN ("Get nearest index from time...", "ppgb", 20030216)
INTRO ("A @query to ask the selected tier object "
	"(@DurationTier, @FormantTier, @IntensityTier, @PitchTier, @TextTier) "
	"which point is nearest to the specified time.")
ENTRY ("Argument")
TAG ("%Time (s)")
DEFINITION ("the time near which you want to get the point index.")
ENTRY ("Return value")
NORMAL ("This query returns the index of the point with the highest time less than or equal to %time."
	"It is @undefined if there are no points.")
MAN_END

MAN_BEGIN ("Remove point...", "ppgb", 20030216)
INTRO ("A command to remove one point from every selected time-based tier object "
	"(@DurationTier, @FormantTier, @IntensityTier, @PitchTier, @TextTier).")
ENTRY ("Argument")
TAG ("%%Point number")
DEFINITION ("the index of the point you want to remove.")
ENTRY ("Behaviour")
NORMAL ("If %%point number% is 3, the third point counted from the start of the tier (if it exists) "
	"is removed from the tier.")
MAN_END

MAN_BEGIN ("Remove point near...", "ppgb", 20030216)
INTRO ("A command to remove one point from every selected time-based tier object "
	"(@DurationTier, @FormantTier, @IntensityTier, @PitchTier, @TextTier).")
ENTRY ("Argument")
TAG ("%Time (s)")
DEFINITION ("the time near which you want to remove a point.")
ENTRY ("Behaviour")
NORMAL ("The point nearest to %time (if there is any point) is removed from the tier.")
MAN_END

MAN_BEGIN ("Remove points between...", "ppgb", 20030216)
INTRO ("A command to remove some points from every selected time-based tier object "
	"(@DurationTier, @FormantTier, @IntensityTier, @PitchTier, @TextTier).")
ENTRY ("Arguments")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the times between which you want to remove all points.")
ENTRY ("Behaviour")
NORMAL ("Any points between %tmin and %tmax (inclusive) are removed from the tier.")
MAN_END

MAN_BEGIN ("Cochleagram", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. It represents the excitation pattern "
	"of the basilar membrane in the inner ear (see @Excitation) as a function of time.")
MAN_END

MAN_BEGIN ("Cochleagram: Formula...", "ppgb", 20021206)
INTRO ("A command for changing the data in all selected @Cochleagram objects.")
NORMAL ("See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN ("Create DurationTier...", "ppgb", 20021204)
INTRO ("A command in the @@New menu@ to create an empty @DurationTier object.")
NORMAL ("The resulting object will have the specified name and time domain, but contain no duration points. "
	"To add some points to it, use @@DurationTier: Add point...@.")
ENTRY ("Scripting example")
NORMAL ("To create a tier 0.9 seconds long, with an deceleration around 0.6 seconds, you do:")
CODE ("Create DurationTier... dur 0 0.9")
CODE ("Add point... 0.3 1")
CODE ("Add point... 0.6 2.3")
CODE ("Add point... 0.7 1")
NORMAL ("The result will look like")
PICTURE (5, 2.5, draw_CreateDurationTier)
NORMAL ("The target duration will be the area under this curve, which is 0.9 + 1/2 \\.c 1.3 \\.c 0.4 = 1.16 seconds.")
MAN_END

MAN_BEGIN ("Create empty PointProcess...", "ppgb", 20021204)
INTRO ("A command in the @@New menu@ to create an empty @PointProcess. The newly created object is put in the list of objects.")
MAN_END

MAN_BEGIN ("Create IntensityTier...", "ppgb", 20021204)
INTRO ("A command in the @@New menu@ to create an empty @IntensityTier object.")
NORMAL ("The resulting object will have the specified name and time domain, but contain no formant points. "
	"To add some points to it, use @@IntensityTier: Add point...@.")
NORMAL ("For an example, see @@Source-filter synthesis@.")
MAN_END

MAN_BEGIN ("Create Matrix...", "ppgb", 20021212)
INTRO ("A command in the @@New menu@ to create a @Matrix with the specified sampling attributes, "
	"filled with values from a formula (see @@Matrix: Formula...@).")
MAN_END

MAN_BEGIN ("Create Poisson process...", "ppgb", 20041005)
INTRO ("A command to create a @PointProcess object that represents a Poisson process.")
NORMAL ("A Poisson process is a stationary point process with a fixed density %\\la, "
	"which means that there are, on the average, %\\la events per second.")
ENTRY ("Settings")
TAG ("%%Start time% (seconds)")
DEFINITION ("%t__%min_, the beginning of the time domain.")
TAG ("%%End time% (seconds)")
DEFINITION ("%t__%max_, the end of the time domain.")
TAG ("%Density (Hertz)")
DEFINITION ("the average number of points per second.")
ENTRY ("Algorithm")
NORMAL ("First, the number of points %N in the time domain is determined. Its expectation value is")
FORMULA ("%\\la = (%t__%max_ \\-- %t__%min_) \\.c %density")
NORMAL ("but its actual value is taken from the Poisson distribution:")
FORMULA ("%p(%n) = (%%\\la^n% / %n!) %e^^\\--%\\la")
NORMAL ("Then, %N points are computed throughout the time domain, according to a uniform distribution:")
FORMULA ("%p(%t) = 1 / (%t__%max_ \\-- %t__%min_)   for %t \\e= [%t__%min_, %t__%max_]")
FORMULA ("%p(%t) = 0   outside [%t__%min_, %t__%max_]")
MAN_END

MAN_BEGIN ("Create simple Matrix...", "ppgb", 20021204)
INTRO ("A command in the @@New menu@ to create a @Matrix with the specified number of rows and columns, "
	"filled with values from a formula (see @@Matrix: Formula...@).")
MAN_END

MAN_BEGIN ("Create Strings as directory list...", "ppgb", 20060919)
INTRO ("A command in the @@New menu@ to create a @Strings object containing a list of directories in a given parent directory. "
	"It works completely analogously to @@Create Strings as file list...@.")
MAN_END

MAN_BEGIN ("Create Strings as file list...", "ppgb", 20060919)
INTRO ("A command in the @@New menu@ to create a @Strings object containing a list of files in a given directory.")
ENTRY ("Settings")
TAG ("%%Name")
DEFINITION ("the name of the resulting Strings object, usually \"fileList\".")
TAG ("%%Path")
DEFINITION ("the directory name, with an optional wildcard for selecting files.")
ENTRY ("Behaviour")
NORMAL ("The resulting Strings object will contain an alphabetical list of file names, "
	"without the preceding path through the directory structures. If there are not files that match %path, "
	"the Strings object will contain no strings.")
ENTRY ("Usage")
NORMAL ("There are two ways to specify the path.")
NORMAL ("One way is to specify a directory name only. On Unix, you could type "
	"##/usr/people/miep/sounds# or ##/usr/people/miep/sounds/#, for instance. On Windows, "
	"##C:\\bsDocument and Settings\\bsMiep\\bsSounds# or ##C:\\bsDocument and Settings\\bsMiep\\bsSounds\\bs#. "
	"On Macintosh, ##/Users/miep/Sounds# or ##/Users/miep/Sounds/#. Any of these return "
	"a list of all the files in the specified directory.")
NORMAL ("The other way is to specify a wildcard (a single asterisk) for the file names. "
	"To get a list of all the files whose names start with \"hal\" and end in \".wav\", "
	"type ##/usr/people/miep/sounds/hal*.wav#, ##C:\\bsDocument and Settings\\bsMiep\\bsSounds\\bshal*.wav#, "
	"or ##/Users/miep/Sounds/hal*.wav#.")
ENTRY ("Script usage")
NORMAL ("In a script, you can use this command to cycle through the files in a directory. "
	"For instance, to read in all the sound files in a specified directory, "
	"you could use the following script:")
CODE ("directory\\$  = \"/usr/people/miep/sounds\"")
CODE ("Create Strings as file list... list 'directory\\$ '/*.wav")
CODE ("numberOfFiles = Get number of strings")
CODE ("for ifile to numberOfFiles")
CODE ("   select Strings list")
CODE ("   fileName\\$  = Get string... ifile")
CODE ("   Read from file... 'directory\\$ '/'fileName\\$ '")
CODE ("endfor")
NORMAL ("If the script has been saved to a script file, you can use paths that are relative to the directory "
	"where you saved the script. Thus, with")
CODE ("Create Strings as file list... list *.wav")
NORMAL ("you get a list of all the .wav files that are in the same directory as the script that contains this line. "
	"And to get a list of all the .wav files in the directory Sounds that resides in the same directory as your script, "
	"you can do")
CODE ("Create Strings as file list... list Sounds/*.wav")
NORMAL ("As is usual in Praat scripting, the forward slash (\"/\") in this example can be used on all platforms, including Windows. "
	"This makes your script portable across platforms.")
ENTRY ("See also")
NORMAL ("To get a list of directories instead of files, use @@Create Strings as directory list...@.")
MAN_END

MAN_BEGIN ("Distributions", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. Inherits most actions from @TableOfReal.")
ENTRY ("Actions")
LIST_ITEM ("@@Distributions: To Strings...@")
MAN_END

MAN_BEGIN ("Distributions: To Strings...", "ppgb", 19971022)
INTRO ("A command to create a @Strings object from every selected @Distributions object.")
ENTRY ("Arguments")
TAG ("%%Column number")
DEFINITION ("the column (in the #Distributions object) that contains the distribution that you are "
	"interested in. Often the #Distributions object will only contain a single distribution, "
	"so this argument will often be 1. If the #Distributions object contains nine distributions, "
	"specify any number between 1 and 9.")
TAG ("%%Number of strings")
DEFINITION ("the number of times a string will be drawn from the chosen distribution. "
	"This is the number of strings that the resulting @Strings object is going to contain.")
ENTRY ("Behaviour")
NORMAL ("Every string in the resulting #Strings object will be a row label of the #Distributions object. "
	"The number in each row at the specified column will be considered the relative frequency of "
	"occurrence of that row.")
NORMAL ("%#Example. Suppose we have the following #Distributions:")
CODE ("File type = \"ooTextFile\"")
CODE ("Object class = \"Distributions\"")
CODE ("2 (number of columns)")
CODE ("         \"English\"  \"French\" (column labels)")
CODE ("3 (number of rows)")
CODE ("\"the\"     108        1.5")
CODE ("\"a\"       58.1       33")
CODE ("\"pour\"    0.7        15.5")
NORMAL ("If you set %Column to 1 and %%Number of strings% to 1000, "
	"you will get a @Strings object with approximately 647 occurrences of \"the\", "
	"348 occurrences of \"a\", and 4 occurrences of \"pour\". "
	"If you had set %Column to 2 (\"French\"), you would have gotten about "
	"30 times \"the\", 660 times \"a\", and 310 times \"pour\". "
	"The actual numbers will vary because the choice of a string will not depend on previous choices.")
MAN_END

MAN_BEGIN ("DurationTier", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. "
	"A DurationTier object contains a number of (%time, %duration) points, "
	"where %duration is to be interpreted as a relative duration (e.g. the duration of a manipulated sound "
	"as compared to the duration of the original). For instance, "
	"if your DurationTier contains two points, one with a duration value of 1.5 at a time of 0.5 seconds "
	"and one with a duration value of 0.6 at a time of 1.1 seconds, this is to be interpreted as "
	"a relative duration of 1.5 (i.e. a slowing down) for all original times before 0.5 seconds, "
	"a relative duration of 0.6 (i.e. a speeding up) for all original times after 1.1 seconds, "
	"and a linear interpolation between 0.5 and 1.1 seconds (e.g. a relative duration of "
	"1.2 at 0.7 seconds, and of 0.9 at 0.9 seconds).")
NORMAL ("See @@Intro 8.2. Manipulation of duration@ and @@Create DurationTier...@.")
MAN_END

MAN_BEGIN ("DurationTier: Add point...", "ppgb", 20030216)
INTRO ("A command to add a point to each selected @DurationTier. "
	"For an example, see @@Create DurationTier...@.")
ENTRY ("Arguments")
TAG ("%Time (s)")
DEFINITION ("the time at which a point is to be added.")
TAG ("%%Relative duration")
DEFINITION ("the relative duration value of the requested new point.")
ENTRY ("Behaviour")
NORMAL ("The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN ("DurationTier: Get target duration...", "ppgb", 19991016)
INTRO ("A @query to the selected @DurationTier for the target duration of a specified time range.")
ENTRY ("Arguments")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the start and end of the time range. If %fromTime or %toTime is outside the time domain "
	"of the Duration object, there will be .")
ENTRY ("Return value")
NORMAL ("the target duration in seconds.")
MAN_END

MAN_BEGIN ("DurationTierEditor", "ppgb", 20010330)
INTRO ("One of the @editors in the Praat program, for viewing and editing a @DurationTier object. "
	"To create a DurationTierEditor window, select a DurationTier and click Edit.")
MAN_END

MAN_BEGIN ("Editors", "ppgb", 20041110)
INTRO ("Many @@types of objects@ in P\\s{RAAT} can be viewed and edited in their own windows.")
ENTRY ("Editor windows")
LIST_ITEM ("\\bu @SoundEditor")
LIST_ITEM ("\\bu @LongSoundEditor")
LIST_ITEM ("\\bu @TextGridEditor")
LIST_ITEM ("\\bu @ManipulationEditor")
LIST_ITEM ("\\bu @SpectrumEditor")
LIST_ITEM ("\\bu @PitchEditor")
LIST_ITEM ("\\bu @PointEditor")
LIST_ITEM ("\\bu @PitchTierEditor")
LIST_ITEM ("\\bu @IntensityTierEditor")
LIST_ITEM ("\\bu @DurationTierEditor")
LIST_ITEM ("\\bu #SpectrogramEditor")
LIST_ITEM ("\\bu #ArtwordEditor")
LIST_ITEM ("\\bu @OTGrammarEditor")
LIST_ITEM ("\\bu (any type: @Inspect)")
ENTRY ("How to open an editor for an object")
NORMAL ("To open an editor window for an object in the list, select the object and choose #Edit "
	"(if the #Edit button exists, it is usually at the top of the @@Dynamic menu@). "
	"The name of the object will appear as the title of the editor window.")
NORMAL ("Objects that cannot be modified (e.g. LongSound) have the command #View instead of #Edit.")
ENTRY ("General behaviour")
NORMAL ("Changes that you make to an object in its editor window will take effect immediately. "
	"For instance, you do not have close the editor window before saving the changed object to disk.")
NORMAL ("If you @Remove an object that you are viewing or editing from the @@List of Objects@, "
	"the editor window will automatically disappear from the screen.")
NORMAL ("All editors are independent windows: you can minimize and maximize them; "
	"if an editor window goes hiding behind another window, "
	"you can raise it by choosing the #Edit command again.")
NORMAL ("If you rename an object that you are viewing or editing (with @@Rename...@), "
	"the title of the editor window immediately changes to the new name.")
ENTRY ("Ways to control an editor window")
LIST_ITEM ("\\bu @@Click")
LIST_ITEM ("\\bu @@Shift-click")
LIST_ITEM ("\\bu @@Drag")
LIST_ITEM ("\\bu @@Shift-drag")
LIST_ITEM ("\\bu @@Time selection")
LIST_ITEM ("\\bu @@Keyboard shortcuts")
MAN_END

MAN_BEGIN ("Excitation", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. It represents the excitation pattern "
	"of the basilar membrane in the inner ear.")
ENTRY ("Inside an Excitation object")
NORMAL ("With @Inspect, you will see the following attributes.")
TAG ("%xmin = 0")
DEFINITION ("minimum place or frequency (Bark).")
TAG ("%xmax = 25.6 Bark")
DEFINITION ("maximum place or frequency (Bark).")
TAG ("%nx")
DEFINITION ("number of places or frequencies.")
TAG ("%dx = 25.6 / %nx")
DEFINITION ("Place or frequency step (Bark).")
TAG ("%x1 = %dx / 2")
DEFINITION ("centre of first place or frequency band (Bark).")
TAG ("%ymin = %ymax = %dy = %y__1_ = 1; %ny = 1")
DEFINITION ("dummies.")
TAG ("%z [1]")
DEFINITION ("intensity (sensation level) in phon.")
MAN_END

MAN_BEGIN ("Excitation: Formula...", "ppgb", 20021206)
INTRO ("A command for changing the data in all selected @Excitation objects.")
NORMAL ("See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN ("Excitation: Get loudness", "ppgb", 19991016)
INTRO ("A @query to ask the selected @Excitation object for its loudness.")
ENTRY ("Return value")
NORMAL ("the loudness in sone units.")
ENTRY ("Algorithm")
NORMAL ("The loudness is defined as")
FORMULA ("\\in%df 2^^(%e(%f) - 40 phon) / 10^")
NORMAL ("where %f is the frequency in Bark, and %e(%f) the excitation in phon. "
	"For our discrete Excitation object, the loudness is computed as")
FORMULA ("\\De%f \\su 2^^(%e__%i_ - 40) / 10")
NORMAL ("where \\De%f is the distance between the excitation channels (in Bark).")
MAN_END

MAN_BEGIN ("Excitation_hertzToBark", "ppgb", 19970401)
INTRO ("A routine for converting frequency into basilar place, "
	"the inverse of @Excitation_barkToHertz.")
ENTRY ("Syntax")
PROTOTYPE ("##double Excitation_hertzToBark (double #%hertz##);")
ENTRY ("Algorithm")
NORMAL ("Returns 7 \\.c ln (%hertz / 650 + \\Vr (1 + (%hertz / 650)^2)).")
MAN_END

MAN_BEGIN ("Excitation_barkToHertz", "ppgb", 19970401)
INTRO ("A routine for converting basilar place into frequency, "
	"the inverse of @Excitation_hertzToBark.")
ENTRY ("Syntax")
PROTOTYPE ("##double Excitation_barkToHertz (double #%bark##);")
ENTRY ("Algorithm")
NORMAL ("Returns 650 \\.c sinh (%bark / 7).")
MAN_END

/*
double Excitation_soundPressureToPhon (double soundPressure, double bark);
Uses auditory filter (width apx. 1 Bark) for masking.

Excitation Excitation_create (double df, long nf);
	Function:
		return a new instance of Excitation, or NULL if out of memory.
	Preconditions:
		df > 0.0;
		nf >= 1;
	Postconditions:
		result -> xmin == 0.0;		result -> ymin == 1;
		result -> xmax == 25.6;		result -> ymax == 1;
		result -> nx == nf;			result -> ny == 1;
		result -> dx == df;			result -> dy == 1;
		result -> x1 == 0.5 * df;		result -> y1 == 1;
		result -> z [1] [1..nt] == 0.0;
double Excitation_getDistance (Excitation me, Excitation thee);

void Excitation_draw (Excitation me, Graphics g, double fmin, double fmax, double minimum, double maximum, int garnish);

Matrix Excitation_to_Matrix (Excitation me);
	Function:
		Create a Matrix from an Excitation,
		with deep copy of all of its Matrix attributes, except class information and methods.
	Return NULL if out of memory.  
Excitation Matrix_to_Excitation (Matrix me);
	Function:
		create an Excitation from a Matrix.
		Return NULL if out of memory.
	Postconditions:
		thy xmin == 0.0;
		thy xmax == my nx / (1 / my dx);
		thy nx == my nx;
		thy dx == 1 / (1 / my dx);
		thy x1 == 0.5 * thy dx;
		thy ymin ymax ny dy y1 == 1;
		thy z [1] [...] == my z [1] [...];
*/

/*"Any object of one of the types Polygon, PointProcess, Sound, Pitch, Spectrum,\n"
"Spectrogram, Excitation, Cochleagram, VocalTract\n"
"is convertible to and from a Matrix.\n"
"This allows extra hacking and drawing possibilities.\n"
"For example, to square the y values of a Polygon, do the following:\n"
"   select a Polygon, click on `To Matrix', click on `Formula...',\n"
"   type \"self * if row=1 then 1 else self fi\", click on `To Polygon'."*/

MAN_BEGIN ("Formula...", "ppgb", 19980319)
NORMAL ("See @@Matrix: Formula...@")
MAN_END

MAN_BEGIN ("Frequency selection", "ppgb", 20010402)
INTRO ("The way to select a frequency domain in the @SpectrumEditor. "
	"This works completely analogously to the @@time selection@ in other editors.")
MAN_END

MAN_BEGIN ("Get area...", "ppgb", 20030216)
INTRO ("A @query to the selected tier object (@PitchTier, @IntensityTier, @DurationTier).")
ENTRY ("Return value")
NORMAL ("the area under the curve.")
ENTRY ("Attributes")
TAG ("%%From time% (s), %%To time% (s)")
DEFINITION ("the selected time domain. Values outside this domain are ignored. "
	"If %%To time% is not greater than %%From time%, the entire time domain of the tier is considered.")
ENTRY ("Algorithm")
NORMAL ("The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the area is")
FORMULA ("1/2 (%f__1_ + %f__2_) (%t__2_ \\-- %t__1_)")
MAN_END

MAN_BEGIN ("Intensity", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An Intensity object represents an intensity contour at linearly spaced time points "
	"%t__%i_ = %t__1_ + (%i \\-- 1) %dt, with values in dB SPL, i.e. dB relative to 2\\.c10^^-5^ Pascal, "
	"which is the normative auditory threshold for a 1000-Hz sine wave.")
MAN_END

MAN_BEGIN ("Intensity: Get maximum...", "ppgb", 20041107)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Return value")
NORMAL ("the maximum value within the specified time domain, expressed in dB.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN ("Intensity: Get mean...", "ppgb", 20041107)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Return value")
NORMAL ("the mean (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG ("%%Averaging method")
DEFINITION ("the units in which the averaging is performed. If the method is #energy, "
	"the returned dB value is based on the mean power (in Pa^2/s) between %t__1_ and %t__2_. "
	"If the method is #dB, the returned value is the mean of the intensity curve in dB. "
	"If the method is #sones, the returned value is in between these two, "
	"and based on averaging properties of the human ear.")
ENTRY ("Algorithm")
NORMAL ("If the averaging method is #dB, the mean intensity between the times %t__1_ and %t__2_ is defined as")
FORMULA ("1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %x(%t) %dt")
NORMAL ("where %x(%t) is the intensity as a function of time, in dB. If the method is #energy, the result is")
FORMULA ("10 log__10_ { 1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ 10^^%x(%t)/10^ %dt }")
NORMAL ("If the method is #sones, the result is")
FORMULA ("10 log__2_ { 1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ 2^^%x(%t)/10^ %dt }")
ENTRY ("Behaviour")
NORMAL ("After you do @@Sound: To Intensity...@, the mean intensity of the resulting #Intensity, "
	"if the averaging method is #energy, should be close to the mean SPL of the original #Sound, "
	"which can be found with #Info.")
MAN_END

MAN_BEGIN ("Intensity: Get minimum...", "ppgb", 20041107)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Return value")
NORMAL ("the minimum value within a specified time domain, expressed in dB.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN ("Intensity: Get standard deviation...", "ppgb", 20041107)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Return value")
NORMAL ("the standard deviation (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
ENTRY ("Algorithm")
NORMAL ("The standard deviation between the times %t__1_ and %t__2_ is defined as")
FORMULA ("\\Vr {1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %dt (%x(%t) - %\\mu)^2}")
NORMAL ("where %x(%t) is the intensity (in dB) as a function of time, and %\\mu its mean. "
	"For our discrete Intensity object, the standard deviation is approximated by")
FORMULA ("\\Vr {1/(%n-1) \\su__%i=%m..%m+%n-1_ (%x__%i_ - %\\mu)^2}")
NORMAL ("where %n is the number of frames between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN ("Intensity: Get time of maximum...", "ppgb", 20041107)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Return value")
NORMAL ("the time (in seconds) associated with the maximum intensity within a specified time domain.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN ("Intensity: Get time of minimum...", "ppgb", 20041107)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Return value")
NORMAL ("the time (in seconds) associated with the minimum intensity within a specified time domain.")
ENTRY ("Settings")
TAG ("%%Time range% (s)")
DEFINITION ("the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN ("Intensity: Get value at time...", "ppgb", 20030916)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Return value")
NORMAL ("the intensity (in dB) at a specified time. If %time is outside the frames of the Intensity, the result is 0.")
ENTRY ("Arguments")
TAG ("%Time (s)")
DEFINITION ("the time at which the value is to be evaluated.")
TAG ("%%Interpolation")
DEFINITION ("the interpolation method, see @@vector value interpolation@. "
	"The standard is Cubic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN ("Intensity: Get value in frame...", "ppgb", 19991016)
INTRO ("A @query to the selected @Intensity object.")
ENTRY ("Argument")
TAG ("%%Frame number")
DEFINITION ("the frame whose value is to be looked up.")
ENTRY ("Return value")
NORMAL ("the intensity value (in dB) in the specified frame. "
	"If the index is less than 1 or greater than the number of frames, the result is 0; "
	"otherwise, it is %z [1] [%%frame number%].")
MAN_END

MAN_BEGIN ("Intensity: To IntensityTier", "ppgb", 19970321)
INTRO ("A command to convert each selected @Intensity object to an @IntensityTier.")
ENTRY ("Behaviour")
NORMAL ("Every sample in the @Intensity object is copied to a point on the @IntensityTier.")
ENTRY ("Postconditions")
DEFINITION ("Equal time domains:")
LIST_ITEM ("\\bu %result. %xmin == %intensity. %xmin")
LIST_ITEM ("\\bu %result. %xmax == %intensity. %xmax")
DEFINITION ("Equal number of points:")
LIST_ITEM ("\\bu %result. %points. %size == %intensity. %nx")
NORMAL ("For all points %i = 1 ... %intensity. %nx:")
DEFINITION ("   Explicit times:")
LIST_ITEM ("   \\bu %result. %points. %item [%i]. %time == %intensity. %x1 + (%i \\-- 1) * %intensity. %dx")
DEFINITION ("   Equal number of points:")
LIST_ITEM ("   \\bu %result. %points. %item [%i]. %value == %intensity. %z [1] [%i]")
MAN_END

MAN_BEGIN ("Intensity & TextTier: To IntensityTier...", "ppgb", 19970321)
INTRO ("A command to copy information from an @Intensity, at times specified by a @TextTier, "
	"to points on an @IntensityTier.")
ENTRY ("Behaviour")
NORMAL ("For all the times of the points in the TextTier, an intensity is computed from the "
	"information in the Intensity object, by linear interpolation.")
MAN_END

MAN_BEGIN ("IntensityTier", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. "
	"An IntensityTier object represents a time-stamped intensity contour, i.e., it contains a series of (%time, %intensity) points. "
	"The intensity values are in dB.")
NORMAL ("For examples, see @@Source-filter synthesis@.")
ENTRY ("IntensityTier commands")
NORMAL ("Creation:")
LIST_ITEM ("From scratch:")
LIST_ITEM ("\\bu @@Create IntensityTier...")
LIST_ITEM ("\\bu @@IntensityTier: Add point...")
LIST_ITEM ("Copy from another object:")
LIST_ITEM ("\\bu @@Intensity: To IntensityTier@: trivial copying of linearly spaced points.")
LIST_ITEM ("\\bu @@Intensity & TextTier: To IntensityTier...@: copying interpolated values at specified points.")
LIST_ITEM ("\\bu @@PointProcess: Up to IntensityTier...@: equal values at specified points.")
NORMAL ("Viewing and editing:")
LIST_ITEM ("\\bu @IntensityTierEditor")
NORMAL ("Conversion:")
LIST_ITEM ("\\bu @@IntensityTier: Down to PointProcess@: copy times.")
NORMAL ("Synthesis (see @@Source-filter synthesis@):")
LIST_ITEM ("\\bu @@Sound & IntensityTier: Multiply@")
NORMAL ("Queries:")
LIST_ITEM ("\\bu @@Get low index from time...")
LIST_ITEM ("\\bu @@Get high index from time...")
LIST_ITEM ("\\bu @@Get nearest index from time...")
NORMAL ("Modification:")
LIST_ITEM ("\\bu @@Remove point...")
LIST_ITEM ("\\bu @@Remove point near...")
LIST_ITEM ("\\bu @@Remove points between...")
LIST_ITEM ("\\bu @@IntensityTier: Add point...@")
MAN_END

MAN_BEGIN ("IntensityTier: Add point...", "ppgb", 20010410)
INTRO ("A command to add a point to each selected @IntensityTier.")
ENTRY ("Arguments")
TAG ("%Time (s)")
DEFINITION ("the time at which a point is to be added.")
TAG ("%Intensity (dB)")
DEFINITION ("the intensity value of the requested new point.")
ENTRY ("Behaviour")
NORMAL ("The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN ("IntensityTier: Down to PointProcess", "ppgb", 20010410)
INTRO ("A command to degrade every selected @IntensityTier to a @PointProcess.")
ENTRY ("Behaviour")
NORMAL ("The times of all the points are trivially copied, and so is the time domain. The intensity information is lost.")
MAN_END

MAN_BEGIN ("IntensityTierEditor", "ppgb", 20010330)
INTRO ("One of the @editors in the Praat program, for viewing and editing an @IntensityTier object. "
	"To create a IntensityTierEditor window, select an IntensityTier and click Edit.")
MAN_END

MAN_BEGIN ("Keyboard shortcuts", "ppgb", 20030316)
INTRO ("A way to accelerate the control of @Editors in P\\s{RAAT}.")
ENTRY ("Purpose")
NORMAL ("to choose a menu command with the keyboard. All of these commands can also be chosen "
	"from a menu.")
ENTRY ("Command key")
NORMAL ("When mentioning the %%Command key%, this manual refers to the key immediately to the left "
	"of the space bar. It is positioned in such a way that the most common keyboard shortcuts "
	"(Command-X: Cut, Command-C: Copy, Command-V: Paste, Command-Z: Undo, Command-Q: Quit, "
	"Command-W: Close) can be typed easily with one hand.")
NORMAL ("On Macintosh, this key is labelled with an Apple. On SGI, this key is labelled #Alt. ")
ENTRY ("Option key")
NORMAL ("When mentioning the %%Option key%, this manual refers to the key immediately to the left "
	"of the Command key. In Praat, this key is sometimes used together with the Command key "
	"for destructive actions that are the reverse of the actions invoked by using the Command key only. "
	"For instance, if Command-T means \"add a target at the cursor position\", "
	"Option-Command-T may mean \"remove the selected targets\".")
NORMAL ("On Macintosh, this key may be labelled #Option or #Alt. On SGI, this key is labelled #Ctrl. ")
MAN_END

MAN_BEGIN ("Log files", "ppgb", 20060804)
INTRO ("With some commands in the @Query menu of the @SoundEditor and @TextGridEditor, "
	"you can write combined information about times, pitch values, formants, and intensities "
	"to the @@Info window@ and to a log file.")
NORMAL ("A log file is a text file on disk. It consists of a number of similar lines, "
	"whose format you determine with the log settings in the Query menu.")
NORMAL ("Every time you press F12 (or choose ##Log 1# from the Query menu, "
	"Praat writes a line to log file 1. If you press Shift-F12, Praat writes a line to log file 2.")
NORMAL ("With the ##log settings# dialog, you determine the following:")
TAG ("%%Log 1 to Info window")
DEFINITION ("this determines whether your log line will be written to the Info window or not.")
TAG ("%%Log 1 to log file")
DEFINITION ("this determines whether your log line will be written to the log file or not.")
TAG ("%%Log file 1")
DEFINITION ("the name of the log file. On Windows, this has to be a complete path name, such as "
	"$$C:\\bsWINDOWS\\bsDESKTOP\\bsPitch Log.txt$. "
	"On Unix and MacOS X, it can either be a complete path name, e.g. $$/home/mary/pitch_log$, "
	"or a home-relative name such as $$~/Desktop/Pitch log$.")
TAG ("%%Log 1 format")
DEFINITION ("the format of the line that Praat will write. See below.")
NORMAL ("The same goes for log file 2.")
ENTRY ("Usage")
NORMAL ("The logging facility has been implemented in Praat especially for former users of Kay CSL, "
	"who have been used to doing it for years and like to continue doing it in Praat. "
	"Otherwise, you may prefer to use the @TextGridEditor to mark time points and run "
	"an automatic analysis afterwards.")
NORMAL ("If you do want to use the logging facility, you typically start by deleting any old "
	"log file (by choosing ##Delete log file 1# or ##Delete log file 2#), if you want to re-use "
	"the file name. Otherwise, you can change the log file name (with ##Log settings...#). "
	"After this, you will move the cursor to various time locations and press F12 (or Shift-F12) "
	"each time, so that information about the current time will be written to the log file.")
ENTRY ("Example 1: pitch logging")
NORMAL ("Suppose you want to log the time of the cursor and the pitch value at the cursor. "
	"You could use the following log format:")
CODE ("Time \'time:6\' seconds, pitch \'f0:2\' Hertz")
NORMAL ("If you now click at 3.456789876 seconds, and the pitch happens to be 355.266 Hertz "
	"at that time, the following line will be appended to the log file and/or to the Info window:")
CODE ("Time 3.456790 seconds, pitch 355.27 Hertz.")
NORMAL ("The parts \":6\" and \":2\" denote the number of digits after the decimal point. "
	"If you leave them out, the values will be written with a precision of 17 digits.")
NORMAL ("The words \'time\' and \'f0\' mean exactly the same as the result of the commands "
	"##Get cursor# and ##Get pitch#. Therefore, if instead of setting a cursor line you selected a larger "
	"piece of the sound, \'time\' will give the centre of the selection and \'f0\' will give the mean pitch "
	"in the selection.")
NORMAL ("Beware of the following pitfall: if your pitch units are not Hertz, but semitones, "
	"then \'f0\' will give the result in semitones. A format as in this example will then be misleading.")
ENTRY ("Example 2: formant logging")
NORMAL ("Suppose you want to log the start and finish of the selection, its duration, and the mean values "
	"of the first three formants, all separated by tab stops for easy importation into Microsoft\\re Excel\\tm. "
	"You could use the following log format:")
CODE ("\'t1:4\'\'tab\\$ \'\'t2:4\'\'tab\\$ \'\'f1:0\'\'tab\\$ \'\'f2:0\'\'tab\\$ \'\'f3:0\'")
NORMAL ("You see that \'t1\' and \'t2\' are the start and finish of the selection, respectively, "
	"and that they are written with 4 digits after the decimal point. By using \":0\", the three formant values "
	"are rounded to whole numbers in Hertz. The word \'tab\\$ \' is the tab stop.")
ENTRY ("Loggable values")
NORMAL ("The following values can be logged:")
LIST_ITEM ("\'time\': the time of the cursor, or the centre of the selection.")
LIST_ITEM ("\'t1\': the start of the selection (\"B\").")
LIST_ITEM ("\'t2\': the end of the selection (\"E\").")
LIST_ITEM ("\'dur\': the duration of the selection.")
LIST_ITEM ("\'freq\': the frequency at the frequency cursor.")
LIST_ITEM ("\'f0\': the pitch at the cursor time, or the mean pitch in the selection.")
LIST_ITEM ("\'f1\', \'f2\', \'f3\', \'f4\', \'f5\': the first/second/third/fourth/fifth formant at the cursor time, "
	"or the mean first/second/third/fourth/fifth formant in the selection.")
LIST_ITEM ("\'b1\', \'b2\', \'b3\', \'b4\', \'b5\': the bandwidth of the first/second/third/fourth/fifth formant "
	"at the cursor time or at the centre of the selection.")
LIST_ITEM ("\'intensity\': the intensity at the cursor time, or the mean intensity in the selection, in dB.")
LIST_ITEM ("\'power\': the spectral power at the cursor cross, in Pa^2/Hz.")
LIST_ITEM ("\'tab\\$ \': the tab stop.")
LIST_ITEM ("\'editor\\$ \': the title of the editor window (i.e. the name of the visible Sound or TextGrid).")
ENTRY ("More flexibility in logging")
NORMAL ("You may sometimes require information in your log file that cannot be generated directly "
	"by the loggable values above. Suppose, for instance, that you want to log the values for F1 and F2-F1 "
	"at the points where you click. You could write the following script:")
CODE ("f1 = Get first formant")
CODE ("f2 = Get second formant")
CODE ("f21 = f2 - f1")
CODE ("printline 'f1:0' 'f21:0'")
CODE ("fileappend \"D:\\bsPraat logs\\bsFormant log.txt\" 'f1:0''tab\\$ ''f21:0''newline\\$ '")
NORMAL ("With this script, the information would be appended both to the Info window and to the "
	"file \"Formant log.txt\" on your desktop.")
NORMAL ("You can make this script accessible with Option-F12 (or Command-F12) "
	"by saving the script and specifying the name of the script file in the ##Log script 3# (or #4) field "
	"in the ##Log settings...# dialog.")
NORMAL ("These scripts may take arguments. Suppose, for instance, that you want to specify a vowel symbol "
	"as you press Option-F12. The following script will take care of that:")
CODE ("form Save vowel and formants")
CODE1 ("word Vowel a")
CODE ("endform")
CODE ("f1 = Get first formant")
CODE ("f2 = Get second formant")
CODE ("f21 = f2 - f1")
CODE ("printline 'vowel\\$ ' 'f1:0' 'f21:0'")
CODE ("fileappend \"~/Praat logs/Vowels and formants log\" 'vowel\\$ ''f1:0''tab\\$ ''f21:0''newline\\$ '")
NORMAL ("Beware of the following pitfall: because of the nature of scripts, you should not try to do this "
	"when you have two editor windows with the same name. I cannot predict which of the two windows "
	"will answer the #Get queries...")
MAN_END

MAN_BEGIN ("Manipulation", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}, for changing the pitch and duration contours of a sound.")
ENTRY ("Inside a manipulation object")
NORMAL ("With @Inspect, you will see the following attributes:")
TAG ("%%timeStep%")
DEFINITION ("the time step (or %%frame length%) used in the pitch analysis. A common value is 0.010 seconds.")
TAG ("%%minimumPitch%")
DEFINITION ("the minimum pitch frequency considered in the pitch analysis. A common value is 75 Hertz.")
TAG ("%%maximumPitch%")
DEFINITION ("the maximum pitch frequency considered in the pitch analysis. A common value is 600 Hertz.")
NORMAL ("A Manipulation object also contains the following smaller objects:")
LIST_ITEM ("1. The original @Sound.")
LIST_ITEM ("2. A @PointProcess representing glottal pulses.")
LIST_ITEM ("3. A @PitchTier.")
LIST_ITEM ("4. A @DurationTier.")
ENTRY ("Analysis")
NORMAL ("When a Manipulation object is created from a sound, the following steps are performed:")
LIST_ITEM ("1. A pitch analysis is performed on the original sound, with the method of @@Sound: To Pitch...@. "
	"This uses the time step, minimum pitch, and maximum pitch parameters.")
LIST_ITEM ("2. The information of the resulting pitch contour (frequency and voiced/unvoiced decisions) "
	"is used to posit glottal pulses where the original sound contains much energy. "
	"The method is the same as in @@Sound & Pitch: To PointProcess (cc)@.")
LIST_ITEM ("3. The pitch contour is converted to a pitch tier with many points (targets), "
	"with the method of @@Pitch: To PitchTier@.")
LIST_ITEM ("4. An empty @DurationTier is created.")
ENTRY ("Resynthesis")
TAG ("A Manipulation object can produce Sound input. This Sound can be computed in several ways:")
LIST_ITEM ("\\bu @@overlap-add@: from original sound + pulses + pitch tier + duration tier;")
LIST_ITEM ("\\bu #LPC: from LPC (from original sound) + pulses + pitch tier;")
LIST_ITEM ("\\bu from the pulses only, as a pulse train or hummed;")
LIST_ITEM ("\\bu from the pitch tier only, as a pulse train or hummed.")
MAN_END

MAN_BEGIN ("Manipulation: Extract duration tier", "ppgb", 20010330)
INTRO ("A command to extract a copy of the duration information in each selected @Manipulation object into a new @DurationTier object.")
MAN_END

MAN_BEGIN ("Manipulation: Extract original sound", "ppgb", 20010330)
INTRO ("A command to copy the original sound in each selected @Manipulation object to a new @Sound object.")
MAN_END

MAN_BEGIN ("Manipulation: Extract pitch tier", "ppgb", 20010330)
INTRO ("A command to extract a copy of the pitch information in each selected @Manipulation object into a new @PitchTier object.")
MAN_END

MAN_BEGIN ("Manipulation: Extract pulses", "ppgb", 20010330)
INTRO ("A command to extract a copy of the vocal-pulse information in each selected @Manipulation object into a new @PointProcess object.")
MAN_END

MAN_BEGIN ("Manipulation: Play (overlap-add)", "ppgb", 20070722)
INTRO ("A command to play each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN ("Manipulation: Get resynthesis (overlap-add)", "ppgb", 20070722)
INTRO ("A command to extract the sound from each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN ("Manipulation: Replace duration tier", "ppgb", 20030216)
INTRO ("You can replace the duration tier that you see in your @Manipulation object "
	"with a separate @DurationTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create DurationTier...@.")
NORMAL ("To do this, select your Manipulation object together with the @DurationTier object and click ##Replace duration tier#.")
MAN_END

MAN_BEGIN ("Manipulation: Replace pitch tier", "ppgb", 20030216)
INTRO ("You can replace the pitch tier that you see in your @Manipulation object "
	"with a separate @PitchTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create PitchTier...@.")
NORMAL ("To do this, select your Manipulation object together with the @PitchTier object and click ##Replace pitch tier#.")
MAN_END

MAN_BEGIN ("Manipulation: Replace pulses", "ppgb", 20010330)
INTRO ("A command to replace the vocal-pulse information in the selected @Manipulation object with the selected @PointProcess object.")
MAN_END

MAN_BEGIN ("Manipulation: Replace original sound", "ppgb", 20010330)
INTRO ("A command to replace the original sound in the selected @Manipulation object with the selected @Sound object.")
MAN_END

MAN_BEGIN ("ManipulationEditor", "ppgb", 20030316)
	INTRO ("One of the @Editors in P\\s{RAAT}, for viewing and manipulating a @Manipulation object.")
ENTRY ("Objects")
	NORMAL ("The editor shows:")
	LIST_ITEM ("\\bu The original @Sound.")
	LIST_ITEM ("\\bu The @PointProcess that represents the glottal %pulses. "
		"You can edit it for improving the pitch analysis.")
	LIST_ITEM ("\\bu A pitch contour based on the locations of the pulses, for comparison (drawn as grey dots). "
		"Changes shape if you edit the pulses.")
	LIST_ITEM ("\\bu The @PitchTier that determines the pitch contour of the resynthesized @Sound (drawn as blue circles). "
		"At the creation of the @Manipulation object, it is computed from the original pitch contour. "
		"You can manipulate it by simplifying it (i.e., removing targets), "
		"or by moving parts of it up and down, and back and forth.")
	LIST_ITEM ("\\bu A @DurationTier for manipulating the relative durations of the voiced parts of the sound.")
ENTRY ("Playing")
	NORMAL ("To play (a part of) the %resynthesized sound (by any of the methods shown in the #Synth menu, "
		"like @@overlap-add@ and #LPC), @click on any of the 1 to 8 buttons below and above the drawing area "
		"or use the Play commands from the View menu.")
	NORMAL ("To play the %original sound instead, use ##Shift-click#.")
ENTRY ("Pulses")
	TAG ("To add:")
	DEFINITION ("#click at the desired time location, and choose ##Add pulse at cursor# or type ##Command-p#.")
	TAG ("To remove:")
	DEFINITION ("make a @@time selection@, and choose ##Remove pulse(s)# or type ##Option-Command-p#. "
		"If there is no selection, the pulse nearest to the cursor is removed.")
ENTRY ("Pitch points")
	TAG ("To add one at a specified %%time and frequency%:")
	DEFINITION ("#click at the desired time-frequency location, and choose ##Add pitch point at cursor# or type ##Command-t#.")
	TAG ("To add one at a specified %time only:")
	DEFINITION ("#click at the desired time, and choose ##Add pitch point at time slice#. ManipulationEditor tries to compute the frequency from the "
		"intervals between the pulses, basically by a median-of-three method.")
	TAG ("To remove:")
	DEFINITION ("make a @@time selection@, and choose ##Remove pitch point(s)# or type ##Option-Command-t#. "
		"If there is no selection, the pitch point nearest to the cursor is removed.")
	TAG ("To move %some:")
	DEFINITION ("make a @@time selection@ (the points become red) and ##Shift-drag# the points across the window. "
		"You cannot drag them across adjacent points, or below 50 Hz, or above the maximum frequency. "
		"You can only drag them horizontally if the %%dragging strategy% is ##All# or ##Only horizontal#, "
		"and you can drag them vertically if the dragging strategy is not ##Only horizontal#. "
		"You can change the dragging strategy with ##Set pitch dragging strategy...# from the #Pitch menu.")
	TAG ("To move %one:")
	DEFINITION ("@drag that point across the window. "
		"You can only drag it horizontally if the dragging strategy is not ##Only vertical#, "
		"and you can drag it vertically if the dragging strategy is not ##Only horizontal#.")
ENTRY ("Duration points")
	NORMAL ("Work pretty much the same as pitch points.")
ENTRY ("Stylization")
	NORMAL ("Before editing the Pitch points, you may want to reduce their number by choosing any of the #Stylize "
		"commands from the #Pitch menu.")
MAN_END

MAN_BEGIN ("Matrix", "ppgb", 20030216)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. "
	"A Matrix object represents a function %z (%x, %y) "
	"on the domain [%x__%min_, %x__%max_] \\xx [%y__%min_, %y__%max_]. "
	"The domain has been sampled in the %x and %y directions "
	"with constant sampling intervals (%dx and %dy) along each direction. "
	"The samples are thus %z [%i__%y_] [%i__%x_], %i__%x_ = 1 ... %n__%x_, %i__%y_ = 1 ... %n__%y_. "
	"The samples represent the function values %z (%x__1_ + (%ix - 1) %dx, %y__1_ + (%iy - 1) %dy).")
ENTRY ("Matrix commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Create Matrix...")
LIST_ITEM ("\\bu @@Create simple Matrix...")
LIST_ITEM ("\\bu @@Read from file...")
LIST_ITEM ("\\bu @@Read Matrix from raw text file...")
LIST_ITEM ("\\bu ##Read Matrix from LVS AP file...")
NORMAL ("Drawing:")
LIST_ITEM ("\\bu ##Matrix: Draw rows...")
LIST_ITEM ("\\bu ##Matrix: Draw contours...")
LIST_ITEM ("\\bu ##Matrix: Paint contours...")
LIST_ITEM ("\\bu ##Matrix: Paint cells...")
LIST_ITEM ("\\bu ##Matrix: Scatter plot...")
LIST_ITEM ("\\bu @@Matrix: Draw as squares...")
LIST_ITEM ("\\bu ##Matrix: Draw value distribution...")
LIST_ITEM ("\\bu ##Matrix: Paint surface...")
NORMAL ("Modification:")
LIST_ITEM ("\\bu @@Matrix: Formula...")
LIST_ITEM ("\\bu ##Matrix: Scale...")
ENTRY ("Inside a Matrix object")
NORMAL ("With @Inspect, you will see the following attributes.")
TAG ("%xmin, %xmax \\>_ %xmin")
DEFINITION ("%x domain.")
TAG ("%nx \\>_ 1")
DEFINITION ("number of columns.")
TAG ("%dx > 0.0")
DEFINITION ("distance between columns.")
TAG ("%x1")
DEFINITION ("%x value associated with first column.")
TAG ("%ymin, %ymax \\>_ %ymin")
DEFINITION ("%y domain.")
TAG ("%ny \\>_ 1")
DEFINITION ("number of rows.")
TAG ("%dy > 0.0")
DEFINITION ("distance between rows.")
TAG ("%y1")
DEFINITION ("%y value associated with first row.")
TAG ("%z [1..%ny] [1..%nx]")
DEFINITION ("The sample values.")
NORMAL ("After creation of the #Matrix, %xmin, %xmax, %ymin, %ymax, "
	"%nx, %ny, %dx, %dy, %x1, and %y1 "
	"do not usually change. The contents of %z do.")
NORMAL ("Normally, you will want %xmin \\<_ %x1 and %xmax \\>_ %x1 + (%nx - 1) %dx.")
ENTRY ("Example: simple matrix")
NORMAL ("If a simple matrix has %x equal to column number "
	"and %y equal to row number, it has the following attributes:")
LIST_ITEM ("%xmin = 1;   %xmax = %nx;   %dx = 1;  %x1 = 1;")
LIST_ITEM ("%ymin = 1;   %ymax = %ny;   %dy = 1;  %y1 = 1;")
ENTRY ("Example: sampled signal")
NORMAL ("If the matrix represents a sampled signal of 1 second duration "
	"with a sampling frequency of 10 kHz, it has the following attributes:")
LIST_ITEM ("%xmin = 0.0;   %xmax = 1.0;   %nx = 10000 ;   %dx = 1.0\\.c10^^-4^;   %x1 = 0.5\\.c10^^-4^;")
LIST_ITEM ("%ymin = 1;   %ymax = 1;   %ny = 1;   %dy = 1;   %y1 = 1;")
ENTRY ("Example: complex signal")
NORMAL ("If the matrix represents a complex spectrum "
	"derived with an @FFT from the sound of example 2, it has the following attributes:")
LIST_ITEM ("%xmin = 0.0;   %xmax = 5000.0;   %nx = 8193 ;   %dx = 5000.0 / 8192;   %x1 = 0.0;")
LIST_ITEM ("%ny = 2 (real and imaginary part);")
LIST_ITEM ("%ymin = 1 (first row, real part);")
LIST_ITEM ("%ymax = 2 (second row, imaginary part);")
LIST_ITEM ("%dy = 1;   %y1 = 1;  (so that %y is equal to row number)")
MAN_END

MAN_BEGIN ("Matrix: Draw as squares...", "ppgb", 19980319)
INTRO ("A command to draw a @Matrix object into the @@Picture window@.")
ENTRY ("Arguments")
TAG ("%Xmin, %Xmax")
DEFINITION ("the windowing domain in the %x direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Xmin \\>_ %Xmax), the entire %x domain [%x__%min_, %x__%max_] of the Matrix is used.")
TAG ("%Ymin, %Ymax")
DEFINITION ("the windowing domain in the %y direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Ymin \\>_ %Ymax), the entire %y domain [%y__%min_, %y__%max_] of the Matrix is used.")
TAG ("%Garnish")
DEFINITION ("determines whether axes are drawn around the picture. "
	"Turn this button off if you prefer to garnish your picture by yourself with the @Margins menu.")
ENTRY ("Behaviour")
NORMAL ("For every element of the Matrix inside the specified windowing domain, "
	"an opaque white or black rectangle is painted (white if the value of the element is positive, "
	"black if it is negative), surrounded by a thin black box. "
	"The %area of the rectangle is proportional to the value of the element.")
ENTRY ("Trick")
NORMAL ("If you prefer the %sides of the rectangle (instead of the area) to be proportional "
	"to the value of the element, you can use the formula \"$$self\\^ 2$\" before drawing (see @@Matrix: Formula...@).") 
MAN_END

MAN_BEGIN ("Matrix: Formula...", "ppgb", 20021206)
INTRO ("A command for changing the data in all selected @Matrix objects.")
NORMAL ("See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN ("Matrix: Paint cells...", "ppgb", 20021204)
INTRO ("A command to draw the contents of a @Matrix to the @@Picture window@.")
NORMAL ("Every cell of the matrix is drawn as a rectangle filled with a grey value between white (if the content "
	"of the cell is small) and black (if the content is large).")
MAN_END

/*
if x > 1 and x < 1.5 then self * 1.3 else self fi



je had gewoon de manual kunnen lezen (Help klikken bij Formula...).

>poging 1:
>  for col:=16000 to 24000 do {self[col] := self[col] * 1.3 }

geen lussen in formules. Er is een automatische lus:
for row := 1 to nrow do for col := 1 to ncol do self := ...

>poging 2:
>  col:=16000; for col:=16000 to 24000 do {self[col] := self[col] * 1.3 }

geen toekenningen. Als je het 1000ste element op 8 wilt zetten, kun je doen

Formula... if col=1000 then 8 else self fi

of sneller:

Set value... 1 1000 8

Wat dus wel werkt, maar ERG traag is:

for col = 16000 to 24000
   value = Get value... 1 col
   Set value... 1 col value*1.3
endfor

>toen ben ik het wilde weg gaan proberen:
>  col=16000; for col:=16000 to 24000 do {self[col] := self[col] * 1.3 }

de puntkomma betekent einde formule. Deze formule zet alles op 0,
behalve element 16000, dat op 1 gezet wordt. De expressie is namelijk booleaans:
"is col gelijk aan 16000"?

Dit staat allemaal in de on-line handleiding, dus je hoeft niet te gokken!
*/

MAN_BEGIN ("Matrix: Set value...", "ppgb", 19980319)
INTRO ("A command to change the value of one cell in each selected @Matrix object.")
ENTRY ("Arguments")
TAG ("%%Row number")
DEFINITION ("the number of the row of the cell whose value you want to change.")
TAG ("%%Column number")
DEFINITION ("the number of the column of the cell whose value you want to change.")
TAG ("%%New value")
DEFINITION ("the value that you want the specified cell to have.")
MAN_END

/*
1. The Hilbert transform.

I wondered whether my use of taking the square was worse than taking
the Hilbert envelope. My formula is

   my_envelope1(t) = x(t) ^ 2

For a sampled signal, this introduces higher frequencies, so I should have
filtered the signal at half the Nyquist frequency to prevent aliasing. I wonder
how the Hilbert envelope behaves in this respect:

   your_envelope1(t) = x(t) ^ 2 + Px(t) ^ 2

I think it has the same problem, since it involves squaring the real and imaginary parts.

Of course, the Hilbert transform is much smoother, but that cannot matter for
frequencies above 100 Hz or so, since we are going to filter at 30 Hz anyway.
Thus, I think these methods are comparable. Do you agree?


2. The non-linearity.

In the second step, I take the logarithm:

   my_envelope2(t) = 10 log10 (my_envelope1(t) + 1e-6)

Of course, the 1e-6 is there to guard against zero values in my_envelope1.
I took the logarithm because it can handle negative values, which will arise
in the filtering process. You take the square root instead:

   your_envelope2(t) = sqrt (your_envelope1(t))


3. Filtering between 3 and 30 Hz.

You use a second-order Butterworth filter in the time domain:

   your_envelope3(t) = your_envelope2(t) * Butterworth(t)

What do you mean by "forward and backward filtering"?
Did you use the second-order filter twice, thus making it fourth-order effectively?

My alternative was to use the two Gaussians:

   my_spectrum2(f) = fft (my_envelope2(t))
   my_spectrum3(f) = my_spectrum2(f) (exp(-(f/35)^2)-exp(-(f/3.5)^2)
   my_envelope3(t) = ifft (my_spectrum3 (f))

This is a filter with several desirable properties: no ringing, phase-preserving,
an area of zero. On the other hand, it is not causal, but I think that that is not
an issue here. Is it?


4. The new band.

In my algorithm, I have to undo the non-linearity:

   my_envelope4(t) = 10 ^ (my_envelope3(t) / 2)

You see that I divide by 2, not 10, thus effectively multiplying the dynamics,
expressed in dB, by a factor of 5. Since I am going to use the new envelope as
a multiplication factor, I will have to limit it to a factor of 10, because the
maximum enhancement is 20 dB. But this limiting factor is band-dependent:

   ceiling(Fmid) = 1 + 9 (1/2 - 1/2 cos (pi Fmid / 13))

where Fmid is expressed in Bark. The limiting is done smoothly:

   my_envelope5(t) = 1 / (1 / my_envelope4(t) + 1 / ceiling)

So this is the final multiplication factor for the band-filtered signal:

   new_band(t) = x(t) my_envelope5(t)

Your multiplication factor is very different. Please tell me whether I am correct.

   your_envelope4(t) = rectify (your_envelope3(t))

This step is needed because you cannot handle negative values. It introduces
higher-frequency components, which will have to be filtered out later.
The new signal will have to carry the new envelope, so the multiplication
factor is taken relative to the envelope of the original signal:

   your_envelope5(t) = your_envelope4(t) / your_envelope2(t)

The components that were introduced by the rectification must be thrown out:

   your_envelope6(t) = your_envelope5(t) * Butterworth?(t)

What were the characteristics of that filter? You did not specify them in your paper.
Well, the next step is normalization to the power of the original band:

   new_band(t) = your_envelope6(t) ||x(t)|| / ||your_envelope5(t)||

At least, that's your formula (12). I suppose that it contains a mistake, and that
the norm in the denominator should refer to the filtered your_envelope6(t) instead?

By the normalization, the fast movements within each band have been strengtened,
and the slow parts have been weakened. Thus, there is dynamic expansion within
each band, whereas my algorithm has this only in the mid frequency range, am I correct?
Could you also explain figure 5 to me? Some of the thick curves show enhanced peaks,
but how about the valleys, are some of them deepened as well?


5. The new signal.

My algorithm simply adds all the bands:

   new_signal(t) = SUM new_band(Fmid,t)

The mid-frequency bands are favoured because they have stronger modulation deepening
than the other bands. In your algorithm, the favouring of the mid frequencies is
implemented in the last step:

   new_signal(t) = SUM new_band(Fmid,t) gain(Fmid)

This would mean, for instance, that if the modulation is not deepened (for instance
if there are no fast movements), the mid-frequency range is still multiplied by a factor
of 10, whereas my algorithm would not change the signal at all in such a case. Is that so?

If so, could any of the learning results have been produced by the general emphasis
in the 1-4 kHz range? Have you any proof that the modulation deepening itself
is the cause of the learning results?

So the differences between the two algorithms are clear now: mine does modulation
deepening selectively in the F2 range, yours does modulation deepening everywhere
plus an independent emphasis in the F2 range. Since both algorithms will lead to
emphasis in the F2 range, that leaves the question of whether a modulation change
is necessary for the other frequency bands. Have you any idea, or was your choice
for this just one of the minor decisions while constructing your algorithm?

To sum up, I think that both algorithms perform what they were intended to do,
namely selectively enhancing fast F2 transitions. If you agree with this, I may
advise XXX to use my algorithm without change. I would, however, welcome any
suggestions as to oversights in my implementation, or other possible problems
or incorrect choices.
*/

MAN_BEGIN ("Matrix: To TableOfReal", "ppgb", 19991030)
INTRO ("A command to convert every selected @Matrix to a @TableOfReal.")
NORMAL ("This command is available from the #Cast menu. The resulting TableOfReal "
	"has the same number of rows and columns as the original Matrix, "
	"and the same data in the cells. However, it does not yet have any row or column "
	"labels; you can add those with some commands from the TableOfReal #Modify menu.")
MAN_END

MAN_BEGIN ("Modify", "ppgb", 20021204)
INTRO ("The title of a submenu of the @@dynamic menu@ for many object types. "
	"This submenu usually collects all the commands that can change the selected object.")
MAN_END

MAN_BEGIN ("PairDistribution", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. A "
	"PairDistribution object represents the relative probabilities with which "
	"the specified pairs of strings occur.")
ENTRY ("Class description")
TAG ("##struct-list# pairs")
DEFINITION ("a list of relative string-pair probabilities. Each element consists of:")
TAG1 ("#string %string1")
DEFINITION1 ("the first string.")
TAG1 ("#string %string2")
DEFINITION1 ("the second string.")
TAG1 ("#real %weight")
DEFINITION1 ("the relative probability associated with the string pair. This value cannot be negative.")
MAN_END

MAN_BEGIN ("PairDistribution: To Stringses...", "ppgb", 20030916)
INTRO ("A command to generate a number of string pairs from the selected @PairDistribution object. "
	"This command will create two aligned @Strings objects of equal size.")
ENTRY ("Arguments")
TAG ("%Number (standard: 1000)")
DEFINITION ("the number of the strings in either resulting Strings object.")
TAG ("%%Name of first Strings% (standard: \"input\")")
DEFINITION ("the name of the resulting Strings object associated with the first string of each pair.")
TAG ("%%Name of second Strings% (standard: \"output\")")
DEFINITION ("the name of the resulting Strings object associated with the second string of each pair.")
ENTRY ("Example")
NORMAL ("Suppose the PairDistribution contains the following:")
CODE ("4 pairs")
CODE ("\"at+ma\"  \"atma\"  100")
CODE ("\"at+ma\"  \"apma\"    0")
CODE ("\"an+pa\"  \"anpa\"   20")
CODE ("\"an+pa\"  \"ampa\"   80")
NORMAL ("The resulting Strings object \"input\" may then contain:")
FORMULA ("at+ma, an+pa, an+pa, at+ma, at+ma, an+pa, an+pa, an+pa, an+pa, at+ma, ...")
NORMAL ("The Strings object \"output\" may then contain:")
FORMULA ("atma,  ampa,  ampa,  atma,  atma,  ampa,  anpa,  ampa,  ampa,  atma, ...")
MAN_END

MAN_BEGIN ("ParamCurve", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of class #ParamCurve represents a sequence of time-stamped points (%x (%%t__i_), %y (%%t__i_)) "
	"in a two-dimensional space.")
MAN_END

MAN_BEGIN ("PointEditor", "ppgb", 20030316)
INTRO ("One of the @Editors in P\\s{RAAT}, for viewing and manipulating a @PointProcess object, "
	"which is optionally shown together with a @Sound object.")
ENTRY ("Objects")
NORMAL ("The editor shows:")
LIST_ITEM ("\\bu The @Sound, if you selected a Sound object together with the PointProcess object "
	"before you clicked \"Edit\".")
LIST_ITEM ("\\bu The @PointProcess; vertical blue lines represent the points.")
ENTRY ("Playing")
NORMAL ("To play (a part of) the %resynthesized sound (pulse train): "
	"@click on any of the 8 buttons below and above the drawing area, or choose a Play command from the View menu.")
NORMAL ("To play the %original sound instead, use @@Shift-click@.")
ENTRY ("Adding a point")
NORMAL ("@Click at the desired time location, and choose \"Add point at cursor\" or type ##Command-P#.")
ENTRY ("Removing points")
NORMAL ("To remove one or more points, "
	"make a @@time selection@ and choose ##Remove point(s)# from the ##Point# menu. "
	"If there is no selection, the point nearest to the cursor is removed.")
MAN_END

MAN_BEGIN ("PointProcess", "ppgb", 20030521)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("A PointProcess object represents a %%point process%, "
	"which is a sequence of %points %t__%i_ in time, defined on a domain [%t__%min_, %t__%max_]. "
	"The index %i runs from 1 to the number of points. The points are sorted by time, i.e. %t__%i+1_ > %t__%i_.")
ENTRY ("PointProcess commands")
NORMAL ("Creation from scratch:")
LIST_ITEM ("\\bu @@Create empty PointProcess...@")
LIST_ITEM ("\\bu @@Create Poisson process...@")
NORMAL ("Creation of a pulse train from a pitch contour:")
LIST_ITEM ("\\bu @@PitchTier: To PointProcess@: area-1 along entire time domain.")
LIST_ITEM ("\\bu @@Pitch: To PointProcess@: same, but excludes voiceless intervals.")
LIST_ITEM ("\\bu @@Sound & Pitch: To PointProcess (cc)@: \"pitch-synchronous\": near locations of high amplitude.")
LIST_ITEM ("\\bu @@Sound & Pitch: To PointProcess (peaks)...@: \"pitch-synchronous\": near locations of high amplitude.")
LIST_ITEM ("\\bu @@Sound: To PointProcess (periodic, cc)...@: near locations of high amplitude.")
LIST_ITEM ("\\bu @@Sound: To PointProcess (periodic, peaks)...@: near locations of high amplitude.")
NORMAL ("Creation from converting another object:")
LIST_ITEM ("\\bu ##Matrix: To PointProcess")
LIST_ITEM ("\\bu @@TextTier: Down to PointProcess@")
LIST_ITEM ("\\bu @@PitchTier: Down to PointProcess@")
LIST_ITEM ("\\bu @@IntensityTier: Down to PointProcess@")
NORMAL ("Hearing:")
LIST_ITEM ("\\bu @@PointProcess: Play@: pulse train.")
LIST_ITEM ("\\bu @@PointProcess: Hum@: pulse train with formants.")
NORMAL ("Drawing:")
LIST_ITEM ("\\bu @@PointProcess: Draw...@")
NORMAL ("Editing:")
LIST_ITEM ("\\bu ##PointProcess: Edit#: invokes a @PointEditor.")
LIST_ITEM ("\\bu ##PointProcess & Sound: Edit#: invokes a @PointEditor.")
LIST_ITEM ("\\bu Inside a @ManipulationEditor.")
NORMAL ("Queries:")
LIST_ITEM ("\\bu @@PointProcess: Get jitter (local)...@: periodic jitter.")
LIST_ITEM ("\\bu @@PointProcess: Get jitter (local, absolute)...@: periodic jitter.")
LIST_ITEM ("\\bu @@PointProcess: Get jitter (rap)...@: periodic jitter.")
LIST_ITEM ("\\bu @@PointProcess: Get jitter (ppq5)...@: periodic jitter.")
LIST_ITEM ("\\bu @@PointProcess: Get jitter (ddp)...@: periodic jitter.")
LIST_ITEM ("\\bu @@PointProcess: Get low index...@: index of nearest point not after specified time.")
LIST_ITEM ("\\bu @@PointProcess: Get high index...@: index of nearest point not before specified time.")
LIST_ITEM ("\\bu @@PointProcess: Get nearest index...@: index of point nearest to specified time.")
LIST_ITEM ("\\bu @@PointProcess: Get interval...@: duration of interval around specified time.")
NORMAL ("Set calculations:")
LIST_ITEM ("\\bu @@PointProcesses: Union@: the union of two point processes.")
LIST_ITEM ("\\bu @@PointProcesses: Intersection@: the intersection of two point processes.")
LIST_ITEM ("\\bu @@PointProcesses: Difference@: the difference of two point processes.")
NORMAL ("Modification:")
LIST_ITEM ("\\bu @@PointProcess: Add point...@: at a specified time.")
LIST_ITEM ("\\bu @@PointProcess: Remove point...@: at specified index.")
LIST_ITEM ("\\bu @@PointProcess: Remove point near...@: near specified time.")
LIST_ITEM ("\\bu @@PointProcess: Remove points...@: between specified indices.")
LIST_ITEM ("\\bu @@PointProcess: Remove points between...@: between specified times.")
NORMAL ("Analysis:")
LIST_ITEM ("\\bu @@PointProcess: To PitchTier...@: pitch values in interval centres.")
LIST_ITEM ("\\bu ##PointProcess & Sound: To Manipulation")
NORMAL ("Synthesis:")
LIST_ITEM ("\\bu @@PointProcess: To Sound (pulse train)...@")
LIST_ITEM ("\\bu @@PointProcess: To Sound (hum)...@")
NORMAL ("Conversion:")
LIST_ITEM ("\\bu ##PointProcess: To Matrix")
LIST_ITEM ("\\bu @@PointProcess: Up to TextTier...")
LIST_ITEM ("\\bu @@PointProcess: Up to PitchTier...")
LIST_ITEM ("\\bu @@PointProcess: Up to IntensityTier...")
MAN_END

MAN_BEGIN ("PointProcess: Add point...", "ppgb", 20010410)
INTRO ("A command to add a point to each selected @PointProcess.")
ENTRY ("Argument")
TAG ("%Time (s)")
DEFINITION ("the time at which a point is to be added.")
ENTRY ("Behaviour")
NORMAL ("The point process is modified so that it contains the new point. "
	"If a point at the specified time was already present in the point process, nothing happens.")
MAN_END

MAN_BEGIN ("PointProcesses: Difference", "ppgb", 20021212)
INTRO ("A command to compute the difference of two selected @PointProcess objects.")
ENTRY ("Behaviour")
NORMAL ("The resulting #PointProcess will contain only those points of the first selected original point process "
	"that do not occur in the second.")
NORMAL ("The time domain of the resulting point process is equal to the time domain of the first original point process.")
MAN_END

MAN_BEGIN ("PointProcess: Draw...", "ppgb", 20021212)
INTRO ("A command to draw every selected @PointProcess into the @@Picture window@.")
MAN_END

MAN_BEGIN ("PointProcess: Get high index...", "ppgb", 20021212)
INTRO ("A @query to the selected @PointProcess object.")
ENTRY ("Return value")
NORMAL ("the index of the nearest point at or after the specified time, "
	"0 if the point process contains no points, "
	"or a number higher than the number of points if the specified time is after the last point.")
ENTRY ("Argument")
TAG ("%Time (seconds)")
DEFINITION ("the time from which a point is looked for.")
MAN_END

MAN_BEGIN ("PointProcess: Get interval...", "ppgb", 20021212)
INTRO ("A @query to the selected @PointProcess object.")
ENTRY ("Return value")
NORMAL ("the duration of the interval around a specified time. "
	"if the point process contains no points or if the specified time falls before the first point "
	"or not before the last point, the value is @undefined. Otherwise, the result is the distance between "
	"the nearest points to the left and to the right of the specified time. "
	"If the point process happens to contain a point at exactly the specified time, "
	"the duration of the interval following this point is returned.")
ENTRY ("Argument")
TAG ("%Time (seconds)")
DEFINITION ("the time around which a point is looked for.")
MAN_END

MAN_BEGIN ("PointProcess: Get jitter (local)...", "ppgb", 20030521)
INTRO ("A @query to the selected @PointProcess object. See @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN ("PointProcess: Get jitter (local, absolute)...", "ppgb", 20030521)
INTRO ("A @query to the selected @PointProcess object. See @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN ("PointProcess: Get jitter (rap)...", "ppgb", 20030521)
INTRO ("A @query to the selected @PointProcess object. See @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN ("PointProcess: Get jitter (ppq5)...", "ppgb", 20030521)
INTRO ("A @query to the selected @PointProcess object. See @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN ("PointProcess: Get jitter (ddp)...", "ppgb", 20030521)
INTRO ("A @query to the selected @PointProcess object.")
ENTRY ("Return value")
NORMAL ("the periodic jitter, which is defined as the relative mean absolute "
	"third-order difference of the point process (= the second-order difference of the interval process):")
FORMULA ("%jitter = \\su__%i=2_^^%N-1^ |2%T__%i_ - %T__%i-1_ - %T__%i+1_|  /  \\su__%i=2_^^%N-1^ %T__%i_")
NORMAL ("where %T__%i_ is the %%i%th interval and %N is the number of intervals. "
	"If no sequences of three intervals can be found whose durations "
	"are between %%Shortest period% and %%Longest period%, the result is @undefined.")
ENTRY ("Arguments")
TAG ("%%Shortest period% (seconds)")
DEFINITION ("the shortest possible interval that will be considered. For intervals %T__%i_ shorter than this, "
	"the (%i-1)st, %%i%th, and (%i+1)st terms in the formula are taken as zero. "
	"This argument will normally be very small, say 0.1 ms.")
TAG ("%%Longest period% (seconds)")
DEFINITION ("the shortest possible interval that will be considered. For intervals %T__%i_ longer than this, "
	"the (%i-1)st, %%i%th, and (%i+1)st terms in the formula are taken as zero. "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 20 milliseconds; "
	"intervals longer than that will be considered voiceless.")
ENTRY ("Usage")
NORMAL ("The periodic jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
MAN_END

MAN_BEGIN ("PointProcess: Get low index...", "ppgb", 20021212)
INTRO ("A @query to the selected @PointProcess object.")
ENTRY ("Return value")
NORMAL ("the index of the nearest point before or at the specified time, "
	"or 0 if the point process contains no points or the specified time is before the first point.")
ENTRY ("Argument")
TAG ("%Time (seconds)")
DEFINITION ("the time from which a point is looked for.")
MAN_END

MAN_BEGIN ("PointProcess: Get nearest index...", "ppgb", 20021212)
INTRO ("A @query to the selected @PointProcess object.")
ENTRY ("Return value")
NORMAL ("the index of the point nearest to the specified time, "
	"or 0 if the point process contains no points.")
ENTRY ("Argument")
TAG ("%Time (seconds)")
DEFINITION ("the time around which a point is looked for.")
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

MAN_BEGIN ("PointProcess: Hum", "ppgb", 19970330)
INTRO ("A command to hear a @PointProcess.")
ENTRY ("Algorithm")
NORMAL ("A @Sound is created with the algorithm described at @@PointProcess: To Sound (hum)...@.")
NORMAL ("This sound is then played.")
MAN_END

MAN_BEGIN ("PointProcesses: Intersection", "ppgb", 20021212)
INTRO ("A command to merge two selected @PointProcess objects into one.")
ENTRY ("Behaviour")
NORMAL ("The resulting #PointProcess will contain only those points that occur in both original point processes.")
NORMAL ("The time domain of the resulting point process is the intersection of the time domains of the original point processes.")
MAN_END

MAN_BEGIN ("PointProcess: Play", "ppgb", 19970330)
INTRO ("A command to hear a @PointProcess.")
ENTRY ("Algorithm")
NORMAL ("A @Sound is created with the algorithm described at @@PointProcess: To Sound (pulse train)...@.")
NORMAL ("This sound is then played.")
MAN_END

MAN_BEGIN ("PointProcess: Remove point...", "ppgb", 20021212)
INTRO ("A command to remove a point from every selected @PointProcess.")
ENTRY ("Arguments")
TAG ("%Index")
DEFINITION ("the index of the point that is to be removed.")
ENTRY ("Behaviour")
NORMAL ("Does nothing if %index is less than 1 or greater than the number of points %nt in the point process. "
	"Otherwise, one point is removed (e.g., if %index is 3, the third point is removed), and the other points stay the same.")
MAN_END

MAN_BEGIN ("PointProcess: Remove point near...", "ppgb", 20021212)
INTRO ("A command to remove a point from every selected @PointProcess.")
ENTRY ("Arguments")
TAG ("%Time (seconds)")
DEFINITION ("the time around which a point is to be removed.")
ENTRY ("Behaviour")
NORMAL ("Does nothing if there are no points in the point process. "
	"Otherwise, the point nearest to %time is removed, and the other points stay the same.")
MAN_END

MAN_BEGIN ("PointProcess: Remove points...", "ppgb", 20021212)
INTRO ("A command to remove a range of points from every selected @PointProcess.")
ENTRY ("Arguments")
TAG ("%%From index% (\\>_ 1)")
DEFINITION ("the first index of the range of points that are to be removed.")
TAG ("%%To index%")
DEFINITION ("the last index of the range of points that are to be removed.")
ENTRY ("Behaviour")
NORMAL ("All points that originally fell in the range [%fromIndex, %toIndex] are removed, and the other points stay the same.")
MAN_END

MAN_BEGIN ("PointProcess: Remove points between...", "ppgb", 20021212)
INTRO ("A command to remove a range of points from every selected @PointProcess.")
ENTRY ("Arguments")
TAG ("%%From time% (seconds)")
DEFINITION ("the start of the domain from which all points are to be removed.")
TAG ("%%To time% (seconds)")
DEFINITION ("the end of the domain from which all points are to be removed.")
ENTRY ("Behaviour")
NORMAL ("All points that originally fell in the domain [%fromTime, %toTime], including the edges, are removed, "
	"and the other points stay the same.")
MAN_END

MAN_BEGIN ("PointProcess: To Sound (hum)...", "ppgb", 19970330)
INTRO ("A command to convert every selected @PointProcess into a @Sound.")
ENTRY ("Algorithm")
NORMAL ("A @Sound is created with the algorithm described at @@PointProcess: To Sound (pulse train)...@. "
	"This sound is then run through a sequence of second-order filters that represent five formants.")
MAN_END

MAN_BEGIN ("PointProcess: To Sound (phonation)...", "ppgb", 20070225)
INTRO ("A command to convert every selected @PointProcess into a @Sound.")
ENTRY ("Algorithm")
NORMAL ("A glottal waveform is generated at every point in the point process. "
	"Its shape depends on the settings %power1 and %power2 according to the formula")
FORMULA ("%U(%x) = %x^^%power1^ - %x^^%power2^")
NORMAL ("where %x is a normalized time that runs from 0 to 1 and %U(%x) is the normalized glottal flow in arbitrary units (the real unit is m^3/s). "
	"If %power1 = 2.0 and %power2 = 3.0, the glottal flow shape is that proposed by @@Rosenberg (1971)@, "
	"upon which for instance the Klatt synthesizer is based (@@Klatt & Klatt (1990)@):")
SCRIPT (4.5, 3,
	"Axes... 0 1 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (x^2-x^3)*6\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
NORMAL ("If %power1 = 3.0 and %power2 = 4.0, the glottal flow shape starts somewhat smoother, "
	"reflecting the idea that the glottis opens like a zipper:")
SCRIPT (4.5, 3,
	"Axes... 0 1 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (x^3-x^4)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
NORMAL ("For the generation of speech sounds, we do not take the glottal flow itself, "
	"but rather its derivative (this takes into account the influence of raditaion at the lips). "
	"The glottal flow derivative is given by")
FORMULA ("%dU(%x)%dx = %power1 %x^^(%power1-1)^ - %power2 %x^^(%power2-1)^")
NORMAL ("The flow derivative clearly shows the influence of the smoothing mentioned above. "
	"The unsmoothed curve, with %power1 = 2.0 and %power2 = 3.0, looks like:")
SCRIPT (4.5, 4,
	"Axes... 0 1 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (2*x-3*x^2)*6\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL ("Unlike the unsmoothed curve, the smoothed curve, with %power1 = 3.0 and %power2 = 4.0, starts out horizontally:")
SCRIPT (4.5, 4,
	"Axes... 0 1 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (3*x^2-4*x^3)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL ("Aother setting is the %%open phase%. If it is 0.70, the glottis will be open during 70 percent of a period. "
	"Suppose that the PointProcess has a pulse at time 0, at time 1, at time 2, and so on. The pulses at times 1 and 2 will then be turned "
	"into glottal flows starting at times 0.30 and 1.30:")
SCRIPT (4.5, 2.5,
	"Axes... 0 2 -0.1 1\n"
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
	"Axes... 0 2 -9 3\n"
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
NORMAL ("The final setting that influences the shape of the glottal flow is the %%collision phase%. "
	"If it is 0.03, for instance, the glottal flow derivative will not go abruptly to 0 at a pulse, "
	"but will instead decay by a factor of %e (\\~~ 2.7183) every 3 percent of a period. "
	"In order to keep the glottal flow curve smooth (and the derivative continuous), "
	"the basic shape discussed above has to be shifted slightly to the right and truncated "
	"at the time of the pulse, to be replaced there with the exponential decay curve; "
	"this also makes sure that the average of the derivative stays zero, as it was above "
	"(i.e. the area under the positive part of the curve equals the area above the negative part). "
	"This is what the curves look like if %power1 = 3.0, %power2 = 4.0, %openPhase = 0.70 and %collisionPhase = 0.03:")
SCRIPT (4.5, 2.5,
	"Axes... 0 2 -0.1 1\n"
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
	"Axes... 0 2 -9 3\n"
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
NORMAL ("These curves have moved 2.646 percent of a period to the right. At time 1, "
	"the glottal flow curve turns from a convex polynomial into a concave exponential, "
	"and the derivative still has its minimum there.")
ENTRY ("Arguments")
TAG ("%%Sampling frequency")
DEFINITION ("the sampling frequency of the resulting Sound object, e.g. 44100 Hertz.")
TAG ("%%Adaptation factor")
DEFINITION ("the factor by which a pulse height will be multiplied if the pulse time is not within "
	"%maximumPeriod from the previous pulse, and by which a pulse height will again be multiplied "
	"if the previous pulse time is not within %maximumPeriod from the pre-previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TAG ("%%Maximum period")
DEFINITION ("the minimal period that will be considered a silence, e.g. 0.05 seconds. "
	"Example: if %adaptationFactor is 0.6, and %adaptationTime is 0.02 s, "
	"then the heights of the first two pulses after silences of at least 20 ms "
	"will be multiplied by 0.36 and 0.6, respectively.")
MAN_END

MAN_BEGIN ("PointProcess: To Sound (pulse train)...", "ppgb", 20070225)
INTRO ("A command to convert every selected @PointProcess into a @Sound.")
ENTRY ("Algorithm")
NORMAL ("A pulse is generated at every point in the point process. This pulse is filtered at the Nyquist frequency "
	"of the resulting #Sound by converting it into a sampled #sinc function.")
ENTRY ("Arguments")
TAG ("%%Sampling frequency")
DEFINITION ("the sampling frequency of the resulting Sound object, e.g. 44100 Hertz.")
TAG ("%%Adaptation factor")
DEFINITION ("the factor by which a pulse height will be multiplied if the pulse time is not within "
	"%adaptationTime from the pre-previous pulse, and by which a pulse height will again be multiplied "
	"if the pulse time is not within %adaptationTime from the previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TAG ("%%Adaptation time")
DEFINITION ("the minimal period that will be considered a silence, e.g. 0.05 seconds.")
TAG ("%%Interpolation depth")
DEFINITION ("the extent of the sinc function to the left and to the right of the peak, e.g. 2000 samples.")
NORMAL ("Example: if %adaptationFactor is 0.6, and %adaptationTime is 0.02 s, "
	"then the heights of the first two pulses after silences of at least 20 ms "
	"will be multiplied by 0.36 and 0.6, respectively.")
MAN_END

MAN_BEGIN ("PointProcesses: Union", "ppgb", 20021212)
INTRO ("A command to merge two selected @PointProcess objects into one.")
ENTRY ("Behaviour")
NORMAL ("The resulting #PointProcess will contain all the points of the two original point processes, sorted by time. "
	"Points that occur in both original point processes, will occur only once in the resulting point process.")
NORMAL ("The time domain of the resulting point process is the union of the time domains of the original point processes.")
MAN_END

MAN_BEGIN ("PointProcess: Up to IntensityTier...", "ppgb", 19970329)
INTRO ("A command to promote every selected @PointProcess to an @IntensityTier.")
ENTRY ("Argument")
TAG ("%Intensity (dB)")
DEFINITION ("the intensity that will be associated with every point.")
ENTRY ("Behaviour")
NORMAL ("The times of all the points are trivially copied, and so is the time domain. "
	"The intensity information will be the same for every point.")
MAN_END

MAN_BEGIN ("PointProcess: Up to PitchTier...", "ppgb", 19970329)
INTRO ("A command to promote every selected @PointProcess to a @PitchTier.")
ENTRY ("Argument")
TAG ("%Frequency (Hz)")
DEFINITION ("the pitch frequency that will be associated with every point.")
ENTRY ("Behaviour")
NORMAL ("The times of all the points are trivially copied, and so is the time domain. "
	"The pitch information will be the same for every point.")
MAN_END

MAN_BEGIN ("Polygon", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("A Polygon object represents a sequence of points (%%x__i_, %%y__i_) in a two-dimensional space.")
MAN_END

MAN_BEGIN ("Read Matrix from raw text file...", "ppgb", 19980322)
INTRO ("A command to read a @Matrix object from a file on disk.")
ENTRY ("File format")
NORMAL ("The file should contain each row of the matrix on a separate line. Within each row, "
	"the elements must be separated by spaces or tabs.")
NORMAL ("For instance, the following text file will be read as a Matrix with three rows and four columns:")
CODE ("0.19 3 245 123")
CODE ("18e-6 -3e18 0 0.0")
CODE ("1.5 2.5 3.5 4.5")
NORMAL ("The resulting Matrix will have the same domain and sampling as Matrices created with "
	"##Create simple Matrix...#. In the above example, this means that the Matrix will have "
	"%x__%min_ = 0.5, %x__%max_ = 4.5, %n__%x_ = 4, %dx = 1.0, %x__1_ = 1.0, "
	"%y__%min_ = 0.5, %y__%max_ = 3.5, %n__%y_ = 3, %dy = 1.0, %y__1_ = 1.0.")
MAN_END

MAN_BEGIN ("Read Strings from raw text file...", "ppgb", 19990502)
INTRO ("A command to read a @Strings object from a simple text file. "
	"Each line is read as a separate string. See @Strings for an example.")
MAN_END

MAN_BEGIN ("Sound: To Intensity...", "ppgb", 20050830)
INTRO ("A command to create an @Intensity object from every selected @Sound.")
ENTRY ("Settings")
TAG ("%%Minimum pitch% (Hz)")
DEFINITION ("specifies the minimum periodicity frequency in your signal. If you set the minimum pitch too high, "
	"you will end up with a pitch-synchronous intensity modulation. If you set it too low, "
	"your intensity contour may appear smeared, so you should set it as high as allowed by the signal "
	"if you want a sharp contour.")
TAG ("%%Time step% (s)")
DEFINITION ("the time step of the resulting intensity contour. If you set it to zero, the time step is computed as "
	"one quarter of the effective window length, i.e. as 0.8 / (%minimum_pitch).")
TAG ("%%Subtract mean")
DEFINITION ("See @@Intro 6.2. Configuring the intensity contour@.")
ENTRY ("Algorithm")
NORMAL ("The values in the sound are first squared, then convolved with a Gaussian analysis window (Kaiser-20; sidelobes below -190 dB). "
	"The effective duration of this analysis window is 3.2 / (%minimum_pitch), which will guarantee that a periodic signal is analysed as having a "
	"pitch-synchronous intensity ripple not greater than our 4-byte floating-point precision (i.e., < 0.00001 dB).")
MAN_END

MAN_BEGIN ("Sound & IntensityTier: Multiply", "ppgb", 20000724)
INTRO ("A command to create a new Sound from the selected @Sound and @Intensity objects.")
NORMAL ("The resulting Sound equals the original sound, multiplied by a linear interpolation of the intensity. "
	"Afterwards, the resulting Sound is scaled so that its maximum absolute amplitude is 0.9.")
MAN_END

MAN_BEGIN ("Strings", "ppgb", 20041110)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. Represents an ordered list of strings.")
ENTRY ("Creation")
NORMAL ("The difficult way is to create a #Strings object from a generic Praat text file:")
CODE ("\"ooTextFile\"")
CODE ("\"Strings\"")
CODE ("5 ! number of strings")
CODE ("\"Hello\"")
CODE ("\"Goodbye\"")
CODE ("\"Auf wiedersehen\"")
CODE ("\"Tsch\\bsu\\\" \\\" ss\"")
CODE ("\"Arrivederci\"")
NORMAL ("In this example, we see that a double quote within a string should be written twice; "
	"the fourth string will therefore be read as ##Tsch\\bsu\\\" ss#, "
	"and will be shown in info messages or in graphical text as ##Tsch\\u\"ss# (see @@special symbols@). "
	"This file can be read simply with the generic @@Read from file...@ command from the Read menu.")
NORMAL ("An easier way is to use the special command @@Read Strings from raw text file...@. "
	"The file can then simply look like this:")
CODE ("Hello")
CODE ("Goodbye")
CODE ("Auf wiedersehen")
CODE ("Tsch\\bsu\\\" ss")
CODE ("Arrivederci")
NORMAL ("In this example, all the strings are in the generic system-independent ASCII format that is used "
	"everywhere in Praat (messages, graphical text) "
	"for @@special symbols@. You could also have supplied the strings in a native format, which is "
	"ISO-Latin1 encoding on Unix and Windows computers, or Mac encoding on Macintosh computers. "
	"The file would then have simply looked like:")
CODE ("Hello")
CODE ("Goodbye")
CODE ("Auf wiedersehen")
CODE ("Tsch\\u\"ss")
CODE ("Arrivederci")
NORMAL ("To convert this into the generic system-independent ASCII format, use the #Genericize command.")
NORMAL ("You can also create a #Strings object from a directory listing or from some other objects:")
LIST_ITEM ("\\bu @@Create Strings as file list...")
LIST_ITEM ("\\bu @@Distributions: To Strings...@")
LIST_ITEM ("\\bu @@OTGrammar: Generate inputs...@")
LIST_ITEM ("\\bu @@OTGrammar & Strings: Inputs to outputs...@")
MAN_END

MAN_BEGIN ("Strings: To Distributions", "ppgb", 19971025)
INTRO ("A command to analyse each selected @Strings object into a @Distributions object.")
NORMAL ("The resulting #Distributions will collect the occurrences of every string in the #Strings object, "
	"and put the number of occurrences in its first and only column.")
ENTRY ("Example")
NORMAL ("We start from the following #Strings:")
CODE ("6 (number of strings)")
CODE ("\"hallo\"")
CODE ("\"dag allemaal\"")
CODE ("\"hallo\"")
CODE ("\"tot morgen\"")
CODE ("\"hallo\"")
CODE ("\"tot morgen\"")
NORMAL ("This will give us the following #Distributions:")
CODE ("1 (number of columns) \"\" (no column name)")
CODE ("\"hallo\"         3")
CODE ("\"dag allemaal\"  1")
CODE ("\"tot morgen\"    2")
MAN_END

MAN_BEGIN ("Table", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. See the @Statistics tutorial.")
MAN_END

MAN_BEGIN ("TableOfReal", "ppgb", 20030316)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("A TableOfReal object contains a number of %cells. Each cell belongs to a %row and a %column. "
	"For instance, a TableOfReal with 10 rows and 3 columns has 30 cells.")
NORMAL ("Each row and each column may be labeled with a %title.")
ENTRY ("Creating a TableOfReal from data in a text file")
NORMAL ("Suppose you have F1 and F2 data for vowels. "
	"You can create a simple text file like the following:")
CODE ("\"ooTextFile\"  ! The line by which Praat can recognize your file")
CODE ("\"TableOfReal\" ! The line that tells Praat about the contents")
CODE ("2   \"F1\"  \"F2\"      ! Number of columns, and column labels")
CODE ("3                   ! Number of rows")
CODE ("\"a\" 800 1100         ! Row label (vowel), F1 value, F2 value")
CODE ("\"i\" 280 2800         ! Row label (vowel), F1 value, F2 value")
CODE ("\"u\" 260  560         ! Row label (vowel), F1 value, F2 value")
NORMAL ("Praat is rather forgiving about the use of spaces, tabs, and newlines. "
	"See @@Write to text file...@ for general information.")
NORMAL ("You will often have your data in a file with a self-describing format, "
	"i.e. in which the number of values on a line equals the number of columns "
	"of the table:")
CODE ("800 1100")
CODE ("280 2800")
CODE ("260 560")
NORMAL ("Such a file can be read with @@Read Matrix from raw text file...@. "
	"This creates a Matrix object, which can be cast to a TableOfReal object "
	"by @@Matrix: To TableOfReal@. The resulting TableOfReal does not have "
	"any row or column labels yet. You could add column labels with:")
CODE ("Set column label (index)... 1 F1")
CODE ("Set column label (index)... 2 F2")
NORMAL ("Of course, if the row labels contain crucial information, "
	"and the number of rows is large, this is not a feasible method.")
MAN_END

MAN_BEGIN ("TableOfReal: Set value...", "ppgb", 19980105)
INTRO ("A command to change the value of one table cell in each selected @TableOfReal object.")
ENTRY ("Arguments")
TAG ("%%Row number")
DEFINITION ("the number of the row of the cell whose value you want to change.")
TAG ("%%Column number")
DEFINITION ("the number of the column of the cell whose value you want to change.")
TAG ("%%New value")
DEFINITION ("the value that you want the specified cell to have.")
MAN_END

}

/* End of file manual_Fon.c */
