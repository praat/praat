/* manual_Script.cpp
 *
 * Copyright (C) 1992-2011,2013,2014,2015,2016,2017 Paul Boersma
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
#include "praat_version.h"

void manual_Script_init (ManPages me);
void manual_Script_init (ManPages me) {

MAN_BEGIN (U"Action commands", U"ppgb", 20110129)
INTRO (U"The commands in the @@Dynamic menu@ of the @@Object window@.")
NORMAL (U"These commands are only available if the right kinds of objects are selected. They are shown in a scrollable list, "
	"or in the #Save menu if they start with \"Save as \" or \"Append to \".")
MAN_END

MAN_BEGIN (U"Add action command...", U"ppgb", 20060920)
INTRO (U"One of the hidden commands in the #Praat menu of the @@Object window@. "
	"With this command, you add a button to the dynamic menu in the Object window.")
ENTRY (U"Settings")
NORMAL (U"See @@Add to dynamic menu...@.")
ENTRY (U"Usage")
NORMAL (U"You can use this command in your @@initialization script@ or in @@plug-ins@.")
NORMAL (U"Normally, however, if you want to add a command to the dynamic menu, "
	"you would use the command @@Add to dynamic menu...@ of the @ScriptEditor instead.")
MAN_END

MAN_BEGIN (U"Add menu command...", U"ppgb", 20060920)
INTRO (U"One of the hidden commands in the #Praat menu of the @@Object window@. "
	"With this command, you add a button to any of the fixed menus in the Object or Picture window.")
ENTRY (U"Settings")
NORMAL (U"See @@Add to fixed menu...@.")
ENTRY (U"Usage")
NORMAL (U"You can use this command in your @@initialization script@ or in @@plug-ins@.")
NORMAL (U"Normally, however, if you want to add a command to a fixed menu, "
	"you would use the command @@Add to fixed menu...@ of the @ScriptEditor instead.")
MAN_END

MAN_BEGIN (U"Add to dynamic menu...", U"ppgb", 20140107)
INTRO (U"A command in the #File menu of the @ScriptEditor.")
NORMAL (U"With this command, you add a button to the dynamic menu in the @@Object window@. "
	"This button will only be visible if the specified combination of objects is selected. "
	"Clicking the button will invoke the specified @@Praat script@.")
ENTRY (U"Settings")
TAG (U"%%Class 1")
DEFINITION (U"the name of the class of the object to be selected. "
	"For instance, if a button should only appear if the user selects a Sound, this would be \"Sound\".")
TAG (U"%%Number 1")
DEFINITION (U"the number of objects of %class1 that have to be selected. For most built-in commands, this number is unspecified (0); "
	"e.g., the user can choose #Draw... or ##To Spectrum# regardless of whether she selected 1, 2, 3, or more Sound objects. "
	"If the number of selected objects is different from %number1, the button will be visible but insensitive.")
TAG (U"%%Class 2")
DEFINITION (U"the name of the class of the second object to be selected, different from %class1. "
	"Normally the empty string (\"\").")
TAG (U"%%Number 2")
DEFINITION (U"the number of selected objects of %class2.")
TAG (U"%%Class 3")
DEFINITION (U"the name of the class of the third object to be selected, different from %class1 and %class2. "
	"Normally the empty string (\"\").")
TAG (U"%%Number 3")
DEFINITION (U"the number of selected objects of %class3.")
TAG (U"%%Command")
DEFINITION (U"the title of the new command button (or label, or submenu title). "
	"To get a separator line instead of a command text (only in a submenu), "
	"you specify a unique string that starts with a hyphen ('-'); the @ButtonEditor may contain some examples of this. "
	"If the command starts with \"Save as \", it will be placed in the @@Save menu@.")
TAG (U"%%After command")
DEFINITION (U"a button title in the dynamic menu or submenu where you want your new button. "
	"If you specify the empty string (\"\"), your button will be put at the bottom. "
	"You can specify a push button, a label (subheader), or a cascade button (submenu title) here.")
TAG (U"%%Depth")
DEFINITION (U"0 if you want your button in the main menu, 1 if you want it in a submenu.")
TAG (U"%%Script")
DEFINITION (U"the full path name of the script to invoke. If you saved the script you are editing, "
	"its name will already have been filled in here. "
	"If you do not specify a script, you will get a separating label or cascading menu title instead, "
	"depending on the %depth of the following command.")
ENTRY (U"Example")
NORMAL (U"If one object of class Sound is selected, you want a submenu called \"Filters\" "
	"after the #Convolve button, containing the commands \"Autocorrelation\" and \"Band filter...\", "
	"separated by a horizontal separator line:")
CODE (U"Add to dynamic menu: \"Sound\", 0, \"\", 0, \"\", 0, \"Filters -\", \"Convolve\", 0, \"\"")
CODE (U"Add to dynamic menu: \"Sound\", 1, \"\", 0, \"\", 0, \"Autocorrelation\", \"Filters -\", 1, \"/u/praats/demo/autocorrelation.praat\"")
CODE (U"Add to dynamic menu: \"Sound\", 0, \"\", 0, \"\", 0, \"-- band filter --\", \"Autocorrelation\", 1, \"\"")
CODE (U"Add to dynamic menu: \"Sound\", 1, \"\", 0, \"\", 0, \"Band filter...\", \"-- band filter --\", 1, \"/u/praats/demo/bandFilter.praat\"")
NORMAL (U"Note that \"Filters -\" will be a submenu title, %because it is followed by subcommands (%depth 1). "
	"Note that %number1 is 1 only for the executable buttons; for the cascade button and the separator line, "
	"this number is ignored.")
ENTRY (U"Usage convention")
NORMAL (U"Please adhere to the convention that command that take arguments, such as \"Band filter...\" above, end in three dots.")
ENTRY (U"Using this command in a script")
NORMAL (U"To add a dynamic button from a script (perhaps your @@initialization script@ or a @@plug-ins|plug-in@), "
	"use the hidden shell command @@Add action command...@ instead.")
MAN_END

MAN_BEGIN (U"Add to fixed menu...", U"ppgb", 20140107)
INTRO (U"A command in the #File menu of the @ScriptEditor.")
NORMAL (U"With this command, you add a button to any fixed menu in the @@Object window@ or in the @@Picture window@. "
	"Clicking the added button will invoke the specified @@Praat script@.")
ENTRY (U"Settings")
TAG (U"%Window")
DEFINITION (U"the name of the window (\"Objects\" or \"Picture\") that contains the menu that you want to change.")
TAG (U"%Menu")
DEFINITION (U"the title of the menu that you want to change. If %window is \"Objects\", you can specify "
	"the #Praat, #New, #Open, #Help, #Goodies, #Preferences, or #Technical menu (for the #Save menu, which depends on the objects selected, "
	"you would use @@Add to dynamic menu...@ instead). If %window is \"Picture\", you can specify "
	"the #File, #Edit, #Margins, #World, #Select, #Pen, #Font, or #Help menu.")
TAG (U"%Command")
DEFINITION (U"the title of the new menu button. To get a separator line instead of a command text, "
	"you specify a unique string that starts with a hyphen ('-'); the @ButtonEditor contains many examples of this.")
TAG (U"%%After command")
DEFINITION (U"a button title in the menu or submenu after which you want your new button to be inserted. "
	"If you specify the empty string (\"\"), your button will be put in the main menu.")
TAG (U"%Depth")
DEFINITION (U"0 if you want your button in the main menu, 1 if you want it in a submenu.")
TAG (U"%Script")
DEFINITION (U"the full path name of the script to invoke. If you saved the script you are editing, "
	"its name will already have been filled in here. If you do not specify a script, "
	"you will get a cascading menu title instead.")
ENTRY (U"Example 1")
NORMAL (U"In the #Matrix submenu of the @@New menu@, you want a separator line followed by the command \"Peaks\":")
CODE (U"Add to fixed menu: \"Objects\", \"New\", \"-- peaks --\", \"Create simple Matrix...\", 1, \"\"")
CODE (U"Add to fixed menu: \"Objects\", \"New\", \"Peaks\", \"-- peaks --\", 1, \"/u/praats/demo/peaks.praat\"")
ENTRY (U"Example 2")
NORMAL (U"In the @@New menu@, you want a submenu called \"Demo\", with a subitem titled \"Lorenz...\":")
CODE (U"Add to fixed menu: \"Objects\", \"New\", \"Demo\", \"\", 0,, \"\"")
CODE (U"Add to fixed menu: \"Objects\", \"New\", \"Lorenz...\", \"Demo\", 1, \"/u/praats/demo/lorentz.praat\"")
ENTRY (U"Usage convention")
NORMAL (U"Please adhere to the convention that command that take arguments, such as \"Lorenz...\" above, end in three dots.")
ENTRY (U"Using this command in a script")
NORMAL (U"To add a fixed button from a script (perhaps your @@initialization script@ or a @@plug-ins|plug-in@), "
	"use the hidden shell command @@Add menu command...@ instead.")
MAN_END

MAN_BEGIN (U"binomialQ", U"ppgb", 20140223)
INTRO (U"A function that can be used in @@Formulas@. The complement of the cumulative binomial distribution.")
ENTRY (U"Syntax")
TAG (U"$$binomialQ (%p, %k, %n)")
DEFINITION (U"the probability that in %n trials an event with probability %p will occur at least %k times.")
ENTRY (U"Calculator example")
NORMAL (U"A die is suspected to yield more sixes than a perfect die would do. In order to test this suspicion, "
	"you throw it 1,000 times. The result is 211 sixes.")
NORMAL (U"The probability that a perfect die yields at least 211 sixes is, according to @@Calculator...@, "
	"$$binomialQ (1/6, 211, 1000)$ = 0.000152.")
ENTRY (U"Script example")
NORMAL (U"You convert 1000 values of pitch targets in Hz to the nearest note on the piano keyboard. "
	"597 of those values turn out to be in the A, B, C, D, E, F, or G regions (the white keys), and 403 values turn out "
	"to be in the A\\# , C\\# , D\\# , F\\# , or G\\#  regions (the black keys). "
	"Do our subjects have a preference for the white keys? "
	"The following script computes the probability that in the case of no preference the subjects "
	"would target the white keys at least 597 times. This is compared with a %\\ci^2 test.")
CODE (U"a = 597")
CODE (U"b = 403")
CODE (U"p = 7/12 ; no preference")
CODE (U"writeInfoLine: \"*** Binomial test \", a, \", \", b, \", p = \", fixed\\$  (p, 6), \" ***\"")
CODE (U"pbin = binomialQ (p, a, a+b)")
CODE (U"appendInfoLine: \"P (binomial) = \", fixed\\$  (pbin, 6)")
CODE (U"\\#  Chi-square test with Yates correction:")
CODE (U"x2 = (a - 1/2 - p * (a+b))\\^ 2/(p*(a+b)) + (b + 1/2 - (1-p) * (a+b))\\^ 2/((1-p)*(a+b))")
CODE (U"px2 = chiSquareQ (x2, 1)")
CODE (U"appendInfoLine: \"P (chi-square) = \", fixed\\$  (px2, 6)")
NORMAL (U"The result is:")
CODE (U"*** Binomial test 597, 403, p = 0.583333 ***")
CODE (U"P (binomial) = 0.199330")
CODE (U"P (chi-square) = 0.398365")
NORMAL (U"The %\\ci^2 test is two-sided (it signals a preference for the white or for the black keys), "
	"so it has twice the probability of the binomial test.")
NORMAL (U"We cannot conclude from this test that people have a preference for the white keys. "
	"Of course, we cannot conclude either that people do not have such a preference.")
MAN_END

MAN_BEGIN (U"ButtonEditor", U"ppgb", 20060920)
INTRO (U"An editor for viewing, hiding, showing, removing, and executing the commands "
	"in the fixed and dynamic menus of the Praat program. To open it, choose ##Buttons...# "
	"from the #Praat menu of the @@Object window@.")
ENTRY (U"What the button editor shows")
NORMAL (U"The button editor gives a list of:")
LIST_ITEM (U"1. The five fixed buttons.")
LIST_ITEM (U"2. The built-in and added @@fixed menu commands@, lexicographically sorted by window and menu name.")
LIST_ITEM (U"3. The built-in and added @@action commands@, sorted by the names of the selected objects.")
ENTRY (U"Visibility of built-in commands")
NORMAL (U"Most built-in commands are visible by default, but some are hidden by default (see @@Hidden commands@). "
	"The button editor shows these commands as \"shown\" or \"hidden\", respectively. "
	"You can change the visibility of a command by clicking on the blue \"shown\" or \"hidden\" text; "
	"this text will then be replaced with \"HIDDEN\" or \"SHOWN\", with capitals to signal their non-standard settings. "
	"These changes will be remembered in the @@buttons file@ across sessions of your program. "
	"To return to the standard settings, click the blue \"HIDDEN\" or \"SHOWN\" texts again.")
NORMAL (U"Some built-in commands cannot be hidden. They are marked as \"unhidable\". "
	"The most notable example is the ##Buttons...# button "
	"(a failure to make the ##Commands...# command unhidable in Microsoft Word "
	"causes some computer viruses to be very hard to remove...).")
ENTRY (U"Added commands")
NORMAL (U"Commands that you have added to the fixed or dynamic menus (probably with @@Add to fixed menu...@ or "
	"@@Add to dynamic menu...@ in the @ScriptEditor), "
	"are marked as \"ADDED\". They are remembered in the @@buttons file@. "
	"You can change the availability of these commands by clicking on the blue \"ADDED\" text, which will then "
	"be replaced with \"REMOVED\". After this, the added command will no longer be remembered in the @@buttons file@. "
	"To make the command available again, click the blue \"REMOVED\" text again, before leaving the program.")
ENTRY (U"Start-up commands")
NORMAL (U"Commands that were added in an @@initialization script@ or @@plug-ins|plug-in@ (with @@Add menu command...@ or "
	"@@Add action command...@) are marked as \"START-UP\". "
	"They are %not remembered in the @@buttons file@. "
	"You can change the visibility of these commands by clicking on the blue \"START-UP\" text, which will then "
	"be replaced with \"HIDDEN\". This setting %will be remembered in the @@buttons file@. "
	"To make the command visible again, click the blue \"HIDDEN\" text again.")
ENTRY (U"Executing commands")
NORMAL (U"The button editor allows you to choose hidden commands without first making them visible in the fixed or dynamic menus.")
NORMAL (U"The editor shows all the executable commands in blue. These include:")
LIST_ITEM (U"1. The fixed #Remove button, if one or more objects are selected in the @@List of Objects@.")
LIST_ITEM (U"2. The other fixed buttons, if exactly one object is selected.")
LIST_ITEM (U"3. All of the fixed menu commands, hidden or not, and \"removed\" or not.")
LIST_ITEM (U"4. Those action commands that match the currently selected objects with respect to class and number.")
NORMAL (U"To execute any of these blue commands, just click on it.")
MAN_END

MAN_BEGIN (U"buttons file", U"ppgb", 20151020)
NORMAL (U"The file into which changes in the availability and visibility of commands in the fixed "
	"and dynamic menus are recorded.")
NORMAL (U"The buttons file is written to disk when you quit Praat, "
	"and it is read again when you start Praat the next time. It is a simple @@Praat script@ that you can read "
	"(but should not edit) with any text editor.")
ENTRY (U"Adding buttons")
NORMAL (U"To add a command to a fixed or dynamic menu, you typically use the @ScriptEditor.")
ENTRY (U"Removing buttons")
NORMAL (U"To remove an added command from a fixed or dynamic menu, you typically use the @ButtonEditor.")
ENTRY (U"Hiding and showing buttons")
NORMAL (U"To hide a built-in command from a fixed or dynamic menu, or to make a hidden command visible, "
	"you typically use the @ButtonEditor.")
ENTRY (U"Where is the buttons file?")
NORMAL (U"The buttons file is in your Praat @@preferences directory@.")
NORMAL (U"On Windows the file is called ##Buttons5.ini#, "
	"for instance ##C:\\bsUsers\\bsMiep\\bsPraat\\bsButtons5.ini#.")
NORMAL (U"On MacOS it is called #Buttons5, "
	"for instance ##/Users/miep/Library/Preferences/Praat Prefs/Buttons5#.")
NORMAL (U"On Linux it is called #buttons5, "
	"for instance ##/home/miep/.praat-dir/buttons5#.")
MAN_END

MAN_BEGIN (U"Calculator", U"ppgb", 20021201)
INTRO (U"A window that allows you to calculate all kinds of simple or complicated mathematical and string expressions. "
	"To show the calculator, type Command-U or choose the @@Calculator...@ command. "
	"The result will be written to the Info window.")
NORMAL (U"See the @Formulas tutorial for all the things that you can calculate with this command.")
MAN_END

MAN_BEGIN (U"Calculator...", U"ppgb", 20050822)
INTRO (U"A command in the @Goodies submenu of the @@Praat menu@ of the @@Object window@. Shortcut: Command-U. "
	"Choosing this command brings up Praat's @calculator.")
MAN_END

MAN_BEGIN (U"Clear history", U"ppgb", 20000927)
INTRO (U"A command in the #Edit menu of the @ScriptEditor for clearing the remembered history. "
	"See @@History mechanism@.")
MAN_END

MAN_BEGIN (U"differenceLimensToPhon", U"ppgb", 20021215)
INTRO (U"A routine for converting intensity difference limens into sensation level, "
	"the inverse of @phonToDifferenceLimens.")
ENTRY (U"Formula")
FORMULA (U"differenceLimensToPhon (%ndli) = ln (1 + %ndli / 30) / ln (61 / 60)")
MAN_END

MAN_BEGIN (U"Fixed menu commands", U"ppgb", 20120915)
INTRO (U"The commands in the fixed menus of the @@Object window@ (#Praat, #New, #Open, #Help, #Goodies, "
	"#Preferences, and #Technical) and the @@Picture window@ (#File, #Edit, #Margins, #World, #Select, #Pen, #Font, #Help).")
NORMAL (U"These commands are always clickable (if not hidden) and scriptable (if not added).")
MAN_END

MAN_BEGIN (U"Formulas", U"ppgb", 20040414)
INTRO (U"You can use numeric expressions and string (text) expressions in many places in Praat:")
LIST_ITEM (U"\\bu in the @calculator in Praat's @Goodies submenu;")
LIST_ITEM (U"\\bu in the numeric fields of most settings windows;")
LIST_ITEM (U"\\bu in a @@Praat script@.")
NORMAL (U"For some types of objects (mainly Sound and Matrix), you can also apply formulas to all their contents at the same time:")
LIST_ITEM (U"\\bu when you create a Sound or a Matrix from the @@New menu@;")
LIST_ITEM (U"\\bu when you choose @@Formula...@ from the @Modify menu for a selected object.")
NORMAL (U"You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM (U"1. @@Formulas 1. My first formulas|My first formulas@ (where to use)")
LIST_ITEM1 (U"1.1. @@Formulas 1.1. Formulas in the calculator|Formulas in the calculator@")
LIST_ITEM1 (U"1.2. @@Formulas 1.2. Numeric expressions|Numeric expressions@")
LIST_ITEM1 (U"1.3. @@Formulas 1.3. String expressions|String expressions@")
LIST_ITEM1 (U"1.4. @@Formulas 1.4. Representation of numbers|Representation of numbers@")
LIST_ITEM1 (U"1.5. @@Formulas 1.5. Representation of strings|Representation of strings@")
LIST_ITEM1 (U"1.6. @@Formulas 1.6. Formulas in settings windows|Formulas in settings windows@")
LIST_ITEM1 (U"1.7. @@Formulas 1.7. Formulas for creation|Formulas for creation@")
LIST_ITEM1 (U"1.8. @@Formulas 1.8. Formulas for modification|Formulas for modification@")
LIST_ITEM1 (U"1.9. @@Formulas 1.9. Formulas in scripts|Formulas in scripts@")
LIST_ITEM (U"2. @@Formulas 2. Operators|Operators@ (+, -, *, /, \\^ )")
LIST_ITEM (U"3. @@Formulas 3. Constants|Constants@ (pi, e, undefined)")
LIST_ITEM (U"4. @@Formulas 4. Mathematical functions|Mathematical functions@")
LIST_ITEM (U"5. @@Formulas 5. String functions|String functions@")
LIST_ITEM (U"6. @@Formulas 6. Control structures|Control structures@ (if then else fi, semicolon)")
LIST_ITEM (U"7. @@Formulas 7. Attributes of objects|Attributes of objects@")
LIST_ITEM (U"8. @@Formulas 8. Data in objects|Data in objects@")
MAN_END

MAN_BEGIN (U"Formulas 1. My first formulas", U"ppgb", 20040414)
LIST_ITEM (U"1.1. @@Formulas 1.1. Formulas in the calculator|Formulas in the calculator@")
LIST_ITEM (U"1.2. @@Formulas 1.2. Numeric expressions|Numeric expressions@")
LIST_ITEM (U"1.3. @@Formulas 1.3. String expressions|String expressions@")
LIST_ITEM (U"1.4. @@Formulas 1.4. Representation of numbers|Representation of numbers@")
LIST_ITEM (U"1.5. @@Formulas 1.5. Representation of strings|Representation of strings@")
LIST_ITEM (U"1.6. @@Formulas 1.6. Formulas in settings windows|Formulas in settings windows@")
LIST_ITEM (U"1.7. @@Formulas 1.7. Formulas for creation|Formulas for creation@")
LIST_ITEM (U"1.8. @@Formulas 1.8. Formulas for modification|Formulas for modification@")
LIST_ITEM (U"1.9. @@Formulas 1.9. Formulas in scripts|Formulas in scripts@")
MAN_END

MAN_BEGIN (U"Formulas 1.1. Formulas in the calculator", U"ppgb", 20050822)
INTRO (U"To use the Praat @calculator, go to the @@Praat menu@ "
	"and choose @@Calculator...@ from the @Goodies submenu. Or simply type Command-U anywhere in Praat.")
ENTRY (U"Calculating numbers")
NORMAL (U"You can do arithmetic computations. Type the formula")
CODE (U"8*17")
NORMAL (U"and click OK. The Info window will pop up and show the result:")
CODE (U"136")
ENTRY (U"Calculating strings")
NORMAL (U"You can also do text computations. Type the formula")
CODE (U"\"see\" + \"king\"")
NORMAL (U"and click OK. The Info window will show the result:")
CODE (U"seeking")
MAN_END

MAN_BEGIN (U"Formulas 1.2. Numeric expressions", U"ppgb", 20050901)
INTRO (U"All the formulas whose outcome is a number are called numeric expressions. "
	"For the following examples, all the outcomes can be checked with the @calculator.")
ENTRY (U"Examples with numbers")
NORMAL (U"Some numeric expressions involve numbers only:")
TAG (U"##8*17")
DEFINITION (U"computes a multiplication. Outcome: 136.")
TAG (U"##2\\^ 10")
DEFINITION (U"computes the tenth power of 2. Outcome: 1024.")
TAG (U"##sqrt (2) / 2")
DEFINITION (U"computes the square root of 2, and divides the result by 2. Outcome: 0.7071067811865476.")
TAG (U"##sin (1/4 * pi)")
DEFINITION (U"computes the sine of %\\pi/4. Outcome: 0.7071067811865476 (again).")
ENTRY (U"Examples with strings")
NORMAL (U"Some numeric expressions compute numeric properties of strings:")
TAG (U"##length (\"internationalization\")")
DEFINITION (U"computes the length of the string \"internationalization\". Outcome: 20.")
TAG (U"##index (\"internationalization\", \"ation\")")
DEFINITION (U"computes the location of the first occurrence of the string \"ation\" in the string \"internationalization\". Outcome: 7, "
	"because the first letter of \"ation\" lines up with the seventh letter of \"internationalization\". "
	"If the substring does not occur, the outcome is 0.")
MAN_END

MAN_BEGIN (U"Formulas 1.3. String expressions", U"ppgb", 20040414)
INTRO (U"All the formulas whose outcome is a text string are called string expressions. "
	"Again, the outcomes of the following examples can be checked with the @calculator.")
TAG (U"##\"see\" + \"king\"")
DEFINITION (U"concatenates two strings. Outcome: seeking.")
TAG (U"##left\\$  (\"internationalization\", 6)")
DEFINITION (U"computes the leftmost six letters of the string; the dollar sign is used for all functions whose result is a string. Outcome: intern.")
TAG (U"##mid\\$  (\"internationalization\", 6, 8)")
DEFINITION (U"computes the 8-letter substring that starts at the sixth letter of \"internationalization\". Outcome: national.")
TAG (U"##date\\$  ( )")
DEFINITION (U"computes the current date and time. Outcome at the time I am writing this: Mon Dec  2 02:23:45 2002.")
MAN_END

MAN_BEGIN (U"Formulas 1.4. Representation of numbers", U"ppgb", 20040414)
INTRO (U"Formulas can work with integer numbers as well as with real numbers.")
ENTRY (U"Real numbers")
NORMAL (U"You can type many real numbers by using a decimal notation, for instance 3.14159, 299792.5, or -0.000123456789. "
	"For very large or small numbers, you can use the %e-notation: 6.022\\.c10^^23^ is typed as 6.022e23 or 6.022e+23, "
	"and -1.6021917\\.c10^^-19^ is typed as -1.6021917e-19. You can use also use the percent notation: 0.157 "
	"can be typed as 15.7\\% .")
NORMAL (U"There are some limitations as to the values that real numbers can have in Praat. "
	"The numbers must lie between -10^^308^ and +10^^308^. If you type")
CODE (U"1e200 * 1e100")
NORMAL (U"the outcome will be")
CODE (U"1e+300")
NORMAL (U"but if you type")
CODE (U"1e300 * 1e100")
NORMAL (U"the outcome will be")
CODE (U"--undefined--")
NORMAL (U"Another limitation is that the smallest non-zero numbers lie near -10^^-308^ and +10^^-308^. If you type")
CODE (U"1e-200 / 1e100")
NORMAL (U"the outcome will be")
CODE (U"1e-300")
NORMAL (U"but if you type")
CODE (U"1e-300 / 1e100")
NORMAL (U"the outcome will be")
CODE (U"0")
NORMAL (U"Finally, the precision of real numbers is limited by the number of bits that every real number is stored with "
	"in the computer, namely 64. For instance, if you type")
CODE (U"pi")
NORMAL (U"the outcome will be")
CODE (U"3.141592653589793")
NORMAL (U"because only 16 digits of precision are stored. This can lead to unexpected results caused by rounding. "
	"For instance, the formula")
CODE (U"0.34999999999999999 - 0.35")
NORMAL (U"will result in")
CODE (U"0")
NORMAL (U"rather than the correct value of 1e-17. This is because the numbers 0.34999999999999999 and 0.35 cannot "
	"be distinguished in the computer's memory. If you simply type")
CODE (U"0.34999999999999999")
NORMAL (U"the outcome will be")
CODE (U"0.35")
NORMAL (U"(as in this example, the calculator will always come up with the minimum number of digits needed to represent the number unambiguously).")
NORMAL (U"Another example of inaccuracy is the formula")
CODE (U"1 / 7 / 59 * 413")
NORMAL (U"Because of rounding errors, the result will be")
CODE (U"0.9999999999999999")
ENTRY (U"Integer numbers")
NORMAL (U"Formulas can work with integer (whole) numbers between -1,000,000,000,000,000 and +1,000,000,000,000,000. "
	"You type them without commas and without the plus sign: 337, -848947328345289.")
NORMAL (U"You %can work with larger numbers than that (up to 10^^308^), but there will again be rounding errors. "
	"For instance, the formula")
CODE (U"1000000000000000 + 1")
NORMAL (U"correctly yields")
CODE (U"1000000000000001")
NORMAL (U"but the formula")
CODE (U"10000000000000000 + 1")
NORMAL (U"yields an incorrect outcome:")
CODE (U"1e16")
MAN_END

MAN_BEGIN (U"Formulas 1.5. Representation of strings", U"ppgb", 20021203)
INTRO (U"Formulas can work with strings that are put between two double quotes, "
	"as in \"goodbye\" or \"how are you doing?\".")
NORMAL (U"If a string has to contain a double quote, "
	"you have to type it twice. For instance, if you type")
CODE (U"\"I asked: \\\" \\\" how are you doing?\\\" \\\" \"")
NORMAL (U"into the calculator, the outcome will be")
CODE (U"I asked: \"how are you doing?\"")
MAN_END

MAN_BEGIN (U"Formulas 1.6. Formulas in settings windows", U"ppgb", 20070225)
INTRO (U"Into numeric fields in settings windows you usually simply type a number. "
	"However, you can use any numeric expression instead.")
NORMAL (U"For instance, suppose you want to create a Sound that contains exactly 10000 samples. "
	"If the sampling frequency is 44100 Hz, the duration will be 10000/44100 seconds. "
	"You can create such a Sound by choosing @@Create Sound from formula...@ from the @@New menu@, "
	"then typing")
CODE (U"10000/44100")
NORMAL (U"into the ##End time# field.")
NORMAL (U"Into text fields in settings windows, you can only type text directly; there is no way "
	"to use string expressions (except if you use scripts; see @@Formulas 1.9. Formulas in scripts@).")
MAN_END

MAN_BEGIN (U"Formulas 1.7. Formulas for creation", U"ppgb", 20110128)
INTRO (U"With some commands in the @@New menu@, you can supply a formula that Praat will apply to all elements of the new object.")
ENTRY (U"Creating a Sound from a formula")
NORMAL (U"Choose @@Create Sound from formula...@ and type the following into the #%Formula field:")
CODE (U"1/2 * sin (2 * pi * 377 * x)")
NORMAL (U"When you click OK, a new @Sound object will appear in the list. "
	"After you click ##View & Edit# and zoom in a couple of times, you will see that the sound is a sine wave "
	"with a frequency of 377 hertz (cycles per second). This worked because the %x in the formula represents the time, "
	"i.e. the formula was applied to every sample separately, with a different value of %x for each sample.")
ENTRY (U"Creating a Matrix from a formula")
NORMAL (U"Choose @@Create simple Matrix...@ and type the following into the #%Formula field:")
CODE (U"8")
NORMAL (U"When you click OK, a new @Matrix object will appear in the list. When you click #Info, "
	"you will see that it is a matrix with 10 rows and 10 columns, and that all the 100 cells contain the value 8 "
	"(you can see this because both the minimum and the maximum are reported as being 8).")
NORMAL (U"A more interesting example is the formula")
CODE (U"row * col")
NORMAL (U"For the resulting Matrix, "
	"choose @@Matrix: Paint cells...|Paint cells...@ and click OK. The Picture window will show a 10\\xx10 "
	"matrix whose elements are the product of the row and column numbers, i.e., they have values between "
	"1 and 100. Beside %row and %col, you can use %x for the distance along the horizontal axis and %y for the "
	"distance along the vertical axis; see the following page for examples.")
MAN_END

MAN_BEGIN (U"Formulas 1.8. Formulas for modification", U"ppgb", 20021204)
INTRO (U"Analogously to the formulas that you can use for creating new objects (see the previous page), "
	"you can use formulas for modifying existing objects. You do this with the command ##Formula...# that you "
	"can find in the @Modify menu when you select an object.")
ENTRY (U"Modifying a Sound with a formula")
NORMAL (U"Record a sound with your microphone and talk very lowly. If you don't know how to record a sound in Praat, "
	"consult the @Intro. Once the Sound object is in the list, click #%Play. The result will sound very soft. "
	"Then choose ##Formula...# from the #Modify menu and type")
CODE (U"self * 3")
NORMAL (U"Click OK, then click #Play again. The sound is much louder now. You have multiplied the amplitude of every sample "
	"in the sound with a factor of 3.")
ENTRY (U"Replacing the contents of a Sound with a formula")
NORMAL (U"If you don't use $self in your formula, the formula does not refer to the existing contents of the Sound. Hence, the formula ")
CODE (U"1/2 * sin (2 * pi * 377 * x)")
NORMAL (U"will simply replace your recorded speech with a 377-Hz sine wave.")
ENTRY (U"Modifying a Matrix with a formula")
NORMAL (U"Many objects can be thought of as matrices: "
	"they consist of a number of rows and columns with data in every cell:")
LIST_ITEM (U"@Sound: one row; columns represent samples.")
LIST_ITEM (U"@Spectrum: two rows (first row is real part, second row is imaginary part); "
	"columns represent frequencies.")
LIST_ITEM (U"@Spectrogram, @Cochleagram: rows represent frequencies; columns represent times.")
LIST_ITEM (U"@Excitation: one row; columns represent frequency bands.")
LIST_ITEM (U"@Harmonicity: one row; columns represent time frames.")
NORMAL (U"The formula is performed on every column of every row. The formula")
CODE (U"self\\^ 2$")
NORMAL (U"will square all matrix elements.")
NORMAL (U"The formula first works on the first row, and in that row from the first column on; "
	"this can work recursively. The formula")
CODE (U"self + self [row, col - 1]")
NORMAL (U"integrates each row.")
ENTRY (U"Referring to the current position in the object")
NORMAL (U"You can refer to the current position in a Matrix (or Sound, etc.) by index or by %x and %y values:")
TAG (U"#row")
DEFINITION (U"the current row")
TAG (U"#col")
DEFINITION (U"the current column")
TAG (U"#x")
DEFINITION (U"the %x value associated with the current column:")
DEFINITION (U"    for a Sound, Spectrogram, Cochleagram, or Harmonicity: time")
DEFINITION (U"    for a Spectrum: frequency (Hz)")
DEFINITION (U"    for an Excitation: frequency (Bark)")
TAG (U"#y")
DEFINITION (U"the %y value associated with the current row:")
DEFINITION (U"    for a Spectrogram: frequency (Hz)")
DEFINITION (U"    for a Cochleagram: frequency (Bark)")
ENTRY (U"Referring to the contents of the object itself")
NORMAL (U"You can refer to values in the current Matrix (or Sound, etc.) by index.")
TAG (U"##self")
DEFINITION (U"refers to the value in the current Matrix at the current row and column, or to the value in the current Sound at the current sample.")
TAG (U"##self [%%column-expression%]")
DEFINITION (U"refers to the value in the current Sound (or Intensity etc.) at the current sample (or frame). "
	"The %%column-expression% is rounded to the nearest integer. "
	"If the index is out of range (less than 1 or greater than %n__%x_), the expression evaluates as 0.")
NORMAL (U"#Example. An integrator is")
CODE (U"   self [col - 1] + self * dx")
TAG (U"##self [%%row-expression%, %%column-expression%]")
DEFINITION (U"refers to the value in the current Matrix (or Spectrogram etc.) at the specified row and column. "
	"The expressions are rounded to the nearest integers.")
NORMAL (U"You can refer to values in the current Matrix (or Spectrogram, etc.) by %x and %y position:")
TAG (U"\\bu ##self (%%x-expression%, %%y-expression%)")
DEFINITION (U"the expressions are linearly interpolated between the four nearest matrix points.")
NORMAL (U"You can refer to values in the current Sound (or Intensity etc.) by %x position:")
TAG (U"\\bu ##self (%%x-expression%)")
DEFINITION (U"the expression is linearly interpolated between the two nearest samples (or frames).")
MAN_END

MAN_BEGIN (U"Formulas 1.9. Formulas in scripts", U"ppgb", 20140223)
INTRO (U"In scripts, you can assign numeric expressions to numeric variables, "
	"and string expressions to string variables. You can also use numeric and string variables in expressions.")
ENTRY (U"Example: report a square")
NORMAL (U"Choose @@New Praat script@ from the @@Praat menu@. A script editor window will become visible. "
	"Type the following lines into that window:")
CODE (U"x = 99")
CODE (U"x2 = x * x")
CODE (U"writeInfoLine: \"The square of \", x, \" is \", x2, \".\"")
NORMAL (U"This is an example of a simple @@Praat script@; it assigns the results of the numeric formulas $$99$ and $$x * x$ "
	"to the numeric variables %x and %x2. Note that the formula $$x * x$ itself refers to the variable %x. "
	"To run (execute) this script, type Command-R or choose #Run from the #Run menu. "
	"Praat will then write the following text into the Info window:")
CODE (U"The square of 99 is 9801.")
NORMAL (U"For more information on scripts, see the @Scripting tutorial.")
ENTRY (U"Example: rename the city of Washington")
NORMAL (U"Type the following text into the script editor window:")
CODE (U"current\\$  = \"Bush\"")
CODE (U"previous\\$  = \"Clinton\"")
CODE (U"famous\\$  = \"Lincoln\"")
CODE (U"newCapital\\$  = current\\$  + mid\\$  (famous\\$ , 2, 3) + right\\$  (previous\\$ , 3)")
CODE (U"writeInfoLine: \"The new capital will be \", newCapital\\$ , \".\"")
NORMAL (U"This script assigns the results of four string expressions to the four string variables %%current\\$ %, "
	"%%previous\\$ %, %%famous\\$ %, and %%newCapital\\$ %. The dollar sign is the notation for a string variable or "
	"for a function whose result is a string (like ##left\\$ #). Note that the formula in the fourth line refers to three existing "
	"variables.")
NORMAL (U"To see what the new name of the capital will be, choose #Run.")
ENTRY (U"Example: numeric expressions in settings in scripts")
NORMAL (U"As in real settings windows, you can use numeric expressions in all numeric fields. "
	"The example of two pages back becomes:")
CODE (U"Create Sound from formula: \"sine\", \"Mono\", 0, 10000 / 44100, 44100, \"0.9 * sin (2*pi*377*x)\"")
ENTRY (U"Example: string expressions in settings in scripts")
NORMAL (U"As in real settings windows, you can use string expressions in all text fields:")
CODE (U"soundName\\$  = \"hello\"")
CODE (U"Read from file: soundName\\$  + \".wav\"")
ENTRY (U"Example: numeric expressions in creation in scripts")
NORMAL (U"Suppose you want to generate a sine wave whose frequency is held in a variable. This is the way:")
CODE (U"frequency = 377")
CODE (U"Create Sound from formula: \"sine\", \"Mono\", 0, 1, 44100, \"0.9 * sin (2*pi*frequency*x)\"")
NORMAL (U"In this example, Praat will protest if %x is a variable as well, because that would be ambiguous "
	"with the %x that refers to the time in the sound (see @@Formulas 1.8. Formulas for modification@).")
MAN_END

MAN_BEGIN (U"Formulas 2. Operators", U"ppgb", 20060127)
NORMAL (U"In formulas you can use the numerical and logical operators that are described on this page. "
	"The order of evaluation of the operators is the order that is most usual in programming languages. "
	"To force a different order, you use parentheses.")
NORMAL (U"The operators with the highest precedence are #negation (-) and #exponentation (\\^ ):")
CODE (U"--6 \\-> 6")
CODE (U"2\\^ 6 \\-> 64")
NORMAL (U"Sequences of negation and exponentiation are evaluated from right to left:")
CODE (U"2\\^ -6 \\-> 0.015625")
CODE (U"-(1+1)\\^ 6 \\-> -64")
CODE (U"4\\^ 3\\^ 2 \\-> 4\\^ 9 \\-> 262144")
NORMAL (U"Note that changing the spacing does not change the meaning:")
CODE (U"4\\^ 3 \\^  2 \\-> 262144")
NORMAL (U"To change the order of evaluation, you have to use parentheses:")
CODE (U"(4 \\^  3) \\^  2 \\-> 4096")
NORMAL (U"The following construction is not allowed because of an ambiguity between a negative number "
	"and negation of a positive number:")
CODE (U"-2\\^ 6 \\-> ?")
NORMAL (U"Instead, you use any of the following:")
CODE (U"(-2)\\^ 6 \\-> 64")
CODE (U"-(2\\^ 6) \\-> -64")
CODE (U"-(2)\\^ 6 \\-> -64")
NORMAL (U"The operators with the next highest precedence are #multiplication (*) and #division (/). They are evaluated "
	"from left to right:")
CODE (U"1/4*5 \\-> 1.25        (from left to right)")
CODE (U"1 / 4*5 \\-> 1.25      (spacing does not help)")
CODE (U"1 / (4*5) \\-> 0.05    (use parentheses to change the order)")
CODE (U"3 * 2 \\^  4 \\-> 48      (exponentiation before multiplication)")
CODE (U"3*2 \\^  4 \\-> 48        (this spacing does not matter and is misleading)")
CODE (U"(3 * 2) \\^  4 \\-> 1296  (use parentheses to change the order)")
NORMAL (U"##Integer division# operators (#div and #mod) have the same precedence as * and /, "
	"and are likewise evaluated from left to right:")
CODE (U"54 div 5 \\-> 10       (division rounded down)")
CODE (U"54 mod 5 \\-> 4        (the remainder)")
CODE (U"54.3 div 5.1 \\-> 10   (works for real numbers as well)")
CODE (U"54.3 mod 5.1 \\-> 3.3  (the remainder)")
CODE (U"-54 div 5 \\-> -11     (division rounded down; negation before division)")
CODE (U"-54 mod 5 \\-> 1       (the remainder)")
CODE (U"-(54 div 5) \\-> -10   (use parentheses to change the order)")
CODE (U"-(54 mod 5) \\-> -4")
CODE (U"3 * 18 div 5 \\-> 10   (from left to right)")
CODE (U"3 * (18 div 5) \\-> 9")
CODE (U"3 * 18 mod 5 \\-> 4")
CODE (U"3 * (18 mod 5) \\-> 9")
CODE (U"54 div 5 * 3 \\-> 30   (from left to right)")
CODE (U"54 div (5 * 3) \\-> 3")
CODE (U"54 mod 5 * 3 \\-> 12")
CODE (U"54 mod (5 * 3) \\-> 9")
NORMAL (U"The operators with the next highest precedence are #addition (+) and #subtraction (-), "
	"evaluated from left to right:")
CODE (U"3 - 8 + 7 \\-> 2       (from left to right)")
CODE (U"3 - (8 + 7) \\-> -12   (use parentheses to change the order)")
CODE (U"3 + 8 * 7 \\-> 59      (multiplication before addition)")
CODE (U"(3 + 8) * 7 \\-> 77    (use parentheses to change the order)")
CODE (U"3 + - (2 \\^  4) \\-> -13   (exponentiation, negation, addition)")
CODE (U"3 + 5 / 2 + 3 \\-> 8.5")
CODE (U"(3 + 5) / (2 + 3) \\-> 1.6")
NORMAL (U"The operators with the next highest precedence are the #comparison operators "
	"(=  <>  <  >  <=  >=). These operators always yield 0 (%false) or 1 (%true):")
CODE (U"5 + 6 = 10 \\-> 0      (equal)")
CODE (U"5 + 6 = 11 \\-> 1")
CODE (U"5 + 6 <> 10 \\-> 1     (unequal)")
CODE (U"5 + 6 <> 11 \\-> 0")
CODE (U"5 + 6 < 10 \\-> 0      (less than)")
CODE (U"5 + 6 < 11 \\-> 0")
CODE (U"5 + 6 > 10 \\-> 1      (greater than)")
CODE (U"5 + 6 > 11 \\-> 0")
CODE (U"5 + 6 <= 10 \\-> 0     (less than or equal)")
CODE (U"5 + 6 <= 11 \\-> 1")
CODE (U"5 + 6 >= 10 \\-> 1     (greater or equal)")
CODE (U"5 + 6 >= 11 \\-> 1")
NORMAL (U"The comparison operators are mainly used in #if, #while, and #until conditions.")
NORMAL (U"The operators of lowest precedence are the #logical operators (#not, #and, and #or), of which #not has the "
	"highest precedence and #or the lowest:")
CODE (U"not 5 + 6 = 10 \\-> 1")
CODE (U"x > 5 and x < 10               (is x between 5 and 10?)")
CODE (U"not x <= 5 and not x >= 10     (same as previous line)")
CODE (U"not (x <= 5 or x >= 10)        (same as previous line)")
ENTRY (U"String comparison")
TAG (U"##a\\$  = b\\$ ")
DEFINITION (U"gives the value %true (= 1) if the strings are equal, and %false (= 0) otherwise.")
TAG (U"##a\\$  <> b\\$ ")
DEFINITION (U"gives the value %true if the strings are unequal, and %false otherwise.")
TAG (U"##a\\$  < b\\$ ")
DEFINITION (U"gives %true if the string %%a\\$ % precedes the string %%b\\$ % in ASCII sorting order. "
	"Thus, \"ha\" < \"hal\" and \"ha\" < \"ja\" are true, but \"ha\" < \"JA\" is false, "
	"because all capitals precede all lower-case characters in the ASCII sorting order.")
TAG (U"##a\\$  > b\\$ ")
DEFINITION (U"%true if %%a\\$ % comes after %%b\\$ % in ASCII sorting order. ")
TAG (U"##a\\$  <= b\\$ ")
DEFINITION (U"gives the value %true if the string %%a\\$ % precedes the string %%b\\$ % in ASCII sorting order, "
	"or if the strings are equal.")
TAG (U"##a\\$  >= b\\$ ")
DEFINITION (U"%true if %%a\\$ % comes after %%b\\$ % or the two are equal.")
ENTRY (U"String concatenation and truncation")
TAG (U"##a\\$  + b\\$ ")
DEFINITION (U"concatenates the two strings. After")
CODE1 (U"text\\$  = \"hallo\" + \"dag\"")
DEFINITION (U"The variable %%text\\$ % contains the string \"hallodag\".")
TAG (U"##a\\$  - b\\$ ")
DEFINITION (U"subtracts the second string from the end of the first. After")
CODE2 (U"soundFileName\\$  = \"hallo.aifc\"")
CODE2 (U"textgridFileName\\$  = soundFileName\\$  - \".aifc\" + \".TextGrid\"")
DEFINITION (U"the variable %%textgridFileName\\$ % contains the string \"hallo.TextGrid\". "
	"If the first string %%a\\$ % does not end in the string %%b\\$ %, the result of the subtraction is the string %%a\\$ %.")
MAN_END

MAN_BEGIN (U"Formulas 3. Constants", U"ppgb", 20080318)
TAG (U"##pi")
DEFINITION (U"%\\pi, 3.14159265358979323846264338328")
TAG (U"##e")
DEFINITION (U"%e, 2.71828182845904523536028747135")
TAG (U"##undefined")
DEFINITION (U"a special value, see @undefined")
MAN_END

MAN_BEGIN (U"Formulas 4. Mathematical functions", U"ppgb", 20170718)
TAG (U"##abs (%x)")
DEFINITION (U"absolute value")
TAG (U"##round (%x)")
DEFINITION (U"nearest integer; round (1.5) = 2")
TAG (U"##floor (%x)")
DEFINITION (U"round down: highest integer value not greater than %x")
TAG (U"##ceiling (%x)")
DEFINITION (U"round up: lowest integer value not less than %x")
TAG (U"##sqrt (%x)")
DEFINITION (U"square root: \\Vr%x, %x \\>_ 0")
TAG (U"##min (%x, ...)")
DEFINITION (U"the minimum of a series of numbers, e.g. min (7.2, -5, 3) = -5")
TAG (U"##max (%x, ...)")
DEFINITION (U"the maximum of a series of numbers, e.g. max (7.2, -5, 3) = 7.2")
TAG (U"##imin (%x, ...)")
DEFINITION (U"the location of the minimum, e.g. imin (7.2, -5, 3) = 2")
TAG (U"##imax (%x, ...)")
DEFINITION (U"the location of the maximum, e.g. imax (7.2, -5, 3) = 1")
TAG (U"##sin (%x)")
DEFINITION (U"sine")
TAG (U"##cos (%x)")
DEFINITION (U"cosine")
TAG (U"##tan (%x)")
DEFINITION (U"tangent")
TAG (U"##arcsin (%x)")
DEFINITION (U"arcsine, -1 \\<_ %x \\<_ 1")
TAG (U"##arccos (%x)")
DEFINITION (U"arccosine, -1 \\<_ %x \\<_ 1")
TAG (U"##arctan (%x)")
DEFINITION (U"arctangent")
TAG (U"##arctan2 (%y, %x)")
DEFINITION (U"argument angle")
TAG (U"##sinc (%x)")
DEFINITION (U"sinus cardinalis: sin (%x) / %x")
TAG (U"##sincpi (%x)")
DEFINITION (U"sinc__%\\pi_: sin (%\\pi%x) / (%\\pi%x)")
TAG (U"##exp (%x)")
DEFINITION (U"exponentiation: %e^%x; same as ##e\\^ %x")
TAG (U"##ln (%x)")
DEFINITION (U"natural logarithm, base %e")
TAG (U"##log10 (%x)")
DEFINITION (U"logarithm, base 10")
TAG (U"##log2 (%x)")
DEFINITION (U"logarithm, base 2")
TAG (U"##sinh (%x)")
DEFINITION (U"hyperbolic sine: (%e^%x - %e^^-%x^) / 2")
TAG (U"##cosh (%x)")
DEFINITION (U"hyperbolic cosine: (%e^%x + %e^^-%x^) / 2")
TAG (U"##tanh (%x)")
DEFINITION (U"hyperbolic tangent: sinh (%x) / cosh (%x)")
TAG (U"##arcsinh (%x)")
DEFINITION (U"inverse hyperbolic sine: ln (%x + \\Vr(1+%x^2))")
TAG (U"##arccosh (%x)")
DEFINITION (U"inverse hyperbolic cosine: ln (%x + \\Vr(%x^2\\--1))")
TAG (U"##arctanh (%x)")
DEFINITION (U"inverse hyperbolic tangent")
TAG (U"##sigmoid (%x)")
DEFINITION (U"#R \\-> (0,1): 1 / (1 + %e^^\\--%x^) or 1 \\-- 1 / (1 + %e^%x)")
TAG (U"##invSigmoid (%x)")
DEFINITION (U"(0,1) \\-> #R: ln (%x / (1 \\-- %x))")
TAG (U"##erf (%x)")
DEFINITION (U"the error function: 2/\\Vr%\\pi __0_\\in^%x exp(-%t^2) %dt")
TAG (U"##erfc (%x)")
DEFINITION (U"the complement of the error function: 1 - erf (%x)")
TAG (U"##randomUniform (%min, %max)")
DEFINITION (U"uniform random real number between %min (inclusive) and %max (exclusive)")
TAG (U"##randomInteger (%min, %max)")
DEFINITION (U"uniform random integer number between %min and %max (inclusive)")
TAG (U"##randomGauss (%\\mu, %\\si)")
DEFINITION (U"Gaussian random real number with mean %\\mu and standard deviation %\\si")
TAG (U"##randomPoisson (%mean)")
DEFINITION (U"Poisson random real number")
TAG (U"##lnGamma (%x)")
DEFINITION (U"logarithm of the \\Ga function")
TAG (U"##gaussP (%z)")
DEFINITION (U"the area under the Gaussian distribution between \\--\\oo and %z")
TAG (U"##gaussQ (%z)")
DEFINITION (U"the area under the Gaussian distribution between %z and +\\oo: "
	"the one-tailed \"statistical significance %p\" of a value that is %z standard deviations "
	"away from the mean of a Gaussian distribution")
TAG (U"##invGaussQ (%q)")
DEFINITION (U"the value of %z for which $gaussQ (%z) = %q")
TAG (U"##chiSquareP (%chiSquare, %df)")
DEFINITION (U"the area under the %\\ci^2 distribution between 0 and %chiSquare, for %df degrees of freedom")
TAG (U"##chiSquareQ (%chiSquare, %df)")
DEFINITION (U"the area under the %\\ci^2 distribution between %chiSquare and +\\oo, "
	"for %df degrees of freedom: the \"statistical significance %p\" "
	"of the %\\ci^2 difference between two distributions in %df+1 dimensions")
TAG (U"##invChiSquareQ (%q, %df)")
DEFINITION (U"the value of %\\ci^2 for which $chiSquareQ (%\\ci^2, %df) = %q")
TAG (U"##studentP (%t, %df)")
DEFINITION (U"the area under the student T-distribution from -\\oo to %t")
TAG (U"##studentQ (%t, %df)")
DEFINITION (U"the area under the student T-distribution from %t to +\\oo")
TAG (U"##invStudentQ (%q, %df)")
DEFINITION (U"the value of %t for which $studentQ (%t, %df) = %q")
TAG (U"##fisherP (%f, %df1, %df2)")
DEFINITION (U"the area under Fisher's F-distribution from 0 to %f")
TAG (U"##fisherQ (%f, %df1, %df2)")
DEFINITION (U"the area under Fisher's F-distribution from %f to +\\oo")
TAG (U"##invFisherQ (%q, %df1, %df2)")
DEFINITION (U"the value of %f for which $fisherQ (%f, %df1, %df2) = %q")
TAG (U"##binomialP (%p, %k, %n)")
DEFINITION (U"the probability that in %n experiments, an event with probability %p will occur at most %k times")
TAG (U"@binomialQ (%p, %k, %n)")
DEFINITION (U"the probability that in %n experiments, an event with probability %p will occur at least %k times; equals 1 - $binomialP (%p, %k - 1, %n)")
TAG (U"##invBinomialP (%P, %k, %n)")
DEFINITION (U"the value of %p for which $binomialP (%p, %k, %n) = %P")
TAG (U"##invBinomialQ (%Q, %k, %n)")
DEFINITION (U"the value of %p for which $binomialQ (%p, %k, %n) = %Q")
TAG (U"##hertzToBark (%x)")
DEFINITION (U"from acoustic frequency to Bark-rate (perceptual spectral frequency; place on basilar membrane): "
	"7 ln (%x/650 + \\Vr(1 + (%x/650)^2))")
TAG (U"##barkToHertz (%x)")
DEFINITION (U"650 sinh (%x / 7)")
TAG (U"##hertzToMel (%x)")
DEFINITION (U"from acoustic frequency to perceptual pitch: 550 ln (1 + %x / 550)")
TAG (U"##melToHertz (%x)")
DEFINITION (U"550 (exp (%x / 550) - 1)")
TAG (U"##hertzToSemitones (%x)")
DEFINITION (U"from acoustic frequency to a logarithmic musical scale, relative to 100 Hz: 12 ln (%x / 100) / ln 2")
TAG (U"##semitonesToHertz (%x)")
DEFINITION (U"100 exp (%x ln 2 / 12)")
TAG (U"##erb (%f)")
DEFINITION (U"the perceptual %%equivalent rectangular bandwidth% (ERB) in hertz, for a specified acoustic frequency (also in hertz): "
	"6.23\\.c10^^-6^ %f^2 + 0.09339 %f + 28.52")
TAG (U"##hertzToErb (%x)")
DEFINITION (U"from acoustic frequency to ERB-rate: 11.17 ln ((%x + 312) / (%x + 14680)) + 43")
TAG (U"##erbToHertz (%x)")
DEFINITION (U"(14680 %d - 312) / (1 - %d) where %d = exp ((%x - 43) / 11.17)")
TAG (U"@phonToDifferenceLimens (%x)")
DEFINITION (U"from perceptual loudness (intensity sensation) level in phon, to the number of intensity "
	"difference limens above threshold: 30 \\.c ((61/60)^^ %x^ \\-- 1).")
TAG (U"@differenceLimensToPhon (%x)")
DEFINITION (U"the inverse of the previous: ln (1 + %x / 30) / ln (61 / 60).")
TAG (U"##beta (%x, %y)")
TAG (U"##besselI (%n, %x)")
TAG (U"##besselK (%n, %x)")
MAN_END

MAN_BEGIN (U"Formulas 5. String functions", U"ppgb", 20140223)
INTRO (U"String functions are functions that either return a text string or have at least one text string as an argument. "
	"Since string computations are not very useful in the @calculator, in settings windows, or in creation and "
	"modification formulas, this page only gives examples of strings in scripts, so that the example may contain "
	"string variables.")
TAG (U"##length (a\\$ )")
DEFINITION (U"gives the length of the string. After")
		CODE2 (U"string\\$  = \"hallo\"")
		CODE2 (U"length = length (string\\$  + \"dag\")")
DEFINITION (U"the variable %length contains the number 8 (by the way, from this example "
	"you see that variables can have the same names as functions, without any danger of confusing the interpreter).")
TAG (U"##left\\$  (a\\$ , n)")
DEFINITION (U"gives a string consisting of the first %n characters of %%a\\$ %. After")
		CODE2 (U"head\\$  = left\\$  (\"hallo\", 3)")
DEFINITION (U"the variable %%head\\$ % contains the string \"hal\".")
TAG (U"##right\\$  (a\\$ , n)")
DEFINITION (U"gives a string consisting of the last %n characters of %%a\\$ %. After")
		CODE2 (U"english\\$  = \"he\" + right\\$  (\"hallo\", 3)")
DEFINITION (U"the variable %%english\\$ % contains the string \"hello\".")
TAG (U"##mid\\$  (\"hello\" , 3, 2)")
DEFINITION (U"gives a string consisting of 2 characters from \"hello\", starting at the third character. Outcome: ll.")
TAG (U"##index (a\\$ , b\\$ )")
DEFINITION (U"gives the index of the first occurrence of the string %%b\\$ % in the string %%a\\$ %. After")
		CODE2 (U"where = index (\"hallo allemaal\", \"al\")")
DEFINITION (U"the variable %where contains the number 2, because the first \"al\" starts at the second character of the longer string. "
	"If the first string does not contain the second string, %index returns 0.")
TAG (U"##rindex (a\\$ , b\\$ )")
DEFINITION (U"gives the index of the last occurrence of the string %%b\\$ % in the string %%a\\$ %. After")
		CODE2 (U"where = rindex (\"hallo allemaal\", \"al\")")
DEFINITION (U"the variable %where contains the number 13, because the last \"al\" starts at the 13th character. "
	"If the first string does not contain the second string, %rindex returns 0.")
TAG (U"##startsWith (a\\$ , b\\$ )")
DEFINITION (U"determines whether the string %%a\\$ % starts with the string %%b\\$ %. After")
		CODE2 (U"where = startsWith (\"internationalization\", \"int\")")
DEFINITION (U"the variable %where contains the number 1 (true).")
TAG (U"##endsWith (a\\$ , b\\$ )")
DEFINITION (U"determines whether the string %%a\\$ % ends with the string %%b\\$ %. After")
		CODE2 (U"where = endsWith (\"internationalization\", \"nation\")")
DEFINITION (U"the variable %where contains the number 0 (false).")
TAG (U"##replace\\$  (a\\$ , b\\$ , c\\$ , n)")
DEFINITION (U"gives a string that is like %%a\\$ %, but where (at most %n) occurrences of %%b\\$ % are replaced with the string %%c\\$ %. After")
		CODE2 (U"s\\$  = replace\\$  (\"hello\", \"l\", \"m\", 0)")
DEFINITION (U"the variable %%s\\$ % contains the string \"hemmo\". After")
		CODE2 (U"s\\$  = replace\\$  (\"hello\", \"l\", \"m\", 1)")
DEFINITION (U"the variable %%s\\$ % contains the string \"hemlo\". The number %n determines the maximum number of occurrences of %%b\\$ % "
	"that can be replaced. If %n is 0, all occurrences are replaced.")
TAG (U"##index_regex (a\\$ , b\\$ )")
DEFINITION (U"determines where the string %%a\\$ % first matches the @@regular expressions|regular expression@ %%b\\$ %. After")
		CODE2 (U"where = index_regex (\"internationalization\", \"a.*n\")")
DEFINITION (U"the variable %where contains the number 7. If there is no match, the outcome is 0.")
TAG (U"##rindex_regex (a\\$ , b\\$ )")
DEFINITION (U"determines where the string %%a\\$ % last matches the @@regular expressions|regular expression@ %%b\\$ %. After")
		CODE2 (U"where = rindex_regex (\"internationalization\", \"a.*n\")")
DEFINITION (U"the variable %where contains the number 16. If there is no match, the outcome is 0.")
TAG (U"##replace_regex\\$  (a\\$ , b\\$ , c\\$ , n)")
DEFINITION (U"gives a string that is like %%a\\$ %, but where (at most %n) substrings that match the @@regular expressions|regular expression@ %%b\\$ % "
	"are replaced with the expression %%c\\$ %. After")
		CODE2 (U"s\\$  = replace_regex\\$  (\"hello\", \".\", \"&&\", 0)")
DEFINITION (U"the variable %%s\\$ % contains the string \"hheelllloo\". If there is no match, "
	"the outcome is the original string a\\$ . After")
		CODE2 (U"s\\$  = replace_regex\\$  (\"hello\", \".\", \"&&\", 1)")
DEFINITION (U"the variable %%s\\$ % contains the string \"hhello\". The number %n determines the maximum number of text pieces "
	"that can be replaced. If %n is 0, all matching text pieces are replaced.")
TAG (U"##string\\$  (number)")
DEFINITION (U"formats a number as a string. Thus, $$string\\$  (5e6)$ "
	"becomes the string $$5000000$, and $$string\\$  (56\\% )$ becomes the string $$0.56$.")
TAG (U"##fixed\\$  (number, precision)")
DEFINITION (U"formats a number as a string with %precision digits after the decimal point. Thus, $$fixed\\$  (72.65687, 3)$ "
	"becomes the string $$72.657$, and $$fixed\\$  (72.65001, 3)$ becomes the string $$72.650$. "
	"In these examples, we see that the result can be rounded up and that trailing zeroes are kept. "
	"At least one digit of precision is always given, e.g. $$fixed\\$  (0.0000157, 3)$ becomes the string $$0.00002$. "
	"The number 0 always becomes the string $0.")
TAG (U"##percent\\$  (number, precision)")
DEFINITION (U"the same as ##fixed\\$ #, but with a percent sign. For instance, $$percent\\$ (0.157, 3)$ becomes $$15.700\\% $, "
	"$$percent\\$ (0.000157, 3)$ becomes $$0.016\\% $, and $$percent\\$  (0.000000157, 3)$ becomes $$0.00002\\% $. "
	"The number 0 always becomes the string $0.")
TAG (U"##number (a\\$ )")
DEFINITION (U"interprets a string as a number.")
		CODE2 (U"string\\$  = \"5e6\"")
		CODE2 (U"writeInfoLine: 3 + number (string\\$ )")
DEFINITION (U"the Info window contains the number 500003.")
TAG (U"##date\\$  ( )")
DEFINITION (U"gives the date and time in the following format:")
		CODE2 (U"Mon Jun 24 17:11:21 2002")
DEFINITION (U"To write the day of the month into the Info window, you type:")
		CODE2 (U"date\\$  = date\\$  ()")
		CODE2 (U"day\\$  = mid\\$  (date\\$ , 9, 2)")
		CODE2 (U"writeInfoLine: \"The month day is \", day\\$ , \".\"")
TAG (U"##extractNumber (\"Type: Sound\" + newline\\$  + \"Name: hello there\" + newline\\$  + \"Size: 44007\", \"Size:\")")
DEFINITION (U"looks for a number after the first occurrence of \"Size:\" in the long string. Outcome: 44007. "
	"This is useful in scripts that try to get information from long reports, as the following script that "
	"runs in the Sound editor window:")
		CODE2 (U"report\\$  = Editor info")
		CODE2 (U"maximumFrequency = extractNumber (report\\$ , \"Spectrogram window length:\")")
TAG (U"##extractWord\\$  (\"Type: Sound\" + newline\\$  + \"Name: hello there\" + newline\\$  + \"Size: 44007\", \"Type:\")")
DEFINITION (U"looks for a word without spaces after the first occurrence of \"Type:\" in the long string. Outcome: Sound.")
TAG (U"##extractLine\\$  (\"Type: Sound\" + newline\\$  + \"Name: hello there\" + newline\\$  + \"Size: 44007\", \"Name: \")")
DEFINITION (U"looks for the rest of the line (including spaces) after the first occurrence of \"Name: \" in the long string. "
	"Outcome: hello there. Note how \"Name: \" includes a space, so that the `rest of the line' starts with the %h.")
TAG (U"##backslashTrigraphsToUnicode\\$  (x\\$ ), unicodeToBackslashTrigraphs\\$  (x\\$ )")
DEFINITION (U"converts e.g. \\bsct to \\ct or the reverse. See @@Special symbols@.")
MAN_END

MAN_BEGIN (U"Formulas 6. Control structures", U"ppgb", 20030519)
ENTRY (U"if ... then ... else ... fi")
NORMAL (U"You can use conditional expressions in all formulas. For example, ")
CODE (U"3 * if 52\\%  * 3809 > 2000 then 5 else 6 fi")
NORMAL (U"evaluates to 15. Instead of %fi, you can also use %endif.")
NORMAL (U"Another example: you can clip the absolute amplitude of a Sound to 0.5 by supplying the following formula:")
CODE (U"if abs(self)>0.5 then if self>0 then 0.5 else -0.5 fi else self fi")
ENTRY (U"The semicolon")
NORMAL (U"The semicolon ends the evaluation of the formula. This can be convenient "
	"if you do not want to overwrite a long formula in your text field: the formula")
CODE (U"800;sqrt(2)*sin(2*pi*103*0.5)+10\\^ (-40/20)*randomGauss(0,1)")
NORMAL (U"evaluates to 800.")
MAN_END

MAN_BEGIN (U"Formulas 7. Attributes of objects", U"ppgb", 20170614)
NORMAL (U"You can refer to several attributes of objects that are visible in the @@List of Objects@. "
	"To do so, use either the unique ID of the object, or the type and the name of the object. "
	"Thus, $$object[113]$ refers to the object that has the number 113 in the list, "
	"and $$object[\"Sound hallo\"]$ refers to an existing Sound object whose name is \"hallo\" "
	"(if there is more than one such object, it refers to the one that was created last).")
NORMAL (U"To refer to an attribute, you use the period (\".\"). "
	"Thus, $$object[\"Sound hallo\"].nx$ is the number of samples of the Sound called %hallo, and "
	"$$1/object[\"Sound hallo\"].dx$ is its sampling frequency.")
ENTRY (U"Attributes in the calculator")
NORMAL (U"Record a Sound (read the @Intro if you do not know how to do that), "
	"and name it \"mysound\" (or anything else). An object with a name like \"3. Sound mysound\" "
	"will appear in the list. Then type into the @calculator the formula")
CODE (U"object[3].nx")
NORMAL (U"or")
CODE (U"object[\"Sound mysound\"].nx")
NORMAL (U"After you click OK, the Info window will show the number of samples. Since you could have got this result "
	"by simply selecting the object and choosing ##%%Get number of samples#% from the @Query menu, "
	"these attribute tricks are not very useful in the calculator. "
	"We will see that they are much more useful in creation and modification formulas and in scripts.")
ENTRY (U"List of possible attributes")
NORMAL (U"The following attributes are available:")
TAG (U"#xmin")
DEFINITION (U"the start of the time domain (usually 0) for a @Sound, @Pitch, @Formant, "
	"@Spectrogram, @Intensity, @Cochleagram, @PointProcess, or @Harmonicity object, in seconds; "
	"the lowest frequency (always 0) for a @Spectrum object, in hertz; "
	"the lowest frequency (usually 0) for an @Excitation object, in Bark; "
	"the left edge of the %x domain for a @Matrix object.")
TAG (U"#xmax")
DEFINITION (U"the end of the time domain (usually the duration, if %xmin is zero) for a Sound, Pitch, Formant, "
	"Spectrogram, Intensity, Cochleagram, PointProcess, or Harmonicity object, in seconds; "
	"the highest frequency (@@Nyquist frequency@) for a Spectrum object, e.g. 11025 hertz; "
	"the highest frequency for an Excitation object, often 25.6 Bark; "
	"the right edge of the %x domain for a Matrix object.")
TAG (U"#ncol")
DEFINITION (U"the number of columns in a @Matrix, @TableOfReal, or @Table object.")
TAG (U"#nrow")
DEFINITION (U"the number of rows in a @Matrix, @TableOfReal, or @Table object.")
TAG (U"##col\\$ # [%i]")
DEFINITION (U"the name of column %i in a @TableOfReal or @Table object.")
TAG (U"##row\\$ # [%i]")
DEFINITION (U"the name of row %i in a @TableOfReal object.")
TAG (U"#nx")
DEFINITION (U"the number of samples in a Sound object; "
	"the number of analysis frames in a Pitch, Formant, Spectrogram, Intensity, Cochleagram, or Harmonicity object; "
	"the number of frequency bins in a Spectrum or Excitation object; "
	"the number of divisions of the %x domain for a Matrix object (= %ncol).")
TAG (U"#dx")
DEFINITION (U"the sample period (time distance between consecutive samples) in a Sound object (the inverse of the sampling frequency), in seconds; "
	"the time step between consecutive frames in a Pitch, Formant, Spectrogram, Intensity, Cochleagram, or Harmonicity object, in seconds; "
	"the width of a frequency bin in a Spectrum object, in hertz; "
	"the width of a frequency bin in an Excitation object, in Bark; "
	"the horizontal distance between cells in a Matrix object.")
TAG (U"#ymin")
DEFINITION (U"the lowest frequency (usually 0) for a Spectrogram object, in hertz; "
	"the lowest frequency (usually 0) for a Cochleagram object, in Bark; "
	"the bottom of the %y domain for a Matrix object.")
TAG (U"#ymax")
DEFINITION (U"the highest frequency for a Spectrogram object, e.g. 5000 hertz; "
	"the highest frequency for a Cochleagram object, often 25.6 Bark; "
	"the top of the %y domain for a Matrix object.")
TAG (U"#ny")
DEFINITION (U"the number of frequency bands in a Spectrogram or Cochleagram object; "
	"for a Spectrum object: always 2 (first row is real part, second row is imaginary part) "
	"the number of divisions of the %y domain for a Matrix object (= %nrow).")
TAG (U"#dy")
DEFINITION (U"the distance between adjacent frequency bands in a Spectrogram object, in hertz; "
	"the distance between adjacent frequency bands in a Cochleagram object, in Bark; "
	"the vertical distance between cells in a Matrix object.")
ENTRY (U"Attributes in a creation formula")
NORMAL (U"In formulas for creating a new object, you can refer to the attributes of any object, "
	"but you will often want to refer to the attributes of the object that is just being created. You can do that in two ways.")
NORMAL (U"The first way is to use the name of the object, as above. Choose @@Create Sound from formula...@, supply %hello for its name, "
	"supply arbitrary values for the starting and finishing time, and type the following formula:")
CODE (U"(x - object[\"Sound hello\"].xmin) / (object[\"Sound hello\"].xmax - object[\"Sound hello\"].xmin)")
NORMAL (U"When you edit this sound, you can see that it creates a straight line that rises from 0 to 1 within the time domain.")
NORMAL (U"The formula above will also work if the Sound under creation is called %goodbye, and a Sound called %hello already exists; "
	"of course, in such a case $$object[\"Sound hello\"].xmax$ refers to a property of the already existing sound.")
NORMAL (U"If a formula refers to an object under creation, there is a shorter way: you do not have to supply the name of the object at all, "
	"so you can simply write")
CODE (U"(x - xmin) / (xmax - xmin)")
NORMAL (U"The attributes that you can use in this implicit way are %xmin, %xmax, %ncol, %nrow, %nx, %dx, %ny, and %dy. "
	"To disambiguate in case there exists a script variable %xmin as well "
	"(Praat will complain if this is the case), you can write $$Self.xmin$.")
ENTRY (U"Attributes in a modification formula")
NORMAL (U"In formulas for modifying an existing object, you refer to attributes in the same way as in creation formulas, "
	"i.e., you do not have to specify the name of the object that is being modified. The formula")
CODE (U"self * 20 \\^  (- (x - xmin) / (xmax - xmin))")
NORMAL (U"causes the sound to decay exponentially in such a way that it has only 5 percent of its initial amplitude at the end. "
	"If you apply this formula to multiple Sound objects at the same time, $xmax will refer to the finishing time of each Sound separately "
	"as that Sound is modified.")
NORMAL (U"More examples of the use of attributes are on the next page.")
MAN_END

MAN_BEGIN (U"Formulas 8. Data in objects", U"ppgb", 20170614)
NORMAL (U"With square brackets, you can get the values inside some objects.")
ENTRY (U"Object contents in the calculator")
NORMAL (U"The outcomes of the following examples can be checked with the @calculator.")
TAG (U"##object [%%objectName\\$  or id%, %rowNumber, %columnNumber]")
TAG (U"$$object [\"Matrix hello\", 10, 3]")
DEFINITION (U"gives the value in the cell at the third column of the 10th row of the Matrix called %hello.")
TAG (U"$$object [5, 10, 3]")
DEFINITION (U"gives the value in the cell at the third column of the 10th row of the Matrix whose unique ID is 5 "
	"(i.e. that is labelled with the number 5 in the list of objects).")
TAG (U"$$object [\"Sound hello\", 0, 10000]")
DEFINITION (U"gives the value (in Pa) of the 10000th sample of the Sound %hello, averaged over the channels.")
TAG (U"$$object [23, 1, 10000]")
DEFINITION (U"gives the value (in Pa) of the 10000th sample of the left channel of the Sound with ID 23.")
TAG (U"$$object [23, 2, 10000]")
DEFINITION (U"gives the value (in Pa) of the 10000th sample of the right channel of the Sound with ID 23.")
TAG (U"$$object [\"TableOfReal tokens\", 5, 12]")
DEFINITION (U"gives the value in the cell at the fifth row of the 12th column of the TableOfReal called %tokens.")
TAG (U"$$object [\"TableOfReal tokens\", 5, \"F1\"]")
DEFINITION (U"gives the value in the cell at the fifth row of the column labelled %F1 of the TableOfReal %tokens.")
TAG (U"$$object [\"TableOfReal tokens\", \"\\bsct\", \"F1\"]")
DEFINITION (U"gives the value in the cell at the row labelled %%\\bsct% of column %F1 of the TableOfReal %tokens.")
TAG (U"$$object [\"Table listeners\", 3, \"m3ae\"]")
DEFINITION (U"gives the numeric value in the cell at the third row of column %m3ae of the Table %listeners.")
TAG (U"$$object [\"Table listeners\", 3, 12]")
DEFINITION (U"gives the numeric value in the cell at the third row of the 12th column of the Table %listeners.")
TAG (U"$$object\\$  [\"Table results\", 3, \"response\"]")
DEFINITION (U"gives the string value in the cell at the third row of column %response of the Table %results.")
TAG (U"$$object\\$  [\"Table results\", 3, 12]")
DEFINITION (U"gives the string value in the cell at the third row of the 12th column of the Table %results.")
TAG (U"$$object [\"PitchTier hello\", 8]")
DEFINITION (U"gives the pitch (in Hertz) of the 8th point in the PitchTier %hello.")
NORMAL (U"Cells (or samples, or points) outside the objects are considered to contain zeroes.")
ENTRY (U"Interpolation")
NORMAL (U"The values inside some objects can be interpolated.")
TAG (U"$$object (\"Sound hello\", 0.7, 0)")
DEFINITION (U"gives the value (in Pa) at a time of 0.7 seconds in the Sound %hello, by linear interpolation between "
	"the two samples that are nearest to 0.7 seconds. The channels are averaged.")
TAG (U"$$object (\"Sound hello\", 0.7, 1)")
DEFINITION (U"gives the interpolated value (in Pa) at a time of 0.7 seconds in the left channel of the Sound %hello.")
TAG (U"$$object (\"Sound hello\", 0.7, 2)")
DEFINITION (U"gives the interpolated value (in Pa) at a time of 0.7 seconds in the right channel of the Sound %hello.")
TAG (U"$$object (\"Spectrogram hallo\", 0.7, 2500)")
DEFINITION (U"gives the value at a time of 0.7 seconds and at a frequency of 2500 Hz in the Spectrogram %hallo, "
	"by linear interpolation between the four samples that are nearest to that point.")
TAG (U"$$object (\"PitchTier hullo\", 0.7)")
DEFINITION (U"gives the pitch (in Hertz) at a time of 0.7 seconds in the PitchTier %hullo.")
NORMAL (U"In the interpolation, times outside the time domain of the objects are considered to contain zeroes (this does not apply to PitchTiers and the like, "
	"which undergo @@constant extrapolation@).")
ENTRY (U"Object contents in a modification formula")
NORMAL (U"Suppose you want to do the difficult way of reversing the contents of a Sound called %hello (the easy way is to choose #Reverse "
	"from the @Modify menu). You select this sound, then choose @@Copy...@ to duplicate it to a new Sound, which you name %%hello_reverse%. "
	"You select this new Sound and choose ##Formula...# from the @Modify menu. The formula will be")
CODE (U"object [\"Sound hello\", row, ncol + 1 - col]")
NORMAL (U"From this example, you see that the indices between [ ] may be formulas themselves, and that you can use implicit attributes like %ncol "
	"and position references like %col (also %row, which here means that the reversal is performed for each channel). "
	"An alternative formula is")
CODE (U"object (\"Sound hello\", xmax - x, y)")
NORMAL (U"at least if %xmin is zero. The advantage of the second method is that it also works correctly if the two sounds have different sampling frequencies; "
	"the disadvantage is that it may do some interpolation between the samples, which deteriorates the sound quality "
	"(the use of %y here means that the reversal is done for all %y values, i.e. all channels).")
ENTRY (U"Object contents in a script")
NORMAL (U"In scripts, the indices between [ ] and the values between ( ) may be formulas themselves and contain variables. "
	"The following script computes the sum of all the cells along the diagonal of a Matrix.")
CODE (U"matrix = Create simple matrix: 10, 10, \"x*y\"")
CODE (U"sumDiagonal = 0")
CODE (U"for i to object[matrix].ncol")
	CODE1 (U"sumDiagonal += object [matrix, i, i]")
CODE (U"endfor")
CODE (U"writeInfoLine: \"The sum of the cells along the diagonal is \", sumDiagonal, \".\"")
NORMAL (U"This example could have been written completely with commands from the dynamic menu:")
CODE (U"matrix = Create simple matrix: 10, 10, \"x*y\"")
CODE (U"sumDiagonal = 0")
CODE (U"ncol = Get number of columns")
CODE (U"for i to ncol")
	CODE1 (U"value = Get value in cell: i, i")
	CODE1 (U"sumDiagonal += value")
CODE (U"endfor")
CODE (U"writeInfoLine: \"The sum of the cells along the diagonal is \", sumDiagonal, \".\"")
NORMAL (U"The first version, which accesses the contents directly, is not only two lines shorter, but also three times faster.")
MAN_END

MAN_BEGIN (U"Hidden commands", U"ppgb", 20110129)
NORMAL (U"Some commands in Praat's fixed and dynamic menus are hidden by default. "
	"You can still call hidden commands from scripts, run them by clicking on them in a @ButtonEditor, "
	"or make them visible with the help of the @ButtonEditor.")
NORMAL (U"To hide commands that are visible by default, use the @ButtonEditor.")
ENTRY (U"What commands are hidden by default?")
NORMAL (U"Commands that are expected to be of very limited use, are hidden by default. Examples are:")
LIST_ITEM (U"1. The commands @@Add menu command...@, ##Hide menu command...#, ##Show menu command...#, "
	"@@Add action command...@, ##Hide action command...#, and ##Show action command...# in the #Praat menu "
	"of the @@Object window@. These are used in the @@buttons file@ and could be used by an @@initialization script@ or a @@plug-ins|plug-in@ "
	"as well; in an interactive session, however, the functionality of these commands is part of the "
	"@ScriptEditor and the @ButtonEditor.")
LIST_ITEM (U"2. The command ##Read from old Praat picture file...# in the #File menu of the @@Picture window@. "
	"For reading a file format that was in use before May, 1995.")
LIST_ITEM (U"3. In the Praat program, the action ##Sound: Save as Sesam file...#. Writes a file format in common use "
	"in the Netherlands on Vax machines. In the Dutch phonetics departments, the plugs were pulled from the Vaxes in 1994.")
LIST_ITEM (U"4. In the Praat program, the action ##Sound: To Cochleagram (edb)...#. Needed by one person in 1994. "
	"An interesting, but undocumented procedure (De Boer's gammatone filter bank plus Meddis & Hewitt's "
	"synapse model), which does not create a normally interpretable Cochleagram object.")
MAN_END

MAN_BEGIN (U"History mechanism", U"ppgb", 20040414)
INTRO (U"The easiest way to do @@scripting@. "
	"The %history is the sequence of all menu commands "
	"(in the Objects or Picture window or in the editors), "
	"action commands (in the dynamic menu), "
	"or mouse clicks on objects (in the list of objects), that you performed during your Praat session, "
	"together with the settings that you specified in the settings windows "
	"that popped up as a result of those commands.")
ENTRY (U"Viewing the history")
NORMAL (U"To view your history, you first open a @ScriptEditor with @@New Praat script@ or @@Open Praat script...@. "
	"You then choose @@Paste history@ from the #Edit menu.")
ENTRY (U"Recording a macro")
NORMAL (U"To record a sequence of mouse clicks for later re-use, "
	"perform the following steps:")
LIST_ITEM (U"1. Choose @@Clear history@ from the #Edit menu. "
	"This makes the history mechanism forget all previous clicks.")
LIST_ITEM (U"2. Perform the actions that you want to record.")
LIST_ITEM (U"3. Choose @@Paste history@ from the #Edit menu. Because you cleared the history "
	"before you started, the resulting script contains only the actions "
	"that you performed in step 2. "
	"You can now already re-run the actions that you performed in step 2.")
LIST_ITEM (U"4. You can save the recorded actions to a script file by choosing #Save from the #File menu.")
LIST_ITEM (U"5. You can put this script file under a button in the @@dynamic menu@ "
	"by choosing @@Add to dynamic menu...@ from the File menu, "
	"or under a button in a fixed menu by choosing @@Add to fixed menu...@. "
	"This button will be preserved across Praat sessions.")
NORMAL (U"This macro mechanism is much more flexible than the usual opaque macro mechanism "
	"used by most programs, because you can edit the script and make some "
	"of the arguments variable by putting them in the #form clause at the top of the script. "
	"In this way, the script will prompt the user for these arguments, "
	"just as with all the menu and action commands that end in the three dots (...). "
	"See the @Scripting tutorial for all the things that you can do in scripts.")
MAN_END

MAN_BEGIN (U"initialization script", U"ppgb", 20151020)
INTRO (U"Your initialization script is a normal @@Praat script@ that is run as soon as you start Praat.")
NORMAL (U"On Unix or MacOS X, you create an initialization script by creating a file named \"praat-startUp\" "
	"in the directory /usr/local, "
	"or putting a file \".praat-user-startUp\" or \"praat-user-startUp\" in your home directory "
	"(if you rename the Praat executable, these names have to change as well).")
NORMAL (U"If you have more than one of these files, they are run in the above order.")
NORMAL (U"On Windows, you create an initialization script by creating a file named "
	"\"praat-user-startUp\" in your home directory, "
	"which could be C:\\bsUsers\\bsMiep if you are Miep.")
NORMAL (U"If you have both of these files, they are run in the above order.")
ENTRY (U"Example")
NORMAL (U"If you like to be greeted by your husband when Praat starts up, "
	"you could put the following lines in your initialization script:")
CODE (U"Read from file: \"C:\\bsUsers\\bsMiep\\bshelloMiep.wav\"   ! Windows")
CODE (U"Read from file: \"/Users/miep/helloMiep.wav\"   ! Mac")
CODE (U"Read from file: \"/home/miep/helloMiep.wav\"   ! Linux")
CODE (U"Play")
CODE (U"Remove")
ENTRY (U"What not to use an initialization script for")
NORMAL (U"You could set preferences like the default font in your initialization script, "
	"but these will be automatically remembered between invocations of Praat anyway (in your @@preferences file@), "
	"so this would often be superfluous.")
NORMAL (U"For installing sets of menu commands at start-up you will probably prefer to use @@plug-ins@ "
	"rather than a single start-up file.")
MAN_END

MAN_BEGIN (U"New Praat script", U"ppgb", 20050822)
INTRO (U"A command in the @@Praat menu@ for creating a new Praat script. "
	"It creates a @ScriptEditor with an empty script that you can edit, run, and save.") 
MAN_END

MAN_BEGIN (U"Open Praat script...", U"ppgb", 20050822)
INTRO (U"A command in the @@Praat menu@ for editing an existing @@Praat script@. "
	"It creates a @ScriptEditor and asks "
	"you to select a file. If you click #%OK, the file is read into the ScriptEditor window, "
	"and you can run and edit it; if you click #%Cancel, you get an empty script, as with @@New Praat script@.") 
MAN_END

MAN_BEGIN (U"Paste history", U"ppgb", 20050822)
INTRO (U"A command in the #Edit menu of a @ScriptEditor, for inserting the history of commands. "
	"See @@History mechanism@.")
MAN_END

MAN_BEGIN (U"phonToDifferenceLimens", U"ppgb", 20021215)
INTRO (U"A routine for converting sensation level in phons into intensity difference limen level, "
	"the inverse of @differenceLimensToPhon.")
ENTRY (U"Formula")
FORMULA (U"phonToDifferenceLimens (%phon) = 30 \\.c ((61/60)^^ %phon^ \\-- 1)")
ENTRY (U"Derivation")
NORMAL (U"In first approximation, humans can detect an intensity difference of 1 phon, i.e. "
	"if two sounds that differ only in intensity are played a short time after each other, "
	"people can generally detect their intensity difference if it is greater than 1 phon.")
NORMAL (U"But the sensitivity is somewhat better for louder sounds. "
	"According to @@Jesteadt, Wier & Green (1977)@, the relative difference limen "
	"of intensity is given by")
FORMULA (U"DLI = \\De%I / %I = 0.463 \\.c (%I / %I__0_)^^ \\--0.072^")
NORMAL (U"In this formula, %I is the intensity of the sound in Watt/m^2, %I__0_ is the intensity of "
	"the auditory threshold (i.e. 10^^\\--12^ Watt/m^2 at 1000 Hz), and \\De%I is the just noticeable difference.")
NORMAL (U"@@Boersma (1998)|Boersma (1998: 109)@ calculates a difference-limen scale from this. "
	"Given an intensity %I, the number of difference limens above threshold is ")
FORMULA (U"\\in__%I0_^%I %dx \\De%I(%x) "
	"= (1 / 0.463) \\in__%I0_^%I %dx %I__0_^^\\--0.072^ %x^^0.072\\--1^")
FORMULA (U"= (1 / (0.463\\.c0.072)) ((%I/%I__0_)^^0.072^ \\-- 1)")
NORMAL (U"The sensation level in phon is defined as")
FORMULA (U"SL = 10 log__10_ (%I/%I__0_)")
NORMAL (U"so that the number of difference limens above threshold is")
FORMULA (U"(1 / (0.463\\.c0.072)) (10^^(0.072/10)(10log(%I/%I__0_))^ \\-- 1) "
	"= 30 \\.c (1.0167^^SL^ \\-- 1)")
MAN_END

MAN_BEGIN (U"plug-ins", U"ppgb", 20151020)
INTRO (U"Experienced Praat script writers can distribute their product as a plug-in to Praat.")
ENTRY (U"The Praat plug-in mechanism")
NORMAL (U"When Praat starts up, it will execute all Praat scripts called ##setup.praat# "
	"that reside in directories whose name starts with ##plugin_# and that themselves reside in "
	"your Praat @@preferences directory@.")
ENTRY (U"How to write a Praat plug-in")
NORMAL (U"Suppose that you have a set of Praat scripts specialized in the analysis and synthesis of vocalizations of guinea pigs, "
	"and that these scripts are called ##analyseQueak.praat# and ##createQueak.praat# (\"queak\" is what guinea pigs tend to say). "
	"With the @ScriptEditor, you have put the script ##analyseQueak.praat# in the dynamic menu that "
	"appears if the user selects a Sound object, and you have put the script ##createQueak.praat# in the @@New menu@. "
	"Only the latter script requires the user to supply some settings in a form, so the two menu commands "
	"are ##Analyse queak# (without dots) and ##Create queak...# (with three dots), respectively. "
	"Suppose now that you want to distribute those two commands to other guinea pig vocalization researchers.")
NORMAL (U"What you do is that you create a Praat script called ##setup.praat# (in the same directory as the two other scripts), "
	"that contains the following two lines:")
CODE (U"@@Add action command...|Add action command:@ \"Sound\", 1, \"\", 0, \"\", 0, \"Analyse queak\", \"\", 0, \"analyseQueak.praat\"")
CODE (U"@@Add menu command...|Add menu command:@ \"Objects\", \"New\", \"Create queak...\", \"\", 0, \"createQueak.praat\"")
NORMAL (U"(If you ran this script, Praat would install those two commands in the correct menus, and remember them in the @@buttons file@; "
	"but you are now going to install them in a different way.)")
NORMAL (U"You now put the three scripts in a new directory called ##plugin_Queak#, "
	"and put this directory in your Praat preferences directory. If you are on Windows, "
	"you will now have a directory called something like ##C:\\bsUsers\\bsYour Name\\bsPraat\\bsplugin_Queak#.")
NORMAL (U"If you now start up Praat, Praat will automatically execute the script "
	"##C:\\bsUsers\\bsYour Name\\bsPraat\\bsplugin_Queak\\bssetup.praat# "
	"and thereby install the two buttons. The two buttons will %not be remembered in the buttons file, "
	"but they will be installed at every Praat start-up. De-installation involves removing (or renaming) the ##plugin_Queak# directory.")
NORMAL (U"To distribute the Queak plug-in among your colleague guinea pig researchers, you can use any installer program to put "
	"the ##plugin_Queak# directory into the user's Praat preferences directory; or you could ask those colleagues to move "
	"the ##plugin_Queak# directory there by hand.")
ENTRY (U"The structure of your plug-in directory")
NORMAL (U"In the example ##setup.praat# file above, the names of the scripts ##analyseQueak.praat# and ##createQueak.praat# "
	"occur without any directory information. This works because Praat regards these file names as being relative to the directory "
	"where ##setup.praat# is located. If your plug-in is much larger than two scripts, you may want to put subdirectories into "
	"the directory ##plugin_Queak#. For instance, if you put ##analyseQueak.praat# into the subdirectory ##analysis#, "
	"your line in the ##setup.praat# script would look as follows:")
CODE (U"@@Add action command...|Add action command:@ \"Sound\", 1, \"\", 0, \"\", 0, \"Analyse queak\", \"\", 0, \"analysis/analyseQueak.praat\"")
NORMAL (U"The forward slash (\"/\") in this example makes your plug-in platform-independent: it will work unchanged "
	"on Windows, Macintosh, and Unix.")
NORMAL (U"Nothing prevents you from adding data files to your plug-in. For instance, your ##plugin_Queak# directory "
	"could contain a subdirectory #sounds full of guinea pig recordings, and you could make them available in the New or Open menu.")
ENTRY (U"Using a plug-in for site-wide customization")
NORMAL (U"If your local guinea pig research group shares a number of Praat scripts, these can be made available to everybody "
	"in the following way:")
LIST_ITEM (U"1. Create a script that adds buttons to the fixed and dynamic menus, using the commands "
	"@@Add menu command...@ and @@Add action command...@. This script could be a slightly edited copy of someone's "
	"@@buttons file@.")
LIST_ITEM (U"2. Put this script where everybody can see it, "
	"for instance in ##U:\\bsMaldenGuineaPigResearchButtons.praat#, where U is your shared computer.")
LIST_ITEM (U"3. Create a file ##setup.praat# that contains only the following line:")
CODE1 (U"runScript: \"U:\\bsMaldenGuineaPigResearchButtons.praat\"")
LIST_ITEM (U"4. Put the ##setup.praat# file in a new directory called ##plugin_MaldenGuineaPigResearch#, "
	"and distribute this directory among your local colleagues.")
NORMAL (U"This procedure allows all members of the group to automatically enjoy all the later changes in your "
	"custom command set.")
MAN_END

MAN_BEGIN (U"plugins", U"ppgb", 20060920)
INTRO (U"See @@plug-ins@.")
MAN_END

MAN_BEGIN (U"Praat script", U"ppgb", 19980824)
INTRO (U"An executable text that consists of menu commands and action commands.")
NORMAL (U"See the @Scripting tutorial.")
MAN_END

MAN_BEGIN (U"preferences directory", U"ppgb", 20151020)
INTRO (U"The Praat preferences directory is the directory where Praat saves the @@preferences file@ and the @@buttons file@, "
	"and where you can install @@plug-ins@ and save the preferences of your scripts (in your subdirectory of the #apps subdirectory). "
	"If the preferences directory does not exist, it will automatically be created when you start Praat.")
ENTRY (U"Windows")
NORMAL (U"If you are user #Miep, your Praat preferences directory will be ##C:\\bsUsers\\bsMiep\\bsPraat\\bs#.")
ENTRY (U"Macintosh")
NORMAL (U"If you are user #miep, your Praat preferences directory will be ##/Users/miep/Library/Preferences/Praat Prefs/#.")
ENTRY (U"Linux")
NORMAL (U"If your home directory is ##/home/miep/#, your Praat preferences directory will be ##/home/miep/.praat-dir/#.")
MAN_END

MAN_BEGIN (U"preferences file", U"ppgb", 20151020)
NORMAL (U"The file into which some of your preferences are saved across your sessions with Praat. "
	"For instance, if you change the font used by the Picture window to Palatino and quit Praat, "
	"the Picture-window font will still be Palatino when you start Praat again.")
NORMAL (U"The preferences file is written to disk when you quit Praat, "
	"and it is read when you start Praat. It is a simple text file that you can read "
	"(but should not edit) with any text editor.")
ENTRY (U"Where is the preferences file?")
NORMAL (U"The preferences file is in your Praat @@preferences directory@.")
NORMAL (U"On Windows it is called ##Preferences5.ini#, "
	"for instance ##C:\\bsUsers\\bsMiep\\bsPraat\\bsPreferences5.ini#.")
NORMAL (U"On Macintosh it is called #Prefs5, "
	"for instance ##/Users/miep/Library/Preferences/Praat Prefs/Prefs5#.")
NORMAL (U"On Linux the file is called #prefs5, "
	"for instance ##/home/miep/.praat-dir/prefs5#.")
MAN_END

MAN_BEGIN (U"Scripting", U"ppgb", 20170718)
INTRO (U"This is one of the tutorials of the Praat program. It assumes you are familiar with the @Intro.")
NORMAL (U"A %script is a text that consists of menu commands and action commands. "
	"If you %run the script (perhaps from a @ScriptEditor), "
	"the commands are executed as if you clicked on them.")
NORMAL (U"You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM (U"@@Scripting 1. Your first scripts@ (how to create, how to run, how to save)")
LIST_ITEM (U"@@Scripting 2. How to script settings windows@ (numeric, boolean, multiple-choice, text, file)")
LIST_ITEM (U"@@Scripting 3. Simple language elements")
LIST_ITEM1 (U"@@Scripting 3.1. Hello world@ (writeInfoLine, appendInfoLine)")
LIST_ITEM1 (U"@@Scripting 3.2. Numeric variables@ (assignments)")
LIST_ITEM1 (U"@@Scripting 3.3. Numeric queries")
LIST_ITEM1 (U"@@Scripting 3.4. String variables@ (assignments)")
LIST_ITEM1 (U"@@Scripting 3.5. String queries")
LIST_ITEM1 (U"@@Scripting 3.6. \"For\" loops@ (for, endfor)")
LIST_ITEM1 (U"@@Scripting 3.7. Layout@ (white space, comments, continuation lines)")
LIST_ITEM (U"@@Scripting 4. Object selection@")
LIST_ITEM1 (U"@@Scripting 4.1. Selecting objects")
LIST_ITEM1 (U"@@Scripting 4.2. Removing objects")
LIST_ITEM1 (U"@@Scripting 4.3. Querying objects")
LIST_ITEM (U"@@Scripting 5. Language elements reference@")
LIST_ITEM1 (U"@@Scripting 5.1. Variables@ (numeric, string)")
LIST_ITEM1 (U"@@Scripting 5.2. Expressions@ (numeric, string)")
LIST_ITEM1 (U"@@Scripting 5.3. Jumps@ (if, then, elsif, else, endif)")
LIST_ITEM1 (U"@@Scripting 5.4. Loops@ (for/endfor, while/endwhile, repeat/until)")
LIST_ITEM1 (U"@@Scripting 5.5. Procedures@ (\\@ , procedure)")
LIST_ITEM1 (U"@@Scripting 5.6. Arrays and dictionaries")
LIST_ITEM1 (U"@@Scripting 5.7. Vectors and matrices")
LIST_ITEM1 (U"@@Scripting 5.8. Including other scripts")
LIST_ITEM1 (U"@@Scripting 5.9. Quitting@ (exitScript)")
LIST_ITEM (U"@@Scripting 6. Communication outside the script")
LIST_ITEM1 (U"@@Scripting 6.1. Arguments to the script@ (form/endform, runScript)")
LIST_ITEM1 (U"@@Scripting 6.2. Writing to the Info window@ (writeInfoLine, appendInfoLine, appendInfo, tab\\$ )")
LIST_ITEM1 (U"@@Scripting 6.3. Query commands@ (Get, Count)")
LIST_ITEM1 (U"@@Scripting 6.4. Files@ (fileReadable, readFile, writeFile, deleteFile, createDirectory)")
LIST_ITEM1 (U"@@Scripting 6.5. Calling system commands@ (runSystem, environment\\$ , stopwatch)")
LIST_ITEM1 (U"@@Scripting 6.6. Controlling the user@ (pause, beginPause/endPause, chooseReadFile\\$ )")
LIST_ITEM1 (U"@@Scripting 6.7. Sending a message to another program@ (sendsocket)")
LIST_ITEM1 (U"@@Scripting 6.8. Messages to the user@ (exitScript, assert, nowarn, nocheck)")
LIST_ITEM1 (U"@@Scripting 6.9. Calling from the command line")
LIST_ITEM (U"@@Scripting 7. Scripting the editors")
LIST_ITEM1 (U"@@Scripting 7.1. Scripting an editor from a shell script@ (editor/endeditor)")
LIST_ITEM1 (U"@@Scripting 7.2. Scripting an editor from within")
LIST_ITEM (U"@@Scripting 8. Controlling Praat from another program")
LIST_ITEM1 (U"@@Scripting 8.1. The sendpraat subroutine")
LIST_ITEM1 (U"@@Scripting 8.2. The sendpraat program")
LIST_ITEM1 (U"@@Scripting 8.3. The sendpraat directive")
// 9. Hiding your script
// 9.1 Incorporating your script into Praat
// 9.2 Incorporating many scripts into Praat (plug-ins)
LIST_ITEM (U"@@Scripting 9.1. Turning a script into a stand-alone program")
LIST_ITEM (U"@@Scripting 9.2. Old functions")
NORMAL (U"Also see the @@scripting examples@.")
MAN_END

MAN_BEGIN (U"Scripting 1. Your first scripts", U"ppgb", 20140106)
INTRO (U"This page tells you how to create, run and save a script. "
	"To get a feel for how it works, you are advised to try out all the steps.")
ENTRY (U"1. A minimal script")
NORMAL (U"Suppose that you want to create a script that allows you to play a selected Sound object twice. "
	"You first create an empty script, by choosing @@New Praat script@ from the #Praat menu in the ##Praat Objects# window. "
	"A @ScriptEditor window will appear on your screen:")
SCRIPT (6, 4, U""
	Manual_DRAW_WINDOW (4, "untitled script", "File   Edit   Search   Convert   Font   Run   Help")
	"Draw rectangle: 0, 560, 0, 360\n"
	"info$ = Picture info\n"
	"fontSize = extractNumber (info$, \"Font size: \")\n"
	";Text: 50, \"centre\", 50, \"half\", string$ (fontSize)\n"
)
NORMAL (U"In this window, you type")
CODE (U"Play")
CODE (U"Play")
NORMAL (U"Now select a Sound in the ##Praat Objects# window. As you expect from selecting a Sound, a #Play button will "
	"appear in the dynamic menu. If you now choose #Run from the #Run menu in the ScriptEditor, Praat will play the sound twice. "
	"This works because #Play is a command that becomes available in the dynamic menu when you select a Sound.")
ENTRY (U"2. Some more commands")
NORMAL (U"In the above example, you could use the #Play command because that was "
	"the text on a button currently available in the dynamic menu. "
	"Apart from these selection-dependent (dynamic) commands, "
	"you can also use all fixed commands from the menus of the @@Object window@ "
	"and the @@Picture window@. For instance, try the following script:")
CODE (U"Erase all")
CODE (U"Draw inner box")
CODE (U"Play")
CODE (U"Play")
CODE (U"Erase all")
NORMAL (U"When you run this script, you'll see a rectangle appear in the ##Praat Picture# window "
	"(that's what the command ##Draw inner box# in the #Margins menu does), "
	"then you'll hear the Sound play tiwce, then you'll see the rectangle disappear from the Picture window "
	"(that's what the command ##Erase all# from the #Edit menu does).")
NORMAL (U"Here we see that the Praat scripting language is an example of a %%procedural programming language%, "
	"which means that the five %statements are executed in the order in which they appear in the script, "
	"i.e. first ##Erase all#, then ##Draw inner box#, then ##Play# twice, and finally ##Erase all#.")
ENTRY (U"3. Experimenting with your script")
NORMAL (U"You don't have to be afraid of making mistakes. Here are a couple that you can try to make.")
NORMAL (U"First, try to run the script when a Sound is not selected "
	"(e.g. you create a Pitch object from it and keep that selected, or you throw away the Sound). "
	"You will notice that Praat gives you an error message saying ##The command \"Play\" is not available for the current selection#. "
	"Indeed, if you select a Pitch or if you select nothing, then no command #Play appears in the dynamic menu, "
	"so the script cannot execute it. Note that the commands ##Erase all# and ##Draw inner box# are still available, "
	"because they continue to be present in the menus of the Picture window; "
	"therefore, the script will execute the first two lines ($$Erase all $and$$ Draw inner box$) "
	"and stop running at the third line, i.e. at your first$$ Play$. "
	"The result is that the \"box\" will stay visible in the Picture window, because the fifth line of the script, "
	"which should erase the box, is never executed.")
NORMAL (U"Second, try to mistype a command (there's a good chance you already did it by accident), "
	"e.g. write$$ PLay $instead of$$ Play$, or$$ Draw inner bocks $or whatever. "
	"Again, you are likely to get a message saying that that command is not available. "
	"Such messages are the most common messages that you'll see when writing scripts; "
	"now you know that they mean either that you mistyped something or that you made the wrong selection.")
ENTRY (U"4. Saving your script")
NORMAL (U"The #File menu of the ScriptEditor has a command #Save, "
	"with which you can save your script as a file on disk, for instance under the name$$ test.praat$.")
NORMAL (U"Please try this with the five-line script you just typed. "
	"After saving the script, the name of the script file will appear in the window title:")
SCRIPT (6, 4, U""
	Manual_DRAW_WINDOW (4, "Script \"/Users/Rose/Desktop/test.praat\"", "File   Edit   Search   Convert   Font   Run   Help")
	"Courier\n"
	"Text: 0, \"left\",  75, \"half\", \"\\s{Erase all}\"\n"
	"Text: 0, \"left\",  90, \"half\", \"\\s{Draw inner box}\"\n"
	"Text: 0, \"left\", 105, \"half\", \"\\s{Play}\"\n"
	"Text: 0, \"left\", 120, \"half\", \"\\s{Play}\"\n"
	"Text: 0, \"left\", 135, \"half\", \"\\s{Erase all}\"\n"
	"Draw rectangle: 0, 560, 0, 360\n"
)
NORMAL (U"After you save your script, you can close the ScriptEditor window without losing the script: "
	"you can reopen the script file by using @@Open Praat script...@ from the #Praat menu, "
	"or by choosing ##New Praat script# again, followed by ##Open...# from the ScriptEditor's #File menu.")
NORMAL (U"It advisable to use$$ .praat $as the extension for script file names. "
	"On the Mac, if you double-click a$$ .praat $file, Praat will automatically start up and show the script. "
	"On the Mac and on Windows, if you drag a$$ .praat $file on the Praat icon, Praat will also start up and show the script.")
MAN_END

MAN_BEGIN (U"Scripting 2. How to script settings windows", U"ppgb", 20140119)
INTRO (U"Not all menu commands are as simple as those on the @@Scripting 1. Your first scripts|previous page@, "
	"which act immediately once you choose them from a menu (e.g. ##Play#, ##Erase all#). "
	"Most commands in Praat require the user to supply additional information; "
	"these are the commands whose title ends in \"...\".")
NORMAL (U"For instance, when you select a Sound, the command ##Draw...# will appear in the #Draw menu, "
	"and when you click it, Praat will present you with a %%settings window%, "
	"which asks you to supply six pieces of additional information, i.e. six so-called %settings (or in programming jargon: %arguments):")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (= auto)")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN ("Garnish", 1)
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU ("Drawing method", "Curve")
)
NORMAL (U"In this example, all the settings have their standard values: you want to draw the whole time domain of the Sound, "
	"you want to have autoscaling vertically, you want to see garnishings around the picture (a box, labelled axes, and numbers), "
	"and you want the waveform to be drawn as a curve. Pressing the OK button in the above window is equivalent to executing the following script line:")
CODE (U"Draw: 0, 0, 0, 0, \"yes\", \"Curve\"")
NORMAL (U"You see that in a script, all of the arguments are supplied after the command, preceded by a colon and separated by commas, "
	"in the same order as in the settings window, counted from top to bottom (and, within a line, from left to right). "
	"The texts \"(= all)\" and \"(= auto)\" above are just Praat's explanations of what it means to type a zero in those fields "
	"(namely `draw all times' and `use vertical autoscaling', respectively); in a script they are superfluous and you shouldn't write them.")
NORMAL (U"If you want to draw the sound with different settings, say from 1 to 3.2 seconds, scaled between -1 and +1 instead of automatically, "
	"with garnishings off, and with the waveform drawn as poles, you would have the following settings window:")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), U""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw", 4)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "1.0", "3.2")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "-1", "1")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN ("Garnish", 0)
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU ("Drawing method", "Poles")
)
NORMAL (U"In a script this would look like")
CODE (U"Draw: 1.0, 3.2, -1, 1, \"no\", \"Poles\"")
ENTRY (U"1. Numeric arguments")
NORMAL (U"The first four arguments in the above examples are %%numeric arguments%: they are (real or integer) numbers. "
	"You just write them in the script as you would write them into the settings window.")
ENTRY (U"2. Boolean (yes/no) arguments")
NORMAL (U"The fifth argument in the above examples (#Garnish) is a %%boolean argument% (yes/no choice) and is represented by a %%check button%. "
	"In the script you write it as $$\"yes\"$ (including the quotes) or $$\"no\"$ (or as 1 or 0).")
ENTRY (U"3. Multiple-choice arguments")
NORMAL (U"The sixth argument in the above examples (##Drawing method#) is a %%multiple-choice argument% and is represented by an %%option menu%. "
	"In the script you write the text of the choice, i.e. $$\"Curve\"$ or $$\"Poles\"$ in the examples.")
NORMAL (U"A multiple choice argument is sometimes represented by a %%radio box% instead of by an option menu. "
	"For instance, the last example above could equally well have looked like")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (6.1), U""   // 7 - 3 * 0.3 (three is the number of additional radio buttons)
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw", 6.1)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "1.0", "3.2")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "-1", "1")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN ("Garnish", 0)
	Manual_DRAW_SETTINGS_WINDOW_RADIO ("Drawing method", "Curve", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_RADIO ("", "Bars", 0)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_RADIO ("", "Poles", 1)
	"y -= 12\n"
	Manual_DRAW_SETTINGS_WINDOW_RADIO ("", "Speckles", 0)
)
NORMAL (U"In supplying arguments to a command in a script, there is no difference between an option menu and a radio box. "
	"This last example will therefore again look like the following in a script:")
CODE (U"Draw: 1.0, 3.2, -1, 1, \"no\", \"Poles\"")
ENTRY (U"4. Text arguments")
NORMAL (U"Consider another frequently used menu command, namely ##Create Sound from formula...# in the #New menu:")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (6.6), U""
	Manual_DRAW_SETTINGS_WINDOW ("Create Sound from formula", 6.6)   // 0.6 extra for the text
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Name", "sine")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Number of channels", "1")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Start time (s)", "0.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("End time (s)", "1.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Sampling frequency (Hz)", "44100")
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("Formula", "1/2 * sin(2*pi*377*x)")
)
NORMAL (U"In a script this would look like:")
CODE (U"Create Sound from formula: \"sine\", 1, 0.0, 1.0, 44100, \"1/2 * sin(2*pi*377*x)\"")
NORMAL (U"Both the first argument (#Name) and the sixth argument (#Formula) are %%text arguments%. "
	"In a script they are written within quotes.")
ENTRY (U"5. File arguments")
NORMAL (U"The commands from the Open and Save menus, and several other commands whose names "
	"start with #Read, #Open, or #Save, present a %%file selector window% instead of a typical Praat "
	"settings window. File selector windows ask the user to supply a single argument: the file name.")
NORMAL (U"In a script you can either supply the %%complete path% to the file, or supply a %%relative path%.")
NORMAL (U"A complete path includes both the directory (folder) hierarchy and the name of the file. "
	"This goes slightly differently on the Windows platform on the one hand, "
	"and on the Mac and Linux platforms on the other. If your user name is Miep, "
	"and your home directory contains a folder #Sounds, "
	"and this folder contains a folder #Animals, and this contains the file ##miauw.wav#, "
	"you can open that file as follows:")
CODE (U"Read from file: \"C:/Users/Miep/Sounds/Animals/miauw.wav\"   ; Windows")
CODE (U"Read from file: \"/Users/Miep/Sounds/Animals/miauw.wav\"   ; Mac")
CODE (U"Read from file: \"/home/miep/Sounds/Animals/miauw.wav\"   ; Linux")
NORMAL (U"(the part before your user name may be slightly different on your computer; "
	"use your command or terminal window to find out)")
NORMAL (U"In these examples, \"C\" is the Windows %%drive letter% and "
	"##/Users/Miep# or ##/home/Miep# is your %%home directory%. Both the home directory and the drive letter "
	"can be abbreviated away by using the tilde (\"~\"):")
CODE (U"Read from file: \"~/Sounds/Animals/miauw.wav\"")
NORMAL (U"If your #Sounds folder is not in your home directory but on your desktop, you do")
CODE (U"Read from file: \"~/Desktop/Sounds/Animals/miauw.wav\"")
NORMAL (U"(this works because on all three platforms, the desktop folder is a subfolder of your home directory)")
NORMAL (U"If your Sounds folder is on a USB drive called PORCH, it would be something like:")
CODE (U"Read from file: \"G:/Sounds/Animals/miauw.wav\"   ; Windows")
CODE (U"Read from file: \"/Volumes/PORCH/Sounds/Animals/miauw.wav\"   ; Mac")
CODE (U"Read from file: \"/media/PORCH/Sounds/Animals/miauw.wav\"   ; Linux")
NORMAL (U"Instead of all these complete path names, you can use %relative path names. "
	"These are taken as relative to the directory in which your script resides, "
	"and help to make your script portable if you move the script along with your data.")
NORMAL (U"Thus, if your script (after you have saved it!) is in the #Animals folder mentioned above, "
	"i.e. in the same folder as ##miauw.wav#, you would simply open the file with")
CODE (U"Read from file: \"miauw.wav\"")
NORMAL (U"If your script is in the #Sounds folder mentioned above, "
	"i.e. in the same folder as where the #Animals folder is, you would open the file with")
CODE (U"Read from file: \"Animals/miauw.aifc\"")
NORMAL (U"If your script is in the folder #Scripts that is inside the #Sounds folder, "
	"i.e. if your script is a sister folder of the #Animals folder, you would open the file with")
CODE (U"Read from file: \"../Animals/miauw.aifc\"")
NORMAL (U"where \"..\" is the general way on all platforms to go one folder up in the hierarchy.")
NORMAL (U"Note that on Windows you could use the backslash (\"\\bs\") instead of the forward slash (\"/\"), "
	"but with the forward slash your script will work on all three platforms.")
ENTRY (U"6. How to supply arguments automatically")
NORMAL (U"Now you know all the ways to write the arguments of commands in a script line. "
	"If you dislike manually copying arguments from settings windows into your script, "
	"or if you are not sure whether something is a numeric or a string argument, "
	"you can use the @@history mechanism@: "
	"choose @@Clear history@ from the #Edit menu in your @ScriptEditor, "
	"click your command button, edit the arguments, and click #OK. The command will be executed. "
	"Then choose @@Paste history@, and the command line, including the arguments, "
	"will appear in the ScriptEditor at the position of the text cursor. You can build whole new scripts on the basis of this mechanism.")
MAN_END

MAN_BEGIN (U"Scripting 3. Simple language elements", U"ppgb", 20130421)
INTRO (U"The Praat scripting language doesn't only call the menu commands "
	"discussed in the @@Scripting 1. Your first scripts|first@ and @@Scripting 2. How to script settings windows|second@ chapters of this tutorial, "
	"it is only a general procedural programming language that allows you to compute numbers, handle texts, and make custom analyses.")
NORMAL (U"This chapter focuses on the things you need most. It is designed in such a way that you can work through it even if you haven't written computer programs before.")
LIST_ITEM (U"@@Scripting 3.1. Hello world@ (writeInfoLine, appendInfoLine)")
LIST_ITEM (U"@@Scripting 3.2. Numeric variables@ (assignments)")
LIST_ITEM (U"@@Scripting 3.3. Numeric queries")
LIST_ITEM (U"@@Scripting 3.4. String variables@ (assignments)")
LIST_ITEM (U"@@Scripting 3.5. String queries")
LIST_ITEM (U"@@Scripting 3.6. \"For\" loops@ (for, endfor)")
LIST_ITEM (U"@@Scripting 3.7. Layout@ (white space, comments, continuation lines)")
MAN_END

#define Manual_DRAW_PICTURE_WINDOW(height,vpLeft,vpRight,vpTop,vpBottom) \
	Manual_DRAW_WINDOW (height, "Praat Picture", "File   Edit   Margins   World   Select   Pen   Font   Help") \
	"worldHeight = " #height " - 1\n" \
	"Select inner viewport: 0.2, 5.8, 0.8, 0.8+worldHeight\n" \
	"Axes: 0, 5.6, worldHeight, 0\n" \
	"vpLeft = " #vpLeft "\nvpRight = " #vpRight "\nvpTop = " #vpTop "\nvpBottom = " #vpBottom "\n" \
	"Paint rectangle: \"Pink\", vpLeft, vpRight, vpTop, vpBottom\n" \
	"Paint rectangle: \"White\", vpLeft+0.69, vpRight-0.69, vpTop+0.46, vpBottom-0.46\n" \
	"Yellow\n" \
	"Draw line: 3, 0, 3, worldHeight\n" \
	"for i to worldHeight/3\n" \
	"   Draw line: 0, i*3, 5.6, i*3\n" \
	"Red\n" \
	"for i to 5\n" \
	"   Text special: i, \"centre\", 0, \"top\", \"Helvetica\", fontSize/1.2, \"0\", string$(i)\n" \
	"endfor\n" \
	"for i to worldHeight\n" \
	"   Text special: 0, \"left\", i, \"half\", \"Helvetica\", fontSize/1.2, \"0\", string$(i)\n" \
	"endfor\n" \
	"Black\n" \
	"Draw line: 0, 0, 5.6, 0\n" \

MAN_BEGIN (U"Scripting 3.1. Hello world", U"ppgb", 20140111)
INTRO (U"Many manuals of computer programming languages start with their answer on the following question:")
NORMAL (U"%%How do I write the text \"Hello world\" on the screen?")
NORMAL (U"For the Praat scripting language, there are two answers.")
ENTRY (U"1. \"Hello world\" in the Info window")
NORMAL (U"The simplest answer is that you open the ScriptEditor window with ##New Praat script# from the #Praat menu, "
	"then type the following line into the ScriptEditor window:")
CODE (U"writeInfoLine: \"Hello world\"")
NORMAL (U"and finally choose #Run from the #Run menu.")
NORMAL (U"When you try this, the result should be that the Info window comes to the front, and that it shows the text $$Hello world$:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{Hello world}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"Now suppose that you to write two lines instead of just one, so you try a script with two lines:")
CODE (U"writeInfoLine: \"Hello world\"")
CODE (U"writeInfoLine: \"How do you do?\"")
NORMAL (U"This turns out not to do what you want: it seems to write only the text $$How do you do?$. "
	"This happens because the #writeInfoLine function first erases the Info window, then writes the line of text. "
	"So the first line of the script did write the text $$Hello world$, but the second line wiped it out "
	"and wrote $$How do you do?$ instead. The script that does what you want is")
CODE (U"writeInfoLine: \"Hello world\"")
CODE (U"appendInfoLine: \"How do you do?\"")
NORMAL (U"Now the result will be")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{Hello world}\"\n"
	"Text: 0, \"left\", 90, \"half\", \"\\s{How do you do?}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This works because #appendInfoLine writes a line without erasing the Info window first.")
NORMAL (U"Finally, try the following script:")
CODE (U"appendInfoLine: \"Another try\"")
CODE (U"appendInfoLine: \"Goodbye\"")
NORMAL (U"The result could be")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{Hello world}\"\n"
	"Text: 0, \"left\", 90, \"half\", \"\\s{How do you do?}\"\n"
	"Text: 0, \"left\", 105, \"half\", \"\\s{Another try}\"\n"
	"Text: 0, \"left\", 120, \"half\", \"\\s{Goodbye}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"In other words, #appendInfoLine writes lines into the Info window without erasing it, even if you run a script anew. "
	"This is why many Praat scripts that write into the Info window do a #writeInfoLine first, and follow it with a series of #appendInfoLine calls.")
NORMAL (U"For more information on these commands, see @@Scripting 6.2. Writing to the Info window@.")
ENTRY (U"2. \"Hello world\" in the Picture window.")
NORMAL (U"You can also show text in the Picture window. If you are an experienced Praat user, you have probably used the comamnd ##Text top...# before. "
	"You can use it do draw a text at the top of the current %viewport, which is the part of the Picture window where the next drawing will occur "
	"and which is marked by the pink %margins. Thus, when you select the top 4\\xx3 inches of the Picture window (with the mouse), "
	"set the font size to 12 (with the #Pen menu), and run the script")
CODE (U"Text top: \"yes\", \"Hello world\"")
NORMAL (U"then you'll see")
SCRIPT (6, 4.5, U""
	Manual_DRAW_PICTURE_WINDOW (4.5, 0,4,0,3)
	"Select outer viewport: 0.2, 4.2, 0.8, 12\n" \
	"Times\n" \
	"Text top: \"yes\", \"Hello world\"\n" \
	"Select inner viewport: 0.2, 5.8, 0.2, 4.3\n"\
	"Axes: 0, 1, 0, 1\n" \
	"Draw rectangle: 0, 1, 0, 1\n"
)
NORMAL (U"So this works the same as when you choose ##Text top...# from the #Margins menu by hand, with #Far switched on.")
NORMAL (U"If you want your script to always show the same text at the same position, with nothing else in the picture, "
	"then you can make your script a bit more extensive:")
CODE (U"Erase all")
CODE (U"Times")
CODE (U"Font size: 12")
CODE (U"Select outer viewport: 0, 4, 0, 3")
CODE (U"Text top: \"yes\", \"Hello world\"")
NORMAL (U"In this script, line 1 erases the Picture window, so that nothing besides your text can appear in the Picture window.")
NORMAL (U"Line 2 executes the command #Times from the #Font menu, so that the script will always draw the text in Times, "
	"even if you choose #Helvetica in the #Font menu with the mouse before you run the script "
	"(after the script has run, you'll see that #Times is chosen in the #Font menu).")
NORMAL (U"Line 3 executes the command ##Font size...# from the #Font menu, setting the font size to 12 and setting the width of the pink margins "
	"accordingly.")
NORMAL (U"Line 4 executes the command @@Select outer viewport...@ from the #Select menu. "
	"This performs an action that you would normally do by dragging the mouse, "
	"namely selecting the part of the Picture window that runs from 0 to 4 inches horizontally "
	"and from 0 to 3 inches vertically. After running the script, "
	"the %viewport is indeed [0, 4] \\xx [0, 3], as you can clearly see "
	"from the pink margins above.")
NORMAL (U"Line 5 finally writes the text.")
NORMAL (U"For more information on these commands, see @@Picture window@.")
MAN_END

MAN_BEGIN (U"Scripting 3.2. Numeric variables", U"ppgb", 20130411)
INTRO (U"In any general procedural programming language you can work with %variables, "
	"which are places in your computer's memory where you can store a number or anything else.")
NORMAL (U"For instance, you could put the number 3.1 into the variable $b in the following way:")
CODE (U"b = 3.1")
NORMAL (U"This statement is called as %assignment, i.e., you %assign the %value 3.1 to the %variable $b. "
	"We read this statement aloud as \"b becomes 3.1\". "
	"What this means is that after this statement, the memory location $b %contains the numeric value (number) 3.1.")
NORMAL (U"You can regard a variable as a box: you put the value 3.1 into the box named $b. "
	"Or you can regard a variable as a house: the house is called $b and now the family \"3.1\" is living there. "
	"Or you can regard it as any other storage location.")
NORMAL (U"To see what value a variable contains (what's in the box, or who lives in the house), "
	"you can use the #writeInfoLine function:")
CODE (U"b = 3.1")
CODE (U"writeInfoLine: \"The value is \", b, \".\"")
NORMAL (U"This will put the text $$The value is 3.1.$ into the Info window, as you are invited to verify.")
NORMAL (U"A variable is called a variable because it is %variable, i.e. its value can change. Try the script")
CODE (U"b = 3.1")
CODE (U"b = 5.8")
CODE (U"writeInfoLine: \"The value is \", b, \".\"")
NORMAL (U"You will see that $b ends up having the value 5.8. The first line puts the value 3.1 there, but the second line "
	"replaces it with 5.8. It's like taking the 3.1 out of the box and putting the 5.8 in its stead. "
	"Or the family 3.1 moves from the house, and the family called 5.8 moves in.")
NORMAL (U"In an assignment, the part to the right of the \"becomes\" sign (the \"=\" sign) doesn't have to be a number; "
	"it can be any %formula that %evaluates to a number. For instance, the script")
CODE (U"b = 3.1 * 2")
CODE (U"writeInfoLine: \"The value is \", b, \".\"")
NORMAL (U"puts the text $$The value is 6.2.$ into the Info window. This works because Praat handles the first line "
	"in the following way:")
LIST_ITEM (U"1. the formula $$3.1 * 2$ is %evaluated (i.e. its value is computed), and the result is 6.2.")
LIST_ITEM (U"2. the value 6.2 is subsequently stored in the variable $b.")
NORMAL (U"After line 1 has been executed, the variable $b just contains the value 6.2, nothing more; "
	"the variable $b doesn't remember that that value has been computed by multiplying 3.1 with 2.")
NORMAL (U"Formulas can contain more things than numbers: they can also contain other variables:")
CODE (U"b = 3.1")
CODE (U"c = b * 2")
CODE (U"writeInfoLine: \"The value of b is \", b, \", and the value of c is \", c, \".\"")
NORMAL (U"In the first line, $b gets the value 3.1. In the second line, the formula $$b * 2$ first has to be evaluated. "
	"Praat looks up the value of $b (which is 3.1), so that it knows that the formula actually means $$3.1 * 2$. "
	"Praat evaluates this formula and stores the result (namely the value 6.2) "
	"into the variable $c, which will then contain nothing else than the value 6.2. "
	"The Info window thus reports $$The value of b is 3.1, and the value of c is 6.2.$.")
NORMAL (U"After these explanations, consider the following script:")
CODE (U"b = 3.1")
CODE (U"c = b * 2")
CODE (U"b = 5.8")
CODE (U"writeInfoLine: \"The value of c is \", c, \".\"")
NORMAL (U"Can you figure out what the Info will report? If you think it will report "
	"$$The value of c is 6.2.$, then you are correct: after the first line, $b contains the value 3.1; "
	"after the second line, the value of $c is therefore 6.2, and nothing more; "
	"after line 3, the value of $b has changed to 5.8, but the value of $c hasn't changed and is still 6.2.")
NORMAL (U"If you thought that $c would end up having the value 11.6, then you're thinking in terms "
	"of a non-procedural language such as Prolog; you may have thought that the thing assigned to $c in the second line "
	"is the whole %formula $$b * 2$, so that $c changes when $b changes. But this is not the case: "
	"the thing stored in $c is just the %value of the formula $$b * 2$ at that moment, which is 6.2, "
	"and $c doesn't remember how it got that value. If you have trouble understanding this, "
	"consult anybody who writes programs.")
MAN_END

MAN_BEGIN (U"Scripting 3.3. Numeric queries", U"ppgb", 20140111)
INTRO (U"Now that you know how to script a menu command, and you know how variables work, "
	"you are ready to combine the two.")
NORMAL (U"Suppose you have selected a Sound in the object list. One of the commands available in the #Query menu "
	"is ##Get power...#. When you choose it, you will see the following settings window:")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (1), U""   // 7 - 3 * 0.3 (three is the number of additional radio buttons)
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Get power", 1)
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Time range (s)", "0.0", "0.0 (= all)")
)
NORMAL (U"When you click OK, something like the following will appear in the Info window:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{0.1350605005239421 Pa2}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This is the mean power of the whole Sound.")
NORMAL (U"In a script, you want to use the value of this power in the script itself, not in the Info window, "
	"perhaps because you want to do computations with it or because you want to report the value with a nice text around it. "
	"This is how you do the latter:")
CODE (U"power = Get power: 0.0, 0.0")
CODE (U"writeInfoLine: \"The power of this sound is \", power, \" Pascal-squared.\"")
NORMAL (U"The first line of this script executes the menu command ##Get power...#, "
	"but puts the value 0.1350605005239421 into the variable $power instead of into the Info window "
	"(the variable can have any name you like, as long as it starts with a lower-case letter "
	"and consists of letters and digits; see @@Scripting 5.1. Variables@).")
NORMAL (U"The second line then reports the value in the Info window, this time with a nice text around it:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{The power of this sound is 0.1350605005239421 Pascal-squared.}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
MAN_END

MAN_BEGIN (U"Scripting 3.4. String variables", U"ppgb", 20130411)
INTRO (U"Just as you can store @@Scripting 3.2. Numeric variables|numeric variables@, "
	"you can store %%string variables%, which contain text instead of numbers. Here is an example:")
CODE (U"word1\\$  = \"Hello\"")
CODE (U"word2\\$  = \"world\"")
CODE (U"sentence\\$  = word1\\$  + \" \" + word2\\$ ")
CODE (U"writeInfoLine: \"The whole sentence is: \", sentence\\$ ")
NORMAL (U"Yes, this is another way to get the sentence $$Hello world$ into the Info window. "
	"It's a more linguistically valid way to do it, and here is how it works:")
LIST_ITEM (U"1. In line 1, the value \"Hello\", which is a text (as we can see by its use of quotes), "
	"is stored into the variable $$word1\\$ $, which is a string variable (as we can see because its name ends in a dollar sign).")
LIST_ITEM (U"2. In line 2, the text value \"world\" is stored into the string variable $$word2\\$ $.")
LIST_ITEM (U"3. In line 3, we have the formula $$word1\\$  + \" \" + word2\\$ $, which contains two variables, "
	"namely $$word1\\$ $ and $$word2\\$ $.")
LIST_ITEM (U"4. The values of the two variables are \"Hello\" and \"world\", respectively, "
	"so what the formula actually says is \"Hello\" + \" \" + \"world\".")
LIST_ITEM (U"5. The pluses in the formula mean \"concatenate\", so we concatenate the three strings "
	"\"Hello\", \" \", and \"world\", giving the longer string \"Hello world\".")
LIST_ITEM (U"6. Still in line 3, the string value \"Hello world\" is assigned to the string variable $$sentence\\$ $.")
LIST_ITEM (U"7. Line 4 reports in the Info window: $$The whole sentence is: Hello world$")
MAN_END

MAN_BEGIN (U"Scripting 3.5. String queries", U"ppgb", 20140111)
INTRO (U"Just as you can use menu commands (usually in a #Query menu) to query @@Scripting 3.3. Numeric queries|numbers@, "
	"you can query texts as well.")
NORMAL (U"For instance, when you select a Textgrid, the #Query menu will contain the command ##Get label of interval...#, "
	"which takes two numeric arguments, namely ##Tier number# and ##Interval number#:")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (2), U""
	Manual_DRAW_SETTINGS_WINDOW ("TextGrid: Get label of interval", 2)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Tier number", "1")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Interval number", "3")
)
NORMAL (U"When you click OK, and interval 3 of tier 1 happens to contain the text $$hello$, the following appears in the Info window:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{hello}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"In a script, you will want to put the result of the query in a string variable instead of in the Info window, "
	"because you want to manipulate it further:")
CODE (U"text\\$  = Get label of interval: 1, 3")
CODE (U"writeInfoLine: \"The text in interval 3 of tier 1 is: \", text\\$ ")
NORMAL (U"The script first stores the text of the interval, i.e. $$hello$, into the variable ##text\\$ #, "
	"then writes it, preceded by some informative text, into the Info window:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{The text in interval 3 of tier 1 is: hello}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"Hey, yet another way to implement \"Hello world\" with the Praat scripting language!")
ENTRY (U"The difference between numeric queries and string queries")
NORMAL (U"A string query stores in a string variable the whole text that would appear in the Info window. "
	"For instance, the script")
CODE (U"power\\$  = Get power: 0.0, 0.0")
CODE (U"writeInfoLine: power\\$ ")
NORMAL (U"could give you the following result:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{0.1350605005239421 Pa2}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"A numeric query stores in a numeric variable only the first number that it can find in the text that would appear in the Info window. "
	"For instance, the script")
CODE (U"power = Get power: 0.0, 0.0")
CODE (U"writeInfoLine: power")
NORMAL (U"could give you the following result:")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{0.1350605005239421}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
MAN_END

MAN_BEGIN (U"Scripting 3.6. \"For\" loops", U"ppgb", 20140111)
INTRO (U"The power of a procedural programming language is most easily illustrated with the %%for-loop%.")
NORMAL (U"Take the example of the @@Scripting 3.5. String queries|previous page@, "
	"whereas you wanted to know the text in the third interval of the first tier of a selected TextGrid. "
	"It's easy to imagine that you actually want the texts of %%all the first five% intervals. "
	"With knowledge from the previous sections, you could write it like this:")
CODE (U"writeInfoLine: \"The texts in the first five intervals:\"")
CODE (U"text\\$  = Get label of interval: 1, 1")
CODE (U"appendInfoLine: \"Interval 1: \", text\\$ ")
CODE (U"text\\$  = Get label of interval: 1, 2")
CODE (U"appendInfoLine: \"Interval 2: \", text\\$ ")
CODE (U"text\\$  = Get label of interval: 1, 3")
CODE (U"appendInfoLine: \"Interval 3: \", text\\$ ")
CODE (U"text\\$  = Get label of interval: 1, 4")
CODE (U"appendInfoLine: \"Interval 4: \", text\\$ ")
CODE (U"text\\$  = Get label of interval: 1, 5")
CODE (U"appendInfoLine: \"Interval 5: \", text\\$ ")
NORMAL (U"The result will be something like")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{The texts in the first five intervals:}\"\n"
	"Text: 0, \"left\", 90, \"half\", \"\\s{Interval 1: I}\"\n"
	"Text: 0, \"left\", 105, \"half\", \"\\s{Interval 2: say}\"\n"
	"Text: 0, \"left\", 120, \"half\", \"\\s{Interval 3: hello}\"\n"
	"Text: 0, \"left\", 135, \"half\", \"\\s{Interval 4: and}\"\n"
	"Text: 0, \"left\", 150, \"half\", \"\\s{Interval 5: you}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This can be done more nicely. The first step is to realize that the sentences starting with $$text\\$ $ are similar to each other, "
	"and the sentence starting with $appendInfoLine are also similar to each other. They only differ in the interval number, "
	"and can therefore be made %identical by using a variable for the interval number, like this:")
CODE (U"writeInfoLine: \"The texts in the first five intervals:\"")
CODE (U"intervalNumber = 1")
CODE (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: \"Interval \", intervalNumber, \": \", text\\$ ")
CODE (U"intervalNumber = 2")
CODE (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: \"Interval \", intervalNumber, \": \", text\\$ ")
CODE (U"intervalNumber = 3")
CODE (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: \"Interval \", intervalNumber, \": \", text\\$ ")
CODE (U"intervalNumber = 4")
CODE (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: \"Interval \", intervalNumber, \": \", text\\$ ")
CODE (U"intervalNumber = 5")
CODE (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE (U"appendInfoLine: \"Interval \", intervalNumber, \": \", text\\$ ")
NORMAL (U"A new trick that you see here is that as a numeric argument (##Interval number#, "
	"the second argument to ##Get label of interval...#), you can use not only a number "
	"(as in all previous examples), but also a variable ($intervalNumber). "
	"The rest of the script should be known stuff by now.")
NORMAL (U"The script above is long, but it can be made much shorter with the use of a %%for-loop%:")
CODE (U"writeInfoLine: \"The texts in the first five intervals:\"")
CODE (U"for intervalNumber from 1 to 5")
CODE1 (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE1 (U"appendInfoLine: \"Interval \", intervalNumber, \": \", text\\$ ")
CODE (U"endfor")
NORMAL (U"The two lines that were repeated five times in the previous version now show up with indentation "
	"between a %for line and its corresponding %endfor. Those two lines (the $$text\\$ $ and the $appendInfoLine line) "
	"are executed five times: for $intervalNumber equal to 1, for $intervalNumber equal to 2, for $intervalNumber equal to 3, "
	"for $intervalNumber equal to 4, and for $intervalNumber equal to 5, in that order.")
NORMAL (U"In the above example, using a loop does not do much more than save eight lines, at the cost of adding two new lines. "
	"But imagine the case in which you want to list %all the texts in the intervals: "
	"the version without the loop is no longer possible. By contrast, the version %with the loop is still possible, "
	"because we have the command ##Get number of intervals...#, which gives us the number of intervals in the specified tier "
	"(here, tier 1). So you do:")
CODE (U"numberOfIntervals = Get number of intervals: 1")
CODE (U"writeInfoLine: \"The texts in all \", numberOfIntervals, \" intervals:\"")
CODE (U"for intervalNumber from 1 to numberOfIntervals")
CODE1 (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE1 (U"appendInfoLine: \"Interval \", intervalNumber, \": \", text\\$ ")
CODE (U"endfor")
NORMAL (U"This may yield something like")
SCRIPT (6, 3, U""
	Manual_DRAW_WINDOW (3, "Praat Info", "File   Edit   Search   Convert   Font   Help")
	"Courier\n"
	"Text: 0, \"left\", 75, \"half\", \"\\s{The texts in all 7 intervals:}\"\n"
	"Text: 0, \"left\", 90, \"half\", \"\\s{Interval 1: I}\"\n"
	"Text: 0, \"left\", 105, \"half\", \"\\s{Interval 2: say}\"\n"
	"Text: 0, \"left\", 120, \"half\", \"\\s{Interval 3: hello}\"\n"
	"Text: 0, \"left\", 135, \"half\", \"\\s{Interval 4: and}\"\n"
	"Text: 0, \"left\", 150, \"half\", \"\\s{Interval 5: you}\"\n"
	"Text: 0, \"left\", 165, \"half\", \"\\s{Interval 6: say}\"\n"
	"Text: 0, \"left\", 180, \"half\", \"\\s{Interval 7: goodbye}\"\n"
	"Draw rectangle: 0, 560, 0, 260\n"
)
NORMAL (U"This is the first script in this tutorial that is useful in itself. On the basis of it "
	"you can create all kinds of ways to list the texts in intervals. Here is how you would also list the durations "
	"of those intervals:")
CODE (U"numberOfIntervals = Get number of intervals: 1")
CODE (U"writeInfoLine: \"The durations and texts in all \", numberOfIntervals, \" intervals:\"")
CODE (U"for intervalNumber from 1 to numberOfIntervals")
CODE1 (U"startTime = Get start point: 1, intervalNumber")
CODE1 (U"endTime = Get end point: 1, intervalNumber")
CODE1 (U"duration = endTime - startTime")
CODE1 (U"text\\$  = Get label of interval: 1, intervalNumber")
CODE1 (U"appendInfoLine: \"Interval \", intervalNumber, \" is \", duration, \" seconds long and contains the text: \", text\\$ ")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Scripting 3.7. Layout", U"ppgb", 20140111)
INTRO (U"This chapter handles the way you use white space, comments, and continuation lines in a Praat script.")
ENTRY (U"White space")
NORMAL (U"Praat ignores all white space (spaces and tabs) that you put at the beginning of lines. The indentation "
	"that you saw on the @@Scripting 3.6. \"For\" loops|previous page@ was therefore used solely for readability. "
	"You are advised to use indenting, though, with three or four spaces for each level, "
	"as in the following example, which loops over all tiers and intervals of a TextGrid:")
CODE (U"writeInfoLine: \"The texts in all tiers and intervals:\"")
CODE (U"numberOfTiers = Get number of tiers")
CODE (U"for tierNumber from 1 to numberOfTiers")
CODE1 (U"numberOfIntervals = Get number of intervals: tierNumber")
CODE1 (U"for intervalNumber from 1 to numberOfIntervals")
CODE2 (U"text\\$  = Get label of interval: tierNumber, intervalNumber")
CODE2 (U"appendInfoLine: \"Tier \", tierNumber, \", interval \", intervalNumber, \": \", text\\$ ")
CODE1 (U"endfor")
CODE (U"endfor")
NORMAL (U"Praat also ignores lines that are empty or consist solely of white space, "
	"so you use those to structure your script visually.")
ENTRY (U"Comments")
NORMAL (U"Comments are lines that start with \"\\# \" or \";\". Praat ignores these lines when your script is running:")
CODE (U"\\#  Create 1 second of a sine wave with a frequency of 100 Hertz,")
CODE (U"\\#  sampled at 44100 Hz:")
CODE (U"Create Sound from formula: \"sine\", 1, 0, 1, 44100, \"sin (2*pi*100*x)\"")
NORMAL (U"Because of its visibility, you are advised to use \"\\# \" for comments that structure your script, "
	"and \";\" perhaps only for \"commenting out\" a statement, i.e. to temporarily put it before a line "
	"that you don't want to execute.")
ENTRY (U"Continuation lines")
NORMAL (U"There is normally one line per statement, and one statement per line. But some statements are very long, "
	"such as this one on a previous page:")
CODE1 (U"appendInfoLine: \"Interval \", intervalNumber, \" is \", duration, \" seconds long and contains the text: \", text\\$ ")
NORMAL (U"By making the current window wider, you can see that I really put this whole statement on a single line. "
	"I could have distributed it over two lines in the following way, by using three dots (an %ellipsis):")
CODE1 (U"appendInfoLine: \"Interval \", intervalNumber, \" is \", duration, \" seconds long")
CODE1 (U"... and contains the text: \", text\\$ ")
NORMAL (U"Here is another common type of example:")
CODE (U"Create Sound from formula: \"windowedSine\", 1, 0, 1, 44100,")
CODE (U"... \"0.5 * sin(2*pi*1000*x) * exp(-0.5*((x-0.5)/0.1)\\^ 2)\"")
NORMAL (U"You will normally want to follow such an ellipsis with a space, unless you want to concatenate "
	"the parts of a long word:")
CODE (U"Select outer viewport: 0, 10, 0, 4")
CODE (U"Text top: \"yes\", \"It's a long way to Llanfairpwllgwyngyll")
CODE (U"...gogerychwyrndrobwllllantysiliogogogoch,")
CODE (U"... unless you start from Tyddyn-y-felin.\"")
MAN_END

MAN_BEGIN (U"Scripting 4. Object selection", U"ppgb", 20130501)
INTRO (U"This chapter is about how to select objects from your script, "
	"and how to find out what objects are currently selected.")
LIST_ITEM (U"@@Scripting 4.1. Selecting objects")
LIST_ITEM (U"@@Scripting 4.2. Removing objects")
LIST_ITEM (U"@@Scripting 4.3. Querying objects")
MAN_END

MAN_BEGIN (U"Scripting 4.1. Selecting objects", U"ppgb", 20140223)
NORMAL (U"To simulate the mouse-clicked and dragged selection in the list of objects, "
	"you have the commands #selectObject, #plusObject and #minusObject.")
NORMAL (U"Suppose you start Praat and use ##Create Sound as tone...# to create a Sound called %tone. "
	"In the object list it looks like \"1. Sound tone\". "
	"Suppose you then do ##To Spectrum...# from the ##Analyse Spectrum# menu. "
	"A second object, called \"2. Spectrum tone\" appears in the list and is selected. "
	"To select and play the Sound, you can do either")
CODE (U"#selectObject: 1")
CODE (U"Play")
NORMAL (U"or")
CODE (U"#selectObject: \"Sound tone\"")
CODE (U"Play")
NORMAL (U"So you can select an object either by its unique ID (identifier: the unique number by which it appears in the list) "
	"or by name.")
NORMAL (U"The function #selectObject works by first deselecting all objects, and then selecting the one you mention. "
	"If you don't want to deselect the existing selection, you can use #plusObject or #minusObject. "
	"When the Sound is selected, you can select the Spectrum as well by doing")
CODE (U"#plusObject: 2")
NORMAL (U"or")
CODE (U"#plusObject: \"Spectrum tone\"")
NORMAL (U"If you then want to deselect the Sound, and keep the Spectrum selected, you can do")
CODE (U"#minusObject: 1")
NORMAL (U"or")
CODE (U"#minusObject: \"Sound tone\"")
NORMAL (U"All these functions can take more than one argument. To select the Sound and the Spectrum together, you can do")
CODE (U"#selectObject: 1, 2")
NORMAL (U"or")
CODE (U"#selectObject: \"Sound tone\", \"Spectrum tone\"")
NORMAL (U"or even")
CODE (U"#selectObject: 1, \"Spectrum tone\"")
ENTRY (U"How to refer to objects created in your script")
NORMAL (U"In a script, you typically don't know whether the IDs of the objects are 1 and 2, or much higher numbers. "
	"Fortunately, commands that create a new object give you the ID of the object that is created, "
	"so that you can refer to the object later on. For instance, suppose you want to generate a sine wave, play it, "
	"draw its spectrum, and then throw away both the Sound and the Spectrum. Here is how you do it:")
CODE (U"sound = Create Sound as pure tone: \"sine377\",")
CODE (U"... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; remember the ID of the Sound")
CODE (U"Play   ; the Sound is selected, so it plays")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Draw: 0, 5000, 20, 80, \"yes\"   ; the Spectrum is selected, so it is drawn")
CODE (U"\\#  Remove the created Spectrum and Sound:")
CODE (U"#plusObject: sound   ; the Spectrum was already selected")
CODE (U"Remove")
NORMAL (U"You could also select the objects by name:")
CODE (U"Create Sound as pure tone: \"sine377\",")
CODE (U"... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; no need to remember the ID of the Sound")
CODE (U"Play   ; the Sound is selected, so it plays")
CODE (U"To Spectrum: \"yes\"")
CODE (U"Draw: 0, 5000, 20, 80, \"yes\"   ; the Spectrum is selected, so it is drawn")
CODE (U"\\#  Remove the created Spectrum and Sound:")
CODE (U"#plusObject: \"Sound sine377\"   ; the Spectrum was already selected")
CODE (U"Remove")
NORMAL (U"This works even if there are multiple objects called \"Sound sine377\", "
	"because if there are more objects with the same name, #selectObject and #plusObject select the most recently created one, "
	"i.e., the one nearest to the bottom of the list of objects.")
MAN_END

MAN_BEGIN (U"Scripting 4.2. Removing objects", U"ppgb", 20140111)
NORMAL (U"In @@Scripting 4.1. Selecting objects|\\SS4.1@ we saw that objects could be removed by selecting them first and then calling the #Remove command. "
	"A faster way is the #removeObject function, which can also remove unselected objects:")
CODE (U"sound = Create Sound as pure tone: \"sine377\",")
CODE (U"... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; remember the ID of the Sound")
CODE (U"Play   ; the Sound is selected, so it plays")
CODE (U"spectrum = To Spectrum: \"yes\"   ; remember the ID of the Spectrum")
CODE (U"Draw: 0, 5000, 20, 80, \"yes\"   ; the Spectrum is selected, so it is drawn")
CODE (U"\\#  Remove the created Spectrum and Sound:")
CODE (U"#removeObject: sound, spectrum   ; remove one selected and one unselected object")
NORMAL (U"The #removeObject function keeps the objects selected that were selected before "
	"(except of course the ones it throws away). "
	"This allows you to easily throw away objects as soon as you no longer need them:")
CODE (U"sound = Create Sound as pure tone: \"sine377\",")
CODE (U"... 1, 0, 1, 44100, 377, 0.2, 0.01, 0.01   ; remember the ID of the Sound")
CODE (U"Play   ; the Sound is selected, so it plays")
CODE (U"spectrum = To Spectrum: \"yes\"")
CODE (U"#removeObject: sound   ; we no longer need the Sound, so we remove it")
CODE (U"Draw: 0, 5000, 20, 80, \"yes\"   ; the Spectrum is still selected, so it is drawn")
CODE (U"#removeObject: spectrum   ; remove the last object created by the script")
ENTRY (U"Selecting and removing all objects from the list (don't)")
NORMAL (U"A very strange command, which you should not normally use, is ##select all#:")
CODE1 (U"##select all")
CODE1 (U"Remove")
NORMAL (U"This selects all objects in the list and then removes them. "
	"Please try not to use this, because it will remove even the objects that your script did not create! "
	"After all, you don't want the users of your script to lose the objects they created! "
	"So please try to remove in your script only the objects that your script created, "
	"even if the script is for your own use (because if it is a nice script, others will want to use it).")
MAN_END

MAN_BEGIN (U"Scripting 4.3. Querying objects", U"ppgb", 20140111)
NORMAL (U"You can get the name of a selected object into a string variable. "
	"For instance, the following reads the name of the second selected Sound "
	"(as counted from the top of the list of objects) into the variable %name\\$ :")
CODE (U"name\\$  = ##selected\\$ # (\"Sound\", 2)")
NORMAL (U"If the Sound was called \"Sound hallo\", the variable %name\\$  will contain the string \"hallo\". "
	"To get the name of the topmost selected Sound object, you can leave out the number:")
CODE (U"name\\$  = ##selected\\$ # (\"Sound\")")
NORMAL (U"To get the full name (type + name) of the third selected object, you do:")
CODE (U"fullName\\$  = ##selected\\$ # (3)")
NORMAL (U"To get the full name of the topmost selected object, you do:")
CODE (U"fullName\\$  = ##selected\\$ # ()")
NORMAL (U"To get the type and name out of the full name, you do:")
CODE (U"type\\$  = extractWord\\$  (fullName\\$ , \"\")")
CODE (U"name\\$  = extractLine\\$  (fullName\\$ , \" \")")
NORMAL (U"Negative numbers count from the bottom. Thus, to get the name of the bottom-most selected Sound "
	"object, you say")
CODE (U"name\\$  = ##selected\\$ # (\"Sound\", -1)")
NORMAL (U"You would use ##selected\\$ # for drawing the object name in a picture:")
CODE (U"Draw: 0, 0, 0, 0, \"yes\"")
CODE (U"name\\$  = ##selected\\$ # (\"Sound\")")
CODE (U"Text top: \"no\", \"This is sound \" + name\\$ ")
NORMAL (U"For identifying previously selected objects, this method is not very suitable, since "
	"there may be multiple objects with the same name:")
CODE (U"\\#  The following two lines are OK:")
CODE (U"soundName\\$  = ##selected\\$ # (\"Sound\", -1)")
CODE (U"pitchName\\$  = ##selected\\$ # (\"Pitch\")")
CODE (U"\\#  But the following line is questionable, since it doesn't")
CODE (U"\\#  necessarily select the previously selected Pitch again:")
CODE (U"#selectObject: \"Pitch \" + pitchName\\$ ")
NORMAL (U"Instead of this error-prone approach, you should get the object's unique ID. "
	"The correct version of our example becomes:")
CODE (U"sound = #selected (\"Sound\", -1)")
CODE (U"pitch = #selected (\"Pitch\")")
CODE (U"\\#  Correct:")
CODE (U"#selectObject: pitch")
NORMAL (U"To get the number of selected Sound objects into a variable, use")
CODE (U"numberOfSelectedSounds = #numberOfSelected (\"Sound\")")
NORMAL (U"To get the number of selected objects into a variable, use")
CODE (U"numberOfSelectedObjects = #numberOfSelected ()")
ENTRY (U"Example: doing something to every selected Sound")
CODE (U"n = #numberOfSelected (\"Sound\")")
CODE (U"#for i to n")
	CODE1 (U"sound [i] = #selected (\"Sound\", i)")
CODE (U"#endfor")
CODE (U"\\#  Median pitches of all selected sounds:")
CODE (U"#for i to n")
	CODE1 (U"#selectObject: sound [i]")
	CODE1 (U"To Pitch: 0.0, 75, 600")
	CODE1 (U"f0 = Get quantile: 0, 0, 0.50, \"Hertz\"")
	CODE1 (U"appendInfoLine: f0")
	CODE1 (U"Remove")
CODE (U"#endfor")
CODE (U"\\#  Restore selection:")
CODE (U"#selectObject ( )   ; deselect all objects")
CODE (U"#for i from 1 to n")
	CODE1 (U"#plusObject: sound [i]")
CODE (U"#endfor")
MAN_END

MAN_BEGIN (U"Scripting 5. Language elements reference", U"ppgb", 20170718)
NORMAL (U"In a Praat script, you can use variables, expressions, and functions, of numeric as well as string type, "
	"and most of the control structures known from other procedural computer languages. "
	"The way the distinction between numbers and strings is made, may remind you of the programming language Basic.")
LIST_ITEM (U"@@Scripting 5.1. Variables@ (numeric, string)")
LIST_ITEM (U"@@Scripting 5.2. Expressions@ (numeric, string)")
LIST_ITEM (U"@@Scripting 5.3. Jumps@ (if, then, elsif, else, endif)")
LIST_ITEM (U"@@Scripting 5.4. Loops@ (for/endfor, while/endwhile, repeat/until)")
LIST_ITEM (U"@@Scripting 5.5. Procedures@ (\\@ , procedure)")
LIST_ITEM (U"@@Scripting 5.6. Arrays and dictionaries@")
LIST_ITEM (U"@@Scripting 5.7. Vectors and matrices")
LIST_ITEM (U"@@Scripting 5.8. Including other scripts@")
LIST_ITEM (U"@@Scripting 5.9. Quitting@ (exit)")
MAN_END

MAN_BEGIN (U"Scripting 5.1. Variables", U"ppgb", 20140111)
INTRO (U"A %variable is a location in your computer's memory that has a name and where you can store something, "
	"as explained in @@Scripting 3.2. Numeric variables|\\SS3.2@ and @@Scripting 3.4. String variables|\\SS3.4@. "
	"In a Praat script, you can store numbers and texts, i.e. you can use %%numeric variables% and %%string variables%.")
ENTRY (U"Numeric variables")
NORMAL (U"Numeric variables can hold integer numbers between -1,000,000,000,000,000 and +1,000,000,000,000,000 "
	"or real numbers between -10^^308^ and +10^^308^. The smallest numbers lie near -10^^-308^ and +10^^-308^.")
NORMAL (U"You use numeric variables in your script like this:")
CODE (U"#length = 10")
CODE (U"Draw line: 0, #length, 1, 1")
NORMAL (U"This draws a line in the Picture window from position (0, 10) to position (1, 1). "
	"In the first line, you assign the value 10 to the variable called %length, "
	"and in the second line you use the value of %length as the second argument to the command \"Draw line...\".")
NORMAL (U"Names of numeric variables must start with a lower-case letter, optionally followed by a sequence "
	"of letters, digits, and underscores.")
ENTRY (U"String variables")
NORMAL (U"You use string variables, which contain text, as follows:")
CODE (U"##title\\$ # = \"Dutch nasal place assimilation\"")
CODE (U"Text top: \"yes\", ##title\\$ #")
NORMAL (U"This writes the text \"Dutch nasal place assimilation\"")
NORMAL (U"As in the programming language Basic, the names of string variables end in a dollar sign.")
ENTRY (U"Making numeric variables visible")
NORMAL (U"You can write the content of numeric variables directly to the info window:")
CODE (U"x = 2.0")
CODE (U"root = sqrt (x)")
CODE (U"#writeInfoLine: \"The square root of \", x, \" is \", root, \".\"")
NORMAL (U"This will write the following text to the Info window:")
CODE (U"The square root of 2 is 1.4142135623730951.")
NORMAL (U"You can fix the number of digits after the decimal point by use of the ##fixed\\$ # function:")
CODE (U"x = 2.0")
CODE (U"root = sqrt (x)")
CODE (U"writeInfoLine: \"The square root of \", ##fixed\\$ # (x, 3), \" is approximately \", ##fixed\\$ # (root, 3), \".\"")
NORMAL (U"This will write the following text to the Info window:")
CODE (U"The square root of 2.000 is approximately 1.414.")
NORMAL (U"By using 0 decimal digits, you round to whole values:")
CODE (U"root = sqrt (2)")
CODE (U"writeInfoLine: \"The square root of 2 is very approximately \", ##fixed\\$ # (root, #0), \".\"")
NORMAL (U"This will write the following text to the Info window:")
CODE (U"The square root of 2 is very approximately 1.")
NORMAL (U"By using the ##percent\\$ # function, you give the result in a percent format:")
CODE (U"jitter = 0.0156789")
CODE (U"writeInfoLine: \"The jitter is \", ##percent\\$ # (jitter, 3), \".\"")
NORMAL (U"This will write the following text to the Info window:")
CODE (U"The jitter is 1.568\\% .")
NORMAL (U"The number 0, however, will always be written as 0, and for small numbers the number of "
	"significant digits will never be less than 1:")
CODE (U"jitter = 0.000000156789")
CODE (U"writeInfoLine: \"The jitter is \", percent\\$  (jitter, 3), \".\"")
NORMAL (U"This will write the following text to the Info window:")
CODE (U"The jitter is 0.00002\\% .")
ENTRY (U"Predefined variables")
NORMAL (U"All of the variables you saw earlier in this tutorial were defined at the first moment a value was assigned to them. "
	"Some variables, however, are already defined implicitly at the start of your script.")
#define xstr(s) str(s)
#define str(s) #s
NORMAL (U"Some predefined ##numeric variables# are $macintosh, $windows, and $unix, which are 1 if the script "
	"is running on a Macintosh, Windows, or Unix platform (respectively), and which are otherwise zero. "
	"Another one is $praatVersion, which is e.g. " xstr(PRAAT_VERSION_NUM) " for the current version of Praat.")
NORMAL (U"Some ##predefined string variables# are $$newline\\$ $,  $$tab\\$ $, and $$shellDirectory\\$ $. "
	"The last one specifies the directory that was the default directory when Praat started up; "
	"you can use it in scripts that run from the Unix or Windows command line. "
	"Likewise, there exist the predefined string variables $$homeDirectory\\$ $, "
	"$$preferencesDirectory\\$ $, and $$temporaryDirectory\\$ $. These three refer to your home directory "
	"(which is where you log in), the Praat @@preferences directory@, and a directory for saving temporary files; "
	"if you want to know what they are on your computer, try to write them into a script window. "
	"The variable $$defaultDirectory\\$ $ is available for formulas in scripts; it is the directory that contains the script file. "
	"Finally, we have $$praatVersion\\$ $, which is \"" xstr(PRAAT_VERSION_STR) "\" for the current version of Praat.")
ENTRY (U"Functions that handle variables")
NORMAL (U"To check whether a variable exists, you can use the function")
CODE (U"%variableExists (%%variableName\\$ %)")
MAN_END
/*
form Convert from WAV to AIFF
   text fileName hello.wav
endform
fileName$ = fileName$ - ".wav"
Read from file: shellDirectory$ + \"/\" + fileName$ + \".wav\"
Save as AIFF file: shellDirectory$ + \"/\" + fileName$ + \".aiff\"

if left$ (fileName$) <> "/"
   fileName$ = shellDirectory$ + \"/\" + fileName$
endif
*/

