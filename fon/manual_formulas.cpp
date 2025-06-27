/* manual_formulas.cpp
 *
 * Copyright (C) 1992-2025 Paul Boersma
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

void manual_formulas_init (ManPages me);
void manual_formulas_init (ManPages me) {

MAN_BEGIN (U"Calculator", U"ppgb", 20210228)
INTRO (U"A window that allows you to calculate all kinds of simple or complicated mathematical and string expressions. "
	"To show the calculator, type @@Keyboard shortcuts|Command-U@ or choose the @@Calculator...@ command. "
	"The result will appear in the Info window.")
NORMAL (U"See the @Formulas tutorial for all the things that you can calculate with this command.")
MAN_END

MAN_BEGIN (U"Calculator...", U"ppgb", 20050822)
INTRO (U"A command in the @Goodies submenu of the @@Praat menu@ of the @@Objects window@. Shortcut: @@Keyboard shortcuts|Command-U@. "
	"Choosing this command brings up Praat's @calculator.")
MAN_END

MAN_BEGIN (U"undefined", U"ppgb", 20170910)
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
	CODE1 (U"# Take some exceptional action.")
CODE (U"else")
	CODE1 (U"# Take the normal action.")
CODE (U"endif")
ENTRY (U"Details for hackers")
NORMAL (U"In text files, this value is written as ##--undefined--#. "
	"In binary files, it is written as a big-endian IEEE positive infinity. "
	"In memory, it is usually a specific \"not-a-number\" (NaN), namely the result of dividing 0 by 0, "
	"although other NaNs, and also infinities, will equally be reported as --undefined--.")
MAN_END

MAN_BEGIN (U"Formulas", U"ppgb", 20170916)
INTRO (U"You can use numeric expressions and string (text) expressions in many places in Praat:")
LIST_ITEM (U"\\bu in the @calculator in Praat’s @Goodies submenu;")
LIST_ITEM (U"\\bu in the numeric fields of most settings windows;")
LIST_ITEM (U"\\bu in a @@Praat script@.")
NORMAL (U"For some types of objects (mainly Sound and Matrix), you can also apply formulas to all their contents at the same time:")
LIST_ITEM (U"\\bu when you create a Sound or a Matrix from the @@New menu@;")
LIST_ITEM (U"\\bu when you choose @@Formula...@ from the @Modify menu for a selected object.")
NORMAL (U"You can read this tutorial sequentially with the help of the “##< 1#” and “##1 >#” buttons.")
LIST_ITEM (U"1. @@Formulas 1. My first formulas|My first formulas@")
LIST_ITEM1 (U"1.1. @@Formulas 1.1. Formulas in the calculator|Formulas in the calculator@")
LIST_ITEM1 (U"1.2. @@Formulas 1.2. Numeric expressions|Numeric expressions@")
LIST_ITEM1 (U"1.3. @@Formulas 1.3. String expressions|String expressions@")
LIST_ITEM1 (U"1.4. @@Formulas 1.4. Array expressions|Array expressions@")
LIST_ITEM1 (U"1.5. @@Formulas 1.5. Formulas in settings windows|Formulas in settings windows@")
LIST_ITEM1 (U"1.6. @@Formulas 1.6. Formulas for creation|Formulas for creation@")
LIST_ITEM1 (U"1.7. @@Formulas 1.7. Formulas for modification|Formulas for modification@")
LIST_ITEM1 (U"1.8. @@Formulas 1.8. Formulas in scripts|Formulas in scripts@")
LIST_ITEM (U"2. @@Formulas 2. Representations|Representations@")
LIST_ITEM1 (U"2.1. @@Formulas 2.1. Representation of numbers|Representation of numbers@")
LIST_ITEM1 (U"2.2. @@Formulas 2.2. Representation of strings|Representation of strings@")
LIST_ITEM1 (U"2.3. @@Formulas 2.3. Representation of arrays|Representation of arrays@")
LIST_ITEM (U"3. @@Formulas 3. Operators|Operators@ (`+`, `-`, `*`, `/`, `^`)")
LIST_ITEM (U"4. @@Formulas 4. Constants|Constants@ (`pi`, `e`, `undefined`)")
LIST_ITEM (U"5. @@Formulas 5. Mathematical functions|Mathematical functions@")
LIST_ITEM (U"6. @@Formulas 6. String functions|String functions@")
LIST_ITEM (U"7. @@Formulas 7. Control structures|Control structures@ (`if then else fi`, semicolon)")
LIST_ITEM (U"8. @@Formulas 8. Attributes of objects|Attributes of objects@")
LIST_ITEM (U"9. @@Formulas 9. Data in objects|Data in objects@")
MAN_END

MAN_BEGIN (U"Formulas 1. My first formulas", U"ppgb", 20170916)
LIST_ITEM (U"1.1. @@Formulas 1.1. Formulas in the calculator|Formulas in the calculator@")
LIST_ITEM (U"1.2. @@Formulas 1.2. Numeric expressions|Numeric expressions@")
LIST_ITEM (U"1.3. @@Formulas 1.3. String expressions|String expressions@")
LIST_ITEM (U"1.4. @@Formulas 1.4. Array expressions|Array expressions@")
LIST_ITEM (U"1.5. @@Formulas 1.5. Formulas in settings windows|Formulas in settings windows@")
LIST_ITEM (U"1.6. @@Formulas 1.6. Formulas for creation|Formulas for creation@")
LIST_ITEM (U"1.7. @@Formulas 1.7. Formulas for modification|Formulas for modification@")
LIST_ITEM (U"1.8. @@Formulas 1.8. Formulas in scripts|Formulas in scripts@")
MAN_END

MAN_BEGIN (U"Formulas 1.1. Formulas in the calculator", U"ppgb", 20170916)
INTRO (U"To use the Praat @calculator, go to the @@Praat menu@ "
	"and choose @@Calculator...@ from the @Goodies submenu. Or simply type @@Keyboard shortcuts|Command-U@ anywhere in Praat.")
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
ENTRY (U"Calculating arrays")
NORMAL (U"You can perform computations on vectors (arrays of numbers). Type the formula")
CODE (U"{ 11, 13, 17 } + 0.5")
NORMAL (U"and click OK. The Info window will show the result:")
CODE (U"11.5")
CODE (U"13.5")
CODE (U"17.5")
MAN_END

MAN_BEGIN (U"Formulas 1.2. Numeric expressions", U"ppgb", 20170916)
INTRO (U"All the formulas whose outcome is a number are called numeric expressions. "
	"For the following examples, all the outcomes can be checked with the @calculator.")
ENTRY (U"Examples with numbers")
NORMAL (U"Some numeric expressions involve numbers only:")
TERM (U"##8*17")
DEFINITION (U"computes a multiplication. Outcome: 136.")
TERM (U"##2\\^ 10")
DEFINITION (U"computes the tenth power of 2. Outcome: 1024.")
TERM (U"##sqrt (2) / 2")
DEFINITION (U"computes the square root of 2, and divides the result by 2. Outcome: 0.7071067811865476.")
TERM (U"##sin (1/4 * pi)")
DEFINITION (U"computes the sine of %\\pi/4. Outcome: 0.7071067811865476 (again).")
ENTRY (U"Examples with strings")
NORMAL (U"Some numeric expressions compute numeric properties of strings:")
TERM (U"##length (\"internationalization\")")
DEFINITION (U"computes the length of the string \"internationalization\". Outcome: 20.")
TERM (U"##index (\"internationalization\", \"ation\")")
DEFINITION (U"computes the location of the first occurrence of the string \"ation\" in the string \"internationalization\". Outcome: 7, "
	"because the first letter of \"ation\" lines up with the seventh letter of \"internationalization\". "
	"If the substring does not occur, the outcome is 0.")
ENTRY (U"Examples with arrays")
NORMAL (U"Some numeric expressions compute numeric properties of numeric vectors:")
TERM (U"##size ({ 40, 70, 60, 50 })")
DEFINITION (U"computes the length of the vector { 40, 70, 60, 50 }, i.e. the number of its elements. Outcome: 4.")
TERM (U"##mean ({ 40, 70, 60, 50 })")
DEFINITION (U"computes the mean of the four numbers 40, 70, 60 and 50. Outcome: 55.")
MAN_END

MAN_BEGIN (U"Formulas 1.3. String expressions", U"ppgb", 20170916)
INTRO (U"All the formulas whose outcome is a text are called string expressions. "
	"A %string is programming jargon for a text; think of a text as a “string” of characters (letters).")
NORMAL (U"Again, the outcomes of the following examples can be checked with the @calculator.")
TERM (U"##\"see\" + \"king\"")
DEFINITION (U"concatenates two strings. Outcome: seeking.")
TERM (U"##left\\$  (\"internationalization\", 6)")
DEFINITION (U"computes the leftmost six letters of the string; the dollar sign is used for all functions whose result is a string. Outcome: intern.")
TERM (U"##mid\\$  (\"internationalization\", 6, 8)")
DEFINITION (U"computes the 8-letter substring that starts at the sixth letter of \"internationalization\". Outcome: national.")
TERM (U"##date\\$  ( )")
DEFINITION (U"computes the current date and time. Outcome at the time I am writing this: Mon Dec  2 02:23:45 2002.")
MAN_END

MAN_BEGIN (U"Formulas 1.4. Array expressions", U"ppgb", 20170916)
INTRO (U"A numeric vector expression is an expression whose value is a numeric vector.")
NORMAL (U"You can check the outcomes of the following examples with the @calculator.")
TERM (U"##{ 11, 13, 17 } + 0.5")
DEFINITION (U"adds 0.5 to each element of a vector with three elements, giving a new vector with three elements. "
	"Praat writes the outcome to the Info window as three lines, containing the numbers 11.5, 13.5 and 17.5:")
CODE (U"11.5")
CODE (U"13.5")
CODE (U"17.5")
TERM (U"@`zero#` (5)")
DEFINITION (U"creates a vector with 5 zeroes. Praat writes them on five lines:")
CODE (U"0")
CODE (U"0")
CODE (U"0")
CODE (U"0")
CODE (U"0")
TERM (U"@`repeat#` ({ 1, 5 }, 6)")
DEFINITION (U"creates a vector with 12 elements, in which the sequence { 1, 5 } is repeated 6 times. Outcome:")
CODE (U"1")
CODE (U"5")
CODE (U"1")
CODE (U"5")
CODE (U"1")
CODE (U"5")
CODE (U"1")
CODE (U"5")
CODE (U"1")
CODE (U"5")
CODE (U"1")
CODE (U"5")
MAN_END

MAN_BEGIN (U"Formulas 1.5. Formulas in settings windows", U"ppgb", 20170916)
INTRO (U"Into numeric fields in settings windows you usually simply type a number. "
	"However, you can use any numeric expression instead.")
NORMAL (U"For instance, suppose you want to create a Sound that contains exactly 10000 samples. "
	"If the sampling frequency is 44100 Hz, the duration will be 10000/44100 seconds. "
	"You can create such a Sound by choosing @@Create Sound from formula...@ from the @@New menu@, "
	"then typing")
CODE (U"10000/44100")
NORMAL (U"into the ##End time# field.")
NORMAL (U"Into text fields in settings windows, you can only type text directly; there is no way "
	"to use string expressions (except if you use scripts; see @@Formulas 1.8. Formulas in scripts@).")
NORMAL (U"Into numeric vector fields in settings windows, you can type any numeric vector (array) expression.")
MAN_END

MAN_BEGIN (U"Formulas 1.6. Formulas for creation", U"ppgb", 20110128)
INTRO (U"With some commands in the @@New menu@, you can supply a formula that Praat will apply to all elements of the new object.")
ENTRY (U"Creating a Sound from a formula")
NORMAL (U"Choose @@Create Sound from formula...@ and type the following into the #%Formula field:")
CODE (U"1/2 * sin (2 * pi * 377 * x)")
NORMAL (U"When you click #OK, a new @Sound object will appear in the list. "
	"After you click ##View & Edit# and zoom in a couple of times, you will see that the sound is a sine wave "
	"with a frequency of 377 hertz (cycles per second). This worked because the $x in the formula represents the time, "
	"i.e. the formula was applied to every sample separately, with a different value of $x for each sample.")
ENTRY (U"Creating a Matrix from a formula")
NORMAL (U"Choose @@Create simple Matrix...@ and type the following into the #%Formula field:")
CODE (U"8")
NORMAL (U"When you click OK, a new @Matrix object will appear in the list. When you click #Info, "
	"you will see that it is a matrix with 10 rows and 10 columns, and that all the 100 cells contain the value 8 "
	"(you can see this because both the minimum and the maximum are reported as being 8).")
NORMAL (U"A more interesting example is the formula")
CODE (U"row * col")
NORMAL (U"For the resulting Matrix, "
	"choose @@Matrix: Paint cells...|Paint cells...@ and click #OK. The Picture window will show a 10\\xx10 "
	"matrix whose elements are the product of the row and column numbers, i.e., they have values between "
	"1 and 100. Beside $row and $col, you can use $x for the distance along the horizontal axis and $y for the "
	"distance along the vertical axis; see the following page for examples.")
MAN_END

MAN_BEGIN (U"Formulas 1.7. Formulas for modification", U"ppgb", 20170916)
INTRO (U"Analogously to the formulas that you can use for creating new objects (see the previous page), "
	"you can use formulas for modifying existing objects. You do this with the command ##Formula...# that you "
	"can find in the @Modify menu when you select an object.")
ENTRY (U"Modifying a Sound with a formula")
NORMAL (U"Record a sound with your microphone and talk very lowly. If you don't know how to record a sound in Praat, "
	"consult the @Intro. Once the Sound object is in the list, click #Play. The result will sound very soft. "
	"Then choose ##Formula...# from the Modify menu and type")
CODE (U"self * 3")
NORMAL (U"Click OK, then click #Play again. The sound is much louder now. You have multiplied the amplitude of every sample "
	"in the sound with a factor of 3.")
ENTRY (U"Replacing the contents of a Sound with a formula")
NORMAL (U"If you don’t use `self` in your formula, the formula does not refer to the existing contents of the Sound. Hence, the formula ")
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
CODE (U"self^2")
NORMAL (U"will square all matrix elements.")
NORMAL (U"The formula first works on the first row, and in that row from the first column on; "
	"this can work recursively. The formula")
CODE (U"self + self [row, col - 1]")
NORMAL (U"integrates each row.")
ENTRY (U"Referring to the current position in the object")
NORMAL (U"You can refer to the current position in a Matrix (or Sound, etc.) by index or by %x and %y values:")
TERM (U"#`row`")
DEFINITION (U"the current row")
TERM (U"#`col`")
DEFINITION (U"the current column")
TERM (U"#`x`")
DEFINITION (U"the %x value associated with the current column:")
	LIST_ITEM2 (U"for a Sound, Spectrogram, Cochleagram, or Harmonicity: time, as in the 377-Hz sine wave example above")
	LIST_ITEM2 (U"for a Spectrum: frequency (Hz)")
	LIST_ITEM2 (U"for an Excitation: frequency (Bark)")
TERM (U"#`y`")
DEFINITION (U"the %y value associated with the current row:")
	LIST_ITEM2 (U"for a Spectrogram: frequency (Hz)")
	LIST_ITEM2 (U"for a Cochleagram: frequency (Bark)")
ENTRY (U"Referring to the contents of the object itself")
NORMAL (U"You can refer to values in the current Matrix (or Sound, etc.) by index.")
TERM (U"#`self`")
DEFINITION (U"refers to the value in the current Matrix at the current row and column, or to the value in the current Sound at the current sample.")
TERM (U"#`self` [%`column-expression`]")
DEFINITION (U"refers to the value in the current Sound (or Intensity etc.) at the current sample (or frame). "
	"The %`column-expression` is rounded to the nearest integer. "
	"If the index is out of range (less than 1 or greater than %n__%x_), the expression evaluates as 0.")
NORMAL (U"#Example. An integrator is")
CODE (U"   self [col - 1] + self * dx")
TERM (U"#`self` [%`row-expression`, %`column-expression`]")
DEFINITION (U"refers to the value in the current Matrix (or Spectrogram etc.) at the specified row and column. "
	"The expressions are rounded to the nearest integers.")
NORMAL (U"You can refer to values in the current Matrix (or Spectrogram, etc.) by %x and %y position:")
TERM (U"#`self` (%`x-expression`, %`y-expression`)")
DEFINITION (U"the expressions are linearly interpolated between the four nearest matrix points.")
NORMAL (U"You can refer to values in the current Sound (or Intensity etc.) by %x position:")
TERM (U"#`self` (%`x-expression`)")
DEFINITION (U"the expression is linearly interpolated between the two nearest samples (or frames).")
MAN_END

MAN_BEGIN (U"Formulas 1.8. Formulas in scripts", U"ppgb", 20170916)
INTRO (U"In scripts, you can assign numeric expressions to numeric variables, "
	"string expressions to string variables, and array expressions to array variables. "
	"You can also use numeric, string and array variables in expressions.")
ENTRY (U"Example: report a square")
NORMAL (U"Choose @@New Praat script@ from the @@Praat menu@. A script editor window will become visible. "
	"Type the following lines into that window:")
CODE (U"x = 99")
CODE (U"x2 = x * x")
CODE (U"writeInfoLine: “The square of ”, x, “ is ”, x2, “.”")
NORMAL (U"This is an example of a simple @@Praat script@; "
	"it assigns the results of the numeric formulas `99` and `x * x` "
	"to the numeric variables %`x` and %`x2`. Note that the formula `x * x` itself refers to the variable %`x`. "
	"To run (execute) this script, type @@Keyboard shortcuts|Command-R@ or choose #Run from the Run menu. "
	"Praat will then write the following text into the Info window:")
CODE (U"The square of 99 is 9801.")
NORMAL (U"For more information on scripts, see the @Scripting tutorial.")
ENTRY (U"Example: rename the city of Washington")
NORMAL (U"Type the following text into the script editor window:")
CODE (U"current$ = “Bush”")
CODE (U"previous$ = “Clinton”")
CODE (U"famous$ = “Lincoln”")
CODE (U"newCapital$ = current$ + mid$ (famous$, 2, 3) + right$ (previous$, 3)")
CODE (U"writeInfoLine: “The new capital will be ”, newCapital$, “.”")
NORMAL (U"This script assigns the results of four string expressions to the four string variables `current$`, "
	"`previous$`, `famous$`, and `newCapital$`. The dollar sign is the notation for a string variable or "
	"for a function whose result is a string (like `left$`). Note that the formula in the fourth line refers to three existing "
	"variables.")
NORMAL (U"To see what the new name of the capital will be, choose #Run.")
ENTRY (U"Example: report five squares")
NORMAL (U"Type the following script:")
CODE (U"x# = { 1, 2, 3, 4, 5 }")
CODE (U"x2# = x# * x#")
CODE (U"writeInfoLine: “The squares of ”, x#, “ are ”, x2#, “.”")
NORMAL (U"Praat will then write the following text into the Info window:")
CODE (U"The squares of 1 2 3 4 5 are 1 4 9 16 25.")
ENTRY (U"Example: numeric expressions in settings in scripts")
NORMAL (U"As in real settings windows, you can use numeric expressions in all numeric fields. "
	"The example of two pages back becomes:")
CODE (U"Create Sound from formula: “sine”, 1, 0, 10000 / 44100, 44100, ~ 0.9 * sin (2*pi*377*x)")
ENTRY (U"Example: string expressions in settings in scripts")
NORMAL (U"As in real settings windows, you can use string expressions in all text fields:")
CODE (U"soundName$ = “hello”")
CODE (U"Read from file: soundName$ + “.wav”")
ENTRY (U"Example: numeric expressions in creation in scripts")
NORMAL (U"Suppose you want to generate a sine wave whose frequency is held in a variable. This is the way:")
CODE (U"frequency = 377")
CODE (U"Create Sound from formula: “sine”, 1, 0.0, 1.0, 44100, ~ 0.9 * sin (2*pi*frequency*x)")
NORMAL (U"In this example, Praat will protest if `x` is a variable as well (%`x`), because that would be ambiguous "
	"with the `x` that refers to the time in the sound (see @@Formulas 1.7. Formulas for modification@).")
MAN_END

MAN_BEGIN (U"Formulas 2. Representations", U"ppgb", 20170916)
LIST_ITEM (U"2.1. @@Formulas 2.1. Representation of numbers|Representation of numbers@")
LIST_ITEM (U"2.2. @@Formulas 2.2. Representation of strings|Representation of strings@")
LIST_ITEM (U"2.3. @@Formulas 2.3. Representation of arrays|Representation of arrays@")
MAN_END

MAN_BEGIN (U"Formulas 2.1. Representation of numbers", U"ppgb", 20170916)
INTRO (U"Formulas can work with integer numbers as well as with real numbers.")
ENTRY (U"Real numbers")
NORMAL (U"You can type many real numbers by using a decimal notation, for instance 3.14159, 299792.5, or -0.000123456789. "
	"For very large or small numbers, you can use the %e-notation: 6.022\\.c10^^23^ is typed as `6.022e23` or `6.022e+23`, "
	"and -1.6021917\\.c10^^-19^ is typed as `-1.6021917e-19`. You can use also use the percent notation: 0.157 "
	"can be typed as `15.7%`.")
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

MAN_BEGIN (U"Formulas 2.2. Representation of strings", U"ppgb", 20021203)
INTRO (U"Formulas can work with strings that are put between two double quotes, "
	"as in “goodbye” or “how are you doing?”.")
NORMAL (U"If a string has to contain a double quote, "
	"you have to type it twice. For instance, if you type")
CODE (U"\"I asked: \"\"how are you doing?\"\"\"")
NORMAL (U"into the calculator, the outcome will be")
CODE (U"I asked: \"how are you doing?\"")
MAN_END

MAN_BEGIN (U"Formulas 2.3. Representation of arrays", U"ppgb", 20170916)
INTRO (U"Formulas can work with vectors that are put between opening and closing braces, "
	"as in { 11, 17, 13.5 }. If you type this into the calculator, the outcome will be")
CODE (U"11")
CODE (U"17")
CODE (U"13.5")
MAN_END

MAN_BEGIN (U"Formulas 3. Operators", U"ppgb", 20230124)
NORMAL (U"In formulas you can use the numerical and logical operators that are described on this page. "
	"The order of evaluation of the operators is the order that is most usual in programming languages. "
	"To force a different order, you use parentheses.")
NORMAL (U"The operators with the highest precedence are #negation (-) and #exponentation (\\^ ):")
CODE (U"--6 \\-> 6")
CODE (U"2^6 \\-> 64")
/*@praat
	# Tests for the manual page "Operators".

	assert --6 = 6
	assert 2^6 = 64
@*/
NORMAL (U"Sequences of negation and exponentiation are evaluated from right to left:")
CODE (U"2^-6 \\-> 0.015625")                       //@praat assert 2^-6 = 0.015625
CODE (U"-(1+1)^6 \\-> -64")                        //@praat assert -(1+1)^6 = -64
CODE (U"4^3^2 \\-> 4^9 \\-> 262144")               //@praat assert 4^3^2 = 262144
NORMAL (U"Note that changing the spacing does not change the meaning:")
CODE (U"4^3 ^ 2 \\-> 262144")                      //@praat assert 4^3 ^ 2 = 262144
NORMAL (U"To change the order of evaluation, you have to use parentheses:")
CODE (U"(4 ^ 3) ^ 2 \\-> 4096")                    //@praat assert (4 ^ 3) ^ 2 = 4096
NORMAL (U"The following construction is not allowed because of an ambiguity between a negative number "
	"and negation of a positive number:")
