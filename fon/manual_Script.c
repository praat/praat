/* manual_Script.c
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

void manual_Script_init (ManPages me);
void manual_Script_init (ManPages me) {

MAN_BEGIN ("Action commands", "ppgb", 20021130)
INTRO ("The commands in the @@Dynamic menu@ of the @@Object window@.")
NORMAL ("These commands are only available if the right kinds of objects are selected. They are shown in a scrollable list, "
	"or in the #Write menu if they start with \"Write to \" or \"Append to \".")
MAN_END

MAN_BEGIN ("Add action command...", "ppgb", 19970518)
INTRO ("One of the hidden commands in the Control menu of the @@Object window@. "
	"With this command, you add a button to the dynamic menu in the Object window.")
ENTRY ("Arguments")
NORMAL ("See @@Add to dynamic menu...@.")
ENTRY ("Usage")
NORMAL ("You can use this command in your @@Initialization script@, and you will see it in your @@Buttons file@ "
	"after you have added an action button with this command or with the @ScriptEditor.")
NORMAL ("Normally, however, if you want to add a command to the dynamic menu, "
	"you would use the command @@Add to dynamic menu...@ of the @ScriptEditor instead.")
MAN_END

MAN_BEGIN ("Add menu command...", "ppgb", 19970518)
INTRO ("One of the hidden commands in the Control menu of the @@Object window@. "
	"With this command, you add a button to any of the fixed menus in the Object or Picture window.")
ENTRY ("Arguments")
NORMAL ("See @@Add to fixed menu...@.")
ENTRY ("Usage")
NORMAL ("You can use this command in your @@Initialization script@, and you will see it in your @@Buttons file@ "
	"after you have added a menu button with this command or with the @ScriptEditor.")
NORMAL ("Normally, however, if you want to add a command to a fixed menu, "
	"you would use the command @@Add to fixed menu...@ of the @ScriptEditor instead.")
MAN_END

MAN_BEGIN ("Add to dynamic menu...", "ppgb", 19980105)
INTRO ("A command in the File menu of the @ScriptEditor.")
NORMAL ("With this command, you add a button to the dynamic menu in the @@Object window@. "
	"This button will only be visible if the specified combination of objects is selected. "
	"Clicking the button will invoke the specified @@Praat script@.")
ENTRY ("Arguments")
TAG ("%%Class 1")
DEFINITION ("the name of the class of the object to be selected. "
	"For instance, if a button should only appear if the user selects a Sound, this would be \"Sound\".")
TAG ("%%Number 1")
DEFINITION ("the number of objects of %class1 that have to be selected. For most built-in commands, this number is unspecified (0); "
	"e.g., the user can choose #Draw... or ##To Spectrum# regardless of whether she selected 1, 2, 3, or more Sound objects. "
	"If the number of selected objects is different from %number1, the button will be visible but insensitive.")
TAG ("%%Class 2")
DEFINITION ("the name of the class of the second object to be selected, different from %class1. "
	"Normally the empty string (\"\").")
TAG ("%%Number 2")
DEFINITION ("the number of selected objects of %class2.")
TAG ("%%Class 3")
DEFINITION ("the name of the class of the third object to be selected, different from %class1 and %class2. "
	"Normally the empty string (\"\").")
TAG ("%%Number 3")
DEFINITION ("the number of selected objects of %class3.")
TAG ("%%Command")
DEFINITION ("the title of the new command button (or label, or submenu title). "
	"To get a separator line instead of a command text (only in a submenu), "
	"you specify a unique string that starts with a hyphen ('-'); the @ButtonEditor may contain some examples of this. "
	"If the command starts with \"Write to \", it will be placed in the @@Write menu@.")
TAG ("%%After command")
DEFINITION ("a button title in the dynamic menu or submenu where you want your new button. "
	"If you specify the empty string (\"\"), your button will be put at the bottom. "
	"You can specify a push button, a label (subheader), or a cascade button (submenu title) here.")
TAG ("%%Depth")
DEFINITION ("0 if you want your button in the main menu, 1 if you want it in a submenu.")
TAG ("%%Script")
DEFINITION ("the full path name of the script to invoke. If you saved the script you are editing, "
	"its name will already have been filled in here. "
	"If you do not specify a script, you will get a separating label or cascading menu title instead, "
	"depending on the %depth of the following command.")
ENTRY ("Example")
NORMAL ("If one object of class Sound is selected, you want a submenu called \"Filters\" "
	"after the #Convolve button, containing the commands \"Autocorrelation\" and \"Band filter...\", "
	"separated by a horizontal separator line:")
CODE ("Add to dynamic menu... Sound 0 \"\" 0 \"\" 0 \"Filters -\" \"Convolve\" 0")
CODE ("Add to dynamic menu... Sound 1 \"\" 0 \"\" 0 \"Autocorrelation\" \"Filters -\" 1 /u/praats/demo/autocorrelation.praat")
CODE ("Add to dynamic menu... Sound 0 \"\" 0 \"\" 0 \"-- band filter --\" \"Autocorrelation\" 1")
CODE ("Add to dynamic menu... Sound 1 \"\" 0 \"\" 0 \"Band filter...\" \"-- band filter --\" 1 /u/praats/demo/bandFilter.praat")
NORMAL ("Note that \"Filters -\" will be a submenu title, %because it is followed by subcommands (%depth 1). "
	"Note that %number1 is 1 only for the executable buttons; for the cascade button and the separator line, "
	"this number is ignored.")
ENTRY ("Usage convention")
NORMAL ("Please adhere to the convention that commands that take arguments, end in three dots (...).")
ENTRY ("Using this command in a script")
NORMAL ("To add a dynamic button from a script (perhaps your @@Initialization script@), "
	"use the hidden shell command @@Add action command...@ instead.")
MAN_END

MAN_BEGIN ("Add to fixed menu...", "ppgb", 20021204)
INTRO ("A command in the File menu of the @ScriptEditor.")
NORMAL ("With this command, you add a button to any fixed menu in the @@Object window@ or in the @@Picture window@. "
	"Clicking the added button will invoke the specified @@Praat script@.")
ENTRY ("Arguments")
TAG ("%Window")
DEFINITION ("the name of the window (\"Objects\" or \"Picture\") that contains the menu that you want to change.")
TAG ("%Menu")
DEFINITION ("the title of the menu that you want to change. If %window is \"Objects\", you can specify "
	"the Control, New, Read, Help, Goodies, or Preferences menu (for the Write menu, which depends on the objects selected, "
	"you would use @@Add to dynamic menu...@ instead). If %window is \"Picture\", you can specify "
	"the File, Edit, Margins, World, Select, Pen, Font, or Help menu.")
TAG ("%Command")
DEFINITION ("the title of the new menu button. To get a separator line instead of a command text, "
	"you specify a unique string that starts with a hyphen ('-'); the @ButtonEditor contains many examples of this.")
TAG ("%%After command")
DEFINITION ("a button title in the menu or submenu after which you want your new button to be inserted. "
	"If you specify the empty string (\"\"), your button will be put in the main menu.")
TAG ("%Depth")
DEFINITION ("0 if you want your button in the main menu, 1 if you want it in a submenu.")
TAG ("%Script")
DEFINITION ("the full path name of the script to invoke. If you saved the script you are editing, "
	"its name will already have been filled in here. If you do not specify a script, "
	"you will get a cascading menu title instead.")
ENTRY ("Example 1")
NORMAL ("In the #Matrix submenu of the @@New menu@, you want a separator line followed by the command \"Peaks\":")
CODE ("Add to fixed menu... Objects New \"-- peaks --\" \"Create simple Matrix...\" 1")
CODE ("Add to fixed menu... Objects New \"Peaks\" \"-- peaks --\" 1 /u/praats/demo/peaks.praat")
ENTRY ("Example 2")
NORMAL ("In the @@New menu@, you want a submenu called \"Demo\", with a subitem titled \"Lorenz...\":")
CODE ("Add to fixed menu... Objects New \"Demo\" \"\" 0")
CODE ("Add to fixed menu... Objects New \"Lorenz...\" \"Demo\" 1 /u/praats/demo/lorentz.praat")
ENTRY ("Usage convention")
NORMAL ("Please adhere to the convention that commands that take arguments, end in three dots (...).")
ENTRY ("Using this command in a script")
NORMAL ("To add a fixed button from a script (perhaps your @@Initialization script@), "
	"use the hidden shell command @@Add menu command...@ instead.")
MAN_END

MAN_BEGIN ("binomialQ", "ppgb", 20021204)
INTRO ("A function that can be used in @@Formulas@. The complement of the cumulative binomial distribution.")
ENTRY ("Syntax")
TAG ("$$binomialQ (%p, %k, %n)")
DEFINITION ("the probability that in %n trials an event with probability %p will occur at least %k times.")
ENTRY ("Calculator example")
NORMAL ("A die is suspected to yield more sixes than a perfect die would do. In order to test this suspicion, "
	"you throw it 1,000 times. The result is 211 sixes.")
NORMAL ("The probability that a perfect die yields at least 211 sixes is, according to @@Calculator...@, "
	"$$binomialQ (1/6, 211, 1000)$ = 0.000152.")
ENTRY ("Script example")
NORMAL ("You convert 1000 values of pitch targets in Hz to the nearest note on the piano keyboard. "
	"597 of those values turn out to be in the A, B, C, D, E, F, or G regions (the white keys), and 403 values turn out "
	"to be in the A\\# , C\\# , D\\# , F\\# , or G\\#  regions (the black keys). "
	"Do our subjects have a preference for the white keys? "
	"The following script computes the probability that in the case of no preference the subjects "
	"would target the white keys at least 597 times. This is compared with a %\\ci^2 test.")
CODE ("a = 597")
CODE ("b = 403")
CODE ("p = 7/12 ; no preference")
CODE ("echo *** Binomial test 'a', 'b', p = 'p:6' ***")
CODE ("pbin = binomialQ (p, a, a+b)")
CODE ("printline P (binomial) = 'pbin:6'")
CODE ("\\#  Chi-square test with Yates correction:")
CODE ("x2 = (a - 1/2 - p * (a+b))\\^ 2/(p*(a+b)) + (b + 1/2 - (1-p) * (a+b))\\^ 2/((1-p)*(a+b))")
CODE ("px2 = chiSquareQ (x2, 1)")
CODE ("printline P (chi-square) = 'px2:6'")
NORMAL ("The result is:")
CODE ("*** Binomial test 597, 403, p = 0.583333 ***")
CODE ("P (binomial) = 0.199330")
CODE ("P (chi-square) = 0.398365")
NORMAL ("The %\\ci^2 test is two-sided (it signals a preference for the white or for the black keys), "
	"so it has twice the probability of the binomial test.")
NORMAL ("We cannot conclude from this test that people have a preference for the white keys. "
	"Of course, we cannot conclude either that people do not have such a preference.")
MAN_END

MAN_BEGIN ("ButtonEditor", "ppgb", 20030916)
INTRO ("An editor for viewing, hiding, showing, removing, and executing the commands "
	"in the fixed and dynamic menus of the P\\s{RAAT} program. To open it, choose ##Buttons...# "
	"from the #Control menu of the @@Object window@.")
ENTRY ("What the button editor shows")
NORMAL ("The button editor gives a list of:")
LIST_ITEM ("1. The five fixed buttons.")
LIST_ITEM ("2. The built-in and added @@fixed menu commands@, lexicographically sorted by window and menu name.")
LIST_ITEM ("3. The built-in and added @@action commands@, sorted by the names of the selected objects.")
ENTRY ("Visibility of built-in commands")
NORMAL ("Most built-in commands are visible by default, but some are hidden by default (see @@Hidden commands@). "
	"The button editor shows these commands as \"shown\" or \"hidden\", respectively. "
	"You can change the visibility of a command by clicking on the blue \"shown\" or \"hidden\" text; "
	"this text will then be replaced with \"HIDDEN\" or \"SHOWN\", with capitals to signal their non-standard settings. "
	"These changes will be remembered in the @@Buttons file@ across sessions of your program. "
	"To return to the standard settings, click the blue \"HIDDEN\" or \"SHOWN\" texts again.")
NORMAL ("Some built-in commands cannot be hidden. They are marked as \"unhidable\". "
	"The most notable example is the ##Buttons...# button "
	"(a failure to make the ##Commands...# command unhidable in Microsoft Word "
	"causes some computer viruses to be very hard to remove...).")
ENTRY ("Added commands")
NORMAL ("Commands that you have added to the fixed or dynamic menus (probably with @@Add to fixed menu...@ or "
	"@@Add to dynamic menu...@ in the @ScriptEditor), "
	"are marked as \"ADDED\". They are remembered in the @@Buttons file@. "
	"You can change the availability of these commands by clicking on the blue \"ADDED\" text, which will then "
	"be replaced with \"REMOVED\". After this, the added command will no longer be remembered in the @@Buttons file@. "
	"To make the command available again, click the blue \"REMOVED\" text again, before leaving the program.")
ENTRY ("Start-up commands")
NORMAL ("Commands that were added in an @@Initialization script@ (with @@Add menu command...@ or "
	"@@Add action command...@) are marked as \"START-UP\". "
	"They are %not remembered in the @@Buttons file@. "
	"You can change the visibility of these commands by clicking on the blue \"START-UP\" text, which will then "
	"be replaced with \"HIDDEN\". This setting %will be remembered in the @@Buttons file@. "
	"To make the command visible again, click the blue \"HIDDEN\" text again.")
ENTRY ("Executing commands")
NORMAL ("The button editor allows you to choose hidden commands without first making them visible in the fixed or dynamic menus.")
NORMAL ("The editor shows all the executable commands in blue. These include:")
LIST_ITEM ("1. The fixed #Remove button, if one or more objects are selected in the @@List of Objects@.")
LIST_ITEM ("2. The other fixed buttons, if exactly one object is selected.")
LIST_ITEM ("3. All of the fixed menu commands, hidden or not, and \"removed\" or not.")
LIST_ITEM ("4. Those action commands that match the currently selected objects with respect to class and number.")
NORMAL ("To execute any of these blue commands, just click on it.")
MAN_END

MAN_BEGIN ("Buttons file", "ppgb", 20021204)
NORMAL ("The file into which changes in the availability and visibility of commands in the fixed "
	"and dynamic menus are recorded.")
NORMAL ("The buttons file is written to disk when you leave Praat, "
	"and it is read again when you enter Praat the next time. It is a simple @@Praat script@ that you can read "
	"(but should not edit) with any text editor.")
ENTRY ("Adding buttons")
NORMAL ("To add a command to a fixed or dynamic menu, you typically use the @ScriptEditor.")
ENTRY ("Removing buttons")
NORMAL ("To remove an added command from a fixed or dynamic menu, you typically use the @ButtonEditor.")
ENTRY ("Hiding and showing buttons")
NORMAL ("To hide a built-in command from a fixed or dynamic menu, or to make a hidden command visible, "
	"you typically use the @ButtonEditor.")
ENTRY ("Unix")
NORMAL ("If your home directory is /people/miep, the buttons file is /people/miep/.praat-dir/buttons. "
	"If the directory .praat-dir does not exist, it is created when you enter Praat. If you rename Praat, "
	"the name of the directory will also be different.")
ENTRY ("MacOS X")
NORMAL ("If you are Miep, the buttons file will be /Users/Miep/Library/Preferences/Praat Prefs/Buttons.")
ENTRY ("Classic Macintosh")
NORMAL ("The buttons file, which is called \"Buttons\", "
	"will be in a folder named \"Praat Preferences\" in the Preferences folder in your System Folder. "
	"If your hard disk is called \"H\\ael' sji'f\", and you have a Dutch system, "
	"the complete path to your preferences file is:")
CODE ("H\\ael' sji'f:Systeemmap:Voorkeuren:Praat Preferences:Buttons")
ENTRY ("Windows")
NORMAL ("The buttons file may be C:\\bsWINDOWS\\bsPraat\\bsButtons.ini.")
MAN_END

MAN_BEGIN ("Calculator", "ppgb", 20021201)
INTRO ("A window that allows you to calculate all kinds of simple or complicated mathematical and string expressions. "
	"To show the calculator, type Command-U or choose the @@Calculator...@ command. "
	"The result will be written to the Info window.")
NORMAL ("See the @Formulas tutorial for all the things that you can calculate with this command.")
MAN_END

MAN_BEGIN ("Calculator...", "ppgb", 20021204)
INTRO ("A command in the @Goodies submenu of the @@Control menu@ of the @@Object window@. Shortcut: Command-U. "
	"Choosing this command brings up Praat's @calculator.")
MAN_END

MAN_BEGIN ("Clear history", "ppgb", 20000927)
INTRO ("A command in the Edit menu of the @ScriptEditor for clearing the remembered history. "
	"See @@History mechanism@.")
MAN_END

MAN_BEGIN ("differenceLimensToPhon", "ppgb", 20021215)
INTRO ("A routine for converting intensity difference limens into sensation level, "
	"the inverse of @phonToDifferenceLimens.")
ENTRY ("Formula")
FORMULA ("differenceLimensToPhon (%ndli) = ln (1 + %ndli / 30) / ln (61 / 60)")
MAN_END

MAN_BEGIN ("Fixed menu commands", "ppgb", 19970518)
INTRO ("The commands in the fixed menus of the @@Object window@ (#Control, #New, #Read, #Help, #Goodies, "
	"and #Preferences) and the @@Picture window@ (#File, #Edit, #Margins, #World, #Select, #Pen, #Font, #Help).")
NORMAL ("These commands are always clickable (if not hidden) and scriptable (if not added).")
MAN_END

MAN_BEGIN ("Formulas", "ppgb", 20040414)
INTRO ("You can use numeric expressions and string (text) expressions in many places in Praat:")
LIST_ITEM ("\\bu in the @calculator in Praat's @Goodies submenu;")
LIST_ITEM ("\\bu in the numeric fields of most settings windows;")
LIST_ITEM ("\\bu in a @@Praat script@.")
NORMAL ("For some types of objects (mainly Sound and Matrix), you can also apply formulas to all their contents at the same time:")
LIST_ITEM ("\\bu when you create a Sound or a Matrix from the @@New menu@;")
LIST_ITEM ("\\bu when you choose @@Formula...@ from the @Modify menu for a selected object.")
NORMAL ("You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM ("1. @@Formulas 1. My first formulas|My first formulas@ (where to use)")
LIST_ITEM1 ("1.1. @@Formulas 1.1. Formulas in the calculator|Formulas in the calculator@")
LIST_ITEM1 ("1.2. @@Formulas 1.2. Numeric expressions|Numeric expressions@")
LIST_ITEM1 ("1.3. @@Formulas 1.3. String expressions|String expressions@")
LIST_ITEM1 ("1.4. @@Formulas 1.4. Representation of numbers|Representation of numbers@")
LIST_ITEM1 ("1.5. @@Formulas 1.5. Representation of strings|Representation of strings@")
LIST_ITEM1 ("1.6. @@Formulas 1.6. Formulas in settings windows|Formulas in settings windows@")
LIST_ITEM1 ("1.7. @@Formulas 1.7. Formulas for creation|Formulas for creation@")
LIST_ITEM1 ("1.8. @@Formulas 1.8. Formulas for modification|Formulas for modification@")
LIST_ITEM1 ("1.9. @@Formulas 1.9. Formulas in scripts|Formulas in scripts@")
LIST_ITEM ("2. @@Formulas 2. Operators|Operators@ (+, -, *, /, \\^ )")
LIST_ITEM ("3. @@Formulas 3. Constants|Constants@ (pi, e, undefined)")
LIST_ITEM ("4. @@Formulas 4. Mathematical functions|Mathematical functions@")
LIST_ITEM ("5. @@Formulas 5. String functions|String functions@")
LIST_ITEM ("6. @@Formulas 6. Control structures|Control structures@ (if then else fi, semicolon)")
LIST_ITEM ("7. @@Formulas 7. Attributes of objects|Attributes of objects@")
LIST_ITEM ("8. @@Formulas 8. Data in objects|Data in objects@")
MAN_END

MAN_BEGIN ("Formulas 1. My first formulas", "ppgb", 20040414)
LIST_ITEM ("1.1. @@Formulas 1.1. Formulas in the calculator|Formulas in the calculator@")
LIST_ITEM ("1.2. @@Formulas 1.2. Numeric expressions|Numeric expressions@")
LIST_ITEM ("1.3. @@Formulas 1.3. String expressions|String expressions@")
LIST_ITEM ("1.4. @@Formulas 1.4. Representation of numbers|Representation of numbers@")
LIST_ITEM ("1.5. @@Formulas 1.5. Representation of strings|Representation of strings@")
LIST_ITEM ("1.6. @@Formulas 1.6. Formulas in settings windows|Formulas in settings windows@")
LIST_ITEM ("1.7. @@Formulas 1.7. Formulas for creation|Formulas for creation@")
LIST_ITEM ("1.8. @@Formulas 1.8. Formulas for modification|Formulas for modification@")
LIST_ITEM ("1.9. @@Formulas 1.9. Formulas in scripts|Formulas in scripts@")
MAN_END

MAN_BEGIN ("Formulas 1.1. Formulas in the calculator", "ppgb", 20021204)
INTRO ("To use the Praat @calculator, go to the @@Control menu@ (in MacOS X, this is the Praat menu), "
	"and choose @@Calculator...@ from the @Goodies submenu. Or simply type Command-U anywhere in Praat.")
ENTRY ("Calculating numbers")
NORMAL ("You can do arithmetic computations. Type the formula")
CODE ("8*17")
NORMAL ("and click OK. The Info window will pop up and show the result:")
CODE ("136")
ENTRY ("Calculating strings")
NORMAL ("You can also do text computations. Type the formula")
CODE ("\"see\" + \"king\"")
NORMAL ("and click OK. The Info window will show the result:")
CODE ("seeking")
MAN_END

MAN_BEGIN ("Formulas 1.2. Numeric expressions", "ppgb", 20050614)
INTRO ("All the formulas whose outcome is a number are called numeric expressions. "
	"For the following examples, all the outcomes can be checked with the @calculator.")
ENTRY ("Examples with numbers")
NORMAL ("Some numeric expressions involve numbers only:")
TAG ("##8*17")
DEFINITION ("computes a multiplication. Outcome: 136.")
TAG ("##2\\^ 10")
DEFINITION ("computes the tenth power of 2. Outcome: 1024.")
TAG ("##sqrt (2) / 2")
DEFINITION ("computes the square root of 2, and divides the result by 2. Outcome: 0.7071067811865476.")
TAG ("##sin (1/4 * pi)")
DEFINITION ("computes the sine of %\\pi/4. Outcome: 0.7071067811865476 (again).")
ENTRY ("Examples with strings")
NORMAL ("Some numeric expressions compute numeric properties of strings:")
TAG ("##length (\"internationalization\")")
DEFINITION ("computes the length of the string \"internationalization\". Outcome: 20.")
TAG ("##index (\"internationalization\", \"ation\")")
DEFINITION ("computes the location of the first occurrence of the string \"ation\" in the string \"internationalization\". Outcome: 7, "
	"because the first letter of \"ation\" lines up with the seventh letter of \"internationalization\". "
	"If the substring does not occur, the outcome is 0.")
TAG ("##rindex (\"internationalization\", \"ation\")")
DEFINITION ("computes the location of the last occurrence of the string \"ation\" in the string \"internationalization\". Outcome: 16.")
TAG ("##startsWith (\"internationalization\", \"int\")")
DEFINITION ("determines whether the string \"internationalization\" starts with \"intern\". Outcome: 1 (true).")
TAG ("##endsWith (\"internationalization\", \"nation\")")
DEFINITION ("determines whether the string \"internationalization\" ends with \"nation\". Outcome: 0 (false).")
TAG ("##index_regex (\"internationalization\", \"a.*n\")")
DEFINITION ("determines where the string \"internationalization\" first matches the @@regular expressions|regular expression@ \"a.*n\". Outcome: 7. "
	"If there is no match, the outcome is 0.")
TAG ("##rindex_regex (\"internationalization\", \"a.*n\")")
DEFINITION ("determines where the string \"internationalization\" last matches the @@regular expressions|regular expression@ \"a.*n\". Outcome: 16.")
MAN_END

MAN_BEGIN ("Formulas 1.3. String expressions", "ppgb", 20040414)
INTRO ("All the formulas whose outcome is a text string are called string expressions. "
	"Again, the outcomes of the following examples can be checked with the @calculator.")
TAG ("##\"see\" + \"king\"")
DEFINITION ("concatenates two strings. Outcome: seeking.")
TAG ("##left\\$  (\"internationalization\", 6)")
DEFINITION ("computes the leftmost six letters of the string; the dollar sign is used for all functions whose result is a string. Outcome: intern.")
TAG ("##mid\\$  (\"internationalization\", 6, 8)")
DEFINITION ("computes the 8-letter substring that starts at the sixth letter of \"internationalization\". Outcome: national.")
TAG ("##date\\$  ( )")
DEFINITION ("computes the current date and time. Outcome at the time I am writing this: Mon Dec  2 02:23:45 2002.")
MAN_END

MAN_BEGIN ("Formulas 1.4. Representation of numbers", "ppgb", 20040414)
INTRO ("Formulas can work with integer numbers as well as with real numbers.")
ENTRY ("Real numbers")
NORMAL ("You can type many real numbers by using a decimal notation, for instance 3.14159, 299792.5, or -0.000123456789. "
	"For very large or small numbers, you can use the %e-notation: 6.022\\.c10^^23^ is typed as 6.022e23 or 6.022e+23, "
	"and -1.6021917\\.c10^^-19^ is typed as -1.6021917e-19. You can use also use the percent notation: 0.157 "
	"can be typed as 15.7\\% .")
NORMAL ("There are some limitations as to the values that real numbers can have in Praat. "
	"The numbers must lie between -10^^308^ and +10^^308^. If you type")
CODE ("1e200 * 1e100")
NORMAL ("the outcome will be")
CODE ("1e+300")
NORMAL ("but if you type")
CODE ("1e300 * 1e100")
NORMAL ("the outcome will be")
CODE ("--undefined--")
NORMAL ("Another limitation is that the smallest non-zero numbers lie near -10^^-308^ and +10^^-308^. If you type")
CODE ("1e-200 / 1e100")
NORMAL ("the outcome will be")
CODE ("1e-300")
NORMAL ("but if you type")
CODE ("1e-300 / 1e100")
NORMAL ("the outcome will be")
CODE ("0")
NORMAL ("Finally, the precision of real numbers is limited by the number of bits that every real number is stored with "
	"in the computer, namely 64. For instance, if you type")
CODE ("pi")
NORMAL ("the outcome will be")
CODE ("3.141592653589793")
NORMAL ("because only 16 digits of precision are stored. This can lead to unexpected results caused by rounding. "
	"For instance, the formula")
CODE ("0.34999999999999999 - 0.35")
NORMAL ("will result in")
CODE ("0")
NORMAL ("rather than the correct value of 1e-17. This is because the numbers 0.34999999999999999 and 0.35 cannot "
	"be distinguished in the computer's memory. If you simply type")
CODE ("0.34999999999999999")
NORMAL ("the outcome will be")
CODE ("0.35")
NORMAL ("(as in this example, the calculator will always come up with the minimum number of digits needed to represent the number unambiguously).")
NORMAL ("Another example of inaccuracy is the formula")
CODE ("1 / 7 / 59 * 413")
NORMAL ("Because of rounding errors, the result will be")
CODE ("0.9999999999999999")
ENTRY ("Integer numbers")
NORMAL ("Formulas can work with integer (whole) numbers between -1,000,000,000,000,000 and +1,000,000,000,000,000. "
	"You type them without commas and without the plus sign: 337, -848947328345289.")
NORMAL ("You %can work with larger numbers than that (up to 10^^308^), but there will again be rounding errors. "
	"For instance, the formula")
CODE ("1000000000000000 + 1")
NORMAL ("correctly yields")
CODE ("1000000000000001")
NORMAL ("but the formula")
CODE ("10000000000000000 + 1")
NORMAL ("yields an incorrect outcome:")
CODE ("1e16")
MAN_END

MAN_BEGIN ("Formulas 1.5. Representation of strings", "ppgb", 20021203)
INTRO ("Formulas can work with strings that are put between two double quotes, "
	"as in \"goodbye\" or \"how are you doing?\".")
NORMAL ("If a string has to contain a double quote, "
	"you have to type it twice. For instance, if you type")
CODE ("\"I asked: \\\" \\\" how are you doing?\\\" \\\" \"")
NORMAL ("into the calculator, the outcome will be")
CODE ("I asked: \"how are you doing?\"")
MAN_END

MAN_BEGIN ("Formulas 1.6. Formulas in settings windows", "ppgb", 20040414)
INTRO ("Into numeric fields in settings windows you usually simply type a number. "
	"However, you can use any numeric expression instead.")
NORMAL ("For instance, suppose you want to create a Sound that contains exactly 10000 samples. "
	"If the sampling frequency is 22050 Hz, the duration will be 10000/22050 seconds. "
	"You can create such a Sound by choosing @@Create Sound...@ from the @@New menu@, "
	"then typing")
CODE ("10000/22050")
NORMAL ("into the ##%%Finishing time#% field.")
NORMAL ("Into text fields in settings windows, you can only type text directly; there is no way "
	"to use string expressions (except if you use scripts; see @@Formulas 1.9. Formulas in scripts@).")
MAN_END

MAN_BEGIN ("Formulas 1.7. Formulas for creation", "ppgb", 20030316)
INTRO ("With some commands in the @@New menu@, you can supply a formula that Praat will apply to all elements of the new object.")
ENTRY ("Creating a Sound from a formula")
NORMAL ("Choose @@Create Sound...@ and type the following into the #%Formula field:")
CODE ("1/2 * sin (2 * pi * 377 * x)")
NORMAL ("When you click OK, a new @Sound object will appear in the list. "
	"After you click #%Edit and zoom in a couple of times, you will see that the sound is a sine wave "
	"with a frequency of 377 Hertz (cycles per second). This worked because the %x in the formula represents the time, "
	"i.e. the formula was applied to every sample separately, with a different value of %x for each sample.")
ENTRY ("Creating a Matrix from a formula")
NORMAL ("Choose @@Create simple Matrix...@ and type the following into the #%Formula field:")
CODE ("8")
NORMAL ("When you click OK, a new @Matrix object will appear in the list. When you click Info, "
	"you will see that it is a matrix with 10 rows and 10 columns, and that all the 100 cells contain the value 8 "
	"(you can see this because both the minimum and the maximum are reported as being 8).")
NORMAL ("A more interesting example is the formula")
CODE ("row * col")
NORMAL ("For the resulting Matrix, "
	"choose @@Matrix: Paint cells...|Paint cells...@ and click OK. The Picture window will show a 10\\xx10 "
	"matrix whose elements are the product of the row and column numbers, i.e., they have values between "
	"1 and 100. Beside %row and %col, you can use %x for the distance along the horizontal axis and %y for the "
	"distance along the vertical axis; see the following page for examples.")
MAN_END

MAN_BEGIN ("Formulas 1.8. Formulas for modification", "ppgb", 20021204)
INTRO ("Analogously to the formulas that you can use for creating new objects (see the previous page), "
	"you can use formulas for modifying existing objects. You do this with the command ##%Formula...%# that you "
	"can find in the @Modify menu when you select an object.")
ENTRY ("Modifying a Sound with a formula")
NORMAL ("Record a sound with your microphone and talk very lowly. If you don't know how to record a sound in Praat, "
	"consult the @Intro. Once the Sound objetc is in the list, click #%Play. The result will sound very soft. "
	"Then choose ##%%Formula...#% from the #%Modify menu and type")
CODE ("self * 3")
NORMAL ("Click OK, then click #%Play again. The sound is much louder now. You have multiplied the amplitude of every sample "
	"in the sound with a factor of 3.")
ENTRY ("Replacing the contents of a Sound with a formula")
NORMAL ("If you don't use $self in your formula, the formula does not refer to the existing contents of the Sound. Hence, the formula ")
CODE ("1/2 * sin (2 * pi * 377 * x)")
NORMAL ("will simply replace your recorded speech with a 377-Hz sine wave.")
ENTRY ("Modifying a Matrix with a formula")
NORMAL ("Many objects can be thought of as matrices: "
	"they consist of a number of rows and columns with data in every cell:")
LIST_ITEM ("@Sound: one row; columns represent samples.")
LIST_ITEM ("@Spectrum: two rows (first row is real part, second row is imaginary part); "
	"columns represent frequencies.")
LIST_ITEM ("@Spectrogram, @Cochleagram: rows represent frequencies; columns represent times.")
LIST_ITEM ("@Excitation: one row; columns represent frequency bands.")
LIST_ITEM ("@Harmonicity: one row; columns represent time frames.")
NORMAL ("The formula is performed on every column of every row. The formula")
CODE ("self\\^ 2$")
NORMAL ("will square all matrix elements.")
NORMAL ("The formula first works on the first row, and in that row from the first column on; "
	"this can work recursively. The formula")
CODE ("self + self [row, col - 1]")
NORMAL ("integrates each row.")
ENTRY ("Referring to the current position in the object")
NORMAL ("You can refer to the current position in a Matrix (or Sound, etc.) by index or by %x and %y values:")
TAG ("#row")
DEFINITION ("the current row")
TAG ("#col")
DEFINITION ("the current column")
TAG ("#x")
DEFINITION ("the %x value associated with the current column:")
DEFINITION ("    for a Sound, Spectrogram, Cochleagram, or Harmonicity: time")
DEFINITION ("    for a Spectrum: frequency (Hertz)")
DEFINITION ("    for an Excitation: frequency (Bark)")
TAG ("#y")
DEFINITION ("the %y value associated with the current row:")
DEFINITION ("    for a Spectrogram: frequency (Hertz)")
DEFINITION ("    for a Cochleagram: frequency (Bark)")
ENTRY ("Referring to the contents of the object itself")
NORMAL ("You can refer to values in the current Matrix (or Sound, etc.) by index.")
TAG ("##self")
DEFINITION ("refers to the value in the current Matrix at the current row and column, or to the value in the current Sound at the current sample.")
TAG ("##self [%%column-expression%]")
DEFINITION ("refers to the value in the current Sound (or Intensity etc.) at the current sample (or frame). "
	"The %%column-expression% is rounded to the nearest integer. "
	"If the index is out of range (less than 1 or greater than %n__%x_), the expression evaluates as 0.")
NORMAL ("#Example. An integrator is")
CODE ("   self [col - 1] + self * dx")
TAG ("##self [%%row-expression%, %%column-expression%]")
DEFINITION ("refers to the value in the current Matrix (or Spectrogram etc.) at the specified row and column. "
	"The expressions are rounded to the nearest integers.")
NORMAL ("You can refer to values in the current Matrix (or Spectrogram, etc.) by %x and %y position:")
TAG ("\\bu ##self (%%x-expression%, %%y-expression%)")
DEFINITION ("the expressions are linearly interpolated between the four nearest matrix points.")
NORMAL ("You can refer to values in the current Sound (or Intensity etc.) by %x position:")
TAG ("\\bu ##self (%%x-expression%)")
DEFINITION ("the expression is linearly interpolated between the two nearest samples (or frames).")
MAN_END

MAN_BEGIN ("Formulas 1.9. Formulas in scripts", "ppgb", 20030309)
INTRO ("In scripts, you can assign numeric expressions to numeric variables, "
	"and string expressions to string variables. You can also use numeric and string variables in expressions.")
ENTRY ("Example: report a square")
NORMAL ("Choose @@New Praat script@ from the @@Control menu@. A script editor window will become visible. "
	"Type the following lines into that window:")
CODE ("x = 99")
CODE ("x2 = x * x")
CODE ("echo The square of 'x' is 'x2'.")
NORMAL ("This is an example of a simple @@Praat script@; it assigns the results of the numeric formulas $$99$ and $$x * x$ "
	"to the numeric variables %x and %x2. Note that the formula $$x * x$ itself refers to the variable %x. "
	"To run (execute) this script, type Command-R or choose #%Run from the %#Run menu. "
	"Praat will then write the following text into the Info window:")
CODE ("The square of 99 is 9801.")
NORMAL ("For more information on scripts, see the @Scripting tutorial.")
ENTRY ("Example: rename the city of Washington")
NORMAL ("Type the following text into the script editor window:")
CODE ("current\\$  = \"Bush\"")
CODE ("previous\\$  = \"Clinton\"")
CODE ("famous\\$  = \"Lincoln\"")
CODE ("newCapital\\$  = current\\$  + mid\\$  (famous\\$ , 2, 3) + right\\$  (previous\\$ , 3)")
CODE ("echo The new capital will be 'newCapital\\$ '.")
NORMAL ("This script assigns the results of four string expressions to the four string variables %%current\\$ %, "
	"%%previous\\$ %, %%famous\\$ %, and %%newCapital\\$ %. The dollar sign is the notation for a string variable or "
	"for a function whose result is a string (like ##left\\$ #). Note that the formula in the fourth line refers to three existing "
	"variables.")
NORMAL ("To see what the new name of the capital will be, choose #%Run.")
ENTRY ("Example: numeric expressions in settings in scripts")
NORMAL ("As in real settings windows, you can use numeric expressions in all numeric fields. "
	"The example of the previous page becomes:")
CODE ("Create Sound... sine 0 10000/22050 22050   0.9 * sin (2*pi*377*x)")
NORMAL ("If the numeric field is not the last field of the settings window, "
	"you will want to write the formula without any spaces, "
	"e.g. as 10000/22050, since spaces are used to separate the fields.")
ENTRY ("Example: string expressions in settings in scripts")
NORMAL ("As in real settings windows, you cannot use string expressions in text fields directly, "
	"but you can still use the trick of variable substitution with single quotes (see @@Scripting 5.1. Variables@):")
CODE ("soundName\\$  = \"hello\"")
CODE ("fileName\\$  = soundName\\$  + \".wav\"")
CODE ("Read from file... 'fileName\\$ '")
ENTRY ("Example: numeric expressions in creation in scripts")
NORMAL ("Suppose you want to generate a sine wave whose frequency is held in a variable. This is the way:")
CODE ("frequency = 377")
CODE ("Create Sound... sine 0 1 22050   0.9 * sin (2*pi*frequency*x)")
NORMAL ("In this example, Praat will protest if %x is a variable as well, because that would be ambiguous "
	"with the %x that refers to the time in the sound (see @@Formulas 1.8. Formulas for modification@).")
MAN_END

MAN_BEGIN ("Formulas 2. Operators", "ppgb", 20030211)
NORMAL ("In formulas you can use the numerical and logical operators that are described on this page. "
	"The order of evaluation of the operators is the order that is most usual in programming languages. "
	"To force a different order, you use parentheses.")
NORMAL ("The operators with the highest precedence are #negation (-) and #exponentation (\\^ ):")
CODE ("--6 \\-> 6")
CODE ("2\\^ 6 \\-> 64")
NORMAL ("Sequences of negation and exponentiation are evaluated from right to left:")
CODE ("2\\^ -6 \\-> 0.015625")
CODE ("-(1+1)\\^ 6 \\-> -64")
CODE ("4\\^ 3\\^ 2 \\-> 4\\^ 9 \\-> 262144")
NORMAL ("Note that changing the spacing does not change the meaning:")
CODE ("4\\^ 3 \\^  2 \\-> 262144")
NORMAL ("To change the order of evaluation, you have to use parentheses:")
CODE ("(4 \\^  3) \\^  2 \\-> 4096")
NORMAL ("The following construction is not allowed because of an ambiguity between a negative number "
	"and negation of a positive number:")
CODE ("-2\\^ 6 \\-> ?")
NORMAL ("Instead, you use any of the following:")
CODE ("(-2)\\^ 6 \\-> 64")
CODE ("-(2\\^ 6) \\-> -64")
CODE ("-(2)\\^ 6 \\-> -64")
NORMAL ("The operators with the next highest precedence are #multiplication (*) and #division (/). They are evaluated "
	"from left to right:")
CODE ("1/4*5 \\-> 1.25        (from left to right)")
CODE ("1 / 4*5 \\-> 1.25      (spacing does not help)")
CODE ("1 / (4*5) \\-> 0.05    (use parentheses to change the order)")
CODE ("3 * 2 \\^  4 \\-> 48      (exponentiation before multiplication)")
CODE ("3*2 \\^  4 \\-> 48        (this spacing does not matter and is misleading)")
CODE ("(3 * 2) \\^  4 \\-> 1296  (use parentheses to change the order)")
NORMAL ("##Integer division# operators (#div and #mod) have the same precedence as * and /, "
	"and are likewise evaluated from left to right:")
CODE ("54 div 5 \\-> 10       (division rounded down)")
CODE ("54 mod 5 \\-> 4        (the remainder)")
CODE ("54.3 div 5.1 \\-> 10   (works for real numbers as well)")
CODE ("54.3 mod 5.1 \\-> 3.3  (the remainder)")
CODE ("-54 div 5 \\-> -11     (division rounded down; negation before division)")
CODE ("-54 mod 5 \\-> 1       (the remainder)")
CODE ("-(54 div 5) \\-> -10   (use parentheses to change the order)")
CODE ("-(54 mod 5) \\-> -4")
CODE ("3 * 18 div 5 \\-> 10   (from left to right)")
CODE ("3 * (18 div 5) \\-> 9")
CODE ("3 * 18 mod 5 \\-> 4")
CODE ("3 * (18 mod 5) \\-> 9")
CODE ("54 div 5 * 3 \\-> 30   (from left to right)")
CODE ("54 div (5 * 3) \\-> 3")
CODE ("54 mod 5 * 3 \\-> 12")
CODE ("54 mod (5 * 3) \\-> 9")
NORMAL ("The operators with the next highest precedence are #addition (+) and #subtraction (-), "
	"evaluated from left to right:")
CODE ("3 - 8 + 7 \\-> 2       (from left to right)")
CODE ("3 - (8 + 7) \\-> -12   (use parentheses to change the order)")
CODE ("3 + 8 * 7 \\-> 59      (multiplication before addition)")
CODE ("(3 + 8) * 7 \\-> 77    (use parentheses to change the order)")
CODE ("3 + - 2 \\^  4 \\-> -13   (exponentiation, negation, addition)")
CODE ("3 + 5 / 2 + 3 \\-> 8.5")
CODE ("(3 + 5) / (2 + 3) \\-> 1.6")
NORMAL ("The operators with the next highest precedence are the #comparison operators "
	"(=  <>  <  >  <=  >=). These operators always yield 0 (%false) or 1 (%true):")
CODE ("5 + 6 = 10 \\-> 0      (equal)")
CODE ("5 + 6 = 11 \\-> 1")
CODE ("5 + 6 <> 10 \\-> 1     (unequal)")
CODE ("5 + 6 <> 11 \\-> 0")
CODE ("5 + 6 < 10 \\-> 0      (less than)")
CODE ("5 + 6 < 11 \\-> 0")
CODE ("5 + 6 > 10 \\-> 1      (greater than)")
CODE ("5 + 6 > 11 \\-> 0")
CODE ("5 + 6 <= 10 \\-> 0     (less than or equal)")
CODE ("5 + 6 <= 11 \\-> 1")
CODE ("5 + 6 >= 10 \\-> 1     (greater or equal)")
CODE ("5 + 6 >= 11 \\-> 1")
NORMAL ("The comparison operators are mainly used in #if, #while, and #until conditions.")
NORMAL ("The operators of lowest precedence are the #logical operators (#not, #and, and #or), of which #not has the "
	"highest precedence and #or the lowest:")
CODE ("not 5 + 6 = 10 \\-> 1")
CODE ("x > 5 and x < 10               (is x between 5 and 10?)")
CODE ("not x <= 5 and not x >= 10     (same as previous line)")
CODE ("not (x <= 5 or x >= 10)        (same as previous line)")
ENTRY ("String comparison")
TAG ("##a\\$  = b\\$ ")
DEFINITION ("gives the value %true (= 1) if the strings are equal, and %false (= 0) otherwise.")
TAG ("##a\\$  <> b\\$ ")
DEFINITION ("gives the value %true if the strings are unequal, and %false otherwise.")
TAG ("##a\\$  < b\\$ ")
DEFINITION ("gives %true if the string %%a\\$ % precedes the string %%b\\$ % in ASCII sorting order. "
	"Thus, \"ha\" < \"hal\" and \"ha\" < \"ja\" are true, but \"ha\" < \"JA\" is false, "
	"because all capitals precede all lower-case characters in the ASCII sorting order.")
TAG ("##a\\$  > b\\$ ")
DEFINITION ("%true if %%a\\$ % comes after %%b\\$ % in ASCII sorting order. ")
TAG ("##a\\$  <= b\\$ ")
DEFINITION ("gives the value %true if the string %%a\\$ % precedes the string %%b\\$ % in ASCII sorting order, "
	"or if the strings are equal.")
TAG ("##a\\$  >= b\\$ ")
DEFINITION ("%true if %%a\\$ % comes after %%b\\$ % or the two are equal.")
ENTRY ("String concatenation and truncation")
TAG ("##a\\$  + b\\$ ")
DEFINITION ("concatenates the two strings. After")
CODE1 ("text\\$  = \"hallo\" + \"dag\"")
DEFINITION ("The variable %%text\\$ % contains the string \"hallodag\".")
TAG ("##a\\$  - b\\$ ")
DEFINITION ("subtracts the second string from the end of the first. After")
CODE2 ("soundFileName\\$  = \"hallo.aifc\"")
CODE2 ("textgridFileName\\$  = soundFileName\\$  - \".aifc\" + \".TextGrid\"")
DEFINITION ("the variable %%textgridFileName\\$ % contains the string \"hallo.TextGrid\". "
	"If the first string %%a\\$ % does not end in the string %%b\\$ %, the result of the subtraction is the string %%a\\$ %.")
MAN_END

MAN_BEGIN ("Formulas 3. Constants", "ppgb", 20030211)
TAG ("##pi")
DEFINITION ("%\\pi, 3.14159265358979323846")
TAG ("##e")
DEFINITION ("%e, 2.7182818284590452354")
TAG ("##undefined")
DEFINITION ("a special value, see @undefined")
MAN_END

MAN_BEGIN ("Formulas 4. Mathematical functions", "ppgb", 20030726)
TAG ("##abs (%x)")
DEFINITION ("absolute value")
TAG ("##round (%x)")
DEFINITION ("nearest integer; round (1.5) = 2")
TAG ("##floor (%x)")
DEFINITION ("round down: highest integer value not greater than %x")
TAG ("##ceiling (%x)")
DEFINITION ("round up: lowest integer value not less than %x")
TAG ("##sqrt (%x)")
DEFINITION ("square root: \\Vr%x, %x \\>_ 0")
TAG ("##min (%x, ...)")
DEFINITION ("the minimum of a series of numbers, e.g. min (7.2, -5, 3) = -5")
TAG ("##max (%x, ...)")
DEFINITION ("the maximum of a series of numbers, e.g. max (7.2, -5, 3) = 7.2")
TAG ("##imin (%x, ...)")
DEFINITION ("the location of the minimum, e.g. imin (7.2, -5, 3) = 2")
TAG ("##imax (%x, ...)")
DEFINITION ("the location of the maximum, e.g. imax (7.2, -5, 3) = 1")
TAG ("##sin (%x)")
DEFINITION ("sine")
TAG ("##cos (%x)")
DEFINITION ("cosine")
TAG ("##tan (%x)")
DEFINITION ("tangent")
TAG ("##arcsin (%x)")
DEFINITION ("arcsine, -1 \\<_ %x \\<_ 1")
TAG ("##arccos (%x)")
DEFINITION ("arccosine, -1 \\<_ %x \\<_ 1")
TAG ("##arctan (%x)")
DEFINITION ("arctangent")
TAG ("##arctan2 (%y, %x)")
DEFINITION ("argument angle")
TAG ("##exp (%x)")
DEFINITION ("exponentiation: %e^%x; same as ##e\\^ %x")
TAG ("##ln (%x)")
DEFINITION ("natural logarithm, base %e")
TAG ("##log10 (%x)")
DEFINITION ("logarithm, base 10")
TAG ("##log2 (%x)")
DEFINITION ("logarithm, base 2")
TAG ("##sinh (%x)")
DEFINITION ("hyperbolic sine: (%e^%x - %e^^-%x^) / 2")
TAG ("##cosh (%x)")
DEFINITION ("hyperbolic cosine: (%e^%x + %e^^-%x^) / 2")
TAG ("##tanh (%x)")
DEFINITION ("hyperbolic tangent: sinh (%x) / cosh (%x)")
TAG ("##arcsinh (%x)")
DEFINITION ("inverse hyperbolic sine: ln (%x + \\Vr(1+%x^2))")
TAG ("##arccosh (%x)")
DEFINITION ("inverse hyperbolic cosine: ln (%x + \\Vr(%x^2\\--1))")
TAG ("##arctanh (%x)")
DEFINITION ("inverse hyperbolic tangent")
TAG ("##sigmoid (%x)")
DEFINITION ("#R \\-> (0,1): 1 / (1 + %e^^\\--%x^) or 1 \\-- 1 / (1 + %e^%x)")
TAG ("##erf (%x)")
DEFINITION ("the error function: 2/\\Vr%\\pi __0_\\in^%x exp(-%t^2) %dt")
TAG ("##erfc (%x)")
DEFINITION ("the complement of the error function: 1 - erf (%x)")
TAG ("##randomUniform (%min, %max)")
DEFINITION ("uniform random deviate between %min (inclusive) and %max (exclusive)")
TAG ("##randomInteger (%min, %max)")
DEFINITION ("uniform random deviate between %min and %max (inclusive)")
TAG ("##randomGauss (%\\mu, %\\si)")
DEFINITION ("Gaussian random deviate with mean %\\mu and standard deviation %\\si")
TAG ("##randomPoisson (%mean)")
DEFINITION ("Poisson random deviate")
TAG ("##lnGamma (%x)")
DEFINITION ("logarithm of the \\Ga function")
TAG ("##gaussP (%z)")
DEFINITION ("the area under the Gaussian distribution between \\--\\oo and %z")
TAG ("##gaussQ (%z)")
DEFINITION ("the area under the Gaussian distribution between %z and +\\oo: "
	"the one-tailed \"statistical significance %p\" of a value that is %z standard deviations "
	"away from the mean of a Gaussian distribution")
TAG ("##invGaussQ (%q)")
DEFINITION ("the value of %z for which $gaussQ (%z) = %q")
TAG ("##chiSquareP (%chiSquare, %df)")
DEFINITION ("the area under the %\\ci^2 distribution between 0 and %chiSquare, for %df degrees of freedom")
TAG ("##chiSquareQ (%chiSquare, %df)")
DEFINITION ("the area under the %\\ci^2 distribution between %chiSquare and +\\oo, "
	"for %df degrees of freedom: the \"statistical significance %p\" "
	"of the %\\ci^2 difference between two distributions in %df+1 dimensions")
TAG ("##invChiSquareQ (%q, %df)")
DEFINITION ("the value of %\\ci^2 for which $chiSquareQ (%\\ci^2, %df) = %q")
TAG ("##studentP (%t, %df)")
DEFINITION ("the area under the student T-distribution from -\\oo to %t")
TAG ("##studentQ (%t, %df)")
DEFINITION ("the area under the student T-distribution from %t to +\\oo")
TAG ("##invStudentQ (%q, %df)")
DEFINITION ("the value of %t for which $studentQ (%t, %df) = %q")
TAG ("##fisherP (%f, %df1, %df2)")
DEFINITION ("the area under Fisher's F-distribution from 0 to %f")
TAG ("##fisherQ (%f, %df1, %df2)")
DEFINITION ("the area under Fisher's F-distribution from %f to +\\oo")
TAG ("##invFisherQ (%q, %df1, %df2)")
DEFINITION ("the value of %f for which $fisherQ (%f, %df1, %df2) = %q")
TAG ("##binomialP (%p, %k, %n)")
DEFINITION ("the probability that in %n experiments, an event with probability %p will occur at most %k times")
TAG ("@binomialQ (%p, %k, %n)")
DEFINITION ("the probability that in %n experiments, an event with probability %p will occur at least %k times; equals 1 - $binomialP (%p, %k - 1, %n)")
TAG ("##invBinomialP (%P, %k, %n)")
DEFINITION ("the value of %p for which $binomialP (%p, %k, %n) = %P")
TAG ("##invBinomialQ (%Q, %k, %n)")
DEFINITION ("the value of %p for which $binomialQ (%p, %k, %n) = %Q")
TAG ("##hertzToBark (%x)")
DEFINITION ("from acoustic frequency to Bark-rate (perceptual spectral frequency; place on basilar membrane): "
	"7 ln (%x/650 + \\Vr(1 + (%x/650)^2))")
TAG ("##barkToHertz (%x)")
DEFINITION ("650 sinh (%x / 7)")
TAG ("##hertzToMel (%x)")
DEFINITION ("from acoustic frequency to perceptual pitch: 550 ln (1 + %x / 550)")
TAG ("##melToHertz (%x)")
DEFINITION ("550 (exp (%x / 550) - 1)")
TAG ("##hertzToSemitones (%x)")
DEFINITION ("from acoustic frequency to a logarithmic musical scale, relative to 100 Hz: 12 ln (%x / 100) / ln 2")
TAG ("##semitonesToHertz (%x)")
DEFINITION ("100 exp (%x ln 2 / 12)")
TAG ("##erb (%f)")
DEFINITION ("the perceptual %%equivalent rectangular bandwidth% (ERB) in Hertz, for a specified acoustic frequency (also in Hertz): "
	"6.23\\.c10^^-6^ %f^2 + 0.09339 %f + 28.52")
TAG ("##hertzToErb (%x)")
DEFINITION ("from acoustic frequency to ERB-rate: 11.17 ln ((%x + 312) / (%x + 14680)) + 43")
TAG ("##erbToHertz (%x)")
DEFINITION ("(14680 %d - 312) / (1 - %d) where %d = exp ((%x - 43) / 11.17)")
TAG ("@phonToDifferenceLimens (%x)")
DEFINITION ("from perceptual loudness (intensity sensation) level in phon, to the number of intensity "
	"difference limens above threshold: 30 \\.c ((61/60)^^ %x^ \\-- 1).")
TAG ("@differenceLimensToPhon (%x)")
DEFINITION ("the inverse of the previous: ln (1 + %x / 30) / ln (61 / 60).")
TAG ("##beta (%x, %y)")
TAG ("##besselI (%n, %x)")
TAG ("##besselK (%n, %x)")
MAN_END

MAN_BEGIN ("Formulas 5. String functions", "ppgb", 20040414)
INTRO ("String functions are functions that either return a text string or have at least one text string as an argument. "
	"Since string computations are not very useful in the @calculator, in settings windows, or in creation and "
	"modification formulas, this page only gives examples of strings in scripts, so that the example may contain "
	"string variables.")
TAG ("##length (a\\$ )")
DEFINITION ("gives the length of the string. After")
CODE2 ("string\\$  = \"hallo\"")
CODE2 ("length = length (string\\$  + \"dag\")")
DEFINITION ("the variable %length contains the number 8. From this example, "
	"you see that variables can have the same names as functions, without any danger of confusing the interpreter).")
TAG ("##left\\$  (a\\$ , n)")
DEFINITION ("gives a string consisting of the first %n characters of %%a\\$ %. After")
CODE2 ("head\\$  = left\\$  (\"hallo\", 3)")
DEFINITION ("the variable %%head\\$ % contains the string \"hal\".")
TAG ("##right\\$  (a\\$ , n)")
DEFINITION ("gives a string consisting of the last %n characters of %%a\\$ %. After")
CODE2 ("english\\$  = \"he\" + right\\$  (\"hallo\", 3)")
DEFINITION ("the variable %%english\\$ % contains the string \"hello\".")
TAG ("##mid\\$  (\"hello\" , 3, 2)")
DEFINITION ("gives a string consisting of 2 characters from \"hello\", starting at the third character. Outcome: ll.")
TAG ("##index (a\\$ , b\\$ )")
DEFINITION ("gives the index of the first occurrence of the string %%b\\$ % in the string %%a\\$ %. After")
CODE2 ("where = index (\"hallo allemaal\", \"al\")")
DEFINITION ("the variable %where contains the number 2, because the first \"al\" starts at the second character of the longer string. "
	"If the first string does not contain the second string, %index returns 0.")
TAG ("##rindex (a\\$ , b\\$ )")
DEFINITION ("gives the index of the last occurrence of the string %%b\\$ % in the string %%a\\$ %. After")
CODE2 ("where = rindex (\"hallo allemaal\", \"al\")")
DEFINITION ("the variable %where contains the number 13, because the last \"al\" starts at the 13th character. "
	"If the first string does not contain the second string, %rindex returns 0.")
TAG ("##fixed\\$  (number, precision)")
DEFINITION ("formats a number as a string with %precision digits after the decimal point. Thus, $$fixed\\$  (72.65687, 3)$ "
	"becomes the string $$72.657$, and $$fixed\\$  (72.65001, 3)$ becomes the string $$72.650$. "
	"In these examples, we see that the result can be rounded up and that trailing zeroes are kept. "
	"At least one digit of precision is always given, e.g. $$fixed\\$  (0.0000157, 3)$ becomes the string $$0.00002$. "
	"The number 0 always becomes the string $0.")
TAG ("##percent\\$  (number, precision)")
DEFINITION ("the same as ##fixed\\$ #, but with a percent sign. For instance, $$percent\\$ (0.157, 3)$ becomes $$15.700\\% $, "
	"$$percent\\$ (0.000157, 3)$ becomes $$0.016\\% $, and $$percent\\$ (0.000000157, 3)$ becomes $$0.00002\\% $. "
	"The number 0 always becomes the string $0.")
TAG ("##date\\$  ()")
DEFINITION ("gives the date and time in the following format:")
CODE2 ("Mon Jun 24 17:11:21 2002")
DEFINITION ("To write the day of the month into the Info window, you type:")
CODE2 ("date\\$  = date\\$  ()")
CODE2 ("day\\$  = mid\\$  (date\\$ , 9, 2)")
CODE2 ("echo The month day is 'day\\$ '.")
TAG ("##extractNumber (\"Type: Sound'newline\\$ 'Name: hello there'newline\\$ 'Size: 44007\", \"Size:\")")
DEFINITION ("looks for a number after the first occurrence of \"Size:\" in the long string. Outcome: 44007. "
	"This is useful in scripts that try to get information from long reports, as the following script that "
	"runs in the Sound editor window:")
CODE2 ("report\\$  = Settings report")
CODE2 ("maximumFrequency = extractNumber (report\\$ , \"Spectrogram maximum frequency:\")")
TAG ("##extractWord\\$  (\"Type: Sound'newline\\$ 'Name: hello there'newline\\$ 'Size: 44007\", \"Type:\")")
DEFINITION ("looks for a word without spaces after the first occurrence of \"Type:\" in the long string. Outcome: Sound.")
TAG ("##extractLine\\$  (\"Type: Sound'newline\\$ 'Name: hello there'newline\\$ 'Size: 44007\", \"Name: \")")
DEFINITION ("looks for the rest of the line (including spaces) after the first occurrence of \"Name: \" in the long string. "
	"Outcome: hello there. Note how \"Name: \" includes a space, so that the `rest of the line' starts with the %h.")
MAN_END

MAN_BEGIN ("Formulas 6. Control structures", "ppgb", 20030519)
ENTRY ("if ... then ... else ... fi")
NORMAL ("You can use conditional expressions in all formulas. For example, ")
CODE ("3 * if 52\\%  * 3809 > 2000 then 5 else 6 fi")
NORMAL ("evaluates to 15. Instead of %fi, you can also use %endif.")
NORMAL ("Another example: you can clip the absolute amplitude of a Sound to 0.5 by supplying the following formula:")
CODE ("if abs(self)>0.5 then if self>0 then 0.5 else -0.5 fi else self fi")
ENTRY ("The semicolon")
NORMAL ("The semicolon ends the evaluation of the formula. This can be convenient "
	"if you do not want to overwrite a long formula in your text field: the formula")
CODE ("800;sqrt(2)*sin(2*pi*103*0.5)+10\\^ (-40/20)*randomGauss(0,1)")
NORMAL ("evaluates to 800.")
MAN_END

MAN_BEGIN ("Formulas 7. Attributes of objects", "ppgb", 20021204)
NORMAL ("You can refer to several attributes of objects that are visible in the @@List of Objects@. "
	"To do so, use the type and the name of the object, connected with an underscore. "
	"Thus, $$Sound_hallo$ refers to an existing Sound object whose name is \"hallo\" "
	"(if there is more than one such object, it refers to the one that was created last).")
NORMAL ("To refer to an attribute, you use the period ( `.'). "
	"Thus, $$Sound_hallo.nx$ is the number of samples of Sound_hallo, and "
	"$$1 / Sound_hallo.dx$ is the sampling frequency of Sound_hallo.")
ENTRY ("Attributes in the calculator")
NORMAL ("Record a Sound (read the @Intro if you do not know how to do that), "
	"name it \"mysound\" (or anything else), and type the following formula into the @calculator:")
CODE ("Sound_mysound.nx")
NORMAL ("After you click OK, the Info window will show the number of samples. Since you could have got this result "
	"by simply choosing ##%%Get number of samples#% from the @Query menu, these attribute tricks are not very "
	"useful in the calculator. We will see that they are much more useful in creation and modification formulas and in scripts.")
ENTRY ("List of possible attributes")
NORMAL ("The following attributes are available:")
TAG ("#xmin")
DEFINITION ("the start of the time domain (usually 0) for a @Sound, @Pitch, @Formant, "
	"@Spectrogram, @Intensity, @Cochleagram, @PointProcess, or @Harmonicity object, in seconds; "
	"the lowest frequency (always 0) for a @Spectrum object, in Hertz; "
	"the lowest frequency (usually 0) for an @Excitation object, in Bark; "
	"the left edge of the %x domain for a @Matrix object.")
TAG ("#xmax")
DEFINITION ("the end of the time domain (usually the duration, if %xmin is zero) for a Sound, Pitch, Formant, "
	"Spectrogram, Intensity, Cochleagram, PointProcess, or Harmonicity object, in seconds; "
	"the highest frequency (@@Nyquist frequency@) for a Spectrum object, e.g. 11025 Hertz; "
	"the highest frequency for an Excitation object, often 25.6 Bark; "
	"the right edge of the %x domain for a Matrix object.")
TAG ("#ncol")
DEFINITION ("the number of columns in a @Matrix, @TableOfReal, or @Table object.")
TAG ("#nrow")
DEFINITION ("the number of rows in a @Matrix, @TableOfReal, or @Table object.")
TAG ("#nx")
DEFINITION ("the number of samples in a Sound object; "
	"the number of analysis frames in a Pitch, Formant, Spectrogram, Intensity, Cochleagram, or Harmonicity object; "
	"the number of frequency bins in a Spectrum or Excitation object; "
	"the number of divisions of the %x domain for a Matrix object (= %ncol).")
TAG ("#dx")
DEFINITION ("the sample period (time distance between consecutive samples) in a Sound object (the inverse of the sampling frequency), in seconds; "
	"the time step between consecutive frames in a Pitch, Formant, Spectrogram, Intensity, Cochleagram, or Harmonicity object, in seconds; "
	"the width of a frequency bin in a Spectrum object, in Hertz; "
	"the width of a frequency bin in an Excitation object, in Bark; "
	"the horizontal distance between cells in a Matrix object.")
TAG ("#ymin")
DEFINITION ("the lowest frequency (usually 0) for a Spectrogram object, in Hertz; "
	"the lowest frequency (usually 0) for a Cochleagram object, in Bark; "
	"the bottom of the %y domain for a Matrix object.")
TAG ("#ymax")
DEFINITION ("the highest frequency for a Spectrogram object, e.g. 5000 Hertz; "
	"the highest frequency for a Cochleagram object, often 25.6 Bark; "
	"the top of the %y domain for a Matrix object.")
TAG ("#ny")
DEFINITION ("the number of frequency bands in a Spectrogram or Cochleagram object; "
	"for a Spectrum object: always 2 (first row is real part, second row is imaginary part) "
	"the number of divisions of the %y domain for a Matrix object (= %nrow).")
TAG ("#dy")
DEFINITION ("the distance between adjacent frequency bands in a Spectrogram object, in Hertz; "
	"the distance between adjacent frequency bands in a Cochleagram object, in Hertz; "
	"the vertical distance between cells in a Matrix object.")
ENTRY ("Attributes in a creation formula")
NORMAL ("In formulas for creating a new object, you can refer to the attributes of any object, "
	"but you will often want to refer to the attributes of the object that is just being created. You can do that in two ways.")
NORMAL ("The first way is to use the name of the object, as above. Choose @@Create Sound...@, supply %hello for its name, "
	"supply arbitrary values for the starting and finishing time, and type the following formula:")
CODE ("(x - Sound_hello.xmin) / (Sound_hello.xmax - Sound_hello.xmin)")
NORMAL ("When you edit this sound, you can see that it creates a straight line that rises from 0 to 1 within the time domain.")
NORMAL ("The formula above will also work if the Sound under creation is called %goodbye, and a Sound called %hello already exists; "
	"of course, in such a case $$Sound_hello.xmax$ refers to a property of the already existing sound.")
NORMAL ("If a formula refers to an object under creation, there is a shorter way: you do not have to supply the name of the object at all, "
	"so you can simply write")
CODE ("(x - xmin) / (xmax - xmin)")
NORMAL ("The attributes that you can use in this implicit way are %xmin, %xmax, %ncol, %nrow, %nx, %dx, %ny, and %dy.")
ENTRY ("Attributes in a modification formula")
NORMAL ("In formulas for modifying an existing object, you refer to attributes in the same way as in creation formulas, "
	"i.e., you do not have to specify the name of the object that is being modified. The formula")
CODE ("self * 20 \\^  (- (x - xmin) / (xmax - xmin))")
NORMAL ("causes the sound to decay exponentially in such a way that it has only 5 percent of its initial amplitude at the end. "
	"If you apply this formula to multiple Sound objects at the same time, %xmax will refer to the finishing time of each Sound separately "
	"as it is modified.")
NORMAL ("More examples of the use of attributes are on the next page.")
MAN_END

MAN_BEGIN ("Formulas 8. Data in objects", "ppgb", 20021204)
NORMAL ("With square brackets, you can get the values inside some objects.")
ENTRY ("Object contents in the calculator")
NORMAL ("The outcomes of the following examples can be checked with the @calculator.")
TAG ("##Matrix_hello [10, 3]")
DEFINITION ("gives the value in the cell at the third column of the 10th row of the Matrix called %hello.")
TAG ("##Sound_hello [10000]")
DEFINITION ("gives the value (in Pa) of the 1000th sample of the Sound %hello.")
TAG ("##TableOfReal_tokens [5, 12]")
DEFINITION ("gives the value in the cell at the fifth row of the 12th column of the TableOfReal called %tokens.")
TAG ("##TableOfReal_tokens [5, \"F1\"]")
DEFINITION ("gives the value in the cell at the fifth row of the column labelled %F1 of the TableOfReal %tokens.")
TAG ("##TableOfReal_tokens [\"\\bsct\", \"F1\"]")
DEFINITION ("gives the value in the cell at the row labelled %%\\bsct% of column %F1 of the TableOfReal %tokens.")
TAG ("##Table_listeners [3, \"m3ae\"]")
DEFINITION ("gives the numeric value in the cell at the third row of column %m3ae of the Table %listeners.")
TAG ("##Table_listeners [3, 12]")
DEFINITION ("gives the numeric value in the cell at the third row of the 12th column of the Table %listeners.")
NORMAL ("Cells outside the objects are considered to contain zeroes.")
ENTRY ("Interpolation")
NORMAL ("The values inside some objects can be interpolated.")
TAG ("##Sound_hallo (0.7)")
DEFINITION ("gives the value (in Pa) at a time of 0.7 seconds in the Sound \"hallo\", by linear interpolation between "
	"the two samples that are nearest to 0.7 seconds.")
TAG ("##Spectrogram_hallo (0.7, 2500)")
DEFINITION ("gives the value at a time of 0.7 seconds and at a frequency of 2500 Hz in the Spectrogram \"hallo\", "
	"by linear interpolation between the four samples that are nearest to that point.")
NORMAL ("In the interpolation, cells outside the objects are considered to contain zeroes.")
ENTRY ("Object contents in a modification formula")
NORMAL ("Suppose you want to do the difficult way of reversing the contents of a Sound called %hello (the easy way is to choose #%Reverse "
	"from the @Modify menu). You select this sound, then choose @@Copy...@ to duplicate it to a new Sound, which you name %%hello_reverse. "
	"You select this new Sound and choose ##%%Formula...#% from the @Modify menu. The formula will be")
CODE ("Sound_hello [ncol + 1 - col]")
NORMAL ("From this example, you see that the indices between [ ] may be formulas themselves, and that you can use implicit attributes like %ncol "
	"and position references like %col. An alternative formula is")
CODE ("Sound_hello (xmax - x)")
NORMAL ("at least if %xmin is zero. The advantage of the second method is that is also works correctly if the two sounds have different sampling frequencies; "
	"the disadvantage is that it may do some interpolation between the samples, which deteriorates the sound quality.")
ENTRY ("Object contents in a script")
NORMAL ("In scripts, the indices between [ ] and the values between ( ) may be formulas themselves and contain variables. "
	"The following script computes the sum of all the cells along the diagonal of a Matrix named %hello.")
CODE ("sumDiagonal = 0")
CODE ("for i to Matrix_hello.ncol")
CODE1 ("sumDiagonal += Matrix_hello [i, i]")
CODE ("endfor")
CODE ("echo The sum of cells along the diagonal is 'sumDiagonal'.")
NORMAL ("This example could have been written completely with commands from the dynamic menu:")
CODE ("select Matrix hello")
CODE ("sumDiagonal = 0")
CODE ("ncol = Get number of columns")
CODE ("for i to ncol")
CODE1 ("value = Get value in cell... i i")
CODE1 ("sumDiagonal += value")
CODE ("endfor")
CODE ("echo The sum of cells along the diagonal is 'sumDiagonal'.")
NORMAL ("The first version, which accesses the contents directly, is not only three lines shorter, but also three times faster.")
MAN_END

MAN_BEGIN ("Hidden commands", "ppgb", 20030528)
NORMAL ("Some commands in P\\s{RAAT}'s fixed and dynamic menus are hidden by default. "
	"You can still call hidden commands from scripts, run them by clicking on them in a @ButtonEditor, "
	"or make them visible with the help of the @ButtonEditor.")
NORMAL ("To hide commands that are visible by default, use the @ButtonEditor.")
ENTRY ("What commands are hidden by default?")
NORMAL ("Commands that are expected to be of very limited use, are hidden by default. Examples are:")
LIST_ITEM ("1. The commands @@Add menu command...@, ##Hide menu command...#, ##Show menu command...#, "
	"@@Add action command...@, ##Hide action command...#, and ##Show action command...# in the #Control menu "
	"of the @@Object window@. These are used in the @@Buttons file@ and could be used by an @@Initialization script@ "
	"as well; in an interactive session, however, the functionality of these commands is part of the "
	"@ScriptEditor and the @ButtonEditor.")
LIST_ITEM ("2. The command ##Read from old Praat picture file...# in the #File menu of the @@Picture window@. "
	"For reading a file format that was in use before May, 1995.")
LIST_ITEM ("3. In the Praat program, the action ##Sound: Write to Sesam file...#. Writes a file format in common use "
	"in the Netherlands on Vax machines. In the Dutch phonetics departments, the plugs were pulled from the Vaxes in 1994.")
LIST_ITEM ("4. In the Praat program, the action ##Sound: To Cochleagram (edb)...#. Needed by one person in 1994. "
	"An interesting, but undocumented procedure (De Boer's gamma-tone filter bank plus Meddis & Hewitt's "
	"synapse model), which does not create a normally interpretable Cochleagram object.")
MAN_END

MAN_BEGIN ("History mechanism", "ppgb", 20040414)
INTRO ("The easiest way to do @@scripting@. "
	"The %history is the sequence of all menu commands "
	"(in the Objects or Picture window or in the editors), "
	"action commands (in the dynamic menu), "
	"or mouse clicks on objects (in the list of objects), that you performed during your Praat session, "
	"together with the settings that you specified in the settings windows "
	"that popped up as a result of those commands.")
ENTRY ("Viewing the history")
NORMAL ("To view your history, you first open a @ScriptEditor with @@New Praat script@ or @@Open Praat script...@. "
	"You then choose @@Paste history@ from the Edit menu.")
ENTRY ("Recording a macro")
NORMAL ("To record a sequence of mouse clicks for later re-use, "
	"perform the following steps:")
LIST_ITEM ("1. Choose @@Clear history@ from the Edit menu. "
	"This makes the history mechanism forget all previous clicks.")
LIST_ITEM ("2. Perform the actions that you want to record.")
LIST_ITEM ("3. Choose @@Paste history@ from the Edit menu. Because you cleared the history "
	"before you started, the resulting script contains only the actions "
	"that you performed in step 2. "
	"You can now already re-run the actions that you performed in step 2.")
LIST_ITEM ("4. You can save the recorded actions to a script file by choosing #Save from the File menu.")
LIST_ITEM ("5. You can put this script file under a button in the @@dynamic menu@ "
	"by choosing @@Add to dynamic menu...@ from the File menu, "
	"or under a button in a fixed menu by choosing @@Add to fixed menu...@. "
	"This button will be preserved across Praat sessions.")
NORMAL ("This macro mechanism is much more flexible than the usual opaque macro mechanism "
	"used by most programs, because you can edit the script and make some "
	"of the arguments variable by putting them in the #form clause at the top of the script. "
	"In this way, the script will prompt the user for these arguments, "
	"just as with all the menu and action commands that end in the three dots (...). "
	"See the @Scripting tutorial for all the things that you can do in scripts.")
MAN_END

MAN_BEGIN ("Initialization script", "ppgb", 20041229)
INTRO ("Your initialization script is a normal @@Praat script@ that is run as soon as you start Praat.")
#if defined (UNIX) || defined (__MACH__)
NORMAL ("On Unix or MacOS X, you create an initialization script by creating a file named \"praat-startUp\" "
	"in the directory /usr/local, "
	"or putting a file \".praat-user-startUp\" or \"praat-user-startUp\" in your home directory "
	"(if you rename the Praat executable, these names have to change as well).")
NORMAL ("If you have more than one of these files, they are run in the above order.")
#elif defined (_WIN32)
NORMAL ("On Windows NT or XP, you create an initialization script by creating a file named "
	"\"praat-user-startUp\" in your home directory, "
	"which on my Windows XP computer is C:\\bsDocuments and Settings\\bsPaul.")
NORMAL ("If you have both of these files, they are run in the above order.")
#elif defined (macintosh)
NORMAL ("It only works on Unix, Windows, and MacOS X, unless you ask me to make it available for MacOS 7/8/9.")
#endif
ENTRY ("Example")
NORMAL ("If you like to be greeted by your husband when Praat starts up, "
	"you could put the following lines in your initialization script:")
#if defined (UNIX)
	CODE ("Read from file... /u/miep/helloMiep.wav")
#elif defined (__MACH__)
	CODE ("Read from file... /Users/miep/helloMiep.wav")
#elif defined (_WIN32)
	CODE ("Read from file... C:\\bsDocuments and Settings\\bsMiep\\bshelloMiep.wav")
#elif defined (macintosh)
	CODE ("Read from file... Macintosh HD:helloMiep.wav")
#else
	#error Some audio file reading example should go here
#endif
CODE ("Play")
CODE ("Remove")
ENTRY ("What not to use an initialization script for")
NORMAL ("You could set preferences like the default font in your initialization script, "
	"but these will be automatically remembered between invocations of Praat anyway (in your @@Preferences file@), "
	"so this would often be superfluous. Added and removed commands are also remembered across "
	"Praat sessions (in your @@Buttons file@), but you may want to call a changeable list of them.")
ENTRY ("Using an initialization script for site-wide customization")
NORMAL ("If your research group shares a number of Praat scripts, these can be included in everybody's "
	"version of the program in the following way:")
LIST_ITEM ("1. Create a script that adds buttons to the fixed and dynamic menus, using the commands "
	"@@Add menu command...@ and @@Add action command...@. This script could be a slightly edited copy of someone's "
	"@@Buttons file@.")
#ifdef UNIX
LIST_ITEM ("2. Put this script in \"/usr/local/%%myProg%-startUp\", or have everyone call this script "
	"from her own start-up file (with @@Run script...@).")
#else
LIST_ITEM ("2. Have everyone call this script "
	"from her own start-up file (with @@Run script...@).")
#endif
NORMAL ("This procedure allows all members of the group to automatically enjoy all the later changes in the "
	"custom command set.")
MAN_END

MAN_BEGIN ("New Praat script", "ppgb", 20021130)
INTRO ("A command in the Control menu for creating a new Praat script. "
	"It creates a @ScriptEditor with an empty script that you can edit, run, and save.") 
MAN_END

MAN_BEGIN ("Open Praat script...", "ppgb", 20021130)
INTRO ("A command in the Control menu for editing an existing @@Praat script@. "
	"It creates a @ScriptEditor and asks "
	"you to select a file. If you click #%OK, the file is read into the ScriptEditor window, "
	"and you can run and edit it; if you click #%Cancel, you get an empty script, as with @@New Praat script@.") 
MAN_END

MAN_BEGIN ("Paste history", "ppgb", 19981107)
INTRO ("A command in the Control menu for viewing the history in a @ScriptEditor. "
	"See @@History mechanism@.")
MAN_END

MAN_BEGIN ("phonToDifferenceLimens", "ppgb", 20021215)
INTRO ("A routine for converting sensation level in phons into intensity difference limen level, "
	"the inverse of @differenceLimensToPhon.")
ENTRY ("Formula")
FORMULA ("phonToDifferenceLimens (%phon) = 30 \\.c ((61/60)^^ %phon^ \\-- 1)")
ENTRY ("Derivation")
NORMAL ("In first approximation, humans can detect an intensity difference of 1 phon, i.e. "
	"if two sounds that differ only in intensity are played a short time after each other, "
	"people can generally detect their intensity difference if it is greater than 1 phon.")
NORMAL ("But the sensitivity is somewhat better for louder sounds. "
	"According to @@Jesteadt, Wier & Green (1977)@, the relative difference limen "
	"of intensity is given by")
FORMULA ("DLI = \\De%I / %I = 0.463 \\.c (%I / %I__0_)^^ \\--0.072^")
NORMAL ("In this formula, %I is the intensity of the sound in Watt/m^2, %I__0_ is the intensity of "
	"the auditory threshold (i.e. 10^^\\--12^ Watt/m^2 at 1000 Hz), and \\De%I is the just noticeable difference.")
NORMAL ("@@Boersma (1998)|Boersma (1998: 109)@ calculates a difference-limen scale from this. "
	"Given an intensity %I, the number of difference limens above threshold is ")
FORMULA ("\\in__%I0_^%I %dx \\De%I(%x) "
	"= (1 / 0.463) \\in__%I0_^%I %dx %I__0_^^\\--0.072^ %x^^0.072\\--1^")
FORMULA ("= (1 / (0.463\\.c0.072)) ((%I/%I__0_)^^0.072^ \\-- 1)")
NORMAL ("The sensation level in phon is defined as")
FORMULA ("SL = 10 log__10_ (%I/%I__0_)")
NORMAL ("so that the number of difference limens above threshold is")
FORMULA ("(1 / (0.463\\.c0.072)) (10^^(0.072/10)(10log(%I/%I__0_))^ \\-- 1) "
	"= 30 \\.c (1.0167^^SL^ \\-- 1)")
MAN_END

MAN_BEGIN ("Praat script", "ppgb", 19980824)
INTRO ("An executable text that consists of menu commands and action commands.")
NORMAL ("See the @Scripting tutorial.")
MAN_END

MAN_BEGIN ("Preferences file", "ppgb", 20030519)
NORMAL ("The file into which some of your preferences are saved across your sessions with P\\s{RAAT}. "
	"For instance, if you change the font used by the Picture window to Palatino and leave P\\s{RAAT}, "
	"the Picture-window font will still be Palatino when you enter P\\s{RAAT} again.")
NORMAL ("The preferences file is written to disk when you leave P\\s{RAAT}, "
	"and it is read when you enter P\\s{RAAT}. It is a simple text file that you can read "
	"(but should not edit) with any text editor.")
ENTRY ("Unix")
NORMAL ("If your home directory is /people/miep "
	"the preferences file is /people/miep/.praat-dir/prefs. "
	"If the directory .praat-dir does not exist, it is created when you enter P\\s{RAAT}.")
ENTRY ("Macintosh")
NORMAL ("In MacOS X, the preferences file is called %Prefs, and it will be in the folder %%Praat Prefs% "
	"in the %Preferences folder of your personal %Library folder. On my iBook, the preferences file is "
	"##/Users/pboersma/Library/Preferences/Praat Prefs/Prefs#.")
NORMAL ("In MacOS 7, 8, or 9, the preferences file is also called %Prefs, and it will be in the folder "
	"%%Praat Preferences% in the Preferences folder in your System Folder. "
	"If your hard disk is called \"H\\ael' sji'f\", and you have a Dutch system, "
	"the complete path to your preferences file is:")
CODE ("H\\ael' sji'f:Systeemmap:Voorkeuren:Praat Preferences:Prefs")
ENTRY ("Windows")
NORMAL ("The preferences file is called %%Preferences.ini%, and it will be in the directory %Praat "
	"in your Windows directory, or in the directory %Praat in your home directory if you work on "
	"a shared computer. On my Virtual PC Windows XP Home Edition computer, "
	"the preferences file is ##C:\\bsDocuments and Settings\\bsPaul Boersma\\bsPraat\\bsPreferences.ini#.")
MAN_END

MAN_BEGIN ("Run script...", "ppgb", 20021130)
INTRO ("A hidden command in the #%Control menu of the @@Object window@. "
	"Runs a @@Praat script@.")
ENTRY ("Usage")
NORMAL ("This command is hidden because you would normally open a script "
	"with @@Open Praat script...@, so that you can run it several times without "
	"selecting a file each time.")
NORMAL ("In scripts, the command ##%%Run script...#% is automatically replaced "
	"by the script directive #execute.")
MAN_END

MAN_BEGIN ("Scripting", "ppgb", 20041028)
INTRO ("This is one of the tutorials of the P\\s{RAAT} program. It assumes you are familiar with the @Intro.")
NORMAL ("A %script is a text that consists of menu commands and action commands. "
	"If you %run the script (perhaps from a @ScriptEditor), "
	"the commands are executed as if you clicked on them.")
NORMAL ("You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM ("@@Scripting 1. My first script@ (how to create, how to run)")
LIST_ITEM ("@@Scripting 2. Arguments to commands@ (numeric, check, radio, text, file)")
LIST_ITEM ("@@Scripting 3. Layout@ (white space, comments, continuation lines)")
LIST_ITEM ("@@Scripting 4. Object selection@ (selecting and querying)")
LIST_ITEM ("@@Scripting 5. Language elements@")
LIST_ITEM1 ("@@Scripting 5.1. Variables@ (numeric, string, copy, substitution)")
LIST_ITEM1 ("@@Scripting 5.2. Formulas@ (numeric, string)")
LIST_ITEM1 ("@@Scripting 5.3. Jumps@ (if, then, elsif, else, endif)")
LIST_ITEM1 ("@@Scripting 5.4. Loops@ (for/endfor, while/endwhile, repeat/until)")
LIST_ITEM1 ("@@Scripting 5.5. Procedures@ (call, procedure)")
LIST_ITEM1 ("@@Scripting 5.6. Arrays")
LIST_ITEM1 ("@@Scripting 5.7. Including other scripts")
LIST_ITEM1 ("@@Scripting 5.8. Quitting@ (exit)")
LIST_ITEM ("@@Scripting 6. Communication outside the script")
LIST_ITEM1 ("@@Scripting 6.1. Arguments to the script@ (form/endform, execute)")
LIST_ITEM1 ("@@Scripting 6.2. Calling system commands@ (system, system_nocheck)")
LIST_ITEM1 ("@@Scripting 6.3. Writing to the Info window@ (echo, print, printtab, printline)")
LIST_ITEM1 ("@@Scripting 6.4. Query commands@ (Get, Count)")
LIST_ITEM1 ("@@Scripting 6.5. Files@ (fileReadable, <, >, >>, filedelete, fileappend)")
LIST_ITEM1 ("@@Scripting 6.6. Controlling the user@ (pause)")
LIST_ITEM1 ("@@Scripting 6.7. Sending a message to another program@ (sendsocket)")
LIST_ITEM1 ("@@Scripting 6.8. Messages to the user@ (exit, assert, nowarn, nocheck)")
LIST_ITEM1 ("@@Scripting 6.9. Calling from the command line")
LIST_ITEM ("@@Scripting 7. Scripting the editors")
LIST_ITEM1 ("@@Scripting 7.1. Scripting an editor from a shell script@ (editor/endeditor)")
LIST_ITEM1 ("@@Scripting 7.2. Scripting an editor from within")
LIST_ITEM ("@@Scripting 8. Controlling Praat from another program")
LIST_ITEM1 ("@@Scripting 8.1. The sendpraat subroutine")
LIST_ITEM1 ("@@Scripting 8.2. The sendpraat program")
LIST_ITEM1 ("@@Scripting 8.3. The sendpraat directive")
NORMAL ("Also see the @@scripting examples@.")
MAN_END

MAN_BEGIN ("Scripting 1. My first script", "ppgb", 20040414)
NORMAL ("Suppose that you want to create a script that allows you to play a selected Sound object twice. "
	"You first create an empty script, by choosing @@New Praat script@ from the Control menu. "
	"A @ScriptEditor will appear on your screen. In this editor, you type")
CODE ("Play")
CODE ("Play")
NORMAL ("Now select a #Sound in the object menu. As you expect from selecting a Sound, a #Play button will "
	"appear in the dynamic menu. If you now choose #Run in the ScriptEditor, the sound "
	"will be played twice.")
ENTRY ("What commands can I put into my script?")
NORMAL ("In the above example, you could use the \"Play\" command because that was "
	"the text on a button currently available button in the dynamic menu. "
	"Apart from these selection-dependent (dynamic) commands, "
	"you can also use all fixed commands in the menus of the @@Object window@ "
	"and the @@Picture window@. For how to proceed with commands that need %arguments "
	"(i.e. the commands that end in \"...\" and present a settings window), "
	"see @@Scripting 2. Arguments to commands|\\SS2@.")
ENTRY ("Faster ways of getting a script text")
NORMAL ("Instead of manually typing the command lines, as described above, it may be easier "
	"to create a script text from a %%macro recording%, with the @@History mechanism@. "
	"This allows you to create script texts without typing. "
	"For instance, if you choose @@Clear history@ in your ScriptEditor, then click the #%Play button twice (after selecting a Sound), "
	"and then choose @@Paste history@, your new script text "
	"contains exactly two lines that read \"Play\".")
NORMAL ("To edit a script that is already contained in a file on disk, use @@Open Praat script...@.")
ENTRY ("How to run a script")
NORMAL ("You can run scripts from the @ScriptEditor. If you will have to use the script very often, "
	"it is advisable to create a button for it in the fixed menu or in a dynamic menu. See the "
	"@ScriptEditor manual page.")
NORMAL ("On Unix and Windows, you can also run scripts from the command line. See @@Scripting 6.9. Calling from the command line@.")
MAN_END

MAN_BEGIN ("Scripting 2. Arguments to commands", "ppgb", 20041229)
INTRO ("This chapter describes how your script should represent "
	"commands that need %arguments, i.e. commands whose button has a title that ends in \"...\". "
	"Clicking such a button normally presents a %%settings window%, which asks the user to supply "
	"%arguments (settings) and then press the OK button. In a script, all of these arguments should be supplied on the same "
	"line as the command, in the same order as in the settings window, counted from top to bottom.")
ENTRY ("Numeric arguments")
NORMAL ("The command ##Draw line...# from the World menu in the Picture window, "
	"normally presents a settings window that asks the user to supply values for the numeric parameters %%From x%, "
	"%%From y%, %%To x%, and %%To y% (from top to bottom). To draw a line from the point (0, 0.5) "
	"to the point (1, 1), the values of the arguments would be 0, 0.5, 1, and 1, respectively. "
	"In a script, the command would read")
CODE ("Draw line... 0 0.5 1 1")
ENTRY ("Check buttons")
NORMAL ("In a script, you represent a %%check button% (yes/no choice) by \"true\" or \"false\", "
	"which may be abbreviated to \"t\" etc., or written with a capital; "
	"\"yes\" or \"no\" can also be used, as well as \"1\" or \"0\":")
CODE ("Marks left every... 1.0 100 yes yes no")
ENTRY ("Radio boxes")
NORMAL ("You represent a %%radio box% (multiple choice) by the text on the button that you want to select:")
CODE ("Print picture to PostScript printer... Finest A4 Portrait 1.0")
NORMAL ("If the choice (in a non-final argument) contains spaces, it should be enclosed within double quotes:")
CODE ("Print picture to PostScript printer... Finest \"US Letter\" Portrait 1.0")
NORMAL ("Using these double quotes is necessary because spaces are normally used for separating the arguments. "
	"Putting quotes around \"US Letter\" ensures that the second argument consists of both words. "
	"If you forget the quotes, the interpreter will think that the second argument is \"US\", "
	"the third argument is \"Letter\", and the fourth argument is the rest of the line, namely \"Portrait 1.0\", "
	"and the command will fail with the message")
CODE ("`Paper size' cannot have the value \"US\".")
ENTRY ("Text arguments")
NORMAL ("A text argument at the end should be typed without any quotes:")
CODE ("Text... 400 Centre 1.5 Bottom This is the summit")
NORMAL ("Though spaces separate the arguments, the last argument is always written without surrounding "
	"quotes, even if it contains spaces. This is possible because the interpreter knows that the ##Text...# "
	"command takes five arguments. The fifth argument is simply everything that follows the first four "
	"arguments. With this strategy, you will not suffer from %%quote paranoia%, which would occur "
	"if your text itself contains quotes:")
CODE ("Text... 0.5 Centre 0.5 Half \"hello world\"")
NORMAL ("In this case, the quotes around \"hello world\" will be written to the Picture window as well.")
NORMAL ("For these reasons, nearly all texts in settings windows appear in the last (bottom) field, "
	"so that it can be scripted without using quotes. The exceptions are those few commands that "
	"have more than a single text parameter. A text argument that is not at the end should then be enclosed "
	"in double quotes if it is empty or contains spaces (otherwise, the quotes are optional):")
CODE ("Add menu command... Objects New \"Create a Bicycle...\" \"\" 0 /u/miep/createBicycle.script")
NORMAL ("If you want to include a double quote in such a text, use %two double quotes:")
CODE ("Add menu command... Objects New \"Create a \\\"l\\\"lBicycle\\\"r\\\"r...\" (etc.)")
NORMAL ("This doubling of quotes is what you want to avoid in your scripts, especially if your "
	"texts come from the expansion of string variables.")
ENTRY ("File arguments")
NORMAL ("The commands from the Read and Write menus, and several other commands whose names "
	"start with Read, Open, or Write, present a %%file selector window% instead of a typical Praat "
	"settings window. File selector windows ask the user to supply a single argument: the file name.")
#if defined (UNIX)
	NORMAL ("In a script, you can supply the complete %path, including the directory (folder) hierarchy "
	"and the name of the file. On Unix, it goes like this (if you are user \"miep\"):")
	CODE ("Read from file... /home/miep/sounds/animals/miauw.aifc")
	NORMAL ("or just")
	CODE ("Read from file... ~/sounds/animals/miauw.aifc")
	NORMAL ("where \"~\" is the Unix way to refer to your home directory.")
#elif defined (__MACH__)
	NORMAL ("In a script, you can supply the complete %path, including the directory (folder) hierarchy "
	"and the name of the file. On MacOS X, it goes like this (if you are user \"miep\"):")
	CODE ("Read from file... /Users/miep/Sounds/Animals/miauw.aifc")
	NORMAL ("or just")
	CODE ("Read from file... ~/Sounds/Animals/miauw.aifc")
	NORMAL ("where \"~\" is the Unix way to refer to your home directory. If your file is on the desktop, the command would be:")
	CODE ("Read from file... /Users/miep/Desktop/miauw.aifc")
	NORMAL ("or just")
	CODE ("Read from file... ~/Desktop/miauw.aifc")
	NORMAL ("If your Sounds folder is on a USB drive called Miep, it would be:")
	CODE ("Read from file... /Volumes/Miep/Sounds/Animals/miauw.aifc")
#elif defined (_WIN32)
	NORMAL ("In a script, you can supply the complete %path, including the directory (folder) hierarchy "
	"and the name of the file. In Windows, it goes like this:")
	CODE ("Read from file... D:\\bsSounds\\bsAnimals\\bsmiauw.aifc")
#elif defined (macintosh)
	NORMAL ("In a script, you can supply the complete %path, including the directory (folder) hierarchy "
	"and the name of the file. On MacOS 7/8/9, it goes like this (supposing your hard drive is called \"Macintosh HD\"):")
	CODE ("Read from file... Macintosh HD:Sounds:Animals:miauw.aifc")
	NORMAL ("If your Sounds folder is on the desktop, it would be:")
	CODE ("Read from file... Macintosh HD:Desktop Folder:Sounds:Animals:miauw.aifc")
	NORMAL ("If your Sounds folder is on a USB drive called Miep, it would be:")
	CODE ("Read from file... Miep:Sounds:Animals:miauw.aifc")
#else
	#error Supply an example complete path to a sound file
#endif
NORMAL ("Instead of these complete path names, you can use %relative path names. "
	"These are taken as relative to the directory in which your script resides.")
#if defined (UNIX)
	NORMAL ("On Unix, a relative path name starts without a \"/\". So if your script is "
		"%%/home/miep/sounds/analysis.praat%, the above line could be")
	CODE ("Read from file... animals/miauw.aifc")
	NORMAL ("Finally, your script may not be in a directory %above the directory from which you "
		"like to read, but in a directory on the side, like /home/miep/scripts. The command would then read")
	CODE ("Read from file... ../animals/miauw.aifc")
#elif defined (__MACH__)
	NORMAL ("On MacOS X, a relative path name starts without a \"/\". So if your script is "
		"%%/Users/miep/Sounds/analysis.praat%, the above line could be")
	CODE ("Read from file... Animals/miauw.aifc")
	NORMAL ("Finally, your script may not be in a directory %above the directory from which you "
		"like to read, but in a directory on the side, like /Users/miep/scripts. The command would then read")
	CODE ("Read from file... ../Animals/miauw.aifc")
#elif defined (_WIN32)
	NORMAL ("In Windows, a relative path name starts without a backslash. So if your script is "
		"C:\\bsSounds\\bsAnalysis.praat, the sound file is read by")
	CODE ("Read from file... Animals\\bsmiauw.aifc")
	NORMAL ("Finally, your script may not be in a directory %above the directory from which you "
		"like to read, but in a directory on the side, like D:\\bsScripts. The commands would then read")
	CODE ("Read from file... ..\\bsAnimals\\bsmiauw.aifc")
#elif defined (macintosh)
	NORMAL ("On MacOS 7/8/9, a relative (%partial) path name starts with a colon. So if your script is "
		"Macintosh HD:Sounds:Analysis.praat, the sound file is read by")
	CODE ("Read from file... :Animals:miauw.aifc")
	NORMAL ("Finally, your script may not be in a directory %above the directory from which you "
		"like to read, but in a directory on the side, like Macintosh HD:Scripts. The command would then read")
	CODE ("Read from file... ::Animals:miauw.aifc")
#else
	#error Supply an example partial path to a sound file
#endif
ENTRY ("How to supply arguments automatically")
NORMAL ("If you dislike manually copying arguments from settings windows into your script, "
	"you can use the @@history mechanism@ to automate this process: "
	"choose @@Clear history@ from the Edit menu in your @ScriptEditor, "
	"click your command button, edit the arguments, and click OK. The command will be executed. "
	"Then choose @@Paste history@, and the command line, including the arguments (with correct quotes), "
	"appears in a the ScriptEditor. You can build a new script on the basis of this line.")
MAN_END

MAN_BEGIN ("Scripting 3. Layout", "ppgb", 20020624)
INTRO ("This chapter handles the way you use white space, comments, "
	"and continuation lines in a Praat script.")
ENTRY ("White space")
NORMAL ("All white space (spaces and tabs) at the beginning of lines is ignored. This means that you can "
	"use indenting to make your script readable. You are advised to use three spaces for each level of indenting:")
CODE ("sum = 0")
CODE ("for i to 10")
CODE ("   for j to 10")
CODE ("      sum += i\\^ j")
CODE ("   endfor")
CODE ("endfor")
CODE ("echo The sum of the products is 'sum'")
NORMAL ("Lines that are empty or consist solely of white space, are also ignored.")
ENTRY ("Comments")
NORMAL ("Comments are lines that start with !, \\# , or ;. These lines are ignored when your script is running:")
CODE ("\\#  Create 1 second of a sine wave with a frequency of 100 Hertz,")
CODE ("\\#  sampled at 22050 Hz:")
CODE ("Create Sound... sine 0 1 22050 sin (2*pi*100*x)")
ENTRY ("Continuation lines")
NORMAL ("There is normally one line per command, and one command per line.")
NORMAL ("But you can chop up long lines by using continuaton lines that start with three dots (\"...\"). "
	"You will normally want to follow this %ellipsis with a space, unless you want to concatenate "
	"the parts of a long word:")
CODE ("Viewport... 0 10 0 4")
CODE ("Text top... yes It's a long way to Llanfairpwllgwyngyll")
CODE ("   ...gogerychwyrndrobwllllantysiliogogogoch,")
CODE ("   ... unless you start from Tyddyn-y-felin.")
MAN_END

MAN_BEGIN ("Scripting 4. Object selection", "ppgb", 20040222)
INTRO ("This chapter is about how to select objects from your script, "
	"and how to find out what objects are currently selected.")
ENTRY ("Selecting objects")
NORMAL ("To simulate the mouse-clicked and dragged selection in the list of objects, "
	"you have the following commands:")
TAG ("#select %object")
DEFINITION ("selects one object, and deselects all others. If there are more "
	"objects with the same name, the most recently created one "
	"(i.e., the one nearest to the bottom of the list of objects) is selected:")
CODE ("   #select Sound hallo")
CODE ("   Play")
TAG ("#plus %object")
DEFINITION ("adds one object to the current selection.")
TAG ("#minus %object")
DEFINITION ("removes one object from the current selection.")
TAG ("##select all")
DEFINITION ("selects all objects:")
CODE ("   ##select all")
CODE ("   Remove")
NORMAL ("In the Praat shell, newly created objects are automatically selected. "
	"This is also true in scripts: ")
CODE ("! Generate a sine wave, play it, and draw its spectrum.")
CODE ("Create Sound... sine377  0 1 10000   0.9 * sin (2*pi*377*x)")
CODE ("Play")
CODE ("To Spectrum")
CODE ("! Draw the Spectrum:")
CODE ("Draw... 0 5000 20 80 yes")
CODE ("! Remove the created Spectrum and Sound:")
CODE ("#plus Sound sine377")
CODE ("Remove")
NORMAL ("Instead of by name, you can also select objects by their sequential ID:")
CODE ("   #select 43")
NORMAL ("This selects the 43rd object that you created since you started the program (see below).")
ENTRY ("Querying selected objects")
NORMAL ("You can get the name of a selected object into a string variable. "
	"For instance, the following reads the name of the second selected Sound "
	"(as counted from the top of the list of objects) into the variable %name\\$ :")
CODE1 ("name\\$  = ##selected\\$ # (\"Sound\", 2)")
NORMAL ("If the Sound was called \"Sound hallo\", the variable %name\\$  will contain the string \"hallo\". "
	"To get the name of the topmost selected Sound object, you can leave out the number:")
CODE1 ("name\\$  = ##selected\\$ # (\"Sound\")")
NORMAL ("Negative numbers count from the bottom. Thus, to get the name of the bottom-most selected Sound "
	"object, you say")
CODE1 ("name\\$  = ##selected\\$ # (\"Sound\", -1)")
NORMAL ("You would use ##selected\\$ # for drawing the object name in a picture:")
CODE ("Draw... 0 0 0 0 yes")
CODE ("name\\$  = ##selected\\$ # (\"Sound\")")
CODE ("Text top... no This is sound 'name\\$ '")
NORMAL ("For identifying previously selected objects, this method is not very suitable, since "
	"there may be multiple objects with the same name:")
CODE ("\\#  The following two lines are OK:")
CODE ("soundName\\$  = ##selected\\$ # (\"Sound\", -1)")
CODE ("pitchName\\$  = ##selected\\$ # (\"Pitch\")")
CODE ("\\#  But the following line is questionable, since it doesn't")
CODE ("\\#  necessarily select the previously selected Sound again:")
CODE ("#select Sound 'soundName\\$ '")
NORMAL ("Instead of this error-prone approach, you should get the object's unique ID. "
	"The correct version of our example becomes:")
CODE ("sound = #selected (\"Sound\", -1)")
CODE ("pitch = #selected (\"Pitch\")")
CODE ("\\#  Correct:")
CODE ("#select sound")
NORMAL ("To get the number of selected objects into a variable, use")
CODE ("numberOfSelectedSounds = numberOfSelected (\"Sound\")")
MAN_END

MAN_BEGIN ("Scripting 5. Language elements", "ppgb", 20021218)
NORMAL ("In a Praat script, you can use variables, expressions, and functions, of numeric as well as string type, "
	"and most of the control structures known from other procedural computer languages. "
	"The way the distinction between numbers and strings is made, may remind you of the programming language Basic.")
LIST_ITEM ("@@Scripting 5.1. Variables@ (numeric, string, copy, expansion)")
LIST_ITEM ("@@Scripting 5.2. Formulas@ (numeric, string)")
LIST_ITEM ("@@Scripting 5.3. Jumps@ (if, then, elsif, else, endif)")
LIST_ITEM ("@@Scripting 5.4. Loops@ (for/endfor, while/endwhile, repeat/until)")
LIST_ITEM ("@@Scripting 5.5. Procedures@ (call, procedure)")
LIST_ITEM ("@@Scripting 5.6. Arrays@")
LIST_ITEM ("@@Scripting 5.7. Including other scripts@")
LIST_ITEM ("@@Scripting 5.8. Quitting@ (exit)")
MAN_END

MAN_BEGIN ("Scripting 5.1. Variables", "ppgb", 20040414)
INTRO ("In a Praat script, you can use numeric variables as well as string variables.")
ENTRY ("Numeric variables")
NORMAL ("Numeric variables contain integer numbers between -1,000,000,000,000,000 and +1,000,000,000,000,000 "
	"or real numbers between -10^^308^ and +10^^308^. The smallest numbers lie near -10^^-308^ and +10^^-308^.")
NORMAL ("You can use %%numeric variables% in your script:")
TAG ("%variable = %formula")
DEFINITION ("evaluates a numeric formula and assign the result to a variable.")
NORMAL ("Example:")
CODE ("length = 10")
CODE ("Draw line... 0 length 1 1")
NORMAL ("Names of numeric variables must start with a lower-case letter, optionally followed by a sequence "
	"of letters, digits, and underscores.")
ENTRY ("String variables")
NORMAL ("You can also use %%string variables%, which contain text:")
CODE ("title\\$  = \"Dutch nasal place assimilation\"")
NORMAL ("As in the programming language Basic, the names of string variables end in a dollar sign.")
ENTRY ("Variable substitution")
NORMAL ("Existing variables are substituted when put between quotes:")
CODE ("x = 99")
CODE ("x2 = x * x")
CODE ("echo The square of 'x' is 'x2'.")
NORMAL ("This will write the following text to the Info window:")
CODE ("The square of 99 is 9801.")
NORMAL ("You can reduce the number of digits after the decimal point by use of the colon:")
CODE ("root = sqrt (2)")
CODE ("echo The square root of 2 is approximately 'root:3'.")
NORMAL ("This will write the following text to the Info window:")
CODE ("The square root of 2 is approximately 1.414.")
NORMAL ("By using \":0\", you round to whole values:")
CODE ("root = sqrt (2)")
CODE ("echo The square root of 2 is very approximately 'root:0'.")
NORMAL ("This will write the following text to the Info window:")
CODE ("The square root of 2 is very approximately 1.")
NORMAL ("By using \":3\\% \", you give the result in a percent format:")
CODE ("jitter = 0.0156789")
CODE ("echo The jitter is 'jitter:3\\% '.")
NORMAL ("This will write the following text to the Info window:")
CODE ("The jitter is 1.568\\% .")
NORMAL ("The number 0, however, will always be written as 0, and for small numbers the number of "
	"significant digits will never be less than 1:")
CODE ("jitter = 0.000000156789")
CODE ("echo The jitter is 'jitter:3\\% '.")
NORMAL ("This will write the following text to the Info window:")
CODE ("The jitter is 0.00002\\% .")
NORMAL ("##Predefined string variables# are $$newline\\$ $,  $$tab\\$ $, and $$shellDirectory\\$ $. "
	"The last one specifies the directory that was the default directory when Praat started up; "
	"you can use it in scripts that run from the Unix or DOS command line.")
MAN_END
/*
form Convert from WAV to AIFF
   text fileName hello.wav
endform
fileName$ = fileName$ - ".wav"
Read from file... 'shellDirectory$'/'fileName$'.wav
Write to AIFF file... 'shellDirectory$'/'fileName$'.aiff

if left$ (fileName$) <> "/"
   fileName$ = 'shellDirectory$'/'fileName$'
endif
*/