MAN_BEGIN (U"Scripting 5.2. Expressions", U"ppgb", 20140111)
INTRO (U"In a Praat script, you can use numeric expressions as well as string expressions.")
ENTRY (U"Numeric expressions")
NORMAL (U"You can use a large variety of @@Formulas@ in your script:")
CODE (U"length = 10")
CODE (U"height = length/2")
CODE (U"area = length * height")
CODE (U"writeInfoLine: \"The area is \", area, \".\"")
NORMAL (U"You can use numeric variables and formulas in numeric arguments to commands:")
CODE (U"Draw line: 0, 0, length / 2, 2 * height")
NORMAL (U"You can use numeric expressions in assignments (as above), or after "
	"#if, #elsif, #while, #until, and twice after #for.")
ENTRY (U"String expressions")
NORMAL (U"You can use a large variety of @@Formulas@ in your script:")
CODE (U"addressee\\$  = \"Silke\"")
CODE (U"greeting\\$  = \"Hi \" + addressee\\$  + \"!\"")
CODE (U"writeInfoLine: \"The greeting is: \", greeting\\$ ")
NORMAL (U"You can use string variables and formulas in numeric arguments to commands:")
CODE (U"Draw line: 0, length (greeting\\$ ), 0, 100")
CODE (U"Draw line: 0, if answer\\$  = \"yes\" then 20 else 30 fi, 0, 100")
NORMAL (U"You can use numeric and string variables and formulas in string arguments to commands:")
CODE (U"Text top: \"yes\", \"Hi \" + addressee\\$  + \"!\"")
CODE (U"Text top: \"yes\", left\\$  (fileName\\$ , index (fileName\\$ , \".\") - 1)")
NORMAL (U"The two examples from the end of @@Scripting 3.5. String queries|\\SS3.5@ could be abbreviated as the one-liners")
CODE (U"writeInfoLine: do\\$  (\"Get power...\", 0.0, 0.0)")
NORMAL (U"and")
CODE (U"writeInfoLine: do (\"Get power...\", 0.0, 0.0)")
ENTRY (U"Assignments from query commands")
NORMAL (U"On how to get information from commands that normally write to the Info window, "
	"see @@Scripting 6.3. Query commands@.")