CODE (U"-2^6 \\-> ?")
NORMAL (U"Instead, you use any of the following:")
CODE (U"(-2)^6 \\-> 64")                           //@praat assert (-2)^6 = 64
CODE (U"-(2^6) \\-> -64")                          //@praat assert -(2^6) = -64
CODE (U"-(2)^6 \\-> -64")                          //@praat assert -(2)^6 = -64
NORMAL (U"The operators with the next highest precedence are #multiplication (*) and #division (/). They are evaluated "
	"from left to right:")
CODE (U"1/4*5 \\-> 1.25        (from left to right)")
CODE (U"1 / 4*5 \\-> 1.25      (spacing does not help)")
CODE (U"1 / (4*5) \\-> 0.05    (use parentheses to change the order)")
/*@praat
	assert 1/4*5 = 1.25
	assert 1 / 4*5 = 1.25
	assert 1 / (4*5) = 0.05
@*/
CODE (U"3 * 2 ^ 4 \\-> 48      (exponentiation before multiplication)")
CODE (U"3*2 ^ 4 \\-> 48        (this spacing does not matter and is misleading)")
CODE (U"(3 * 2) ^ 4 \\-> 1296  (use parentheses to change the order)")
/*@praat
	assert 3 * 2 ^ 4 = 48
	assert 3*2 ^ 4 = 48
	assert (3 * 2) ^ 4 = 1296
@*/
NORMAL (U"##Integer division# operators (#div and #mod) have the same precedence as * and /, "
	"and are likewise evaluated from left to right:")
