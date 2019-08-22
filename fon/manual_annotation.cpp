/* manual_annotation.cpp
 *
 * Copyright (C) 1992-2011,2014-2017 Paul Boersma
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

void manual_annotation_init (ManPages me);
void manual_annotation_init (ManPages me) {

MAN_BEGIN (U"Create TextGrid...", U"ppgb", 20101228)
INTRO (U"A command to create a @TextGrid from scratch.")
ENTRY (U"Settings")
TAG (U"##Start time (s)")
DEFINITION (U"the start time, usually 0 seconds.")
TAG (U"##End time (s)")
DEFINITION (U"the end time in seconds, usually the duration.")
TAG (U"##Tier names")
DEFINITION (U"a list of the names of the tiers that you want to create, separated by spaces.")
TAG (U"##Point tiers")
DEFINITION (U"a list of the names of the tiers that you want to be %%point tiers%; "
	"the rest of the tiers will be %%interval tiers%.")
MAN_END

MAN_BEGIN (U"PointProcess: To TextGrid...", U"ppgb", 19980113)
INTRO (U"A command to create an empty @TextGrid from every selected @PointProcess.")
NORMAL (U"The only information in the PointProcess that is used, is its starting and finishing times.")
ENTRY (U"Settings")
TAG (U"##Tier names")
DEFINITION (U"a list of the names of the tiers that you want to create, separated by spaces.")
TAG (U"##Point tiers")
DEFINITION (U"a list of the names of the tiers that you want to be %%point tiers%; "
	"the rest of the tiers will be %%interval tiers%.")
ENTRY (U"Example")
NORMAL (U"If ##Tier names# is \"a b c\", and ##Point tiers# is \"b\", "
	"the resulting TextGrid object will contain an interval tier named \"a\", "
	"a point tier named \"b\", and another interval tier named \"c\".")
MAN_END

MAN_BEGIN (U"PointProcess: To TextGrid (vuv)...", U"ppgb", 19980210)
INTRO (U"A command to create a @TextGrid with voiced/unvoiced information "
	"from every selected @PointProcess.")
ENTRY (U"Settings")
TAG (U"##Maximum period (s)")
DEFINITION (U"the maximum interval that will be consider part of a larger voiced interval.")
TAG (U"##Mean period (s)")
DEFINITION (U"half of this value will be taken to be the amount to which a voiced interval "
	"will extend beyond its initial and final points. ##Mean period# should be less than "
	"##Maximum period#, or you may get intervals with negative durations.")
ENTRY (U"Example")
NORMAL (U"If ##Maximum period# is 0.02 s, and ##Mean period# is 0.01 s, "
	"and the point process is 0.1 seconds long, with points at 20, 28, 39, 61, and 72 milliseconds, "
	"the resulting TextGrid object will contain an interval tier "
	"with \"U\" intervals at [0 ms, 15 ms], [44 ms, 56 ms], and [77 ms, 100 ms], "
	"and \"V\" intervals at [15 ms, 44 ms] and [56 ms, 77 ms].")
MAN_END

MAN_BEGIN (U"PointProcess: Up to TextGrid...", U"ppgb", 20101230)
INTRO (U"A command to promote every selected @PointProcess to a @TextGrid with a single text tier in it.")
ENTRY (U"Setting")
TAG (U"##Text")
DEFINITION (U"the text that will be placed in every point.")
ENTRY (U"Behaviour")
NORMAL (U"The times of all the points are trivially copied, and so is the time domain. "
	"The text information will be the same for every point.")
MAN_END

MAN_BEGIN (U"Sound: To TextGrid...", U"ppgb", 19980730)
INTRO (U"A command to create a @TextGrid without any labels, copying the time domain from the selected @Sound.")
ENTRY (U"Settings")
TAG (U"%%Tier names")
DEFINITION (U"a list of the names of the tiers that you want to create, separated by spaces.")
TAG (U"%%Point tiers")
DEFINITION (U"a list of the names of the tiers that you want to be %%point tiers%; "
	"the rest of the tiers will be %%interval tiers%.")
ENTRY (U"Example")
NORMAL (U"If ##Tier names# is \"a b c\", and ##Point tiers# is \"b\", "
	"the resulting TextGrid object will contain an interval tier named \"a\", "
	"a point tier named \"b\", and another interval tier named \"c\".")
MAN_END

MAN_BEGIN (U"SpellingChecker", U"ppgb", 20190616)
INTRO (U"One of the @@types of objects@ in Praat. For checking the spelling in texts and @TextGrid objects.")
ENTRY (U"1. How to create a SpellingChecker object")
NORMAL (U"You normally read in a SpellingChecker with @@Read from file...@ from the #Open menu.")
ENTRY (U"2. How to check the spelling of a @TextGrid")
NORMAL (U"A SpellingChecker object can be used fur purposes of spelling checking. In order "
	"to check the spellings in a @TextGrid object, you first view the TextGrid in an editor window by selecting "
	"the TextGrid together with the SpellingChecker object, and clicking ##View & Edit#. In most cases, you will also want "
	"to select a @Sound or @LongSound object before clicking ##View & Edit#, so that a representation of the sound "
	"is also visible (and audible) in the editor. Thus, you typically select three objects and click ##View & Edit#. "
	"The editor then allows you to check the spellings "
	"(commands ##Check spelling in tier# and ##Check spelling in interval# from the #Spell menu).")
ENTRY (U"3. How to create a SpellingChecker object for the first time")
NORMAL (U"If you are the maintainer of a word list for spelling checking, you will want "
	"to convert this list to a SpellingChecker object that you can distribute among the transcribers of your corpus.")
NORMAL (U"The first step is to create a @WordList object from your text file, as described on the @WordList man page. "
	"Then you simply click ##To SpellingChecker#. A button labelled ##View & Edit...# appears. "
	"This command allows you to set the following attributes of the SpellingChecker object:")
/*TAG (U"##Check matching parentheses")
DEFINITION (U"determines whether it is considered a spelling error if parentheses do not match, as in the string \"Hi) there\".")*/
TAG (U"##Allow all parenthesized")
DEFINITION (U"this flag determines whether text between parentheses is ignored in spelling checking. "
	"This would allow the transcriber to mark utterances in foreign languages, which cannot be found in the lexicon.")