MAN_BEGIN ("Scripting 5.2. Formulas", "ppgb", 20021218)
INTRO ("In a Praat script, you can use numeric expressions as well as string expressions.")
ENTRY ("Numeric expressions")
NORMAL ("You can use a large variety of @@Formulas@ in your script:")
CODE1 ("length = 10")
CODE1 ("height = length/2")
CODE1 ("area = length * height")
NORMAL ("You can use numeric variables and formulas in arguments to commands:")
CODE1 ("Draw line... 0 length 0 length/2")
NORMAL ("Of course, all arguments except the last should either not contain spaces, "
	"or be enclosed in double quotes. So you would write either")
CODE1 ("Draw line... 0 height*2 0 height")
NORMAL ("or")
CODE1 ("Draw line... 0 \"height * 2\" 0 height")
NORMAL ("You can use numeric expressions in assignments (as above), or after "
	"#if, #elsif, #while, #until, and twice after #for.")
NORMAL ("On how to get information from commands that normally write to the Info window, "
	"see @@Scripting 6.4. Query commands@.")
MAN_END

MAN_BEGIN ("Scripting 5.3. Jumps", "ppgb", 19991112)
NORMAL ("You can use conditional jumps in your script:")
TAG ("#if %expression")
TAG ("#elsif %expression")
DEFINITION ("if the expression evaluates to zero or %false, the execution of the script jumps to the next "
	"#elsif or after the next #else or #endif at the same depth..")