CODE (U"54 div 5 \\-> 10       (division rounded down)")                             //@praat assert 54 div 5 = 10
CODE (U"54 mod 5 \\-> 4        (the remainder)")                                     //@praat assert 54 mod 5 = 4
CODE (U"54.3 div 5.1 \\-> 10   (works for real numbers as well)")                    //@praat assert 54.3 div 5.1 = 10
CODE (U"54.3 mod 5.1 \\-> 3.3  (the remainder)")                                     //@praat assert abs ((54.3 mod 5.1) - 3.3) < 1e-14
CODE (U"-54 div 5 \\-> -11     (division rounded down; negation before division)")   //@praat assert -54 div 5 = -11
CODE (U"-54 mod 5 \\-> 1       (the remainder)")                                     //@praat assert -54 mod 5 = 1
CODE (U"-(54 div 5) \\-> -10   (use parentheses to change the order)")               //@praat assert -(54 div 5) = -10
CODE (U"-(54 mod 5) \\-> -4")                                                        //@praat assert -(54 mod 5) = -4
CODE (U"3 * 18 div 5 \\-> 10   (from left to right)")                                //@praat assert 3 * 18 div 5 = 10
CODE (U"3 * (18 div 5) \\-> 9")                                                      //@praat assert 3 * (18 div 5) = 9
CODE (U"3 * 18 mod 5 \\-> 4")                                                        //@praat assert 3 * 18 mod 5 = 4
CODE (U"3 * (18 mod 5) \\-> 9")                                                      //@praat assert 3 * (18 mod 5) = 9
CODE (U"54 div 5 * 3 \\-> 30   (from left to right)")                                //@praat assert 54 div 5 * 3 = 30
CODE (U"54 div (5 * 3) \\-> 3")                                                      //@praat assert 54 div (5 * 3) = 3
CODE (U"54 mod 5 * 3 \\-> 12")                                                       //@praat assert 54 mod 5 * 3 = 12
CODE (U"54 mod (5 * 3) \\-> 9")                                                      //@praat assert 54 mod (5 * 3) = 9
NORMAL (U"The operators with the next highest precedence are #addition (+) and #subtraction (-), "
	"evaluated from left to right:")
