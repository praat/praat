/* manual_Fon.cpp
 *
 * Copyright (C) 1992-2008,2010,2011,2014-2017 Paul Boersma
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

static void draw_CreateDurationTier (Graphics g) {
	static double x [] = { 0.0, 0.3, 0.6, 0.7, 0.9 }, y [] = { 1.0, 1.0, 2.3, 1.0, 1.0 };
	Graphics_setWindow (g, 0.0, 0.9, 0.0, 2.5);
	Graphics_drawInnerBox (g);
	Graphics_marksLeftEvery (g, 1.0, 1.0, true, true, false);
	Graphics_marksBottomEvery (g, 1.0, 0.1, true, true, false);
	Graphics_setInner (g);
	Graphics_setColour (g, Melder_RED);
	Graphics_polyline (g, 5, x, y);
	Graphics_setColour (g, Melder_BLACK);
	Graphics_unsetInner (g);
	Graphics_textBottom (g, true, U"Time (s)");
	Graphics_textLeft (g, true, U"Relative duration");
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

MAN_BEGIN (U"Get high index from time...", U"ppgb", 20101230)
INTRO (U"A @query to ask the selected tier object "
	"(@DurationTier, @IntensityTier, @PitchTier) "
	"which point is nearest to, but no earlier than, the specified time.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time from which you want to get the point index.")
ENTRY (U"Return value")
NORMAL (U"This query returns the index of the point with the lowest time greater than or equal to #Time. "
	"It is @undefined if there are no points. "
	"It is the number of points plus 1 (offright) if the specified time is greater than the time of the last point.")
MAN_END

MAN_BEGIN (U"Get low index from time...", U"ppgb", 20101230)
INTRO (U"A @query to ask the selected tier object "
	"(@DurationTier, @IntensityTier, @PitchTier) "
	"which point is nearest to, but no later than, the specified time.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time from which you want to get the point index.")
ENTRY (U"Return value")
NORMAL (U"This query returns the index of the point with the highest time less than or equal to #Time. "
	"It is @undefined if there are no points. "
	"It is 0 (offleft) if the specified time is less than the time of the first point.")
MAN_END

MAN_BEGIN (U"Get nearest index from time...", U"ppgb", 20101230)
INTRO (U"A @query to ask the selected tier object "
	"(@DurationTier, @IntensityTier, @PitchTier) "
	"which point is nearest to the specified time.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time near which you want to get the point index.")
ENTRY (U"Return value")
NORMAL (U"This query returns the index of the point with the highest time less than or equal to #Time. "
	"It is @undefined if there are no points.")
MAN_END

MAN_BEGIN (U"Remove point...", U"ppgb", 20101230)
INTRO (U"A command to remove one point from every selected time-based tier object "
	"(@DurationTier, @IntensityTier, @PitchTier).")
ENTRY (U"Setting")
TAG (U"##Point number")
DEFINITION (U"the index of the point you want to remove.")
ENTRY (U"Behaviour")
NORMAL (U"If ##Point number# is 3, the third point counted from the start of the tier (if it exists) "
	"is removed from the tier.")
MAN_END

MAN_BEGIN (U"Remove point near...", U"ppgb", 20101230)
INTRO (U"A command to remove one point from every selected time-based tier object "
	"(@DurationTier, @IntensityTier, @PitchTier).")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time near which you want to remove a point.")
ENTRY (U"Behaviour")
NORMAL (U"The point nearest to #Time (if there is any point) is removed from the tier.")
MAN_END

MAN_BEGIN (U"Remove points between...", U"ppgb", 20101230)
INTRO (U"A command to remove some points from every selected time-based tier object "
	"(@DurationTier, @IntensityTier, @PitchTier).")
ENTRY (U"Settings")
TAG (U"##From time (s)")
TAG (U"##To time (s)")
DEFINITION (U"the times between which you want to remove all points.")
ENTRY (U"Behaviour")
NORMAL (U"Any points between ##Frome time# and ##To Time# (inclusive) are removed from the tier.")
MAN_END

MAN_BEGIN (U"AmplitudeTier", U"ppgb", 20070825)
INTRO (U"One of the @@types of objects@ in Praat. "
	"An AmplitudeTier object represents a time-stamped amplitude contour, i.e., it contains a series of (%time, %amplitude) points. "
	"The amplitude values are in Pascal. To see some applications, consult the @IntensityTier information; "
	"the difference between an AmplitudeTier and an IntensityTier is that the former has values in Pascal "
	"which multiply linearly with a Sound (for instance), and the latter has values in dB, "
	"which multiply logarithmically with a Sound.")
MAN_END

MAN_BEGIN (U"Cochleagram", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. It represents the excitation pattern "
	"of the basilar membrane in the inner ear (see @Excitation) as a function of time.")
MAN_END

MAN_BEGIN (U"Cochleagram: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Cochleagram objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Create DurationTier...", U"ppgb", 20140421)
INTRO (U"A command in the @@New menu@ to create an empty @DurationTier object.")
NORMAL (U"The resulting object will have the specified name and time domain, but contain no duration points. "
	"To add some points to it, use @@DurationTier: Add point...@.")
ENTRY (U"Scripting example")
NORMAL (U"To create a tier 0.9 seconds long, with an deceleration around 0.6 seconds, you do:")
CODE (U"Create DurationTier: \"dur\", 0, 0.9")
CODE (U"Add point: 0.3, 1")
CODE (U"Add point: 0.6, 2.3")
CODE (U"Add point: 0.7, 1")
NORMAL (U"The result will look like")
PICTURE (5, 2.5, draw_CreateDurationTier)
NORMAL (U"The target duration will be the area under this curve, which is 0.9 + 1/2 · 1.3 · 0.4 = 1.16 seconds.")
MAN_END

MAN_BEGIN (U"Create empty PointProcess...", U"ppgb", 20021204)
INTRO (U"A command in the @@New menu@ to create an empty @PointProcess. The newly created object is put in the list of objects.")
MAN_END

MAN_BEGIN (U"Create IntensityTier...", U"ppgb", 20021204)
INTRO (U"A command in the @@New menu@ to create an empty @IntensityTier object.")
NORMAL (U"The resulting object will have the specified name and time domain, but contain no formant points. "
	"To add some points to it, use @@IntensityTier: Add point...@.")
NORMAL (U"For an example, see @@Source-filter synthesis@.")
MAN_END

MAN_BEGIN (U"Create Matrix...", U"ppgb", 20021212)
INTRO (U"A command in the @@New menu@ to create a @Matrix with the specified sampling attributes, "
	"filled with values from a formula (see @@Matrix: Formula...@).")
MAN_END

MAN_BEGIN (U"Create Poisson process...", U"ppgb", 20041005)
INTRO (U"A command to create a @PointProcess object that represents a Poisson process.")
NORMAL (U"A Poisson process is a stationary point process with a fixed density %λ, "
	"which means that there are, on the average, %λ events per second.")
ENTRY (U"Settings")
TAG (U"##Start time (s)")
DEFINITION (U"%t__%min_, the beginning of the time domain, in seconds.")
TAG (U"##End time (s)")
DEFINITION (U"%t__%max_, the end of the time domain, in seconds.")
TAG (U"##Density (Hz)")
DEFINITION (U"the average number of points per second.")
ENTRY (U"Algorithm")
NORMAL (U"First, the number of points %N in the time domain is determined. Its expectation value is")
FORMULA (U"%λ = (%t__%max_ – %t__%min_) · %density")
NORMAL (U"but its actual value is taken from the Poisson distribution:")
FORMULA (U"%p(%n) = (%%λ^n% / %n!) %e^^–%λ")
NORMAL (U"Then, %N points are computed throughout the time domain, according to a uniform distribution:")
FORMULA (U"%p(%t) = 1 / (%t__%max_ – %t__%min_)   for %t ∈ [%t__%min_, %t__%max_]")
FORMULA (U"%p(%t) = 0   outside [%t__%min_, %t__%max_]")
MAN_END

MAN_BEGIN (U"Create simple Matrix...", U"ppgb", 20021204)
INTRO (U"A command in the @@New menu@ to create a @Matrix with the specified number of rows and columns, "
	"filled with values from a formula (see @@Matrix: Formula...@).")
MAN_END

MAN_BEGIN (U"Create Strings as folder list...", U"ppgb", 20201226)
INTRO (U"A command in the @@New menu@ to create a @Strings object containing a list of directories in a given parent directory. "
	"It works completely analogously to @@Create Strings as file list...@.")
MAN_END

MAN_BEGIN (U"Create Strings as file list...", U"ppgb", 20201229)
INTRO (U"A command in the @@New menu@ to create a @Strings object containing a list of files in a given folder.")
ENTRY (U"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (2.6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Create Strings as file list", 2.6)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Name", "fileList")
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("File path", "/Users/miep/Sounds/*.wav")
)
TAG (U"##Name")
DEFINITION (U"the name of the resulting Strings object.")
TAG (U"##File path")
DEFINITION (U"the folder name, with an optional %wildcard (see below) for selecting files.")
ENTRY (U"Behaviour")
NORMAL (U"The resulting Strings object will contain an alphabetical list of file names, "
	"without the preceding path through the folder structures. If there are no files that match the file path, "
	"the Strings object will contain no strings.")
ENTRY (U"Usage")
NORMAL (U"There are two ways to specify the file path.")
NORMAL (U"One way is to specify a folder name only. On Unix, the file path could be "
	"##/usr/people/miep/sounds# or ##/usr/people/miep/sounds/#, for instance. On Windows, "
	"##C:\\bsUsers\\bsMiep\\bsSounds# or ##C:\\bsUsers\\bsMiep\\bsSounds\\bs#. "
	"On Macintosh, ##/Users/miep/Sounds# or ##/Users/miep/Sounds/#. Any of these produce "
	"a list of all the files in the specified folder.")
NORMAL (U"The other way is to specify a wildcard (a single asterisk) for the file names. "
	"To get a list of all the files whose names start with \"hal\" and end in \".wav\", "
	"type ##/usr/people/miep/sounds/hal*.wav#, ##C:\\bsUsers\\bsMiep\\bsSounds\\bshal*.wav#, "
	"or ##/Users/miep/Sounds/hal*.wav#.")
ENTRY (U"Script usage")
NORMAL (U"In a script, you can use this command to cycle through the files in a folder. "
	"For instance, to read in all the sound files in a specified folder, "
	"you could use the following script:")
CODE (U"folder\\$  = \"/usr/people/miep/sounds\"")
CODE (U"strings = Create Strings as file list: \"list\", folder\\$  + \"/*.wav\"")
CODE (U"numberOfFiles = Get number of strings")
CODE (U"for ifile to numberOfFiles")
	CODE1 (U"selectObject: strings")
	CODE1 (U"fileName\\$  = Get string: ifile")
	CODE1 (U"Read from file: folder\\$  + \"/\" + fileName\\$ ")
CODE (U"endfor")
NORMAL (U"If the script has been saved to a script file, you can use file paths that are relative to the folder "
	"where you saved the script. Thus, with")
CODE (U"Create Strings as file list: \"list\", \"*.wav\"")
NORMAL (U"you get a list of all the .wav files that are in the same folder as the script that contains this line. "
	"And to get a list of all the .wav files in the folder Sounds that resides in the same folder as your script, "
	"you can do")
CODE (U"Create Strings as file list: \"list\", \"Sounds/*.wav\"")
NORMAL (U"As is usual in Praat scripting, the forward slash (\"/\") in this example can be used on all platforms, including Windows. "
	"This makes your script portable across platforms.")
ENTRY (U"See also")
NORMAL (U"To get a list of folders instead of files, use @@Create Strings as folder list...@.")
MAN_END

MAN_BEGIN (U"Distributions", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. Inherits most actions from @TableOfReal.")
ENTRY (U"Actions")
LIST_ITEM (U"@@Distributions: To Strings...@")
MAN_END

MAN_BEGIN (U"Distributions: To Strings...", U"ppgb", 19971022)
INTRO (U"A command to create a @Strings object from every selected @Distributions object.")
ENTRY (U"Settings")
TAG (U"##Column number")
DEFINITION (U"the column (in the #Distributions object) that contains the distribution that you are "
	"interested in. Often the #Distributions object will only contain a single distribution, "
	"so this argument will often be 1. If the #Distributions object contains nine distributions, "
	"specify any number between 1 and 9.")
TAG (U"##Number of strings")
DEFINITION (U"the number of times a string will be drawn from the chosen distribution. "
	"This is the number of strings that the resulting @Strings object is going to contain.")
ENTRY (U"Behaviour")
NORMAL (U"Every string in the resulting #Strings object will be a row label of the #Distributions object. "
	"The number in each row at the specified column will be considered the relative frequency of "
	"occurrence of that row.")
NORMAL (U"%#Example. Suppose we have the following #Distributions:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"Distributions\"")
CODE (U"2 (number of columns)")
CODE (U"         \"English\"  \"French\" (column labels)")
CODE (U"3 (number of rows)")
CODE (U"\"the\"     108        1.5")
CODE (U"\"a\"       58.1       33")
CODE (U"\"pour\"    0.7        15.5")
NORMAL (U"If you set %Column to 1 and %%Number of strings% to 1000, "
	"you will get a @Strings object with approximately 647 occurrences of \"the\", "
	"348 occurrences of \"a\", and 4 occurrences of \"pour\". "
	"If you had set %Column to 2 (\"French\"), you would have gotten about "
	"30 times \"the\", 660 times \"a\", and 310 times \"pour\". "
	"The actual numbers will vary because the choice of a string will not depend on previous choices.")
MAN_END

MAN_BEGIN (U"DurationTier", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. "
	"A DurationTier object contains a number of (%time, %duration) points, "
	"where %duration is to be interpreted as a relative duration (e.g. the duration of a manipulated sound "
	"as compared to the duration of the original). For instance, "
	"if your DurationTier contains two points, one with a duration value of 1.5 at a time of 0.5 seconds "
	"and one with a duration value of 0.6 at a time of 1.1 seconds, this is to be interpreted as "
	"a relative duration of 1.5 (i.e. a slowing down) for all original times before 0.5 seconds, "
	"a relative duration of 0.6 (i.e. a speeding up) for all original times after 1.1 seconds, "
	"and a linear interpolation between 0.5 and 1.1 seconds (e.g. a relative duration of "
	"1.2 at 0.7 seconds, and of 0.9 at 0.9 seconds).")
NORMAL (U"See @@Intro 8.2. Manipulation of duration@ and @@Create DurationTier...@.")
MAN_END

MAN_BEGIN (U"DurationTier: Add point...", U"ppgb", 20030216)
INTRO (U"A command to add a point to each selected @DurationTier. "
	"For an example, see @@Create DurationTier...@.")
ENTRY (U"Settings")
TAG (U"##Time (s)")
DEFINITION (U"the time at which a point is to be added.")
TAG (U"##Relative duration")
DEFINITION (U"the relative duration value of the requested new point.")
ENTRY (U"Behaviour")
NORMAL (U"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (U"DurationTier: Get target duration...", U"ppgb", 20101228)
INTRO (U"A @query to the selected @DurationTier for the target duration of a specified time range.")
ENTRY (U"Settings")
TAG (U"##From time (s)")
TAG (U"##To time (s)")
DEFINITION (U"the start and end of the (original) time range.")
ENTRY (U"Return value")
NORMAL (U"the target duration in seconds.")
MAN_END

MAN_BEGIN (U"DurationTierEditor", U"ppgb", 20110128)
INTRO (U"One of the @editors in the Praat program, for viewing and editing a @DurationTier object. "
	"To create a DurationTierEditor window, select a DurationTier and click ##View & Edit#.")
MAN_END

MAN_BEGIN (U"Editors", U"ppgb", 20110128)
INTRO (U"Many @@types of objects@ in Praat can be viewed and edited in their own windows.")
ENTRY (U"Editor windows")
LIST_ITEM (U"• @SoundEditor")
LIST_ITEM (U"• @LongSoundEditor")
LIST_ITEM (U"• @TextGridEditor")
LIST_ITEM (U"• @ManipulationEditor")
LIST_ITEM (U"• @SpectrumEditor")
LIST_ITEM (U"• @PitchEditor")
LIST_ITEM (U"• @PointEditor")
LIST_ITEM (U"• @PitchTierEditor")
LIST_ITEM (U"• @IntensityTierEditor")
LIST_ITEM (U"• @DurationTierEditor")
LIST_ITEM (U"• #SpectrogramEditor")
LIST_ITEM (U"• #ArtwordEditor")
LIST_ITEM (U"• @OTGrammarEditor")
LIST_ITEM (U"• (any type: @Inspect)")
ENTRY (U"How to open an editor for an object")
NORMAL (U"To open an editor window for an object in the list, select the object and choose ##View & Edit# "
	"(if the ##View & Edit# button exists, it is usually at the top of the @@Dynamic menu@). "
	"The name of the object will appear as the title of the editor window.")
NORMAL (U"Objects that cannot be modified (e.g. LongSound) just have the command #View instead of ##View & Edit#.")
ENTRY (U"General behaviour")
NORMAL (U"Changes that you make to an object in its editor window will take effect immediately. "
	"For instance, you do not have close the editor window before saving the changed object to disk.")
NORMAL (U"If you @Remove an object that you are viewing or editing from the @@List of Objects@, "
	"the editor window will automatically disappear from the screen.")
NORMAL (U"All editors are independent windows: you can minimize and maximize them; "
	"if an editor window goes hiding behind another window, "
	"you can raise it by choosing the ##View & Edit# command again.")
NORMAL (U"If you rename an object that you are viewing or editing (with @@Rename...@), "
	"the title of the editor window immediately changes to the new name.")
ENTRY (U"Ways to control an editor window")
LIST_ITEM (U"• @@Click")
LIST_ITEM (U"• @@Shift-click")
LIST_ITEM (U"• @@Drag")
LIST_ITEM (U"• @@Shift-drag")
LIST_ITEM (U"• @@Time selection")
LIST_ITEM (U"• @@Keyboard shortcuts")
MAN_END

MAN_BEGIN (U"Excitation", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. It represents the excitation pattern "
	"of the basilar membrane in the inner ear.")
ENTRY (U"Inside an Excitation object")
NORMAL (U"With @Inspect, you will see the following attributes.")
TAG (U"%xmin = 0")
DEFINITION (U"minimum place or frequency (Bark).")
TAG (U"%xmax = 25.6 Bark")
DEFINITION (U"maximum place or frequency (Bark).")
TAG (U"%nx")
DEFINITION (U"number of places or frequencies.")
TAG (U"%dx = 25.6 / %nx")
DEFINITION (U"Place or frequency step (Bark).")
TAG (U"%x1 = %dx / 2")
DEFINITION (U"centre of first place or frequency band (Bark).")
TAG (U"%ymin = %ymax = %dy = %y__1_ = 1; %ny = 1")
DEFINITION (U"dummies.")
TAG (U"%z [1]")
DEFINITION (U"intensity (sensation level) in phon.")
MAN_END

MAN_BEGIN (U"Excitation: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Excitation objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Excitation: Get loudness", U"ppgb", 19991016)
INTRO (U"A @query to ask the selected @Excitation object for its loudness.")
ENTRY (U"Return value")
NORMAL (U"the loudness in sone units.")
ENTRY (U"Algorithm")
NORMAL (U"The loudness is defined as")
FORMULA (U"∫%df 2^^(%e(%f) - 40 phon) / 10^")
NORMAL (U"where %f is the frequency in Bark, and %e(%f) the excitation in phon. "
	"For our discrete Excitation object, the loudness is computed as")
FORMULA (U"Δ%f ∑ 2^^(%e__%i_ - 40) / 10")
NORMAL (U"where Δ%f is the distance between the excitation channels (in Bark).")
MAN_END

MAN_BEGIN (U"Excitation_hertzToBark", U"ppgb", 19970401)
INTRO (U"A routine for converting frequency into basilar place, "
	"the inverse of @Excitation_barkToHertz.")
ENTRY (U"Syntax")
PROTOTYPE (U"##double Excitation_hertzToBark (double #%hertz##);")
ENTRY (U"Algorithm")
NORMAL (U"Returns 7 · ln (%hertz / 650 + √ (1 + (%hertz / 650)^2)).")
MAN_END

MAN_BEGIN (U"Excitation_barkToHertz", U"ppgb", 19970401)
INTRO (U"A routine for converting basilar place into frequency, "
	"the inverse of @Excitation_hertzToBark.")
ENTRY (U"Syntax")
PROTOTYPE (U"##double Excitation_barkToHertz (double #%bark##);")
ENTRY (U"Algorithm")
NORMAL (U"Returns 650 · sinh (%bark / 7).")
MAN_END

/*
double Excitation_soundPressureToPhon (double soundPressure, double bark);
Uses auditory filter (width apx. 1 Bark) for masking.

Excitation Excitation_create (double df, integer nf);
	Function:
		return a new instance of Excitation.
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

void Excitation_draw (Excitation me, Graphics g, double fmin, double fmax, double minimum, double maximum, bool garnish);

Matrix Excitation_to_Matrix (Excitation me);
	Function:
		Create a Matrix from an Excitation,
		with deep copy of all of its Matrix attributes, except class information and methods.
Excitation Matrix_to_Excitation (Matrix me);
	Function:
		create an Excitation from a Matrix.
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

MAN_BEGIN (U"Formula...", U"ppgb", 19980319)
NORMAL (U"See @@Matrix: Formula...@")
MAN_END

MAN_BEGIN (U"Frequency selection", U"ppgb", 20010402)
INTRO (U"The way to select a frequency domain in the @SpectrumEditor. "
	"This works completely analogously to the @@time selection@ in other editors.")
MAN_END

MAN_BEGIN (U"Get area...", U"ppgb", 20030216)
INTRO (U"A @query to the selected tier object (@PitchTier, @IntensityTier, @DurationTier).")
ENTRY (U"Return value")
NORMAL (U"the area under the curve.")
ENTRY (U"Settings")
TAG (U"##From time (s)")
TAG (U"##To time (s)")
DEFINITION (U"the selected time domain. Values outside this domain are ignored. "
	"If ##To time# is not greater than ##From time#, the entire time domain of the tier is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The curve consists of a sequence of line segments. The contribution of the line segment from "
	"(%t__1_, %f__1_) to (%t__2_, %f__2_) to the area is")
FORMULA (U"1/2 (%f__1_ + %f__2_) (%t__2_ – %t__1_)")
MAN_END

MAN_BEGIN (U"Intensity", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An Intensity object represents an intensity contour at linearly spaced time points "
	"%t__%i_ = %t__1_ + (%i – 1) %dt, with values in dB SPL, i.e. dB relative to 2·10^^-5^ Pascal, "
	"which is the normative auditory threshold for a 1000-Hz sine wave.")
MAN_END

MAN_BEGIN (U"Intensity: Get maximum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the maximum value within the specified time domain, expressed in dB.")
ENTRY (U"Settings")
TAG (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (U"%%Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get mean...", U"ppgb", 20041107)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the mean (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY (U"Settings")
TAG (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (U"##Averaging method")
DEFINITION (U"the units in which the averaging is performed. If the method is #energy, "
	"the returned dB value is based on the mean power (in Pa^2/s) between %t__1_ and %t__2_. "
	"If the method is #dB, the returned value is the mean of the intensity curve in dB. "
	"If the method is #sones, the returned value is in between these two, "
	"and based on averaging properties of the human ear.")
ENTRY (U"Algorithm")
NORMAL (U"If the averaging method is #dB, the mean intensity between the times %t__1_ and %t__2_ is defined as")
FORMULA (U"1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ %x(%t) %dt")
NORMAL (U"where %x(%t) is the intensity as a function of time, in dB. If the method is #energy, the result is")
FORMULA (U"10 log__10_ { 1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ 10^^%x(%t)/10^ %dt }")
NORMAL (U"If the method is #sones, the result is")
FORMULA (U"10 log__2_ { 1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ 2^^%x(%t)/10^ %dt }")
ENTRY (U"Behaviour")
NORMAL (U"After you do @@Sound: To Intensity...@, the mean intensity of the resulting #Intensity, "
	"if the averaging method is #energy, should be close to the mean SPL of the original #Sound, "
	"which can be found with #Info.")
MAN_END

MAN_BEGIN (U"Intensity: Get minimum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the minimum value within a specified time domain, expressed in dB.")
ENTRY (U"Settings")
TAG (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get standard deviation...", U"ppgb", 20041107)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the standard deviation (in dB) of the intensity values of the frames within a specified time domain.")
ENTRY (U"Settings")
TAG (U"%%Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
ENTRY (U"Algorithm")
NORMAL (U"The standard deviation between the times %t__1_ and %t__2_ is defined as")
FORMULA (U"√ {1/(%t__2_ - %t__1_)  ∫__%%t%1_^^%%t%2^ %dt (%x(%t) - %μ)^2}")
NORMAL (U"where %x(%t) is the intensity (in dB) as a function of time, and %μ its mean. "
	"For our discrete Intensity object, the standard deviation is approximated by")
FORMULA (U"√ {1/(%n-1) ∑__%i=%m..%m+%n-1_ (%x__%i_ - %μ)^2}")
NORMAL (U"where %n is the number of frames between %t__1_ and %t__2_. Note the \"minus 1\".")
MAN_END

MAN_BEGIN (U"Intensity: Get time of maximum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the time (in seconds) associated with the maximum intensity within a specified time domain.")
ENTRY (U"Settings")
TAG (U"%%Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (U"%%Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get time of minimum...", U"ppgb", 20200912)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the time (in seconds) associated with the minimum intensity within a specified time domain.")
ENTRY (U"Settings")
TAG (U"##Time range (s)")
DEFINITION (U"the time range (%t__1_, %t__2_). Values outside this range are ignored, except for purposes of interpolation. "
	"If %t__1_ is not less than %t__2_, the entire time domain of the Intensity is considered.")
TAG (U"##Interpolation")
DEFINITION (U"the interpolation method (#none, #parabolic, #cubic, #sinc70, #sinc700) of the @@vector peak interpolation@. "
	"The standard is Parabolic because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get value at time...", U"ppgb", 20030916)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Return value")
NORMAL (U"the intensity (in dB) at a specified time. If %time is outside the frames of the Intensity, the result is 0.")
ENTRY (U"Settings")
TAG (U"##Time (s)")
DEFINITION (U"the time at which the value is to be evaluated.")
TAG (U"##Interpolation")
DEFINITION (U"the interpolation method, see @@vector value interpolation@. "
	"The standard is “cubic” because of the usual nonlinearity (logarithm) in the computation of intensity; "
	"sinc interpolation would be too stiff and may give unexpected results.")
MAN_END

MAN_BEGIN (U"Intensity: Get value in frame...", U"ppgb", 19991016)
INTRO (U"A @query to the selected @Intensity object.")
ENTRY (U"Setting")
TAG (U"##Frame number")
DEFINITION (U"the frame whose value is to be looked up.")
ENTRY (U"Return value")
NORMAL (U"the intensity value (in dB) in the specified frame. "
	"If the index is less than 1 or greater than the number of frames, the result is 0; "
	"otherwise, it is %z [1] [%%frame number%].")
MAN_END

MAN_BEGIN (U"Intensity: To IntensityTier", U"ppgb", 19970321)
INTRO (U"A command to convert each selected @Intensity object to an @IntensityTier.")
ENTRY (U"Behaviour")
NORMAL (U"Every sample in the @Intensity object is copied to a point on the @IntensityTier.")
ENTRY (U"Postconditions")
DEFINITION (U"Equal time domains:")
LIST_ITEM (U"• %result. %xmin == %intensity. %xmin")
LIST_ITEM (U"• %result. %xmax == %intensity. %xmax")
DEFINITION (U"Equal number of points:")
LIST_ITEM (U"• %result. %points. %size == %intensity. %nx")
NORMAL (U"For all points %i = 1 ... %intensity. %nx:")
DEFINITION (U"   Explicit times:")
LIST_ITEM (U"   • %result. %points. %item [%i]. %time == %intensity. %x1 + (%i – 1) * %intensity. %dx")
DEFINITION (U"   Equal number of points:")
LIST_ITEM (U"   • %result. %points. %item [%i]. %value == %intensity. %z [1] [%i]")
MAN_END

MAN_BEGIN (U"Intensity & PointProcess: To IntensityTier...", U"ppgb", 20101230)
INTRO (U"A command to copy information from an @Intensity, at times specified by a @PointProcess, "
	"to points on an @IntensityTier.")
ENTRY (U"Behaviour")
NORMAL (U"For all the times of the points in the PointProcess, an intensity is computed from the "
	"information in the Intensity object, by linear interpolation.")
MAN_END

MAN_BEGIN (U"IntensityTier", U"ppgb", 20101230)
INTRO (U"One of the @@types of objects@ in Praat. "
	"An IntensityTier object represents a time-stamped intensity contour, i.e., it contains a series of (%time, %intensity) points. "
	"The intensity values are in dB.")
NORMAL (U"For examples, see @@Source-filter synthesis@.")
ENTRY (U"IntensityTier commands")
NORMAL (U"Creation:")
LIST_ITEM (U"From scratch:")
LIST_ITEM (U"• @@Create IntensityTier...")
LIST_ITEM (U"• @@IntensityTier: Add point...")
LIST_ITEM (U"Copy from another object:")
LIST_ITEM (U"• @@Intensity: To IntensityTier@: trivial copying of linearly spaced points.")
LIST_ITEM (U"• @@Intensity & PointProcess: To IntensityTier...@: copying interpolated values at specified points.")
LIST_ITEM (U"• @@PointProcess: Up to IntensityTier...@: equal values at specified points.")
NORMAL (U"Viewing and editing:")
LIST_ITEM (U"• @IntensityTierEditor")
NORMAL (U"Conversion:")
LIST_ITEM (U"• @@IntensityTier: Down to PointProcess@: copy times.")
NORMAL (U"Synthesis (see @@Source-filter synthesis@):")
LIST_ITEM (U"• @@Sound & IntensityTier: Multiply@")
NORMAL (U"Queries:")
LIST_ITEM (U"• @@Get low index from time...")
LIST_ITEM (U"• @@Get high index from time...")
LIST_ITEM (U"• @@Get nearest index from time...")
NORMAL (U"Modification:")
LIST_ITEM (U"• @@Remove point...")
LIST_ITEM (U"• @@Remove point near...")
LIST_ITEM (U"• @@Remove points between...")
LIST_ITEM (U"• @@IntensityTier: Add point...@")
MAN_END

MAN_BEGIN (U"IntensityTier: Add point...", U"ppgb", 20010410)
INTRO (U"A command to add a point to each selected @IntensityTier.")
ENTRY (U"Settings")
TAG (U"##Time (s)")
DEFINITION (U"the time at which a point is to be added.")
TAG (U"##Intensity (dB)")
DEFINITION (U"the intensity value of the requested new point.")
ENTRY (U"Behaviour")
NORMAL (U"The tier is modified so that it contains the new point. "
	"If a point at the specified time was already present in the tier, nothing happens.")
MAN_END

MAN_BEGIN (U"IntensityTier: Down to PointProcess", U"ppgb", 20010410)
INTRO (U"A command to degrade every selected @IntensityTier to a @PointProcess.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the points are trivially copied, and so is the time domain. The intensity information is lost.")
MAN_END

MAN_BEGIN (U"IntensityTierEditor", U"ppgb", 20110128)
INTRO (U"One of the @editors in the Praat program, for viewing and editing an @IntensityTier object. "
	"To create a IntensityTierEditor window, select an IntensityTier and click ##View & Edit#.")
MAN_END

MAN_BEGIN (U"Keyboard shortcuts", U"ppgb", 20071016)
INTRO (U"A way to accelerate the control of @Editors in Praat.")
ENTRY (U"Purpose")
NORMAL (U"to choose a menu command with the keyboard. All of these commands can also be chosen "
	"from a menu.")
ENTRY (U"Command key")
NORMAL (U"When mentioning the %%Command key%, this manual refers to the key that is marked with an apple on Apple keyboards "
	"or to the key that is marked \"Ctrl\" if you are on a Windows or Linux computer.")
ENTRY (U"Option key")
NORMAL (U"When mentioning the %%Option key%, this manual refers to the key marked \"alt\" or \"option\". "
	"In Praat, this key is sometimes used together with the Command key "
	"for destructive actions that are the reverse of the actions invoked by using the Command key only. "
	"For instance, if Command-T means \"add a target at the cursor position\", "
	"Option-Command-T may mean \"remove the selected targets\".")
ENTRY (U"Shortcuts")
LIST_ITEM (U"Command-A: Zoom all")
LIST_ITEM (U"Command-C: Copy (the selected text, or the selected sound, or the selected part of the Picture window)")
LIST_ITEM (U"Command-D (in Manipulation window): Insert duration point at cursor")
LIST_ITEM (U"Option-Command-D (in Manipulation window): Remove selected duration points")
LIST_ITEM (U"Command-E (in Picture window): Erase all")
LIST_ITEM (U"Shift-Command-H: Move cursor to maximum pitch")
LIST_ITEM (U"Command-I: Zoom in")
LIST_ITEM (U"Command-L (in Objects window): @@Open long sound file...@")
LIST_ITEM (U"Command-L (in sound windows): @@Intro 3.6. Viewing a spectral slice|View spectral slice@")
LIST_ITEM (U"Shift-Command-L: Move cursor to minimum pitch")
LIST_ITEM (U"Command-M: Search Praat manual...")
LIST_ITEM (U"Command-N: Zoom to selection")
LIST_ITEM (U"Command-O (in Objects window): @@Read from file...@")
LIST_ITEM (U"Command-O (in sound windows): Zoom out")
LIST_ITEM (U"Command-P (in Picture window): Print")
LIST_ITEM (U"Command-P (in Manipulation window): Add pulse at cursor")
LIST_ITEM (U"Option-Command-P (in Manipulation window): Remove selected pulses")
LIST_ITEM (U"Command-Q: Quit")
LIST_ITEM (U"Command-R: Reverse selection")
LIST_ITEM (U"Command-S: Save")
LIST_ITEM (U"Command-T (in Manipulation window): Add pitch point at cursor")
LIST_ITEM (U"Option-Command-T (in Manipulation window): Remove selected pitch points")
LIST_ITEM (U"Command-U: @@Calculator...@")
LIST_ITEM (U"Command-V: Paste (insert the text or sound clipboard over the selected text or the selected sound)")
LIST_ITEM (U"Command-W: Close window")
LIST_ITEM (U"Command-X: Cut (the selected text or the selected sound)")
LIST_ITEM (U"Command-Y: Redo")
LIST_ITEM (U"Command-Z: Undo")
LIST_ITEM (U"Command-0: Move cursor to nearest zero crossing")
LIST_ITEM (U"Command-2 (in Manipulation window): Stylize pitch (2 semitones)")
LIST_ITEM (U"Command-4 (in Manipulation window): Interpolate quadratically (4 points)")
LIST_ITEM (U"Shift-Command-?: Local help")
LIST_ITEM (U"Command-,: Move start of selection to nearest zero crossing")
LIST_ITEM (U"Command-.: Move end of selection to nearest zero crossing")
LIST_ITEM (U"F5: Get pitch")
LIST_ITEM (U"Command-F5: Get minimum pitch")
LIST_ITEM (U"Shift-F5: Get maximum pitch")
LIST_ITEM (U"F1: Get first formant")
LIST_ITEM (U"F2: Get second formant")
LIST_ITEM (U"F3: Get third formant")
LIST_ITEM (U"F4: Get fourth formant")
LIST_ITEM (U"F5: Get pitch")
LIST_ITEM (U"F6: Get cursor")
LIST_ITEM (U"F7: Get spectral power at cursor cross")
LIST_ITEM (U"F8: Get intensity")
LIST_ITEM (U"F12: Log 1")
LIST_ITEM (U"Shift-F12: Log 2")
LIST_ITEM (U"Option-F12: Log script 3")
LIST_ITEM (U"Command-F12: Log script 4")
LIST_ITEM (U"Tab (in sound windows): Play selection")
LIST_ITEM (U"Shift-Tab (in sound windows): Play window")
LIST_ITEM (U"Arrow-up (in sound windows): Select earlier")
LIST_ITEM (U"Arrow-down (in sound windows): Select later")
LIST_ITEM (U"Shift-Arrow-up (in sound windows): Move start of selection left")
LIST_ITEM (U"Shift-Arrow-down (in sound windows): Move start of selection right")
LIST_ITEM (U"Command-Arrow-up (in sound windows): Move end of selection left")
LIST_ITEM (U"Command-Arrow-down (in sound windows): Move end of selection right")
LIST_ITEM (U"Page-up (in sound windows): Scroll page back")
LIST_ITEM (U"Page-down (in sound windows): Scroll page forward")
LIST_ITEM (U"Escape: Interrupt playing")
MAN_END

MAN_BEGIN (U"Log files", U"ppgb", 20201230)
INTRO (U"With some commands in the @Query menu of the @SoundEditor and @TextGridEditor, "
	"you can write combined information about times, pitch values, formants, and intensities "
	"to the @@Info window@ and to a log file.")
NORMAL (U"A log file is a text file on disk. It consists of a number of similar lines, "
	"whose format you determine with the log settings in the Query menu.")
NORMAL (U"Every time you press F12 (or choose ##Log 1# from the Query menu, "
	"Praat writes a line to log file 1. If you press Shift-F12, Praat writes a line to log file 2.")
NORMAL (U"With the ##log settings# command window, you determine the following:")
TAG (U"##Log 1 to Info window")
DEFINITION (U"this determines whether your log line will be written to the Info window or not.")
TAG (U"##Log 1 to log file")
DEFINITION (U"this determines whether your log line will be written to the log file or not.")
TAG (U"##Log file 1")
DEFINITION (U"the name of the log file. On Windows, this has to be a complete path name, such as "
	"$$C:\\bsWINDOWS\\bsDESKTOP\\bsPitch Log.txt$. "
	"On Unix and macOS, it can either be a complete path name, e.g. $$/home/mary/pitch_log$, "
	"or a home-relative name such as $$~/Desktop/Pitch log$.")
TAG (U"##Log 1 format")
DEFINITION (U"the format of the line that Praat will write. See below.")
NORMAL (U"The same goes for log file 2.")
ENTRY (U"Usage")
NORMAL (U"The logging facility has been implemented in Praat especially for former users of Kay CSL, "
	"who have been used to doing it for years and like to continue doing it in Praat. "
	"Otherwise, you may prefer to use the @TextGridEditor to mark time points and run "
	"an automatic analysis afterwards.")
NORMAL (U"If you do want to use the logging facility, you typically start by deleting any old "
	"log file (by choosing ##Delete log file 1# or ##Delete log file 2#), if you want to re-use "
	"the file name. Otherwise, you can change the log file name (with ##Log settings...#). "
	"After this, you will move the cursor to various time locations and press F12 (or Shift-F12) "
	"each time, so that information about the current time will be written to the log file.")
ENTRY (U"Example 1: pitch logging")
NORMAL (U"Suppose you want to log the time of the cursor and the pitch value at the cursor. "
	"You could use the following log format:")
CODE (U"Time \'time:6\' seconds, pitch \'f0:2\' hertz")
NORMAL (U"If you now click at 3.456789876 seconds, and the pitch happens to be 355.266 hertz "
	"at that time, the following line will be appended to the log file and/or to the Info window:")
CODE (U"Time 3.456790 seconds, pitch 355.27 hertz.")
NORMAL (U"The parts \":6\" and \":2\" denote the number of digits after the decimal point. "
	"If you leave them out, the values will be written with a precision of 17 digits.")
NORMAL (U"The words \'time\' and \'f0\' mean exactly the same as the result of the commands "
	"##Get cursor# and ##Get pitch#. Therefore, if instead of setting a cursor line you selected a larger "
	"piece of the sound, \'time\' will give the centre of the selection and \'f0\' will give the mean pitch "
	"in the selection.")
NORMAL (U"Beware of the following pitfall: if your pitch units are not hertz, but semitones, "
	"then \'f0\' will give the result in semitones. A format as in this example will then be misleading.")
ENTRY (U"Example 2: formant logging")
NORMAL (U"Suppose you want to log the start and finish of the selection, its duration, and the mean values "
	"of the first three formants, all separated by tab stops for easy importation into Microsoft® Excel™. "
	"You could use the following log format:")
CODE (U"\'t1:4\'\'tab\\$ \'\'t2:4\'\'tab\\$ \'\'f1:0\'\'tab\\$ \'\'f2:0\'\'tab\\$ \'\'f3:0\'")
NORMAL (U"You see that \'t1\' and \'t2\' are the start and finish of the selection, respectively, "
	"and that they are written with 4 digits after the decimal point. By using \":0\", the three formant values "
	"are rounded to whole numbers in hertz. The word \'tab\\$ \' is the tab stop.")
ENTRY (U"Loggable values")
NORMAL (U"The following values can be logged:")
LIST_ITEM (U"\'time\': the time of the cursor, or the centre of the selection.")
LIST_ITEM (U"\'t1\': the start of the selection (\"B\").")
LIST_ITEM (U"\'t2\': the end of the selection (\"E\").")
LIST_ITEM (U"\'dur\': the duration of the selection.")
LIST_ITEM (U"\'freq\': the frequency at the frequency cursor.")
LIST_ITEM (U"\'f0\': the pitch at the cursor time, or the mean pitch in the selection.")
LIST_ITEM (U"\'f1\', \'f2\', \'f3\', \'f4\', \'f5\': the first/second/third/fourth/fifth formant at the cursor time, "
	"or the mean first/second/third/fourth/fifth formant in the selection.")
LIST_ITEM (U"\'b1\', \'b2\', \'b3\', \'b4\', \'b5\': the bandwidth of the first/second/third/fourth/fifth formant "
	"at the cursor time or at the centre of the selection.")
LIST_ITEM (U"\'intensity\': the intensity at the cursor time, or the mean intensity in the selection, in dB.")
LIST_ITEM (U"\'power\': the spectral power at the cursor cross, in Pa^2/Hz.")
LIST_ITEM (U"\'tab\\$ \': the tab stop.")
LIST_ITEM (U"\'editor\\$ \': the title of the editor window (i.e. the name of the visible Sound or TextGrid).")
ENTRY (U"More flexibility in logging")
NORMAL (U"You may sometimes require information in your log file that cannot be generated directly "
	"by the loggable values above. Suppose, for instance, that you want to log the values for F1 and F2-F1 "
	"at the points where you click. You could write the following script:")
CODE (U"f1 = Get first formant")
CODE (U"f2 = Get second formant")
CODE (U"f21 = f2 - f1")
CODE (U"appendInfoLine: fixed\\$  (f1, 0), \" \", fixed\\$  (f21, 0)")
CODE (U"appendFileLine: \"D:\\bsPraat logs\\bsFormant log.txt\", fixed\\$  (f1, 0), tab\\$ , fixed\\$  (f21, 0)")
NORMAL (U"With this script, the information would be appended both to the Info window and to the "
	"file \"Formant log.txt\" on your desktop.")
NORMAL (U"You can make this script accessible with Option-F12 (or Command-F12) "
	"by saving the script and specifying the name of the script file in the ##Log script 3# (or #4) field "
	"in the ##Log settings...# window.")
NORMAL (U"These scripts may take arguments. Suppose, for instance, that you want to specify a vowel symbol "
	"as you press Option-F12. The following script will take care of that:")
CODE (U"form Save vowel and formants")
CODE1 (U"word Vowel a")
CODE (U"endform")
CODE (U"f1 = Get first formant")
CODE (U"f2 = Get second formant")
CODE (U"f21 = f2 - f1")
CODE (U"appendInfoLine: vowel\\$ , \" \", fixed\\$  (f1, 0), \" \", fixed\\$  (f21, 0)")
CODE (U"appendFileLine: \"~/Praat logs/Vowels and formants log\", vowel\\$ , tab\\$ , fixed\\$  (f1, 0), tab\\$ , fixed\\$  (f21, 0)")
NORMAL (U"Beware of the following pitfall: because of the nature of scripts, you should not try to do this "
	"when you have two editor windows with the same name. I cannot predict which of the two windows "
	"will answer the #Get queries...")
MAN_END

MAN_BEGIN (U"Manipulation", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat, for changing the pitch and duration contours of a sound.")
ENTRY (U"Inside a manipulation object")
NORMAL (U"With @Inspect, you will see the following attributes:")
TAG (U"##timeStep")
DEFINITION (U"the time step (or %%frame length%) used in the pitch analysis. A common value is 0.010 seconds.")
TAG (U"##minimumPitch")
DEFINITION (U"the minimum pitch frequency considered in the pitch analysis. A common value is 75 hertz.")
TAG (U"##maximumPitch")
DEFINITION (U"the maximum pitch frequency considered in the pitch analysis. A common value is 600 hertz.")
NORMAL (U"A Manipulation object also contains the following smaller objects:")
LIST_ITEM (U"1. The original @Sound.")
LIST_ITEM (U"2. A @PointProcess representing glottal pulses.")
LIST_ITEM (U"3. A @PitchTier.")
LIST_ITEM (U"4. A @DurationTier.")
ENTRY (U"Analysis")
NORMAL (U"When a Manipulation object is created from a sound, the following steps are performed:")
LIST_ITEM (U"1. A pitch analysis is performed on the original sound, with the method of @@Sound: To Pitch...@. "
	"This uses the time step, minimum pitch, and maximum pitch parameters.")
LIST_ITEM (U"2. The information of the resulting pitch contour (frequency and voiced/unvoiced decisions) "
	"is used to posit glottal pulses where the original sound contains much energy. "
	"The method is the same as in @@Sound & Pitch: To PointProcess (cc)@.")
LIST_ITEM (U"3. The pitch contour is converted to a pitch tier with many points (targets), "
	"with the method of @@Pitch: To PitchTier@.")
LIST_ITEM (U"4. An empty @DurationTier is created.")
ENTRY (U"Resynthesis")
TAG (U"A Manipulation object can produce Sound input. This Sound can be computed in several ways:")
LIST_ITEM (U"• @@overlap-add@: from original sound + pulses + pitch tier + duration tier;")
LIST_ITEM (U"• #LPC: from LPC (from original sound) + pulses + pitch tier;")
LIST_ITEM (U"• from the pulses only, as a pulse train or hummed;")
LIST_ITEM (U"• from the pitch tier only, as a pulse train or hummed.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract duration tier", U"ppgb", 20010330)
INTRO (U"A command to extract a copy of the duration information in each selected @Manipulation object into a new @DurationTier object.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract original sound", U"ppgb", 20010330)
INTRO (U"A command to copy the original sound in each selected @Manipulation object to a new @Sound object.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract pitch tier", U"ppgb", 20010330)
INTRO (U"A command to extract a copy of the pitch information in each selected @Manipulation object into a new @PitchTier object.")
MAN_END

MAN_BEGIN (U"Manipulation: Extract pulses", U"ppgb", 20010330)
INTRO (U"A command to extract a copy of the vocal-pulse information in each selected @Manipulation object into a new @PointProcess object.")
MAN_END

MAN_BEGIN (U"Manipulation: Play (overlap-add)", U"ppgb", 20070722)
INTRO (U"A command to play each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN (U"Manipulation: Get resynthesis (overlap-add)", U"ppgb", 20070722)
INTRO (U"A command to extract the sound from each selected @Manipulation object, resynthesized with the @@overlap-add@ method.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace duration tier", U"ppgb", 20030216)
INTRO (U"You can replace the duration tier that you see in your @Manipulation object "
	"with a separate @DurationTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create DurationTier...@.")
NORMAL (U"To do this, select your Manipulation object together with the @DurationTier object and click ##Replace duration tier#.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace pitch tier", U"ppgb", 20030216)
INTRO (U"You can replace the pitch tier that you see in your @Manipulation object "
	"with a separate @PitchTier object, for instance one that you extracted from another Manipulation "
	"or one that you created with @@Create PitchTier...@.")
NORMAL (U"To do this, select your Manipulation object together with the @PitchTier object and click ##Replace pitch tier#.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace pulses", U"ppgb", 20010330)
INTRO (U"A command to replace the vocal-pulse information in the selected @Manipulation object with the selected @PointProcess object.")
MAN_END

MAN_BEGIN (U"Manipulation: Replace original sound", U"ppgb", 20010330)
INTRO (U"A command to replace the original sound in the selected @Manipulation object with the selected @Sound object.")
MAN_END

MAN_BEGIN (U"ManipulationEditor", U"ppgb", 20030316)
	INTRO (U"One of the @Editors in Praat, for viewing and manipulating a @Manipulation object.")
ENTRY (U"Objects")
	NORMAL (U"The editor shows:")
	LIST_ITEM (U"• The original @Sound.")
	LIST_ITEM (U"• The @PointProcess that represents the glottal %pulses. "
		"You can edit it for improving the pitch analysis.")
	LIST_ITEM (U"• A pitch contour based on the locations of the pulses, for comparison (drawn as grey dots). "
		"Changes shape if you edit the pulses.")
	LIST_ITEM (U"• The @PitchTier that determines the pitch contour of the resynthesized @Sound (drawn as blue circles). "
		"At the creation of the @Manipulation object, it is computed from the original pitch contour. "
		"You can manipulate it by simplifying it (i.e., removing targets), "
		"or by moving parts of it up and down, and back and forth.")
	LIST_ITEM (U"• A @DurationTier for manipulating the relative durations of the voiced parts of the sound.")
ENTRY (U"Playing")
	NORMAL (U"To play (a part of) the %resynthesized sound (by any of the methods shown in the #Synth menu, "
		"like @@overlap-add@ and #LPC), @click on any of the 1 to 8 buttons below and above the drawing area "
		"or use the Play commands from the View menu.")
	NORMAL (U"To play the %original sound instead, use ##Shift-click#.")
ENTRY (U"Pulses")
	TAG (U"To add:")
	DEFINITION (U"#click at the desired time location, and choose ##Add pulse at cursor# or type ##Command-p#.")
	TAG (U"To remove:")
	DEFINITION (U"make a @@time selection@, and choose ##Remove pulse(s)# or type ##Option-Command-p#. "
		"If there is no selection, the pulse nearest to the cursor is removed.")
ENTRY (U"Pitch points")
	TAG (U"To add one at a specified %%time and frequency%:")
	DEFINITION (U"#click at the desired time-frequency location, and choose ##Add pitch point at cursor# or type ##Command-t#.")
	TAG (U"To add one at a specified %time only:")
	DEFINITION (U"#click at the desired time, and choose ##Add pitch point at time slice#. ManipulationEditor tries to compute the frequency from the "
		"intervals between the pulses, basically by a median-of-three method.")
	TAG (U"To remove:")
	DEFINITION (U"make a @@time selection@, and choose ##Remove pitch point(s)# or type ##Option-Command-t#. "
		"If there is no selection, the pitch point nearest to the cursor is removed.")
	TAG (U"To move %some:")
	DEFINITION (U"make a @@time selection@ (the points become red) and ##Shift-drag# the points across the window. "
		"You cannot drag them across adjacent points, or below 50 Hz, or above the maximum frequency. "
		"You can only drag them horizontally if the %%dragging strategy% is ##All# or ##Only horizontal#, "
		"and you can drag them vertically if the dragging strategy is not ##Only horizontal#. "
		"You can change the dragging strategy with ##Set pitch dragging strategy...# from the #Pitch menu.")
	TAG (U"To move %one:")
	DEFINITION (U"@drag that point across the window. "
		"You can only drag it horizontally if the dragging strategy is not ##Only vertical#, "
		"and you can drag it vertically if the dragging strategy is not ##Only horizontal#.")
ENTRY (U"Duration points")
	NORMAL (U"Work pretty much the same as pitch points.")
ENTRY (U"Stylization")
	NORMAL (U"Before editing the Pitch points, you may want to reduce their number by choosing any of the #Stylize "
		"commands from the #Pitch menu.")
MAN_END

MAN_BEGIN (U"Matrix", U"ppgb", 20030216)
INTRO (U"One of the @@types of objects@ in Praat. "
	"A Matrix object represents a function %z (%x, %y) "
	"on the domain [%x__%min_, %x__%max_] × [%y__%min_, %y__%max_]. "
	"The domain has been sampled in the %x and %y directions "
	"with constant sampling intervals (%dx and %dy) along each direction. "
	"The samples are thus %z [%i__%y_] [%i__%x_], %i__%x_ = 1 ... %n__%x_, %i__%y_ = 1 ... %n__%y_. "
	"The samples represent the function values %z (%x__1_ + (%ix - 1) %dx, %y__1_ + (%iy - 1) %dy).")
ENTRY (U"Matrix commands")
NORMAL (U"Creation:")
LIST_ITEM (U"• @@Create Matrix...")
LIST_ITEM (U"• @@Create simple Matrix...")
LIST_ITEM (U"• @@Read from file...")
LIST_ITEM (U"• @@Read Matrix from raw text file...")
LIST_ITEM (U"• ##Read Matrix from LVS AP file...")
NORMAL (U"Drawing:")
LIST_ITEM (U"• ##Matrix: Draw rows...")
LIST_ITEM (U"• ##Matrix: Draw contours...")
LIST_ITEM (U"• ##Matrix: Paint contours...")
LIST_ITEM (U"• ##Matrix: Paint cells...")
LIST_ITEM (U"• ##Matrix: Scatter plot...")
LIST_ITEM (U"• @@Matrix: Draw as squares...")
LIST_ITEM (U"• ##Matrix: Draw value distribution...")
LIST_ITEM (U"• ##Matrix: Paint surface...")
NORMAL (U"Modification:")
LIST_ITEM (U"• @@Matrix: Formula...")
LIST_ITEM (U"• ##Matrix: Scale...")
ENTRY (U"Inside a Matrix object")
NORMAL (U"With @Inspect, you will see the following attributes.")
TAG (U"%xmin, %xmax ≥ %xmin")
DEFINITION (U"%x domain.")
TAG (U"%nx ≥ 1")
DEFINITION (U"number of columns.")
TAG (U"%dx > 0.0")
DEFINITION (U"distance between columns.")
TAG (U"%x1")
DEFINITION (U"%x value associated with first column.")
TAG (U"%ymin, %ymax ≥ %ymin")
DEFINITION (U"%y domain.")
TAG (U"%ny ≥ 1")
DEFINITION (U"number of rows.")
TAG (U"%dy > 0.0")
DEFINITION (U"distance between rows.")
TAG (U"%y1")
DEFINITION (U"%y value associated with first row.")
TAG (U"%z [1..%ny] [1..%nx]")
DEFINITION (U"The sample values.")
NORMAL (U"After creation of the #Matrix, %xmin, %xmax, %ymin, %ymax, "
	"%nx, %ny, %dx, %dy, %x1, and %y1 "
	"do not usually change. The contents of %z do.")
NORMAL (U"Normally, you will want %xmin ≤ %x1 and %xmax ≥ %x1 + (%nx - 1) %dx.")
ENTRY (U"Example: simple matrix")
NORMAL (U"If a simple matrix has %x equal to column number "
	"and %y equal to row number, it has the following attributes:")
LIST_ITEM (U"%xmin = 1;   %xmax = %nx;   %dx = 1;  %x1 = 1;")
LIST_ITEM (U"%ymin = 1;   %ymax = %ny;   %dy = 1;  %y1 = 1;")
ENTRY (U"Example: sampled signal")
NORMAL (U"If the matrix represents a sampled signal of 1 second duration "
	"with a sampling frequency of 10 kHz, it has the following attributes:")
LIST_ITEM (U"%xmin = 0.0;   %xmax = 1.0;   %nx = 10000 ;   %dx = 1.0·10^^-4^;   %x1 = 0.5·10^^-4^;")
LIST_ITEM (U"%ymin = 1;   %ymax = 1;   %ny = 1;   %dy = 1;   %y1 = 1;")
ENTRY (U"Example: complex signal")
NORMAL (U"If the matrix represents a complex spectrum "
	"derived with an @FFT from the sound of example 2, it has the following attributes:")
LIST_ITEM (U"%xmin = 0.0;   %xmax = 5000.0;   %nx = 8193 ;   %dx = 5000.0 / 8192;   %x1 = 0.0;")
LIST_ITEM (U"%ny = 2 (real and imaginary part);")
LIST_ITEM (U"%ymin = 1 (first row, real part);")
LIST_ITEM (U"%ymax = 2 (second row, imaginary part);")
LIST_ITEM (U"%dy = 1;   %y1 = 1;  (so that %y is equal to row number)")
MAN_END

MAN_BEGIN (U"Matrix: Draw as squares...", U"ppgb", 19980319)
INTRO (U"A command to draw a @Matrix object into the @@Picture window@.")
ENTRY (U"Settings")
TAG (U"##Xmin")
TAG (U"##Xmax")
DEFINITION (U"the windowing domain in the %x direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Xmin ≥ %Xmax), the entire %x domain [%x__%min_, %x__%max_] of the Matrix is used.")
TAG (U"##Ymin")
TAG (U"##Ymax")
DEFINITION (U"the windowing domain in the %y direction. Elements outside will not be drawn. "
	"%Autowindowing: if (%Ymin ≥ %Ymax), the entire %y domain [%y__%min_, %y__%max_] of the Matrix is used.")
TAG (U"##Garnish")
DEFINITION (U"determines whether axes are drawn around the picture. "
	"Turn this button off if you prefer to garnish your picture by yourself with the @Margins menu.")
ENTRY (U"Behaviour")
NORMAL (U"For every element of the Matrix inside the specified windowing domain, "
	"an opaque white or black rectangle is painted (white if the value of the element is positive, "
	"black if it is negative), surrounded by a thin black box. "
	"The %area of the rectangle is proportional to the value of the element.")
ENTRY (U"Trick")
NORMAL (U"If you prefer the %sides of the rectangle (instead of the area) to be proportional "
	"to the value of the element, you can use the formula \"$$self\\^ 2$\" before drawing (see @@Matrix: Formula...@).") 
MAN_END

MAN_BEGIN (U"Matrix: Formula...", U"ppgb", 20021206)
INTRO (U"A command for changing the data in all selected @Matrix objects.")
NORMAL (U"See the @Formulas tutorial for examples and explanations.")
MAN_END

MAN_BEGIN (U"Matrix: Paint cells...", U"ppgb", 20021204)
INTRO (U"A command to draw the contents of a @Matrix to the @@Picture window@.")
NORMAL (U"Every cell of the matrix is drawn as a rectangle filled with a grey value between white (if the content "
	"of the cell is small) and black (if the content is large).")
MAN_END

MAN_BEGIN (U"Matrix: Set value...", U"ppgb", 19980319)
INTRO (U"A command to change the value of one cell in each selected @Matrix object.")
ENTRY (U"Settings")
TAG (U"##Row number")
DEFINITION (U"the number of the row of the cell whose value you want to change.")
TAG (U"##Column number")
DEFINITION (U"the number of the column of the cell whose value you want to change.")
TAG (U"##New value")
DEFINITION (U"the value that you want the specified cell to have.")
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

MAN_BEGIN (U"Matrix: To TableOfReal", U"ppgb", 19991030)
INTRO (U"A command to convert every selected @Matrix to a @TableOfReal.")
NORMAL (U"This command is available from the #Cast menu. The resulting TableOfReal "
	"has the same number of rows and columns as the original Matrix, "
	"and the same data in the cells. However, it does not yet have any row or column "
	"labels; you can add those with some commands from the TableOfReal #Modify menu.")
MAN_END

MAN_BEGIN (U"Modify", U"ppgb", 20021204)
INTRO (U"The title of a submenu of the @@dynamic menu@ for many object types. "
	"This submenu usually collects all the commands that can change the selected object.")
MAN_END

MAN_BEGIN (U"PairDistribution", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. A "
	"PairDistribution object represents the relative probabilities with which "
	"the specified pairs of strings occur.")
ENTRY (U"Class description")
TAG (U"##struct-list# pairs")
DEFINITION (U"a list of relative string-pair probabilities. Each element consists of:")
TAG1 (U"#string %string1")
DEFINITION1 (U"the first string.")
TAG1 (U"#string %string2")
DEFINITION1 (U"the second string.")
TAG1 (U"#real %weight")
DEFINITION1 (U"the relative probability associated with the string pair. This value cannot be negative.")
MAN_END

MAN_BEGIN (U"PairDistribution: To Stringses...", U"ppgb", 20030916)
INTRO (U"A command to generate a number of string pairs from the selected @PairDistribution object. "
	"This command will create two aligned @Strings objects of equal size.")
ENTRY (U"Settings")
TAG (U"##Number# (standard: 1000)")
DEFINITION (U"the number of the strings in either resulting Strings object.")
TAG (U"##Name of first Strings# (standard: \"input\")")
DEFINITION (U"the name of the resulting Strings object associated with the first string of each pair.")
TAG (U"##Name of second Strings# (standard: \"output\")")
DEFINITION (U"the name of the resulting Strings object associated with the second string of each pair.")
ENTRY (U"Example")
NORMAL (U"Suppose the PairDistribution contains the following:")
CODE (U"4 pairs")
CODE (U"\"at+ma\"  \"atma\"  100")
CODE (U"\"at+ma\"  \"apma\"    0")
CODE (U"\"an+pa\"  \"anpa\"   20")
CODE (U"\"an+pa\"  \"ampa\"   80")
NORMAL (U"The resulting Strings object \"input\" may then contain:")
FORMULA (U"at+ma, an+pa, an+pa, at+ma, at+ma, an+pa, an+pa, an+pa, an+pa, at+ma, ...")
NORMAL (U"The Strings object \"output\" may then contain:")
FORMULA (U"atma,  ampa,  ampa,  atma,  atma,  ampa,  anpa,  ampa,  ampa,  atma, ...")
MAN_END

MAN_BEGIN (U"ParamCurve", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"An object of class #ParamCurve represents a sequence of time-stamped points (%x (%%t__i_), %y (%%t__i_)) "
	"in a two-dimensional space.")
MAN_END

MAN_BEGIN (U"PointEditor", U"ppgb", 20110128)
INTRO (U"One of the @Editors in Praat, for viewing and manipulating a @PointProcess object, "
	"which is optionally shown together with a @Sound object.")
ENTRY (U"Objects")
NORMAL (U"The editor shows:")
LIST_ITEM (U"• The @Sound, if you selected a Sound object together with the PointProcess object "
	"before you clicked ##View & Edit#.")
LIST_ITEM (U"• The @PointProcess; vertical blue lines represent the points.")
ENTRY (U"Playing")
NORMAL (U"To play (a part of) the %resynthesized sound (pulse train): "
	"@click on any of the 8 buttons below and above the drawing area, or choose a Play command from the View menu.")
NORMAL (U"To play the %original sound instead, use @@Shift-click@.")
ENTRY (U"Adding a point")
NORMAL (U"@Click at the desired time location, and choose \"Add point at cursor\" or type ##Command-P#.")
ENTRY (U"Removing points")
NORMAL (U"To remove one or more points, "
	"make a @@time selection@ and choose ##Remove point(s)# from the ##Point# menu. "
	"If there is no selection, the point nearest to the cursor is removed.")
MAN_END

MAN_BEGIN (U"PointProcess", U"ppgb", 20110128)
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
TAG (U"##Time (s)")
DEFINITION (U"the time at which a point is to be added.")
ENTRY (U"Behaviour")
NORMAL (U"The point process is modified so that it contains the new point. "
	"If a point at the specified time was already present in the point process, nothing happens.")
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
INTRO (U"A @query to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the index of the nearest point at or after the specified time, "
	"0 if the point process contains no points, "
	"or a number higher than the number of points if the specified time is after the last point.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time from which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (U"PointProcess: Get interval...", U"ppgb", 20021212)
INTRO (U"A @query to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the duration of the interval around a specified time. "
	"if the point process contains no points or if the specified time falls before the first point "
	"or not before the last point, the value is @undefined. Otherwise, the result is the distance between "
	"the nearest points to the left and to the right of the specified time. "
	"If the point process happens to contain a point at exactly the specified time, "
	"the duration of the interval following this point is returned.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time around which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (local)...", U"ppgb", 20110302)
INTRO (U"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
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
TAG (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this setting to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (U"##Maximum period factor")
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
FORMULA (U"%jitter(seconds) = ∑__%i=2_^^%N^ |%T__%i_ - %T__%i-1_| / (%N - 1)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - %T__%i-1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 2, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
FORMULA (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute local jitter.")
NORMAL (U"Finally, we compute the (relative) local jitter as")
FORMULA (U"%jitter = %jitter(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is a value between 0 and 2, or between 0 and 200 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (local, absolute)...", U"ppgb", 20110220)
INTRO (U"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
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
TAG (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (U"##Maximum period factor")
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
FORMULA (U"%jitter(seconds) = ∑__%i=2_^^%N^ |%T__%i_ - %T__%i-1_| / (%N - 1)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - %T__%i-1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 2, the result of the command is @undefined).")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (rap)...", U"ppgb", 20110302)
INTRO (U"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
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
TAG (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (U"##Maximum period factor")
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
FORMULA (U"%absAP(seconds) = ∑__%i=2_^^%N-1^ |%T__%i_ - (%T__%i-1_ + %T__%i_ + %T__%i+1_) / 3| / (%N - 2)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ or %T__%i+1_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - (%T__%i-1_ + %T__%i_ + %T__%i+1_) / 3| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 3, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
FORMULA (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute average perturbation.")
NORMAL (U"Finally, we compute the Relative Average Perturbation as")
FORMULA (U"%RAP = %absAP(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is a value between 0 and 2, or between 0 and 200 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (ppq5)...", U"ppgb", 20110302)
INTRO (U"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
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
TAG (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (U"##Maximum period factor")
DEFINITION (U"the largest possible difference between consecutive intervals that will be used in the computation of jitter. "
	"If the ratio of the durations of two consecutive intervals is greater than this, "
	"this pair of intervals will be ignored in the computation of jitter "
	"(each of the intervals could still take part in the computation of jitter in a comparison with its neighbour on the other side).")
ENTRY (U"2. Usage")
NORMAL (U"The jitter can be used as a measure of voice quality. See @@Voice 2. Jitter@.")
ENTRY (U"3. Algorithm")
NORMAL (U"The five-point Period Perturbation Quotient (PPQ5) is defined in terms of five consecutive intervals, as follows.")
NORMAL (U"First, we define the absolute (i.e. non-relative) PPQ5 (in seconds):")
FORMULA (U"%absPPQ5(seconds) = ∑__%i=3_^^%N-2^ |%T__%i_ - (%T__%i-2_ + %T__%i-1_ + %T__%i_ + %T__%i+1_ + %T__%i+2_) / 5| / (%N - 4)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-2_ or %T__%i-1_ or %T__%i_ or %T__%i+1_ or %T__%i+2_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-2_/%T__%i-1_ or %T__%i-1_/%T__%i-2_ or %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ or %T__%i+2_/%T__%i+1_ or %T__%i+1_/%T__%i+2_ is greater than ##Maximum period factor#, "
	"the term |%T__%i_ - (%T__%i-2_ + %T__%i-1_ + %T__%i_ + %T__%i+1_ + %T__%i+2_) / 5| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 5, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
FORMULA (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of the absolute PPQ5.")
NORMAL (U"Finally, we compute the five-point Period Perturbation Quotient as")
FORMULA (U"%PPQ5 = %PPQ5(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is a value between 0 and 4, or between 0 and 400 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get jitter (ddp)...", U"ppgb", 20110302)
INTRO (U"A command that becomes available in the #Query submenu when you select a @PointProcess object.")
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
TAG (U"##Time range (s)")
DEFINITION (U"the start time and end time of the part of the PointProcess that will be measured. "
	"Points outside this range will be ignored.")
TAG (U"##Period floor (s)")
DEFINITION (U"the shortest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is shorter than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"This setting will normally be very small, say 0.1 ms.")
TAG (U"##Period ceiling (s)")
DEFINITION (U"the longest possible interval that will be used in the computation of jitter, in seconds. "
	"If an interval is longer than this, it will be ignored in the computation of jitter "
	"(and the previous and next intervals will not be regarded as consecutive). "
	"For example, if the minimum frequency of periodicity is 50 Hz, set this argument to 0.02 seconds; "
	"intervals longer than that could be regarded as voiceless stretches and will be ignored in the computation.")
TAG (U"##Maximum period factor")
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
FORMULA (U"%absDDP(seconds) = ∑__%i=2_^^%N-1^ |(%T__%i+1_ - %T__%i_) - (%T__%i_ - %T__%i-1_)| / (%N - 2)")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i-1_ or %T__%i_ or %T__%i+1_ is not between ###Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ or %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term |2%T__%i_ - %T__%i-1_ - %T__%i+1_| is not counted in the sum, and %N is lowered by 1 "
	"(if %N ends up being less than 3, the result of the command is @undefined).")
NORMAL (U"Second, we define the mean period as")
FORMULA (U"%meanPeriod(seconds) = ∑__%i=1_^^%N^ %T__%i_ / %N")
NORMAL (U"where %T__%i_ is the duration of the %%i%th interval and %N is the number of intervals. "
	"If an interval %T__%i_ is not between ##Period floor# and ##Period ceiling#, "
	"or if %T__%i-1_/%T__%i_ or %T__%i_/%T__%i-1_ is greater than ##Maximum period factor# "
	"%and %T__%i+1_/%T__%i_ or %T__%i_/%T__%i+1_ is greater than ##Maximum period factor#, "
	"the term %T__%i_ is not counted in the sum, and %N is lowered by 1; "
	"this procedure ensures that in the computation of the mean period we use at least all the intervals "
	"that had taken part in the computation of DDP.")
NORMAL (U"Finally, we compute DDP as")
FORMULA (U"%DDP = %absDDP(seconds) / %meanPeriod(seconds)")
NORMAL (U"The result is exactly 3 times the @@PointProcess: Get jitter (rap)...|RAP@ jitter measurement: "
	"a value between 0 and 6, or between 0 and 600 percent.")
MAN_END

MAN_BEGIN (U"PointProcess: Get low index...", U"ppgb", 20021212)
INTRO (U"A @query to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the index of the nearest point before or at the specified time, "
	"or 0 if the point process contains no points or the specified time is before the first point.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time from which a point is looked for, in seconds.")
MAN_END

MAN_BEGIN (U"PointProcess: Get nearest index...", U"ppgb", 20021212)
INTRO (U"A @query to the selected @PointProcess object.")
ENTRY (U"Return value")
NORMAL (U"the index of the point nearest to the specified time, "
	"or 0 if the point process contains no points.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
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
TAG (U"##Index")
DEFINITION (U"the index of the point that is to be removed.")
ENTRY (U"Behaviour")
NORMAL (U"Does nothing if %index is less than 1 or greater than the number of points %nt in the point process. "
	"Otherwise, one point is removed (e.g., if %index is 3, the third point is removed), and the other points stay the same.")
MAN_END

MAN_BEGIN (U"PointProcess: Remove point near...", U"ppgb", 20021212)
INTRO (U"A command to remove a point from every selected @PointProcess.")
ENTRY (U"Setting")
TAG (U"##Time (s)")
DEFINITION (U"the time (in seconds) around which a point is to be removed.")
ENTRY (U"Behaviour")
NORMAL (U"Does nothing if there are no points in the point process. "
	"Otherwise, the point nearest to %time is removed, and the other points stay the same.")
MAN_END

MAN_BEGIN (U"PointProcess: Remove points...", U"ppgb", 20021212)
INTRO (U"A command to remove a range of points from every selected @PointProcess.")
ENTRY (U"Settings")
TAG (U"##From index (≥ 1)")
DEFINITION (U"the first index of the range of points that are to be removed.")
TAG (U"##To index")
DEFINITION (U"the last index of the range of points that are to be removed.")
ENTRY (U"Behaviour")
NORMAL (U"All points that originally fell in the range [%fromIndex, %toIndex] are removed, and the other points stay the same.")
MAN_END

MAN_BEGIN (U"PointProcess: Remove points between...", U"ppgb", 20021212)
INTRO (U"A command to remove a range of points from every selected @PointProcess.")
ENTRY (U"Settings")
TAG (U"##From time (s)")
DEFINITION (U"the start of the domain from which all points are to be removed.")
TAG (U"##To time (s)")
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
FORMULA (U"%U(%x) = %x^^%power1^ - %x^^%power2^")
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
FORMULA (U"%dU(%x)/%dx = %power1 %x^^(%power1-1)^ - %power2 %x^^(%power2-1)^")
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
TAG (U"##Sampling frequency (Hz)")
DEFINITION (U"the sampling frequency of the resulting Sound object, e.g. 44100 hertz.")
TAG (U"##Adaptation factor")
DEFINITION (U"the factor by which a pulse height will be multiplied if the pulse time is not within "
	"##Maximum period# from the previous pulse, and by which a pulse height will again be multiplied "
	"if the previous pulse time is not within ##Maximum period# from the pre-previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TAG (U"##Maximum period (s)")
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
TAG (U"##Sampling frequency (Hz)")
DEFINITION (U"the sampling frequency of the resulting Sound object, e.g. 44100 hertz.")
TAG (U"##Adaptation factor")
DEFINITION (U"the factor by which a pulse height will be multiplied if the pulse time is not within "
	"##Adaptation time# from the pre-previous pulse, and by which a pulse height will again be multiplied "
	"if the pulse time is not within ##Adaptation time# from the previous pulse. This factor is against "
	"abrupt starts of the pulse train after silences, and is 1.0 if you do want abrupt starts after silences.")
TAG (U"##Adaptation time (s)")
DEFINITION (U"the minimal period that will be considered a silence, e.g. 0.05 seconds.")
TAG (U"##Interpolation depth")
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
TAG (U"##Intensity (dB)")
DEFINITION (U"the intensity that will be associated with every point.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the points are trivially copied, and so is the time domain. "
	"The intensity information will be the same for every point.")
MAN_END

MAN_BEGIN (U"PointProcess: Up to PitchTier...", U"ppgb", 19970329)
INTRO (U"A command to promote every selected @PointProcess to a @PitchTier.")
ENTRY (U"Setting")
TAG (U"##Frequency (Hz)")
DEFINITION (U"the pitch frequency that will be associated with every point.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the points are trivially copied, and so is the time domain. "
	"The pitch information will be the same for every point.")
MAN_END

MAN_BEGIN (U"Polygon", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A Polygon object represents a sequence of points (%%x__i_, %%y__i_) in a two-dimensional space.")
MAN_END

MAN_BEGIN (U"Read Matrix from raw text file...", U"ppgb", 19980322)
INTRO (U"A command to read a @Matrix object from a file on disk.")
ENTRY (U"File format")
NORMAL (U"The file should contain each row of the matrix on a separate line. Within each row, "
	"the elements must be separated by spaces or tabs.")
NORMAL (U"For instance, the following text file will be read as a Matrix with three rows and four columns:")
CODE (U"0.19 3 245 123")
CODE (U"18e-6 -3e18 0 0.0")
CODE (U"1.5 2.5 3.5 4.5")
NORMAL (U"The resulting Matrix will have the same domain and sampling as Matrices created with "
	"##Create simple Matrix...#. In the above example, this means that the Matrix will have "
	"%x__%min_ = 0.5, %x__%max_ = 4.5, %n__%x_ = 4, %dx = 1.0, %x__1_ = 1.0, "
	"%y__%min_ = 0.5, %y__%max_ = 3.5, %n__%y_ = 3, %dy = 1.0, %y__1_ = 1.0.")
MAN_END

MAN_BEGIN (U"Read Strings from raw text file...", U"ppgb", 19990502)
INTRO (U"A command to read a @Strings object from a simple text file. "
	"Each line is read as a separate string. See @Strings for an example.")
MAN_END

MAN_BEGIN (U"Sound: To Intensity...", U"ppgb", 20100605)
INTRO (U"A command to create an @Intensity object from every selected @Sound.")
ENTRY (U"Settings")
TAG (U"##Minimum pitch (Hz)")
DEFINITION (U"the minimum periodicity frequency in your signal. If you set it too high, "
	"you will end up with a pitch-synchronous intensity modulation. If you set it too low, "
	"your intensity contour may appear smeared, so you should set it as high as allowed by the signal "
	"if you want a sharp contour.")
TAG (U"##Time step (s)")
DEFINITION (U"the time step of the resulting intensity contour. If you set it to zero, the time step is computed as "
	"one quarter of the effective window length, i.e. as 0.8 / (%minimum_pitch).")
TAG (U"##Subtract mean")
DEFINITION (U"See @@Intro 6.2. Configuring the intensity contour@.")
ENTRY (U"Algorithm")
NORMAL (U"The values in the sound are first squared, then convolved with a Gaussian analysis window (Kaiser-20; sidelobes below -190 dB). "
	"The effective duration of this analysis window is 3.2 / (%minimum_pitch), which will guarantee that a periodic signal is analysed as having a "
	"pitch-synchronous intensity ripple not greater than 0.00001 dB.")
MAN_END

MAN_BEGIN (U"Sound & IntensityTier: Multiply", U"ppgb", 20000724)
INTRO (U"A command to create a new Sound from the selected @Sound and @Intensity objects.")
NORMAL (U"The resulting Sound equals the original sound, multiplied by a linear interpolation of the intensity. "
	"Afterwards, the resulting Sound is scaled so that its maximum absolute amplitude is 0.9.")
MAN_END

MAN_BEGIN (U"Strings", U"ppgb", 20201226)
INTRO (U"One of the @@types of objects@ in Praat. Represents an ordered list of strings.")
ENTRY (U"Creation")
NORMAL (U"The difficult way is to create a #Strings object from a generic Praat text file "
	"(if there are non-ASCII symbols, use UTF-8 or UTF-16 format):")
CODE (U"\"ooTextFile\"")
CODE (U"\"Strings\"")
CODE (U"5 ! number of strings")
CODE (U"\"Hello\"")
CODE (U"\"Goodbye\"")
CODE (U"\"Auf wiedersehen\"")
CODE (U"\"Tschüss\"")
CODE (U"\"Arrivederci\"")
NORMAL (U"In this example, we see that a double quote within a string should be written twice; "
	"the fourth string will therefore be read as ##Tschüss#. "
	"This file can be read simply with the generic @@Read from file...@ command from the #Open menu.")
NORMAL (U"An easier way is to use the special command @@Read Strings from raw text file...@. "
	"The file can then simply look like this:")
CODE (U"Hello")
CODE (U"Goodbye")
CODE (U"Auf wiedersehen")
CODE (U"Tschüss")
CODE (U"Arrivederci")
NORMAL (U"You can also create a #Strings object from a folder listing or from some other objects:")
LIST_ITEM (U"• @@Create Strings as file list...")
LIST_ITEM (U"• @@Distributions: To Strings...@")
LIST_ITEM (U"• @@OTGrammar: Generate inputs...@")
LIST_ITEM (U"• @@OTGrammar & Strings: Inputs to outputs...@")
MAN_END

MAN_BEGIN (U"Strings: To Distributions", U"ppgb", 19971025)
INTRO (U"A command to analyse each selected @Strings object into a @Distributions object.")
NORMAL (U"The resulting #Distributions will collect the occurrences of every string in the #Strings object, "
	"and put the number of occurrences in its first and only column.")
ENTRY (U"Example")
NORMAL (U"We start from the following #Strings:")
CODE (U"6 (number of strings)")
CODE (U"\"hallo\"")
CODE (U"\"dag allemaal\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
CODE (U"\"hallo\"")
CODE (U"\"tot morgen\"")
NORMAL (U"This will give us the following #Distributions:")
CODE (U"1 (number of columns) \"\" (no column name)")
CODE (U"\"hallo\"         3")
CODE (U"\"dag allemaal\"  1")
CODE (U"\"tot morgen\"    2")
MAN_END

MAN_BEGIN (U"Table", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat. See the @Statistics tutorial.")
MAN_END

MAN_BEGIN (U"TableOfReal", U"ppgb", 20030316)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A TableOfReal object contains a number of %cells. Each cell belongs to a %row and a %column. "
	"For instance, a TableOfReal with 10 rows and 3 columns has 30 cells.")
NORMAL (U"Each row and each column may be labeled with a %title.")
ENTRY (U"Creating a TableOfReal from data in a text file")
NORMAL (U"Suppose you have F1 and F2 data for vowels. "
	"You can create a simple text file like the following:")
CODE (U"\"ooTextFile\"  ! The line by which Praat can recognize your file")
CODE (U"\"TableOfReal\" ! The line that tells Praat about the contents")
CODE (U"2   \"F1\"  \"F2\"      ! Number of columns, and column labels")
CODE (U"3                   ! Number of rows")
CODE (U"\"a\" 800 1100         ! Row label (vowel), F1 value, F2 value")
CODE (U"\"i\" 280 2800         ! Row label (vowel), F1 value, F2 value")
CODE (U"\"u\" 260  560         ! Row label (vowel), F1 value, F2 value")
NORMAL (U"Praat is rather forgiving about the use of spaces, tabs, and newlines. "
	"See @@Save as text file...@ for general information.")
NORMAL (U"You will often have your data in a file with a self-describing format, "
	"i.e. in which the number of values on a line equals the number of columns "
	"of the table:")
CODE (U"800 1100")
CODE (U"280 2800")
CODE (U"260 560")
NORMAL (U"Such a file can be read with @@Read Matrix from raw text file...@. "
	"This creates a Matrix object, which can be cast to a TableOfReal object "
	"by @@Matrix: To TableOfReal@. The resulting TableOfReal does not have "
	"any row or column labels yet. You could add column labels with:")
CODE (U"Set column label (index)... 1 F1")
CODE (U"Set column label (index)... 2 F2")
NORMAL (U"Of course, if the row labels contain crucial information, "
	"and the number of rows is large, this is not a feasible method.")
MAN_END

MAN_BEGIN (U"TableOfReal: Set value...", U"ppgb", 19980105)
INTRO (U"A command to change the value of one table cell in each selected @TableOfReal object.")
ENTRY (U"Settings")
TAG (U"##Row number")
DEFINITION (U"the number of the row of the cell whose value you want to change.")
TAG (U"##Column number")
DEFINITION (U"the number of the column of the cell whose value you want to change.")
TAG (U"##New value")
DEFINITION (U"the value that you want the specified cell to have.")
MAN_END

}

/* End of file manual_Fon.cpp */