MAN_END

MAN_BEGIN (U"Scripting 5.3. Jumps", U"ppgb", 19991112)
NORMAL (U"You can use conditional jumps in your script:")
TAG (U"#if %expression")
TAG (U"#elsif %expression")
DEFINITION (U"if the expression evaluates to zero or %false, the execution of the script jumps to the next "
	"#elsif or after the next #else or #endif at the same depth.")
NORMAL (U"The following script computes the preferred length of a bed for a person 'age' years of age:")
CODE (U"#if age <= 3")
CODE (U"   length = 1.20")
CODE (U"#elsif age <= 8")
CODE (U"   length = 1.60")
CODE (U"#else")
CODE (U"   length = 2.00")
CODE (U"#endif")
NORMAL (U"A variant spelling for #elsif is #elif.")
MAN_END

MAN_BEGIN (U"Scripting 5.4. Loops", U"ppgb", 20140111)
ENTRY (U"\"For\" loops")
TAG (U"#for %variable #from %expression__1_ #to %expression__2_")
TAG (U"#for %variable #to %expression")
DEFINITION (U"the statements between the #for line and the matching #endfor will be executed "
	"while a variable takes on values between two expressions, with an increment (raise) of 1 "
	"on each turn of the loop. If there is no #from, the loop variable starts at 1.")