NORMAL ("The following script computes the preferred length of a bed for a person 'age' years of age:")
CODE ("#if age <= 3")
CODE ("   length = 1.20")
CODE ("#elsif age <= 8")
CODE ("   length = 1.60")
CODE ("#else")
CODE ("   length = 2.00")
CODE ("#endif")
NORMAL ("A variant spelling for #elsif is #elif.")
MAN_END

MAN_BEGIN ("Scripting 5.4. Loops", "ppgb", 20040414)
ENTRY ("\"For\" loops")
TAG ("#for %variable #from %expression__1_ #to %expression__2_")
TAG ("#for %variable #to %expression")
DEFINITION ("the statements between the #for line and the matching #endfor will be executed "
	"while a variable takes on values between two expressions, with an increment of 1 "
	"on each turn of the loop. The default starting value of the loop variable is 1.")
NORMAL ("The following script plays nine sine waves, with frequencies of 200, 300, ..., 1000 Hz:")
CODE ("#for i #from 2 #to 10")
CODE ("   frequency = i * 100")
CODE ("   Create Sound... tone 0 0.3 22050 0.9*sin(2*pi*frequency*x)")
CODE ("   Play")
CODE ("   Remove")
CODE ("#endfor")
NORMAL ("The stop value of the #for loop is evaluated on each turn. If the second expression "
	"is already less than the first expression to begin with, the statements between #for and #endfor "
	"are not executed even once.")