TAG (U"##Separating characters")
DEFINITION (U"determines the set of characters (apart from the space character) that separate words. "
	"The standard is \".,;:()\". If a string like \"error-prone\" should be considered two separate words, "
	"you will like to change this to \".,;:()-\". "
	"For the Corpus of Spoken Dutch (CGN), the hyphen is not a separator, since words like \"mee-eter\" should be checked as a whole. "
	"If a string like \"Mary's\" should be considered two separate words, include the apostrophe.")
/*TAG (U"##Forbidden strings")
DEFINITION (U"this will mainly contain characters that are not allowed in your corpus. For the CGN, this is \": ; \\\" \".")*/
TAG (U"##Allow all names")
DEFINITION (U"determines whether all words that start with a capital are allowed. For the CGN, this is on, "
	"since the lexicon does not contain many names.")
TAG (U"##Name prefixes")
DEFINITION (U"a space-separated list that determines what small groups of characters can precede names. For the CGN, "
	"this is \"'s- d' l'\", since names like %%'s-Gravenhage%, %%d'Ancona%, and %%l'H\\o^pital% should be ignored by the spelling checker.")
TAG (U"##Allow all words containing")
DEFINITION (U"a space-separated list of strings that make a word correct even if not in the lexicon. "
	"For the CGN, this is \"* xxx\", since words like %%keuje*d% and %%verxxxing% should be ignored by the spelling checker.")
TAG (U"##Allow all words starting with")
DEFINITION (U"a space-separated list of prefixes that make a word correct even if not in the lexicon. "
	"For the CGN, this is empty.")
TAG (U"##Allow all words ending in")
DEFINITION (U"a space-separated list of suffixes that make a word correct even if not in the lexicon. "
	"For the CGN, this is \"-\", since the first word in %%verzekerings- en bankwezen% should be ignored by the spelling checker.")
MAN_END

MAN_BEGIN (U"TextGrid", U"ppgb", 20110128)
INTRO (U"One of the @@types of objects@ in Praat, used for %annotation (segmentation and labelling). "
	"For tutorial information, see @@Intro 7. Annotation@.")