CODE (U"3 - 8 + 7 \\-> 2       (from left to right)")                                //@praat assert 3 - 8 + 7 = 2
CODE (U"3 - (8 + 7) \\-> -12   (use parentheses to change the order)")               //@praat assert 3 - (8 + 7) = -12
CODE (U"3 + 8 * 7 \\-> 59      (multiplication before addition)")                    //@praat assert 3 + 8 * 7 = 59
CODE (U"(3 + 8) * 7 \\-> 77    (use parentheses to change the order)")               //@praat assert (3 + 8) * 7 = 77
CODE (U"3 + - (2 \\^  4) \\-> -13   (exponentiation, negation, addition)")           //@praat assert 3 + - (2 ^ 4) = -13
CODE (U"3 + 5 / 2 + 3 \\-> 8.5")                                                     //@praat assert 3 + 5 / 2 + 3 = 8.5
CODE (U"(3 + 5) / (2 + 3) \\-> 1.6")                                                 //@praat assert (3 + 5) / (2 + 3) = 1.6
NORMAL (U"The operators with the next highest precedence are the #comparison operators "
	"(`=, <>, <, >, <=, >=`). These operators always yield 0 (%false) or 1 (%true):")
CODE (U"5 + 6 = 10 \\-> 0      (equal)")                                             //@praat assert ( 5 + 6 = 10 ) = 0
CODE (U"5 + 6 = 11 \\-> 1")                                                          //@praat assert ( 5 + 6 = 11 ) = 1
CODE (U"5 + 6 <> 10 \\-> 1     (unequal)")                                           //@praat assert ( 5 + 6 <> 10 ) = 1
CODE (U"5 + 6 <> 11 \\-> 0")                                                         //@praat assert ( 5 + 6 <> 11 ) = 0
CODE (U"5 + 6 < 10 \\-> 0      (less than)")                                         //@praat assert ( 5 + 6 < 10 ) = 0
CODE (U"5 + 6 < 11 \\-> 0")                                                          //@praat assert ( 5 + 6 < 11 ) = 0
CODE (U"5 + 6 > 10 \\-> 1      (greater than)")                                      //@praat assert ( 5 + 6 > 10 ) = 1
CODE (U"5 + 6 > 11 \\-> 0")                                                          //@praat assert ( 5 + 6 > 11 ) = 0
CODE (U"5 + 6 <= 10 \\-> 0     (less than or equal)")                                //@praat assert ( 5 + 6 <= 10 ) = 0
CODE (U"5 + 6 <= 11 \\-> 1")                                                         //@praat assert ( 5 + 6 <= 11 ) = 1
CODE (U"5 + 6 >= 10 \\-> 1     (greater or equal)")                                  //@praat assert ( 5 + 6 >= 10 ) = 1
CODE (U"5 + 6 >= 11 \\-> 1")                                                         //@praat assert ( 5 + 6 >= 11 ) = 1
NORMAL (U"The comparison operators are mainly used in #if, #while, and #until conditions.")
NORMAL (U"The operators of lowest precedence are the #logical operators (#not, #and, and #or), of which #not has the "
	"highest precedence and #or the lowest:")
CODE (U"not 5 + 6 = 10 \\-> 1")                                                      //@praat assert ( not 5 + 6 = 10 ) = 1
CODE (U"x > 5 and x < 10               (is x between 5 and 10?)")
CODE (U"not x <= 5 and not x >= 10     (means the same as the previous line, except if x is undefined)")
CODE (U"not (x <= 5 or x >= 10)        (again means the same, except if x is undefined)")
/*@praat
	x = 7
	assert ( x > 5 and x < 10 ) = 1
	assert ( not x <= 5 and not x >= 10 ) = 1
	assert ( not (x <= 5 or x >= 10) ) = 1
	x = 3
	assert ( x > 5 and x < 10 ) = 0
	assert ( not x <= 5 and not x >= 10 ) = 0
	assert ( not (x <= 5 or x >= 10) ) = 0
	x = 11
	assert ( x > 5 and x < 10 ) = 0
	assert ( not x <= 5 and not x >= 10 ) = 0
	assert ( not (x <= 5 or x >= 10) ) = 0
	x = undefined
	assert ( x > 5 and x < 10 ) = 0
	assert ( not x <= 5 and not x >= 10 ) = 1
	assert ( not (x <= 5 or x >= 10) ) = 1
@*/
ENTRY (U"String comparison")
TERM (U"#`a$ = b$`")
DEFINITION (U"gives the value %true (= 1) if the strings are equal, and %false (= 0) otherwise.")
TERM (U"#`a$ <> b$`")
DEFINITION (U"gives the value %true if the strings are unequal, and %false otherwise.")
TERM (U"#`a$ < b$`")
DEFINITION (U"gives %true if the string %`a$` precedes the string %`b$` in Unicode sorting order. "
	"Thus, \"ha\" < \"hal\" and \"ha\" < \"ja\" are true, but \"ha\" < \"JA\" is false, "
	"because all capitals precede all lower-case characters in the Unicode sorting order.")
TERM (U"#`a$ > b$`")
DEFINITION (U"%true if %`a$` comes after %`b$` in Unicode sorting order. ")
TERM (U"#`a$ <= b$`")
DEFINITION (U"gives the value %true if the string %`a$` precedes the string %`b$` in Unicode sorting order, "
	"or if the strings are equal.")