NORMAL (U"The following script plays nine sine waves, with frequencies of 200, 300, ..., 1000 Hz:")
CODE (U"#for i #from 2 #to 10")
CODE1 (U"Create Sound as pure tone: \"tone\", 1, 0, 0.3, 44100, i * 100, 0.2, 0.01, 0.01")
CODE1 (U"Play")
CODE1 (U"Remove")
CODE (U"#endfor")
NORMAL (U"The stop value of the #for loop is evaluated on each turn. If the second expression "
	"is already less than the first expression to begin with, the statements between #for and #endfor "
	"are not executed even once.")
ENTRY (U"\"Repeat\" loops")
TAG (U"#until %expression")
DEFINITION (U"the statements between the matching preceding #repeat and the #until line "
	"will be executed again if the expression evaluates to zero or %false.")
NORMAL (U"The following script measures the number of trials it takes me to throw 12 with two dice:")
CODE (U"throws = 0")
CODE (U"#repeat")
	CODE1 (U"eyes = randomInteger (1, 6) + randomInteger (1, 6)")
	CODE1 (U"throws = throws + 1")
CODE (U"#until eyes = 12")
CODE (U"#writeInfoLine: \"It took me \", throws, \" trials to throw 12 with two dice.\"")
NORMAL (U"The statements in the #repeat/#until loop are executed at least once.")
ENTRY (U"\"While\" loops")
TAG (U"#while %expression")
DEFINITION (U"if the expression evaluates to zero or %false, the execution of the script jumps "
	"after the matching #endwhile.")