ENTRY ("\"Repeat\" loops")
TAG ("#until %expression")
DEFINITION ("the statements between the matching preceding #repeat and the #until line "
	"will be executed again if the expression evaluates to zero or %false.")
NORMAL ("The following script measures the number of trials it takes me to throw 12 with two dice:")
CODE ("throws = 0")
CODE ("#repeat")
CODE ("   eyes = randomInteger (1, 6) + randomInteger (1, 6)")
CODE ("   throws = throws + 1")
CODE ("#until eyes = 12")
CODE ("#echo It took me 'throws' trials to throw 12 with two dice.")
NORMAL ("The statements in the #repeat/#until loop are executed at least once.")
ENTRY ("\"While\" loops")
TAG ("#while %expression")
DEFINITION ("if the expression evaluates to zero or %false, the execution of the script jumps "
	"after the matching #endwhile.")
TAG ("#endwhile")
DEFINITION ("execution jumps back to the matching preceding #while line, which is then evaluated again.")
NORMAL ("The following script forces the number %x into the range [0; 2\\pi):")
CODE ("#while x < 0")
CODE ("   x = x + 2 * pi")
CODE ("#endwhile")
CODE ("#while x >= 2 * pi")
CODE ("   x = x - 2 * pi")
CODE ("#endwhile")
NORMAL ("If the expression evaluates to zero or %false to begin with, the statements between #while and #endwhile "
	"are not executed even once.")