TERM (U"#`a$ >= b$`")
DEFINITION (U"%true if %`a$` comes after %`b$` or the two are equal.")
ENTRY (U"String concatenation and truncation")
TERM (U"#`a$ + b$`")
DEFINITION (U"concatenates the two strings. After")
	CODE1 (U"text$ = \"hallo\" + \"dag\"")
DEFINITION (U"the variable %`text$` contains the string “hallodag”.")
TERM (U"#`a$ - b$`")
DEFINITION (U"subtracts the second string from the end of the first. After")
CODE2 (U"soundFileName$ = \"hallo.aifc\"")
CODE2 (U"textgridFileName$ = soundFileName$ - \".aifc\" + \".TextGrid\"")
DEFINITION (U"the variable %`textgridFileName$` contains the string \"hallo.TextGrid\". "
	"If the first string %`a$` does not end in the string %`b$`, the result of the subtraction is the string %`a$`.")
MAN_END

MAN_BEGIN (U"Formulas 4. Constants", U"ppgb", 20080318)
TERM (U"##pi")
DEFINITION (U"%\\pi, 3.14159265358979323846264338328")
TERM (U"##e")
DEFINITION (U"%e, 2.71828182845904523536028747135")
TERM (U"##undefined")
DEFINITION (U"a special value, see @undefined")
MAN_END

MAN_BEGIN (U"Formulas 5. Mathematical functions", U"ppgb", 20200801)
TERM (U"##abs (%x)")
DEFINITION (U"absolute value")
TERM (U"##round (%x)")
DEFINITION (U"nearest integer; round (1.5) = 2")
TERM (U"##floor (%x)")
DEFINITION (U"round down: highest integer value not greater than %x")
TERM (U"##ceiling (%x)")
DEFINITION (U"round up: lowest integer value not less than %x")
TERM (U"##sqrt (%x)")
DEFINITION (U"square root: \\Vr%x, %x \\>_ 0")
TERM (U"##min (%x, ...)")
DEFINITION (U"the minimum of a series of numbers, e.g. min (7.2, -5, 3) = -5")
TERM (U"##max (%x, ...)")
DEFINITION (U"the maximum of a series of numbers, e.g. max (7.2, -5, 3) = 7.2")
TERM (U"##imin (%x, ...)")
DEFINITION (U"the location of the minimum, e.g. imin (7.2, -5, 3) = 2")
TERM (U"##imax (%x, ...)")
DEFINITION (U"the location of the maximum, e.g. imax (7.2, -5, 3) = 1")
TERM (U"##sin (%x)")
DEFINITION (U"sine")
TERM (U"##cos (%x)")
DEFINITION (U"cosine")
TERM (U"##tan (%x)")
DEFINITION (U"tangent")
TERM (U"##arcsin (%x)")
DEFINITION (U"arcsine, -1 \\<_ %x \\<_ 1")
TERM (U"##arccos (%x)")
DEFINITION (U"arccosine, -1 \\<_ %x \\<_ 1")
TERM (U"##arctan (%x)")
DEFINITION (U"arctangent")
TERM (U"##arctan2 (%y, %x)")
DEFINITION (U"argument angle")
TERM (U"##sinc (%x)")
DEFINITION (U"sinus cardinalis: sin (%x) / %x")
TERM (U"##sincpi (%x)")
DEFINITION (U"sinc__%\\pi_: sin (%\\pi%x) / (%\\pi%x)")
TERM (U"##exp (%x)")
DEFINITION (U"exponentiation: %e^%x; same as ##e\\^ %x")
TERM (U"##ln (%x)")
DEFINITION (U"natural logarithm, base %e")
TERM (U"##log10 (%x)")
DEFINITION (U"logarithm, base 10")
TERM (U"##log2 (%x)")
DEFINITION (U"logarithm, base 2")
TERM (U"##sinh (%x)")
DEFINITION (U"hyperbolic sine: (%e^%x - %e^^-%x^) / 2")
TERM (U"##cosh (%x)")
DEFINITION (U"hyperbolic cosine: (%e^%x + %e^^-%x^) / 2")
TERM (U"##tanh (%x)")
DEFINITION (U"hyperbolic tangent: sinh (%x) / cosh (%x)")
TERM (U"##arcsinh (%x)")
DEFINITION (U"inverse hyperbolic sine: ln (%x + \\Vr(1+%x^2))")
TERM (U"##arccosh (%x)")
DEFINITION (U"inverse hyperbolic cosine: ln (%x + \\Vr(%x^2−1))")
TERM (U"##arctanh (%x)")
DEFINITION (U"inverse hyperbolic tangent")
TERM (U"##sigmoid (%x)")
DEFINITION (U"#R \\-> (0,1): 1 / (1 + %e^^−%x^) or 1 − 1 / (1 + %e^%x)")
TERM (U"##invSigmoid (%x)")
DEFINITION (U"(0,1) \\-> #R: ln (%x / (1 − %x))")
TERM (U"##erf (%x)")
DEFINITION (U"the error function: 2/\\Vr%\\pi __0_\\in^%x exp(-%t^2) %dt")
TERM (U"##erfc (%x)")
DEFINITION (U"the complement of the error function: 1 - erf (%x)")
TERM (U"##randomUniform (%min, %max)")
DEFINITION (U"a uniform random real number between %min (inclusive) and %max (exclusive)")
TERM (U"##randomInteger (%min, %max)")
DEFINITION (U"a uniform random integer number between %min and %max (inclusive)")
TERM (U"##randomGauss (%\\mu, %\\si)")
DEFINITION (U"a Gaussian random real number with mean %\\mu and standard deviation %\\si")
TERM (U"##randomPoisson (%mean)")
DEFINITION (U"a Poisson random real number")
TERM (U"##randomGamma (%%shape%, %%rate%)")
DEFINITION (U"a random number drawn from a Gamma distribution with shape parameter %\\al "
	"and rate parameter %\\be, which is defined as "
	"%f(%x; %\\al, %\\be) = (1 / \\Ga (%\\al)) %\\be%^^%\\al^ %x^^%\\al−1^ %e^^−%\\be %x^, "
	"for %x > 0, %\\al > 0 and %\\be > 0, following the method by @@Marsaglia & Tsang (2000)@")
TERM (U"##random\\_ initializeWithSeedUnsafelyButPredictably (%seed)")
DEFINITION (U"can be used in a script to create a reproducible sequence of random numbers "
	"(warning: this exceptional situation will continue to exist throughout Praat until you call the following function)")
TERM (U"##random\\_ initializeSafelyAndUnpredictably ()")
DEFINITION (U"undoes the exceptional situation caused by the previous function")
TERM (U"##lnGamma (%x)")
DEFINITION (U"logarithm of the \\Ga function")
TERM (U"##gaussP (%z)")
DEFINITION (U"the area under the Gaussian distribution between −\\oo and %z")
TERM (U"##gaussQ (%z)")
DEFINITION (U"the area under the Gaussian distribution between %z and +\\oo: "
	"the one-tailed \"statistical significance %p\" of a value that is %z standard deviations "
	"away from the mean of a Gaussian distribution")
TERM (U"##invGaussQ (%q)")
DEFINITION (U"the value of %z for which $gaussQ (%z) = %q")
TERM (U"##chiSquareP (%`chiSquare`, %`df`)")
DEFINITION (U"the area under the %\\ci^2 distribution between 0 and %chiSquare, for %`df` degrees of freedom")
TERM (U"##chiSquareQ (%`chiSquare`, %`df`)")
DEFINITION (U"the area under the %\\ci^2 distribution between %`chiSquare` and +\\oo, "
	"for %df degrees of freedom: the \"statistical significance %p\" "
	"of the %\\ci^2 difference between two distributions in %df+1 dimensions")