TAG (U"#endwhile")
DEFINITION (U"execution jumps back to the matching preceding #while line, which is then evaluated again.")
NORMAL (U"The following script forces the number %x into the range [0; 2\\pi):")
CODE (U"#while x < 0")
	CODE1 (U"x = x + 2 * pi")
CODE (U"#endwhile")
CODE (U"#while x >= 2 * pi")
	CODE1 (U"x = x - 2 * pi")
CODE (U"#endwhile")
NORMAL (U"If the expression evaluates to zero or %false to begin with, the statements between #while and #endwhile "
	"are not executed even once.")
MAN_END

MAN_BEGIN (U"Scripting 5.5. Procedures", U"ppgb", 20140126)
NORMAL (U"Sometimes in a Praat script, you will want to perform the same thing more than once. "
	"In @@Scripting 5.4. Loops|\\SS5.4@ we saw how %loops can help there. "
	"In this section we will see how %procedures (also called %subroutines) can help us.")
NORMAL (U"Imagine that you want to play a musical note with a frequency of 440 Hz (an \"A\") "
	"followed by a note that is one ocatve higher, i.e. has a frequency of 880 Hz (an \"a\"). "
	"You could achieve this with the following script:")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 440, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 880, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"This script creates a sound with a sine wave with an amplitude of 0.4 and a frequency of 440 Hz, "
	"then plays this sound, then changes the sound into a sine wave with a frequency of 880 Hz, "
	"then plays this changed sound, and then removes the Sound object from the object list.")
NORMAL (U"This script is perfect if all you want to do is to play those two notes and nothing more. "
	"But now imagine that you want to play such an octave jump not only for a note of 440 Hz, "
	"but also for a note of 400 Hz and for a note of 500 Hz. You could use the following script:")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 440, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 880, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 400, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 800, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 500, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, 1000, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"This script works but is no longer perfect. It contains many similar lines, and is difficult to read.")
NORMAL (U"Here is where %procedures come in handy. With procedures, you can re-use similar pieces of code. "
	"To make the three parts of the above script more similar, I'll rewrite it using two variables "
	"(%frequency and %octaveHigher):")
CODE (U"frequency = 440")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"octaveHigher = 2 * frequency")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"frequency = 400")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"octaveHigher = 2 * frequency")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"frequency = 500")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"octaveHigher = 2 * frequency")
CODE (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"You can now see that seven lines of the script appear identically three times. "
	"I'll put those seven lines into a %procedure that I name \"playOctave\":")
CODE (U"#procedure playOctave")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"octaveHigher = 2 * frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
CODE (U"#endproc")
NORMAL (U"As you see, a %%procedure definition% in Praat consists of three parts:")
LIST_ITEM (U"1. a line with the word #procedure, followed by the name of the procedure, followed by a pair of parentheses;")
LIST_ITEM (U"2. the %body of the procedure (here: seven lines);")
LIST_ITEM (U"3. a line with the word #endproc.")
NORMAL (U"You can put a procedure definition anywhere in your script; "
	"the beginning or end of the script are common places.")
NORMAL (U"The bodies of procedures are executed only if you %call the procedure explicitly, "
	"which you can do anywhere in the rest of your script:")
CODE (U"frequency = 440")
CODE (U"\\@ playOctave")
CODE (U"frequency = 400")
CODE (U"\\@ playOctave")
CODE (U"frequency = 500")
CODE (U"\\@ playOctave")
CODE (U"#procedure playOctave")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"octaveHigher = 2 * frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
CODE (U"#endproc")
NORMAL (U"This script works as follows. First, the number 440 is assigned to the variable %frequency in line 1. "
	"Then, execution of the script arrives at the ##\\@ # (\"call\") statement of line 2. "
	"Praat then knows that it has to jump to the procedure called %playOctave, "
	"which is found on line 7. The execution of the script then proceeds with the first line of the procedure body, "
	"where a Sound is created. Then, the other lines of the procedure body are also executed, ending with the removal of the Sound. "
	"Then, the execution of the script arrives at the #endproc statement. Here, Praat knows that it has to jump back "
	"to the line after the line that the procedure was called from; since the procedure was called from line 2, "
	"the execution proceeds at line 3 of the script. There, the number 400 is assigned to the variable %frequency. "
	"In line 4, execution will jump to the procedure again, and with the next #endproc the execution will jump back to line 5. "
	"There, 500 is assigned to %frequency, followed by the third jump to the procedure. "
	"the third #endproc jumps back to the line after the third #\\@ , i.e. to line 7. "
	"Here the execution of the script will stop, because there are no more executable commands "
	"(the procedure definition at the end is not executed again).")
ENTRY (U"Arguments")
NORMAL (U"The above example contains something awkward. The procedure %playOctave requires that the variable %frequency "
	"is set to an appropriate value, so before calling %playOctave you always have to insert a line like")