ENTRY (U"Description")
NORMAL (U"A #TextGrid object consists of a number of %tiers. There are two kinds of tiers: "
	"an %%interval tier% is a connected sequence of labelled intervals, with %boundaries in between. "
	"A %%point tier% is a sequence of labelled points.")
ENTRY (U"How to create a TextGrid")
TAG (U"From scratch:")
LIST_ITEM (U"@@Sound: To TextGrid...@ (takes the time domain from the Sound)")
LIST_ITEM (U"@@LongSound: To TextGrid...@ (takes the time domain from the LongSound)")
LIST_ITEM (U"@@PointProcess: To TextGrid...@ (takes the time domain from the PointProcess)")
LIST_ITEM (U"@@PointProcess: To TextGrid (vuv)...@ (labels voiced and unvoiced intervals)")
LIST_ITEM (U"@@Create TextGrid...@")
TAG (U"From merging existing TextGrids with each other:")
LIST_ITEM (U"@@TextGrids: Merge@")
ENTRY (U"How to edit a TextGrid")
NORMAL (U"You select a TextGrid alone or together with a @Sound or @LongSound, and click ##View & Edit#. "
	"A @TextGridEditor will appear on your screen, containing the TextGrid "
	"and an optional copy of the Sound or LongSound.")
ENTRY (U"How to draw a TextGrid")
TAG (U"You can draw a TextGrid to the @@Picture window@ with:")
LIST_ITEM (U"##TextGrid: Draw...")
LIST_ITEM (U"##TextGrid & Sound: Draw...")
LIST_ITEM (U"##TextGrid & Pitch: Draw...")
LIST_ITEM (U"##TextGrid & Pitch: Draw separately...")
MAN_END

MAN_BEGIN (U"TextGrid: Count labels...", U"ppgb", 20140421)
INTRO (U"A command to ask the selected @TextGrid object how many of the specified labels "
	"it contains in the specified tier.")
ENTRY (U"Settings")
TAG (U"##Tier number")
DEFINITION (U"the number (1, 2, 3...) of the tier whose labels you want to investigate.")
TAG (U"##Label text")
DEFINITION (U"the text on the labels that you want to count.")
ENTRY (U"Behaviour")
NORMAL (U"The number of intervals or points with label %%Label text% in tier %%Tier number% "
	"is written into the @@Info window@. If the specified tier does not exist, the number will be 0.")
ENTRY (U"Scripting")
NORMAL (U"You can use this command to put the number into a script variable:")
CODE (U"selectObject: \"TextGrid hallo\"")
CODE (U"number_of_a = Count labels: 1, \"a\"")
NORMAL (U"In this case, the value will not be written into the Info window.")
MAN_END
 
MAN_BEGIN (U"TextGrids: Merge", U"ppgb", 20101230)
INTRO (U"A command to merge all selected @TextGrid objects into a new @TextGrid.")
MAN_END

MAN_BEGIN (U"TextGridEditor", U"ppgb", 20110131)
INTRO (U"One of the @Editors in Praat, for editing a @TextGrid object.")
NORMAL (U"You can optionally include a copy of a @Sound or @LongSound in this editor, "
	"by selecting both the TextGrid and the Sound or LongSound before clicking ##View & Edit#. "
	"The Sound or LongSound is shown in the upper part of the window, the tiers in the lower part. "
	"A text window at the top shows the text of the %#selected interval or point, "
	"i.e. the interval or point at the location of the cursor. "
	"All tiers are visible, and if you do not zoom in, all boundaries, points, and texts are visible, too. "
	"You can do many of the same things that you can do with a @SoundEditor or @LongSoundEditor.")
ENTRY (U"Positioning the cursor or the selection marks")
NORMAL (U"To position the cursor hair, @click in the #Sound, on a boundary, on a point, or inside an interval.")
NORMAL (U"To select any part of the time domain, use the @@time selection@ mechanism; "
	"if you do this by clicking in a tier, "
	"the selected time domain will snap to the nearest boundary or point.")