TERM (U"##invChiSquareQ (%q, %df)")
DEFINITION (U"the value of %\\ci^2 for which $chiSquareQ (%\\ci^2, %df) = %q")
TERM (U"##studentP (%t, %df)")
DEFINITION (U"the area under the student T-distribution from -\\oo to %t")
TERM (U"##studentQ (%t, %df)")
DEFINITION (U"the area under the student T-distribution from %t to +\\oo")
TERM (U"##invStudentQ (%q, %df)")
DEFINITION (U"the value of %t for which $studentQ (%t, %df) = %q")
TERM (U"##fisherP (%f, %df1, %df2)")
DEFINITION (U"the area under Fisher's F-distribution from 0 to %f")
TERM (U"##fisherQ (%f, %df1, %df2)")
DEFINITION (U"the area under Fisher's F-distribution from %f to +\\oo")
TERM (U"##invFisherQ (%q, %df1, %df2)")
DEFINITION (U"the value of %f for which $fisherQ (%f, %df1, %df2) = %q")
TERM (U"##binomialP (%p, %k, %n)")
DEFINITION (U"the probability that in %n experiments, an event with probability %p will occur at most %k times")
TERM (U"@`binomialQ` (%p, %k, %n)")
DEFINITION (U"the probability that in %n experiments, an event with probability %p will occur at least %k times; equals 1 - $binomialP (%p, %k - 1, %n)")
TERM (U"##invBinomialP (%P, %k, %n)")
DEFINITION (U"the value of %p for which $binomialP (%p, %k, %n) = %P")
TERM (U"@`invBinomialQ` (%Q, %k, %n)")
DEFINITION (U"the value of %p for which $binomialQ (%p, %k, %n) = %Q")
TERM (U"##hertzToBark (%x)")
DEFINITION (U"from acoustic frequency to Bark-rate (perceptual spectral frequency; place on basilar membrane): "
	"7 ln (%x/650 + \\Vr(1 + (%x/650)^2))")
TERM (U"##barkToHertz (%x)")
DEFINITION (U"650 sinh (%x / 7)")
TERM (U"##hertzToMel (%x)")
DEFINITION (U"from acoustic frequency to perceptual pitch: 550 ln (1 + %x / 550)")
TERM (U"##melToHertz (%x)")
DEFINITION (U"550 (exp (%x / 550) - 1)")
TERM (U"##hertzToSemitones (%x)")
DEFINITION (U"from acoustic frequency to a logarithmic musical scale, relative to 100 Hz: 12 ln (%x / 100) / ln 2")
TERM (U"##semitonesToHertz (%x)")
DEFINITION (U"100 exp (%x ln 2 / 12)")
TERM (U"##erb (%f)")
DEFINITION (U"the perceptual %%equivalent rectangular bandwidth% (ERB) in hertz, for a specified acoustic frequency (also in hertz): "
	"6.23\\.c10^^-6^ %f^2 + 0.09339 %f + 28.52")
TERM (U"##hertzToErb (%x)")
DEFINITION (U"from acoustic frequency to ERB-rate: 11.17 ln ((%x + 312) / (%x + 14680)) + 43")
TERM (U"##erbToHertz (%x)")
DEFINITION (U"(14680 %d - 312) / (1 - %d) where %d = exp ((%x - 43) / 11.17)")
TERM (U"@`phonToDifferenceLimens` (%x)")
DEFINITION (U"from perceptual loudness (intensity sensation) level in phon, to the number of intensity "
	"difference limens above threshold: 30 · ((61/60)^^ %x^ − 1).")
TERM (U"@`differenceLimensToPhon` (%x)")
DEFINITION (U"the inverse of the previous: ln (1 + %x / 30) / ln (61 / 60).")
TERM (U"##beta (%x, %y)")
TERM (U"##besselI (%n, %x)")
TERM (U"##besselK (%n, %x)")
NORMAL (U"For functions with arrays, see @@Scripting 5.7. Vectors and matrices@.")
MAN_END

MAN_BEGIN (U"Formulas 6. String functions", U"ppgb", 20180825)
INTRO (U"String functions are functions that either return a text string or have at least one text string as an argument. "
	"Since string computations are not very useful in the @calculator, in settings windows, or in creation and "
	"modification formulas, this page only gives examples of strings in scripts, so that the example may contain "
	"string variables.")
/*@praat
	string$ = "hallo"
	length = length (string$ + "dag")
	assert length = 8
@*/
TERM (U"#`length (a$)")
DEFINITION (U"gives the length of the string. After")
		CODE2 (U"string$ = \"hallo\"")
		CODE2 (U"length = length (string$ + \"dag\")")
DEFINITION (U"the variable %`length` contains the number 8 (by the way, from this example "
	"you see that variables can have the same names as functions, without any danger of confusing the interpreter).")
/*@praat
	head$ = left$ ("hallo", 3)
	assert head$ = "hal"
@*/
TERM (U"#`left$ (a$, n)")
DEFINITION (U"gives a string consisting of the first %n characters of %`a$`. After")
		CODE2 (U"head$ = left$ (\"hallo\", 3)")
DEFINITION (U"the variable %`head$` contains the string \"hal\".")
/*@praat
	english$ = "he" + right$ ("hallo", 3)
	assert english$ = "hello"
@*/
TERM (U"#`right$ (a$, n)")
DEFINITION (U"gives a string consisting of the last %n characters of %`a$`. After")
		CODE2 (U"english$ = \"he\" + right$ (\"hallo\", 3)")
DEFINITION (U"the variable %`english$` contains the string \"hello\".")
/*@praat
	assert mid$ ("hello", 3, 2) = "ll"
@*/
TERM (U"#`mid$ (\"hello\" , 3, 2)")
DEFINITION (U"gives a string consisting of 2 characters from “hello”, starting at the third character. Outcome: ll.")
/*@praat
	where = index ("hallo allemaal", "al")
	assert where = 2
	assert index ("hallo allemaal", "fhjgfhj") = 0
@*/
TERM (U"#`index (a$, b$)")
DEFINITION (U"gives the index of the first occurrence of the string %`b$` in the string %`a$`. After")
		CODE2 (U"where = index (\"hallo allemaal\", \"al\")")
DEFINITION (U"the variable %`where` contains the number 2, because the first “al” starts at the second character of the longer string. "
	"If the first string does not contain the second string, %`index` returns 0.")
/*@praat
	where = rindex ("hallo allemaal", "al")
	assert where = 13
	assert rindex ("hallo allemaal", "fhjgfhj") = 0
@*/
TERM (U"#`rindex (a$, b$)")
DEFINITION (U"gives the index of the last occurrence of the string %`b$` in the string %`a$`. After")
		CODE2 (U"where = rindex (\"hallo allemaal\", \"al\")")
DEFINITION (U"the variable %`where` contains the number 13, because the last “al” starts at the 13th character. "
	"If the first string does not contain the second string, %`rindex` returns 0.")
/*@praat
	where = startsWith ("internationalization", "int")
	assert where = 1
@*/
TERM (U"#`startsWith (a$, b$)")
DEFINITION (U"determines whether the string %`a$` starts with the string %`b$`. After")
		CODE2 (U"where = startsWith (\"internationalization\", \"int\")")
DEFINITION (U"the variable %`where` contains the number 1 (true).")
/*@praat
	where = endsWith ("internationalization", "nation")
	assert where = 0
@*/
TERM (U"#`endsWith (a$, b$)")
DEFINITION (U"determines whether the string %`a$` ends with the string %`b$`. After")
		CODE2 (U"where = endsWith (\"internationalization\", \"nation\")")
DEFINITION (U"the variable %where contains the number 0 (false).")
TERM (U"#`replace$ (a$, b$, c$, n)")
DEFINITION (U"gives a string that is like %`a$`, but where (at most %n) occurrences of %`b$` are replaced with the string %`c$`. After")
		CODE2 (U"s$ = replace$ (\"hello\", \"l\", \"m\", 0)")
DEFINITION (U"the variable %`s$` contains the string \"hemmo\". After")
		CODE2 (U"s$ = replace$ (\"hello\", \"l\", \"m\", 1)")
DEFINITION (U"the variable %`s$` contains the string \"hemlo\". The number %n determines the maximum number of occurrences of %`b$` "
	"that can be replaced. If %n is 0, all occurrences are replaced.")
TERM (U"#`index_regex (a$, b$)")
DEFINITION (U"determines where the string %`a$` first matches the @@regular expressions|regular expression@ %`b$`. After")
		CODE2 (U"where = index_regex (\"internationalization\", \"a.*n\")")