MAN_END

MAN_BEGIN ("Scripting 5.5. Procedures", "ppgb", 20021201)
NORMAL ("In a Praat script, you can define and call %procedures (subroutines).")
TAG ("#call %procedureName [%argument1 [%argument2 [...]]]")
DEFINITION ("the execution of the script jumps to the line after the matching #procedure line, "
	"which can be anywhere in the script.")
TAG ("#procedure %procedureName [%parameter1 [%parameter2 [...]]]")
DEFINITION ("introduces a procedure definition (when the execution of the script happens to arrive here, "
	"it jumps after the matching #endproc, i.e., the statements in the procedure are ignored).")
TAG ("#endproc")
DEFINITION ("the execution of the script jumps to the line after the #call line that invoked this procedure.")
NORMAL ("The following script plays three notes:")
CODE ("#call play_note 440")
CODE ("#call play_note 400")
CODE ("#procedure play_note frequency")
CODE ("   Create Sound... note 0 0.3 22050 0.9 * sin (2*pi*'frequency'*x)")
CODE ("   Play")
CODE ("   Remove")
CODE ("#endproc")
CODE ("#call play_note 500")
NORMAL ("The variable %frequency is a normal variable, global to the script. "
	"The procedure uses the same name space as the rest of the script, so beware of possible conflicts.")