ENTRY (U"Creating new intervals, boundaries, points, or tiers")
NORMAL (U"To create a new interval, create a new boundary in an interval tier.")
NORMAL (U"To create a new boundary or point in a tier, @click inside the cursor circle in that tier, "
	"or choose one of the commands in the Boundary/Point menu to insert a boundary at the cursor time "
	"on the selected tier (shortcut: Enter) or on any tier (shortcuts: Command-F1 through Command-F9). The original text "
	"in the interval that is split, is divided up between the two resulting intervals, depending on "
	"the position of the text cursor in the text window.")
NORMAL (U"To create a new tier, choose ##Add interval tier# or ##Add point tier# from the #Tier menu.")
ENTRY (U"Playing an entire interval, or part of it")
NORMAL (U"As in many other editors, you can play a stretch of sound by clicking in any of the rectangles "
	"around the drawing area.")
NORMAL (U"To play an interval of an interval tier, you first @click inside it. This will make the interval %selected, "
	"which means that the visible part of the interval will be drawn in yellow. The cursor will be positioned "
	"at the start of the interval, and the time selection will comprise exactly the interval. This means that "
	"you can use the Tab key to play the interval. If you press it while a sound is playing, "
	"the Tab key will halt the playing sound, and the cursor will move to the time at which the sound stopped playing. "
	"This helps you to divide up a long sentence into parts that you can remember long enough to write them "
	"down.")
NORMAL (U"The Tab key will play the selected interval.")
ENTRY (U"Editing the text in an interval or at a point")
NORMAL (U"To edit the label text of an interval or point:")
LIST_ITEM (U"1. Select that interval or point by clicking in or on it. The text currently in the interval or point "
	"will appear in the text window.")
LIST_ITEM (U"2. Just type the text, and use the mouse and the arrow keys to navigate the text window. "
	"Everything you type will become visible immediately in the text window as well as in the selected "
	"interval or point.")
NORMAL (U"You can use all the @@Special symbols@ "
	"that you can use elsewhere in Praat, including mathematical symbols, Greek and Chinese letters, superscripts, "
	"and phonetic symbols.")
ENTRY (U"Selecting a tier")
NORMAL (U"To select a tier, click anywhere inside it. Its number and name will be drawn in red, "
	"and a pointing finger symbol (☞) will appear on its left.")
ENTRY (U"Selecting a boundary or point")
NORMAL (U"To select a boundary on an interval tier, @click in its vicinity or inside the following interval; "
	"the boundary will be drawn in red. The text in the interval will appear in the text window.")
NORMAL (U"To select a point on a point tier, @click in its vicinity; it will be drawn in red. "
	"The text of the point will appear in the text window.")
ENTRY (U"Moving one or more boundaries or points")
NORMAL (U"To move a boundary or point to another time position, @drag it with the mouse.")
NORMAL (U"To move all the boundaries and points with the same time (on different tiers) to another time position, "
	"@@Shift-drag@ them.")
NORMAL (U"To move boundaries or points to the exact time position of a boundary or point on an other tier, "
	"@drag them into that other tier and into the vicinity of that boundary or point.")
NORMAL (U"To move boundaries or points to the exact time position of the cursor, "
	"@drag them into the vicinity of the cursor.")
ENTRY (U"Removing a boundary, point, or tier")
NORMAL (U"To remove a selected #boundary, choose #Remove from the #Boundary menu. "
	"This creates a new interval which is the union of the two intervals originally adjoining the boundary; "
	"the new text of this interval is the concatenation of the two original texts, "
	"except if these were equal, in which case the new text equals both original texts.")
NORMAL (U"To remove a selected #point, choose #Remove from the #Point menu.")
NORMAL (U"To remove a selected #tier, choose ##Remove entire tier# from the #Tier menu.")
/*"To rename a selected tier, choose 'Rename...' from the 'Tier' menu. "*/
/*"To remove all the boundaries and labels in a selected tier, "
"   choose 'Clear entire tier' from the 'Tier' menu. "*/
ENTRY (U"Extracting a part of the sound")
NORMAL (U"To copy the selected part of the Sound or LongSound as a Sound to the @@List of Objects@, "
	"choose ##Extract sound selection# from the #File menu. You can specify whether you want "
	"the time domain of the resulting Sound to match the starting and finishing times of the "
	"selection or whether you want the time domain of the resulting Sound to start at zero seconds.")