DEFINITION (U"the variable %`where` contains the number 7. If there is no match, the outcome is 0.")
TERM (U"#`rindex_regex (a$, b$)")
DEFINITION (U"determines where the string %`a$` last matches the @@regular expressions|regular expression@ %`b$`. After")
		CODE2 (U"where = rindex_regex (\"internationalization\", \"a.*n\")")
DEFINITION (U"the variable %`where` contains the number 16. If there is no match, the outcome is 0.")
TERM (U"#`replace_regex$ (a$, b$, c$, n)")
DEFINITION (U"gives a string that is like %`a$`, but where (at most %n) substrings that match the @@regular expressions|regular expression@ %`b$` "
	"are replaced with the expression %`c$`. After")
		CODE2 (U"s$ = replace_regex$ (\"hello\", \".\", \"&&\", 0)")
DEFINITION (U"the variable %`s$` contains the string \"hheelllloo\". If there is no match, "
	"the outcome is the original string %`a$`. After")
		CODE2 (U"s$ = replace_regex$ (\"hello\", \".\", \"&&\", 1)")
DEFINITION (U"the variable %`s$` contains the string \"hhello\". The number %n determines the maximum number of text pieces "
	"that can be replaced. If %n is 0, all matching text pieces are replaced.")
TERM (U"#`string$ (number)")
DEFINITION (U"formats a number as a string. Thus, `string$ (5e6)` "
	"becomes the string `5000000`, and `string$ (56%)` becomes the string `0.56`.")
TERM (U"#`fixed$ (number, precision)")
DEFINITION (U"formats a number as a string with %precision digits after the decimal point. Thus, `fixed$ (72.65687, 3)` "
	"becomes the string `72.657`, and `fixed$ (72.65001, 3)` becomes the string `72.650`. "
	"In these examples, we see that the result can be rounded up and that trailing zeroes are kept. "
	"At least one digit of precision is always given, e.g. `fixed$ (0.0000157, 3)` becomes the string `0.00002`. "
	"The number 0 always becomes the string `0`.")
TERM (U"#`percent$ (number, precision)")
DEFINITION (U"the same as `fixed$` (), but with a percent sign. For instance, `percent$ (0.157, 3)` becomes `15.700%`, "
	"`percent$ (0.000157, 3)` becomes `0.016%`, and `percent$ (0.000000157, 3)` becomes `0.00002%`. "
	"The number 0 always becomes the string `0`.")
/*@praat
	string$ = "5e6"
	assert 3 + number (string$) = 5000003
@*/
TERM (U"#`number (a$)")
DEFINITION (U"interprets a string as a number. After")
		CODE2 (U"string$ = \"5e6\"")
		CODE2 (U"writeInfoLine: 3 + number (string$)")
DEFINITION (U"the Info window contains the number 5000003.")
TERM (U"#`date$ ( )")
DEFINITION (U"gives the date and time in the following format:")
		CODE2 (U"Mon Jun 24 17:11:21 2002")
DEFINITION (U"To write the day of the month into the Info window, you type:")
		CODE2 (U"date$ = date$ ()")
		CODE2 (U"day$ = mid$ (date$, 9, 2)")
		CODE2 (U"writeInfoLine: \"The month day is \", day$, \".\"")
TERM (U"#`unicode$ (228)")
DEFINITION (U"gives the 228th Unicode codepoint, i.e. \"ä\".")
TERM (U"#`unicode (\"ä\")")
DEFINITION (U"gives the Unicode codepoint number of \"ä\", i.e. 228.")
TERM (U"#`extractNumber (\"Type: Sound\" + newline$ + \"Name: hello there\" + newline$ + \"Size: 44007\", \"Size:\")")
DEFINITION (U"looks for a number after the first occurrence of “Size:” in the long string. Outcome: 44007. "
	"This is useful in scripts that try to get information from long reports, as the following script that "
	"runs in the Sound editor window:")
		CODE2 (U"report$ = Editor info")
		CODE2 (U"maximumFrequency = extractNumber (report$, \"Spectrogram window length:\")")
TERM (U"#`extractWord$ (\"Type: Sound\" + newline$ + \"Name: hello there\" + newline$ + \"Size: 44007\", \"Type:\")")
DEFINITION (U"looks for a word without spaces after the first occurrence of \"Type:\" in the long string. Outcome: Sound.")
TERM (U"#`extractLine$ (\"Type: Sound\" + newline$ + \"Name: hello there\" + newline$ + \"Size: 44007\", \"Name: \")")
DEFINITION (U"looks for the rest of the line (including spaces) after the first occurrence of “Name: ” in the long string. "
	"Outcome: hello there. Note how “Name: ” includes a space, so that the “rest of the line” starts with the %h.")
TERM (U"#`backslashTrigraphsToUnicode$ (x$), unicodeToBackslashTrigraphs$ (x$)")
DEFINITION (U"converts e.g. \\bsct to \\ct or the reverse. See @@Special symbols@.")
MAN_END

MAN_BEGIN (U"Formulas 7. Control structures", U"ppgb", 20031228)
ENTRY (U"if ... then ... else ... fi")
NORMAL (U"You can use conditional expressions in all formulas. For example, ")
CODE (U"3 * if 52% * 3908 > 2000 then 5 else 6 fi")
NORMAL (U"evaluates to 15. Instead of #`fi`, you can also use #`endif`.")
NORMAL (U"Another example: you can clip the absolute amplitude of a Sound to 0.5 by supplying the following formula:")
CODE (U"if abs(self)>0.5 then if self>0 then 0.5 else -0.5 fi else self fi")
ENTRY (U"The semicolon")
NORMAL (U"The semicolon ends the evaluation of the formula. This can be convenient "
	"if you do not want to overwrite a long formula in your text field: the formula")
CODE (U"800;sqrt(2)*sin(2*pi*103*0.5)+10^(-40/20)*randomGauss(0,1)")
NORMAL (U"evaluates to 800.")
MAN_END

MAN_BEGIN (U"Formulas 8. Attributes of objects", U"ppgb", 20221202)
NORMAL (U"You can refer to several attributes of objects that are visible in the @@List of Objects@. "
	"To do so, use either the unique ID of the object, or the type and the name of the object. "
	"Thus, `object[113]` refers to the object that has the number 113 in the list, "
	"and `object[\"Sound hallo\"]` refers to an existing Sound object whose name is “hallo” "
	"(if there is more than one such object, it refers to the one that was created last).")
NORMAL (U"To refer to an attribute, you use the period (\".\"). "
	"Thus, `object[\"Sound hallo\"].nx` is the number of samples of the Sound called `hallo`, and "
	"`1/object[\"Sound hallo\"].dx` is its sampling frequency.")
ENTRY (U"Attributes in the calculator")
NORMAL (U"Record a Sound (read the @Intro if you do not know how to do that), "
	"and name it `mysound` (or anything else). An object with a name like `3. Sound mysound` "
	"will appear in the list. Then type into the @calculator the formula")
CODE (U"object[3].nx")
NORMAL (U"or")
CODE (U"object[\"Sound mysound\"].nx")
NORMAL (U"After you click OK, the Info window will show the number of samples. Since you could have got this result "
	"by simply selecting the object and choosing ##Get number of samples# from the @@Query submenu@, "
	"these attribute tricks are not very useful in the calculator. "
	"We will see that they are much more useful in creation and modification formulas and in scripts.")
ENTRY (U"List of possible attributes")
NORMAL (U"The following attributes are available:")
TERM (U"#`xmin`")
DEFINITION (U"the start of the time domain (usually 0) for a @Sound, @Pitch, @Formant, "
	"@Spectrogram, @Intensity, @Cochleagram, @PointProcess, or @Harmonicity object, in seconds; "
	"the lowest frequency (always 0) for a @Spectrum object, in hertz; "
	"the lowest frequency (usually 0) for an @Excitation object, in Bark; "
	"the left edge of the %x domain for a @Matrix object.")
TERM (U"#`xmax`")
DEFINITION (U"the end of the time domain (usually the duration, if %xmin is zero) for a Sound, Pitch, Formant, "
	"Spectrogram, Intensity, Cochleagram, PointProcess, or Harmonicity object, in seconds; "
	"the highest frequency (@@Nyquist frequency@) for a Spectrum object, e.g. 11025 hertz; "
	"the highest frequency for an Excitation object, often 25.6 Bark; "
	"the right edge of the %x domain for a Matrix object.")