NORMAL ("For arguments that contain spaces, you use double quotes, except for the last argument, "
	"which is the rest of the line:")
CODE ("#call Conjugate be \"I am\" \"you are\" she is")
CODE ("#procedure Conjugate verb\\$  first\\$  second\\$  third\\$ ")
CODE ("   #echo Conjugation of 'to 'verb\\$ '':")
CODE ("   #printline 1sg 'first\\$ '")
CODE ("   #printline 2sg 'second\\$ '")
CODE ("   #printline 3sg 'third\\$ '")
CODE ("#endproc")
NORMAL ("Arguments (except for the last) that contain double quotes should also be put between double quotes, and the "
	"double quotes should be doubled:")
CODE ("#procedure texts top\\$  bottom\\$ ")
CODE ("   Text top... yes 'top\\$ '")
CODE ("   Text bottom... yes 'bottom\\$ '")
CODE ("#endproc")
CODE ("#call texts \\\" \\\" \\\" hello\\\" \\\"  at the top\\\"  \\\" goodbye\\\"  at the bottom")
MAN_END

MAN_BEGIN ("Scripting 5.6. Arrays", "ppgb", 20020624)
NORMAL ("Quote substitution allows you to simulate arrays of variables:")
CODE ("#for i #from 1 #to 5")
CODE ("   square'i' = i * i")
CODE ("#endfor")
NORMAL ("After this, the variables %square1, %square2, %square3, %square4, and %square5 contain "
	"the values 1, 4, 9, 16, and 25, respectively.")
NORMAL ("You can use any number of variables in a script, but you can also use Matrix or Sound objects for arrays.")
NORMAL ("You can substitute variables with the usual single quotes, as in $$'square3'$. "
	"If the index is also a variable, however, you may need a dummy variable:")
CODE ("#echo Some squares:")
CODE ("#for i #from 1 #to 5")
CODE ("   #hop = square'i'")
CODE ("   #printline The square of 'i' is 'hop'")
CODE ("#endfor")
NORMAL ("The reason for this is that the following line would not work, because of the required "
	"double substitution:")
CODE ("#print The square of 'i' is 'square'i''")
MAN_END

MAN_BEGIN ("Scripting 5.7. Including other scripts", "ppgb", 20021206)
INTRO ("You can include other scripts within your script:")
CODE ("a = 5")
CODE ("include square.praat")
CODE ("echo 'a'")
NORMAL ("The Info window will show the result 25 if the file square.praat is as follows:")
CODE ("a = a * a")
NORMAL ("The inclusion is done before any other part of the script is considered, so you can use the #form statement "
	"and all variables in it. Usually, however, you will put some procedure definitions in the include file, that is "
	"what it seems to be most useful for. Watch out, however, for using variable names in the include file: "
	"the example above shows that there is no such thing as a separate name space.")
NORMAL ("Since including other scripts is the first thing Praat will do when considering a script, "
	"you cannot use variable substitution. For instance, the following will not work:")
CODE ("scriptName\\$  = \"myscript.praat\"")
CODE ("\\# This will *not* work:")
CODE ("include 'scriptName\\$ '")
CODE ("\\# That did *not* work!!!")
NORMAL ("You can use full or relative file names. For instance, the file square.praat is expected to be in the same "
	"directory as the script that says %%include square.praat%. If you use the ScriptEditor, you will first have to save "
	"the script that you are editing before any relative file names become meaningful (this is the same as with other "
	"uses of relative file names in scripts).")
NORMAL ("You can %nest include files, i.e., included scripts can include other scripts. However, relative file names "
	"are always evaluated relative to the directory of the outermost script.")
MAN_END

MAN_BEGIN ("Scripting 5.8. Quitting", "ppgb", 20040414)
NORMAL ("Usually, the execution of your script ends when the interpreter has executed the last line "
	"that is not within a procedure definition. However, you can also explicitly stop the script:")
TAG ("#exit")
DEFINITION ("stops the execution of the script in the normal way, i.e. without any messages to the user. "
	"Any settings window is removed from the screen.")
TAG ("#exit %%error-message%")
DEFINITION ("stops the execution of the script while sending an error message to the user. "
	"Any settings window will stay on the screen.")
NORMAL ("For an example, see @@Scripting 6.8. Messages to the user@.")
MAN_END

MAN_BEGIN ("Scripting 6. Communication outside the script", "ppgb", 20041027)
LIST_ITEM ("@@Scripting 6.1. Arguments to the script@ (form/endform, execute)")
LIST_ITEM ("@@Scripting 6.2. Calling system commands@ (system, system_nocheck)")
LIST_ITEM ("@@Scripting 6.3. Writing to the Info window@ (echo, print, printtab, printline)")
LIST_ITEM ("@@Scripting 6.4. Query commands@ (Get, Count)")
LIST_ITEM ("@@Scripting 6.5. Files@ (fileReadable, <, >, >>, filedelete, fileappend)")
LIST_ITEM ("@@Scripting 6.6. Controlling the user@ (pause)")
LIST_ITEM ("@@Scripting 6.7. Sending a message to another program@ (sendsocket)")
LIST_ITEM ("@@Scripting 6.8. Messages to the user@ (exit, assert, nowarn, nocheck)")
LIST_ITEM ("@@Scripting 6.9. Calling from the command line")
MAN_END

MAN_BEGIN ("Scripting 6.1. Arguments to the script", "ppgb", 20040414)
NORMAL ("You can cause a Praat script to prompt for arguments. The file $$playSine.praat$ may contain the following:")
CODE ("#form Play a sine wave")
CODE ("   #positive Sine_frequency_(Hz) 377")
CODE ("   #positive Gain_(0..1) 0.3 (= not too loud)")
CODE ("#endform")
CODE ("Create Sound... sine'sine_frequency'  0 1 10000   'gain' * sin (2*pi*'sine_frequency'*x)")
CODE ("Play")
CODE ("Remove")
NORMAL ("When running this script, the interpreter puts a settings window (%form) on your screen, "
	"entitled \"Play a sine wave\", "
	"with two fields, titled \"Sine frequency (Hz)\" and \"Gain\", that have been provided "
	"with the standard values \"377\" and \"0.3 (= not too loud)\", which you can change before clicking #OK.")
NORMAL ("As you see, the underscores have been replaced with spaces: that looks better in the form. "
	"Inside the script, the field names can be accessed as variables: these do contain the underscores, "
	"since they must not contain spaces, but the parentheses (Hz) have been chopped off. Note that the first "
	"letter of these variables is converted to lower case, so that you can assign to them in your script.")
NORMAL ("Inside the script, the value \"0.3 (= not too loud)\" will be known as \"0.3\", "
	"because this is a numeric field.")
NORMAL ("You can use the following field types in your forms:")
TAG ("#real %variable %initialValue")
DEFINITION ("for real numbers.")
TAG ("#positive %variable %initialValue")
DEFINITION ("for positive real numbers: the form issues an error message if the number "
	"that you enter is negative or zero; further on in the script, the number may take on any value.")
TAG ("#integer %variable %initialValue")
DEFINITION ("for whole numbers: the form reads the number as an integer; "
	"further on in the script, the number may take on any real value.")
TAG ("#natural %variable %initialValue")
DEFINITION ("for positive whole numbers: the form issues an error message if the number "
	"that you enter is negative or zero; further on in the script, the number may take on any real value.")
TAG ("#word %variable %initialValue")
DEFINITION ("for a string without spaces: the form only reads up to the first space (\"oh yes\" becomes \"oh\"); "
	"further on in the script, the string may contain spaces.")
TAG ("#sentence %variable %initialValue")
DEFINITION ("for any short string.")
TAG ("#text %variable %initialValue")
DEFINITION ("for any possibly long string (the variable name will not be shown in the form).")
TAG ("#boolean %variable %initialValue")
DEFINITION ("a check box will be shown; the value is 0 if off, 1 if on.")
TAG ("#choice %variable %initialValue")
DEFINITION ("a radio box will be shown; the value is 1 or higher. This is followed by a series of:")
TAG ("#button %text")
DEFINITION ("a button in a radio box.")
TAG ("#comment %text")
DEFINITION ("a line with any text.")
NORMAL ("Inside the script, strings are known as string variables, numbers as numeric variables:")
CODE ("#form Sink it")
CODE ("   #sentence Name_of_the_ship Titanic")
CODE ("   #real Distance_to_the_iceberg_(m) 500.0")
CODE ("   #natural Number_of_people 1800")
CODE ("   #natural Number_of_boats 10")
CODE ("#endform")
NORMAL ("In this script, the variables are known as %%name_of_the_ship\\$ %, %distance_to_the_iceberg, "
	"%number_of_people, and %number_of_boats.")
NORMAL ("The variable associated with a radio box will get a numeric as well as a string value:")
CODE ("#form Fill attributes")
CODE ("   #comment Choose any colour and texture for your paintings")
CODE ("   #choice Colour: 5")
CODE ("      #button Dark red")
CODE ("      #button Sea green")
CODE ("      #button Navy blue")
CODE ("      #button Canary yellow")
CODE ("      #button Black")
CODE ("      #button White")
CODE ("   #choice Texture: 1")
CODE ("      #button Smooth")
CODE ("      #button Rough")
CODE ("      #button With holes")
CODE ("#endform")
CODE ("#echo You chose the colour 'colour\\$ ' and texture 'texture\\$ '.")
NORMAL ("This shows two radio boxes. In the Colour box, the fifth button (Black) is the standard value here. "
	"If you click on \"Navy blue\" and then #%OK, the variable %colour will have the value \"3\", "
	"and the variable %%colour\\$ % will have the value \"Navy blue\". "
	"Note that the trailing colon is chopped off, and that the button and comment texts may contain spaces. "
	"So you can test the value of the Colour box in either of the following ways:")
CODE ("if colour = 4")
NORMAL ("or")
CODE ("if colour\\$  = \"Canary yellow\"")
NORMAL ("The field types #optionmenu and #option are completely analogous to #choice and #button, "
	"but use up much less space on the screen:")
CODE ("#form Fill attributes")
CODE ("   #comment Choose any colour and texture for your paintings")
CODE ("   #optionmenu Colour: 5")
CODE ("      #option Dark red")
CODE ("      #option Sea green")
CODE ("      #option Navy blue")
CODE ("      #option Canary yellow")
CODE ("      #option Black")
CODE ("      #option White")
CODE ("   #optionmenu Texture: 1")
CODE ("      #option Smooth")
CODE ("      #option Rough")
CODE ("      #option With holes")
CODE ("#endform")
CODE ("#echo You chose the colour 'colour\\$ ' and texture 'texture\\$ '.")
NORMAL ("You can combine two short fields into one by using %left and %right:")
CODE ("#form Get duration")
CODE ("   #natural left_Year_range 1940")
CODE ("   #natural right_Year_range 1945")
CODE ("#endform")
CODE ("duration = right_Year_range - left_Year_range")
CODE ("#echo The duration is 'duration' years.")
NORMAL ("The interpreter will only show the single text \"Year range\", followed by two small text fields.")
ENTRY ("Calling a script from another script")
NORMAL ("Scripts can be nested: the file %%doremi.praat% may contain the following:")
CODE ("#execute playSine.praat 550 0.9")
CODE ("#execute playSine.praat 615 0.9")
CODE ("#execute playSine.praat 687 0.9")
NORMAL ("With the #execute command, Praat will not display a form window, but simply execute the script "
	"with the two arguments that you supply on the same line (e.g. 550 and 0.9).")
NORMAL ("Arguments (except for the last) that contain spaces must be put between double quotes, "
	"and values for #choice must be passed as strings:")
CODE ("#execute \"fill attributes.praat\" \"Navy blue\" With holes")
NORMAL ("You can pass values for #boolean either as \"yes\" and \"no\" or 1 and 0.")
MAN_END

MAN_BEGIN ("Scripting 6.2. Calling system commands", "ppgb", 20020317)
INTRO ("You can call system commands from a Praat script on Unix, Windows, and MacOS X computers.")
TAG ("#system %command")
DEFINITION ("executes a Unix or Windows shell command, interpreting non-zero return values as errors:")
CODE1 ("#system cd /u/miep/sounds; sfplay hallo.aifc")
TAG ("#system_nocheck %command")
DEFINITION ("executes a Unix or Windows shell command, ignoring return values:")
CODE1 ("#system_nocheck rm dummy.aifc")
NORMAL ("In the last example, using $$#system rm dummy.aifc$ would cause the script to stop "
	"if the file $$dummy.aifc$ does not exist.")
TAG ("##environment\\$  (#%%symbol-string%#)")
DEFINITION ("returns the value of an environment variable under Unix, e.g.")
CODE ("   homeDirectory\\$  = ##environment\\$ # (\"HOME\")")
MAN_END

MAN_BEGIN ("Scripting 6.3. Writing to the Info window", "ppgb", 20000302)
NORMAL ("With the @Info button and several commands in the #Query menus, "
	"you write to the @@Info window@. If your program is run from batch (on Unix or Windows), "
	"the text goes to %stdout.")