CODE (U"frequency = 440")
NORMAL (U"This can be improved upon. In the following version of the script, the procedure %playOctave requires an explicit %argument:")
CODE (U"\\@ playOctave: 440")
CODE (U"\\@ playOctave: 400")
CODE (U"\\@ playOctave: 500")
CODE (U"#procedure playOctave: frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"octaveHigher = 2 * frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
CODE (U"#endproc")
NORMAL (U"This works as follows. The first line of the procedure now not only contains the name (%playOctave), "
	"but also a list of variables (here only one: %frequency). In the first line of the script, "
	"the procedure %playOctave is called with the %%argument% 440. "
	"Execution then jumps to the procedure, where the argument 440 is assigned to the variable %frequency, "
	"which is then used in the body of the procedure.")
ENTRY (U"Encapsulation and local variables")
NORMAL (U"Although the size of the script has now been reduced to 12 lines, which cannot be further improved upon, "
	"there is still something wrong with it. Imagine the following script:")
CODE (U"frequency = 300")
CODE (U"\\@ playOctave: 440")
CODE (U"\\@ playOctave: 400")
CODE (U"\\@ playOctave: 500")
CODE (U"#writeInfoLine: frequency")
CODE (U"#procedure playOctave: frequency")
	CODE1 (U"Create Sound from formula: \"note\", 1, 0, 0.3, 44100, frequency, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"octaveHigher = 2 * frequency")
	CODE1 (U"Create Sound from formula: \"note\", 1, 0, 0.3, 44100, octaveHigher, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
CODE (U"#endproc")
NORMAL (U"You might have thought that this script will write \"300\" to the Info window, "
	"because that is what you expect if you look at the first five lines. "
	"However, the procedure will assign the values 440, 400, and 500 to the variable %frequency, "
	"so that the script will actually write \"500\" to the Info window, because 500 "
	"is the last (fourth!) value that was assigned to the variable %frequency.")
NORMAL (U"What you would want is that variables that are used inside procedures, such as %frequency and %octaveHigher here, "
	"could somehow be made not to \"clash\" with variable names used outside the procedure. "
	"A trick that works would be to include the procedure name into the names of these variables:")
CODE (U"frequency = 300")
CODE (U"\\@ playOctave: 440")
CODE (U"\\@ playOctave: 400")
CODE (U"\\@ playOctave: 500")
CODE (U"#writeInfoLine: frequency")
CODE (U"#procedure playOctave: playOctave.frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, playOctave.frequency, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"playOctave.octaveHigher = 2 * playOctave.frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, playOctave.octaveHigher, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
CODE (U"#endproc")
NORMAL (U"This works. The six tones will be played, and \"300\" will be written to the Info window. "
	"But the formulation is a bit wordy, isn't it?")
NORMAL (U"Fortunately, Praat allows an abbreviated version of these long names: "
	"just leave \"playOctave\" off from the names of the variables, but keep the period (.):")
CODE (U"frequency = 300")
CODE (U"\\@ playOctave: 440")
CODE (U"\\@ playOctave: 400")
CODE (U"\\@ playOctave: 500")
CODE (U"#writeInfoLine: frequency")
CODE (U"#procedure playOctave: .frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, .frequency, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U".octaveHigher = 2 * .frequency")
	CODE1 (U"Create Sound as pure tone: \"note\", 1, 0, 0.3, 44100, .octaveHigher, 0.2, 0.01, 0.01")
	CODE1 (U"Play")
	CODE1 (U"Remove")
CODE (U"#endproc")
NORMAL (U"This is the final version of the script. It works because Praat knows that "
	"you are using the variable names %%.frequency% and %%.octaveHigher% in the context of the procedure %playOctave, "
	"so that Praat knows that by these variable names you actually mean to refer to %%playOctave.frequency% and %%playOctave.octaveHigher%.")
NORMAL (U"It is advisable that you use such \"local\" variable names for all %parameters of a procedure, "
	"i.e. for the variables listed after the #procedure word (e.g. %%.frequency%), "
	"as well as for all variables that you create in the procedure body (e.g. %%.octaveHigher%). "
	"In this way, you make sure that you don't inadvertently use a variable name that is also used outside the procedure "
	"and thereby perhaps inadvertently change the value of a variable that you expect to be constant across a procedure call.")
ENTRY (U"A list of numeric and string arguments")
NORMAL (U"You can use multiple arguments, separated by commas, and string arguments (with a dollar sign in the variable name):")
CODE (U"\\@ listSpeaker: \"Bart\", 38")
CODE (U"\\@ listSpeaker: \"Katja\", 24")
CODE (U"#procedure listSpeaker: .name\\$ , .age")
	CODE1 (U"#appendInfoLine: \"Speaker \", .name\\$ , \" is \", .age, \" years old.\"")
CODE (U"#endproc")
NORMAL (U"or")
CODE (U"\\@ conjugateVerb: \"be\", \"I am\", \"you are\", \"she is\"")
CODE (U"#procedure conjugateVerb: .verb\\$ , .first\\$ , .second\\$ , .third\\$ ")
	CODE1 (U"#writeInfoLine: \"Conjugation of 'to \", .verb\\$ , \"':\"")
	CODE1 (U"#appendInfoLine: \"1sg \", .first\\$ ")
	CODE1 (U"#appendInfoLine: \"2sg \", .second\\$ ")
	CODE1 (U"#appendInfoLine: \"3sg \", .third\\$ ")
CODE (U"#endproc")
NORMAL (U"For the arguments you can use expressions:")
CODE (U"\\@ playOctave: 400 + 100")
NORMAL (U"As with all string literals, the double quotes in literal string arguments should be doubled:")
CODE (U"#procedure texts: .top\\$ , .bottom\\$ ")
	CODE1 (U"Text top: \"yes\", .top\\$ ")
	CODE1 (U"Text bottom: \"yes\", .bottom\\$ ")
CODE (U"#endproc")
CODE (U"\\@ texts: \\\" \\\" \\\" hello\\\" \\\"  at the top\\\" , \\\" \\\" \\\" goodbye\\\" \\\"  at the bottom\\\" ")
ENTRY (U"Functions")
NORMAL (U"The Praat scripting language does not have the concept of a \"function\" like some other scripting languages do. "
	"A function is a procedure that returns a number or a string. For instance, you can imagine the function $$squareNumber$ "
	"which takes a number (e.g. 5) as an argument and returns the square of that number (e.g. 25). "
	"Here is an example of how you can do that, using the global availability of local variables:")
CODE (U"\\@ squareNumber: 5")
CODE (U"#writeInfoLine: \"The square of 5 is \", squareNumber.result, \".\"")
CODE (U"#procedure squareNumber: .number")
	CODE1 (U".result = .number \\^  2")
CODE (U"#endproc")
NORMAL (U"Another way to emulate functions is to use a variable name as an argument:")
CODE (U"\\@ squareNumber: 5, \"square5\"")
CODE (U"#writeInfoLine: \"The square of 5 is \", square5, \".\"")
CODE (U"#procedure squareNumber: .number, .squareVariableName\\$ ")
	CODE1 (U"'.squareVariableName\\$ ' = .number \\^  2")
CODE (U"#endproc")
NORMAL (U"However, this uses variable substitution, a trick better avoided.")
MAN_END

MAN_BEGIN (U"Scripting 5.6. Arrays and dictionaries", U"ppgb", 20170718)
NORMAL (U"You can use arrays of numeric and string variables:")
CODE (U"#for i #from 1 #to 5")
	CODE1 (U"square [i] = i * i")
	CODE1 (U"text\\$  [i] = mid\\$  (\"hello\", i)")
CODE (U"#endfor")
NORMAL (U"After this, the variables $$square[1]$, $$square[2]$, $$square[3]$, $$square[4]$, $$square[5]$, "
	"$$text\\$ [1]$, $$text\\$ [2]$, $$text\\$ [3]$, $$text\\$ [4]$, and $$text\\$ [5]$ contain "
	"the values 1, 4, 9, 16, 25, \"h\", \"e\", \"l\", \"l\", and \"o\", respectively:")
CODE (U"#writeInfoLine: \"Some squares:\"")
CODE (U"#for i #from 1 #to 5")
	CODE1 (U"#appendInfoLine: \"The square of \", i, \" is \", square [i]")
CODE (U"#endfor")
NORMAL (U"In the examples above, the %index into the array was always a number. "
	" A %hash or %dictionary is an array variable where the index is a string:")
CODE (U"age [\"John\"] = 36")
CODE (U"age [\"Babs\"] = 39")
CODE (U"#writeInfoLine: \"John is \", age [\"John\"], \" years old.\"")
ENTRY (U"See also")
NORMAL (U"You can use any number of array and dictionary variables in a script, "
	"but for many applications, namely whenever it were useful to look at a numeric array as a single object, "
	"it may be better to use vectors and matrices (see @@Scripting 5.7. Vectors and matrices@) "
	"or to use Matrix or Sound objects.")
MAN_END

MAN_BEGIN (U"Scripting 5.7. Vectors and matrices", U"ppgb", 20170722)
ENTRY (U"1. What is a vector?")
NORMAL (U"A ##numeric vector# is an array of numbers, regarded as a single object. "
	"For instance, the squares of the first five integers can be collected in the vector { 1, 4, 9, 16, 25 }. "
	"In a Praat script, you can put a vector into a variable whose name ends in a number sign (\"\\# \"):")
CODE (U"squares\\#  = { 1, 4, 9, 16, 25 }")
NORMAL (U"After this, the variable %%squares\\# % contains the value { 1, 4, 9, 16, 25 }. "
	"We say that the vector %%squares\\# % has five %dimensions, i.e. it contains five numbers.")
NORMAL (U"Whereas in @@Scripting 3.2. Numeric variables@ we talked about a numeric variable as being analogous to a house "
	"where somebody (the numeric %value) could live, a numeric vector with five dimensions "
	"can be seen as a %street that contains five houses, which are numbered with the indexes 1, 2, 3, 4 and 5, "
	"each house containing a numeric value. Thus, the street %%squares\\# % contains the following five houses: "
	"%%squares\\# % [1], %%squares\\# % [2], %%squares\\# % [3], %%squares\\# % [4] and %%squares\\# % [5]. "
	"Their values (the numbers that currently live in these houses) are 1, 4, 9, 16 and 25, respectively.")
NORMAL (U"To list the five values with a loop, you could do:")
CODE (U"#writeInfoLine: \"Some squares:\"")
CODE (U"#for i #from 1 #to 5")
	CODE1 (U"#appendInfoLine: \"The square of \", i, \" is \", squares\\#  [i]")
CODE (U"#endfor")
NORMAL (U"Instead of the above procedure to get the vector %%squares\\# %, with a pre-computed list of five squares, "
	"you could compute the five values with a formula, as in the example of @@Scripting 5.6. Arrays and dictionaries@. "
	"However, in order to put a value into an element of the vector, you have to create the vector first "
	"(i.e., you have to build the whole street before you can put something in a house), "
	"so we start by creating a vector with five zeroes in it:")
CODE (U"squares\\#  = zero\\#  (5)")
NORMAL (U"After this, %%squares\\# % is the vector { 0, 0, 0, 0, 0 }, i.e., the value of each element is zero. "
	"Now that the vector (street) exists, we can put values into (populate) the five elements (houses):")
CODE (U"#for i #from 1 #to 5")
	CODE1 (U"squares\\#  [i] = i * i")
CODE (U"#endfor")
NORMAL (U"After this, the variable $$squares\\# $ has the value { 1, 4, 9, 16, 25 }, as before, "
	"but now we had the computer compute the squares.")
ENTRY (U"2. Creating a vector")
NORMAL (U"You can create a vector in many ways. The first way we saw was with a ##vector literal#, "
	"i.e. a series of numbers (or numeric formulas) between braces:")
CODE (U"lengths\\#  = { 1.83, 1.795, 1.76 }")
NORMAL (U"The second way we saw was to create a series of #zeroes. To create a vector consisting of 10,000 zeroes, you do")
CODE (U"zero\\#  (10000)")
NORMAL (U"Another important type of vector is a series of random numbers. "
	"To create a vector consisting of 10,000 values drawn from a ##Gaussian distribution# "
	"with true mean 0.0 and true standard deviation 1.0, you could do")
CODE (U"noise\\#  = randomGauss\\#  (10000, 0.0, 1.0)")
NORMAL (U"To create a vector consisting of 10,000 values drawn from a ##uniform distribution of real numbers# "
	"with true minimum 0.0 and true maximum 1.0, you use")
CODE (U"randomUniform\\#  (10000, 0.0, 1.0)")
NORMAL (U"To create a vector consisting of 10,000 values drawn from a ##uniform distribution of integer numbers# "
	"with true minimum 1 and true maximum 10, you use")
CODE (U"randomInteger\\#  (10000, 1, 10)")
NORMAL (U"Vectors can also be created by some menu commands. For instance, to get vectors representing "
	"the times and pitch frequencies of the frames in a Pitch object, you can do")
CODE (U"selectObject: myPitch")
CODE (U"times\\#  = Get times of frames")
CODE (U"pitches\\#  = Get values in frames")
ENTRY (U"3. Turning a vector into a number")
NORMAL (U"For the vector defined above, you can compute the #sum of the five values as")
CODE (U"sum (squares\\# )")
NORMAL (U"which gives 55. You compute the #average of the five values as")
CODE (U"mean (squares\\# )")
NORMAL (U"which gives 11. You compute the ##standard deviation# of the values as ")
CODE (U"stdev (squares\\# )")
NORMAL (U"which gives 9.669539802906858 (the standard deviation is undefined for vectors with fewer than 2 elements). "
	"The ##center of gravity# of the distribution defined by regarding "
	"the five values as relative frequencies as a function of the index from 1 to 5 is computed by")
CODE (U"center (squares\\# )")
NORMAL (U"which gives 4.090909090909091 (for vector with five elements, the result will always be "
	"a number between 1.0 and 5.0). You compute the ##inner product# of two equally long vectors as follows:")
CODE (U"other\\#  = { 2, 1.5, 1, 0.5, 0 }")
CODE (U"result\\#  = inner (square\\# , other\\# )")
NORMAL (U"which gives 1*2 + 4*1.5 + 9*1 + 16*0.5 + 25*0 = 25. "
	"The formula for this is \\su__%i=1_^5 square[i] * other[i], so that an alternative piece of 1code could be")
CODE (U"result\\#  = sumOver (i to 5, square\\#  [i] * other\\#  [i])")
ENTRY (U"4. Converting vectors to vectors")
CODE (U"a\\#  = squares\\#  + 5   ; adding a number to each element of a vector")
NORMAL (U"causes a\\#  to become the vector { 6, 9, 14, 21, 30 }.")
CODE (U"b\\#  = a\\#  + { 3.14, 2.72, 3.16, -1, 7.5 }   ; adding two vectors of the same length")
NORMAL (U"causes b\\#  to become the vector { 9.14, 16.72, 17.16, 20, 37.5 }.")
CODE (U"c\\#  = b\\#  / 2   ; dividing each element of a vector")
NORMAL (U"causes c\\#  to become the vector { 4.57, 8.36, 8.58, 10, 18.75 }.")
CODE (U"d\\#  = b\\#  * c\\#    ; elementwise multiplication")
NORMAL (U"causes d\\#  to become the vector { xx, 8.36, 8.58, 10, 18.75 }.")
NORMAL (U"A vector can also be given to a ##menu command# that returns another vector. "
	"For instance, to get a vector representing the pitch frequencies at 0.01-second intervals in a Pitch object, "
	"you can do")
CODE (U"selectObject: myPitch")
CODE (U"tmin = Get start time")
CODE (U"tmax = Get end time")
CODE (U"times\\#  = linear\\#  (tmin, tmax, 0.01, xx)")
CODE (U"pitches\\#  = Get values at times: times\\# , \"hertz\", \"linear\"")
MAN_END

MAN_BEGIN (U"Scripting 5.8. Including other scripts", U"ppgb", 20170718)
INTRO (U"You can include other scripts within your script:")
CODE (U"a = 5")
CODE (U"include square.praat")
CODE (U"writeInfoLine: a")
NORMAL (U"The Info window will show the result 25 if the file square.praat contains the following:")
CODE (U"a = a * a")
NORMAL (U"The inclusion is done before any other part of the script is considered, so you can use the #form statement "
	"and all variables in it. Usually, however, you will put some procedure definitions in the include file, that is "
	"what it seems to be most useful for. Watch out, however, for using variable names in the include file: "
	"the example above shows that there is no such thing as a separate name space.")
NORMAL (U"Note that you do not put quotes around the name of the include file. "
        "This is because the name of the include file has to be given explicitly; you cannot put it into a variable, for instance.")
NORMAL (U"You can use full or relative file names. For instance, the file square.praat is expected to be in the same "
	"directory as the script that says %%include square.praat%. If you use the ScriptEditor, you will first have to save "
	"the script that you are editing before any relative file names become meaningful (this is the same as with other "
	"uses of relative file names in scripts).")
NORMAL (U"You can \"nest\" include files, i.e., included scripts can include other scripts. However, relative file names "
	"are always evaluated relative to the directory of the outermost script.")
NORMAL (U"The #include statement can only be at the start of a line: you cannot put any spaces in front of it.")
MAN_END

MAN_BEGIN (U"Scripting 5.9. Quitting", U"ppgb", 20170718)
NORMAL (U"Usually, the execution of your script ends when the interpreter has executed the last line "
	"that is not within a procedure definition. However, you can also explicitly stop the script:")
TAG (U"#exitScript ( )")
DEFINITION (U"stops the execution of the script in the normal way, i.e. without any messages to the user. "
	"Any settings window is removed from the screen.")
TAG (U"#exitScript: %%error-message%")
DEFINITION (U"stops the execution of the script while sending an error message to the user. "
	"You can use the same argument list as with #writeInfoLine. "
	"Any settings window will stay on the screen.")
NORMAL (U"For an example, see @@Scripting 6.8. Messages to the user@.")
MAN_END

MAN_BEGIN (U"Scripting 6. Communication outside the script", U"ppgb", 20141012)
LIST_ITEM (U"@@Scripting 6.1. Arguments to the script@ (form/endform, runScript)")
LIST_ITEM (U"@@Scripting 6.2. Writing to the Info window@ (writeInfoLine, appendInfoLine, appendInfo, tab\\$ )")
LIST_ITEM (U"@@Scripting 6.3. Query commands@ (Get, Count)")
LIST_ITEM (U"@@Scripting 6.4. Files@ (fileReadable, readFile, writeFile, deleteFile, createDirectory)")
LIST_ITEM (U"@@Scripting 6.5. Calling system commands@ (runSystem, environment\\$ , stopwatch)")
LIST_ITEM (U"@@Scripting 6.6. Controlling the user@ (pause, beginPause/endPause, chooseReadFile\\$ )")
LIST_ITEM (U"@@Scripting 6.7. Sending a message to another program@ (sendsocket)")
LIST_ITEM (U"@@Scripting 6.8. Messages to the user@ (exitScript, assert, nowarn, nocheck)")
LIST_ITEM (U"@@Scripting 6.9. Calling from the command line")
MAN_END

MAN_BEGIN (U"Scripting 6.1. Arguments to the script", U"ppgb", 20140212)
NORMAL (U"You can cause a Praat script to prompt for arguments. The file $$playSine.praat$ may contain the following:")
CODE (U"#form Play a sine wave")
	CODE1 (U"#positive Sine_frequency_(Hz) 377")
	CODE1 (U"#positive Gain_(0..1) 0.3 (= not too loud)")
CODE (U"#endform")
CODE (U"Create Sound as pure tone: \"sine\" + string\\$  (sine_frequency), 1, 0, 1, 44100, sine_frequency, gain, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"When running this script, the interpreter puts a settings window (%form) on your screen, "
	"entitled \"Play a sine wave\", "
	"with two fields, titled \"Sine frequency (Hz)\" and \"Gain\", that have been provided "
	"with the standard values \"377\" and \"0.3 (= not too loud)\", which you can change before clicking #OK.")
NORMAL (U"As you see, the underscores have been replaced with spaces: that looks better in the form. "
	"Inside the script, the field names can be accessed as variables: these do contain the underscores, "
	"since they must not contain spaces, but the parentheses (Hz) have been chopped off. Note that the first "
	"letter of these variables is converted to lower case, so that you can assign to them in your script.")
NORMAL (U"Inside the script, the value \"0.3 (= not too loud)\" will be known as \"0.3\", "
	"because this is a numeric field.")
NORMAL (U"You can use the following field types in your forms:")
TAG (U"#real %variable %initialValue")
DEFINITION (U"for real numbers.")
TAG (U"#positive %variable %initialValue")
DEFINITION (U"for positive real numbers: the form issues an error message if the number "
	"that you enter is negative or zero; further on in the script, the number may take on any value.")
TAG (U"#integer %variable %initialValue")
DEFINITION (U"for whole numbers: the form reads the number as an integer; "
	"further on in the script, the number may take on any real value.")
TAG (U"#natural %variable %initialValue")
DEFINITION (U"for positive whole numbers: the form issues an error message if the number "
	"that you enter is negative or zero; further on in the script, the number may take on any real value.")
TAG (U"#word %variable %initialValue")
DEFINITION (U"for a string without spaces: the form only reads up to the first space (\"oh yes\" becomes \"oh\"); "
	"further on in the script, the string may contain spaces.")
TAG (U"#sentence %variable %initialValue")
DEFINITION (U"for any short string.")
TAG (U"#text %variable %initialValue")
DEFINITION (U"for any possibly long string (the variable name will not be shown in the form).")
TAG (U"#boolean %variable %initialValue")
DEFINITION (U"a check box will be shown; the value is 0 if off, 1 if on.")
TAG (U"#choice %variable %initialValue")
DEFINITION (U"a radio box will be shown; the value is 1 or higher. This is followed by a series of:")
TAG (U"#button %text")
DEFINITION (U"a button in a radio box.")
TAG (U"#comment %text")
DEFINITION (U"a line with any text.")
NORMAL (U"Inside the script, strings are known as string variables, numbers as numeric variables. Consider the following form:")
CODE (U"#form Sink it")
	CODE1 (U"#sentence Name_of_the_ship Titanic")
	CODE1 (U"#real Distance_to_the_iceberg_(m) 500.0")
	CODE1 (U"#natural Number_of_people 1800")
	CODE1 (U"#natural Number_of_boats 10")
CODE (U"#endform")
NORMAL (U"In the script following this form, the variables will be known as %%name_of_the_ship\\$ %, %distance_to_the_iceberg, "
	"%number_of_people, and %number_of_boats.")
NORMAL (U"The variable associated with a radio box will get a numeric as well as a string value:")
CODE (U"#form Fill attributes")
	CODE1 (U"#comment Choose any colour and texture for your paintings")
	CODE1 (U"#choice Colour: 5")
		CODE2 (U"#button Dark red")
		CODE2 (U"#button Sea green")
		CODE2 (U"#button Navy blue")
		CODE2 (U"#button Canary yellow")
		CODE2 (U"#button Black")
		CODE2 (U"#button White")
	CODE1 (U"#choice Texture: 1")
		CODE2 (U"#button Smooth")
		CODE2 (U"#button Rough")
		CODE2 (U"#button With holes")
CODE (U"#endform")
CODE (U"#writeInfoLine: \"You chose the colour \", colour\\$ , \" and the texture \", texture\\$ , \".\"")
NORMAL (U"This shows two radio boxes. In the Colour box, the fifth button (Black) is the standard value here. "
	"If you click on \"Navy blue\" and then #%OK, the variable %colour will have the value \"3\", "
	"and the variable %%colour\\$ % will have the value \"Navy blue\". "
	"Note that the trailing colon is chopped off, and that the button and comment texts may contain spaces. "
	"So you can test the value of the Colour box in either of the following ways:")
CODE (U"if colour = 4")
NORMAL (U"or")
CODE (U"if colour\\$  = \"Canary yellow\"")
NORMAL (U"The field types #optionmenu and #option are completely analogous to #choice and #button, "
	"but use up much less space on the screen:")
CODE (U"#form Fill attributes")
	CODE1 (U"#comment Choose any colour and texture for your paintings")
	CODE1 (U"#optionmenu Colour: 5")
		CODE2 (U"#option Dark red")
		CODE2 (U"#option Sea green")
		CODE2 (U"#option Navy blue")
		CODE2 (U"#option Canary yellow")
		CODE2 (U"#option Black")
		CODE2 (U"#option White")
	CODE1 (U"#optionmenu Texture: 1")
		CODE2 (U"#option Smooth")
		CODE2 (U"#option Rough")
		CODE2 (U"#option With holes")
CODE (U"#endform")
CODE (U"#writeInfoLine: \"You chose the colour \", colour\\$ , \" and the texture \", texture\\$ , \".\"")
NORMAL (U"You can combine two short fields into one by using %left and %right:")
CODE (U"#form Get duration")
	CODE1 (U"#natural left_Year_range 1940")
	CODE1 (U"#natural right_Year_range 1945")
CODE (U"#endform")
CODE (U"duration = right_Year_range - left_Year_range")
CODE (U"#writeInfoLine: \"The duration is \", duration, \" years.\"")
NORMAL (U"The interpreter will only show the single text \"Year range\", followed by two small text fields.")
ENTRY (U"Calling a script from another script")
NORMAL (U"Scripts can be nested: the file %%doremi.praat% may contain the following:")
CODE (U"#runScript: \"playSine.praat\", 550, 0.9")
CODE (U"#runScript: \"playSine.praat\", 615, 0.9")
CODE (U"#runScript: \"playSine.praat\", 687, 0.9")
NORMAL (U"With #runScript, Praat will not display a form window, but simply execute the script "
	"with the two arguments that you supply on the same line (e.g. 550 and 0.9).")
NORMAL (U"Values for #choice must be passed as strings:")
CODE (U"#runScript: \"fill attributes.praat\", \"Navy blue\", \"With holes\"")
NORMAL (U"You can pass values for #boolean either as \"yes\" and \"no\" or as 1 and 0.")
MAN_END

MAN_BEGIN (U"Scripting 6.2. Writing to the Info window", U"ppgb", 20140111)
NORMAL (U"With the @Info button and several commands in the #Query menus, "
	"you write to the @@Info window@ (if your program is run from the command line, "
	"the text goes to the console window or to %stdout instead; see @@Scripting 6.9. Calling from the command line|\\SS6.9).")
NORMAL (U"The commands #writeInfo, #writeInfoLine, #appendInfo and #appendInfoLine "
	"allow you to write to the Info window from a script. Those with #write in their name clear the Info window "
	"before they write to it, those with #append in their name do not. Those with #Line in their name make sure "
	"that a following #appendInfo or #appendInfoLine will write on the next line.")
NORMAL (U"These four functions take a variable number of numeric and/or string arguments, separated by commas. "
	"The following script builds a table with statistics about a pitch contour:")
CODE (U"#writeInfoLine: \"  Minimum   Maximum\"")
CODE (U"Create Sound as pure tone: \"sine\", 1, 0, 0.1, 44100, 377, 0.2, 0.01, 0.01")
CODE (U"To Pitch: 0.01, 75, 600")
CODE (U"minimum = Get minimum: 0, 0, \"Hertz\", \"Parabolic\"")
CODE (U"#appendInfo: minimum")
CODE (U"#appendInfo: tab\\$ ")
CODE (U"maximum = Get maximum: 0, 0, \"Hertz\", \"Parabolic\"")
CODE (U"#appendInfo: maximum")
CODE (U"#appendInfoLine: \"\"")
NORMAL (U"You could combine the last four print statements into:")
CODE (U"#appendInfoLine: minimum, tab\\$ , maximum")
NORMAL (U"which is the same as:")
CODE (U"#appendInfo: minimum, tab\\$ , maximum, newline\\$ ")
NORMAL (U"The little string ##tab\\$ # is a %tab character; it allows you to create "
	"table files that can be read by some spreadsheet programs. The little string ##newline\\$ # is a %newline character; "
	"it moves the following text to the next line.")
NORMAL (U"To clear the Info window, you can do")
CODE (U"#writeInfo: \"\"")
NORMAL (U"or")
CODE (U"#clearinfo")
MAN_END

MAN_BEGIN (U"Scripting 6.3. Query commands", U"ppgb", 20140107)
NORMAL (U"If you click the \"Get mean...\" command for a Pitch object, "
	"the Info window will contain a text like \"150 Hz\" as a result. In a script, you would rather "
	"have this result in a variable instead of in the Info window. The solution is simple:")
CODE (U"mean = Get mean: 0, 0, \"Hertz\", \"Parabolic\"")
NORMAL (U"The numeric variable \"mean\" now contains the number 150. When assigning to a numeric variable, "
	"the interpreter converts the part of the text before the first space into a number.")
NORMAL (U"You can also assign to string variables:")
CODE (U"mean\\$  = Get mean: 0, 0, \"Hertz\", \"Parabolic\"")
NORMAL (U"The string variable \"mean\\$ \" now contains the entire string \"150 Hz\".")
NORMAL (U"This works for every command that would otherwise write into the Info window.")
MAN_END

MAN_BEGIN (U"Scripting 6.4. Files", U"ppgb", 20150601)
INTRO (U"You can read from and write to text files from a Praat script.")
ENTRY (U"Reading a file")
NORMAL (U"You can check the availability of a file for reading with the function")
CODE (U"#fileReadable (%%fileName\\$ %)")
NORMAL (U"which returns 1 (true) if the file exists and can be read, and 0 (false) otherwise. "
	"Note that %%fileName\\$ % is taken relatively to the directory where the script is saved; "
	"for instance, if your script is in the directory ##Paolo/project1#, then the file name "
	"\"hello.wav\" refers to ##Paolo/project1/hello.wav#, the file name \"yesterday/hello.wav\" "
	"refers to ##Paolo/project1/yesterday/hello.wav#, and the file name \"../project2/hello.wav\" "
	"refers to ##Paolo/project2/hello.wav# (\"..\" goes one directory up). "
	"You can also use full path names such as \"C:/Users/Paolo/project1/hello.wav\" "
	"on Windows and \"/Users/Paolo/project1/hello.wav\" on the Mac.")
NORMAL (U"To read the contents of an existing text file into a string variable or into a numeric variable, you use")
CODE (U"text\\$  = readFile\\$  (\"myFile.txt\")")
NORMAL (U"or")
CODE (U"number = readFile (\"myFile.txt\")")
NORMAL (U"If the file does not exist, the script terminates with an error message.")
ENTRY (U"Example: reading a settings file")
NORMAL (U"Suppose that the file ##height.inf# may contain an appropriate value for a numeric variable "
	"called $height, which we need to use in our script. We would like to read it with")
CODE (U"height = readFile (\"height.inf\")")
NORMAL (U"However, this script will fail if the file ##height.inf# does not exist. To guard "
	"against this situation, we could check the existence of the file, and supply a default "
	"value in case the file does not exist:")
CODE (U"fileName\\$  = \"height.inf\"")
CODE (U"if fileReadable (fileName\\$ )")
	CODE1 (U"height = readFile (fileName\\$ )")
CODE (U"else")
	CODE1 (U"height = 180")
CODE (U"endif")
ENTRY (U"Writing a file")
NORMAL (U"You write into a new text file just as you write into the Info window:")
CODE (U"writeFileLine: \"myFile.txt\", \"The present year is \", 2000 + 13, \".\"")
NORMAL (U"and likewise you use %writeFile if you don't want a newline symbol at the end of the file. "
	"If the file cannot be created, the script terminates with an error message.")
NORMAL (U"To append text at the end of an existing file, you use")
CODE (U"appendFileLine: \"myFile.txt\", \"Next year it will be \", 2000 + 14, \".\"")
NORMAL (U"With %appendFileLine (and %appendFile, which does not add the newline), "
	"we follow the rule that if the file does not yet exist, it is created first.")
NORMAL (U"You can create a directory with")
CODE (U"#createDirectory: %%directoryName\\$ %")
NORMAL (U"where, as with file names, %%directoryName\\$ % can be relative to the directory of the script "
	"(e.g. \"data\", or \"yesterday/data\", or \"../project2/yesterday/data\") "
	"or an absolute path (e.g. \"C:/Users/Paolo/project1/yesterday/data\" on Windows "
	"or \"/Users/Paolo/project1/yesterday/data\" on the Mac). "
	"If the directory already exists, this command does nothing.")
NORMAL (U"You can delete an existing file with the function")
CODE (U"#deleteFile: %%fileName\\$ %")
NORMAL (U"If the file does not exist, this command does nothing.")
ENTRY (U"Example: writing a table of squares")
NORMAL (U"Suppose that we want to create a file with the following text:")
CODE (U"The square of 1 is 1")
CODE (U"The square of 2 is 4")
CODE (U"The square of 3 is 9")
CODE (U"...")
CODE (U"The square of 100 is 10000")
NORMAL (U"We can do this by appending 100 lines:")
CODE (U"deleteFile: \"squares.txt\"")
CODE (U"for i to 100")
	CODE1 (U"appendFileLine: \"squares.txt\", \"The square of \", i, \" is \", i * i")
CODE (U"endfor")
NORMAL (U"Note that we delete the file before appending to it, "
	"in order that we do not append to an already existing file.")
NORMAL (U"You can append the contents of the Info window to a file with")
CODE (U"appendFile: \"out.txt\", info\\$  ( )")
ENTRY (U"Directory listings")
NORMAL (U"To get the names of the files if a certain type in a certain directory, "
	"use @@Create Strings as file list...@.")
ENTRY (U"Alternative syntax")
NORMAL (U"If, on the basis of the syntax of commands and functions in earlier sections you expected that")
CODE (U"text\\$  = readFile\\$  (\"myFile.txt\")")
CODE (U"number = readFile (\"myFile.txt\")")
NORMAL (U"could be written as")
CODE (U"text\\$  = readFile\\$ : \"myFile.txt\"")
CODE (U"number = readFile: \"myFile.txt\"")
NORMAL (U"then you are right. The syntax with the colon is equivalent to the syntax with the two parentheses. Conversely, instead of")
CODE (U"#deleteFile: %%fileName\\$ %")
NORMAL (U"you can also write")
CODE (U"#deleteFile (%%fileName\\$ %)")
MAN_END

MAN_BEGIN (U"Scripting 6.5. Calling system commands", U"ppgb", 20141012)
INTRO (U"From a Praat script you can call system commands. "
	"These are the same commands that you would normally type into a terminal window or into the Window command line prompt. "
	"The syntax is the same as that of the #writeInfo command.")
NORMAL (U"Most system commands are different on different platforms. "
	"For instance, to throw away all WAV files in the directory whose name (relative to the script's directory) is "
	"in the variable directory\\$ , you would write")
CODE (U"#runSystem: \"del \", directory\\$ , \"\\bs*.wav\"")
NORMAL (U"on Windows, but")
CODE (U"#runSystem: \"rm \", directory\\$ , \"/*.wav\"")
NORMAL (U"on Macintosh and Linux.")
NORMAL (U"The script will stop running if a system command returns an error. For instance,")
CODE (U"#runSystem: \"rm \", directory\\$ , \"/*.wav\"")
NORMAL (U"will stop the script if there are no WAV files in the directory. "
	"In order to prevent this, you can tell Praat to ignore the return value of the runSystem command.")
NORMAL (U"Thus, to make sure that the directory contains no WAV files, you would write")
CODE (U"#runSystem_nocheck: \"rm \", directory\\$ , \"/*.wav\"")
ENTRY (U"Getting the values of system variables")
TAG (U"##environment\\$  (#%%symbol-string%#)")
DEFINITION (U"returns the value of an environment variable, e.g.")
CODE1 (U"homeDirectory\\$  = ##environment\\$ # (\"HOME\")")
ENTRY (U"Getting system duration")
TAG (U"##stopwatch")
DEFINITION (U"returns the time that has elapsed since the previous #stopwatch.")
NORMAL (U"Here is a Praat script that measures how long it takes to do a million assignments:")
CODE (U"stopwatch")
CODE (U"for i to 1000000")
	CODE1 (U"a = 1.23456789e123")
CODE (U"endfor")
CODE (U"time = stopwatch")
CODE (U"writeInfoLine: a, \" \", fixed\\$  (time, 3)")
MAN_END

MAN_BEGIN (U"Scripting 6.6. Controlling the user", U"ppgb", 20170317)
INTRO (U"You can temporarily halt a Praat script:")
TAG (U"#pauseScript: %message")
DEFINITION (U"suspends execution of the script, and allows the user to interrupt it. "
	"A message window will appear with the %message (you can use the same argument list as with #writeInfoLine) and the buttons Stop and Continue:")
CODE (U"#pauseScript: \"The next file will be \", fileName\\$ ")
NORMAL (U"The pauseScript function is useful if you want to send a simple message to the user, "
	"and you only want to ask the user whether she wants to proceed or not. "
	"More interesting interactions between your script and the user are possible with the %%pause window%. "
	"In a pause window you can include the same kinds of arguments as in a @@Scripting 6.1. Arguments to the script|form@. "
	"Here is an extensive example:")
CODE (U"writeInfoLine: \"script\"")
CODE (U"compression = 1")
CODE (U"number_of_channels = 2")
CODE (U"worth = 3")
CODE (U"for i to 5")
	CODE1 (U"#beginPause: \"Hi\"")
		CODE2 (U"#comment: \"Type a lot of nonsense below.\"")
		CODE2 (U"#natural: \"Number of people\", 10")
		CODE2 (U"#real: \"Worth\", worth + 1")
		CODE2 (U"#positive: \"Sampling frequency (Hz)\", \"44100.0 (= CD quality)\"")
		CODE2 (U"#word: \"hi\", \"hhh\"")
		CODE2 (U"#sentence: \"lo\", \"two words\"")
		CODE2 (U"#text: \"ko\", \"jkgkjhkj g gdfg dfg\"")
		CODE2 (U"#boolean: \"You like it?\", 1")
		CODE2 (U"if worth < 6")
			CODE3 (U"#choice: \"Compression\", compression")
				CODE4 (U"#option: \"lossless (FLAC)\"")
				CODE4 (U"#option: \"MP3\"")
				CODE4 (U"#option: \"Ogg\"")
		CODE2 (U"endif")
		CODE2 (U"#optionMenu: \"Number of channels\", number_of_channels")
			CODE3 (U"#option: \"mono\"")
			CODE3 (U"#option: \"stereo\"")
			CODE3 (U"#option: \"quadro\"")
		CODE2 (U"#comment: \"Then click Stop or one of the continuation buttons.\"")
	CODE1 (U"clicked = #endPause: \"Continue\", \"Next\", \"Proceed\", 2")
	CODE1 (U"appendInfoLine: number_of_people, \" \", worth, \" \", sampling_frequency, \" \", clicked")
	CODE1 (U"appendInfoLine: \"Compression: \", compression, \" (\", compression\\$ ")
	CODE1 (U"appendInfoLine: \"Number of channels: \", number_of_channels\\$ ")
CODE (U"endfor")
NORMAL (U"This example uses several tricks. A useful one is seen with %number_of_channels: "
	"this is at the same time the value that is passed to #optionMenu (and therefore determines the setting of "
	"the \"Number of channels\" menu when the window appears) and the name of the variable in which the user's "
	"chosen value of \"Number of channels\" is stored (because \"number_of_channels\" is what you get "
	"by replacing the spaces in \"Number of channels\" with underscores and turning its first letter to lower case.")
NORMAL (U"Your own pause windows are not likely to be as rich as the above example. "
	"For instance, the example has three continuation buttons (the second of these is the default button, "
	"i.e. the button that you can \"click\" by pressing the Enter or Return key). "
	"You will often use only one continuation button, for instance")
CODE (U"#endPause: \"Continue\", 1")
NORMAL (U"or")
CODE (U"#endPause: \"Finish\", 1")
NORMAL (U"or")
CODE (U"#endPause: \"OK\", 1")
NORMAL (U"If your script shows multiple different pause windows, then it is in fact a %wizard, "
	"and it becomes useful to have")
CODE (U"#endPause: \"Next\", 1")
NORMAL (U"for most of them, and")
CODE (U"#endPause: \"Finish\", 1")
NORMAL (U"for the last one.")
NORMAL (U"The possibility of multiple continuation buttons can save the user a mouse click. "
	"The following script, for instance, requires two mouse clicks per sound:")
CODE (U"for i to 20")
	CODE1 (U"Read from file: \"sound\" + string\\$  (i) + \".wav\"")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"#beginPause: \"Rate the quality\"")
		CODE2 (U"#comment: \"How good is the sound on a scale from 1 to 7?\"")
		CODE2 (U"#choice: \"Quality\", 4")
			CODE3 (U"#option: \"1\"")
			CODE3 (U"#option: \"2\"")
			CODE3 (U"#option: \"3\"")
			CODE3 (U"#option: \"4\"")
			CODE3 (U"#option: \"5\"")
			CODE3 (U"#option: \"6\"")
			CODE3 (U"#option: \"7\"")
	CODE1 (U"#endPause: if i = 20 then \"Finish\" else \"Next\" fi, 1")
	CODE1 (U"appendInfoLine: quality")
CODE (U"endfor")
NORMAL (U"The following script works faster:")
CODE (U"for i to 20")
	CODE1 (U"Read from file: \"sound\" + string\\$  (i) + \".wav\"")
	CODE1 (U"Play")
	CODE1 (U"Remove")
	CODE1 (U"#beginPause: \"Rate the quality\"")
		CODE2 (U"#comment: \"How good is the sound on a scale from 1 to 7?\"")
	CODE1 (U"quality = #endPause: \"1\", \"2\", \"3\", \"4\", \"5\", \"6\", \"7\", 0")
	CODE1 (U"appendInfoLine: quality")
CODE (U"endfor")
NORMAL (U"In this example, the 0 at the end of #endPause means that there is no default button.")
ENTRY (U"File selection")
NORMAL (U"If you want the user to choose a file name for reading (opening), do")
CODE (U"fileName\\$  = ##chooseReadFile\\$ #: \"Open a table file\"")
CODE (U"if fileName\\$  <> \"\"")
	CODE1 (U"table = Read Table from tab-separated file: fileName\\$ ")
CODE (U"endif")
NORMAL (U"A file selector window will appear, with (in this example) \"Open a table file\" as the title. "
	"If the user clicks #OK, the variable $$fileName\\$ $ will contain the name of the file that the user selected; "
	"if the user clicks #Cancel, the variable $$fileName\\$ $ will contain the empty string (\"\").")
NORMAL (U"If you want the user to choose a file name for writing (saving), do")
CODE (U"selectObject: mySound")
CODE (U"fileName\\$  = ##chooseWriteFile\\$ #: \"Save as a WAV file\", \"mySound.wav\"")
CODE (U"if fileName\\$  <> \"\"")
	CODE1 (U"Save as WAV file: fileName\\$ ")
CODE (U"endif")
NORMAL (U"A file selector window will appear, with (in this example) \"Save as a WAV file\" as the title "
	"and \"mySound.wav\" as the suggested file name (which the user can change). "
	"If the user clicks #OK, the form will ask for confirmation if the file name that the user typed already exists. "
	"If the user clicks #OK with a new file name, or clicks #OK in the confirmation window, "
	"the variable $$fileName\\$ $ will contain the file name that the user typed; "
	"if the user clicks #Cancel at any point, the variable $$fileName\\$ $ will contain the empty string (\"\").")
NORMAL (U"If you want the user to choose a directory (folder) name, do")
CODE (U"directoryName\\$  = ##chooseDirectory\\$ #: \"Choose a directory to save all the new files in\"")
CODE (U"if directoryName\\$  <> \"\"")
	CODE1 (U"for i to numberOfSelectedSounds")
		CODE2 (U"selectObject: sound [i]")
		CODE2 (U"Save as WAV file: directoryName\\$  + \"/sound\" + string\\$  (i) + \".wav\"")
	CODE1 (U"endfor")
CODE (U"endif")
NORMAL (U"A directory selector window will appear, with (in this example) \"Choose a directory to save all the new files in\" as the title. "
	"If the user clicks #OK, the variable $$directoryName\\$ $ will contain the name of the directory that the user selected; "
	"if the user clicks #Cancel, the variable $$directoryName\\$ $ will contain the empty string (\"\").")
ENTRY (U"A non-pausing pause window without a Stop button")
NORMAL (U"Especially if you use the pause window within the @@Demo window@, you may not want to give the user the capability of "
	"ending the script by hitting #Stop or closing the pause window. In that case, you can add an extra argument to #endPause "
	"that denotes the cancel button:")
CODE (U"#beginPause: \"Learning settings\"")
	CODE1 (U"#positive: \"Learning rate\", \"0.01\"")
	CODE1 (U"#choice: \"Directions\", 3")
		CODE2 (U"#option: \"Forward\"")
		CODE2 (U"#option: \"Backward\"")
		CODE2 (U"#option: \"Bidirectional\"")
CODE (U"clicked = #endPause: \"Cancel\", \"OK\", 2, 1")
CODE (U"if clicked = 2")
CODE1 (U"learningRate = learning_rate")
CODE1 (U"includeForward = directions = 1 or directions = 3")
CODE1 (U"includeBackward = directions = 2 or directions = 3")
CODE (U"endif")
NORMAL (U"In this example, the default button is 2 (i.e. #OK), and the cancel button is 1 (i.e. #Cancel). "
	"The form will now contain no #Stop button, and if the user closes the window, "
	"this will be the same as clicking #Cancel, namely that $clicked will be 1 (because the Cancel button is the first button) "
	"and the variables $$learning_rate$, $directions and $$directions\\$ $ will not be changed (i.e. they might remain undefined).")
ENTRY (U"Pausing for a fixed time without a window")
NORMAL (U"You can pause Praat for 1.3 seconds by saying")
CODE (U"sleep (1.3)")
NORMAL (U"This is of course not about controlling the user, "
	"but it is mentioned here because this section is about pausing.")
MAN_END

MAN_BEGIN (U"Scripting 6.7. Sending a message to another program", U"ppgb", 20151020)
NORMAL (U"To send messages to running programs that use the Praat shell, "
	"use $sendpraat (see @@Scripting 8. Controlling Praat from another program@).")
NORMAL (U"To send a message to another running program that listens to a socket, "
	"you can use the $sendsocket directive. This works on Linux and Windows only.")
ENTRY (U"Example")
NORMAL (U"Suppose we are in the Praat-shell program #Praat, which is a system for doing phonetics by computer. "
	"From this program, we can send a message to the %%non%-Praat-shell program #MovieEdit, "
	"which does know how to display a sound file:")
CODE (U"Save as file: \"hallo.wav\"")
CODE (U"sendsocket fonsg19.hum.uva.nl:6667 display hallo.wav")
NORMAL (U"In this example, $$fonsg19.hum.uva.nl$ is the computer on which MovieEdit is running; "
	"you can specify any valid Internet address instead, as long as that computer allows you to send messages to it. "
	"If MovieEdit is running on the same computer as Praat, you can specify $localhost instead of the full Internet address.")
NORMAL (U"The number 6667 is the port number on which MovieEdit is listening. Other programs will use different port numbers.")
MAN_END

MAN_BEGIN (U"Scripting 6.8. Messages to the user", U"ppgb", 20170718)
NORMAL (U"If the user makes a mistake (e.g. types conflicting settings into your form window), "
	"you can use the #exitScript function (@@Scripting 5.9. Quitting|\\SS5.8@) "
	"to stop the execution of the script with an error message:")
CODE (U"form My analysis")
	CODE1 (U"real Starting_time_(s) 0.0")
	CODE1 (U"real Finishing_time_(s) 1.0")
CODE (U"endform")
CODE (U"if finishing_time <= starting_time")
	CODE1 (U"#exitScript: \"The finishing time should exceed \", starting_time, \" seconds.\"")
CODE (U"endif")
CODE (U"\\#  Proceed with the analysis...")
NORMAL (U"For things that should not normally go wrong, you can use the #assert directive:")
CODE (U"power = Get power")
CODE (U"assert power > 0")
NORMAL (U"This is the same as:")
CODE (U"if (power > 0) = undefined")
	CODE1 (U"exitScript: \"Assertion failed in line \", lineNumber, \" (undefined): power > 0\"")
CODE (U"elsif not (power > 0)")
	CODE1 (U"exitScript: \"Assertion failed in line \", lineNumber, \" (false): power > 0\"")
CODE (U"endif")
NORMAL (U"You can prevent Praat from issuing warning messages:")
CODE (U"nowarn Save as WAV file: \"hello.wav\"")
NORMAL (U"This prevents warning messages about clipped samples, for instance.")
NORMAL (U"You can also prevent Praat from showing a progress window:")
CODE (U"noprogress To Pitch: 0, 75, 500")
NORMAL (U"This prevents the progress window from popping up during lengthy operations. "
	"Use this only if you want to prevent the user from stopping the execution of the script.")
NORMAL (U"Finally, you can make Praat ignore error messages:")
CODE (U"nocheck Remove")
NORMAL (U"This would cause the script to continue even if there is nothing to remove.")
MAN_END

MAN_BEGIN (U"Scripting 6.9. Calling from the command line", U"ppgb", 20151031)
INTRO (U"Previous sections of this tutorial have shown you how to run a Praat script from the Script window. "
	"However, you can also call a Praat script from the command line (text console) instead. "
	"Information that would normally show up in the Info window, then goes to %stdout, "
	"and error messages go to %stderr. "
	"You cannot use commands in your script that create windows, such as ##View & Edit#. "
	"Before describing how to achieve this (from section 4 below on), we first describe "
	"how the normal Praat, with its usual Objects and Picture (and perhaps Info) window, "
	"can be started from the command line.")

ENTRY (U"1. Starting Praat from the command line")
NORMAL (U"Before seeing how a Praat script can be called from the command line, "
	"you should first know that just calling Praat from the command line just starts up Praat "
	"with its usual GUI (Graphical User Interface), i.e. with its two windows. "
	"For instance, on Windows you can start the Command Prompt window (the \"Console\"), and type")
CODE (U"\"C:\\bsProgram Files\\bsPraat.exe\"")
NORMAL (U"(including the quotes) if Praat.exe is indeed in the folder $$C:\\bsProgram Files$.")
NORMAL (U"On the Mac, the executable is hidden inside the $$app$ file, so you open a Terminal window "
	"and type something like")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat")
NORMAL (U"On Linux, you type into the Terminal something like")
CODE (U"/usr/bin/praat")

ENTRY (U"2. Calling Praat to open data files")
NORMAL (U"On Windows, you can open Praat with a sound file and a TextGrid file by typing")
CODE (U"\"C:\\bsProgram Files\\bsPraat.exe\" --open data\\bshello.wav data\\bshello.TextGrid")
NORMAL (U"or")
CODE (U"\"C:\\bsProgram Files\\bsPraat.exe\" --open data/hello.wav data/hello.TextGrid")
NORMAL (U"at least if your current directory (see the Console's $$cd$ and $$dir$ commands) "
	"contains the folder $$data$ and that folder contains those two files. "
	"Praat will start up, and shows the two files as a Sound and a TextGrid object in the list. "
	"If Praat was already running when you typed the command, "
	"the two files are added as objects to the existing list in Praat.")
NORMAL (U"On the Mac, you do")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --open data/hello.wav data/hello.TextGrid")
NORMAL (U"and on Linux")
CODE (U"/usr/bin/praat --open data/hello.wav data/hello.TextGrid")

ENTRY (U"3. Calling Praat to open a script")
NORMAL (U"On Windows, when you type")
CODE (U"\"C:\\bsProgram Files\\bsPraat.exe\" --open \"my script.praat\"")
NORMAL (U"Praat will start up, opening the script $$my script.praat$$ in a script window. "
	"If Praat was already running when you typed the command, "
	"the script window will appear within the already running instantiation of Praat.")
NORMAL (U"On the Mac, you do")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --open \"my script.praat\"")
NORMAL (U"and on Linux")
CODE (U"/usr/bin/praat --open \"my script.praat\"")
NORMAL (U"Note that on all three platforms, you have to supply quotes around the file name "
	"if that file name contains one or more spaces, as here between $$my$ and $$script$ "
	"or above between $$Program$ and $$Files$. This is because the script languages of "
	"the Console or Terminal use spaces for separating commands and arguments.")

ENTRY (U"4. Calling Praat to run a script")
NORMAL (U"Now we are ready to discuss how to run Praat without a GUI.")
NORMAL (U"On Windows, when you type")
CODE (U"\"C:\\bsProgram Files\\bsPraat.exe\" --run \"my script.praat\"")
NORMAL (U"Praat will execute the script $$my script.praat$$ without showing Praat's GUI, "
	"i.e. without showing its usual two windows. "
	"In fact, any output that would normally go to the Info window, "
	"will now go directly to the Console window in which you typed the command. "
	"If Praat was already running when you typed the command, "
	"its windows will not be affected. In fact, the GUI-instantiation of Praat and the Console-instantiation "
	"can run simultaneously without them noticing each other's existence; "
	"moreover, multiple Console-instantiations of Praat can run simultaneously, each in their own Console.")
NORMAL (U"On the Mac, you type")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --run \"my script.praat\"")
NORMAL (U"and on Linux")
CODE (U"/usr/bin/praat --run \"my script.praat\"")
NORMAL (U"What happens on all platforms is that the Console or Terminal starts up Praat, "
	"then Praat executes the script, and then Praat closes itself.")

ENTRY (U"5. Calling Praat to run a script with arguments")
NORMAL (U"Consider the following script:")
CODE (U"form Test command line calls")
CODE1 (U"sentence First_text I love you")
CODE1 (U"real Beep_duration 0.4")
CODE1 (U"sentence Second_text Me too")
CODE (U"endform")
CODE (U"")
CODE (U"writeInfoLine: \"She: \"\"\", first_text\\$ , \"\"\"\"")
CODE (U"appendInfoLine: \"He: \"\"\", second_text\\$ , \"\"\"\"")
CODE (U"")
CODE (U"synth1 = Create SpeechSynthesizer: \"English\", \"f1\"")
CODE (U"Play text: first_text\\$ ")
CODE (U"Create Sound as pure tone: \"beep\", 1, 0.0, beep_duration,")
CODE (U"... 44100, 440, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
CODE (U"synth2 = Create SpeechSynthesizer: \"English\", \"m1\"")
CODE (U"Play text: second_text\\$ ")
NORMAL (U"When you run this script from within Praat, it writes two lines to the Info window "
	"and plays first a female voice speaking the first sentence, then a beep, and then a male voice "
	"speaking the second sentence. To make this happen from the Windows command line instead, you type")
CODE (U"\"C:\\bsProgram Files\\bsPraat.exe\" --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"In the Mac terminal, you type")
CODE (U"/Applications/Praat.app/Contents/MacOS/Praat --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"and in the Linux terminal, you do")
CODE (U"/usr/bin/praat --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"Note that each argument that contains one or more spaces has to be put within quotes, "
	"on all three platforms. As with #runScript, Praat will not present a form window, "
	"but simply run the script with the arguments given on the command line "
	"(see @@Scripting 6.1. Arguments to the script@).")
NORMAL (U"What then happens on all three platforms is that a console instantiation of Praat writes "
	"the two lines to the Console window and plays the three sounds.")

ENTRY (U"6. Calling Praat from other programs such as Python")
NORMAL (U"You can run the above script from several programming languages, not just from a Console or Terminal. "
	"In Python, for instance, you can do it using the same syntax as you would use in the Console or Terminal:")
CODE (U"import os")
CODE (U"os.system ('\"C:\\bs\\bsProgram Files\\bs\\bsPraat.exe\" --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"')")
NORMAL (U"Note that you have to double the backslashes!")
NORMAL (U"A disadvantage of the $$os.system$ method is that you have to use quotes within quotes. "
	"A somewhat cleaner approach is:")
CODE (U"import subprocess")
CODE (U"subprocess.call(['C:\\bs\\bsProgram Files\\bs\\bsPraat.exe', '--run', 'testCommandLineCalls.praat', 'I love you', '0.4', 'Me too'])")
NORMAL (U"This way you specify the arguments directly, with quotes only because they are all strings, "
	"but without having to worry about spaces. And perhaps even more importantly, this syntax "
	"makes it easy to use variables as arguments, as in:")
CODE (U"first_line = 'I love you'")
CODE (U"second_line = 'me too'")
CODE (U"subprocess.call(['C:\\bs\\bsProgram Files\\bs\\bsPraat.exe', '--run', 'testCommandLineCalls.praat', first_line, '0.4', second_line])")
NORMAL (U"Many other programs beside Python have a $$system$-like command, so that you can run a command like")
CODE (U"system ('\"C:\\bs\\bsProgram Files\\bs\\bsPraat.exe\" --run testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"')")

ENTRY (U"7. What happens if I specify neither --open nor --run?")
NORMAL (U"If you specify neither $$--open$ nor $$--run$, Praat's behaviour is not guaranteed. "
	"If you type something like")
CODE (U"praat testCommandLineCalls.praat \"I love you\" 0.4 \"Me too\"")
NORMAL (U"into a Console or Terminal window by hand, Praat will typically run the script. "
	"Also, the $$--run$ option can probably be left out from the Python call above. "
	"However, if you redirect the output of Praat to a file or pipe, you cannot typically leave out the $$--run$ option; "
	"if you do, Praat may start its GUI and %open the file rather than run it.")

ENTRY (U"8. Running Praat interactively from the command line")
NORMAL (U"On the Mac and Linux, you have the possibility of running the program interactively from the command line:")
CODE (U"> /usr/bin/praat -")
NORMAL (U"You can then type in any of the fixed and dynamic commands, and commands that handle object selection, "
	"such as #selectObject. This method also works in pipes:")
CODE (U"> echo \"Report memory use\" | /usr/bin/praat -")

ENTRY (U"9. Calling Praat from a web server")
NORMAL (U"If you call Praat from a web server, you typically do not want to read and write its preferences and buttons files. "
	"To achieve this, you use the ##--no-pref-files# command line option before the script name:")
CODE (U"system ('/users/apache/praat --run --no-pref-files /user/apache/scripts/computeAnalysis.praat 1234 blibla')")

ENTRY (U"10. All command line options")
TAG (U"##--open")
DEFINITION (U"Interpret the command line arguments as files to be opened in the GUI.")
TAG (U"##--run")
DEFINITION (U"Interpret the command line arguments as a script file name and its arguments.")
TAG (U"##--no-pref-files#")
DEFINITION (U"Ignore the preferences file and the buttons file at start-up, and don't write them when quitting (see above).")
TAG (U"##--no-plugins#")
DEFINITION (U"Don't activate the plugins at start-up.")
TAG (U"##--pref-dir=#/var/www/praat_plugins")
DEFINITION (U"Set the preferences directory to /var/www/praat_plugins (for instance). "
	"This can come in handy if you require access to preference files and/or plugins that are not in your home directory.")
TAG (U"##--version")
DEFINITION (U"Print the Praat version.")
TAG (U"##--help")
DEFINITION (U"Print this list of command line options.")
TAG (U"##-a#, ##--ansi#")
DEFINITION (U"On Windows: use ISO Latin-1 encoding instead of the Console's native UTF-16 Little Endian encoding. "
	"This is not recommended, but might be necessary if you want to use Praat in a pipe "
	"or with redirection to a file.")
MAN_END

MAN_BEGIN (U"Scripting 7. Scripting the editors", U"ppgb", 20040222)
NORMAL (U"With a Praat script, you can automatize your work in the editors.")
NORMAL (U"#Warning: if the purpose of your script is to get information about "
	"analyses (pitch, formants, intensity, spectrogram) from the Sound, "
	"we do %not advise to script the Sound editor window. "
	"It is much simpler, faster, and more reproducible to create the analyses "
	"with the commands of the @@dynamic menu@, then use the Query commands of the dynamic menu "
	"to extract information from the analyses. This also applies if you want to use a TextGrid "
	"to determine the times at which you want to query the analyses. "
	"See @@Scripting examples@.")
LIST_ITEM (U"@@Scripting 7.1. Scripting an editor from a shell script@ (editor/endeditor)")
LIST_ITEM (U"@@Scripting 7.2. Scripting an editor from within@")
MAN_END

MAN_BEGIN (U"Scripting 7.1. Scripting an editor from a shell script", U"ppgb", 20140526)
NORMAL (U"From a Praat shell script, you can switch to an editor and back again:")
CODE (U"sound\\$  = \"hallo\"")
CODE (U"start = 0.3")
CODE (U"finish = 0.7")
CODE (U"sound = Read from file: sound\\$  + \".aifc\"")
CODE (U"View & Edit")
CODE (U"#editor: sound")
	CODE1 (U"Zoom: start, finish")
CODE (U"#endeditor")
CODE (U"Play")
NORMAL (U"This script reads a sound file from disk, pops up an editor for the resulting object, "
	"makes this editor zoom in on the part between 0.3 and 0.7 seconds, "
	"and returns to the Praat shell to play the entire sound.")
NORMAL (U"After #editor you can either give the unique id of the object, as above, or its name:")
CODE (U"#editor: \"Sound \" + sound\\$ ")
MAN_END

MAN_BEGIN (U"Scripting 7.2. Scripting an editor from within", U"ppgb", 20140113)
NORMAL (U"This section will show how you can permanently extend the functionality of an editor.")
NORMAL (U"As an example, consider the following problem: you want to see a graphic representation "
	"of the spectrum of the sound around the cursor position in the SoundEditor. To achieve this, "
	"follow these steps:")
LIST_ITEM (U"1. Create a Sound.")
LIST_ITEM (U"2. View it in a SoundEditor by clicking @@View & Edit@.")
LIST_ITEM (U"3. Choose ##New editor script# from the @@File menu@ in the SoundEditor. The resulting @ScriptEditor "
	"will have a name like \"untitled script [Sound hallo]\".")
LIST_ITEM (U"4. Type the following lines into the ScriptEditor:")
CODE2 (U"cursor = Get cursor")
CODE2 (U"Select: cursor - 0.02, cursor + 0.02")
CODE2 (U"Extract selected sound (windowed): \"slice\", \"Kaiser2\", 2, \"no\"")
CODE1 (U"#endeditor")
CODE1 (U"To Spectrum: \"yes\"")
CODE1 (U"View & Edit")
NORMAL (U"If you choose #Run from the #Run menu in the ScriptEditor, a region of 40 milliseconds around the "
	"current cursor position in the SoundEditor will become selected. This piece will be copied to the list of objects, "
	"after applying a double Kaiser window (total length 80 ms). Thus, a Sound named \"slice\" will appear in the list. "
	"Subsequently, a Spectrum object also called \"slice\" will appear in the list, and a SpectrumEditor titled "
	"\"Spectrum slice\" will finally appear on your screen.")
LIST_ITEM (U"5. Save the script to disk, e.g. as /us/miep/spectrum.praat. The title of the ScriptEditor will change accordingly.")
LIST_ITEM (U"6. Since you will want this script to be available in all future SoundEditors, you choose ##Add to menu...# from the #File menu. "
	"For the %Window, you specify \"SoundEditor\" (this is preset). For the %Menu, you may want to choose \"Spectrum\" "
	"instead of the preset value (\"File\"). For the name of the %Command, you type something like \"Show spectrum at cursor\" "
	"(instead of \"Do it...\"). Then you click #OK.")
NORMAL (U"The command will be visible in every SoundEditor that you create from now on. "
	"To see this, close the one visible SoundEditor, select the original Sound, choose ##View & Edit# again, and inspect the #Spectrum menu. "
	"You can now view the spectrum around the cursor just by choosing this menu command.")
NORMAL (U"After you leave Praat and start it again, the command will continue to appear in the SoundEditor. "
	"If you don't like the command any longer, you can remove it with the @ButtonEditor, which you can start "
	"by choosing #Buttons from the #Preferences submenu of the @@Praat menu@.")
ENTRY (U"Improving your script")
NORMAL (U"The above spectrum-viewing example has a number of disadvantages. It clutters the object list with a number "
	"of indiscriminable Sounds and Spectra called \"slice\", and the spectrum is shown up to the Nyquist frequency "
	"while we may just be interested in the lower 5000 Hz. Furthermore, the original selection in the SoundEditor is lost.")
NORMAL (U"To improve the script, we open it again with ##Open editor script...# from the #File menu in the SoundEditor. After every change, "
	"we can run it with #Run from the #Run menu again; alternatively, we could save it (with #Save from the #File menu) and choose our new "
	"\"Show spectrum at cursor\" button (this button will always run the version on disk, never the one viewed in a ScriptEditor).")
NORMAL (U"To zoom in on the first 5000 Hz, we add the following code at the end of our script:")
CODE (U"#editor: \"Spectrum slice\"")
	CODE1 (U"Zoom: 0, 5000")
NORMAL (U"To get rid of the \"Sound slice\", we can add:")
CODE (U"#endeditor")
CODE (U"#removeObject: \"Sound slice\"")
NORMAL (U"Note that #$endeditor is needed to change from the environment of a SpectrumEditor to the environment of the object & picture windows.")
NORMAL (U"If you now choose the \"Show spectrum at cursor\" button for several cursor positions, you will notice that all those editors have the same name. "
	"To remedy the ambiguity of the line $$#editor Spectrum slice$, we give each slice a better name. For example, if the cursor was at "
	"635 milliseconds, the slice could be named \"635ms\". We can achieve this by changing the extraction in the following way:")
CODE (U"milliseconds = round (cursor*1000)")
CODE (U"Extract selection sound (windowed): string\\$  (milliseconds) + \"ms\", \"Kaiser2\", 2, \"no\"")
NORMAL (U"The names of the Sound and Spectrum objects will now have more chance of being unique. Two lines will have to be edited trivially.")
NORMAL (U"Finally, we will reset the selection to the original. At the top of the script, we add two lines to remember the positions of the selection markers:")
CODE (U"start = Get start of selection")
CODE (U"end = Get end of selection")
NORMAL (U"At the bottom, we reset the selection:")
CODE (U"#editor")
CODE1 (U"Select: start, end")
NORMAL (U"Note that the #$editor directive if not followed by the name of an editor, returns the script to the original environment.")
NORMAL (U"The complete script is:")
	CODE1 (U"start = Get start of selection")
	CODE1 (U"end = Get end of selection")
	CODE1 (U"cursor = Get cursor")
	CODE1 (U"Select: cursor - 0.02, cursor + 0.02")
	CODE1 (U"\\#  Create a name. E.g. \"670ms\" means at 670 milliseconds.")
	CODE1 (U"milliseconds = round (cursor*1000)")
	CODE1 (U"Extract windowed selection: string\\$  (milliseconds) + \"ms\", \"Kaiser2\", 2, \"no\"")
CODE (U"#endeditor")
CODE (U"To Spectrum: \"yes\"")
CODE (U"View & Edit")
CODE (U"#editor: \"Spectrum \" + string\\$  (milliseconds) + \"ms\"")
	CODE1 (U"Zoom: 0, 5000")
CODE (U"#endeditor")
CODE (U"#removeObject: \"Sound \" + string\\$  (milliseconds) + \"ms\"")
CODE (U"#editor")
	CODE1 (U"Select: start, end")
NORMAL (U"This script is useful as it stands. It is good enough for safe use. For instance, if the created Sound object has the same name "
	"as an already existing Sound object, it will be the newly created Sound object that will be removed by #removeObject, "
	"because in case of ambiguity #removeObject always removes the most recently created object of that name.")
MAN_END

MAN_BEGIN (U"sendpraat", U"ppgb", 20000927)
NORMAL (U"See @@Scripting 8. Controlling Praat from another program@.")
MAN_END

MAN_BEGIN (U"Scripting 8. Controlling Praat from another program", U"ppgb", 20021218)
LIST_ITEM (U"@@Scripting 8.1. The sendpraat subroutine")
LIST_ITEM (U"@@Scripting 8.2. The sendpraat program")
LIST_ITEM (U"@@Scripting 8.3. The sendpraat directive")
MAN_END

MAN_BEGIN (U"Scripting 8.1. The sendpraat subroutine", U"ppgb", 20151020)
INTRO (U"A subroutine for sending messages to a %running Praat. "
	"Also a Windows console, MacOS, or Linux console program with the same purpose.")
ENTRY (U"Syntax")
LIST_ITEM (U"##sendpraat (void *#%display##, const char *#%program##, long #%timeOut##, char *#%text##);")
ENTRY (U"Arguments")
TAG (U"%display")
DEFINITION (U"the display pointer if the subroutine is called from a running X program; "
	"if null, sendpraat will open the display by itself. On Windows and Macintosh, "
	"this argument is ignored.")
TAG (U"%program")
DEFINITION (U"the name of a running program that uses the Praat shell, e.g. \"Praat\" or \"ALS\". "
	"The first letter may be specified as lower or upper case; it will be converted "
	"to upper case for Windows or MacOS and to lower case for Linux.")
TAG (U"%timeOut (MacOS and Linux only)")
DEFINITION (U"the number of seconds that sendpraat will wait for an answer "
	"before writing an error message. A %timeOut of 0 means that "
	"the message will be sent asynchronously, i.e., that sendpraat "
	"will return immediately without issuing any error message.")
TAG (U"%text")
DEFINITION (U"the script text to be sent. Sendpraat may alter this text!")
ENTRY (U"Example 1: killing a program")
CODE (U"char message [100], *errorMessage;")
CODE (U"strcpy (message, \"Quit\");")
CODE (U"errorMessage = #sendpraat (NULL, \"praat\", 0, message);")
CODE (U"if (errorMessage) fprintf (stderr, \"\\% s\", errorMessage);")
NORMAL (U"This causes the program #Praat to quit (gracefully), because #Quit is a fixed "
	"command in one of the menus of that program. "
	"On MacOS and Linux, sendpraat returns immediately; on Windows, the %timeOut argument is ignored. "
	"The return value %errorMessage is a statically allocated string internal to sendpraat, "
	"and is overwritten by the next call to sendpraat.")
ENTRY (U"Example 2: playing a sound file in reverse")
NORMAL (U"Suppose you have a sound file whose name is in the variable $fileName, "
	"and you want the program #Praat, which can play sounds, "
	"to play this sound backwards.")
CODE (U"char message [1000], *errorMessage;")
CODE (U"sprintf (message, \"Read from file... \\% s\\bsnPlay reverse\\bsnRemove\", fileName);")
CODE (U"errorMessage = #sendpraat (NULL, \"praat\", 1000, message);")
NORMAL (U"This will work because ##Play reverse# is an action command "
	"that becomes available in the dynamic menu when a Sound is selected. "
	"On Linux, sendpraat will allow #Praat at most 1000 seconds to perform this.")
ENTRY (U"Example 3: executing a large script file")
NORMAL (U"Sometimes, it may be unpractical to send a large script directly to #sendpraat. "
	"Fortunately, the receiving program knows #runScript:")
CODE (U"char message [100], *errorMessage;")
CODE (U"strcpy (message, \"runScript: \\bs\"doAll.praat\\bs\", 20\");")
CODE (U"errorMessage = #sendpraat (NULL, \"praat\", 0, message);")
NORMAL (U"This causes the program #Praat to run the script ##doAll.praat# with an argument of \"20\".")
ENTRY (U"How to download")
NORMAL (U"You can download the source code of the sendpraat subroutine and program "
	"via ##www.praat.org# or from ##http://www.fon.hum.uva.nl/praat/sendpraat.html#.")
ENTRY (U"Instead")
NORMAL (U"Instead of using sendpraat, you can also just take the following simple steps in your program:")
LIST_ITEM (U"1. on Linux, write the script that you want to run, and save it as ##~/.praat-dir/message#;")
LIST_ITEM (U"2. get Praat's process id from ##~/.praat-dir/pid#;")
LIST_ITEM (U"3. if Praat's process id is e.g. 1178, send it a SIGUSR1 signal: $$kill -USR1 1178")
NORMAL (U"If the first line of your script is the comment \"\\#  999\", where 999 stands for the process id of your program, "
	"Praat will send your program a SIGUSR2 signal back when it finishes handling the script.")
ENTRY (U"See also")
NORMAL (U"To start a program from the command line instead and sending it a message, "
	"you would not use #sendpraat, but instead run the program with a script file as an argument. "
	"See @@Scripting 6.9. Calling from the command line@.")
MAN_END

MAN_BEGIN (U"Scripting 8.2. The sendpraat program", U"ppgb", 20151020)
INTRO (U"A Windows console or Unix (MacOS, Linux) terminal program for sending messages to a %running Praat program.")
ENTRY (U"Syntax")
CODE (U"#sendpraat [%timeOut] %program %message...")
NORMAL (U"For the meaning of the arguments, see @@Scripting 8.1. The sendpraat subroutine|the sendpraat subroutine@.")
ENTRY (U"Example 1: killing a program")
CODE (U"sendpraat 0 praat Quit")
NORMAL (U"Causes the program #Praat to quit (gracefully), because #Quit is a fixed command in one of its menus. "
	"On Unix, #sendpraat returns immediately; on Windows, you leave out the %timeOut argument.")
ENTRY (U"Example 2: playing a sound file in reverse")
CODE (U"sendpraat 1000 praat \"Read from file... hello.wav\" \"Play reverse\" \"Remove\"")
NORMAL (U"This works because ##Play reverse# is an action command "
	"that becomes available in the dynamic menu of the #Praat program when a Sound is selected. "
	"On Unix, sendpraat will allow #Praat at most 1000 seconds to perform this.")
NORMAL (U"Each line is a separate argument. Lines that contain spaces should be put inside double quotes.")
ENTRY (U"Example 3: drawing")
CODE (U"sendpraat als \"for i from 1 to 5\" \"Draw circle: 0.5, 0.5, i\" \"endfor\"")
NORMAL (U"This causes the program #Als to draw five concentric circles into the Picture window.")
ENTRY (U"Example 4: running a large script")
CODE (U"sendpraat praat \"runScript: \\bs\"doAll.praat\\bs\", 20\"")
NORMAL (U"This causes the program #Praat to execute the script ##doAll.praat# with an argument of \"20\".")
MAN_END

MAN_BEGIN (U"Scripting 8.3. The sendpraat directive", U"ppgb", 20140112)
INTRO (U"Besides being a subroutine (@@Scripting 8.1. The sendpraat subroutine@) "
	"and a program (@@Scripting 8.2. The sendpraat program@), @sendpraat "
	"can also be called from within a Praat script.")
ENTRY (U"Example 1: killing a program")
NORMAL (U"Suppose we are in the Praat-shell program #Als, which is a browser for dictionaries, "
	"and we want to kill the Praat-shell program #Praat, which is a program for phonetics research:")
CODE (U"beginSendpraat: \"Praat\"")
CODE1 (U"Quit")
CODE (U"endSendpraat")
ENTRY (U"Example 2: playing a sound")
NORMAL (U"Suppose we are in the Praat-shell program #Als, which is a browser for dictionaries, "
	"and has no idea of what a %sound is. From this program, we can play a sound file "
	"by sending a message to the Praat-shell program #Praat, which does know about sounds:")
CODE (U"fileName\\$  = chooseReadFile\\$ : \"Play a sound file\"")
CODE (U"beginSendpraat: \"Praat\", \"fileName\\$ \"")
CODE1 (U"Read from file: fileName\\$ ")
CODE1 (U"Play")
CODE1 (U"Remove")
CODE (U"endSendpraat")
NORMAL (U"After #beginSendpraat, you first mention the name of the receiving program (here \"Praat\"), "
        "then the names of the variables you want the receiving program to know about.")
NORMAL (U"To have the receiving program return information to you, specify the variables that are to be handed back:")
CODE (U"fileName\\$  = chooseReadFile\\$ : \"Measure a sound file\"")
CODE (U"beginSendpraat: \"Praat\", \"fileName\\$ \"")
CODE1 (U"Read from file: fileName\\$ ")
CODE1 (U"duration = Get total duration")
CODE1 (U"Remove")
CODE (U"endSendpraat: \"duration\"")
CODE (U"writeInfoLine: \"That sound file lasts \", duration, \" seconds.\"")
MAN_END

/*
ENTRY (U"How to run a script")
NORMAL (U"You can run scripts from the @ScriptEditor. If you will have to use the script very often, "
	"it is advisable to create a button for it in the fixed menu or in a dynamic menu. See the "
	"@ScriptEditor manual page.")
NORMAL (U"(You can also run scripts from the command line. See @@Scripting 6.9. Calling from the command line|\\SS6.9@)")
*/

MAN_BEGIN (U"Scripting 9.1. Turning a script into a stand-alone program", U"ppgb", 20150713)
INTRO (U"You can turn your script into a double-clickable stand-alone program by including it into Praat's #main procedure. "
	"If you want to try this, you should already know how to compile and link the Praat program on your computer.")
NORMAL (U"These stand-alone programs do not show the Objects window and the Picture window; "
	"therefore, you will usually want to use @@Demo window@ commands in your script. Here is an example:")
CODE (U"\\# include \"praat.h\"")
CODE (U"")
CODE (U"const char32 myScript [ ] = U\"\"")
	CODE1 (U"\"demo Text: 0.5, \\bs\"centre\\bs\", 0.5, \\bs\"half\\bs\", \\bs\"Hello world\\bs\"\\bsn\"")
	CODE1 (U"\"demoWaitForInput ( )\\bsn\"")
CODE (U";")
CODE (U"")
CODE (U"int main (int argc, char *argv [ ]) {")
	CODE1 (U"praat_setStandAloneScriptText (myScript);")
	CODE1 (U"praat_init (U\"Hello\", argc, argv);")
	CODE1 (U"INCLUDE_LIBRARY (praat_uvafon_init)")
	CODE1 (U"praat_run ();")
	CODE1 (U"return 0;")
CODE (U"}")
NORMAL (U"The script in this example raises the Demo window, writes \"Hello world\" in the middle of the window, "
	"waits until the user clicks the mouse or presses a key, and then closes.")
NORMAL (U"Note that Praat is distributed under the General Public License (GPL). This means that if you distribute "
	"a Praat-based stand-alone program, you have to make it open source under the GPL as well.")
NORMAL (U"See also @@Programming with Praat@.")
ENTRY (U"Details")
NORMAL (U"Your program can save its preferences in a directory of its choice, "
	"e.g. in ##'preferencesDirectory\\$ '/../GuineaPigAnalyzer# if your program is called GuineaPigAnalyzer. "
	"If you want to be less conspicuous and like to use the Praat preferences directory instead, "
	"please use the ##apps# subdirectory, in this way:")
CODE (U"createDirectory: preferencesDirectory\\$  + \"/apps\"")
CODE (U"createDirectory: preferencesDirectory\\$  + \"/apps/GuineaPigAnalyzer\"")
MAN_END

MAN_BEGIN (U"Scripting 9.2. Old functions", U"ppgb", 20140112)
INTRO (U"The Praat scripting language improves and changes, but old scripts should continue to work correctly. "
	"Here are some examples of what you can see in old scripts, and what they mean:")
NORMAL (U"The meaning of")
CODE (U"echo Hello, my name is 'name\\$ ' and I am 'age' years old.")
NORMAL (U"is")
CODE (U"writeInfoLine: \"Hello, my name is \", name\\$ , \" and I am \", age, \" years old.\"")
NORMAL (U"The meaning of")
CODE (U"Draw... 0 0 0 0 yes Curve")
NORMAL (U"is")
CODE (U"Draw: 0, 0, 0, 0, \"yes\", \"Curve\"")
NORMAL (U"The meaning of")
CODE (U"Read from file... 'fileName\\$ '")
NORMAL (U"is")
CODE (U"Read from file: fileName\\$ ")
MAN_END

MAN_BEGIN (U"ScriptEditor", U"ppgb", 20140107)
INTRO (U"An aid to @@scripting@.")
NORMAL (U"The ScriptEditor is a text editor that allows you to edit, save, and run "
	"any @@Praat script@. You can type such a script from scratch, "
	"but it is sometimes easier to use the @@History mechanism@, which automatically records "
	"all your commands and mouse clicks, and which can paste these directly "
	"into a ScriptEditor.")
NORMAL (U"To add a script as a button to a fixed or dynamic menu, "
	"use @@Add to fixed menu...@ or @@Add to dynamic menu...@ from the @@File menu@.")
ENTRY (U"Example 1")
NORMAL (U"In this example, we create a fixed button that will play a 0.4-second sine wave with a specified frequency.")
NORMAL (U"First, we create a ScriptEditor by choosing @@New Praat script@ from the @@Praat menu@. "
	"Then, we choose @@Clear history@ from the #Edit menu in the ScriptEditor. "
	"We then perform some actions that will create a sine wave, play it, and remove it:")
LIST_ITEM (U"1. Choose ##Create Sound as pure tone...# from the @@New menu@ and click OK.")
LIST_ITEM (U"2. Click #Play in the dynamic menu.")
LIST_ITEM (U"3. Click the fixed #Remove button.")
NORMAL (U"We then choose @@Paste history@ from the #Edit menu in the ScriptEditor (or type Command-H). "
	"The text will now contain at least the following lines (delete any other lines):")
CODE (U"Create Sound as pure tone: \"tone\", 1, 0, 0.4, 44100, 440, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"We can run this script again by choosing #Run from the #Run menu (or typing Command-R). "
	"However, this always plays a sine with a frequency of 440 Hz, so we will add the variable \"Frequency\" "
	"to the script, which then looks like:")
CODE (U"#form Play a sine wave")
	CODE1 (U"#positive Frequency")
CODE (U"#endform")
CODE (U"Create Sound as pure tone: \"tone\", 1, 0, 0.4, 44100, frequency, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"When we choose #Run, the ScriptEditor will ask us to supply a value for the \"Frequency\" variable. "
	"We can now play 1-second sine waves with any frequency.")
NORMAL (U"It is advisable to supply a standard value for each argument in your script. "
	"If the duration should be variable, too, the final script could look like:")
CODE (U"#form Play a sine wave")
	CODE1 (U"#positive Frequency 440")
	CODE1 (U"#positive Duration 1.0")
CODE (U"#endform")
CODE (U"Create Sound as pure tone: \"tone\", 1, 0, duration, 44100, frequency, 0.2, 0.01, 0.01")
CODE (U"Play")
CODE (U"Remove")
NORMAL (U"When you run this script, the ScriptEditor will ask you to supply values for the two variables, "
	"but the values \"440\" and \"1.0\" are already visible in the form window, "
	"so that you will get a sensible result if you just click #OK.")
NORMAL (U"If this script is useful to you, you may want to put a button for it in the @@New menu@, "
	"in the ##Sound# submenu:")
LIST_ITEM (U"1. Save the script to a file, with #Save from the #File menu. The file name that you supply, will "
	"be shown in the title bar of the ScriptEditor window.")
LIST_ITEM (U"2. Choose @@Add to fixed menu...@ from the #File menu. Supply #Objects for the %window, "
	"#New for the %menu, \"Play sine wave...\" for the %command, "
	"##Create Sound from formula...# for %%after command%, and \"1\" for the depth (because it is supposed to be in a submenu); "
	"the %script argument has already been set to the file name that you supplied in step 1.")
LIST_ITEM (U"3. Click #OK and ensure that the button has been added in the @@New menu@. This button will still be there "
	"after you leave the program and enter it again; to remove it from the menu, use the @ButtonEditor.")
ENTRY (U"Example 2")
NORMAL (U"In this example, we will create a shortcut for the usual complex pitch-analysis command.")
NORMAL (U"First, we perform the required actions:")
LIST_ITEM (U"1. Select a Sound object.")
LIST_ITEM (U"2. Click ##To Pitch...# and set the arguments to your personal standard values.")
LIST_ITEM (U"3. Click #OK. A new #Pitch object will appear.")
NORMAL (U"We then paste the history into the ScriptEditor, after which this will contain at least a line like (delete all the other lines):")
CODE (U"To Pitch: 0.01, 150, 900")
NORMAL (U"You can run this script only after selecting one or more Sound objects.")
NORMAL (U"If this script is useful to you, you may want to put a button for it in the dynamic menu:")
LIST_ITEM (U"1. Save the script to a file, with #Save from the #File menu.")
LIST_ITEM (U"2. Choose @@Add to dynamic menu...@ from the #File menu. Supply \"Sound\" for %class1 "
	"(because the button is supposed to be available only if a Sound is selected), \"0\" for %number1 "
	"(because the command is supposed to work for any number of selected Sound objects), "
	"\"To Pitch (child)\" for the %command, "
	"\"To Spectrum\" for %%after command%, and \"0\" for the depth (because it is not supposed to be in a submenu); "
	"the %script argument has already been set to the file name that you supplied in step 1.")
LIST_ITEM (U"3. Click #OK and ensure that the button is clickable if you select one or more Sound objects. "
	"This button will still be available after you leave the program and enter it again; "
	"to remove it from the dynamic menus, use the @ButtonEditor.")
MAN_END

MAN_BEGIN (U"undefined", U"ppgb", 20140112)
INTRO (U"When you give a query command for a numeric value, Praat sometimes writes the numeric value ##--undefined--# "
	"into the @@Info window@ (two hyphens at both sides of the word). This happens if the value you ask for is not defined, "
	"as in the following examples:")
LIST_ITEM (U"\\bu You select a Sound with a finishing time of 1.0 seconds and ask for the minimum point in the wave form "
	"between 1.5 and 2.0 seconds (with the query command ##Get minimum...#).")
LIST_ITEM (U"\\bu You ask for a pitch value in a voiceless part of the sound (select a #Pitch, "
	"then choose ##Get value at time...#).")
LIST_ITEM (U"\\bu You type into the @Calculator the following formula: 10\\^ 400.")
ENTRY (U"Usage in a script")
NORMAL (U"In a Praat script, this value is simply represented as \"undefined\". You use it to test whether "
	"a query command returned a valid number:")
CODE (U"selectObject: \"Pitch hallo\"")
CODE (U"meanPitch = Get mean: 0.1, 0.2, \"Hertz\", \"Parabolic\"")
CODE (U"if meanPitch = undefined")
	CODE1 (U"\\#  Take some exceptional action.")
CODE (U"else")
	CODE1 (U"\\#  Take the normal action.")
CODE (U"endif")
ENTRY (U"Details for hackers")
NORMAL (U"In text files, this value is written as ##--undefined--#. "
	"In binary files, it is written as a big-endian IEEE positive infinity. "
	"In memory, it is the ANSI-C constant HUGE_VAL, which equals infinity on IEEE machines.")
MAN_END

MAN_BEGIN (U"Scripting examples", U"ppgb", 20040222)
INTRO (U"Here is a number of examples of how to use scripting in the Praat program. "
	"Refer to the @scripting tutorial when necessary.")
LIST_ITEM (U"@@Script for listing time\\--F0 pairs")
LIST_ITEM (U"@@Script for listing time\\--F0\\--intensity")
LIST_ITEM (U"@@Script for listing F0 statistics")
LIST_ITEM (U"@@Script for creating a frequency sweep")
LIST_ITEM (U"@@Script for onset detection")
LIST_ITEM (U"@@Script for TextGrid boundary drawing")
LIST_ITEM (U"@@Script for analysing pitch with a TextGrid")
MAN_END

MAN_BEGIN (U"Script for listing time\\--F0 pairs", U"ppgb", 20140223)
INTRO (U"“I wish to have a list of time markers in one column and F0 in the other. "
	"Those times that have no voiced data should be represented as “.” in the F0 column.”")
CODE (U"writeInfoLine: \"Time:    Pitch:\"")
CODE (U"numberOfFrames = Get number of frames")
CODE (U"for iframe to numberOfFrames")
	CODE1 (U"time = Get time from frame: iframe")
	CODE1 (U"pitch = Get value in frame: iframe, \"Hertz\"")
	CODE1 (U"if pitch = undefined")
		CODE2 (U"appendInfoLine: fixed\\$  (time, 6)")
	CODE1 (U"else")
		CODE2 (U"appendInfoLine: fixed\\$  (time, 6), \" \", fixed\\$  (pitch, 3)")
	CODE1 (U"endif")
CODE (U"endfor")
NORMAL (U"If you want to see this in a text file, you can copy and paste from the Info window, or save the Info window, "
	"or add a line to the script like")
CODE (U"appendFile: \"out.txt\", info\\$ ( )")
MAN_END

MAN_BEGIN (U"Script for listing time\\--F0\\--intensity", U"ppgb", 20140112)
INTRO (U"\"I want a list of pitch and intensity values at the same times.\"")
NORMAL (U"Since @@Sound: To Pitch...@ and @@Sound: To Intensity...@ do not give values at the same times, "
	"you create separate pitch and intensity contours with high time resolution, then interpolate. "
	"In the following example, you get pitch and intensity values at steps of 0.01 seconds "
	"by interpolating curves that have a time resolution of 0.001 seconds.")
CODE (U"sound = selected (\"Sound\")")
CODE (U"tmin = Get start time")
CODE (U"tmax = Get end time")
CODE (U"To Pitch: 0.001, 75, 300")
CODE (U"Rename: \"pitch\"")
CODE (U"selectObject: sound")
CODE (U"To Intensity: 75, 0.001")
CODE (U"Rename: \"intensity\"")
CODE (U"writeInfoLine: \"Here are the results:\"")
CODE (U"for i to (tmax-tmin)/0.01")
	CODE1 (U"time = tmin + i * 0.01")
	CODE1 (U"selectObject: \"Pitch pitch\"")
	CODE1 (U"pitch = Get value at time: time, \"Hertz\", \"Linear\"")
	CODE1 (U"selectObject: \"Intensity intensity\"")
	CODE1 (U"intensity = Get value at time: time, \"Cubic\"")
	CODE1 (U"appendInfoLine: fixed\\$  (time, 2), \" \", fixed\\$  (pitch, 3), \" \", fixed\\$  (intensity, 3)")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Script for listing F0 statistics", U"ppgb", 20140112)
INTRO (U"\"I need to split the wave into 50 msec sections, and then for each of those sections "
	"get the F0 statistics. That is, for each 50 msec section of speech I want to get the average F0, "
	"min, max, and standard deviation.\"")
NORMAL (U"First you create the complete pitch contour, i.e., you select the Sound and choose "
	"@@Sound: To Pitch...|To Pitch...@. You can then use the commands from the #Query menu in a loop:")
CODE (U"startTime = Get start time")
CODE (U"endTime = Get end time")
CODE (U"numberOfTimeSteps = (endTime - startTime) / 0.05")
CODE (U"writeInfoLine: \"   tmin     tmax    mean   fmin   fmax  stdev\"")
CODE (U"for step to numberOfTimeSteps")
	CODE1 (U"tmin = startTime + (step - 1) * 0.05")
	CODE1 (U"tmax = tmin + 0.05")
	CODE1 (U"mean = Get mean: tmin, tmax, \"Hertz\"")
	CODE1 (U"minimum = Get minimum: tmin, tmax, \"Hertz\", \"Parabolic\"")
	CODE1 (U"maximum = Get maximum: tmin, tmax, \"Hertz\", \"Parabolic\"")
	CODE1 (U"stdev = Get standard deviation: tmin, tmax, \"Hertz\"")
	CODE1 (U"appendInfoLine: fixed\\$  (tmin, 6), \" \", fixed\\$  (tmax, 6), \" \", fixed\\$  (mean, 2),")
	CODE1 (U"... \" \", fixed\\$  (minimum, 2), \" \", fixed\\$  (maximum, 2), \" \", fixed\\$  (stdev, 2)")
CODE (U"endfor")
ENTRY (U"Notes")
NORMAL (U"One should not cut the sound up into pieces of 50 ms and then do ##To Pitch...# on each of them, "
	"because Praat will not compute F0 values in the first or last 20 ms (or so) of each piece. "
	"This is because the analysis requires a window of 40 ms (or so) for every pitch frame. "
	"Instead, one typically does the analysis on the whole sound, then queries the resulting large Pitch object. "
	"In that way, the information loss of windowing only affects the two 20 ms edges of the whole sound.")
NORMAL (U"The example writes lines to the #Info window. If you want to write to a file instead, "
	"you start with something like")
	CODE1 (U"deleteFile: \"~/results/out.txt\"")
NORMAL (U"and add lines in the following way:")
	CODE1 (U"appendFileLine: \"~/results/out.txt \", fixed\\$  (tmin, 6), \" \", fixed\\$  (tmax, 6), \" \",")
	CODE1 (U"... fixed\\$  (mean, 2), \" \", fixed\\$  (minimum, 2), \" \", fixed\\$  (maximum, 2), \" \",")
	CODE1 (U"... fixed\\$  (stdev, 2)")
MAN_END

MAN_BEGIN (U"Script for creating a frequency sweep", U"ppgb", 20140107)
INTRO (U"\"I have to find a formula for a sinewave that sweeps from 1 kHz to 12 kHz in "
	"60 seconds while ramping the amplitude from 1 to 12 volts in the same amount of time.\"")
NORMAL (U"The absolute amplitude in volts cannot be handled, of course, but linear crescendo is easy:")
CODE (U"Create Sound from formula: \"sweep\", 1, 0, 60, 44100,")
CODE (U"... \"0.05 * (1 + 11 * x/60) * sin (2*pi * (1000 + 11000/2 * x/60) * x)\"")
NORMAL (U"Note the \"/2\" in this formula. Here is the derivation of the formula:")
FORMULA (U"%frequency (%t) = 1000 + 11000 %t / 60")
FORMULA (U"%phase (%t) = \\in %frequency (%t) %dt = 1000 %t + 11000 (%t^2/2) / 60")
FORMULA (U"%signal (%t) = sin (%phase (%t))")
MAN_END

MAN_BEGIN (U"Script for onset detection", U"ppgb", 20140112)
INTRO (U"\"Can anybody provide me with a script that detects the onset of sound (i.e. the end of silence).\"")
NORMAL (U"You can create an Intensity contour and look for the first frame that is above some predefined threshold:")
CODE (U"To Intensity: 100, 0")
CODE (U"n = Get number of frames")
CODE (U"for i to n")
	CODE1 (U"intensity = Get value in frame: i")
	CODE1 (U"if intensity > 40")
		CODE2 (U"time = Get time from frame: i")
		CODE2 (U"writeInfoLine: \"Onset of sound at: \", fixed\\$  (time, 3), \" seconds.\"")
		CODE2 (U"exit")
	CODE1 (U"endif")
CODE (U"endfor")
NORMAL (U"Since the intensity is computed with rather long windows, the result may be 0.01 or 0.02 seconds "
	"before the actual start of sound.")
MAN_END

MAN_BEGIN (U"Script for TextGrid boundary drawing", U"ppgb", 20140107)
INTRO (U"\"I want only the dotted lines of the textgrid marked on top of another analysis (e.g. pitch, intensity or so) "
	"without the labels being shown below it.\"")
CODE (U"n = Get number of intervals: 1")
CODE (U"for i to n-1")
    CODE1 (U"t = Get end point: 1, i")
    CODE1 (U"One mark bottom: t, \"no\", \"no\", \"yes\"")
CODE (U"endfor")
MAN_END

MAN_BEGIN (U"Script for analysing pitch with a TextGrid", U"ppgb", 20141001)
INTRO (U"\"I want the mean pitch of every interval that has a non-empty label on tier 5.\"")
CODE (U"if numberOfSelected (\"Sound\") <> 1 or numberOfSelected (\"TextGrid\") <> 1")
	CODE1 (U"exitScript: \"Please select a Sound and a TextGrid first.\"")
CODE (U"endif")
CODE (U"sound = selected (\"Sound\")")
CODE (U"textgrid = selected (\"TextGrid\")")
CODE (U"writeInfoLine: \"Result:\"")
CODE (U"selectObject: sound")
CODE (U"To Pitch: 0.0, 75, 600")
CODE (U"pitch = selected (\"Pitch\")")
CODE (U"selectObject: textgrid")
CODE (U"n = Get number of intervals: 5")
CODE (U"for i to n")
	CODE1 (U"tekst\\$  = Get label of interval: 5, i")
	CODE1 (U"if tekst\\$  <> \"\"")
		CODE2 (U"t1 = Get starting point: 5, i")
		CODE2 (U"t2 = Get end point: 5, i")
		CODE2 (U"selectObject: pitch")
		CODE2 (U"f0 = Get mean: t1, t2, \"Hertz\"")
		CODE2 (U"appendInfoLine: fixed\\$  (t1, 3), \" \", fixed\\$  (t2, 3), \" \", round (f0), \" \", tekst\\$ ")
		CODE2 (U"selectObject: textgrid")
	CODE1 (U"endif")
CODE (U"endfor")
CODE (U"selectObject: sound, textgrid")
MAN_END

MAN_BEGIN (U"Demo window", U"ppgb", 20170327)
INTRO (U"The Demo window is a window in which you can draw and ask for user input. "
	"You can use it for demonstrations, presentations, simulations, adaptive listening experiments, "
	"and stand-alone programs (see @@Scripting 9.1. Turning a script into a stand-alone program@).")
NORMAL (U"The Demo window is Praat's least visible window: you can create it only through a script. "
	"Try the following script after selecting a Sound object:")
CODE (U"demo Draw: 0, 3, -1, 1, \"yes\", \"curve\"")
NORMAL (U"You see the Demo window turning up on the screen, with the Sound painted into it. "
	"It works because the ##Draw...# command is available in the Objects window when you select a Sound. Then try:")
CODE (U"demo Draw line: 0, -1, 3, 1")
NORMAL (U"You see a line drawn from (0 seconds, -1 Pa) to (3 seconds, +1 Pascal) in the waveform. "
	"It works because the ##Draw line...# command is available in the Picture window. Then try:")
CODE (U"demo Erase all")
CODE (U"demo Red")
CODE (U"demo Axes: 0, 100, 0, 100")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"Hello\"")
NORMAL (U"You see a text appearing in red, in the centre of the window. "
	"This works because you are using commands from the Picture window, including the @@Axes...@ command, "
	"which sets the world coordinates to something else than before (before, the world coordinates were determined by the Sound).")
NORMAL (U"Now suppose you want the Sound to appear in the top half of the window, "
	"and some texts in the bottom left and bottom right corners of the window. "
	"You can use @@Select outer viewport...@ and @@Select inner viewport...@, "
	"if you know that the size of the Demo window is \"100\" horizontally and \"100\" vertically (rather than 12\\xx12, as the Picture window), "
	"and that the point (0, 0) lies in the bottom left (rather than the top left, as in the Picture window):")
CODE (U"demo Erase all")
CODE (U"demo Black")
CODE (U"demo Times")
CODE (U"demo 24")
CODE (U"demo Select outer viewport: 0, 100, 50, 100")
CODE (U"demo Draw: 0, 0, 0, 0, \"yes\", \"curve\"")
CODE (U"demo Select inner viewport: 0, 100, 0, 100")
CODE (U"demo Axes: 0, 10, 0, 10")
CODE (U"demo Text: 0, \"left\", 0, \"bottom\", \"Left bottom corner\"")
CODE (U"demo Text: 10, \"right\", 0, \"bottom\", \"Right bottom corner\"")
NORMAL (U"As the title page of a presentation, you could do:")
CODE (U"demo Erase all")
CODE (U"demo Select inner viewport: 0, 100, 0, 100")
CODE (U"demo Axes: 0, 100, 0, 100")
CODE (U"demo Paint rectangle: \"purple\", 0, 100, 0, 100")
CODE (U"demo Pink")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"This is my title\"")
ENTRY (U"Getting user input")
NORMAL (U"For almost all applications, you will want the user (or the participant in an experiment) to be able to click on things in the Demo window, "
	"or to control the Demo window by pressing keys. Here is a presentation with two screens:")
CODE (U"demo Erase all")
CODE (U"demo Select inner viewport: 0, 100, 0, 100")
CODE (U"demo Axes: 0, 100, 0, 100")
CODE (U"demo Paint rectangle: \"purple\", 0, 100, 0, 100")
CODE (U"demo Pink")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"This is the first page\"")
CODE (U"#demoWaitForInput ( )")
CODE (U"demo Erase all")
CODE (U"demo Paint rectangle: \"purple\", 0, 100, 0, 100")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"This is the second page\"")
NORMAL (U"In this example, you go from the first to the second screen either by clicking with the mouse or by pressing any key. "
	"You will usually want to be more selective in your choice of user actions to respond to. "
	"The function #demoWaitForInput always returns 1, so that you can use it nicely in a loop, in which you can react selectively:")
CODE (U"label FIRST_SCREEN")
CODE (U"demo Erase all")
CODE (U"demo Black")
CODE (U"demo Times")
CODE (U"demo 24")
CODE (U"demo Select inner viewport: 0, 100, 0, 100")
CODE (U"demo Axes: 0, 100, 0, 100")
CODE (U"demo Paint rectangle: \"purple\", 0, 100, 0, 100")
CODE (U"demo Pink")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"This is the first page\"")
CODE (U"while demoWaitForInput ( )")
	CODE1 (U"if #demoClicked ( )")
		CODE2 (U"goto SECOND_SCREEN")
	CODE1 (U"elsif #demoKeyPressed ( )")
		CODE2 (U"if ##demoKey\\$ # ( ) = \"\\->\" or demoKey\\$  ( ) = \" \"")
			CODE3 (U"goto SECOND_SCREEN")
		CODE2 (U"endif")
	CODE1 (U"endif")
CODE (U"endwhile")
CODE (U"label SECOND_SCREEN")
CODE (U"demo Erase all")
CODE (U"demo Paint rectangle: \"purple\", 0, 100, 0, 100")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"This is the second page\"")
CODE (U"while demoWaitForInput ( )")
	CODE1 (U"if demoClicked ( )")
		CODE2 (U"goto END")
	CODE1 (U"elsif demoKeyPressed ( )")
		CODE2 (U"if demoKey\\$  ( ) = \"\\<-\"")
			CODE3 (U"goto FIRST_SCREEN")
		CODE2 (U"elsif demoKey\\$  ( ) = \"\\->\" or demoKey\\$  ( ) = \" \"")
			CODE3 (U"goto END")
		CODE2 (U"endif")
	CODE1 (U"endif")
CODE (U"endwhile")
CODE (U"label END")
NORMAL (U"This script allows you to use the arrow keys and the space bar to navigate between the two screens. A shorter version is:")
CODE (U"label FIRST_SCREEN")
CODE (U"demo Erase all")
CODE (U"demo Black")
CODE (U"demo Times")
CODE (U"demo 24")
CODE (U"demo Select inner viewport: 0, 100, 0, 100")
CODE (U"demo Axes: 0, 100, 0, 100")
CODE (U"demo Paint rectangle: \"purple\", 0, 100, 0, 100")
CODE (U"demo Pink")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"This is the first page\"")
CODE (U"while demoWaitForInput ( )")
	CODE1 (U"goto SECOND_SCREEN #demoInput (\"\\bu\\-> \")")