TERM (U"#`ncol`")
DEFINITION (U"the number of columns in a @Matrix, @TableOfReal, or @Table object.")
TERM (U"#`nrow`")
DEFINITION (U"the number of rows in a @Matrix, @TableOfReal, or @Table object.")
TERM (U"#`col$` [%`i`]")
DEFINITION (U"the name of column %`i` in a @TableOfReal or @Table object.")
TERM (U"#`row$` [%`i`]")
DEFINITION (U"the name of row %`i` in a @TableOfReal object.")
TERM (U"#`nx`")
DEFINITION (U"the number of samples in a Sound object; "
	"the number of analysis frames in a Pitch, Formant, Spectrogram, Intensity, Cochleagram, or Harmonicity object; "
	"the number of frequency bins in a Spectrum or Excitation object; "
	"the number of divisions of the %x domain for a Matrix object (= %`ncol`).")
TERM (U"#`dx`")
DEFINITION (U"the sample period (time distance between consecutive samples) in a Sound object (the inverse of the sampling frequency), in seconds; "
	"the time step between consecutive frames in a Pitch, Formant, Spectrogram, Intensity, Cochleagram, or Harmonicity object, in seconds; "
	"the width of a frequency bin in a Spectrum object, in hertz; "
	"the width of a frequency bin in an Excitation object, in Bark; "
	"the horizontal distance between cells in a Matrix object.")
TERM (U"#`ymin`")
DEFINITION (U"the lowest frequency (usually 0) for a Spectrogram object, in hertz; "
	"the lowest frequency (usually 0) for a Cochleagram object, in Bark; "
	"the bottom of the %y domain for a Matrix object.")
TERM (U"#`ymax`")
DEFINITION (U"the highest frequency for a Spectrogram object, e.g. 5000 hertz; "
	"the highest frequency for a Cochleagram object, often 25.6 Bark; "
	"the top of the %y domain for a Matrix object.")
TERM (U"#`ny`")
DEFINITION (U"the number of frequency bands in a Spectrogram or Cochleagram object; "
	"for a Spectrum object: always 2 (first row is real part, second row is imaginary part) "
	"the number of divisions of the %y domain for a Matrix object (= %`nrow`).")
TERM (U"#`dy`")
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
	"of course, in such a case `object[\"Sound hello\"].xmax` refers to a property of the already existing sound.")
NORMAL (U"If a formula refers to an object under creation, there is a shorter way: you do not have to supply the name of the object at all, "
	"so you can simply write")
CODE (U"(x - xmin) / (xmax - xmin)")
NORMAL (U"The attributes that you can use in this implicit way are %xmin, %xmax, %ncol, %nrow, %nx, %dx, %ny, and %dy. "
	"To disambiguate in case there exists a script variable %xmin as well "
	"(Praat will complain if this is the case), you can write `Self.xmin`.")
ENTRY (U"Attributes in a modification formula")
NORMAL (U"In formulas for modifying an existing object, you refer to attributes in the same way as in creation formulas, "
	"i.e., you do not have to specify the name of the object that is being modified. The formula")
CODE (U"self * 20 ^ (- (x - xmin) / (xmax - xmin))")
NORMAL (U"causes the sound to decay exponentially in such a way that it has only 5 percent of its initial amplitude at the end. "
	"If you apply this formula to multiple Sound objects at the same time, $xmax will refer to the finishing time of each Sound separately "
	"as that Sound is modified.")
NORMAL (U"More examples of the use of attributes are on the next page.")
MAN_END

MAN_BEGIN (U"Formulas 9. Data in objects", U"ppgb", 20170614)
NORMAL (U"With square brackets, you can get the values inside some objects.")
ENTRY (U"Object contents in the calculator")
NORMAL (U"The outcomes of the following examples can be checked with the @calculator.")
TERM (U"##object [%%objectName\\$  or id%, %rowNumber, %columnNumber]")
TERM (U"$$object [\"Matrix hello\", 10, 3]")
DEFINITION (U"gives the value in the cell at the third column of the 10th row of the Matrix called %hello.")
TERM (U"$$object [5, 10, 3]")
DEFINITION (U"gives the value in the cell at the third column of the 10th row of the Matrix whose unique ID is 5 "
	"(i.e. that is labelled with the number 5 in the list of objects).")
TERM (U"$$object [\"Sound hello\", 0, 10000]")
DEFINITION (U"gives the value (in Pa) of the 10000th sample of the Sound %hello, averaged over the channels.")
TERM (U"$$object [23, 1, 10000]")
DEFINITION (U"gives the value (in Pa) of the 10000th sample of the left channel of the Sound with ID 23.")
TERM (U"$$object [23, 2, 10000]")
DEFINITION (U"gives the value (in Pa) of the 10000th sample of the right channel of the Sound with ID 23.")
TERM (U"$$object [\"TableOfReal tokens\", 5, 12]")
DEFINITION (U"gives the value in the cell at the fifth row of the 12th column of the TableOfReal called %tokens.")
TERM (U"$$object [\"TableOfReal tokens\", 5, \"F1\"]")
DEFINITION (U"gives the value in the cell at the fifth row of the column labelled %F1 of the TableOfReal %tokens.")
TERM (U"$$object [\"TableOfReal tokens\", \"\\bsct\", \"F1\"]")
DEFINITION (U"gives the value in the cell at the row labelled %%\\bsct% of column %F1 of the TableOfReal %tokens.")
TERM (U"$$object [\"Table listeners\", 3, \"m3ae\"]")
DEFINITION (U"gives the numeric value in the cell at the third row of column %m3ae of the Table %listeners.")
TERM (U"$$object [\"Table listeners\", 3, 12]")
DEFINITION (U"gives the numeric value in the cell at the third row of the 12th column of the Table %listeners.")
TERM (U"$$object\\$  [\"Table results\", 3, \"response\"]")
DEFINITION (U"gives the string value in the cell at the third row of column %response of the Table %results.")
TERM (U"$$object\\$  [\"Table results\", 3, 12]")
DEFINITION (U"gives the string value in the cell at the third row of the 12th column of the Table %results.")
TERM (U"$$object [\"PitchTier hello\", 8]")
DEFINITION (U"gives the pitch (in Hertz) of the 8th point in the PitchTier %hello.")
NORMAL (U"Cells (or samples, or points) outside the objects are considered to contain zeroes.")
ENTRY (U"Interpolation")
NORMAL (U"The values inside some objects can be interpolated.")
TERM (U"$$object (\"Sound hello\", 0.7, 0)")
DEFINITION (U"gives the value (in Pa) at a time of 0.7 seconds in the Sound %hello, by linear interpolation between "
	"the two samples that are nearest to 0.7 seconds. The channels are averaged.")
TERM (U"$$object (\"Sound hello\", 0.7, 1)")
DEFINITION (U"gives the interpolated value (in Pa) at a time of 0.7 seconds in the left channel of the Sound %hello.")
TERM (U"$$object (\"Sound hello\", 0.7, 2)")
DEFINITION (U"gives the interpolated value (in Pa) at a time of 0.7 seconds in the right channel of the Sound %hello.")
TERM (U"$$object (\"Spectrogram hallo\", 0.7, 2500)")
DEFINITION (U"gives the value at a time of 0.7 seconds and at a frequency of 2500 Hz in the Spectrogram %hallo, "
	"by linear interpolation between the four samples that are nearest to that point.")
TERM (U"$$object (\"PitchTier hullo\", 0.7)")
DEFINITION (U"gives the pitch (in Hertz) at a time of 0.7 seconds in the PitchTier %hullo.")
NORMAL (U"In the interpolation, times outside the time domain of the objects are considered to contain zeroes (this does not apply to PitchTiers and the like, "
	"which undergo @@constant extrapolation@).")
ENTRY (U"Object contents in a modification formula")
NORMAL (U"Suppose you want to do the difficult way of reversing the contents of a Sound called `hello` (the easy way is to choose #Reverse "
	"from the @Modify menu). You select this sound, then choose @@Copy...@ to duplicate it to a new Sound, which you name `hello_reverse`. "
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

}

/* End of file manual_formulas.cpp */