NORMAL ("The following commands allow you to write to the Info window from a script only:")
TAG ("#echo %text")
DEFINITION ("clears the Info window and writes some text to it:")
CODE ("   #echo Starting simulation...")
TAG ("#clearinfo")
DEFINITION ("clears the Info window.")
TAG ("#print %text")
DEFINITION ("appends some text to the Info window, without clearing it and "
	"without going to a new line.")
TAG ("#printtab")
DEFINITION ("appends a %tab character to the Info window. This allows you to create "
	"table files that can be read by some spreadsheet programs.")
TAG ("#printline [%text]")
DEFINITION ("causes the following text in the Info window to begin at a new line. "
	"You can add text, just like with #print.")
NORMAL ("The following script builds a table with statistics about a pitch contour:")
CODE ("#clearinfo")
CODE ("#printline  Minimum   Maximum")
CODE ("Create Sound... sin 0 0.1 10000 sin(2*pi*377*x)")
CODE ("To Pitch... 0.01 75 600")
CODE ("minimum = Get minimum... 0 0 Hertz Parabolic")
CODE ("#print 'minimum'")
CODE ("#printtab")
CODE ("maximum = Get maximum... Hertz")
CODE ("#print 'maximum'")
CODE ("#printline")
NORMAL ("You could combine the last four print statements into:")
CODE ("#printline 'minimum''tab\\$ ''maximum'")
NORMAL ("or:")
CODE ("#print 'minimum''tab\\$ ''maximum''newline\\$ '")
MAN_END

MAN_BEGIN ("Scripting 6.4. Query commands", "ppgb", 19990108)
NORMAL ("If you click the \"Get mean...\" command for a Pitch object, "
	"the Info window will contain a text like \"150 Hz\" as a result. In a script, you would rather "
	"have this result in a variable instead of in the Info window. The solution is simple:")
CODE ("mean = Get mean... 0 0 Hertz Parabolic")
NORMAL ("The numeric variable \"mean\" now contains the number 150. When assigning to a numeric variable, "
	"the interpreter converts the part of the text before the first space into a number.")
NORMAL ("You can also assign to string variables:")
CODE ("mean\\$  = Get mean... 0 0 Hertz Parabolic")
NORMAL ("The string variable \"mean\\$ \" now contains the entire string \"150 Hz\".")
NORMAL ("This works for every command that would otherwise write into the Info window.")
MAN_END

MAN_BEGIN ("Scripting 6.5. Files", "ppgb", 20010821)
INTRO ("You can read from and write to text files from a Praat script.")
ENTRY ("Reading a file")
NORMAL ("You can check the availability of a file for reading with the function")
CODE ("#fileReadable (fileName\\$ )")
NORMAL ("which returns #true if the file exists and can be read, and #false otherwise.")
NORMAL ("To read the contents of an existing text file into a string variable, you use")
CODE ("text\\$  ##<# %fileName")
NORMAL ("where $$text\\$ $ is any string variable and $$%fileName$ is an unquoted string. "
	"If the file does not exist, the script terminates with an error message.")
ENTRY ("Example: reading a settings file")
NORMAL ("Suppose that the file ##height.inf# may contain an appropriate value for a numeric variable "
	"called $height, which we need to use in our script. We would like to read it with")
CODE ("height\\$  < height.inf")
CODE ("height = 'height\\$ '")
NORMAL ("However, this script will fail if the file ##height.inf# does not exist. To guard "
	"against this situation, we could check the existence of the file, and supply a default "
	"value in case the file does not exist:")
CODE ("fileName\\$  = \"height.inf\"")
CODE ("if fileReadable (fileName\\$ )")
CODE ("   height\\$  < 'fileName\\$ '")
CODE ("   height = 'height\\$ '")
CODE ("else")
CODE ("   height = 180")
CODE ("endif")
ENTRY ("Writing a file")
NORMAL ("To write the contents of an existing string into a new text file, you use")
CODE ("text\\$  ##># %fileName")
NORMAL ("where $$text\\$ $ is any string variable and $$%fileName$ is an unquoted string. "
	"If the file cannot be created, the script terminates with an error message.")
NORMAL ("To append the contents of an existing string at the end of an existing text file, you use")
CODE ("text\\$  ##>># %fileName")
NORMAL ("If the file does not yet exist, it is created first.")
NORMAL ("You can delete an existing file with")
CODE ("#filedelete %fileName")
NORMAL ("If the file does not exist, #filedelete does nothing.")
NORMAL ("The simplest way to append text to a file is by using #fileappend:")
CODE ("#fileappend out.txt Hello world!")
ENTRY ("Example: writing a table of squares")
NORMAL ("Suppose that we want to create a file with the following text:")
CODE ("The square of 1 is 1")
CODE ("The square of 2 is 4")
CODE ("The square of 3 is 9")
CODE ("...")
CODE ("The square of 100 is 10000")
NORMAL ("We can do this by collecting each line in a variable:")
CODE ("filedelete squares.txt")
CODE ("for i to 100")
CODE ("   square = i * i")
CODE ("   fileappend squares.txt The square of 'i' is 'square''newline\\$ '")
CODE ("endfor")
NORMAL ("Note that we delete the file before appending to it, "
	"in order that we do not append to an already existing file.")
NORMAL ("If you put the name of the file into a variable, make sure to surround it "
	"with double quotes when using #fileappend, since the file name may contain spaces "
	"and is not at the end of the line:")
CODE ("name\\$  = \"Hard disk:Desktop Folder:squares.text\"")
CODE ("filedelete 'name\\$ '")
CODE ("for i to 100")
CODE1 ("square = i * i")
CODE1 ("fileappend \"'name\\$ '\" The square of 'i' is 'square''newline\\$ '")
CODE ("endfor")
NORMAL ("Finally, you can append the contents of the Info window to a file with")
CODE ("#fappendinfo %fileName")
ENTRY ("Directory listings")
NORMAL ("To get the names of the files if a certain type in a certain directory, "
	"use @@Create Strings as file list...@.")
MAN_END

MAN_BEGIN ("Scripting 6.6. Controlling the user", "ppgb", 20040414)
INTRO ("You can temporarily halt a Praat script:")
TAG ("#pause %text")
DEFINITION ("suspends execution of the script, and allows the user to interrupt it. "
	"A message window will appear with the %text and the buttons Continue and Stop:")
CODE ("   #pause The next file will be beerbeet.TextGrid")
MAN_END

MAN_BEGIN ("Scripting 6.7. Sending a message to another program", "ppgb", 20021218)
NORMAL ("To send messages to running programs that use the Praat shell, "
	"use $sendpraat (see @@Scripting 8. Controlling Praat from another program@).")
NORMAL ("To send a message to another running program that listens to a socket, "
	"you can use the $sendsocket directive. This works on Unix and Windows only.")
ENTRY ("Example")
NORMAL ("Suppose we are in the Praat-shell program #Praat, which is a system for doing phonetics by computer. "
	"From this program, we can send a message to the %%non%-Praat-shell program #MovieEdit, "
	"which does know how to display a sound file:")
CODE ("Write to file... hallo.wav")
CODE ("sendsocket fonsg19.hum.uva.nl:6667 display hallo.wav")
NORMAL ("In this example, $$fonsg19.hum.uva.nl$ is the computer on which MovieEdit is running; "
	"you can specify any valid Internet address instead, as long as that computer allows you to send messages to it. "
	"If MovieEdit is running on the same computer as Praat, you can specify $localhost instead of the full Internet address.")
NORMAL ("The number 6667 is the port number on which MovieEdit is listening. Other programs will use different port numbers.")
MAN_END

MAN_BEGIN ("Scripting 6.8. Messages to the user", "ppgb", 20041027)
NORMAL ("If the user makes a mistake (e.g. types conflicting settings into your form window), "
	"you can use the #exit directive (@@Scripting 5.8. Quitting|\\SS5.7@) "
	"to stop the execution of the script with an error message:")
CODE ("form My analysis")
CODE1 ("real Starting_time_(s) 0.0")
CODE1 ("real Finishing_time_(s) 1.0")
CODE ("endform")
CODE ("if finishing_time <= starting_time")
CODE1 ("#exit The finishing time should exceed 'starting_time' seconds.")
CODE ("endif")
CODE ("\\#  Proceed with the analysis...")
NORMAL ("For things that should not normally go wrong, you can use the #assert directive:")
CODE ("power = Get power")
CODE ("assert power > 0")
NORMAL ("This is the same as:")
CODE ("if (power > 0) = undefined")
CODE1 ("exit Assertion failed in line xx (undefined): power > 0")
CODE ("elsif not (power > 0)")
CODE1 ("exit Assertion failed in line xx (false): power > 0")
CODE ("endif")
NORMAL ("You can prevent Praat from issuing warning messages:")
CODE ("nowarn Write to WAV file... hello.wav")
NORMAL ("This prevents warning messages about clipped samples, for instance.")
NORMAL ("You can also prevent Praat from showing a progress window:")
CODE ("noprogress To Pitch... 0 75 500")
NORMAL ("This prevents the progress window from popping up during lengthy operations. "
	"Use this only if you want to prevent the user from stopping the execution of the script.")
NORMAL ("Finally, you can make Praat ignore error messages:")
CODE ("nocheck Remove")
NORMAL ("This would cause the script to continue even if there is nothing to remove.")
MAN_END

MAN_BEGIN ("Scripting 6.9. Calling from the command line", "ppgb", 20040414)
INTRO ("On most computers, you can call a Praat script from the command line.")
ENTRY ("Command lines on Unix and MacOS X")
NORMAL ("On Unix or MacOS X, you call Praat scripts from the command line like this:")
CODE ("> /people/mietta/praat doit.praat 50 hallo")
NORMAL ("or")
CODE ("> /Applications/Praat.app/Contents/MacOS/Praat doit.praat 50 hallo")
NORMAL ("This opens P\\s{RAAT}, runs the script ##doit.praat# with arguments \"50\" and \"hallo\", "
	"and closes P\\s{RAAT}.")
NORMAL ("You also have the possibility of running the program interactively from the command line:")
CODE ("> /people/mietta/praat -")
NORMAL ("You can then type in any of the fixed and dynamic commands, and commands that handle object selection, "
	"like #select. This method also works in pipes:")
CODE ("> echo \"Statistics...\" | /people/mietta/praat -")
ENTRY ("Command lines on Windows")
NORMAL ("On Windows, you call Praat scripts from the command line like this:")
CODE ("e:\\bspraatcon.exe e:\\bsdoit.praat 50 hallo")
NORMAL ("Note that you use ##praatcon.exe# instead of ##praat.exe#. The script will write to the console output, and its output "
	"can be used in pipes.")
ENTRY ("How to get arguments into the script")
NORMAL ("In the above example, the script ##doit.praat# requires two arguments. In the script ##doit.praat#, "
	"you use #form and #endform to receive these arguments. See @@Scripting 6.1. Arguments to the script@. "
	"As with the #execute command, Praat will not present a form window, but simply execute the script "
	"with the arguments given on the command line. The example given in @@Scripting 6.1. Arguments to the script@ "
	"will be called in the following way:")
CODE ("> /people/mietta/praat playSine.praat 550 0.9")
NORMAL ("or")
CODE ("e:\\bspraatcon.exe playSine.praat 550 0.9")
MAN_END

MAN_BEGIN ("Scripting 7. Scripting the editors", "ppgb", 20040222)
NORMAL ("With a Praat script, you can automatize your work in the editors.")
NORMAL ("#Warning: if the purpose of your script is to get information about "
	"analyses (pitch, formants, intensity, spectrogram) from the Sound, "
	"we do %not advise to script the Sound editor window. "
	"It is much simpler, faster, and more reproducible to create the analyses "
	"with the commands of the @@dynamic menu@, then use the Query commands of the dynamic menu "
	"to extract information from the analyses. This also applies if you want to use a TextGrid "
	"to determine the times at which you want to query the analyses. "
	"See @@Scripting examples@.")
LIST_ITEM1 ("@@Scripting 7.1. Scripting an editor from a shell script@ (editor/endeditor)")
LIST_ITEM1 ("@@Scripting 7.2. Scripting an editor from within@")
MAN_END

MAN_BEGIN ("Scripting 7.1. Scripting an editor from a shell script", "ppgb", 20010606)
NORMAL ("From a Praat shell script, you can switch to an editor and back again:")
CODE ("sound\\$  = \"hallo\"")
CODE ("start = 0.3")
CODE ("finish = 0.7")
CODE ("Read from file... 'sound\\$ '.aifc")
CODE ("Edit")
CODE ("#editor Sound 'sound\\$ '")
CODE ("   Zoom... start finish")
CODE ("#endeditor")
CODE ("Play")
NORMAL ("This script reads a sound file from disk, pops up an editor for the resulting object, "
	"makes this editor zoom in on the part between 0.3 and 0.7 seconds, "
	"and returns to the Praat shell to play the entire sound.")
MAN_END

MAN_BEGIN ("Scripting 7.2. Scripting an editor from within", "ppgb", 20021204)
NORMAL ("This section will show how you can permanently extend the functionality of an editor.")
NORMAL ("As an example, consider the following problem: you want to see a graphic representation "
	"of the spectrum of the sound around the cursor position in the SoundEditor. To achieve this, "
	"follow these steps:")
LIST_ITEM ("1. Create a Sound.")
LIST_ITEM ("2. View it in a SoundEditor by clicking @Edit.")
LIST_ITEM ("3. Choose ##New editor script# from the @@File menu@ in the SoundEditor. The resulting @ScriptEditor "
	"will have a name like \"untitled script [Sound hallo]\".")
LIST_ITEM ("4. Type the following lines into the ScriptEditor:")
CODE2 ("cursor = Get cursor")
CODE2 ("Select... cursor-0.02 cursor+0.02")
CODE2 ("Extract windowed selection... slice Kaiser2 2 no")
CODE1 ("#endeditor")
CODE1 ("To Spectrum (fft)")
CODE1 ("Edit")
NORMAL ("If you choose Run from the Run menu in the ScriptEditor, a region of 40 milliseconds around the "
	"current cursor position in the SoundEditor will become selected. This piece will be copied to the list of objects, "
	"after applying a double Kaiser window (total length 80 ms). Thus, a Sound named \"slice\" will appear in the list. "
	"Subsequently, a Spectrum object also called \"slice\" will appear in the list, and a SpectrumEditor titled "
	"\"Spectrum slice\" will finally appear on your screen.")
LIST_ITEM ("5. Save the script to disk, e.g. as /us/miep/spectrum.praat. The title of the ScriptEditor will change accordingly.")
LIST_ITEM ("6. Since you will want this script to be available in all future SoundEditors, you choose ##Add to menu...# from the File menu. "
	"For the %Window, you specify \"SoundEditor\" (this is preset). For the %Menu, you may want to choose \"Spec.\" "
	"instead of the preset value (\"File\"). For the name of the %Command, you type something like \"Show spectrum at cursor\" "
	"(instead of \"Do it...\"). Then you click OK.")
NORMAL ("The command will be visible in every SoundEditor that you create from now on. "
	"To see this, close the one visible SoundEditor, select the original Sound, choose Edit again, and inspect the \"Spec.\" menu. "
	"You can now view the spectrum around the cursor just by choosing this menu command.")
NORMAL ("After you leave Praat and start it again, the command will continue to appear in the SoundEditor. "
	"If you don't like the command any longer, you can remove it with the @ButtonEditor, which you can start "
	"by choosing #Buttons from the Preferences submenu of the Control menu in the Objects window.")
ENTRY ("Improving your script")
NORMAL ("The above spectrum-viewing example has a number of disadvantages. It clutters the object list with a number "
	"of indiscriminable Sounds and Spectra called \"slice\", and the spectrum is shown up to the Nyquist frequency "
	"while we may just be interested in the lower 5000 Hz. Furthermore, the original selection in the SoundEditor is lost.")
NORMAL ("To improve the script, we open it again with ##Open editor script...# from the File menu in the SoundEditor. After every change, "
	"we can run it with Run from the Run menu again; alternatively, we could save it (with Save from the File menu) and choose our new "
	"\"Show spectrum at cursor\" button (this button will always run the version on disk, never the one viewed in a ScriptEditor).")
NORMAL ("To zoom in on the first 5000 Hz, we add the following code at the end of our script:")
CODE ("#editor Spectrum slice")
CODE1 ("Zoom... 0 5000")
NORMAL ("To get rid of the \"Sound slice\", we can add:")
CODE ("#endeditor")
CODE ("select Sound slice")
CODE ("Remove")
NORMAL ("Note that #endeditor is needed to change from the environment of a SpectrumEditor to the environment of the object & picture windows.")
NORMAL ("If you now choose the \"Show spectrum at cursor\" button for several cursor positions, you will notice that all those editors have the same name. "
	"To remedy the ambiguity of the line $$#editor Spectrum slice$, we give each slice a better name. For example, if the cursor was at "
	"635 milliseconds, the slice could be named \"635ms\". We can achieve this by changing the extraction in the following way:")
CODE ("milliseconds = round (cursor*1000)")
CODE ("Extract windowed selection... 'milliseconds'ms Kaiser2 2 no")
NORMAL ("The names of the Sound and Spectrum objects will now have more chance of being unique. Two lines will have to be edited trivially.")
NORMAL ("Finally, we will reset the selection to the original. At the top of the script, we add two lines to remember the positions of the selection markers:")
CODE ("begin = Get begin of selection")
CODE ("end = Get end of selection")
NORMAL ("At the bottom, we reset the selection:")
CODE ("#editor")
CODE1 ("Select... begin end")
NORMAL ("Note that the #editor directive if not followed by the name of an editor, returns the script to the original environment.")
NORMAL ("The complete script is:")
CODE1 ("begin = Get begin of selection")
CODE1 ("end = Get end of selection")
CODE1 ("cursor = Get cursor")
CODE1 ("Select... cursor-0.02 cursor+0.02")
CODE1 ("\\#  Create a name. E.g. \"670ms\" means at 670 milliseconds.")
CODE1 ("milliseconds = round (cursor*1000)")
CODE1 ("Extract windowed selection... 'milliseconds'ms Kaiser2 2 no")
CODE ("#endeditor")
CODE ("To Spectrum (fft)")
CODE ("Edit")
CODE ("#editor Spectrum 'milliseconds'ms")
CODE1 ("Zoom... 0 5000")
CODE ("#endeditor")
CODE ("select Sound 'milliseconds'ms")
CODE ("Remove")
CODE ("#editor")
CODE1 ("Select... begin end")
NORMAL ("This script is useful as it stands. It is good enough for safe use. For instance, if the created Sound object has the same name "
	"as an already existing Sound object, it will be the newly created Sound object that will be removed in the $Remove line, "
	"because #select always selects the most recently created object in case of ambiguity.")
MAN_END

MAN_BEGIN ("sendpraat", "ppgb", 20000927)
NORMAL ("See @@Scripting 8. Controlling Praat from another program@.")
MAN_END

MAN_BEGIN ("Scripting 8. Controlling Praat from another program", "ppgb", 20021218)
LIST_ITEM ("@@Scripting 8.1. The sendpraat subroutine")
LIST_ITEM ("@@Scripting 8.2. The sendpraat program")
LIST_ITEM ("@@Scripting 8.3. The sendpraat directive")
MAN_END

MAN_BEGIN ("Scripting 8.1. The sendpraat subroutine", "ppgb", 20030528)
INTRO ("A subroutine for sending messages to a %running P\\s{RAAT}. "
	"Also a Unix, MacOS, or DOS console program with the same purpose.")
ENTRY ("Syntax")
LIST_ITEM ("##sendpraat (void *#%display##, const char *#%program##, long #%timeOut##, char *#%text##);")
ENTRY ("Arguments")
TAG ("%display")
DEFINITION ("the display pointer if the subroutine is called from a running X program; "
	"if NULL, sendpraat will open the display by itself. On Windows and Macintosh, "
	"this argument is ignored.")
TAG ("%program")
DEFINITION ("the name of a running program that uses the Praat shell, e.g. \"Praat\" or \"ALS\". "
	"The first letter may be specified as lower or upper case; it will be converted to lower case for Unix "
	"and to upper case for Macintosh and Windows.")
TAG ("%message")
DEFINITION ("a sequence of Praat shell lines (commands and directives).")
TAG ("%timeOut (Unix and Macintosh only)")
DEFINITION ("the number of seconds that sendpraat will wait for an answer "
	"before writing an error message. A %timeOut of 0 means that "
	"the message will be sent asynchronously, i.e., that sendpraat "
	"will return immediately without issuing any error message.")
TAG ("%text")
DEFINITION ("the script text to be sent. Sendpraat may alter this text!")
ENTRY ("Example 1: killing a program")
CODE ("char message [100], *errorMessage;")
CODE ("strcpy (message, \"Quit\");")
CODE ("errorMessage = #sendpraat (NULL, \"praat\", 0, message);")
CODE ("if (errorMessage != NULL) fprintf (stderr, \"\\% s\", errorMessage);")
NORMAL ("This causes the program #Praat to quit (gracefully), because #Quit is a fixed "
	"command in the Control menu of that program. "
	"On Unix and Macintosh, sendpraat returns immediately; on Windows, you the %timeOut argument is ignored. "
	"The return value %errorMessage is a statically allocated string internal to sendpraat, "
	"and is overwritten by the next call to sendpraat.")