NORMAL (U"If you are viewing a LongSound, you can save the selected part of it to a 16-bit sound file "
	"(AIFF, AIFC, WAV, NeXT/Sun, NIST) with a command from the File menu.")
ENTRY (U"Accelerations")
NORMAL (U"To save the @TextGrid object as a text file without going to the @@Object window@: "
	"choose ##Save TextGrid as text file...# from the #File menu.")
ENTRY (U"Searching")
NORMAL (U"The Search menu contains the command #Find (Command-F), which will allow you to specify "
	"a text whose first occurrence will then be looked for in the currently selected tier "
	"(starting from the currently selected text in the currently selected interval). "
	"The command ##Find again# (Command-G) will search for the next occurrence of the same search text.")
ENTRY (U"Checking the spelling")
NORMAL (U"You can check the spelling of the intervals in your tiers by including a @SpellingChecker "
	"object as you launch the editor: select TextGrid + (Long)Sound + SpellingChecker, "
	"then click ##View & Edit#. The #Spell menu will contain the commands ##Check spelling in tier# (Command-N), and ##Check spelling in interval# "
	"which will search for the next word in the tier or interval that does not occur in the lexicon.")
MAN_END

MAN_BEGIN (U"WordList", U"ppgb", 20190616)
INTRO (U"One of the @@types of objects@ in Praat. "
	"An object of class WordList contains a sorted list of strings in a system-independent format. "
	"WordList objects can be used for spelling checking after conversion to a @SpellingChecker object.")
ENTRY (U"1. How to create a WordList object")
NORMAL (U"You will normally create a WordList object by reading a binary WordList file. "
	"You'll use the generic @@Read from file...@ command from the #Open menu.")
NORMAL (U"See below under 3 for how to create such a file.")
ENTRY (U"2. What you can do with a Wordlist object")
NORMAL (U"The main functionality of a WordList is its ability to tell you whether it contains a certain string. "
	"If you select a WordList, you can query the existence of a specific word by using the ##Has word# "
	"command. You supply the word and press OK. If the WordList does contain the word, "
	"the value \"1\" will be written to the Info window; otherwise, the value \"0\" will be written.")
ENTRY (U"3. How to create a binary WordList file")
NORMAL (U"You can create a binary WordList file from a simple text file that contains a long list of words. "
	"Perhaps such a text file has been supplied by a lexicographic institution in your country; "
	"because of copyright issues, such word lists cannot be distributed with the Praat program. "
	"To convert the simple text file into a compressed WordList file, you basically take the following steps:")
CODE (U"Read Strings from raw text file: \"lexicon.txt\"")
CODE (U"Sort")
CODE (U"To WordList")
CODE (U"Save as binary file: \"lexicon.WordList\"")
NORMAL (U"I'll explain these steps in detail. "
	"For instance, a simple text file may contain the following list of words:")
CODE (U"cook")
CODE (U"cooked")
CODE (U"cookie")
CODE (U"cookies")
CODE (U"cooking")
CODE (U"cooks")
CODE (U"Copenhagen")
CODE (U"Købnhavn")
CODE (U"München")
CODE (U"Munich")
CODE (U"ångström")
NORMAL (U"These are just 11 words, but the procedure will work fine if you have a million of them, "
	"and enough memory in your computer.")
NORMAL (U"You can read the file into a @Strings object with @@Read Strings from raw text file...@ "
	"from the #Open menu in the Objects window. The resulting @Strings object contains 11 strings in the above order, "
	"as you can verify by viewing them with @Inspect.")
NORMAL (U"If you select the Strings, you can click the ##To WordList# button. "
	"However, you will get the following complaint:")
CODE1 (U"String \"Copenhagen\" not sorted. Please sort first.")
NORMAL (U"This complaint means that the strings have not been sorted in Unicode sorting order. "
	"So you click #Sort, and the Strings object becomes:")
