/* manual_Manual.cpp
 *
 * Copyright (C) 1992-2011,2013 Paul Boersma
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

void manual_Manual_init (ManPages me);
void manual_Manual_init (ManPages me) {

MAN_BEGIN (L"Manual", L"ppgb", 20110101)
INTRO (L"The documentation system for the Praat program.")
NORMAL (L"You will get a manual window every time you choose anything from a #Help menu or press a #Help button.")
ENTRY (L"How to find what you are looking for")
NORMAL (L"You can navigate the manual in several ways:")
LIST_ITEM (L"\\bu To go to the Intro, use the #Home button.")
LIST_ITEM (L"\\bu To go to the information behind a %link (a piece of blue text), just click on it.")
LIST_ITEM (L"\\bu To go forward and backward through a tutorial with numbered pages, use ##1 ># and ##< 1#.")
LIST_ITEM (L"\\bu To %revisit previous pages, use the #< and #> buttons.")
LIST_ITEM (L"\\bu To browse %alphabetically, use the horizontal scroll bar and the buttons "
	"named ##< 1# and ##1 >#, or the ##Search for page (list)...# command in the ##Go to# menu.")
LIST_ITEM (L"\\bu To find a page with a %%known title%, use the ##Search for page...# command.")
NORMAL (L"The fastest way to find what you want is usually the #Search button.")
ENTRY (L"Search")
NORMAL (L"In the text field after the Search button, you can type strings, separated by spaces. "
	"When you press the #Return (or #Enter) key, or click the #Search button, "
	"all manual pages are searched for the combination of strings that you typed. "
	"The titles of the 20 best matching pages are displayed as links.")
NORMAL (L"##Example:# to know how to create a pitch contour from a sound, type")
CODE (L"sou pit")
NORMAL (L"and press #Return. The best matches should appear on top. These should include "
	"##Sound: To Pitch (ac)...# and ##Sound: To Pitch (cc)...#.")
NORMAL (L"The search is case-insensitive. For instance, the search string \"$script\" will give you all "
	"the pages that contain the words %script, %Script, %description, %PostScript, or %SCRIPT, and so on.")
NORMAL (L"#Background. The search algorithm uses the following heuristics:")
LIST_ITEM (L"\\bu A match in the page title is better than one in the rest of the text.")
LIST_ITEM (L"\\bu Pages with many matches are better than those with few.")
ENTRY (L"Your own manual pages")
NORMAL (L"To create your own manual pages, create @ManPages text files.")
MAN_END

MAN_BEGIN (L"ManPages", L"ppgb", 20130814)
INTRO (L"You can create a documentation or education system with files that you and others "
	"can read into Praat (with the @@Read from file...@ command). "
	"Your files will become a hypertext system very similar to the usual @Manual.")
ENTRY (L"Example 1: a single document")
NORMAL (L"If you create a single ManPages text file, it will look like a manual with a single page. "
	"Here is an example:")
CODE (L"ManPagesTextFile")
CODE (L"\"Welkom\" \"miep\" 19970820 0")
CODE (L"<intro> \"Hallo allemaal!\"")
CODE (L"<entry> \"Belangrijk...\"")
CODE (L"<normal> \"Hoogge\\bse\\\" \\\" erd publiek!\"")
CODE (L"<normal> \"Einde.\"")
NORMAL (L"A ManPages text file should start with the following information:")
LIST_ITEM (L"1. The word \"ManPagesTextFile\" on the first line.")
LIST_ITEM (L"2. The title of the manual page, between double quotes. "
	"This will be drawn at the top of the page. "
	"The name of the ManPages text file should be derived from this title (see below).")
LIST_ITEM (L"3. The author of the manual page, between double quotes. "
	"This will be drawn at the bottom of the page.")
LIST_ITEM (L"4. The date you created or modified the page, "
	"in the format year \\-- month (two digits) \\-- day (two digits), without spaces.")
LIST_ITEM (L"5. The recording time. If this is not zero, "
	"three sound buttons (see below) will appear at the top of the page.")
LIST_ITEM (L"6. A sequence of paragraph types and texts. "
	"You put the types between < and >, and the texts between double quotes "
	"(if your text contains a double quote, you should write two double quotes).")
NORMAL (L"The format of a ManPages text file is rather free, as long as the first line is correct, "
	"the four required pieces of information are there in the correct order, "
	"and there is a correct alternation between paragraph texts and types. "
	"If you put multiple elements on a line, there should be at least one space between them. "
	"You may distribute texts across multiple lines, as long as you do not add any spaces:")
CODE (L"<normal> \"Hoogge\\bse\\\" \\\" erd")
CODE (L"publiek!\"")
NORMAL (L"This will have exactly the same effect as above.")
ENTRY (L"Example 2: multiple documents")
NORMAL (L"The above example with a single document is not very useful. "
	"You will usually want to refer to other documents:")
CODE (L"ManPagesTextFile")
CODE (L"\"Welcome\" \"Paul Boersma\" 19970820 1.0")
CODE (L"<intro> \"Welcome to Paul's transcription course.\"")
CODE (L"<entry> \"Groups of speech sounds\"")
CODE (L"<normal> \"You can listen to the following sounds")
CODE (L"from the languages of the world,")
CODE (L"pronounced by a single speaker (me):\\\"r")
CODE (L"<list_item> \"\\@ Vowels, quite problematic for Dutch students!\"")
CODE (L"<list_item> \"\\@ \\@ Dorsal fricatives\\@ , equally problematic!\"")
NORMAL (L"With the symbol `\\@ ', you create a %link to another ManPages text file. "
	"A link will be drawn in blue on your screen. "
	"In this example, you have created links to the files ##Vowels.man# "
	"and ##Dorsal_fricatives.man# in the same directory as the current file "
	"(all ##.man# files have to be in the same directory; this makes it likely "
	"that their names are unique). "
	"If the link contains spaces or other non-alphanumeric symbols, "
	"you have to use three `\\@ ' symbols, as shown; "
	"with a single word, you may use a single `\\@ '.")
NORMAL (L"In resolving the file name, the ManPages system replaces spaces "
	"and other special symbols with underscores, "
	"and converts any initial lower-case character by its upper-case variant. "
	"For instance, if your link is \"\\@ \\@ back vowels\\@ \", "
	"the file name will be ##Back_vowels.man#.")
NORMAL (L"The title in the second line of ##Back_vowels.man# must be equal to the link name, "
	"though capitalization of the first letter is allowed. "
	"Thus, the title of ##Back_vowels.man# will probably be \"Back vowels\". "
	"Likewise, the starting file with the title \"Welcome\" should have the name ##Welcome.man# "
	"if any other files refer to it.")
ENTRY (L"Paragraph types")
NORMAL (L"A normal paragraph will have type <normal>. The hypertext system will "
	"leave a blank space between paragraphs with this type. "
	"The first paragraph of a manual page will normally have the type <intro>. "
	"Though this may look the same as <normal>, the search system of the @Manual "
	"may take account of the distinction.")
NORMAL (L"Headings (like the title \"Paragraph types\" of this subsection) "
	"have type <entry>. This will be drawn in a larger character size.")
NORMAL (L"For lists, you use the type <list_item>. You will often combine this with %button symbols, "
	"like in the following:")
CODE (L"<normal> \"Choose a colour:\"")
CODE (L"<list_item> \"\\bsbu \\@ Red.\"")
CODE (L"<list_item> \"\\bsbu \\@ Green.\"")
CODE (L"<list_item> \"\\bsbu \\@ Blue.\"")
NORMAL (L"For text that should appear with a fixed character width, you use the type <code>.")
NORMAL (L"For a paragraph that should be connected with the following paragraph "
	"without leaving a blank space "
	"(probably a list item or a definition), you use the type <tag>.")
NORMAL (L"For a paragraph with a blank left margin, you use the type <definition>.")
NORMAL (L"For a paragraph with an embedded script (a picture), you use the type <script> (see below).")
ENTRY (L"Special symbols and styles")
NORMAL (L"You can use all of Praat's @@special symbols@ and @@text styles@, "
	"except that some %single text-style symbols have different meanings:")
LIST_ITEM (L"\\bu A single percent sign introduces a word in italic: $$\\% pot$ gives %pot.")
LIST_ITEM (L"\\bu A single number sign introduces a word in bold: $$\\# pot$ gives #pot.")
LIST_ITEM (L"\\bu A single dollar sign introduces a word in monospace: $$\\$ pot$ gives $pot.")
LIST_ITEM (L"\\bu A single underscore is rendered as an underscore: $$a_b$ gives a_b.")
NORMAL (L"To create a single italic, bold, or subscripted letter, "
	"you revert to the usual technique for stretches of italic, bold, or subscripted text. "
	"So, to get %F__1_, you type $$\\% F_\\_ 1_.")
ENTRY (L"Sound links")
NORMAL (L"Your text may contain links to sound files. They are drawn in blue. "
	"The format is:")
CODE (L"<normal> \"You should know that \\@ \\@ \\bsFIct.aifc|\\bsct\\@  is more open than "
	"\\@ \\@ \\bsFIo.aifc|o\\@ .\"")
NORMAL (L"On your screen, you will see an #\\ct and an #o symbol, both drawn in blue. "
	"If you click on one of these sound links, one of the sound files ##ct.aifc# "
	"or ##o.aifc# will be played.")
NORMAL (L"The format of the sound link \"$$\\@ \\@ \\bsFIo.aifc|o\\@ $\" is to be understood "
	"as follows. The pipe symbol separates the link information (\\bsFIo.aifc) from the viewable "
	"link text (o). The link information is introduced with a symbol (\\bsFI) that "
	"tells the manual system that a sound file name follows. The manual system reads "
	"this file, sees that it contains a sound, and plays that sound.")
NORMAL (L"You can use relative path names, e.g., \\bsFIsounds/o.aifc refers to the file "
	"##o.aifc# in the subdirectory #sounds, which must be contained in the same directory "
	"as the ##.man# files. To make sure that your manual pages run on all platforms "
	"(Windows, Macintosh, Unix), you will want to use the forward slash (/) to separate "
	"the directory name(s) from the file name, as in this example "
	"(i.e. you avoid the backslash (\\bs) that is usual on Windows computers).")
ENTRY (L"Pictures as embedded scripts")
NORMAL (L"Your text may contain Praat scripts. They typically draw a picture in your manual page, "
	"with the font and font size of the manual until you specify otherwise in the script. The format is:")
CODE (L"<script> 4.5 4 \"")
CODE1 (L"Draw inner box")
CODE1 (L"Axes... 0 100 0 100")
CODE1 (L"Text... 50 Centre 50 Half Hello!!")
CODE (L"\\\"r")
NORMAL (L"The two numbers after ##<script># are the width and the height of the picture "
	"(the \"outer viewport\") in inches, if the font size of the manual is 12. "
	"If the font size is larger, the viewport will be scaled up accordingly.")
NORMAL (L"Please note that the script is enclosed within double quotes. "
	"Therefore, you will have to double any double quotes that occur in the script.")
NORMAL (L"If needed, a script like this can create objects in the object list of the manual. "
	"However, you have to make sure that you remove them after use:")
CODE (L"<script> 6 3 \"")
CODE1 (L"Create Sound from formula... sineWithNoise Mono 0.0 1.0 44100 1/2*sin(2*pi*377*x)+randomGauss(0,0.1)")
CODE1 (L"To Spectrogram... 0.005 5000 0.002 20 Gaussian")
CODE1 (L"Paint... 0 0 0 0 100.0 yes 50.0 6.0 0.0 yes")
CODE1 (L"plus Sound sineWithNoise")
CODE1 (L"Remove")
CODE (L"\\\"r")
NORMAL (L"Note that unlike the previous script, this script does not set the font and font size. "
	"This means that the drawing will use the font and font size of the manual page, "
	"which is usually what you want.")
NORMAL (L"For obvious safety reasons, embedded scripts cannot contain commands that change the contents of any disk "
	"or send messages. Thus, commands like ##Save as WAV file...#, ##filedelete out.txt#, ##string\\$  >> out.txt#, "
	"#system, or #sendpraat are forbidden. Several other commands, such as #pause and #editor, "
	"are irrelevant inside pictures and are therefore forbidden as well. "
	"Note that commands like #echo, ##Read from file...#, and #execute are still available "
	"(with the last two, you can use relative paths; "
	"with #execute, you can only run scripts that do not contain any of the forbidden commands).")
NORMAL (L"The commands ##Set outer viewport...# and ##Set inner viewport...# are available; "
	"they count in inches (if the font size of the manual is 12). The (0, 0) point is in the upper left corner, "
	"as in the Picture window, so that you can test your picture with a normal Praat script; "
	"for instance, the following script draws a cross in the upper half of the picture and a rectangle in the lower half:")
CODE (L"<script> 4.5 4 \"")
CODE1 (L"Axes... 0 100 0 100")
CODE1 (L"Select inner viewport... 0 4.5 0 2")
CODE1 (L"Draw line... 0 0 100 100")
CODE1 (L"Draw line... 0 100 100 0")
CODE1 (L"Select inner viewport... 0 4.5 2 4")
CODE1 (L"Draw rectangle... 0 100 0 100")
CODE (L"\\\"r")
SCRIPT (4.5, 4, L""
	"Axes... 0 100 0 100\n"
	"Select inner viewport... 0 4.5 0 2\n"
	"Draw line... 0 0 100 100\n"
	"Draw line... 0 100 100 0\n"
	"Select inner viewport... 0 4.5 2 4\n"
	"Draw rectangle... 0 100 0 100\n"
)
ENTRY (L"Script links")
NORMAL (L"Your text may contain links to Praat scripts. They are drawn in blue. "
	"The format is:")
CODE (L"<normal> \"Here is a script that \\@ \\@ \\bsSCdraw.praat|draws\\@  what you have selected.\"")
NORMAL (L"On your screen, you will see the word #draws, drawn in blue. "
	"If you click on it, the script ##draw.praat# will be executed. The string \"$$\\bsSC$\" indicates "
	"that a script file name follows. As with sounds, you can use relative file paths. The script can take "
	"arguments:")
CODE (L"<normal> \"This script \\@ \\@ \\bsSCload2files.praat hello.wav hello.TextGrid|loads\\@  some files.\"")
NORMAL (L"If the script file name or any arguments except the last contain spaces, they have to be enclosed "
	"within double quotes, which have to be doubled again in the ManPage code:")
CODE (L"<normal> \"Here is a script that \\@ \\@ \\bsSC\\\" \\\" my scripts/draw.praat\\\" \\\" |draws\\@  what you have selected.\"")
CODE (L"<normal> \"This script \\@ \\@ \\bsSC\\\" \\\" my scripts/load2files.praat\\\" \\\"  \\\" \\\" my sounds/hello.wav\\\" \\\"  my textgrids/hello.TextGrid|loads\\@  some files.\"")
NORMAL (L"For safety reasons, the user will see a warning about "
	"\"trusting the author of the manual pages\" if you include script links in your manual.")
ENTRY (L"How to separate link information and link text")
NORMAL (L"The separation between link information and viewable link text, as seen "
	"in the above description of sound and script links, is actually a general mechanism "
	"in the hypertext system. For instance, if you want to see the text \"x\" "
	"in blue on your screen, and create a link from it to the page \"Dorsal fricatives\", "
	"you specify the link as $$\\@ \\@ Dorsal fricatives|x\\@ $.")
ENTRY (L"Sound buttons")
NORMAL (L"If the %%recording time% in the second line is not 0, "
	"three buttons will appear near the top of the page:")
NORMAL (L"1. The #Record button allows you to record a sound from the microphone.")
NORMAL (L"2. The #Play button allows you to play this recorded sound. "
	"In this way, you can compare your own utterances with the sounds "
	"behind the sound links, for example.")
NORMAL (L"3. The ##Copy last played to list# button copies the latest sound to the list of objects, "
	"allowing you to perform analyses on it, save it to disk, et cetera. "
	"The latest sound may be a sound that you recorded with the #Record button, "
	"a sound that you played with the #Play button, "
	"or a sound that you played by clicking on a sound link, "
	"whichever occurred most recently.")
ENTRY (L"And beyond")
NORMAL (L"If you need even more flexibility than ManPages offer you, consider using the @@Demo window@ instead.")
MAN_END

}

/* End of file manual_Manual.cpp */