ENTRY ("Example 2: playing a sound file in reverse")
NORMAL ("Suppose you have a sound file whose name is in the variable $fileName, "
	"and you want the program #Praat, which can play sounds, "
	"to play this sound backwards.")
CODE ("char message [1000], *errorMessage;")
CODE ("sprintf (message, \"Read from file... \\% s\\bsnPlay reverse\\bsnRemove\", fileName);")
CODE ("errorMessage = #sendpraat (NULL, \"praat\", 0, message);")
NORMAL ("This will work because ##Play reverse# is an action command "
	"that becomes available in the dynamic menu when a Sound is selected. "
	"On Unix, sendpraat will allow #Praat at most 1000 seconds to perform this.")
ENTRY ("Example 3: executing a large script file")
NORMAL ("Sometimes, it may be unpractical to send a large script directly to #sendpraat. "
	"Fortunately, the receiving program knows the #execute directive:")
CODE ("char message [100], *errorMessage;")
CODE ("strcpy (message, \"doAll.praat 20\");")
CODE ("errorMessage = #sendpraat (NULL, \"praat\", 0, message);")
NORMAL ("This causes the program #Praat to execute the script ##doAll.praat# with an argument of \"20\".")
ENTRY ("How to download")
NORMAL ("You can download the source code of the sendpraat subroutine and program "
	"via ##www.praat.org# or from ##http://www.fon.hum.uva.nl/praat/sendpraat.html#.")
ENTRY ("See also")
NORMAL ("To start a program from the command line instead and sending it a message, "
	"you would not use #sendpraat, but instead run the program with a script file as an argument. "
	"See @@Scripting 6.9. Calling from the command line@.")
MAN_END

MAN_BEGIN ("Scripting 8.2. The sendpraat program", "ppgb", 20030528)
INTRO ("A Unix or DOS console program for sending messages to a %running P\\s{RAAT} program.")
ENTRY ("Syntax")
CODE ("#sendpraat [%timeOut] %program %message...")
NORMAL ("For the meaning of the arguments, see @@Scripting 8.1. The sendpraat subroutine|the sendpraat subroutine@.")
ENTRY ("Example 1: killing a program")
CODE ("sendpraat 0 praat Quit")
NORMAL ("Causes the program #Praat to quit (gracefully), because #Quit is a fixed command in the Control menu. "
	"On Unix, #sendpraat returns immediately; on Windows, you leave out the %timeOut argument.")
ENTRY ("Example 2: playing a sound file in reverse")
CODE ("sendpraat 1000 praat \"Read from file... hello.wav\" \"Play reverse\" \"Remove\"")
NORMAL ("This works because ##Play reverse# is an action command "
	"that becomes available in the dynamic menu of the #Praat program when a Sound is selected. "
	"On Unix, sendpraat will allow #Praat at most 1000 seconds to perform this.")
NORMAL ("Each line is a separate argument. Lines that contain spaces should be put inside double quotes.")
ENTRY ("Example 3: drawing")
CODE ("sendpraat als \"for i from 1 to 5\" \"Draw circle... 0.5 0.5 i\" \"endfor\"")
NORMAL ("This causes the program #Als to draw five concentric circles into the Picture window.")
ENTRY ("Example 4: executing a large script")
CODE ("sendpraat praat \"execute doAll.praat 20\"")
NORMAL ("This causes the program #Praat to execute the script ##doAll.praat# with an argument of \"20\".")
MAN_END

MAN_BEGIN ("Scripting 8.3. The sendpraat directive", "ppgb", 20021218)
INTRO ("Besides being a subroutine (@@Scripting 8.1. The sendpraat subroutine@) "
	"and a program (@@Scripting 8.2. The sendpraat program@), @sendpraat "
	"can also be called from within a Praat script.")
ENTRY ("Example 1: killing a program")
NORMAL ("Suppose we are in the Praat-shell program #Als, which is a browser for dictionaries, "
	"and we want to kill the Praat-shell program #Praat, which is a program for phonetics research:")
CODE ("sendpraat Praat Quit")
ENTRY ("Example 2: playing a sound")
NORMAL ("Suppose we are in the Praat-shell program #Als, which is a browser for dictionaries, "
	"and has no idea of what a %sound is. From this program, we can play a sound file "
	"by sending a message to the Praat-shell program #Praat, which does know about sounds:")
CODE ("fileName\\$  = \"hallo.wav\"")
CODE ("sendpraat Praat")
CODE ("...'newline\\$ ' Read from file... 'fileName\\$ '")
CODE ("...'newline\\$ ' Play")
CODE ("...'newline\\$ ' Remove")
NORMAL ("The first $$newline\\$ $ is superfluous, but this format seems to read nicely.")
MAN_END

MAN_BEGIN ("ScriptEditor", "ppgb", 20040414)
INTRO ("An aid to @@scripting@.")
NORMAL ("The #ScriptEditor is a text editor that allows you to edit, save, and run "
	"any @@Praat script@. You could type such a script from scratch, "
	"but it is easier to use the @@History mechanism@, which automatically records "
	"all your commands and mouse clicks, and which can paste these directly "
	"into a #ScriptEditor.")
NORMAL ("To add a script as a button to a fixed or dynamic menu, "
	"use @@Add to fixed menu...@ or @@Add to dynamic menu...@ from the @@File menu@.")
ENTRY ("Example 1")
NORMAL ("In this example, we create a fixed button that will play a 1-second sine wave with a specified frequency.")
NORMAL ("First, we create a ScriptEditor by choosing @@New Praat script@ from the @@Control menu@. "
	"Then, we choose @@Clear history@ from the Edit menu in the ScriptEditor. "
	"We then perform some actions that will create a sine wave, play it, and remove it:")
LIST_ITEM ("1. Choose ##Create Sound...# from the @@New menu@ and type the formula of a sine wave (i.e. "
	"remove the \"randomGauss\" term).")
LIST_ITEM ("2. Click #Play in the dynamic menu.")
LIST_ITEM ("3. Click the fixed #Remove button.")
NORMAL ("We then choose @@Paste history@ from the Edit menu in the ScriptEditor (or type Command-H). "
	"The text will now contain at least the following lines (delete any other lines):")
CODE ("Create Sound... sine 0 1 22050 1/2*sin(2*pi*377*x)")
CODE ("Play")
CODE ("Remove")
NORMAL ("We can run this script again by choosing #Run from the #Run menu (or typing Command-R). "
	"However, this always plays a sine with a frequency of 377 Hz, so we will add the variable \"Frequency\" "
	"to the script, which then looks like:")
CODE ("#form Play a sine wave")
CODE ("   #positive Frequency")
CODE ("#endform")
CODE ("Create Sound... sine'frequency' 0 1 22050 1/2*sin(2*pi*frequency*x)")
CODE ("Play")
CODE ("Remove")
NORMAL ("When we choose #Run, the ScriptEditor will ask us to supply a value for the \"Frequency\" variable. "
	"We can now play 1-second sine waves with any frequency. Note that the name of the temporary Sound "
	"is now \"sine356\" if %Frequency is \"356\": any occurrence of the string \"\'Frequency\'\" is replaced "
	"with the supplied argument.")
NORMAL ("It is advisable to supply a standard value for each argument in your script. "
	"If the duration should be variable, too, the final script could look like:")
CODE ("#form Play a sine wave")
CODE ("   #positive Frequency 440")
CODE ("   #positive Duration 1")
CODE ("#endform")
CODE ("Create Sound... sine'frequency' 0 'Duration' 22050 0.9*sin(2*pi*frequency*x)")
CODE ("Play")
CODE ("Remove")
NORMAL ("When you run this script, the ScriptEditor will ask you to supply values for the two variables, "
	"but the values \"440\" and \"1\" are already visible in the form window, "
	"so that you will get a sensible result if you just click #OK.")
NORMAL ("If this script is useful to you, you may want to put a button for it in the @@New menu@, "
	"in the ##Create Sound# submenu:")
LIST_ITEM ("1. Save the script to a file, with #Save from the #File menu. The file name that you supply, will "
	"be shown in the title bar of the ScriptEditor window.")
LIST_ITEM ("2. Choose @@Add to fixed menu...@ from the #%File menu. Supply #Objects for the %window, "
	"#New for the %menu, \"Play sine wave...\" for the %command, "
	"##Create Sound...# for %%after command%, and \"1\" for the depth (because it is supposed to be in a submenu); "
	"the %script argument has already been set to the file name that you supplied in step 1.")
LIST_ITEM ("3. Click #OK and ensure that the button has been added in the @@New menu@. This button will still be there "
	"after you leave the program and enter it again; to remove it from the menu, use the @ButtonEditor.")
ENTRY ("Example 2")
NORMAL ("In this example, we will create a shortcut for the usual complex pitch-analysis command.")
NORMAL ("First, we perform the required actions:")
LIST_ITEM ("1. Select a Sound object.")
LIST_ITEM ("2. Click ##To Pitch...# and set the arguments to your personal standard values.")
LIST_ITEM ("3. Click #OK. A new #Pitch object will appear.")
NORMAL ("We then paste the history into the ScriptEditor, after which this will contain at least a line like (delete all the other lines):")
CODE ("To Pitch... 0.01 150 900")
NORMAL ("You can run this script only after selecting one or more Sound objects.")
NORMAL ("If this script is useful to you, you may want to put a button for it in the dynamic menu:")
LIST_ITEM ("1. Save the script to a file, with #Save from the #File menu.")
LIST_ITEM ("2. Choose @@Add to dynamic menu...@ from the #File menu. Supply \"Sound\" for %class1 "
	"(because the button is supposed to be available only if a Sound is selected), \"0\" for %number1 "
	"(because the command is supposed to work for any number of selected Sound objects), "
	"\"To Pitch (child)\" for the %command, "
	"\"To Spectrum\" for %%after command%, and \"0\" for the depth (because it is not supposed to be in a submenu); "
	"the %script argument has already been set to the file name that you supplied in step 1.")
LIST_ITEM ("3. Click #OK and ensure that the button is clickable if you select one or more Sound objects. "
	"This button will still be available after you leave the program and enter it again; "
	"to remove it from the dynamic menus, use the @ButtonEditor.")
MAN_END

MAN_BEGIN ("undefined", "ppgb", 20040414)
INTRO ("When you give a query command for a numeric value, Praat sometimes writes the numeric value ##--undefined--# "
	"into the @@Info window@ (two hyphens at both sides of the word). This happens if the value you ask for is not defined, "
	"as in the following examples:")
LIST_ITEM ("\\bu You select a Sound with a finishing time of 1.0 seconds and ask for the minimum point in the wave form "
	"between 1.5 and 2.0 seconds (with the query command ##Get minimum...#).")
LIST_ITEM ("\\bu You ask for a pitch value in a voiceless part of the sound (select a #Pitch, "
	"then choose ##Get value at time...#).")
LIST_ITEM ("\\bu You type into the @Calculator the following formula: 10\\^ 400.")
ENTRY ("Usage in a script")
NORMAL ("In a Praat script, this value is simply represented as \"undefined\". You use it to test whether "
	"a query command returned a valid number:")
CODE ("select Pitch hallo")
CODE ("meanPitch = Get mean... 0.1 0.2 Hertz Parabolic")
CODE ("if meanPitch = undefined")
CODE ("  \\#  Take some exceptional action.")
CODE ("else")
CODE ("  \\#  Take the normal action.")
CODE ("endif")
ENTRY ("Details for hackers")
NORMAL ("In text files, this value is written as ##--undefined--#. "
	"In binary files, it is written as a big-endian IEEE positive infinity. "
	"In memory, it is the ANSI-C constant HUGE_VAL, which equals infinity on IEEE machines.")
MAN_END

MAN_BEGIN ("Scripting examples", "ppgb", 20040222)
INTRO ("Here is a number of examples of how to use scripting in the Praat program. "
	"Refer to the @scripting tutorial when necessary.")
LIST_ITEM ("@@Script for listing time\\--F0 pairs")
LIST_ITEM ("@@Script for listing time\\--F0\\--intensity")
LIST_ITEM ("@@Script for listing F0 statistics")
LIST_ITEM ("@@Script for creating a frequency sweep")
LIST_ITEM ("@@Script for onset detection")
LIST_ITEM ("@@Script for TextGrid boundary drawing")
LIST_ITEM ("@@Script for analysing pitch with a TextGrid")
MAN_END

MAN_BEGIN ("Script for listing time\\--F0 pairs", "ppgb", 20041005)
INTRO ("\"I wish to have a list of time markers in one column and F0 in the other. "
	"Those times that have no voiced data should be represented as \\\"l.\\\"r in the F0 column.\"")
CODE (  "echo Time:    Pitch:")
CODE (  "numberOfFrames = Get number of frames")
CODE (  "for iframe to numberOfFrames")
CODE1 (    "time = Get time from frame... iframe")
CODE1 (    "pitch = Get value in frame... iframe Hertz")
CODE1 (    "if pitch = undefined")
CODE2 (       "printline 'time:6' .")
CODE1 (    "else")
CODE2 (       "printline 'time:6' 'pitch:3'")
CODE1 (    "endif")
CODE (  "endfor")
NORMAL ("If you want to see this in a text file, you can copy and paste from the Info window, or save the Info window, "
	"or add a line to the script like")
CODE ("fappendinfo out.txt")
MAN_END

MAN_BEGIN ("Script for listing time\\--F0\\--intensity", "ppgb", 20040222)
INTRO ("\"I want a list of pitch and intensity values at the same times.\"")
NORMAL ("Since @@Sound: To Pitch...@ and @@Sound: To Intensity...@ do not give values at the same times, "
	"you create separate pitch and intensity contours with high time resolution, then interpolate. "
	"In the following example, you get pitch and intensity values at steps of 0.01 seconds "
	"by interpolating curves that have a time resolution of 0.001 seconds.")
CODE (  "sound = selected (\"Sound\")")
CODE (  "tmin = Get starting time")
CODE (  "tmax = Get finishing time")
CODE (  "To Pitch... 0.001 75 300")
CODE (  "Rename... pitch")
CODE (  "select sound")
CODE (  "To Intensity... 75 0.001")
CODE (  "Rename... intensity")
CODE (  "echo Here are the results:")
CODE (  "for i to (tmax-tmin)/0.01")
CODE1 (    "time = tmin + i * 0.01")
CODE1 (    "select Pitch pitch")
CODE1 (    "pitch = Get value at time... time Hertz Linear")
CODE1 (    "select Intensity intensity")
CODE1 (    "intensity = Get value at time... time Cubic")
CODE1 (    "printline 'time:2' 'pitch:3' 'intensity:3'")
CODE (  "endfor")
MAN_END

MAN_BEGIN ("Script for listing F0 statistics", "ppgb", 20041005)
INTRO ("\"I need to split the wave into 50 msec sections, and then for each of those sections "
	"get the F0 statistics. That is, for each 50 msec section of speech I want to get the average F0, "
	"min, max, and standard deviation.\"")
NORMAL ("First you create the complete pitch contour, i.e., you select the Sound and choose "
	"@@Sound: To Pitch...|To Pitch...@. You can then use the commands from the #Query menu in a loop:")
CODE ("startingTime = Get starting time")
CODE ("finishingTime = Get finishing time")
CODE ("numberOfTimeSteps = (finishingTime - startingTime) / 0.05")
CODE ("echo   tmin     tmax    mean   fmin   fmax  stdev")
CODE ("for step to numberOfTimeSteps")
CODE1 ("tmin = startingTime + (step - 1) * 0.05")
CODE1 ("tmax = tmin + 0.05")
CODE1 ("mean = Get mean... tmin tmax Hertz")
CODE1 ("minimum = Get minimum... tmin tmax Hertz Parabolic")
CODE1 ("maximum = Get maximum... tmin tmax Hertz Parabolic")
CODE1 ("stdev = Get standard deviation... tmin tmax Hertz")
CODE1 ("printline 'tmin:6' 'tmax:6' 'mean:2'")
CODE1 ("... 'minimum:2' 'maximum:2' 'stdev:2'")
CODE ("endfor")
ENTRY ("Notes")
NORMAL ("One should not cut the sound up into pieces of 50 ms and then do ##To Pitch...# on each of them, "
	"because Praat will not compute F0 values in the first or last 20 ms (or so) of each piece. "
	"This is because the analysis requires a window of 40 ms (or so) for every pitch frame. "
	"Instead, one typically does the analysis on the whole sound, then queries the resulting large Pitch object. "
	"In that way, the information loss of windowing only affects the two 20 ms edges of the whole sound.")
NORMAL ("The example writes lines to the Info window. If you want to write to a file instead, "
	"you start with something like")
CODE1 ("filedelete ~/results/out.txt")
NORMAL ("and add lines in the following way:")
CODE1 ("fileappend ~/results/out.txt 'tmin:6' 'tmax:6' 'mean:2'")
CODE1 ("... 'minimum:2' 'maximum:2' 'stdev:2''newline\\$ '")
MAN_END

MAN_BEGIN ("Script for creating a frequency sweep", "ppgb", 20041005)
INTRO ("\"I have to find a formula for a sinewave that sweeps from 1 kHz to 12 kHz in "
	"60 seconds while ramping the amplitude from 1 to 12 volts in the same amount of time.\"")
NORMAL ("The absolute amplitude in volts cannot be handled, of course, but linear crescendo is easy:")
CODE ("Create Sound... sweep 0 60 44100")
CODE ("... 0.05 * (1 + 11 * x/60) * sin (2*pi * (1000 + 11000/2 * x/60) * x)")
NORMAL ("Note the \"/2\" in this formula. Here is the derivation of the formula:")
FORMULA ("%frequency (%t) = 1000 + 11000 %t / 60")
FORMULA ("%phase (%t) = \\in %frequency (%t) %dt = 1000 %t + 11000 (%t^2/2) / 60")
FORMULA ("%signal (%t) = sin (%phase (%t))")
MAN_END

MAN_BEGIN ("Script for onset detection", "ppgb", 20041005)
INTRO ("\"Can anybody provide me with a script that detects the onset of sound (i.e. the end of silence).\"")
NORMAL ("You can create an Intensity contour and look for the first frame that is above some predefined threshold:")
CODE ("To Intensity... 100 0")
CODE ("n = Get number of frames")
CODE ("for i to n")
CODE1 ("intensity = Get value in frame... i")
CODE1 ("if intensity > 40")
CODE2 ("time = Get time from frame... i")
CODE2 ("echo Onset of sound at: 'time:3' seconds.")
CODE2 ("exit")
CODE1 ("endif")
CODE ("endfor")
NORMAL ("Since the intensity is computed with rather long windows, the result may be 0.01 or 0.02 seconds "
	"before the actual start of sound.")
MAN_END

MAN_BEGIN ("Script for TextGrid boundary drawing", "ppgb", 20040222)
INTRO ("\"I want only the dotted lines of the textgrid marked on top of another analysis (e.g. pitch, intensity or so) "
	"without the labels being shown below it.\"")
CODE ("n = Get number of intervals... 1")
CODE ("for i to n-1")
CODE1 ("t = Get end point... 1 i")
CODE1 ("One mark bottom... t no no yes")
CODE ("endfor")
MAN_END

MAN_BEGIN ("Script for analysing pitch with a TextGrid", "ppgb", 20040222)
INTRO ("\"I want the mean pitch of every interval that has a non-empty label on tier 5.\"")
CODE ("if numberOfSelected (\"Sound\") <> 1 or numberOfSelected (\"TextGrid\") <> 1")
CODE1 ("exit Please select a Sound and a TextGrid first.")
CODE ("endif")
CODE ("sound = selected (\"Sound\")")
CODE ("textgrid = selected (\"TextGrid\")")
CODE ("echo Result:")
CODE ("select sound")
CODE ("To Pitch... 0.0 75 600")
CODE ("pitch = selected (\"Pitch\")")
CODE ("select textgrid")
CODE ("n = Get number of intervals... 5")
CODE ("for i to n")
CODE1 ("tekst$ = Get label of interval... 5 i")
CODE1 ("if tekst$ <> \"\"")
CODE2 ("t1 = Get starting point... 5 i")
CODE2 ("t2 = Get end point... 5 i")
CODE2 ("select pitch")
CODE2 ("f0 = Get mean... t1 t2 Hertz")
CODE2 ("printline 't1:3' 't2:3' 'f0:0' 'tekst\\$ '")
CODE2 ("select textgrid")
CODE1 ("endif")
CODE ("endfor")
CODE ("select sound")
CODE ("plus textgrid")
MAN_END

}

/* End of file manual_Script.c */