CODE (U"endwhile")
CODE (U"label SECOND_SCREEN")
CODE (U"demo Erase all")
CODE (U"demo Paint rectangle: \"purple\", 0, 100, 0, 100")
CODE (U"demo Text: 50, \"centre\", 50, \"half\", \"This is the second page\"")
CODE (U"while demoWaitForInput ( )")
	CODE1 (U"goto END demoInput (\"\\bu\\-> \")")
	CODE1 (U"goto FIRST_SCREEN demoInput (\"\\<-\")")
CODE (U"endwhile")
CODE (U"label END")
NORMAL (U"This uses two tricks, namely the possibility of following the #goto statement by a condition "
	"and using #demoInput to quickly test for multiple possible inputs (the bullet represents a mouse click).")
ENTRY (U"Getting click locations")
NORMAL (U"You can use the functions #demoX and #demoY to see where the user has clicked. "
	"These functions respond in world coordinates. To see whether the user has clicked in the sound that occupies the "
	"upper half of the screen in the above example, you do for instance")
CODE (U"while demoWaitForInput ( )")
	CODE1 (U"if demoClicked ( )")
		CODE2 (U"demo Select outer viewport: 0, 100, 50, 100")
		CODE2 (U"demo Axes: 0, 3, -1, 1")
		CODE2 (U"if #demoX ( ) >= 0 and demoX ( ) < 3 and #demoY ( ) >= -1 and demoY ( ) < 1")