CODE (U"Copenhagen")
CODE (U"Købnhavn")
CODE (U"Munich")
CODE (U"München")
CODE (U"cook")
CODE (U"cooked")
CODE (U"cookie")
CODE (U"cookies")
CODE (U"cooking")
CODE (U"cooks")
CODE (U"ångström")
NORMAL (U"The strings are now in Unicode sorting order, in which capitals come before lower-case letters, "
	"and composite characters follow the latter.")
NORMAL (U"Clicking ##To WordList# now succeeds, and a WordList object appears in the list. "
	"If you save it to a text file (with the Save menu), you will get the following file:")
CODE (U"File type = \"ooTextFile\"")
CODE (U"Object class = \"WordList\"")
CODE (U"")
CODE (U"string = \"Copenhagen")
CODE (U"Købnhavn")
CODE (U"Munich")
CODE (U"München")
CODE (U"cook")
CODE (U"cooked")
CODE (U"cookie")
CODE (U"cookies")
CODE (U"cooking")
CODE (U"cooks")
CODE (U"ångström\\\" ")
NORMAL (U"Note that any double quotes (\\\" ) that appear inside the strings, will be doubled, "
	"as is done everywhere inside strings in Praat text files.")
NORMAL (U"After you have created a WordList text file, you can create a WordList object just by reading this file "
	"with @@Read from file...@ from the #Open menu.")
NORMAL (U"The WordList object has the advantage over the Strings object that it won't take up more "
	"memory than the original word list. This is because the WordList is stored as a single string: "
	"a contiguous list of strings, separated by new-line symbols.")
MAN_END

/*
longSound = selected ("LongSound")
longSoundNaam$ = selected$ ("LongSound")
textGrid = selected ("TextGrid")
select 'textGrid'
aantalIntervallen = Get number of intervals... 1
for interval to aantalIntervallen
   select 'textGrid'
   begintijd = Get starting point... 1 interval
   eindtijd = Get end point... 1 interval
   select 'longSound'
   Extract part... begintijd eindtijd no
   Save as WAV file... C:\Geluiden\'longSoundNaam$'_'interval'.wav
   Remove
endfor


# This is a Praat script that assumes that a single TextGrid is selected,
# and that the data in the first (or only) tier is in the form
# silence-text-silence-text and so on. The script will then write
# the durations of the silences and the texts into the Info window.

form Tabulate speech and silences
   comment Give the output file name:
   text fileName c:\windows\desktop\kim\out.txt
endform

# The following line provides the header,
# separated by tabs for easy inclusion in Excel.
echo Pause'tab$'Speech'tab$'Text

# Every next line writes three pieces of data,
# separated by tabs again.

# The following is a command from the Query menu
# that appears when you select a TextGrid object.
numberOfIntervals = Get number of intervals... 1

# The number of silence-speech pairs is half of that number,
# disregarding any silence at the end.
numberOfPairs = numberOfIntervals div 2

# Cycle through all the silence-speech pairs.

for pair to numberOfPairs
   silenceInterval = 2 * pair - 1   ; interval 1, 3, 5, and so on
   speechInterval = 2 * pair   ; interval 2, 4, 6, and so on
   # Check that the silence interval is indeed empty.
   silenceText$ = Get label of interval... 1 silenceInterval
   if silenceText$ <> ""
      exit Interval 'silenceInterval' should be a silence, but isn't.
   endif
   # Also check that the speech interval contains some text.
   speechText$ = Get label of interval... 1 speechInterval
   if speechText$ = ""
      exit Interval 'speechInterval' should contain text, but doesn't.
   endif
   # So now we have checked that the intervals contain the data
   # that Kim wanted them to contain. We're ready to write the data!
   silenceBegin = Get starting point... 1 silenceInterval
   silenceEnd = Get end point... 1 silenceInterval
   silenceDuration_ms = (silenceEnd - silenceBegin) * 1000
   speechBegin = Get starting point... 1 speechInterval
   speechEnd = Get end point... 1 speechInterval
   speechDuration_ms = (speechEnd - speechBegin) * 1000
   printline 'silenceDuration_ms:0''tab$'
   ...'speechDuration_ms:0''tab$'
   ...'speechText$'
endfor
filedelete 'fileName$'
fappendinfo 'fileName$'

*/

}

/* End of file manual_annotation.cpp */
