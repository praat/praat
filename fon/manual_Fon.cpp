/* manual_Fon.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
	static double x [] = { 0, 0.3, 0.6, 0.7, 0.9 }, y [] = { 1, 1, 2.3, 1, 1 };
	Graphics_setWindow (g, 0, 0.9, 0, 2.5);
	Graphics_drawInnerBox (g);
	Graphics_marksLeftEvery (g, 1, 1, TRUE, TRUE, FALSE);
	Graphics_marksBottomEvery (g, 1, 0.1, TRUE, TRUE, FALSE);
	Graphics_setInner (g);
	Graphics_setColour (g, Graphics_RED);
	Graphics_polyline (g, 5, x, y);
	Graphics_setColour (g, Graphics_BLACK);
	Graphics_unsetInner (g);
	Graphics_textBottom (g, TRUE, L"Time (s)");
	Graphics_textLeft (g, TRUE, L"Relative duration");
}

void manual_Sampling_init (ManPages me);
void manual_sound_init (ManPages me);
void manual_pitch_init (ManPages me);
void manual_spectrum_init (ManPages me);
void manual_formant_init (ManPages me);
void manual_annotation_init (ManPages me);

void manual_Fon_init (ManPages me);
void manual_Fon_init (ManPages me) {

manual_Sampling_init (me);
manual_sound_init (me);
manual_pitch_init (me);
manual_spectrum_init (me);
manual_formant_init (me);
manual_annotation_init (me);

MAN_BEGIN (L"Get high index from time...", L"ppgb", 20101230)
INTRO (L"A @query to ask the selected tier object "
	"(@DurationTier, @IntensityTier, @PitchTier) "
	"which point is nearest to, but no earlier than, the specified time.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time from which you want to get the point index.")
ENTRY (L"Return value")
NORMAL (L"This query returns the index of the point with the lowest time greater than or equal to #Time. "
	"It is @undefined if there are no points. "
	"It is the number of points plus 1 (offright) if the specified time is greater than the time of the last point.")
MAN_END

MAN_BEGIN (L"Get low index from time...", L"ppgb", 20101230)
INTRO (L"A @query to ask the selected tier object "
	"(@DurationTier, @IntensityTier, @PitchTier) "
	"which point is nearest to, but no later than, the specified time.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time from which you want to get the point index.")
ENTRY (L"Return value")
NORMAL (L"This query returns the index of the point with the highest time less than or equal to #Time. "
	"It is @undefined if there are no points. "
	"It is 0 (offleft) if the specified time is less than the time of the first point.")
MAN_END

MAN_BEGIN (L"Get nearest index from time...", L"ppgb", 20101230)
INTRO (L"A @query to ask the selected tier object "
	"(@DurationTier, @IntensityTier, @PitchTier) "
	"which point is nearest to the specified time.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time near which you want to get the point index.")
ENTRY (L"Return value")
NORMAL (L"This query returns the index of the point with the highest time less than or equal to #Time. "
	"It is @undefined if there are no points.")
MAN_END

MAN_BEGIN (L"Remove point...", L"ppgb", 20101230)
INTRO (L"A command to remove one point from every selected time-based tier object "
	"(@DurationTier, @IntensityTier, @PitchTier).")
ENTRY (L"Setting")
TAG (L"##Point number")
DEFINITION (L"the index of the point you want to remove.")
ENTRY (L"Behaviour")
NORMAL (L"If ##Point number# is 3, the third point counted from the start of the tier (if it exists) "
	"is removed from the tier.")
MAN_END

MAN_BEGIN (L"Remove point near...", L"ppgb", 20101230)
INTRO (L"A command to remove one point from every selected time-based tier object "
	"(@DurationTier, @IntensityTier, @PitchTier).")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time near which you want to remove a point.")
ENTRY (L"Behaviour")
NORMAL (L"The point nearest to #Time (if there is any point) is removed from the tier.")
MAN_END

MAN_BEGIN (L"Remove points between...", L"ppgb", 20101230)
INTRO (L"A command to remove some points from every selected time-based tier object "
	"(@DurationTier, @IntensityTier, @PitchTier).")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the times between which you want to remove all points.")
ENTRY (L"Behaviour")
NORMAL (L"Any points between ##Frome time# and ##To Time# (inclusive) are removed from the tier.")
MAN_END

MAN_BEGIN (L"AmplitudeTier", L"ppgb", 20070825)
INTRO (L"One of the @@types of objects@ in Praat. "
	"An AmplitudeTier object represents a time-stamped amplitude contour, i.e., it contains a series of (%time, %amplitude) points. "
	"The amplitude values are in Pascal. To see some applications, consult the @IntensityTier information; "
	"the difference between an AmplitudeTier and an IntensityTier is that the former has values in Pascal "
	"which multiply linearly with a Sound (for instance), and the latter has values in dB, "
	"which multiply logarithmically with a Sound.")
MAN_END

MAN_BEGIN (L"Cochleagram", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat. It represents the excitation pattern "
	"of the basilar membrane in the inner ear (see @Excitation) as a function of time.")
MAN_END

MAN_BEGIN (L"Cochleagram: Formula...", L"ppgb", 20021206)
INTRO (L"A command for changing the data in all selected @Cochleagram objects.")
NORMAL (L"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (L"Create DurationTier...", L"ppgb", 20021204)
INTRO (L"A command in the @@New menu@ to create an empty @DurationTier object.")
NORMAL (L"The resulting object will have the specified name and time domain, but contain no duration points. "
	"To add some points to it, use @@DurationTier: Add point...@.")
ENTRY (L"Scripting example")
NORMAL (L"To create a tier 0.9 seconds long, with an deceleration around 0.6 seconds, you do:")
CODE (L"Create DurationTier... dur 0 0.9")
CODE (L"Add point... 0.3 1")
CODE (L"Add point... 0.6 2.3")
CODE (L"Add point... 0.7 1")
NORMAL (L"The result will look like")
PICTURE (5, 2.5, draw_CreateDurationTier)
NORMAL (L"The target duration will be the area under this curve, which is 0.9 + 1/2 \\.c 1.3 \\.c 0.4 = 1.16 seconds.")
MAN_END

MAN_BEGIN (L"Create empty PointProcess...", L"ppgb", 20021204)
INTRO (L"A command in the @@New menu@ to create an empty @PointProcess. The newly created object is put in the list of objects.")
MAN_END

MAN_BEGIN (L"Create IntensityTier...", L"ppgb", 20021204)
INTRO (L"A command in the @@New menu@ to create an empty @IntensityTier object.")
NORMAL (L"The resulting object will have the specified name and time domain, but contain no formant points. "
	"To add some points to it, use @@IntensityTier: Add point...@.")
NORMAL (L"For an example, see @@Source-filter synthesis@.")
MAN_END

MAN_BEGIN (L"Create Matrix...", L"ppgb", 20021212)
INTRO (L"A command in the @@New menu@ to create a @Matrix with the specified sampling attributes, "
	"filled with values from a formula (see @@Matrix: Formula...@).")
MAN_END

MAN_BEGIN (L"Create Poisson process...", L"ppgb", 20041005)
INTRO (L"A command to create a @PointProcess object that represents a Poisson process.")
NORMAL (L"A Poisson process is a stationary point process with a fixed density %\\la, "
	"which means that there are, on the average, %\\la events per second.")
ENTRY (L"Settings")
TAG (L"##Start time (s)")
DEFINITION (L"%t__%min_, the beginning of the time domain, in seconds.")
TAG (L"##End time (s)")
DEFINITION (L"%t__%max_, the end of the time domain, in seconds.")
TAG (L"##Density (Hz)")
DEFINITION (L"the average number of points per second.")
ENTRY (L"Algorithm")
NORMAL (L"First, the number of points %N in the time domain is determined. Its expectation value is")
FORMULA (L"%\\la = (%t__%max_ \\-- %t__%min_) \\.c %density")
NORMAL (L"but its actual value is taken from the Poisson distribution:")
FORMULA (L"%p(%n) = (%%\\la^n% / %n!) %e^^\\--%\\la")
NORMAL (L"Then, %N points are computed throughout the time domain, according to a uniform distribution:")
FORMULA (L"%p(%t) = 1 / (%t__%max_ \\-- %t__%min_)   for %t \\e= [%t__%min_, %t__%max_]")
FORMULA (L"%p(%t) = 0   outside [%t__%min_, %t__%max_]")
MAN_END

MAN_BEGIN (L"Create simple Matrix...", L"ppgb", 20021204)
INTRO (L"A command in the @@New menu@ to create a @Matrix with the specified number of rows and columns, "
	"filled with values from a formula (see @@Matrix: Formula...@).")
MAN_END

MAN_BEGIN (L"Create Strings as directory list...", L"ppgb", 20060919)
INTRO (L"A command in the @@New menu@ to create a @Strings object containing a list of directories in a given parent directory. "
	"It works completely analogously to @@Create Strings as file list...@.")
MAN_END

MAN_BEGIN (L"Create Strings as file list...", L"ppgb", 20130521)
INTRO (L"A command in the @@New menu@ to create a @Strings object containing a list of files in a given directory.")
ENTRY (L"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (2.6), L""
	Manual_DRAW_SETTINGS_WINDOW ("Create Strings as file list", 2.6)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Name", "fileList")
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("File path", "/Users/miep/Sounds/*.wav")
)
TAG (L"##Name")
DEFINITION (L"the name of the resulting Strings object.")
TAG (L"##File path")
DEFINITION (L"the directory name, with an optional %wildcard (see below) for selecting files.")
ENTRY (L"Behaviour")
NORMAL (L"The resulting Strings object will contain an alphabetical list of file names, "
	"without the preceding path through the directory structures. If there are no files that match the file path, "
	"the Strings object will contain no strings.")
ENTRY (L"Usage")
NORMAL (L"There are two ways to specify the file path.")
NORMAL (L"One way is to specify a directory name only. On Unix, the file path could be "
	"##/usr/people/miep/sounds# or ##/usr/people/miep/sounds/#, for instance. On Windows, "
	"##C:\\bsDocuments and Settings\\bsMiep\\bsSounds# or ##C:\\bsDocuments and Settings\\bsMiep\\bsSounds\\bs#. "
	"On Macintosh, ##/Users/miep/Sounds# or ##/Users/miep/Sounds/#. Any of these produce "
	"a list of all the files in the specified directory.")
NORMAL (L"The other way is to specify a wildcard (a single asterisk) for the file names. "
	"To get a list of all the files whose names start with \"hal\" and end in \".wav\", "
	"type ##/usr/people/miep/sounds/hal*.wav#, ##C:\\bsDocuments and Settings\\bsMiep\\bsSounds\\bshal*.wav#, "
	"or ##/Users/miep/Sounds/hal*.wav#.")
ENTRY (L"Script usage")
NORMAL (L"In a script, you can use this command to cycle through the files in a directory. "
	"For instance, to read in all the sound files in a specified directory, "
	"you could use the following script:")
CODE (L"directory\\$  = \"/usr/people/miep/sounds\"")
CODE (L"strings = do (\"Create Strings as file list...\", \"list\", directory\\$  + \"/*.wav\")")
CODE (L"numberOfFiles = do (\"Get number of strings\")")
CODE (L"for ifile to numberOfFiles")
	CODE1 (L"selectObject (strings)")
	CODE1 (L"fileName\\$  = do\\$  (\"Get string...\", ifile)")
	CODE1 (L"do (\"Read from file...\", directory\\$  + \"/\" + fileName\\$ )")
CODE (L"endfor")
NORMAL (L"If the script has been saved to a script file, you can use file paths that are relative to the directory "
	"where you saved the script. Thus, with")
CODE (L"do (\"Create Strings as file list...\", \"list\", \"*.wav\")")
NORMAL (L"you get a list of all the .wav files that are in the same directory as the script that contains this line. "
	"And to get a list of all the .wav files in the directory Sounds that resides in the same directory as your script, "
	"you can do")
CODE (L"do (\"Create Strings as file list...\", \"list\", \"Sounds/*.wav\")")
NORMAL (L"As is usual in Praat scripting, the forward slash (\"/\") in this example can be used on all platforms, including Windows. "
	"This makes your script portable across platforms.")
ENTRY (L"See also")
NORMAL (L"To get a list of directories instead of files, use @@Create Strings as directory list...@.")
MAN_END

MAN_BEGIN (L"Distributions", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat. Inherits most actions from @TableOfReal.")
ENTRY (L"Actions")
LIST_ITEM (L"@@Distributions: To Strings...@")
MAN_END

MAN_BEGIN (L"Distributions: To Strings...", L"ppgb", 19971022)
INTRO (L"A command to create a @Strings object from every selected @Distributions object.")
ENTRY (L"Settings")
TAG (L"##Column number")
DEFINITION (L"the column (in the #Distributions object) that contains the distribution that you are "
	"interested in. Often the #Distributions object will only contain a single distribution, "
	"so this argument will often be 1. If the #Distributions object contains nine distributions, "
	"specify any number between 1 and 9.")
TAG (L"##Number of strings")
DEFINITION (L"the number of times a string will be drawn from the chosen distribution. "
	"This is the number of strings that the resulting @Strings object is going to contain.")
ENTRY (L"Behaviour")
NORMAL (L"Every string in the resulting #Strings object will be a row label of the #Distributions object. "
	"The number in each row at the specified column will be considered the relative frequency of "
	"occurrence of that row.")
NORMAL (L"%#Example. Suppose we have the following #Distributions:")
CODE (L"File type = \"ooTextFile\"")
CODE (L"Object class = \"Distributions\"")
CODE (L"2 (number of columns)")
CODE (L"         \"English\"  \"French\" (column labels)")
CODE (L"3 (number of rows)")
CODE (L"\"the\"     108        1.5")
CODE (L"\"a\"       58.1       33")
CODE (L"\"pour\"    0.7        15.5")
NORMAL (L"If you set %Column to 1 and %%Number of strings% to 1000, "
	"you will get a @Strings object with approximately 647 occurrences of \"the\", "
	"348 occurrences of \"a\", and 4 occurrences of \"pour\". "
	"If you had set %Column to 2 (\"French\"), you would have gotten about "
	"30 times \"the\", 660 times \"a\", and 310 times \"pour\". "
	"The actual numbers will vary because the choice of a string will not depend on previous choices.")
MAN_END

MAN_BEGIN (L"DurationTier", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat. "
	"A DurationTier object contains a number of (%time, %duration) points, "
	"where %duration is to be interpreted as a relative duration (e.g. the duration of a manipulated sound "
	"as compared to the duration of the original). For instance, "
	"if your DurationTier contains two points, one with a duration value of 1.5 at a time of 0.5 seconds "
	"and one with a duration value of 0.6 at a time of 1.1 seconds, this is to be interpreted as "
	"a relative duration of 1.5 (i.e. a slowing down) for all original times before 0.5 seconds, "
	"a relative duration of 0.6 (i.e. a speeding up) for all original times after 1.1 seconds, "
	"and a linear interpolation between 0.5 and 1.1 seconds (e.g. a relative duration of "
	"1.2 at 0.7 seconds, and of 0.9 at 0.9 seconds).")
NORMAL (L"See @@Intro 8.2. Manipulation of duration@ and @@Create DurationTier...@.")
MAN_END

MAN_BEGIN (L"DurationTier: Add point...", L"ppgb", 20030216)
INTRO (L"A command to add a point to each selected @DurationTier. "
	"For an example, see @@Create DurationTier...@.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time at which a point is to be added.")
TAG (L"##Relative duration")
DEFINITION (L"the relative duration value of the requested new point.")
ENTRY (L"Behaviour")
NORMAL (L"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (L"DurationTier: Get target duration...", L"ppgb", 20101228)
INTRO (L"A @query to the selected @DurationTier for the target duration of a specified time range.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the start and end of the (original) time range.")
ENTRY (L"Return value")
NORMAL (L"the target duration in seconds.")
MAN_END

MAN_BEGIN (L"DurationTierEditor", L"ppgb", 20110128)
INTRO (L"One of the @editors in the Praat program, for viewing and editing a @DurationTier object. "
	"To create a DurationTierEditor window, select a DurationTier and click ##View & Edit#.")
MAN_END

MAN_BEGIN (L"Editors", L"ppgb", 20110128)
INTRO (L"Many @@types of objects@ in Praat can be viewed and edited in their own windows.")
ENTRY (L"Editor windows")
LIST_ITEM (L"\\bu @SoundEditor")
LIST_ITEM (L"\\bu @LongSoundEditor")
LIST_ITEM (L"\\bu @TextGridEditor")
LIST_ITEM (L"\\bu @ManipulationEditor")
LIST_ITEM (L"\\bu @SpectrumEditor")
LIST_ITEM (L"\\bu @PitchEditor")
LIST_ITEM (L"\\bu @PointEditor")
LIST_ITEM (L"\\bu @PitchTierEditor")
LIST_ITEM (L"\\bu @IntensityTierEditor")
LIST_ITEM (L"\\bu @DurationTierEditor")
LIST_ITEM (L"\\bu #SpectrogramEditor")
LIST_ITEM (L"\\bu #ArtwordEditor")
LIST_ITEM (L"\\bu @OTGrammarEditor")
LIST_ITEM (L"\\bu (any type: @Inspect)")
ENTRY (L"How to open an editor for an object")
NORMAL (L"To open an editor window for an object in the list, select the object and choose ##View & Edit# "
	"(if the ##View & Edit# button exists, it is usually at the top of the @@Dynamic menu@). "
	"The name of the object will appear as the title of the editor window.")
NORMAL (L"Objects that cannot be modified (e.g. LongSound) just have the command #View instead of ##View & Edit#.")
ENTRY (L"General behaviour")
NORMAL (L"Changes that you make to an object in its editor window will take effect immediately. "
	"For instance, you do not have close the editor window before saving the changed object to disk.")
NORMAL (L"If you @Remove an object that you are viewing or editing from the @@List of Objects@, "
	"the editor window will automatically disappear from the screen.")
NORMAL (L"All editors are independent windows: you can minimize and maximize them; "
	"if an editor window goes hiding behind another window, "
	"you can raise it by choosing the ##View & Edit# command again.")
NORMAL (L"If you rename an object that you are viewing or editing (with @@Rename...@), "
	"the title of the editor window immediately changes to the new name.")
ENTRY (L"Ways to control an editor window")
LIST_ITEM (L"\\bu @@Click")
LIST_ITEM (L"\\bu @@Shift-click")
LIST_ITEM (L"\\bu @@Drag")
LIST_ITEM (L"\\bu @@Shift-drag")
LIST_ITEM (L"\\bu @@Time selection")
LIST_ITEM (L"\\bu @@Keyboard shortcuts")
MAN_END

MAN_BEGIN (L"Excitation", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat. It represents the excitation pattern "
	"of the basilar membrane in the inner ear.")
ENTRY (L"Inside an Excitation object")
NORMAL (L"With @Inspect, you will see the following attributes.")
TAG (L"%xmin = 0")
DEFINITION (L"minimum place or frequency (Bark).")
TAG (L"%xmax = 25.6 Bark")
DEFINITION (L"maximum place or frequency (Bark).")
TAG (L"%nx")
DEFINITION (L"number of places or frequencies.")
TAG (L"%dx = 25.6 / %nx")
DEFINITION (L"Place or frequency step (Bark).")
TAG (L"%x1 = %dx / 2")
DEFINITION (L"centre of first place or frequency band (Bark).")
TAG (L"%ymin = %ymax = %dy = %y__1_ = 1; %ny = 1")
DEFINITION (L"dummies.")
TAG (L"%z [1]")
DEFINITION (L"intensity (sensation level) in phon.")
MAN_END

MAN_BEGIN (L"Excitation: Formula...", L"ppgb", 20021206)
INTRO (L"A command for changing the data in all selected @Excitation objects.")
NORMAL (L"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (L"Excitation: Get loudness", L"ppgb", 19991016)
INTRO (L"A @query to ask the selected @Excitation object for its loudness.")
ENTRY (L"Return value")
NORMAL (L"the loudness in sone units.")
ENTRY (L"Algorithm")
NORMAL (L"The loudness is defined as")
FORMULA (L"\\in%df 2^^(%e(%f) - 40 phon) / 10^")
NORMAL (L"where %f is the frequency in Bark, and %e(%f) the excitation in phon. "
	"For our discrete Excitation object, the loudness is computed as")
FORMULA (L"\\De%f \\su 2^^(%e__%i_ - 40) / 10")
NORMAL (L"where \\De%f is the distance between the excitation channels (in Bark).")
MAN_END

MAN_BEGIN (L"Excitation_hertzToBark", L"ppgb", 19970401)
INTRO (L"A routine for converting frequency into basilar place, "
	"the inverse of @Excitation_barkToHertz.")
ENTRY (L"Syntax")
PROTOTYPE (L"##double Excitation_hertzToBark (double #%hertz##);")
ENTRY (L"Algorithm")
NORMAL (L"Returns 7 \\.c ln (%hertz / 650 + \\Vr (1 + (%hertz / 650)^2)).")
MAN_END

MAN_BEGIN (L"Excitation_barkToHertz", L"ppgb", 19970401)
INTRO (L"A routine for converting basilar place into frequency, "
	"the inverse of @Excitation_hertzToBark.")
ENTRY (L"Syntax")
PROTOTYPE (L"##double Excitation_barkToHertz (double #%bark##);")
ENTRY (L"Algorithm")
NORMAL (L"Returns 650 \\.c sinh (%bark / 7).")
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

MAN_BEGIN (L"Formula...", L"ppgb", 19980319)
NORMAL (L"See @@Matrix: Formula...@")
MAN_END

MAN_BEGIN (L"Frequency selection", L"ppgb", 20010402)
INTRO (L"The way to select a frequency domain in the @SpectrumEditor. "
	"This works completely analogously to the @@time selection@ in other editors.")
MAN_END

MAN_BEGIN (L"Get area...", L"ppgb", 20030216)
INTRO (L"A @query to the selected tier object (@PitchTier, @IntensityTier, @DurationTier).")
ENTRY (L"Return value")
NORMAL (L"the area under the curve.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
TAG (L"##To time (s)")
DEFINITION (L"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
ENTRY (L"Algorithm")
NORMAL (L"The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the area is")
FORMULA (L"1/2 (%f__1_ + %f__2_) (%t__2_ \\-- %t__1_)")
MAN_END

MAN_BEGIN (L"Intensity", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"An Intensity object represents an intensity contour at linearly spaced time points "
	"%t__%i_ = %t__1_ + (%i \\-- 1) %dt, with values in dB SPL, i.e. dB relative to 2\\.c10^^-5^ Pascal, "
	"which is the normative auditory threshold for a 1000-Hz sine wave.")
MAN_END

MAN_BEGIN (L"Intensity: Get maximum...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Return value")
NORMAL (L"the maximum value within the specified time domain, expressed in dB.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (L"%%Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Intensity: Get mean...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Return value")
NORMAL (L"the mean (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (L"##Averaging method")
DEFINITION (L"the units in which the averaging is performed. If the method is #energy, "
	"the returned dB value is based on the mean power (in Pa^2/s) between %t__1_ and %t__2_. "
	"If the method is #dB, the returned value is the mean of the intensity curve in dB. "
	"If the method is #sones, the returned value is in between these two, "
	"and based on averaging properties of the human ear.")
ENTRY (L"Algorithm")
NORMAL (L"If the averaging method is #dB, the mean intensity between the times %t__1_ and %t__2_ is defined as")
FORMULA (L"1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %x(%t) %dt")
NORMAL (L"where %x(%t) is the intensity as a function of time, in dB. If the method is #energy, the result is")
FORMULA (L"10 log__10_ { 1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ 10^^%x(%t)/10^ %dt }")
NORMAL (L"If the method is #sones, the result is")
FORMULA (L"10 log__2_ { 1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ 2^^%x(%t)/10^ %dt }")
ENTRY (L"Behaviour")
NORMAL (L"After you do @@Sound: To Intensity...@, the mean intensity of the resulting #Intensity, "
	"if the averaging method is #energy, should be close to the mean SPL of the original #Sound, "
	"which can be found with #Info.")
MAN_END

MAN_BEGIN (L"Intensity: Get minimum...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Return value")
NORMAL (L"the minimum value within a specified time domain, expressed in dB.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (#None, #Parabolic, #Cubic, #Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Intensity: Get standard deviation...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Return value")
NORMAL (L"the standard deviation (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY (L"Settings")
TAG (L"%%Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
ENTRY (L"Algorithm")
NORMAL (L"The standard deviation between the times %t__1_ and %t__2_ is defined as")
FORMULA (L"\\Vr {1/(%t__2_ - %t__1_)  \\in__%%t%1_^^%%t%2^ %dt (%x(%t) - %\\mu)^2}")
NORMAL (L"where %x(%t) is the intensity (in dB) as a function of time, and %\\mu its mean. "
	"For our discrete Intensity object, the standard deviation is approximated by")
FORMULA (L"\\Vr {1/(%n-1) \\su__%i=%m..%m+%n-1_ (%x__%i_ - %\\mu)^2}")
NORMAL (L"where %n is the number of frames between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN (L"Intensity: Get time of maximum...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Return value")
NORMAL (L"the time (in seconds) associated with the maximum intensity within a specified time domain.")
ENTRY (L"Settings")
TAG (L"%%Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (L"%%Interpolation")
DEFINITION (L"the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Intensity: Get time of minimum...", L"ppgb", 20041107)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Return value")
NORMAL (L"the time (in seconds) associated with the minimum intensity within a specified time domain.")
ENTRY (L"Settings")
TAG (L"##Time range (s)")
DEFINITION (L"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method (None, Parabolic, Cubic, Sinc) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Intensity: Get value at time...", L"ppgb", 20030916)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Return value")
NORMAL (L"the intensity (in dB) at a specified time. If %time is outside the frames of the Intensity, the result is 0.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time at which the value is to be evaluated.")
TAG (L"##Interpolation")
DEFINITION (L"the interpolation method, see @@vector value interpolation@. "
	"The standard is Cubic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (L"Intensity: Get value in frame...", L"ppgb", 19991016)
INTRO (L"A @query to the selected @Intensity object.")
ENTRY (L"Setting")
TAG (L"##Frame number")
DEFINITION (L"the frame whose value is to be looked up.")
ENTRY (L"Return value")
NORMAL (L"the intensity value (in dB) in the specified frame. "
	"If the index is less than 1 or greater than the number of frames, the result is 0; "
	"otherwise, it is %z [1] [%%frame number%].")
MAN_END

MAN_BEGIN (L"Intensity: To IntensityTier", L"ppgb", 19970321)
INTRO (L"A command to convert each selected @Intensity object to an @IntensityTier.")
ENTRY (L"Behaviour")
NORMAL (L"Every sample in the @Intensity object is copied to a point on the @IntensityTier.")
ENTRY (L"Postconditions")
DEFINITION (L"Equal time domains:")
LIST_ITEM (L"\\bu %result. %xmin == %intensity. %xmin")
LIST_ITEM (L"\\bu %result. %xmax == %intensity. %xmax")
DEFINITION (L"Equal number of points:")
LIST_ITEM (L"\\bu %result. %points. %size == %intensity. %nx")
NORMAL (L"For all points %i = 1 ... %intensity. %nx:")
DEFINITION (L"   Explicit times:")
LIST_ITEM (L"   \\bu %result. %points. %item [%i]. %time == %intensity. %x1 + (%i \\-- 1) * %intensity. %dx")
DEFINITION (L"   Equal number of points:")
LIST_ITEM (L"   \\bu %result. %points. %item [%i]. %value == %intensity. %z [1] [%i]")
MAN_END

MAN_BEGIN (L"Intensity & PointProcess: To IntensityTier...", L"ppgb", 20101230)
INTRO (L"A command to copy information from an @Intensity, at times specified by a @PointProcess, "
	"to points on an @IntensityTier.")
ENTRY (L"Behaviour")
NORMAL (L"For all the times of the points in the PointProcess, an intensity is computed from the "
	"information in the Intensity object, by linear interpolation.")
MAN_END

MAN_BEGIN (L"IntensityTier", L"ppgb", 20101230)
INTRO (L"One of the @@types of objects@ in Praat. "
	"An IntensityTier object represents a time-stamped intensity contour, i.e., it contains a series of (%time, %intensity) points. "
	"The intensity values are in dB.")
NORMAL (L"For examples, see @@Source-filter synthesis@.")
ENTRY (L"IntensityTier commands")
NORMAL (L"Creation:")
LIST_ITEM (L"From scratch:")
LIST_ITEM (L"\\bu @@Create IntensityTier...")
LIST_ITEM (L"\\bu @@IntensityTier: Add point...")
LIST_ITEM (L"Copy from another object:")
LIST_ITEM (L"\\bu @@Intensity: To IntensityTier@: trivial copying of linearly spaced points.")
LIST_ITEM (L"\\bu @@Intensity & PointProcess: To IntensityTier...@: copying interpolated values at specified points.")
LIST_ITEM (L"\\bu @@PointProcess: Up to IntensityTier...@: equal values at specified points.")
NORMAL (L"Viewing and editing:")
LIST_ITEM (L"\\bu @IntensityTierEditor")
NORMAL (L"Conversion:")
LIST_ITEM (L"\\bu @@IntensityTier: Down to PointProcess@: copy times.")
NORMAL (L"Synthesis (see @@Source-filter synthesis@):")
LIST_ITEM (L"\\bu @@Sound & IntensityTier: Multiply@")
NORMAL (L"Queries:")
LIST_ITEM (L"\\bu @@Get low index from time...")
LIST_ITEM (L"\\bu @@Get high index from time...")
LIST_ITEM (L"\\bu @@Get nearest index from time...")
NORMAL (L"Modification:")
LIST_ITEM (L"\\bu @@Remove point...")
LIST_ITEM (L"\\bu @@Remove point near...")
LIST_ITEM (L"\\bu @@Remove points between...")
LIST_ITEM (L"\\bu @@IntensityTier: Add point...@")
MAN_END

MAN_BEGIN (L"IntensityTier: Add point...", L"ppgb", 20010410)
INTRO (L"A command to add a point to each selected @IntensityTier.")
ENTRY (L"Settings")
TAG (L"##Time (s)")
DEFINITION (L"the time at which a point is to be added.")
TAG (L"##Intensity (dB)")
DEFINITION (L"the intensity value of the requested new point.")
ENTRY (L"Behaviour")
NORMAL (L"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (L"IntensityTier: Down to PointProcess", L"ppgb", 20010410)
INTRO (L"A command to degrade every selected @IntensityTier to a @PointProcess.")
ENTRY (L"Behaviour")
NORMAL (L"The times of all the points are trivially copied, and so is the time domain. The intensity information is lost.")
MAN_END

MAN_BEGIN (L"IntensityTierEditor", L"ppgb", 20110128)
INTRO (L"One of the @editors in the Praat program, for viewing and editing an @IntensityTier object. "
	"To create a IntensityTierEditor window, select an IntensityTier and click ##View & Edit#.")
MAN_END

MAN_BEGIN (L"Keyboard shortcuts", L"ppgb", 20071016)
INTRO (L"A way to accelerate the control of @Editors in Praat.")
ENTRY (L"Purpose")
NORMAL (L"to choose a menu command with the keyboard. All of these commands can also be chosen "
	"from a menu.")
ENTRY (L"Command key")
NORMAL (L"When mentioning the %%Command key%, this manual refers to the key that is marked with an apple on Apple keyboards "
	"or to the key that is marked \"Ctrl\" if you are on a Windows or Linux computer.")
ENTRY (L"Option key")
NORMAL (L"When mentioning the %%Option key%, this manual refers to the key marked \"alt\" or \"option\". "
	"In Praat, this key is sometimes used together with the Command key "
	"for destructive actions that are the reverse of the actions invoked by using the Command key only. "
	"For instance, if Command-T means \"add a target at the cursor position\", "
	"Option-Command-T may mean \"remove the selected targets\".")
ENTRY (L"Shortcuts")
LIST_ITEM (L"Command-A: Zoom all")
LIST_ITEM (L"Command-C: Copy (the selected text, or the selected sound, or the selected part of the Picture window)")
LIST_ITEM (L"Command-D (in Manipulation window): Insert duration point at cursor")
LIST_ITEM (L"Option-Command-D (in Manipulation window): Remove selected duration points")
LIST_ITEM (L"Command-E (in Picture window): Erase all")
LIST_ITEM (L"Shift-Command-H: Move cursor to maximum pitch")
LIST_ITEM (L"Command-I: Zoom in")
LIST_ITEM (L"Command-L (in Objects window): @@Open long sound file...@")
LIST_ITEM (L"Command-L (in sound windows): @@Intro 3.6. Viewing a spectral slice|View spectral slice@")
LIST_ITEM (L"Shift-Command-L: Move cursor to minimum pitch")
LIST_ITEM (L"Command-M: Search Praat manual...")
LIST_ITEM (L"Command-N: Zoom to selection")
LIST_ITEM (L"Command-O (in Objects window): @@Read from file...@")
LIST_ITEM (L"Command-O (in sound windows): Zoom out")
LIST_ITEM (L"Command-P (in Picture window): Print")
LIST_ITEM (L"Command-P (in Manipulation window): Add pulse at cursor")
LIST_ITEM (L"Option-Command-P (in Manipulation window): Remove selected pulses")
LIST_ITEM (L"Command-Q: Quit")
LIST_ITEM (L"Command-R: Reverse selection")
LIST_ITEM (L"Command-S: Save")
LIST_ITEM (L"Command-T (in Manipulation window): Add pitch point at cursor")
LIST_ITEM (L"Option-Command-T (in Manipulation window): Remove selected pitch points")
LIST_ITEM (L"Command-U: @@Calculator...@")
LIST_ITEM (L"Command-V: Paste (insert the text or sound clipboard over the selected text or the selected sound)")
LIST_ITEM (L"Command-W: Close window")
LIST_ITEM (L"Command-X: Cut (the selected text or the selected sound)")
LIST_ITEM (L"Command-Y: Redo")
LIST_ITEM (L"Command-Z: Undo")
LIST_ITEM (L"Command-0: Move cursor to nearest zero crossing")
LIST_ITEM (L"Command-2 (in Manipulation window): Stylize pitch (2 semitones)")
LIST_ITEM (L"Command-4 (in Manipulation window): Interpolate quadratically (4 points)")
LIST_ITEM (L"Shift-Command-?: Local help")
LIST_ITEM (L"Command-,: Move start of selection to nearest zero crossing")
LIST_ITEM (L"Command-.: Move end of selection to nearest zero crossing")
LIST_ITEM (L"F5: Get pitch")
LIST_ITEM (L"Command-F5: Get minimum pitch")
LIST_ITEM (L"Shift-F5: Get maximum pitch")
LIST_ITEM (L"F1: Get first formant")
LIST_ITEM (L"F2: Get second formant")
LIST_ITEM (L"F3: Get third formant")
LIST_ITEM (L"F4: Get fourth formant")
LIST_ITEM (L"F5: Get pitch")
LIST_ITEM (L"F6: Get cursor")
LIST_ITEM (L"F7: Get spectral power at cursor cross")
LIST_ITEM (L"F8: Get intensity")
LIST_ITEM (L"F12: Log 1")
LIST_ITEM (L"Shift-F12: Log 2")
LIST_ITEM (L"Option-F12: Log script 3")
LIST_ITEM (L"Command-F12: Log script 4")
LIST_ITEM (L"Tab (in sound windows): Play selection")
LIST_ITEM (L"Shift-Tab (in sound windows): Play window")
LIST_ITEM (L"Arrow-up (in sound windows): Select earlier")
LIST_ITEM (L"Arrow-down (in sound windows): Select later")
LIST_ITEM (L"Shift-Arrow-up (in sound windows): Move start of selection left")
LIST_ITEM (L"Shift-Arrow-down (in sound windows): Move start of selection right")
LIST_ITEM (L"Command-Arrow-up (in sound windows): Move end of selection left")
LIST_ITEM (L"Command-Arrow-down (in sound windows): Move end of selection right")
LIST_ITEM (L"Page-up (in sound windows): Scroll page back")
LIST_ITEM (L"Page-down (in sound windows): Scroll page forward")
LIST_ITEM (L"Escape: Interrupt playing")
MAN_END

MAN_BEGIN (L"Log files", L"ppgb", 20110808)
INTRO (L"With some commands in the @Query menu of the @SoundEditor and @TextGridEditor, "
	"you can write combined information about times, pitch values, formants, and intensities "
	"to the @@Info window@ and to a log file.")
NORMAL (L"A log file is a text file on disk. It consists of a number of similar lines, "
	"whose format you determine with the log settings in the Query menu.")
NORMAL (L"Every time you press F12 (or choose ##Log 1# from the Query menu, "
	"Praat writes a line to log file 1. If you press Shift-F12, Praat writes a line to log file 2.")
NORMAL (L"With the ##log settings# command window, you determine the following:")
TAG (L"##Log 1 to Info window")
DEFINITION (L"this determines whether your log line will be written to the Info window or not.")
TAG (L"##Log 1 to log file")
DEFINITION (L"this determines whether your log line will be written to the log file or not.")
TAG (L"##Log file 1")
DEFINITION (L"the name of the log file. On Windows, this has to be a complete path name, such as "
	"$$C:\\bsWINDOWS\\bsDESKTOP\\bsPitch Log.txt$. "
	"On Unix and MacOS X, it can either be a complete path name, e.g. $$/home/mary/pitch_log$, "
	"or a home-relative name such as $$~/Desktop/Pitch log$.")
TAG (L"##Log 1 format")
DEFINITION (L"the format of the line that Praat will write. See below.")
NORMAL (L"The same goes for log file 2.")
ENTRY (L"Usage")
NORMAL (L"The logging facility has been implemented in Praat especially for former users of Kay CSL, "
	"who have been used to doing it for years and like to continue doing it in Praat. "
	"Otherwise, you may prefer to use the @TextGridEditor to mark time points and run "
	"an automatic analysis afterwards.")
NORMAL (L"If you do want to use the logging facility, you typically start by deleting any old "
	"log file (by choosing ##Delete log file 1# or ##Delete log file 2#), if you want to re-use "
	"the file name. Otherwise, you can change the log file name (with ##Log settings...#). "
	"After this, you will move the cursor to various time locations and press F12 (or Shift-F12) "
	"each time, so that information about the current time will be written to the log file.")
ENTRY (L"Example 1: pitch logging")
NORMAL (L"Suppose you want to log the time of the cursor and the pitch value at the cursor. "
	"You could use the following log format:")
CODE (L"Time \'time:6\' seconds, pitch \'f0:2\' hertz")
NORMAL (L"If you now click at 3.456789876 seconds, and the pitch happens to be 355.266 hertz "
	"at that time, the following line will be appended to the log file and/or to the Info window:")
CODE (L"Time 3.456790 seconds, pitch 355.27 hertz.")
NORMAL (L"The parts \":6\" and \":2\" denote the number of digits after the decimal point. "
	"If you leave them out, the values will be written with a precision of 17 digits.")
NORMAL (L"The words \'time\' and \'f0\' mean exactly the same as the result of the commands "
	"##Get cursor# and ##Get pitch#. Therefore, if instead of setting a cursor line you selected a larger "
	"piece of the sound, \'time\' will give the centre of the selection and \'f0\' will give the mean pitch "
	"in the selection.")
NORMAL (L"Beware of the following pitfall: if your pitch units are not hertz, but semitones, "
	"then \'f0\' will give the result in semitones. A format as in this example will then be misleading.")
ENTRY (L"Example 2: formant logging")
NORMAL (L"Suppose you want to log the start and finish of the selection, its duration, and the mean values "
	"of the first three formants, all separated by tab stops for easy importation into Microsoft\\re Excel\\tm. "
	"You could use the following log format:")
CODE (L"\'t1:4\'\'tab\\$ \'\'t2:4\'\'tab\\$ \'\'f1:0\'\'tab\\$ \'\'f2:0\'\'tab\\$ \'\'f3:0\'")
NORMAL (L"You see that \'t1\' and \'t2\' are the start and finish of the selection, respectively, "
	"and that they are written with 4 digits after the decimal point. By using \":0\", the three formant values "
	"are rounded to whole numbers in hertz. The word \'tab\\$ \' is the tab stop.")
ENTRY (L"Loggable values")
NORMAL (L"The following values can be logged:")
LIST_ITEM (L"\'time\': the time of the cursor, or the centre of the selection.")
LIST_ITEM (L"\'t1\': the start of the selection (\"B\").")
LIST_ITEM (L"\'t2\': the end of the selection (\"E\").")
LIST_ITEM (L"\'dur\': the duration of the selection.")
LIST_ITEM (L"\'freq\': the frequency at the frequency cursor.")
LIST_ITEM (L"\'f0\': the pitch at the cursor time, or the mean pitch in the selection.")
LIST_ITEM (L"\'f1\', \'f2\', \'f3\', \'f4\', \'f5\': the first/second/third/fourth/fifth formant at the cursor time, "
	"or the mean first/second/third/fourth/fifth formant in the selection.")
LIST_ITEM (L"\'b1\', \'b2\', \'b3\', \'b4\', \'b5\': the bandwidth of the first/second/third/fourth/fifth formant "
	"at the cursor time or at the centre of the selection.")
LIST_ITEM (L"\'intensity\': the intensity at the cursor time, or the mean intensity in the selection, in dB.")
LIST_ITEM (L"\'power\': the spectral power at the cursor cross, in Pa^2/Hz.")
LIST_ITEM (L"\'tab\\$ \': the tab stop.")
LIST_ITEM (L"\'editor\\$ \': the title of the editor window (i.e. the name of the visible Sound or TextGrid).")
ENTRY (L"More flexibility in logging")
NORMAL (L"You may sometimes require information in your log file that cannot be generated directly "
	"by the loggable values above. Suppose, for instance, that you want to log the values for F1 and F2-F1 "
	"at the points where you click. You could write the following script:")
CODE (L"f1 = Get first formant")
CODE (L"f2 = Get second formant")
CODE (L"f21 = f2 - f1")
CODE (L"printline 'f1:0' 'f21:0'")
CODE (L"fileappend \"D:\\bsPraat logs\\bsFormant log.txt\" 'f1:0''tab\\$ ''f21:0''newline\\$ '")
NORMAL (L"With this script, the information would be appended both to the Info window and to the "
	"file \"Formant log.txt\" on your desktop.")
NORMAL (L"You can make this script accessible with Option-F12 (or Command-F12) "
	"by saving the script and specifying the name of the script file in the ##Log script 3# (or #4) field "
	"in the ##Log settings...# window.")
NORMAL (L"These scripts may take arguments. Suppose, for instance, that you want to specify a vowel symbol "
	"as you press Option-F12. The following script will take care of that:")
CODE (L"form Save vowel and formants")
CODE1 (L"word Vowel a")
CODE (L"endform")
CODE (L"f1 = Get first formant")
CODE (L"f2 = Get second formant")
CODE (L"f21 = f2 - f1")
CODE (L"printline 'vowel\\$ ' 'f1:0' 'f21:0'")
CODE (L"fileappend \"~/Praat logs/Vowels and formants log\" 'vowel\\$ ''f1:0''tab\\$ ''f21:0''newline\\$ '")
NORMAL (L"Beware of the following pitfall: because of the nature of scripts, you should not try to do this "
	"when you have two editor windows with the same name. I cannot predict which of the two windows "
	"will answer the #Get queries...")
MAN_END

MAN_BEGIN (L"Manipulation", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat, for changing the pitch and duration contours of a sound.")
ENTRY (L"Inside a manipulation object")
NORMAL (L"With @Inspect, you will see the following attributes:")
TAG (L"##timeStep")
DEFINITION (L"the time step (or %%frame length%) used in the pitch analysis. A common value is 0.010 seconds.")
TAG (L"##minimumPitch")
DEFINITION (L"the minimum pitch frequency considered in the pitch analysis. A common value is 75 hertz.")
TAG (L"##maximumPitch")
DEFINITION (L"the maximum pitch frequency considered in the pitch analysis. A common value is 600 hertz.")
NORMAL (L"A Manipulation object also contains the following smaller objects:")
LIST_ITEM (L"1. The original @Sound.")
LIST_ITEM (L"2. A @PointProcess representing glottal pulses.")
LIST_ITEM (L"3. A @PitchTier.")
LIST_ITEM (L"4. A @DurationTier.")
ENTRY (L"Analysis")
NORMAL (L"When a Manipulation object is created from a sound, the following steps are performed:")
LIST_ITEM (L"1. A pitch analysis is performed on the original sound, with the method of @@Sound: To Pitch...@. "
	"This uses the time step, minimum pitch, and maximum pitch parameters.")
LIST_ITEM (L"2. The information of the resulting pitch contour (frequency and voiced/unvoiced decisions) "
	"is used to posit glottal pulses where the original sound contains much energy. "
	"The method is the same as in @@Sound & Pitch: To PointProcess (cc)@.")
LIST_ITEM (L"3. The pitch contour is converted to a pitch tier with many points (targets), "
	"with the method of @@Pitch: To PitchTier@.")
LIST_ITEM (L"4. An empty @DurationTier is created.")
ENTRY (L"Resynthesis")
TAG (L"A Manipulation object can produce Sound input. This Sound can be computed in several ways:")
LIST_ITEM (L"\\bu @@overlap-add@: from original sound + pulses + pitch tier + duration tier;")
LIST_ITEM (L"\\bu #LPC: from LPC (from original sound) + pulses + pitch tier;")
LIST_ITEM (L"\\bu from the pulses only, as a pulse train or hummed;")
LIST_ITEM (L"\\bu from the pitch tier only, as a pulse train or hummed.")
MAN_END

MAN_BEGIN (L"Manipulation: Extract duration tier", L"ppgb", 20010330)
INTRO (L"A command to extract a copy of the duration information in each selected @Manipulation object into a new @DurationTier object.")
MAN_END

MAN_BEGIN (L"Manipulation: Extract original sound", L"ppgb", 20010330)
INTRO (L"A command to copy the original sound in each selected @Manipulation object to a new @Sound object.")
MAN_END

MAN_BEGIN (L"Manipulation: Extract pitch tier", L"ppgb", 20010330)
INTRO (L"A command to extract a copy of the pitch information in each selected @Manipulation object into a new @PitchTier object.")
MAN_END

MAN_BEGIN (L"Manipulation: Extract pulses", L"ppgb", 20010330)
INTRO (L"A command to extract a copy of the vocal-pulse information in each selected @Manipulation object into a new @PointProcess object.")
MAN_END

MAN_BEGIN (L"Manipulation: Play (overlap-add)", L"ppgb", 20070722)
INTRO (L"A command to play each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN (L"Manipulation: Get resynthesis (overlap-add)", L"ppgb", 20070722)
INTRO (L"A command to extract the sound from each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN (L"Manipulation: Replace duration tier", L"ppgb", 20030216)
INTRO (L"You can replace the duration tier that you see in your @Manipulation object "
	"with a separate @DurationTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create DurationTier...@.")
NORMAL (L"To do this, select your Manipulation object together with the @DurationTier object and click ##Replace duration tier#.")
MAN_END

MAN_BEGIN (L"Manipulation: Replace pitch tier", L"ppgb", 20030216)
INTRO (L"You can replace the pitch tier that you see in your @Manipulation object "
	"with a separate @PitchTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create PitchTier...@.")
NORMAL (L"To do this, select your Manipulation object together with the @PitchTier object and click ##Replace pitch tier#.")
MAN_END

MAN_BEGIN (L"Manipulation: Replace pulses", L"ppgb", 20010330)
INTRO (L"A command to replace the vocal-pulse information in the selected @Manipulation object with the selected @PointProcess object.")
MAN_END

MAN_BEGIN (L"Manipulation: Replace original sound", L"ppgb", 20010330)
INTRO (L"A command to replace the original sound in the selected @Manipulation object with the selected @Sound object.")
MAN_END

MAN_BEGIN (L"ManipulationEditor", L"ppgb", 20030316)
	INTRO (L"One of the @Editors in Praat, for viewing and manipulating a @Manipulation object.")
ENTRY (L"Objects")
	NORMAL (L"The editor shows:")
	LIST_ITEM (L"\\bu The original @Sound.")
	LIST_ITEM (L"\\bu The @PointProcess that represents the glottal %pulses. "
		"You can edit it for improving the pitch analysis.")
	LIST_ITEM (L"\\bu A pitch contour based on the locations of the pulses, for comparison (drawn as grey dots). "
		"Changes shape if you edit the pulses.")
	LIST_ITEM (L"\\bu The @PitchTier that determines the pitch contour of the resynthesized @Sound (drawn as blue circles). "
		"At the creation of the @Manipulation object, it is computed from the original pitch contour. "
		"You can manipulate it by simplifying it (i.e., removing targets), "
		"or by moving parts of it up and down, and back and forth.")
	LIST_ITEM (L"\\bu A @DurationTier for manipulating the relative durations of the voiced parts of the sound.")
ENTRY (L"Playing")
	NORMAL (L"To play (a part of) the %resynthesized sound (by any of the methods shown in the #Synth menu, "
		"like @@overlap-add@ and #LPC), @click on any of the 1 to 8 buttons below and above the drawing area "
		"or use the Play commands from the View menu.")
	NORMAL (L"To play the %original sound instead, use ##Shift-click#.")
ENTRY (L"Pulses")
	TAG (L"To add:")
	DEFINITION (L"#click at the desired time location, and choose ##Add pulse at cursor# or type ##Command-p#.")
	TAG (L"To remove:")
	DEFINITION (L"make a @@time selection@, and choose ##Remove pulse(s)# or type ##Option-Command-p#. "
		"If there is no selection, the pulse nearest to the cursor is removed.")
ENTRY (L"Pitch points")
	TAG (L"To add one at a specified %%time and frequency%:")
	DEFINITION (L"#click at the desired time-frequency location, and choose ##Add pitch point at cursor# or type ##Command-t#.")
	TAG (L"To add one at a specified %time only:")
	DEFINITION (L"#click at the desired time, and choose ##Add pitch point at time slice#. ManipulationEditor tries to compute the frequency from the "
		"intervals between the pulses, basically by a median-of-three method.")
	TAG (L"To remove:")
	DEFINITION (L"make a @@time selection@, and choose ##Remove pitch point(s)# or type ##Option-Command-t#. "
		"If there is no selection, the pitch point nearest to the cursor is removed.")
	TAG (L"To move %some:")
	DEFINITION (L"make a @@time selection@ (the points become red) and ##Shift-drag# the points across the window. "
		"You cannot drag them across adjacent points, or below 50 Hz, or above the maximum frequency. "
		"You can only drag them horizontally if the %%dragging strategy% is ##All# or ##Only horizontal#, "
		"and you can drag them vertically if the dragging strategy is not ##Only horizontal#. "
		"You can change the dragging strategy with ##Set pitch dragging strategy...# from the #Pitch menu.")
	TAG (L"To move %one:")
	DEFINITION (L"@drag that point across the window. "
		"You can only drag it horizontally if the dragging strategy is not ##Only vertical#, "
		"and you can drag it vertically if the dragging strategy is not ##Only horizontal#.")
ENTRY (L"Duration points")
	NORMAL (L"Work pretty much the same as pitch points.")
ENTRY (L"Stylization")
	NORMAL (L"Before editing the Pitch points, you may want to reduce their number by choosing any of the #Stylize "
		"commands from the #Pitch menu.")
MAN_END

MAN_BEGIN (L"Matrix", L"ppgb", 20030216)
INTRO (L"One of the @@types of objects@ in Praat. "
	"A Matrix object represents a function %z (%x, %y) "
	"on the domain [%x__%min_, %x__%max_] \\xx [%y__%min_, %y__%max_]. "
	"The domain has been sampled in the %x and %y directions "
	"with constant sampling intervals (%dx and %dy) along each direction. "
	"The samples are thus %z [%i__%y_] [%i__%x_], %i__%x_ = 1 ... %n__%x_, %i__%y_ = 1 ... %n__%y_. "
	"The samples represent the function values %z (%x__1_ + (%ix - 1) %dx, %y__1_ + (%iy - 1) %dy).")
ENTRY (L"Matrix commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Create Matrix...")
LIST_ITEM (L"\\bu @@Create simple Matrix...")
LIST_ITEM (L"\\bu @@Read from file...")
LIST_ITEM (L"\\bu @@Read Matrix from raw text file...")
LIST_ITEM (L"\\bu ##Read Matrix from LVS AP file...")
NORMAL (L"Drawing:")
LIST_ITEM (L"\\bu ##Matrix: Draw rows...")
LIST_ITEM (L"\\bu ##Matrix: Draw contours...")
LIST_ITEM (L"\\bu ##Matrix: Paint contours...")
LIST_ITEM (L"\\bu ##Matrix: Paint cells...")
LIST_ITEM (L"\\bu ##Matrix: Scatter plot...")
LIST_ITEM (L"\\bu @@Matrix: Draw as squares...")
LIST_ITEM (L"\\bu ##Matrix: Draw value distribution...")
LIST_ITEM (L"\\bu ##Matrix: Paint surface...")
NORMAL (L"Modification:")
LIST_ITEM (L"\\bu @@Matrix: Formula...")
LIST_ITEM (L"\\bu ##Matrix: Scale...")
ENTRY (L"Inside a Matrix object")
NORMAL (L"With @Inspect, you will see the following attributes.")
TAG (L"%xmin, %xmax \\>_ %xmin")
DEFINITION (L"%x domain.")
TAG (L"%nx \\>_ 1")
DEFINITION (L"number of columns.")
TAG (L"%dx > 0.0")
DEFINITION (L"distance between columns.")
TAG (L"%x1")
DEFINITION (L"%x value associated with first column.")
TAG (L"%ymin, %ymax \\>_ %ymin")
DEFINITION (L"%y domain.")
TAG (L"%ny \\>_ 1")
DEFINITION (L"number of rows.")
TAG (L"%dy > 0.0")
DEFINITION (L"distance between rows.")
TAG (L"%y1")
DEFINITION (L"%y value associated with first row.")
TAG (L"%z [1..%ny] [1..%nx]")
DEFINITION (L"The sample values.")
NORMAL (L"After creation of the #Matrix, %xmin, %xmax, %ymin, %ymax, "
	"%nx, %ny, %dx, %dy, %x1, and %y1 "
	"do not usually change. The contents of %z do.")
NORMAL (L"Normally, you will want %xmin \\<_ %x1 and %xmax \\>_ %x1 + (%nx - 1) %dx.")
ENTRY (L"Example: simple matrix")
NORMAL (L"If a simple matrix has %x equal to column number "
	"and %y equal to row number, it has the following attributes:")
LIST_ITEM (L"%xmin = 1;   %xmax = %nx;   %dx = 1;  %x1 = 1;")
LIST_ITEM (L"%ymin = 1;   %ymax = %ny;   %dy = 1;  %y1 = 1;")
ENTRY (L"Example: sampled signal")
NORMAL (L"If the matrix represents a sampled signal of 1 second duration "
	"with a sampling frequency of 10 kHz, it has the following attributes:")
LIST_ITEM (L"%xmin = 0.0;   %xmax = 1.0;   %nx = 10000 ;   %dx = 1.0\\.c10^^-4^;   %x1 = 0.5\\.c10^^-4^;")
LIST_ITEM (L"%ymin = 1;   %ymax = 1;   %ny = 1;   %dy = 1;   %y1 = 1;")
ENTRY (L"Example: complex signal")
NORMAL (L"If the matrix represents a complex spectrum "
	"derived with an @FFT from the sound of example 2, it has the following attributes:")
LIST_ITEM (L"%xmin = 0.0;   %xmax = 5000.0;   %nx = 8193 ;   %dx = 5000.0 / 8192;   %x1 = 0.0;")
LIST_ITEM (L"%ny = 2 (real and imaginary part);")
LIST_ITEM (L"%ymin = 1 (first row, real part);")
LIST_ITEM (L"%ymax = 2 (second row, imaginary part);")
LIST_ITEM (L"%dy = 1;   %y1 = 1;  (so that %y is equal to row number)")
MAN_END

MAN_BEGIN (L"Matrix: Draw as squares...", L"ppgb", 19980319)
INTRO (L"A command to draw a @Matrix object into the @@Picture window@.")
ENTRY (L"Settings")
TAG (L"##Xmin")
TAG (L"##Xmax")
DEFINITION (L"the windowing domain in the %x direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Xmin \\>_ %Xmax), the entire %x domain [%x__%min_, %x__%max_] of the Matrix is used.")
TAG (L"##Ymin")
TAG (L"##Ymax")
DEFINITION (L"the windowing domain in the %y direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Ymin \\>_ %Ymax), the entire %y domain [%y__%min_, %y__%max_] of the Matrix is used.")
TAG (L"##Garnish")
DEFINITION (L"determines whether axes are drawn around the picture. "
	"Turn this button off if you prefer to garnish your picture by yourself with the @Margins menu.")
ENTRY (L"Behaviour")
NORMAL (L"For every element of the Matrix inside the specified windowing domain, "
	"an opaque white or black rectangle is painted (white if the value of the element is positive, "
	"black if it is negative), surrounded by a thin black box. "
	"The %area of the rectangle is proportional to the value of the element.")
ENTRY (L"Trick")
NORMAL (L"If you prefer the %sides of the rectangle (instead of the area) to be proportional "
	"to the value of the element, you can use the formula \"$$self\\^ 2$\" before drawing (see @@Matrix: Formula...@).") 
MAN_END

MAN_BEGIN (L"Matrix: Formula...", L"ppgb", 20021206)
INTRO (L"A command for changing the data in all selected @Matrix objects.")
NORMAL (L"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (L"Matrix: Paint cells...", L"ppgb", 20021204)
INTRO (L"A command to draw the contents of a @Matrix to the @@Picture window@.")
NORMAL (L"Every cell of the matrix is drawn as a rectangle filled with a grey value between white (if the content "
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

MAN_BEGIN (L"Matrix: Set value...", L"ppgb", 19980319)
INTRO (L"A command to change the value of one cell in each selected @Matrix object.")
ENTRY (L"Settings")
TAG (L"##Row number")
DEFINITION (L"the number of the row of the cell whose value you want to change.")
TAG (L"##Column number")
DEFINITION (L"the number of the column of the cell whose value you want to change.")
TAG (L"##New value")
DEFINITION (L"the value that you want the specified cell to have.")
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

MAN_BEGIN (L"Matrix: To TableOfReal", L"ppgb", 19991030)
INTRO (L"A command to convert every selected @Matrix to a @TableOfReal.")
NORMAL (L"This command is available from the #Cast menu. The resulting TableOfReal "
	"has the same number of rows and columns as the original Matrix, "
	"and the same data in the cells. However, it does not yet have any row or column "
	"labels; you can add those with some commands from the TableOfReal #Modify menu.")
MAN_END

MAN_BEGIN (L"Modify", L"ppgb", 20021204)
INTRO (L"The title of a submenu of the @@dynamic menu@ for many object types. "
	"This submenu usually collects all the commands that can change the selected object.")
MAN_END

MAN_BEGIN (L"PairDistribution", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat. A "
	"PairDistribution object represents the relative probabilities with which "
	"the specified pairs of strings occur.")
ENTRY (L"Class description")
TAG (L"##struct-list# pairs")
DEFINITION (L"a list of relative string-pair probabilities. Each element consists of:")
TAG1 (L"#string %string1")
DEFINITION1 (L"the first string.")
TAG1 (L"#string %string2")
DEFINITION1 (L"the second string.")
TAG1 (L"#real %weight")
DEFINITION1 (L"the relative probability associated with the string pair. This value cannot be negative.")
MAN_END

MAN_BEGIN (L"PairDistribution: To Stringses...", L"ppgb", 20030916)
INTRO (L"A command to generate a number of string pairs from the selected @PairDistribution object. "
	"This command will create two aligned @Strings objects of equal size.")
ENTRY (L"Settings")
TAG (L"##Number# (standard: 1000)")
DEFINITION (L"the number of the strings in either resulting Strings object.")
TAG (L"##Name of first Strings# (standard: \"input\")")
DEFINITION (L"the name of the resulting Strings object associated with the first string of each pair.")
TAG (L"##Name of second Strings# (standard: \"output\")")
DEFINITION (L"the name of the resulting Strings object associated with the second string of each pair.")
ENTRY (L"Example")
NORMAL (L"Suppose the PairDistribution contains the following:")
CODE (L"4 pairs")
CODE (L"\"at+ma\"  \"atma\"  100")
CODE (L"\"at+ma\"  \"apma\"    0")
CODE (L"\"an+pa\"  \"anpa\"   20")
CODE (L"\"an+pa\"  \"ampa\"   80")
NORMAL (L"The resulting Strings object \"input\" may then contain:")
FORMULA (L"at+ma, an+pa, an+pa, at+ma, at+ma, an+pa, an+pa, an+pa, an+pa, at+ma, ...")
NORMAL (L"The Strings object \"output\" may then contain:")
FORMULA (L"atma,  ampa,  ampa,  atma,  atma,  ampa,  anpa,  ampa,  ampa,  atma, ...")
MAN_END

MAN_BEGIN (L"ParamCurve", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"An object of class #ParamCurve represents a sequence of time-stamped points (%x (%%t__i_), %y (%%t__i_)) "
	"in a two-dimensional space.")
MAN_END

MAN_BEGIN (L"PointEditor", L"ppgb", 20110128)
INTRO (L"One of the @Editors in Praat, for viewing and manipulating a @PointProcess object, "
	"which is optionally shown together with a @Sound object.")
ENTRY (L"Objects")
NORMAL (L"The editor shows:")
LIST_ITEM (L"\\bu The @Sound, if you selected a Sound object together with the PointProcess object "
	"before you clicked ##View & Edit#.")
LIST_ITEM (L"\\bu The @PointProcess; vertical blue lines represent the points.")
ENTRY (L"Playing")
NORMAL (L"To play (a part of) the %resynthesized sound (pulse train): "
	"@click on any of the 8 buttons below and above the drawing area, or choose a Play command from the View menu.")
NORMAL (L"To play the %original sound instead, use @@Shift-click@.")
ENTRY (L"Adding a point")
NORMAL (L"@Click at the desired time location, and choose \"Add point at cursor\" or type ##Command-P#.")
ENTRY (L"Removing points")
NORMAL (L"To remove one or more points, "
	"make a @@time selection@ and choose ##Remove point(s)# from the ##Point# menu. "
	"If there is no selection, the point nearest to the cursor is removed.")
MAN_END

MAN_BEGIN (L"PointProcess", L"ppgb", 20110128)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"A PointProcess object represents a %%point process%, "
	"which is a sequence of %points %t__%i_ in time, defined on a domain [%t__%min_, %t__%max_]. "
	"The index %i runs from 1 to the number of points. The points are sorted by time, i.e. %t__%i+1_ > %t__%i_.")
ENTRY (L"PointProcess commands")
NORMAL (L"Creation from scratch:")
LIST_ITEM (L"\\bu @@Create empty PointProcess...@")
LIST_ITEM (L"\\bu @@Create Poisson process...@")
NORMAL (L"Creation of a pulse train from a pitch contour:")
LIST_ITEM (L"\\bu @@PitchTier: To PointProcess@: area-1 along entire time domain.")
LIST_ITEM (L"\\bu @@Pitch: To PointProcess@: same, but excludes voiceless intervals.")
LIST_ITEM (L"\\bu @@Sound & Pitch: To PointProcess (cc)@: \"pitch-synchronous\": near locations of high amplitude.")
LIST_ITEM (L"\\bu @@Sound & Pitch: To PointProcess (peaks)...@: \"pitch-synchronous\": near locations of high amplitude.")
LIST_ITEM (L"\\bu @@Sound: To PointProcess (periodic, cc)...@: near locations of high amplitude.")
LIST_ITEM (L"\\bu @@Sound: To PointProcess (periodic, peaks)...@: near locations of high amplitude.")
NORMAL (L"Creation from converting another object:")
LIST_ITEM (L"\\bu ##Matrix: To PointProcess")
LIST_ITEM (L"\\bu @@PitchTier: Down to PointProcess@")
LIST_ITEM (L"\\bu @@IntensityTier: Down to PointProcess@")
NORMAL (L"Hearing:")
LIST_ITEM (L"\\bu @@PointProcess: Play@: pulse train.")
LIST_ITEM (L"\\bu @@PointProcess: Hum@: pulse train with formants.")
NORMAL (L"Drawing:")
LIST_ITEM (L"\\bu @@PointProcess: Draw...@")
NORMAL (L"Editing:")
LIST_ITEM (L"\\bu ##PointProcess: View & Edit#: invokes a @PointEditor.")
LIST_ITEM (L"\\bu ##PointProcess & Sound: View & Edit#: invokes a @PointEditor.")
LIST_ITEM (L"\\bu Inside a @ManipulationEditor.")
NORMAL (L"Queries:")
LIST_ITEM (L"\\bu @@PointProcess: Get jitter (local)...@: periodic jitter.")
LIST_ITEM (L"\\bu @@PointProcess: Get jitter (local, absolute)...@: periodic jitter.")
LIST_ITEM (L"\\bu @@PointProcess: Get jitter (rap)...@: periodic jitter.")
LIST_ITEM (L"\\bu @@PointProcess: Get jitter (ppq5)...@: periodic jitter.")
LIST_ITEM (L"\\bu @@PointProcess: Get jitter (ddp)...@: periodic jitter.")
LIST_ITEM (L"\\bu @@PointProcess: Get low index...@: index of nearest point not after specified time.")
LIST_ITEM (L"\\bu @@PointProcess: Get high index...@: index of nearest point not before specified time.")
LIST_ITEM (L"\\bu @@PointProcess: Get nearest index...@: index of point nearest to specified time.")
LIST_ITEM (L"\\bu @@PointProcess: Get interval...@: duration of interval around specified time.")
NORMAL (L"Set calculations:")
LIST_ITEM (L"\\bu @@PointProcesses: Union@: the union of two point processes.")
LIST_ITEM (L"\\bu @@PointProcesses: Intersection@: the intersection of two point processes.")
LIST_ITEM (L"\\bu @@PointProcesses: Difference@: the difference of two point processes.")
NORMAL (L"Modification:")
LIST_ITEM (L"\\bu @@PointProcess: Add point...@: at a specified time.")
LIST_ITEM (L"\\bu @@PointProcess: Remove point...@: at specified index.")
LIST_ITEM (L"\\bu @@PointProcess: Remove point near...@: near specified time.")
LIST_ITEM (L"\\bu @@PointProcess: Remove points...@: between specified indices.")
LIST_ITEM (L"\\bu @@PointProcess: Remove points between...@: between specified times.")
NORMAL (L"Analysis:")
LIST_ITEM (L"\\bu @@PointProcess: To PitchTier...@: pitch values in interval centres.")
LIST_ITEM (L"\\bu ##PointProcess & Sound: To Manipulation")
NORMAL (L"Synthesis:")
LIST_ITEM (L"\\bu @@PointProcess: To Sound (pulse train)...@")
LIST_ITEM (L"\\bu @@PointProcess: To Sound (hum)...@")
NORMAL (L"Conversion:")
LIST_ITEM (L"\\bu ##PointProcess: To Matrix")
LIST_ITEM (L"\\bu @@PointProcess: Up to TextGrid...")
LIST_ITEM (L"\\bu @@PointProcess: Up to PitchTier...")
LIST_ITEM (L"\\bu @@PointProcess: Up to IntensityTier...")
MAN_END

MAN_BEGIN (L"PointProcess: Add point...", L"ppgb", 20010410)
INTRO (L"A command to add a point to each selected @PointProcess.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time at which a point is to be added.")
ENTRY (L"Behaviour")
NORMAL (L"The point process is modified so that it contains the new point. "
	"If a point at the specified time was already present in the point process, nothing happens.")
MAN_END

MAN_BEGIN (L"PointProcesses: Difference", L"ppgb", 20021212)
INTRO (L"A command to compute the difference of two selected @PointProcess objects.")
ENTRY (L"Behaviour")
NORMAL (L"The resulting #PointProcess will contain only those points of the first selected original point process "
	"that do not occur in the second.")
NORMAL (L"The time domain of the resulting point process is equal to the time domain of the first original point process.")
MAN_END

MAN_BEGIN (L"PointProcess: Draw...", L"ppgb", 20021212)
INTRO (L"A command to draw every selected @PointProcess into the @@Picture window@.")
MAN_END

MAN_BEGIN (L"PointProcess: Get high index...", L"ppgb", 20021212)
INTRO (L"A @query to the selected @PointProcess object.")
ENTRY (L"Return value")
NORMAL (L"the index of the nearest point at or after the specified time, "
	"0 if the point process contains no points, "
	"or a number higher than the number of points if the specified time is after the last point.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time from which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (L"PointProcess: Get interval...", L"ppgb", 20021212)
INTRO (L"A @query to the selected @PointProcess object.")
ENTRY (L"Return value")
NORMAL (L"the duration of the interval around a specified time. "
	"if the point process contains no points or if the specified time falls before the first point "
	"or not before the last point, the value is @undefined. Otherwise, the result is the distance between "
	"the nearest points to the left and to the right of the specified time. "
	"If the point process happens to contain a point at exactly the specified time, "
	"the duration of the interval following this point is returned.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time around which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (L"PointProcess: Get jitter (local)...", L"ppgb", 20110302)
INTRO (L"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
NORMAL (L"This command will write into the Info window "
	"the %%local jitter%, which is the average absolute difference between consecutive intervals, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (L"As %jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (L"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), L""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (local)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TAG (L"##Time range (s)")
DEFINITION (L"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (L"##Period floor (s)")
DEFINITION (L"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (L"##Period ceiling (s)")
DEFINITION (L"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this setting to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (L"##Maximum period factor")
DEFINITION (L"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (L"2. Usage")
NORMAL (L"The local jitter can be used as a measure of voice quality; "
	"it is the most common jitter measurement and is usually expressed as a percentage. See @@Voice 2. Jitter@.")
ENTRY (L"3. Algorithm")
NORMAL (L"(In the following the term %absolute means two different things: (1) the absolute (i.e. non-negative) value of a real number, "
	"and (2) the opposite of %relative.)")
NORMAL (L"The local jitter is defined as the relative mean absolute "
	"second-order difference of the point process (= the first-order difference of the interval process), as follows.")
NORMAL (L"First, we define the absolute (non-relative) local jitter (in seconds) as the mean absolute (non-negative) "
	"difference of consecutive intervals:")
FORMULA (L"%jitter(seconds) = \\su__%i=2_^^%N^ |%T__%i_ - %T__%i-1_| / (%N - 1)")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - %T__%i-1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 2, the result of the command is @undefined).")
NORMAL (L"Second, we define the mean period as")
FORMULA (L"%meanPeriod(seconds) = \\su__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute local jitter.")
NORMAL (L"Finally, we compute the (relative) local jitter as")
FORMULA (L"%jitter = %jitter(seconds) / %meanPeriod(seconds)")
NORMAL (L"The result is a value between 0 and 2, or between 0 and 200 percent.")
MAN_END

MAN_BEGIN (L"PointProcess: Get jitter (local, absolute)...", L"ppgb", 20110220)
INTRO (L"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
NORMAL (L"This command will write into the Info window "
	"the %%absolute local jitter%, which is the average absolute difference between consecutive intervals, "
	"in seconds (an interval is the time between two consecutive points).")
NORMAL (L"As %jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (L"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), L""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (local, absolute)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TAG (L"##Time range (s)")
DEFINITION (L"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (L"##Period floor (s)")
DEFINITION (L"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (L"##Period ceiling (s)")
DEFINITION (L"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (L"##Maximum period factor")
DEFINITION (L"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (L"2. Usage")
NORMAL (L"The local jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
ENTRY (L"3. Algorithm")
NORMAL (L"The absolute local jitter is defined as the absolute (i.e. non-relative) mean absolute (i.e. non-negative) "
	"second-order difference of the point process (= the first-order difference of the interval process), as follows.")
NORMAL (L"The absolute local jitter (in seconds) is the mean absolute (non-negative) "
	"difference of consecutive intervals:")
FORMULA (L"%jitter(seconds) = \\su__%i=2_^^%N^ |%T__%i_ - %T__%i-1_| / (%N - 1)")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - %T__%i-1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 2, the result of the command is @undefined).")
MAN_END

MAN_BEGIN (L"PointProcess: Get jitter (rap)...", L"ppgb", 20110302)
INTRO (L"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
NORMAL (L"This command will write into the Info window the %%Relative Average Perturbation% (RAP), "
	"a jitter measure defined as the average absolute difference between an interval and the average of it and its two neighbours, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (L"As jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (L"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), L""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (rap)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TAG (L"##Time range (s)")
DEFINITION (L"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (L"##Period floor (s)")
DEFINITION (L"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (L"##Period ceiling (s)")
DEFINITION (L"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (L"##Maximum period factor")
DEFINITION (L"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (L"2. Usage")
NORMAL (L"The RAP can be used as a measure of voice quality; "
	"it is the second most common jitter measurement (after @@PointProcess: Get jitter (local)...|local jitter@). See @@Voice 2. Jitter@.")
ENTRY (L"3. Algorithm")
NORMAL (L"Relative Average Perturbation is defined in terms of three consecutive intervals, as follows.")
NORMAL (L"First, we define the absolute (i.e. non-relative) Average Perturbation (in seconds):")
FORMULA (L"%absAP(seconds) = \\su__%i=2_^^%N-1^ |%T__%i_ - (%T__%i-1_ + %T__%i_ + %T__%i+1_) / 3| / (%N - 2)")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ or %T__%i+1_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - (%T__%i-1_ + %T__%i_ + %T__%i+1_) / 3| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 3, the result of the command is @undefined).")
NORMAL (L"Second, we define the mean period as")
FORMULA (L"%meanPeriod(seconds) = \\su__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute average perturbation.")
NORMAL (L"Finally, we compute the Relative Average Perturbation as")
FORMULA (L"%RAP = %absAP(seconds) / %meanPeriod(seconds)")
NORMAL (L"The result is a value between 0 and 2, or between 0 and 200 percent.")
MAN_END

MAN_BEGIN (L"PointProcess: Get jitter (ppq5)...", L"ppgb", 20110302)
INTRO (L"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
NORMAL (L"This command will write into the Info window the %%five-point Period Perturbation Quotient%, "
	"a jitter measure defined as the average absolute difference between an interval and the average of it and its four closest neighbours, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (L"As jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (L"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), L""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (rap)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TAG (L"##Time range (s)")
DEFINITION (L"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (L"##Period floor (s)")
DEFINITION (L"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (L"##Period ceiling (s)")
DEFINITION (L"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (L"##Maximum period factor")
DEFINITION (L"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (L"2. Usage")
NORMAL (L"The jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
ENTRY (L"3. Algorithm")
NORMAL (L"The five-point Period Perturbation Quotient (PPQ5) is defined in terms of five consecutive intervals, as follows.")
NORMAL (L"First, we define the absolute (i.e. non-relative) PPQ5 (in seconds):")
FORMULA (L"%absPPQ5(seconds) = \\su__%i=3_^^%N-2^ |%T__%i_ - (%T__%i-2_ + %T__%i-1_ + %T__%i_ + %T__%i+1_ + %T__%i+2_) / 5| / (%N - 4)")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-2_ or %T__%i-1_ or %T__%i_ or %T__%i+1_ or %T__%i+2_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-2_/%T__%i-1_ or %T__%i-1_/%T__%i-2_ or %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ or %T__%i+2_/%T__%i+1_ or %T__%i+1_/%T__%i+2_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - (%T__%i-2_ + %T__%i-1_ + %T__%i_ + %T__%i+1_ + %T__%i+2_) / 5| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 5, the result of the command is @undefined).")
NORMAL (L"Second, we define the mean period as")
FORMULA (L"%meanPeriod(seconds) = \\su__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute PPQ5.")
NORMAL (L"Finally, we compute the five-point Period Perturbation Quotient as")
FORMULA (L"%PPQ5 = %PPQ5(seconds) / %meanPeriod(seconds)")
NORMAL (L"The result is a value between 0 and 4, or between 0 and 400 percent.")
MAN_END

MAN_BEGIN (L"PointProcess: Get jitter (ddp)...", L"ppgb", 20110302)
INTRO (L"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
NORMAL (L"This command will write into the Info window the %%Difference of Differences of Periods%, "
	"a jitter measure defined as the average absolute difference between the consecutives differences between consecutive intervals, "
	"divided by the average interval (an interval is the time between two consecutive points).")
NORMAL (L"As jitter is often used as a measure of voice quality (see @@Voice 2. Jitter@), "
	"the intervals are often considered to be %%glottal periods%. "
	"For this reason, the command has settings that can limit the possible duration of the interval (or period) "
	"or the possible difference in the durations of consecutive intervals (periods).")
ENTRY (L"1. The command window")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), L""
	Manual_DRAW_SETTINGS_WINDOW ("PointProcess: Get jitter (rap)", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period floor (s)", "0.0001")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Period ceiling (s)", "0.02")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Maximum period factor", "1.3")
)
TAG (L"##Time range (s)")
DEFINITION (L"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (L"##Period floor (s)")
DEFINITION (L"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (L"##Period ceiling (s)")
DEFINITION (L"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (L"##Maximum period factor")
DEFINITION (L"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (L"2. Usage")
NORMAL (L"The jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
ENTRY (L"3. Algorithm")
NORMAL (L"(In the following the term %absolute means two different things: (1) the absolute (i.e. non-negative) value of a real number, "
	"and (2) the opposite of %relative.)")
NORMAL (L"DDP is defined as the relative mean absolute (i.e. non-negative) "
	"third-order difference of the point process (= the second-order difference of the interval process), as follows.")
NORMAL (L"First, we define the absolute (i.e. non-relative) Average Perturbation (in seconds) as one third of the mean absolute (non-negative) "
	"difference of difference of consecutive intervals:")
FORMULA (L"%absDDP(seconds) = \\su__%i=2_^^%N-1^ |(%T__%i+1_ - %T__%i_) - (%T__%i_ - %T__%i-1_)| / (%N - 2)")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ or %T__%i+1_ is not between ###Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term |2%T__%i_ - %T__%i-1_ - %T__%i+1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 3, the result of the command is @undefined).")
NORMAL (L"Second, we define the mean period as")
FORMULA (L"%meanPeriod(seconds) = \\su__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (L"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of DDP.")
NORMAL (L"Finally, we compute DDP as")
FORMULA (L"%DDP = %absDDP(seconds) / %meanPeriod(seconds)")
NORMAL (L"The result is exactly 3 times the @@PointProcess: Get jitter (rap)...|RAP@ jitter measurement: "
	"a value between 0 and 6, or between 0 and 600 percent.")
MAN_END

MAN_BEGIN (L"PointProcess: Get low index...", L"ppgb", 20021212)
INTRO (L"A @query to the selected @PointProcess object.")
ENTRY (L"Return value")
NORMAL (L"the index of the nearest point before or at the specified time, "
	"or 0 if the point process contains no points or the specified time is before the first point.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time from which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (L"PointProcess: Get nearest index...", L"ppgb", 20021212)
INTRO (L"A @query to the selected @PointProcess object.")
ENTRY (L"Return value")
NORMAL (L"the index of the point nearest to the specified time, "
	"or 0 if the point process contains no points.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time around which a point is looked for, in seconds.")
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

MAN_BEGIN (L"PointProcess: Hum", L"ppgb", 19970330)
INTRO (L"A command to hear a @PointProcess.")
ENTRY (L"Algorithm")
NORMAL (L"A @Sound is created with the algorithm described at @@PointProcess: To Sound (hum)...@.")
NORMAL (L"This sound is then played.")
MAN_END

MAN_BEGIN (L"PointProcesses: Intersection", L"ppgb", 20021212)
INTRO (L"A command to merge two selected @PointProcess objects into one.")
ENTRY (L"Behaviour")
NORMAL (L"The resulting #PointProcess will contain only those points that occur in both original point processes.")
NORMAL (L"The time domain of the resulting point process is the intersection of the time domains of the original point processes.")
MAN_END

MAN_BEGIN (L"PointProcess: Play", L"ppgb", 19970330)
INTRO (L"A command to hear a @PointProcess.")
ENTRY (L"Algorithm")
NORMAL (L"A @Sound is created with the algorithm described at @@PointProcess: To Sound (pulse train)...@.")
NORMAL (L"This sound is then played.")
MAN_END

MAN_BEGIN (L"PointProcess: Remove point...", L"ppgb", 20021212)
INTRO (L"A command to remove a point from every selected @PointProcess.")
ENTRY (L"Setting")
TAG (L"##Index")
DEFINITION (L"the index of the point that is to be removed.")
ENTRY (L"Behaviour")
NORMAL (L"Does nothing if %index is less than 1 or greater than the number of points %nt in the point process. "
	"Otherwise, one point is removed (e.g., if %index is 3, the third point is removed), and the other points stay the same.")
MAN_END

MAN_BEGIN (L"PointProcess: Remove point near...", L"ppgb", 20021212)
INTRO (L"A command to remove a point from every selected @PointProcess.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time (in seconds) around which a point is to be removed.")
ENTRY (L"Behaviour")
NORMAL (L"Does nothing if there are no points in the point process. "
	"Otherwise, the point nearest to %time is removed, and the other points stay the same.")
MAN_END

MAN_BEGIN (L"PointProcess: Remove points...", L"ppgb", 20021212)
INTRO (L"A command to remove a range of points from every selected @PointProcess.")
ENTRY (L"Settings")
TAG (L"##From index (\\>_ 1)")
DEFINITION (L"the first index of the range of points that are to be removed.")
TAG (L"##To index")
DEFINITION (L"the last index of the range of points that are to be removed.")
ENTRY (L"Behaviour")
NORMAL (L"All points that originally fell in the range [%fromIndex, %toIndex] are removed, and the other points stay the same.")
MAN_END

MAN_BEGIN (L"PointProcess: Remove points between...", L"ppgb", 20021212)
INTRO (L"A command to remove a range of points from every selected @PointProcess.")
ENTRY (L"Settings")
TAG (L"##From time (s)")
DEFINITION (L"the start of the domain from which all points are to be removed.")
TAG (L"##To time (s)")
DEFINITION (L"the end of the domain from which all points are to be removed.")
ENTRY (L"Behaviour")
NORMAL (L"All points that originally fell in the domain [%fromTime, %toTime], including the edges, are removed, "
	"and the other points stay the same.")
MAN_END

MAN_BEGIN (L"PointProcess: To Sound (hum)...", L"ppgb", 19970330)
INTRO (L"A command to convert every selected @PointProcess into a @Sound.")
ENTRY (L"Algorithm")
NORMAL (L"A @Sound is created with the algorithm described at @@PointProcess: To Sound (pulse train)...@. "
	"This sound is then run through a sequence of second-order filters that represent five formants.")
MAN_END

MAN_BEGIN (L"PointProcess: To Sound (phonation)...", L"ppgb", 20070225)
INTRO (L"A command to convert every selected @PointProcess into a @Sound.")
ENTRY (L"Algorithm")
NORMAL (L"A glottal waveform is generated at every point in the point process. "
	"Its shape depends on the settings %power1 and %power2 according to the formula")
FORMULA (L"%U(%x) = %x^^%power1^ - %x^^%power2^")
NORMAL (L"where %x is a normalized time that runs from 0 to 1 and %U(%x) is the normalized glottal flow in arbitrary units (the real unit is m^3/s). "
	"If %power1 = 2.0 and %power2 = 3.0, the glottal flow shape is that proposed by @@Rosenberg (1971)@, "
	"upon which for instance the Klatt synthesizer is based (@@Klatt & Klatt (1990)@):")
SCRIPT (4.5, 3,
	L"Axes... 0 1 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (x^2-x^3)*6\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
NORMAL (L"If %power1 = 3.0 and %power2 = 4.0, the glottal flow shape starts somewhat smoother, "
	"reflecting the idea that the glottis opens like a zipper:")
SCRIPT (4.5, 3,
	L"Axes... 0 1 -0.1 1\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (x^3-x^4)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow\n"
)
NORMAL (L"For the generation of speech sounds, we do not take the glottal flow itself, "
	"but rather its derivative (this takes into account the influence of raditaion at the lips). "
	"The glottal flow derivative is given by")
FORMULA (L"%dU(%x)/%dx = %power1 %x^^(%power1-1)^ - %power2 %x^^(%power2-1)^")
NORMAL (L"The flow derivative clearly shows the influence of the smoothing mentioned above. "
	"The unsmoothed curve, with %power1 = 2.0 and %power2 = 3.0, looks like:")
SCRIPT (4.5, 4,
	L"Axes... 0 1 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (2*x-3*x^2)*6\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL (L"Unlike the unsmoothed curve, the smoothed curve, with %power1 = 3.0 and %power2 = 4.0, starts out horizontally:")
SCRIPT (4.5, 4,
	L"Axes... 0 1 -9 3\n"
	"One mark left... 0 yes yes yes\n"
	"One mark bottom... 0 yes yes no\n"
	"One mark bottom... 1 yes yes no\n"
	"Draw inner box\n"
	"Draw function... 0 1 1000 (3*x^2-4*x^3)*8\n"
	"Text bottom... yes Time (normalized)\n"
	"Text left... yes Glottal flow derivative\n"
)
NORMAL (L"Another setting is the %%open phase%. If it is 0.70, the glottis will be open during 70 percent of a period. "
	"Suppose that the PointProcess has a pulse at time 0, at time 1, at time 2, and so on. The pulses at times 1 and 2 will then be turned "
	"into glottal flows starting at times 0.30 and 1.30:")
SCRIPT (4.5, 2.5,
	L"Axes... 0 2 -0.1 1\n"
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
	L"Axes... 0 2 -9 3\n"
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
NORMAL (L"The final setting that influences the shape of the glottal flow is the %%collision phase%. "
	"If it is 0.03, for instance, the glottal flow derivative will not go abruptly to 0 at a pulse, "
	"but will instead decay by a factor of %e (\\~~ 2.7183) every 3 percent of a period. "
	"In order to keep the glottal flow curve smooth (and the derivative continuous), "
	"the basic shape discussed above has to be shifted slightly to the right and truncated "
	"at the time of the pulse, to be replaced there with the exponential decay curve; "
	"this also makes sure that the average of the derivative stays zero, as it was above "
	"(i.e. the area under the positive part of the curve equals the area above the negative part). "
	"This is what the curves look like if %power1 = 3.0, %power2 = 4.0, %openPhase = 0.70 and %collisionPhase = 0.03:")
SCRIPT (4.5, 2.5,
	L"Axes... 0 2 -0.1 1\n"
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
	L"Axes... 0 2 -9 3\n"
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
NORMAL (L"These curves have moved 2.646 percent of a period to the right. At time 1, "
	"the glottal flow curve turns from a convex polynomial into a concave exponential, "
	"and the derivative still has its minimum there.")
ENTRY (L"Settings")
TAG (L"##Sampling frequency (Hz)")
DEFINITION (L"the sampling frequency of the resulting Sound object, e.g. 44100 hertz.")
TAG (L"##Adaptation factor")
DEFINITION (L"the factor by which a pulse height will be multiplied if the pulse time is not within "
	"##Maximum period# from the previous pulse, and by which a pulse height will again be multiplied "
	"if the previous pulse time is not within ##Maximum period# from the pre-previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TAG (L"##Maximum period (s)")
DEFINITION (L"the minimal period that will be considered a silence, e.g. 0.05 seconds. "
	"Example: if ##Adaptation factor# is 0.6, and ##Adaptation time# is 0.02 s, "
	"then the heights of the first two pulses after silences of at least 20 ms "
	"will be multiplied by 0.36 and 0.6, respectively.")
MAN_END

MAN_BEGIN (L"PointProcess: To Sound (pulse train)...", L"ppgb", 20070225)
INTRO (L"A command to convert every selected @PointProcess into a @Sound.")
ENTRY (L"Algorithm")
NORMAL (L"A pulse is generated at every point in the point process. This pulse is filtered at the Nyquist frequency "
	"of the resulting #Sound by converting it into a sampled #sinc function.")
ENTRY (L"Settings")
TAG (L"##Sampling frequency (Hz)")
DEFINITION (L"the sampling frequency of the resulting Sound object, e.g. 44100 hertz.")
TAG (L"##Adaptation factor")
DEFINITION (L"the factor by which a pulse height will be multiplied if the pulse time is not within "
	"##Adaptation time# from the pre-previous pulse, and by which a pulse height will again be multiplied "
	"if the pulse time is not within ##Adaptation time# from the previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TAG (L"##Adaptation time (s)")
DEFINITION (L"the minimal period that will be considered a silence, e.g. 0.05 seconds.")
TAG (L"##Interpolation depth")
DEFINITION (L"the extent of the sinc function to the left and to the right of the peak, e.g. 2000 samples.")
NORMAL (L"Example: if ##Adaptation factor# is 0.6, and ##Adaptation time# is 0.02 s, "
	"then the heights of the first two pulses after silences of at least 20 ms "
	"will be multiplied by 0.36 and 0.6, respectively.")
MAN_END

MAN_BEGIN (L"PointProcesses: Union", L"ppgb", 20021212)
INTRO (L"A command to merge two selected @PointProcess objects into one.")
ENTRY (L"Behaviour")
NORMAL (L"The resulting #PointProcess will contain all the points of the two original point processes, sorted by time. "
	"Points that occur in both original point processes, will occur only once in the resulting point process.")
NORMAL (L"The time domain of the resulting point process is the union of the time domains of the original point processes.")
MAN_END

MAN_BEGIN (L"PointProcess: Up to IntensityTier...", L"ppgb", 19970329)
INTRO (L"A command to promote every selected @PointProcess to an @IntensityTier.")
ENTRY (L"Setting")
TAG (L"##Intensity (dB)")
DEFINITION (L"the intensity that will be associated with every point.")
ENTRY (L"Behaviour")
NORMAL (L"The times of all the points are trivially copied, and so is the time domain. "
	"The intensity information will be the same for every point.")
MAN_END

MAN_BEGIN (L"PointProcess: Up to PitchTier...", L"ppgb", 19970329)
INTRO (L"A command to promote every selected @PointProcess to a @PitchTier.")
ENTRY (L"Setting")
TAG (L"##Frequency (Hz)")
DEFINITION (L"the pitch frequency that will be associated with every point.")
ENTRY (L"Behaviour")
NORMAL (L"The times of all the points are trivially copied, and so is the time domain. "
	"The pitch information will be the same for every point.")
MAN_END

MAN_BEGIN (L"Polygon", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"A Polygon object represents a sequence of points (%%x__i_, %%y__i_) in a two-dimensional space.")
MAN_END

MAN_BEGIN (L"Read Matrix from raw text file...", L"ppgb", 19980322)
INTRO (L"A command to read a @Matrix object from a file on disk.")
ENTRY (L"File format")
NORMAL (L"The file should contain each row of the matrix on a separate line. Within each row, "
	"the elements must be separated by spaces or tabs.")
NORMAL (L"For instance, the following text file will be read as a Matrix with three rows and four columns:")
CODE (L"0.19 3 245 123")
CODE (L"18e-6 -3e18 0 0.0")
CODE (L"1.5 2.5 3.5 4.5")
NORMAL (L"The resulting Matrix will have the same domain and sampling as Matrices created with "
	"##Create simple Matrix...#. In the above example, this means that the Matrix will have "
	"%x__%min_ = 0.5, %x__%max_ = 4.5, %n__%x_ = 4, %dx = 1.0, %x__1_ = 1.0, "
	"%y__%min_ = 0.5, %y__%max_ = 3.5, %n__%y_ = 3, %dy = 1.0, %y__1_ = 1.0.")
MAN_END

MAN_BEGIN (L"Read Strings from raw text file...", L"ppgb", 19990502)
INTRO (L"A command to read a @Strings object from a simple text file. "
	"Each line is read as a separate string. See @Strings for an example.")
MAN_END

MAN_BEGIN (L"Sound: To Intensity...", L"ppgb", 20100605)
INTRO (L"A command to create an @Intensity object from every selected @Sound.")
ENTRY (L"Settings")
TAG (L"##Minimum pitch (Hz)")
DEFINITION (L"the minimum periodicity frequency in your signal. If you set it too high, "
	"you will end up with a pitch-synchronous intensity modulation. If you set it too low, "
	"your intensity contour may appear smeared, so you should set it as high as allowed by the signal "
	"if you want a sharp contour.")
TAG (L"##Time step (s)")
DEFINITION (L"the time step of the resulting intensity contour. If you set it to zero, the time step is computed as "
	"one quarter of the effective window length, i.e. as 0.8 / (%minimum_pitch).")
TAG (L"##Subtract mean")
DEFINITION (L"See @@Intro 6.2. Configuring the intensity contour@.")
ENTRY (L"Algorithm")
NORMAL (L"The values in the sound are first squared, then convolved with a Gaussian analysis window (Kaiser-20; sidelobes below -190 dB). "
	"The effective duration of this analysis window is 3.2 / (%minimum_pitch), which will guarantee that a periodic signal is analysed as having a "
	"pitch-synchronous intensity ripple not greater than 0.00001 dB.")
MAN_END

MAN_BEGIN (L"Sound & IntensityTier: Multiply", L"ppgb", 20000724)
INTRO (L"A command to create a new Sound from the selected @Sound and @Intensity objects.")
NORMAL (L"The resulting Sound equals the original sound, multiplied by a linear interpolation of the intensity. "
	"Afterwards, the resulting Sound is scaled so that its maximum absolute amplitude is 0.9.")
MAN_END

MAN_BEGIN (L"Strings", L"ppgb", 20041110)
INTRO (L"One of the @@types of objects@ in Praat. Represents an ordered list of strings.")
ENTRY (L"Creation")
NORMAL (L"The difficult way is to create a #Strings object from a generic Praat text file:")
CODE (L"\"ooTextFile\"")
CODE (L"\"Strings\"")
CODE (L"5 ! number of strings")
CODE (L"\"Hello\"")
CODE (L"\"Goodbye\"")
CODE (L"\"Auf wiedersehen\"")
CODE (L"\"Tsch\\bsu\\\" \\\" ss\"")
CODE (L"\"Arrivederci\"")
NORMAL (L"In this example, we see that a double quote within a string should be written twice; "
	"the fourth string will therefore be read as ##Tsch\\bsu\\\" ss#, "
	"and will be shown in info messages or in graphical text as ##Tsch\\u\"ss# (see @@special symbols@). "
	"This file can be read simply with the generic @@Read from file...@ command from the #Open menu.")
NORMAL (L"An easier way is to use the special command @@Read Strings from raw text file...@. "
	"The file can then simply look like this:")
CODE (L"Hello")
CODE (L"Goodbye")
CODE (L"Auf wiedersehen")
CODE (L"Tsch\\bsu\\\" ss")
CODE (L"Arrivederci")
NORMAL (L"In this example, all the strings are in the generic system-independent ASCII format that is used "
	"everywhere in Praat (messages, graphical text) "
	"for @@special symbols@. You could also have supplied the strings in a native format, which is "
	"ISO-Latin1 encoding on Unix and Windows computers, or Mac encoding on Macintosh computers. "
	"The file would then have simply looked like:")
CODE (L"Hello")
CODE (L"Goodbye")
CODE (L"Auf wiedersehen")
CODE (L"Tsch\\u\"ss")
CODE (L"Arrivederci")
NORMAL (L"To convert this into the generic system-independent ASCII format, use the #Genericize command.")
NORMAL (L"You can also create a #Strings object from a directory listing or from some other objects:")
LIST_ITEM (L"\\bu @@Create Strings as file list...")
LIST_ITEM (L"\\bu @@Distributions: To Strings...@")
LIST_ITEM (L"\\bu @@OTGrammar: Generate inputs...@")
LIST_ITEM (L"\\bu @@OTGrammar & Strings: Inputs to outputs...@")
MAN_END

MAN_BEGIN (L"Strings: To Distributions", L"ppgb", 19971025)
INTRO (L"A command to analyse each selected @Strings object into a @Distributions object.")
NORMAL (L"The resulting #Distributions will collect the occurrences of every string in the #Strings object, "
	"and put the number of occurrences in its first and only column.")
ENTRY (L"Example")
NORMAL (L"We start from the following #Strings:")
CODE (L"6 (number of strings)")
CODE (L"\"hallo\"")
CODE (L"\"dag allemaal\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
CODE (L"\"hallo\"")
CODE (L"\"tot morgen\"")
NORMAL (L"This will give us the following #Distributions:")
CODE (L"1 (number of columns) \"\" (no column name)")
CODE (L"\"hallo\"         3")
CODE (L"\"dag allemaal\"  1")
CODE (L"\"tot morgen\"    2")
MAN_END

MAN_BEGIN (L"Table", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat. See the @Statistics tutorial.")
MAN_END

MAN_BEGIN (L"TableOfReal", L"ppgb", 20030316)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"A TableOfReal object contains a number of %cells. Each cell belongs to a %row and a %column. "
	"For instance, a TableOfReal with 10 rows and 3 columns has 30 cells.")
NORMAL (L"Each row and each column may be labeled with a %title.")
ENTRY (L"Creating a TableOfReal from data in a text file")
NORMAL (L"Suppose you have F1 and F2 data for vowels. "
	"You can create a simple text file like the following:")
CODE (L"\"ooTextFile\"  ! The line by which Praat can recognize your file")
CODE (L"\"TableOfReal\" ! The line that tells Praat about the contents")
CODE (L"2   \"F1\"  \"F2\"      ! Number of columns, and column labels")
CODE (L"3                   ! Number of rows")
CODE (L"\"a\" 800 1100         ! Row label (vowel), F1 value, F2 value")
CODE (L"\"i\" 280 2800         ! Row label (vowel), F1 value, F2 value")
CODE (L"\"u\" 260  560         ! Row label (vowel), F1 value, F2 value")
NORMAL (L"Praat is rather forgiving about the use of spaces, tabs, and newlines. "
	"See @@Save as text file...@ for general information.")
NORMAL (L"You will often have your data in a file with a self-describing format, "
	"i.e. in which the number of values on a line equals the number of columns "
	"of the table:")
CODE (L"800 1100")
CODE (L"280 2800")
CODE (L"260 560")
NORMAL (L"Such a file can be read with @@Read Matrix from raw text file...@. "
	"This creates a Matrix object, which can be cast to a TableOfReal object "
	"by @@Matrix: To TableOfReal@. The resulting TableOfReal does not have "
	"any row or column labels yet. You could add column labels with:")
CODE (L"Set column label (index)... 1 F1")
CODE (L"Set column label (index)... 2 F2")
NORMAL (L"Of course, if the row labels contain crucial information, "
	"and the number of rows is large, this is not a feasible method.")
MAN_END

MAN_BEGIN (L"TableOfReal: Set value...", L"ppgb", 19980105)
INTRO (L"A command to change the value of one table cell in each selected @TableOfReal object.")
ENTRY (L"Settings")
TAG (L"##Row number")
DEFINITION (L"the number of the row of the cell whose value you want to change.")
TAG (L"##Column number")
DEFINITION (L"the number of the column of the cell whose value you want to change.")
TAG (L"##New value")
DEFINITION (L"the value that you want the specified cell to have.")
MAN_END

}

/* End of file manual_Fon.cpp */