NORMAL (U"The last line can be shortened to:")
		CODE2 (U"if #demoClickedIn (0, 3, -1, 1)")
NORMAL (U"Another example of when you want to know the click location is when you test for a click on a button "
	"that you drew on the screen:")
CODE (U"demo Paint rounded rectangle: \"pink\", 30, 70, 16, 24")
CODE (U"demo Text: 50, \"centre\", 20, \"half\", \"Analyse\"")
CODE (U"while demoWaitForInput ( )")
	CODE1 (U"goto ANALYSE demoClickedIn (30, 70, 16, 24)")
ENTRY (U"Full-screen viewing")
NORMAL (U"When you click in the \"zoom box\" (the green button in the title bar of the Demo window on the Mac), "
	"the Demo window will zoom out very strongly: it will fill up the whole screen. The menu bar becomes invisible, "
	"although you can still make it temporarily visible and accessible by moving the mouse to the upper edge of the screen. "
	"The Dock also becomes invisible, although you can make it temporarily visible and accessible by moving the mouse to the edge "
	"of the screen (the left, bottom, or right edge, depending on where your Dock normally is). "
	"When you click the zoom box again, the Demo window is restored to its original size. See also Tips and Tricks below.")
ENTRY (U"Asynchronous play")
NORMAL (U"If you select a Sound and execute the command")
CODE (U"Play")
NORMAL (U"Praat will play the whole sound before proceeding to the next line of your script. "
	"You will often instead want Praat to continue running your script while the sound is playing. "
	"To accomplish that, use the \"asynchronous\" directive:")
CODE (U"Create Sound as pure tone: \"tone\", 1, 0, 0.2, 44100, 440, 0.2, 0.01, 0.01")
CODE (U"#asynchronous Play")
CODE (U"Remove")
NORMAL (U"The sound will continue to play, even after the Sound object has been removed.")
NORMAL (U"Please note that a following Play command will interrupt the playing of the first:")
CODE (U"while demoWaitForInput ( )")
	CODE1 (U"if demoClicked ( )")
		CODE2 (U"Create Sound as pure tone: \"tone\", 1, 0, 3.0, 44100,")
		CODE2 (U"... randomGauss (440, 100), 0.2, 0.01, 0.01")
		CODE2 (U"asynchronous Play")
		CODE2 (U"Remove")
	CODE1 (U"endif")
CODE (U"endwhile")
NORMAL (U"The first sound will stop playing soon after the user clicks for the second time.")
ENTRY (U"Animation")
NORMAL (U"In the above examples, things will often get drawn to the screen with some delay, "
	"i.e., you may not see the erasures and paintings happen. This is because several operating systems "
	"use %buffering of graphics. These systems will draw the graphics only just before getting user input. "
	"This means that #demoWaitForInput is the place where your drawings will typically be painted on the screen. "
	"If you want painting to happen earlier (e.g. in animations), you can use")
CODE (U"demoShow ( )")
NORMAL (U"Also in animations, you will often want to regulate the time span between two consecutive drawing. "
	"If you want 0.05 seconds between drawings, you can put Praat to sleep temporarily with")
CODE (U"sleep (0.05)")
NORMAL (U"If you need user input during the animation, you can replace #demoWaitForInput or #demoShow with")
CODE (U"demoPeekInput()")
NORMAL (U"which returns immediately without waiting and will tell you (via e.g. #demoClicked or ##demoKey\\$ #) "
	"whether a mouse or key event happened during drawing or sleeping.")
ENTRY (U"Miscellaneous")
NORMAL (U"To see whether any function keys are pressed (during a mouse click or key press), "
	"you can use ##demoShiftKeyPressed ( )#, ##demoCommandKeyPressed ( )#, ##demoOptionKeyPressed ( )#, and "
	"##demoExtraControlKeyPressed ( )#.")
NORMAL (U"To put some text in the title bar of the Demo window, try")
CODE (U"#demoWindowTitle: \"This is the title of my presentation\"")
ENTRY (U"Tips and Tricks")
NORMAL (U"The initial size of the Demo window when you start it up is 1344\\xx756 pixels, "
	"which is 70 percent of a standard wide screen (1920\\xx1080 pixels). "
	"This means that if a font looks good at a size of 35 in the initial Demo window, "
	"the font will look equally good at a size of 50 when you use a 1920\\xx1080 video projector full-screen.")
NORMAL (U"If you resize the Demo window with the handle in the bottom left, or if you zoom the window out to the full screen, "
	"you may see that the relative positions of the contents of the window will change. Also, clicking on buttons and in parts "
	"of the window may yield unexpected %x and %y values. It is therefore advisable to resize the window only if you are on a page "
	"that you can get out of by pressing a key, or by clicking anywhere in the window without using #demoX, #demoY or #demoClickedIn.")
NORMAL (U"If you click away the Demo window while it is waiting for input, you get a message saying \"You interrupted the script...\". "
	"If you do not want to see this message, you should make sure that the user can reach the end of the script, for instance by "
	"pressing the \\-> key on the last page. To make sure the user sees that the script has ended, you could end it with ##demo Erase all#.")
NORMAL (U"Your demo can save its preferences in a directory of its choice, "
	"e.g. in ##'preferencesDirectory\\$ '/../GuineaPigAnalyzer# if your demo is called GuineaPigAnalyzer. "
	"If you want to be less conspicuous and like to use the Praat preferences directory instead, "
	"please use the ##apps# subdirectory, in this way:")
CODE (U"createDirectory: preferencesDirectory\\$  + \"/apps\"")
CODE (U"createDirectory: preferencesDirectory\\$  + \"/apps/GuineaPigAnalyzer\"")
MAN_END

}

/* End of file manual_Script.cpp */
