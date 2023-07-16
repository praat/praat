/* manual_functions.cpp
 *
 * Copyright (C) 1992-2023 Paul Boersma
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

void manual_functions_init (ManPages me);
void manual_functions_init (ManPages me) {

MAN_PAGES_BEGIN R"~~~(
################################################################################
"Functions"
© Paul Boersma, 2022-05-27

A growing list of functions that you can use in @formulas and @scripting...

, @`abs` (%`x`) – absolute value
, @`abs#` (%`vector#`) – absolute value of each element of %`vector#`
, @`abs##` (%`matrix##`) – absolute value of each cell of %`matrix##`
, @`appendFile` (%`filePath$`, `...`) – write texts, numbers, vectors and so on
	at the end of an existing file (create such a file if it does not exist yet)
, @`appendFileLine` (%`filePath$`, `...`) – write texts, numbers, vectors and so on,
	followed by a newline, at the end of an existing file
	(create such a file if it does not exist yet)
, @`arccos` (%`x`) – inverse cosine
, @`arccos#` (%`vector#`) – inverse cosine of each element of %`vector#`
, @`arccos##` (%`matrix##`) – inverse cosine of each cell of %`matrix##`
, @`arccosh` (%`x`) – inverse hyperbolic cosine
, @`arccosh#` (%`vector#`) – inverse hyperbolic cosine of each element of %`vector#`
, @`arccosh##` (%`matrix##`) – inverse hyperbolic cosine of each cell of %`matrix##`
, @`arcsin` (%`x`) – inverse sine
, @`arcsin#` (%`vector#`) – inverse sine of each element of %`vector#`
, @`arcsin##` (%`matrix##`) – inverse sine of each cell of %`matrix##`
, @`arcsinh` (%`x`) – inverse hyperbolic sine
, @`arcsinh#` (%`vector#`) – inverse hyperbolic sine of each element of %`vector#`
, @`arcsinh##` (%`matrix##`) – inverse hyperbolic sine of each cell of %`matrix##`
, @`arctan` (%`x`) – inverse tangent
, @`arctan#` (%`vector#`) – inverse tangent of each element of %`vector#`
, @`arctan##` (%`matrix##`) – inverse tangent of each cell of %`matrix##`
, @`arctanh` (%`x`) – inverse hyperbolic tangent
, @`arctanh#` (%`vector#`) – inverse hyperbolic tangent of each element of %`vector#`
, @`arctanh##` (%`matrix##`) – inverse hyperbolic tangent of each cell of %`matrix##`
, @`backslashTrigraphsToUnicode$` (%`string$`) – convert e.g. \bsct to \ct
, @`barkToHertz` (%`x`) – from Bark-rate to acoustic frequency
, @`besselI` (%`n`, %`x`) – modified Bessel function of the first kind, %I__%n_
, @`besselK` (%`n`, %`x`) – modified Bessel function of the second kind, %K__%n_
, @`beta` (%`x`, %`y`) – \be function
, @`between_by#` (%`low`, %`high`, %`step`) – symmetrically dispersed numbers between %`low` and %`high`, in steps of %`step`
, @`between_count#` (%`low`, %`high`, %`n`) – %`n` symmetrically dispersed numbers between %`low` and %`high` (bin centres)
, @`binomialP` (%`p`, %`k`, %`n`) – probability of Bernoulli event occurring at most %`k` in %`n` times
, @`binomialQ` (%`p`, %`k`, %`n`) – probability of Bernoulli event occurring at least %`k` in %`n` times
, @`ceiling` (%x) – round up to integer
, @`ceiling#` (%`vector#`) – round up each element of %`vector#`
, @`ceiling##` (%`matrix##`) – round up each cell of %`matrix##`
, @`center` (%`v#`) – center (or centre) of gravity
, @`chiSquareP` (%`chiSquare`, %`df`) – area under the %\ci^2 curve up to %`chiSquare`
, @`chiSquareQ` (%`chiSquare`, %`df`) – area under the %\ci^2 curve after %`chiSquare`
, @`chooseFolder$` (%`title$`) – pops up a folder selection window
, @`chooseReadFile$` (%`title$`) – pops up a file selection window for opening (or appending to) an existing file
, @`chooseWriteFile$` (%`title$`, %`defaultFilename$`) – pops up a file selection window for saving to a new file
, @`columnSums#` (%`matrix##`)
, @`cos` (%`x`) – cosine
, @`cos#` (%`vector#`) – cosine of each element of %`vector#`
, @`cos##` (%`matrix##`) – cosine of each cell of %`matrix##`
, @`cosh` (%x) – hyperbolic cosine
, @`cosh#` (%`vector#`) – hyperbolic cosine of each element of %`vector#`
, @`cosh##` (%`matrix##`) – hyperbolic cosine of each cell of %`matrix##`
, @`createFolder` (%`folderPath$`) – create a new folder, or do nothing if it already exists
, @`date$` ( ) – current local date and time in the form "Mon Nov  8 16:32:42 2021"
, @`date#` ( ) – current local date and time in the form { 2021, 11, 8, 16, 32, 42 }
, @`date_utc$` ( ) – current standard date and time in the form "Mon Nov  8 15:32:42 2021"
, @`date_utc#` ( ) – current standard date and time in the form { 2021, 11, 8, 15, 32, 42 }
, @`deleteFile` (%`filePath$`) – delete a file, or do nothing if it does not exist
, @`differenceLimensToPhon` (%`x`) – from jnd-scale to perceptual loudness
, @`endsWith` (%`string$`, %`part$`) – determine whether %`string$` ends in %`part$`
, @`erb` (%`f`) – equivalent rectangular bandwidth for frequency %`f`
, @`erbToHertz` (%`x`) – from ERB-rate to acoustic frequency
, @`erf` (%`x`) – error function, the integral of the Gaussian
, @`erfc` (%`x`) – complement of the error function, i.e. 1 − erf (%`x`);
	this is a separate function because erf (%`x`) can be close to 1
, @`exp` (%`x`) – exponentiation, i.e. %e^%`x`
, @`exp#` (%`vector#`) – exponentiate each element of %`vector#`
, @`exp##` (%`matrix##`) – exponentiate each cell of %`matrix##`
, @`extractLine$` (%`string$`, %`part$`) – everything that comes after the first occurrence of %`part$` in %`string$`
, @`extractNumber` (%`string$`, %`part$`) – the first number after the first occurrence of %`part$% in %`string$`
, @`extractWord$` (%`string$`, %`part$`) – the first “word” after the first occurrence of %`part$% in %`string$`
, @`fileNames$#` (%`folderNameOrPattern$`) – get the names (not the whole paths) of the files in a folder
	or that match a pattern with an asterisk
, @`fileReadable` (%`filePath$`) – 1 if the file exists and can be read, 0 otherwise
, @`fisherP` (%`f`, %`df1`, %`df2`) – area under the Fisher %F curve up to %`f`
, @`fisherQ` (%`f`, %`df1`, %`df2`) – area under the Fisher %F curve after %`f`
, @`fixed$` (%`number`, %`precision`) – format a number as a string, with %`precision` digits after the decimal point
, @`floor` (%`x`) – round down to integer
, @`floor#` (%`vector#`) – round down each element of %`vector#`
, @`floor##` (%`matrix##`) – round down each cell of %`matrix##`
, @`folderNames$#` (%`folderNameOrPattern$`) – get the names (not the whole paths) of the subfolders in a folder
	or that match a pattern with an asterisk
, @`from_to#` (%`m`, %`n`) – the integers from %`m` through %`n`
, @`from_to_by#` (%`m`, %`n`, %`step`) – numbers from %`m` through %`n`, in steps of %`step`
, @`from_to_count#` (%`start`, %`end`, %`n`) – %`n` numbers from %`start` through %`end`
, @`gaussP` (%`z`) – area under the normal curve up to %`z`
, @`gaussQ` (%`z`) – area under the normal curve after %`z`
, @`hertzToBark` (%`x`) – from acoustic frequency to Bark-rate
, @`hertzToErb` (%`x`) – from acoustic frequency to ERB-rate
, @`hertzToMel` (%`x`) – from acoustic frequency to mel
, @`hertzToSemitones` (%`x`) – from acoustic frequency to logarithmic scale %re 100 Hz
, @`imax` (%`x`, `...`) – location of the maximum
, @`imin` (%`x`, `...`) – location of the minimum
, @`index` (%`string$`, %`part$`) – look up a substring, or 0 if not found
, @`index` (%`strings$#`, %`s$`) – look up whether and where %`s$` first occurs in %`strings$`
, @`index_regex` (%`string$`, %`regex$`) – determine whether and where %`string$` first matches %`regex$`
, @`inner` (%`a#`, %`b#`) – inner product
, @`invBinomialP` (%`P`, %`k`, %`n`) – inverse of `binomialP`
, @`invBinomialQ` (%`Q`, %`k`, %`n`) – inverse of `binomialQ`
, @`invChiSquareQ` (%`q`, %`df`) – inverse of `chiSquareQ`
, @`invFisherQ` (%`q`, %`df1`, %`df2`) – inverse of `fisherQ`
, @`invGaussQ` (%`z`) – inverse of `gaussQ`
, @`invSigmoid` (%`x`) – inverse `sigmoid`
, @`invSigmoid#` (%`vector#`) – inverse sigmoid of each element of %`vector#`
, @`invSigmoid##` (%`matrix##`) – inverse sigmoid of each cell of %`matrix##`
, @`invStudentQ` (%`q`, %`df`) – inverse studentQ
, @`left$` (%`string$`, %`n`) – the %n first characters in %`string$`
, @`length` (%`string$`) – number of Unicode characters in %`string$`
, @`ln` (%`x`) – natural logarithm, base %e
, @`ln#` (%`vector#`) – natural logarithm of each element of %`vector#`
, @`ln##` (%`matrix##`) – natural logarithm of each cell of %`matrix##`
, @`lnGamma` (%`x`) – logarithm of the \Ga function
, @`log2` (%`x`) – logarithm, base 2
, @`log2#` (%`vector#`) – base-2 logarithm of each element of %`vector#`
, @`log2##` (%`matrix##`) – base-2 logarithm of each cell of %`matrix##`
, @`log10` (%x) – logarithm, base 10
, @`log10#` (%`vector#`) – base-10 logarithm of each element of %`vector#`
, @`log10##` (%`matrix##`) – base-10 logarithm of each cell of %`matrix##`
, @`max` (%`x`, `...`) – maximum
, @`mean` (%`v#`) – average of the elements of a vector
, @`melToHertz` (%`x`) – from mel to acoustic frequency
, @`mid$` (%`string$`, %`from`, %`n`) – the %`n` characters in %`string$` starting at position %`from`
, @`min` (%`x`, `...`) – minimum
, @`mul##` (%`a##`, %`b##`) – matrix multiplication
, @`number` (%`a$`) – interpret a string as a number
, @`number#` (%`a$#`) – interpret strings as numbers
, @`numberOfColumns` (%`matrix##`)
, @`numberOfRows` (%`matrix##`)
, @`outer##` (%`a#`, %`b#`) – outer product, i.e. %%result__ij_% = %%a__i_%%%b__j_%
, @`percent$` (%`number`, %`precision`) – format a number as a string,
	with a trailing percent sign and %`precision` digits after the decimal point
, @`phonToDifferenceLimens` (%`x`) – from perceptual loudness to jnd-scale
, @`randomBernoulli` (%`p`) – Bernoulli-distributed random deviate (0 or 1)
, @`randomBernoulli#` (%`n`, %`p`) – %`n` independent Bernoulli-distributed zeroes and ones
, @`randomGamma` (%`shape`, %`rate`) – gamma-distributed random deviate
, @`randomGamma#` (%`n`, %`shape`, %`rate`) – %n independent gamma-distributed random numbers
, @`randomGamma#` (%`vector#`, %`shape`, %`rate`) – duplicate %`vector#`,
	and replace all elements with independent gamma-distributed random numbers
, @`randomGamma##` (%`nrow`, %`ncol`, %`shape`, %`rate`) – %nrow \xx %ncol independent gamma-distributed random numbers
, @`randomGamma##` (%`matrix##`, %`shape`, %`rate`) – duplicate %`matrix##`,
	and replace all cells with independent gamma-distributed random numbers
, @`randomGauss` (%`mu`, %`sigma`) – normally distributed random deviate
, @`randomGauss#` (%`n`, %`mu`, %`sigma`) – %n independent normally distributed random numbers
, @`randomGauss#` (%`vector#`, %`mu`, %`sigma`) – duplicate %`vector#`,
	and replace all elements with independent normally distributed random numbers
, @`randomGauss##` (%`nrow`, %`ncol`, %`mu`, %`sigma`) – %`nrow` \xx %`ncol` independent normally distributed random numbers
, @`randomGauss##` (%`matrix##`, %`mu`, %`sigma`) – duplicate %`matrix##`,
	and replace all cells with independent normally distributed random numbers
, @`randomInteger` (%`min`, %`max`) – uniformly distributed integer random deviate
, @`randomInteger#` (%`n`, %`min`, %`max`) – %`n` independent uniformly distributed random integers
, @`randomInteger#` (%`vector#`, %`min`, %`max`) – duplicate %`vector#`,
	and replace all elements with independent uniformly distributed random integers
, @`randomInteger##` (%`nrow`, %`ncol`, %`min`, %`max`) – %`nrow` \xx %`ncol` independent uniformly distributed random integers
, @`randomInteger##` (%`matrix##`, %`min`, %`max`) – duplicate %`matrix##`,
	and replace all cells with independent uniformly distributed random integers
, @`randomPoisson` (%`mean`) – Poisson-distributed random deviate
, @`randomPoisson#` (%`n`, %`mean`) – %`n` independent Poisson-distributed random numbers
, @`randomPoisson#` (%`vector#`, %`mean`) – duplicate %`vector#`,
	and replace all elements with independent Poisson-distributed random numbers
, @`randomPoisson##` (%nrow, %ncol, %`mean`) – %`nrow` \xx %`ncol` independent Poisson-distributed random numbers
, @`randomPoisson##` (%`matrix##`, %`mean`) – duplicate %`matrix##`,
	and replace all cells with independent Poisson-distributed random numbers
, @`randomUniform` (%`min`, %`max`) – uniformly distributed random deviate
, @`randomUniform#` (%`n`, %`min`, %`max`) – %n independent uniformly distributed random numbers
, @`randomUniform#` (%`vector#`, %min, %max) – duplicate %`vector#`,
	and replace all elements with independent uniformly distributed random numbers
, @`randomUniform##` (%`nrow`, %`ncol`, %`min`, %`max`) – %`nrow` \xx %`ncol` independent uniformly distributed random numbers
, @`randomUniform##` (%`matrix##`, %`min`, %`max`) – duplicate %`matrix##`,
	and replace all cells with independent uniformly distributed random numbers
, @`random_initializeSafelyAndUnpredictably` ( ) – undoes the effects of the following function
, @`random_initializeWithSeedUnsafelyButPredictably` (%`seed`) – reproducible sequence of random numbers
, @`readFile` (%`filePath$`) – read a number from a text file
, @`readFile$` (%`filePath$`) – read a whole text file into a string
, @`readLinesFromFile$#` (%`filePath$`) – read all lines from a text file
, @`rectify` (%`x`) – set to zero if negative (no change if positive)
, @`rectify#` (%`vector#`) – rectify each element of %`vector#`
, @`rectify##` (%`matrix##`) – rectify each cell of %`matrix##`
, @`repeat#` (%`v#`, %`n`) – repeat the whole sequence of elements of %`v#` %`n` times
, @`replace$` (%`a$`, %`b$`, %`c$`, %`n`) – within %`a$`, replace the first %`n` occurrences of %`b$` with %`c$`, or all if %`n` = 0
, @`replace_regex$` (%`a$`, %`b$`, %`c$`, %`n`) – within %`a$`,
	replace the first %`n` matches of regular expression %`b$` with the regular expression %`c$`, or all if %`n` = 0
, @`right$` (%`string$`, %`n`) – the %n last characters in %`string$`
, @`rindex` (%`string$`, %`part$`) – look up a substring from the end, or 0 if not found
, @`rindex` (%`strings$#`, %`s$`) – look up whether and where %`s$` last occurs in %`strings$#`
, @`rindex_regex` (%`string$`, %`regex$`) – determine whether and where %`string$` last matches %`regex$`
, @`round` (%`x`) – nearest integer
, @`round#` (%`vector#`) – nearest integer of each element of %`vector#`
, @`round##` (%`matrix##`) – nearest integer of each cell of %`matrix##`
, @`rowSums#` (%`matrix##`)
, @`semitonesToHertz` (%`x`) – from logarithmic scale %re 100 Hz to acoustic frequency
, @`sigmoid` (%`x`) – 1 / (1 + %e^^-%`x`^)
, @`sigmoid#` (%`vector#`) – sigmoid of each element of %`vector#`
, @`sigmoid##` (%`matrix##`) – sigmoid of each cell of %`matrix##`
, @`sin` (%`x`) – sine
, @`sin#` (%`vector#`) – sine of each element of %`vector#`
, @`sin##` (%`matrix##`) – sine of each cell of %`matrix##`
, @`sinc` (%`x`) – sinus cardinalis: sin (%`x`) / %`x`
, @`sincpi` (%`x`) – sin (%\pi%x) / (%\pi%x)
, @`sinh` (%`x`) – hyperbolic sine
, @`sinh#` (%`vector#`) – hyperbolic sine of each element of %`vector#`
, @`sinh##` (%`matrix##`) – hyperbolic sine of each cell of %`matrix##`
, @`size` (%`v#`) – number of elements
, @`shuffle#` (%`vector#`) – randomize order
, @`softmax#` (%`vector#`)
, @`softmaxPerRow##` (%`matrix##`)
, @`sort#` (%`vector#`) – reshuffle in increasing order
, @`splitByWhitespace$#` (%`string$`) – split a string into inks
, @`sqrt` (%`x`) – square root
, @`sqrt#` (%`vector#`) – square root of each element of %`vector#`
, @`sqrt##` (%`matrix##`) – square root of each cell of %`matrix##`
, @`startsWith` (%`string$`, %`part$`) – determine whether %`string$` starts with %`part$`
, @`stdev` (%`v#`) – standard deviation of the elements of a vector
, @`stopwatch` – give the current value of the timer, which is then immediately reset to zero (note: no parentheses)
, @`string$` (%`number`) – format a number as a string
, @`studentP` (%`t`, %`df`) – area under the Student %T curve up to %`t`
, @`studentQ` (%`t`, %`df`) – area under the Student %T curve after %`t`
, @`sum` (%`v#`) – sum the elements
, @`sumOver` (%`loopVariable` `to` %`n`, %`numericExpressionOptionallyUsingLoopVariable`) – \Si
, @`tan` (%`x`) – tangent
, @`tan#` (%`vector#`) – tangent of each element of %`vector#`
, @`tan##` (%`matrix##`) – tangent of each cell of %`matrix##`
, @`tanh` (%`x`) – hyperbolic tangent
, @`tanh#` (%`vector#`) – hyperbolic tangent of each element of %`vector#`
, @`tanh##` (%`matrix##`) – hyperbolic tangent of each cell of %`matrix##`
, @`to#` (%`n`) – the integers 1 through %`n`
, @`transpose##` (%`matrix##`) – flip matrix along its diagonal
, @`unicode` (%`c$`) – the Unicode codepoint number that corresponds to character %`c$`
, @`unicode$` (%`n`) – the character that corresponds to Unicode codepoint %`n`
, @`unicodeToBackslashTrigraphs$` (%`string$`) – convert e.g. \ct to \bsct
, @`writeFile` (%`filePath$`, `...`) – create a new text file, and write texts, numbers, vectors and so on into it
, @`writeFileLine` (%`filePath$`, `...`) – create a new text file, write texts, numbers, vectors and so on into it, followed by a newline
, @`zero#` (%`n`) – vector with %`n` elements that are all 0
, @`zero##` (%`nrow`, %`ncol`) – matrix with %`nrow` \xx %`ncol` elements that are all 0

################################################################################
"`abs`"
© Paul Boersma 2023

A function that can be used in @@Formulas@. The absolute value.

Examples
========
{
	writeInfoLine: \#{abs} (3.14), " ", \#{abs} (0), " ", \#{abs} (-789e-300)
}

################################################################################
"`abs#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

`abs#` (%`v#`) computes the absolute value (@`abs`) of each element of the vector %`v#`.

Examples
========
{
	a# = \#{abs#} ({ -1, 5.5, 0, -456.789 })
	writeInfoLine: a#
}

################################################################################
"`abs##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

`abs##` (%`m##`) computes the absolute value (@`abs`) of each cell of the matrix %`m##`.

Examples
========
{
	a## = \#{abs##} ({{ -1, 5.5, 0, -456.789 }, { 34, -45, 18.0, 0 },
	... { 1e-89, -3.2e-307, 6e200, -2345e-2 }})
	writeInfoLine: a##
}

################################################################################
"`appendFile`"
© Paul Boersma 2023-06-30

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendFile` (%`filePath$`, ...)
: write texts, numbers, vectors and so on
	at the end of an existing file (create such a file if it does not exist yet)

################################################################################
"`appendFileLine`"
© Paul Boersma 2023-06-30

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendFileLine` (%`filePath$`, ...)
: write texts, numbers, vectors and so on, followed by a newline,
	at the end of an existing file (create such a file if it does not exist yet)

################################################################################
"`appendInfo`"
© Paul Boersma 2023-06-30

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendInfo` (%`filePath$`, ...)
: write texts, numbers, vectors and so on
	to the end of the Info window

################################################################################
"`appendInfoLine`"
© Paul Boersma 2023-06-30

A function that can be used in @Scripting.

Syntax and semantics
====================
#`appendInfoLine` (%`filePath$`, ...)
: write texts, numbers, vectors and so on, followed by a newline,
	to the end of the Info window

################################################################################
"`arccos`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@. The inverse cosine.

Domain
======
#`arccos` (%x) is defined for -1 \<_ %x \<_ 1.

Tests
=====
{
	\`{assert} \#{arccos} (-1.1) = undefined
	\`{assert} abs (\#{arccos} (-1.0) - pi) < 1e-17
	\`{assert} abs (\#{arccos} (0.0) - pi/2) < 1e-17
	\`{assert} \#{arccos} (1.0) = 0
	\`{assert} \#{arccos} (1.1) = undefined
	\`{assert} \#{arccos} (undefined) = undefined
}

################################################################################
"`arccos#`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arccos#` (%`v#`) computes the inverse cosine (@`arccos`) of each element of the vector %`v#`.

Examples
========
{
	writeInfoLine: \#{arccos#} ({ -1.1, -1.0, 0.0, 1.0, 1.1, undefined })
}

################################################################################
"`arccos##`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arccos##` (%`m##`) computes the inverse cosine (@`arccos`) of each cell of the matrix %`m##`.

################################################################################
"`arccosh`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@. The inverse hyperbolic cosine, i.e.
~	#`arccosh` (%x) = ln (%x + \Vr(%x^2−1))

Tests
=====
{
	\`{assert} \#{arccosh} (1.0) = 0
	\`{assert} \#{arccosh} (0.9) = undefined
	\`{assert} \#{arccosh} (0.0) = undefined
	\`{assert} \#{arccosh} (-10.0) = undefined
	\`{assert} \#{arccosh} (undefined) = undefined
}

################################################################################
"`arccosh#`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arccosh#` (%`v#`) computes the inverse hyperbolic cosine (@`arccosh`) of each element of the vector %`v#`.

################################################################################
"`arccosh##`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arccosh##` (%`m##`) computes the inverse hyperbolic cosine (@`arccosh`) of each cell of the matrix %`m##`.

################################################################################
"`arcsin`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@. The inverse sine.

Domain
======
#`arcsin` (%x) is defined for -1 \<_ %x \<_ 1.

Tests
=====
{
	\`{assert} \#{arcsin} (-1.1) = undefined
	\`{assert} abs (\#{arcsin} (-1.0) - -pi/2) < 1e-17
	\`{assert} \#{arcsin} (0.0) = 0
	\`{assert} abs (\#{arcsin} (1.0) - pi/2) < 1e-17
	\`{assert} \#{arcsin} (1.1) = undefined
	\`{assert} \#{arcsin} (undefined) = undefined
}

################################################################################
"`arcsin#`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arcsin#` (%`v#`) computes the inverse sine (@`arcsin`) of each element of the vector %`v#`.

Examples
========
{
}

################################################################################
"`arcsin##`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arcsin##` (%`m##`) computes the inverse sine (@`arcsin`) of each cell of the matrix %`m##`.

################################################################################
"`arcsinh`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@. The inverse hyperbolic sine:
~	#`arcsinh` (%x) = ln (%x + \Vr(1+%x^2))

Tests
=====
{
	\`{assert} \#{arcsinh} (-1.0) < 0
	\`{assert} \#{arcsinh} (0.0) = 0
	\`{assert} \#{arcsinh} (1.0) > 0
	\`{assert} \#{arcsinh} (undefined) = undefined
}

################################################################################
"`arcsinh#`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arcsinh#` (%`v#`) computes the inverse hyperbolic sine (@`arcsinh`) of each element of the vector %`v#`.

Examples
========
{
}

################################################################################
"`arcsinh##`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arcsin##` (%`m##`) computes the inverse hyperbolic sine (@`arcsinh`) of each cell of the matrix %`m##`.

################################################################################
"`arctan`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@. The inverse tangent.

Tests
=====
{
	\`{assert} \#{arctan} (0.0) = 0
}

################################################################################
"`arctan#`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arctan#` (%`v#`) computes the inverse tangent (@`arctan`) of each element of the vector %`v#`.

Examples
========
{
}

################################################################################
"`arctan##`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arctan##` (%`m##`) computes the inverse tangent (@`arctan`) of each cell of the matrix %`m##`.

################################################################################
"`arctan2`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@. The argument angle.

Syntax and semantics
====================
#`arctan2` (%`y`, %`x`)
:	the angle (in radians) from the %X axis to the point (%x, %y) in two-dimensional space;
	positive (i.e. measured counterclockwise) for %y > 0,
	and negative (and measured clockwise) for %y < 0.
	For %y = 0, it’s 0 if %x > 0, and %\pi if %x < 0.
	In the point (0, 0), `arctan2` is undefined.

Tests
=====
{
	\`{assert} \#{arctan2} (0.0, 3.0) = 0
	\`{assert} abs (\#{arctan2} (3.0, 3.0) - pi/4) < 1e-17
	\`{assert} abs (\#{arctan2} (3.0, 0.0) - pi/2) < 1e-17
	\`{assert} abs (\#{arctan2} (3.0, -3.0) - 3*pi/4) < 1e-17
	\`{assert} abs (\#{arctan2} (0.0, -3.0) - pi) < 1e-17
	\`{assert} abs (\#{arctan2} (-3.0, -3.0) + 3*pi/4) < 1e-17
	\`{assert} abs (\#{arctan2} (-3.0, 0.0) + pi/2) < 1e-17
	\`{assert} abs (\#{arctan2} (-3.0, 3.0) + pi/4) < 1e-17
	;\`{assert} \#{arctan2} (0.0, 0.0) = undefined
}
Edge cases
==========
In C, and therefore in Numpy, `arctan2` has arbitrary values
where it cannot be defined. Thus `numpy.arctan2 (0.0, +0.0)` is 0,
`numpy.arctan2 (+0.0, -0.0)` is +%\pi,
and `numpy.arctan2 (-0.0, -0.0)` is \-m%\pi.
The function is then even defined for e.g. %x = \-m∞ and %y = +∞,
where it is 3%\pi/4. In all these cases, `arctan` returns `undefined` in Praat.
This doesn’t mean that the function is very sane in Praat, as
it is still discontinuous for small %`y`:
{
	writeInfoLine: \#{arctan2} (1e-308, -1.0), " ", \#{arctan2} (-1e-308, -1.0)
}
which are indistinguishable from +%\pi and \-m%\pi, respectively.

################################################################################
"`arctanh`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@. The inverse hyperbolic tangent.

Tests
=====
{
	\`{assert} \#{arctanh} (-1.0) = undefined
	\`{assert} \#{arctanh} (0.0) = 0
	\`{assert} \#{arctanh} (1.0) = undefined
	\`{assert} \#{arctanh} (1000) = undefined
	\`{assert} \#{arctanh} (undefined) = undefined
}

################################################################################
"`arctanh#`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arctanh#` (%`v#`) computes the inverse hyperbolic tangent (@`arctanh`) of each element of the vector %`v#`.

Examples
========
{
}

################################################################################
"`arctanh##`"
© Paul Boersma 2023-06-29

A function that can be used in @@Formulas@.
`arctanh##` (%`m##`) computes the inverse hyperbolic tangent (@`arctanh`) of each cell of the matrix %`m##`.

################################################################################
"`assert`"
© Paul Boersma 2023-06-29

A keyword that can be used in @Scripting, to test whether an expected condition holds.

Examples
========

This is an assertion that is satisfied:
{
	a = 5*6
	\#{assert} a = 30
	writeInfoLine: a
}
This is an assertion that is violated:
{;
	a = 5*6
	\#{assert} a < 10
	writeInfoLine: a
}
This three-line script stops at the second line with the error message
“`Assertion violated in line 2: a < 10`”.

That is, because %`a` is not less than 10 (it’s 30),
the assertion is false, and the script stops, mentioning the line number
as well as the text of the assertion (i.e. “`a < 10`”).

################################################################################
"`backslashTrigraphsToUnicode$`"
© Paul Boersma 2023-06-30

A function that can be used in @Formulas.

Syntax and semantics
====================
#`backslashTrigraphsToUnicode$` (%`string$`)
converts from backslash trigraphs to unicode characters
e.g. from “\bsct” to “\ct”.

################################################################################
"`barkToHertz`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`barkToHertz` (%`x`) converts from an auditory Bark-rate value %`x` to an acoustic frequency in Hz,
using the formula 650 sinh (%`x` / 7).

################################################################################
"`besselI`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`besselI` (%`n`, %`x`)
: modified Bessel function of the first kind, %I__%`n`_

################################################################################
"`besselK`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`besselK` (%`n`, %`x`)
: modified Bessel function of the second kind, %K__%`n`_

################################################################################
"`beta`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`beta` (%`x`, %`y`)
: %\be function

################################################################################
"`between_by#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`between_by#` (%`low`, %`high`, %`step`)
: symmetrically dispersed numbers between %`low` and %`high`, in steps of %`step`

################################################################################
"`between_count#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Syntax and semantics
====================
#`between_count#` (%`low`, %`high`, %`n`)
: %`n` symmetrically dispersed numbers between %`low` and %`high` (bin centres)

################################################################################
"`binomialP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@. The cumulative binomial distribution.

Syntax and semantics
====================
#`binomialP` (%`p`, %`k`, %`n`)
:	the probability that in %`n` trials an event with probability %`p` will occur at most %`k` times.

Related function
================
It is probably more common to use the complement of this function, namely @`binomialQ`.

################################################################################
"`binomialQ`"
© Paul Boersma 2014-02-23, 2023

A function that can be used in @@Formulas@. The complement of the cumulative binomial distribution.

Syntax and semantics
====================
#`binomialQ` (%`p`, %`k`, %`n`)
:	the probability that in %`n` trials an event with probability %`p` will occur at least %`k` times.

Calculator example
==================

A die is suspected to yield more sixes than a perfect die would do. In order to test this suspicion,
you throw it 1,000 times. The result is 211 sixes.

The probability that a perfect die yields at least 211 sixes is, according to @@Calculator...@,
{
	appendInfoLine: \#{binomialQ} (1/6, 211, 1000)
}
which should be 0.000152.

Script example
==============

You convert 1000 values of pitch targets in Hz to the nearest note on the piano keyboard.
597 of those values turn out to be in the A, B, C, D, E, F, or G regions (the white keys), and 403 values turn out
to be in the A\# , C\# , D\# , F\# , or G\#  regions (the black keys).
Do our subjects have a preference for the white keys?
The following script computes the probability that in the case of no preference the subjects
would target the white keys at least 597 times. This is compared with a %\ci^2 test.
{
	a = 597
	b = 403
	p = 7/12   ; no preference
	writeInfoLine: “*** Binomial test ”, a, “, ”, b, “, p = ”, fixed$ (p, 6), “ ***”
	pbin = \#{binomialQ} (p, a, a+b)
	appendInfoLine: “P (binomial) = ”, fixed$ (pbin, 6)
	# Chi-square test with Yates correction:
	x2 = (a - 1/2 -   p   * (a+b)) ^ 2 / (  p   * (a+b)) +
	...  (b + 1/2 - (1-p) * (a+b)) ^ 2 / ((1-p) * (a+b))
	px2 = chiSquareQ (x2, 1)
	appendInfoLine: “P (chi-square) = ”, fixed$ (px2, 6)
}
The result is:

	*** Binomial test 597, 403, p = 0.583333 ***
	P (binomial) = 0.199330
	P (chi-square) = 0.398365

The %\ci^2 test is two-sided (it signals a preference for the white or for the black keys),
so it has twice the probability of the binomial test.

We cannot conclude from this test that people have a preference for the white keys.
Of course, we cannot conclude either that people %%don’t% have such a preference.

################################################################################
"`ceiling`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
#`ceiling` (%`x`) rounds %`x` up to the nearest integer number.

################################################################################
"`ceiling#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
`ceiling#` (%`v#`) rounds up (@`ceiling`) each element of the vector %`v#`.

################################################################################
"`ceiling##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
`ceiling##` (%`m##`) rounds up (@`ceiling`) each cell of the matrix %`m##`.

################################################################################
"`center`"
© Paul Boersma 2023-07-01

A function that can be used in @@Formulas@.

`center` (%`v#`) computes the center (or centre) of gravity of the vector %`v#`.

################################################################################
"`chiSquareP`"
© Paul Boersma 2023-07-01

A function that can be used in @@Formulas@.

`chiSquareP` (%`chiSquare`, %`df`) computes the area under the %\ci^2 distribution between 0 and %`chiSquare`,
for %`df` degrees of freedom.

################################################################################
"`chiSquareQ`"
© Paul Boersma 2023-07-01

A function that can be used in @@Formulas@.

`chiSquareQ` (%`chiSquare`, %`df`) computes the area under the %\ci^2 curve between %`chiSquare` and +\\oo,
for %`df` degrees of freedom: the “statistical significance %p”
of the %\ci^2 difference between two distributions in %`df`+1 dimensions.

################################################################################
"`chooseFolder$`"
© Paul Boersma 2023-07-02

A function that can be used in @Scripting.

################################################################################
"`chooseReadFile$`"
© Paul Boersma 2023-07-02

A function that can be used in @Scripting.

################################################################################
"`chooseWriteFile$`"
© Paul Boersma 2023-07-02

A function that can be used in @Scripting.

################################################################################
"`columnSums#`"
© Paul Boersma 2023-07-02

A function that can be used in @Formulas.

################################################################################
"`cos`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
#`cos` (%`x`) computes the cosine of %`x`.
{
	Axes: -5, 5, -1.1, 1.1
	Draw inner box
	Draw function: -5, 5, 1000, ~ \#{cos} (x)
	One mark bottom: -3*pi/2, "no", "yes", "yes", "\-m3%\pi/2"
	One mark bottom: -pi, "no", "yes", "yes", "\-m%\pi"
	One mark bottom: -pi/2, "no", "yes", "yes", "\-m%\pi/2"
	One mark bottom: 0, "no", "yes", "yes", "0"
	One mark bottom: pi/2, "no", "yes", "yes", "+%\pi/2"
	One mark bottom: pi, "no", "yes", "yes", "+%\pi"
	One mark bottom: 3*pi/2, "no", "yes", "yes", "+3%\pi/2"
	One mark left: -1, "no", "yes", "yes", "\-m1"
	One mark left: 0, "no", "yes", "yes", "0"
	One mark left: 1, "no", "yes", "yes", "+1"
	Text bottom: "yes", "%x \->"
	Text left: "yes", "##\#{cos}# (%x)"
	Text top: "no", "##just over one and a half periods of a (co)sine wave"
}
This picture illustrates the following properties of the cosine function:

- #`cos` (%x) is zero for all values of %x that are odd multiples of %\pi/2.
- The extrema of #`cos` (%x) are \-m1 and +1.
- #`cos` (%x) is \-m1 or +1 wherever %x is an integer multiple of %\pi.
- The cosine function is %periodic with period 2%\pi, i.e., #`cos` (%x+2%\pi) = #`cos` (%x).
- The cosine function is %symmetric around 0, i.e. #`cos` (\-m%x) = #`cos` (%x).
- The cosine function is %antisymmetric around %\pi/2, i.e. #`cos` (%\pi/2\-m%x) = \-m #`cos` (%x);
  in other words, if you take the cosine curve and rotate it by 180 degrees
  around the point (%\pi/2, 0), then you get the same curve again.

A cosine wave is a special case of the more general %%sine wave% that is given by
%y = @`sin` (%x + %\fi), where %\fi (an additive constant called the %%initial phase%) is %\pi/2.

################################################################################
"`cos#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
`cos#` (%`v#`) computes the cosine (@`cos`) of each element of the vector %`v#`.

################################################################################
"`cos##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
`cos##` (%`m##`) computes the cosine (@`cos`) of each cell of the matrix %`m##`.

################################################################################
"`cosh`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
#`cosh` (%`x`) computes the hyperbolic cosine of %`x`, i.e. (%e^%x + %e^^-%x^) / 2.

################################################################################
"`cosh#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.
`cosh#` (%`v#`) computes the hyperbolic cosine (@`cosh`) of each element of the vector %`v#`.

################################################################################
"`cosh##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

`cosh##` (%`m##`) computes the hyperbolic cosine (@`cosh`) of each cell of the matrix %`m##`.

################################################################################
"`createFolder`"
© Paul Boersma 2023-07-02

A function that can be used in @Scripting.

################################################################################
"`date$`"
© Paul Boersma 2023-07-02

A function that can be used in @Formulas.

{
	writeInfoLine: date$ ()
}

################################################################################
"`date#`"
© Paul Boersma 2023-07-02

A function that can be used in @Formulas.

{
	writeInfoLine: date# ()
}

################################################################################
"`date_utc$`"
© Paul Boersma 2023-07-02

A function that can be used in @Formulas.

{
	writeInfoLine: date_utc$ ()
}

################################################################################
"`date_utc#`"
© Paul Boersma 2023-07-02

A function that can be used in @Formulas.

{
	writeInfoLine: date_utc# ()
}

################################################################################
"`deleteFile`"
© Paul Boersma 2023

A function that can be used in @Scripting.

################################################################################
"`differenceLimensToPhon`"
© Paul Boersma 2002-12-15, 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`differenceLimensToPhon` (%`ndli`) converts auditory intensity difference limens
into a sensation level in phon units.

Definition
==========
~	#`differenceLimensToPhon` (%ndli) = @`ln` (1 + %ndli / 30) / @`ln` (61 / 60)

Related function
================
#`differenceLimensToPhon` is the inverse of @`phonToDifferenceLimens`.

################################################################################
"`endsWith`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`endsWith` (%`string$`, %`part$`) determines whether %`string$` ends in %`part$`.

################################################################################
"`erb`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erb` (%`f`) computes the auditory %%equivalent rectangular bandwidth% (ERB) in hertz,
for a specified acoustic frequency %f (also in hertz),
using the formula 6.23\.c10^^\-m6^ %f^2 + 0.09339 %f + 28.52.

################################################################################
"`erbToHertz`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erbToHertz` (%`x`) converts from an auditory ERB-rate %x to an acoustic frequency in Hz,
using the formula (14680 %d - 312) / (1 - %d) where %d = exp ((%`x` - 43) / 11.17).

################################################################################
"`erf`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erf` (%`x`) computes the “error function”, the integral of the Gaussian,
i.e. 2/\Vr%\pi _0\in^%`x` exp(-%t^2) %dt.

The complement of #`erf` is @`erfc`.

################################################################################
"`erfc`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`erfc` (%`x`) computes the complement of the error function, i.e. 1 − @`erf` (%`x`)

Why isn’t this function superfluous?
====================================
Because of floating-point rounding errors:
although one might think that @`erf` ic sufficiebt,
#`erfc` is a separate function because @`erf` (%`x`) can be close to 1.

################################################################################
"`exp`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`exp` (%`x`) exponentiates %x, i.e. it computes %e^%`x`.

The inverse of #`exp` is @`ln`.

################################################################################
"`exp#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`exp#` (%`v#`) exponentiates (@`exp`) each element of the vector %`v#`.

################################################################################
"`exp##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`exp##` (%`m##`) exponentiates (@`exp`) each cell of the matrix %`m##`.

################################################################################
"`extractLine$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`extractLine$` (%`string$`, %`part$`) returns everything that comes after the first occurrence of %`part$` in %`string$`.

################################################################################
"`extractNumber`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`extractNumber` (%`string$`, %`part$`) returns the first number after the first occurrence of %`part$% in %`string$`.

################################################################################
"`extractWord$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`extractWord$` (%`string$`, %`part$`) return the first “word” after the first occurrence of %`part$% in %`string$`.

################################################################################
"`fileNames$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`fileNames$#` (%`folderNameOrPattern$`) returns the names (not the whole paths) of the files in a folder
	or that match a pattern with an asterisk.

################################################################################
"`fileReadable`"
© Paul Boersma 2023

A function that can be used in @@Formulas@, especially in @Scripting.

Syntax and semantics
====================
#`fileReadable` (%`filePath$`) returns 1 if the file exists and can be read, or 0 otherwise.

################################################################################
"`fisherP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@: the cumulative Fisher’s F-distribution.

Syntax and semantics
====================
#`fisherP` (%`f`, %`df1`, %`df2`) computes the area under Fisher’s F-distribution \-m\oo to %f,
for %`df1` numerator degrees of freedom and %`df2` denominator degrees of freedom.

################################################################################
"`fisherQ`"
© David Weenink 2000-05-25, Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`fisherQ` (%`f`, %`df1`, %`df2`) computes the area under Fisher’s F-distribution from %f to +\oo,
for %`df1` numerator degrees of freedom and %`df2` denominator degrees of freedom.

Related functions
=================
(1) #`fisherQ` is the inverse of @`invFisherQ`, i.e.
~	fisherQ (invFisherQ (%q, %df_1, %df_2), %df_1, %df_2) = %q

, and
~	invFisherQ (fisherQ (%f, %df_1, %df_2), %df_1, %df_2) = %f

(2) #`fisherQ` is the complement of @`fisherP`, i.e.
~	fisherQ (%f, %df1, %df2) = 1 \-m fisherP (%f, %df1, %df2)

, and
~	fisherP (%f, %df1, %df2) = 1 \-m fisherQ (%f, %df1, %df2)

################################################################################
"`fixed$`"
© Paul Boersma 2023-07-11

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`fixed$` (%`number`, %`precision`) formats a number as a string,
with %`precision` digits after the decimal point.

################################################################################
"`floor`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`floor` (%`x`) rounds %`x` down to the nearest integer number.

################################################################################
"`floor#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
`floor#` (%`v#`) rounds down (@`floor`) each element of the vector %`v#`.

################################################################################
"`floor##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
`floor##` (%`m##`) rounds down (@`floor`) each cell of the matrix %`m##`.

################################################################################
"`folderNames$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@, especially in @Scripting.

Synopsis
========
#`folderNames$#` (%`folderNameOrPattern$`) returns the names (not the whole paths) of the subfolders in a folder
	or that match a pattern with an asterisk.

################################################################################
"`from_to#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`from_to#` (%`m`, %`n`) returns the integers from %`m` through %`n`.

################################################################################
"`from_to_by#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`from_to_by#` (%`m`, %`n`, %`step`) returns the numbers from %`m` through %`n`, in steps of %`step`.

################################################################################
"`from_to_count#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`from_to_count#` (%`start`, %`end`, %`n`) returns %`n` equally-spaced numbers
starting with %`start` and ending with %`end`.

################################################################################
"`gaussP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@: the cumulative normal (Gauss) distribution.

Syntax and semantics
====================
#``gaussP` (%`z`) computes the area under the normal distribution
(with mean 0 and standard deviation 1) from \-m\oo to %`z`.

################################################################################
"`gaussQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`gaussQ` (%`z`) computes the area under the normal distribution
(with mean 0 and standard deviation 1) from %z to +\oo.

Related functions
=================
(1) #`gaussQ` is the inverse of @`invGaussQ`, i.e.
~	gaussQ (invGaussQ (%q)=) = %q

, and
~	invGaussQ (gaussQ (%z)) = %z

(2) #`gaussQ` is the complement of @`gaussP`, i.e.
~	gaussQ (%z) = 1 \-m gaussP (%z)

, and
~	gaussP (%z) = 1 \-m gaussQ (%z)

################################################################################
"`hertzToBark`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToBark` (%`x`) converts from an acoustic frequency %x in Hz to an auditory Bark-rate
(auditory spectral frequency; place on basilar membrane),
using the formula 7 ln (%`x`/650 + \Vr(1 + (%`x`/650)^2)).

################################################################################
"`hertzToErb`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToErb` (%`x`) converts from an acoustic frequency %`x` in Hz to an auditory ERB-rate,
using the formula 11.17 ln ((%`x` + 312) / (%`x` + 14680)) + 43.

################################################################################
"`hertzToMel`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToMel` (%`x`) converts from an acoustic frequency %`x` in Hz to an auditory pitch in mel,
using the formula 550 ln (1 + %`x` / 550).

################################################################################
"`hertzToSemitones`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`hertzToSemitones` (%`x`) converts from an acoustic frequency %x in Hz
to a logarithmic musical scale relative to 100 Hz,
using the formula 12 ln (%`x` / 100) / ln 2.

################################################################################
"`imax`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`imax` (%`x`, ...) computes the location of the maximum of a sequence of numbers.

Tests
=====
{
	assert imax (7.2, -5, 3) = 1
}

################################################################################
"`imin`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`imin` (%`x`, ...) computes the location of the minimum of a sequence of numbers.

Tests
=====
{
	assert imin (7.2, -5, 3) = 2
}

################################################################################
"`index`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`index` (%`string$`, %`part$`) returns the first location of the substring %`part$` inside %`string$`, or 0 if not found.

#`index` (%`strings$#`, %`s$`) looks up whether and where %`s$` first occurs in %`strings$`.

################################################################################
"`index_regex`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`index_regex` (%`string$`, %`regex$`) determines whether and where %`string$` first matches %`regex$`.

################################################################################
"`inner`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`inner` (%`a#`, %`b#`) computes the inner product of the vectors %`a#` and %`b#`.

################################################################################
"`invBinomialP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invBinomialP` (%`p`, %`k`, %`n`) returns the inverse cumulative binomial distribution,
i.e. the value %prob for which @`binomialP` (%`prob`, %`k`, %`n`) = %`p`.

################################################################################
"`invBinomialQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invBinomialQ` (%`q`, %`k`, %`n`) returns the inverse cumulative binomial distribution,
i.e. the value %prob for which @`binomialQ` (%`prob`, %`k`, %`n`) = %`q`.

################################################################################
"`invChiSquareQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invChiSquareQ` (%`q`, %`df`) returns the inverse %\ci^2 (chi-square) distribution,
i.e. the value %chisq for which @`chiSquareQ` (%`chisq`, %`df`) = %`chisq`.

################################################################################
"`invFisherQ`"
© David Weenink 2000, Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invFisherQ` (%`q`, %`df1`, %`df2`) returns the inverse cumulative Fisher F-distribution,
i.e. the value %f for which @`fisherQ` (%`f`, %`df1`, %`df2`) = %`q`.

################################################################################
"`invGaussQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invGaussQ` (%`q`) returns the inverse cumulative normal (Gauss) distribution,
i.e. the value %z for which @`gaussQ` (%`z`) = %`q`.

################################################################################
"`invSigmoid`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invSigmoid` (%`x`) is defined for 0 < %`x` < 1
and computes the inverse @`sigmoid`, i.e. ln (%`x` / (1 − %`x`)).
The possible results are all real numbers.

################################################################################
"`invSigmoid#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invSigmoid#` (%`v#`) computes the inverse sigmoid of each element of the vector %`v#`.

################################################################################
"`invSigmoid##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invSigmoid##` (%`m##`) computes the inverse sigmoid of each cell of the matrix %`m##`.

################################################################################
"`invStudentQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`invStudentQ` (%`q`, %`df`) returns the inverse cumulative Student %t distribution,
i.e. the value %t for which @`studentQ` (%`t`, %`df`) = %`q`.

################################################################################
"`left$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`left$` (%`string$`, %`n`) returns the %`n` first characters in %`string$`.

################################################################################
"`length`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`length` (%`string$`) returns the number of Unicode characters in %`string$`.

################################################################################
"`ln`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`ln` (%`x`) returns the natural logarithm of %`x`, i.e. the logarithm with base %e.

################################################################################
"`ln#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`ln#` (%`v#`) computes the natural logarithm (@`ln`) of each element of the vector %`v#`.

################################################################################
"`ln##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`ln##` (%`m##`) computes the natural logarithm (@`ln`) of each cell of the matrix %`m##`.

################################################################################
"`lnGamma`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`lnGamma` (%`x`) returns the natural logarithm of the \Ga function of %`x`.

################################################################################
"`log2`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`log2` (%`x`) returns the base-2 logarithm of %`x`.

################################################################################
"`log2#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`log2#` (%`v#`) computes the base-2 logarithm (@`log2`) of each element of the vector %`v#`.

################################################################################
"`log2##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`log2##` (%`m##`) computes the base-2 logarithm (@`log2`) of each cell of the matrix %`m##`.

################################################################################
"`log10`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`log10` (%`x`) returns the base-10 logarithm of %`x`.

################################################################################
"`log10#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`log10#` (%`v#`) computes the base-10 logarithm (@`log10`) of each element of the vector %`v#`.

################################################################################
"`log10##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`log10##` (%`m##`) computes the base-10 logarithm (@`log10`) of each cell of the matrix %`m##`.

################################################################################
"`max`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`max` (%`x`, ...) computes the maximum of a list of numbers.

Tests
=====
{
	assert max (7.2, -5, 3) = 7.2
}

################################################################################
"`mean`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`mean` (%`v#`) computes the average of the elements of the vector `v#`.

################################################################################
"`melToHertz`"
© Paul Boersma 2023-06-30

A function that can be used in @Formulas.

Syntax and semantics
====================
#`melToHertz` (%`x`) converts from an auditory pitch %`x` in mel to an acoustic frequency in Hz,
using the formula 550 (exp (%`x` / 550) - 1).

################################################################################
"`mid$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`mid$` (%`string$`, %`from`, %`n`) returns the %`n` characters in %`string$` starting at position %`from`.

################################################################################
"`min`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`min` (%`x`, ...) computes the minimum of a list of numbers.

Tests
=====
{
	assert min (7.2, -5, 3) = -5
}

################################################################################
"`mul##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`mul##` (%`a##`, %`b##`) performs a matrix multiplication of the matrices %`a##` and %`b##`.

################################################################################
"`number`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`number` (%`a$`) interprets a string as a number.

################################################################################
"`number#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`number#` (%`a$#`) interprets strings as numbers.

################################################################################
"`numberOfColumns`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`numberOfColumns` (%`m##`) returns the number of columns of the matrix %`m##`.

################################################################################
"`numberOfRows`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`numberOfRows` (%`m##`) returns the number of rows of the matrix %`m##`.

################################################################################
"`outer##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`outer##` (%`a#`, %`b#`) computes the outer product of the vectors %`a#` and %`b#`, i.e.

~	%result__%i%j_ = %a_%i %b_%j

################################################################################
"`percent$`"
© Paul Boersma 2023-07-11

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`percent$` (%`number`, %`precision`) formats a number as a string,
with a trailing percent sign and %`precision` digits after the decimal point.

################################################################################
"`phonToDifferenceLimens`"
© Paul Boersma 2002-12-15, 2023

A function for converting sensation level in phons into intensity difference limen level,
the inverse of @`differenceLimensToPhon`.

Formula
=======
~	phonToDifferenceLimens (%phon) = 30 · ((61/60)^^ %phon^ − 1)

Derivation
==========

In first approximation, humans can detect an intensity difference of 1 phon, i.e.
if two sounds that differ only in intensity are played a short time after each other,
people can generally detect their intensity difference if it is greater than 1 phon.

But the sensitivity is somewhat better for louder sounds.
According to @@Jesteadt, Wier & Green (1977)@, the relative difference limen
of intensity is given by
~	DLI = \De%I / %I = 0.463 · (%I / %I_0)^^−0.072^

In this formula, %I is the intensity of the sound in Watt/m^2, %I_0 is the intensity of
the auditory threshold (i.e. 10^^−12^ Watt/m^2 at 1000 Hz),
and \De%I is the just noticeable difference.

@@Boersma (1998)|Boersma (1998: 109)@ calculates a difference-limen scale from this.
Given an intensity %I, the number of difference limens above threshold is
~	\in__%%I%0_^%I %dx \De%I(%x)
			= (1 / 0.463) \in__%%I%0_^%I %dx %I_0^^−0.072^ %x^^0.072−1^
~	= (1 / (0.463 · 0.072)) ((%I / %I_0)^^0.072^ − 1)

The sensation level in phon is defined as
~	SL = 10 log__10_ (%I / %I_0)

so that the number of difference limens above threshold is
~	(1 / (0.463 · 0.072)) (10^^(0.072 / 10) (10 log (%I / %I_0))^ − 1)
			= 30 · (1.0167^^SL^ − 1)

################################################################################
"`randomBernoulli`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomBernoulli` (%`p`) generates a Bernoulli-distributed random deviate,
which is 1 with probability %`p`, and 0 with probability 1 \-m %`p`.

################################################################################
"`randomBernoulli#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomBernoulli#` (%`n`, %`p`) generates %`n`
independent Bernoulli-distributed (@`randomBernoulli`) zeroes and ones.

#`randomBernoulli#` (%`model#`, %`p`) creates a vector with the same number of elements
as %`model#`, and fills all elements with independent Bernoulli-distributed zeroes and ones.
This is shorthand for doing

	\#{randomBernoulli#} (\`{size} (model#), \%{p})

################################################################################
"`randomBernoulli##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomBernoulli##` (%`nrow`, %`ncol`, %`p`) generates %`nrow` \xx %`ncol`
independent Bernoulli-distributed (@`randomBernoulli`) zeroes and ones.

#`randomBernoulli##` (%`model#`, %`p`) creates a matrix with the same number of rows and columns
as %`model#`, and fills all cells with independent Bernoulli-distributed zeroes and ones.
This is shorthand for doing

	\#{randomBernoulli##} (\`{numberOfRows} (model##), \`{numberOfColumns} (model##), \%{p})

################################################################################
"`randomGamma`"
© David Weenink 2019, Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomGamma` (%`shape`, %`rate`) generates a random number from a Gamma distribution
with shape parameter %`shape` and rate parameter %`rate`, which is defined as
the probability density function %f(%x; %`shape`, %`rate`) =
(1 / \Ga (%`shape`)) %`rate`^^%`shape`^ %x^^%`shape`−1^ %e^^−%`rate` %`x`^, "
for %`x` > 0, %`shape` > 0 and %`rate` > 0,
according to the method by @@Marsaglia & Tsang (2000)@.

################################################################################
"`randomGamma#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomGamma#` (%`n`, %`shape`, %`rate`) generates %`n`
independent \Ga-distributed (@`randomGamma`) numbers.

#`randomGamma#` (%`model#`, %`shape`, %`rate`) creates a vector with the same number of elements
as %`model#`, and fills all elements with independent \Ga-distributed numbers.
This is shorthand for doing

	\#{randomGamma#} (\`{size} (model#), %`shape`, %`rate`)

################################################################################
"`randomGamma##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomGamma##` (%`nrow`, %`ncol`, %`shape`, %`rate`) generates %`nrow` \xx %`ncol`
independent \Ga-distributed (@`randomGamma`) numbers.

#`randomGamma##` (%`model#`, %`shape`, %`rate`) creates a matrix with the same number of rows and columns
as %`model#`, and fills all cells with independent \Ga-distributed numbers.
This is shorthand for doing

	\#{randomGammma##} (\`{numberOfRows} (model##), \`{numberOfColumns} (model##), %`shape`, %`rate`)

################################################################################
"`randomGauss`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomGauss` (%`mu`, %`sigma`) generates a normally distributed random deviate,
with true population mean %`mu` and true population standard deviation %`sigma`.

################################################################################
"`randomGauss#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomGauss#` (%`n`, %`mu`, %`sigma`) generates %`n`
independent normally distributed (@`randomGauss`) numbers.

#`randomGauss#` (%`model#`, %`mu`, %`sigma`) creates a vector with the same number of elements
as %`model#`, and fills all elements with independent normally distributed numbers.
This is shorthand for doing

	\#{randomGauss#} (\`{size} (model#), %`mu`, %`sigma`)

################################################################################
"`randomGauss##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomGauss##` (%`nrow`, %`ncol`, %`mu`, %`sigma`) generates %`nrow` \xx %`ncol`
independent normally distributed (@`randomGauss`) numbers.

#`randomGauss##` (%`model#`, %`mu`, %`sigma`) creates a matrix with the same number of rows and columns
as %`model#`, and fills all cells with independent normally distributed numbers.
This is shorthand for doing

	\#{randomGauss##} (\`{numberOfRows} (model##), \`{numberOfColumns} (model##), %`mu`, %`sigma`)

################################################################################
"`randomInteger`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomInteger` (%`min`, %`max`) generates a uniformly distributed integer random deviate,
with minimum %`min` and maximum %`max`.

################################################################################
"`randomInteger#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomInteger#` (%`n`, %`min`, %`max`) generates %`n`
independent uniformly distributed (@`randomInteger`) integer numbers.

#`randomInteger#` (%`model#`, %`min`, %`max`) creates a vector with the same number of elements
as %`model#`, and fills all elements with independent uniformly distributed integer numbers.
This is shorthand for doing

	\#{randomInteger#} (\`{size} (model#), %`min`, %`max`)

################################################################################
"`randomInteger##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomInteger##` (%`nrow`, %`ncol`, %`min`, %`max`) generates %`nrow` \xx %`ncol`
independent normally distributed (@`randomInteger`) numbers.

#`randomInteger##` (%`model#`, %`min`, %`max`) creates a matrix with the same number of rows and columns
as %`model#`, and fills all cells with independent uniformly distributed integer numbers.
This is shorthand for doing

	\#{randomInteger##} (\`{numberOfRows} (model##), \`{numberOfColumns} (model##), %`min`, %`max`)

################################################################################
"`randomPoisson`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomPoisson` (%`mean`) generates a Poisson-distributed random deviate,
with true mean %`mean`.

################################################################################
"`randomPoisson#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomPoisson#` (%`n`, %`mean`) generates %`n`
independent Poisson-distributed (@`randomPoisson`) numbers.

#`randomPoisson#` (%`model#`, %`mean`) creates a vector with the same number of elements
as %`model#`, and fills all elements with independent Poisson-distributed numbers.
This is shorthand for doing

	\#{randomPoisson#} (\`{size} (model#), %`mean`)

################################################################################
"`randomPoisson##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomPoisson##` (%`nrow`, %`ncol`, %`mean`) generates %`nrow` \xx %`ncol`
independent Poisson-distributed (@`randomPoisson`) numbers.

#`randomPoisson##` (%`model#`, %`mean`) creates a matrix with the same number of rows and columns
as %`model#`, and fills all cells with independent Poisson-distributed numbers.
This is shorthand for doing

	\#{randomPoisson##} (\`{numberOfRows} (model##), \`{numberOfColumns} (model##), %`mean`)

################################################################################
"`randomUniform`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomUniform` (%`min`, %`max`) generates a uniformly distributed random deviate,
with true minimum %`min` (included) and true maximum %`max` (excluded).

################################################################################
"`randomUniform#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomUniform#` (%`n`, %`min`, %`max`) generates %`n`
independent uniformly distributed (@`randomUniform`) numbers.

#`randomUniform#` (%`model#`, %`min`, %`max`) creates a vector with the same number of elements
as %`model#`, and fills all elements with independent uniformly distributed numbers.
This is shorthand for doing

	\#{randomUniform#} (\`{size} (model#), %`min`, %`max`)

################################################################################
"`randomUniform##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`randomUniform##` (%`nrow`, %`ncol`, %`min`, %`max`) generates %`nrow` \xx %`ncol`
independent uniformly distributed (@`randomUniform`) numbers.

#`randomUniform##` (%`model#`, %`min`, %`max`) creates a matrix with the same number of rows and columns
as %`model#`, and fills all cells with independent uniformly distributed numbers.
This is shorthand for doing

	\#{randomUniform##} (\`{numberOfRows} (model##), \`{numberOfColumns} (model##), %`min`, %`max`)

################################################################################
"`random_initializeSafelyAndUnpredictably`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`random_initializeSafelyAndUnpredictably` ( ) undoes the effects of @`random_initializeWithSeedUnsafelyButPredictably`.

################################################################################
"`random_initializeWithSeedUnsafelyButPredictably`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`random_initializeWithSeedUnsafelyButPredictably` (%`seed`) produces a reproducible sequence of random numbers.

################################################################################
"`readFile`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Synopsis
========
#`readFile` (%`filePath$`) reads a number from a text file.

################################################################################
"`readFile$`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Synopsis
========
#`readFile$` (%`filePath$`) reads a whole text file into a string.

################################################################################
"`readLinesFromFile$#`"
© Paul Boersma 2023

A function that can be used in @Scripting.

Synopsis
========
#`readLinesFromFile$#` (%`filePath$`) reads all lines from a text file.

################################################################################
"`rectify`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Synopsis
========
#`rectify` (%`x`) returns zero if %`x` is negative, and %`x` if %`x` is positive.

################################################################################
"`rectify#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Synopsis
========
#`rectify#` (%`v#`) rectifies (@`rectify`) each element of the vector %`v#`.

################################################################################
"`rectify##`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Synopsis
========
#`rectify##` (%`m##`) rectifies (@`rectify`) each cell of the matrix %`m##`.

################################################################################
"`repeat#`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Synopsis
========
#`repeat#` (%`v#`, %`n`) repeats the whole sequence of elements of %`v#` %`n` times.

################################################################################
"`replace$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Synopsis
========
within %`a$`, #`replace$` (%`a$`, %`b$`, %`c$`, %`n`) replaces
the first %`n` occurrences of %`b$` with %`c$`, or all if %`n` = 0.

################################################################################
"`replace_regex$`"
© Paul Boersma 2023

A function that can be used in @Formulas.

Synopsis
========
within %`a$`, #`replace_regex$` (%`a$`, %`b$`, %`c$`, %`n`) replaces
the first %`n` matches of regular expression %`b$`
with the regular expression %`c$`, or all if %`n` = 0.

################################################################################
"`right$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`right$` (%`string$`, %`n`) returns the %`n` last characters in %`string$`.

################################################################################
"`rindex`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`rindex` (%`string$`, %`part$`) returns the last location of the substring %`part$` inside %`string$`, or 0 if not found.

#`rindex` (%`strings$#`, %`s$`) looks up whether and where %`s$` last occurs in %`strings$`.

################################################################################
"`rindex_regex`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`rindex_regex` (%`string$`, %`regex$`) returns the last match of the regular expression %`regex$` inside %`string$`, or 0 if not found.

################################################################################
"`round`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`round` (%`x`) rounds %`x` the nearest integer number.

{
	assert round (1.3) = 1
	assert round (1.7) = 2
	assert round (-1.3) = -1
	assert round (-1.7) = -2
}
Halves are rounded up:
{
	assert round (-2.5) = -2
	assert round (-1.5) = -1
	assert round (-0.5) = 0
	assert round (0.5) = 1
	assert round (1.5) = 2
	assert round (2.5) = 3
}

################################################################################
"`round#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`round#` (%`v#`) rounds (@`round`) each element of the vector %`v#`.

################################################################################
"`round##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`round##` (%`m##`) rounds (@`round`) each cell of the matrix %`m##`.

################################################################################
"`rowSums#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`rowSums#` (%`m##`) computes the sum of the elements of each row of the matrix %`m##`.

################################################################################
"`semitonesToHertz`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`semitonesToHertz` (%`x`) converts a value %`x` along a logarithmic acoustic scale
in semitones (relative to 100 Hz) to an acoustic frequency in Hz,
using the formula 100 exp (%`x` ln 2 / 12).

################################################################################
"`sigmoid`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sigmoid` (%`x`) is defined for all real numbers %`x`
and computes 1 / (1 + %e^^-%`x`^) or 1 − 1 / (1 + %e^%`x`).
The range are real values between 0 (not included) and 1 (not included).

################################################################################
"`sigmoid#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sigmoid#` (%`v#`) computes the @`sigmoid` of each element of the vector %`v#`.

################################################################################
"`sigmoid##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sigmoid##` (%`m##`) computes the @`sigmoid` of each cell of the matrix %`m##`.

################################################################################
"`sin`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sin` (%`x`) computes the sine of %`x`.
{
	Axes: -5, 5, -1.1, 1.1
	Draw inner box
	Draw function: -5, 5, 1000, ~ \#{sin} (x)
	One mark bottom: -3*pi/2, "no", "yes", "yes", "\-m3%\pi/2"
	One mark bottom: -pi, "no", "yes", "yes", "\-m%\pi"
	One mark bottom: -pi/2, "no", "yes", "yes", "\-m%\pi/2"
	One mark bottom: 0, "no", "yes", "yes", "0"
	One mark bottom: pi/2, "no", "yes", "yes", "+%\pi/2"
	One mark bottom: pi, "no", "yes", "yes", "+%\pi"
	One mark bottom: 3*pi/2, "no", "yes", "yes", "+3%\pi/2"
	One mark left: -1, "no", "yes", "yes", "\-m1"
	One mark left: 0, "no", "yes", "yes", "0"
	One mark left: 1, "no", "yes", "yes", "+1"
	Text bottom: "yes", "%x \->"
	Text left: "yes", "##\#{sin}# (%x)"
	Text top: "no", "##just over one and a half periods of a sine wave"
}
This picture illustrates the following properties of the sine function:

- #`sin` (%x) is zero for all values of %x that are integer multiples of %\pi.
- The extrema of #`sin` (%x) are \-m1 and +1.
- #`sin` (%x) is \-m1 or +1 wherever %x is an odd multiple of %\pi/2.
- The function is %periodic with period 2%\pi, i.e., #`sin` (%x+2%\pi) = #`sin` (%x).
- The function is %symmetric around %\pi/2, i.e. #`sin` (%\pi/2\-m%x) = #`sin` (%x).
- The function is %antisymmetric around 0, i.e. #`sin` (\-m%x) = \-m #`sin` (%x);
  in other words, if you take the sine curve and rotate it by 180 degrees
  around the origin (i.e. the point (0, 0)), then you get the same curve again.

################################################################################
"`sin#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
`sin#` (%`v#`) computes the sine (@`sin`) of each element of the vector %`v#`.

################################################################################
"`sin##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
`sin##` (%`m##`) computes the sine (@`sin`) of each cell of the matrix %`m##`.

################################################################################
"`sinc`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sinc` (%`x`) computes the sinus cardinalis: sin (%`x`) / %`x`.

################################################################################
"`sincpi`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sinc` (%`x`) computes the sinus cardinalis (@`sinc`) of %\pi%x: sin (%\pi%x) / (%\pi%x).

################################################################################
"`sinh`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sinh` (%`x`) computes the hyperbolic sine of %`x`, i.e. (%e^%x - %e^^-%x^) / 2.

################################################################################
"`sinh#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sinh#` (%`v#`) computes the hyperbolic sine (@`sinh`) of each element of the vector %`v#`.

################################################################################
"`sinh##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`sinh##` (%`m##`) computes the hyperbolic sine (@`sinh`) of each cell of the matrix %`m##`.

################################################################################
"`size`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`size` (%`v#`) returns the number of elements of the vector %`v`.

################################################################################
"`shuffle#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`shuffle#` (%`v#`) returns a vector with the same elements as %`v`, but in randomized order.

################################################################################
"`softmax#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`softmax#` (%`v#`) converts the elements of the vector %`v` into their softmax values.

################################################################################
"`softmaxPerRow##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`softmaxPerRow##` (%`m##`) converts the elements of each row of the matrix %`m`
into their softmax (@`softmax#`) values (within that row).

################################################################################
"`sort#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`sort#` (%`v#`) returns a vector with the same elements as %`v`, but in sorted order (from low to high).

################################################################################
"`splitByWhitespace$#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`splitByWhitespace$#` (%`string$`) splits a string into inks.

################################################################################
"`sqrt`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`sqrt` (%`x`) takes the square root of %`x`.

{
	assert sqrt (0) = 0
	assert sqrt (1) = 1
	assert sqrt (4) = 2
	assert sqrt (9) = 3
	assert (sqrt (2) - 1.41421356237309504880168872420969807856967187537694807317667973799) < 1e-17
}

################################################################################
"`sqrt#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#sqrt#` (%`v#`) takes the square root (@`sqrt`) of each element of the vector %`v#`.

################################################################################
"`sqrt##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#sqrt##` (%`m##`) takes the square root (@`sqrt`) of each cell of the matrix %`m##`.

################################################################################
"`startsWith`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`startsWith` (%`string$`, %`part$`) determines whether %`string$` starts with %`part$`.

################################################################################
"`stdev`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`stdev` (%`v#`) returns the standard deviation of the elements of the vector %`v#`.

################################################################################
"`stopwatch`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`stopwatch` gives the current value of the timer, which is then immediately reset to zero (note: no parentheses).

################################################################################
"`string$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`string$` (%`number`) formats a number as a string.

################################################################################
"`studentP`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`studentP` (%`t`, %`df`) computes the area under Student’s %T distribution from \-m\oo to %`t`.

################################################################################
"`studentQ`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`studentQ` (%`t`, %`df`) computes the area under Student’s %T distribution from %`t` to +\oo.

################################################################################
"`sum`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sum` (%`v#`) returns the sum of the elements of the vector %`v`.

################################################################################
"`sumOver`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Syntax and semantics
====================
#`sumOver` (%`loopVariable` `to` %`n`, %`numericExpressionOptionallyUsingLoopVariable`)
computes \Si_1^%`n` %`numericExpressionOptionallyUsingLoopVariable`.

################################################################################
"`tan`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`tan` (%`x`) computes the tangent of %`x`, i.e. sin (%x) / cos (%x).

################################################################################
"`tan#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`tan#` (%`v#`) computes the tangent (@`tan`) of each element of the vector %`v#`.

################################################################################
"`tan##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`tan##` (%`m##`) computes the tangent (@`tan`) of each cell of the matrix %`m##`.

################################################################################
"`tanh`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`tanh` (%`x`) computes the hyperbolic tangent of %`x`, i.e. @`sinh` (%x) / @`cosh` (%x).

################################################################################
"`tanh#`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`tanh#` (%`v#`) computes the hyperbolic tangent (@`tanh`) of each element of the vector %`v#`.

################################################################################
"`tanh##`"
© Paul Boersma 2023-06-30

A function that can be used in @@Formulas@.

Synopsis
========
#`tanh##` (%`m##`) computes the hyperbolic tangent (@`tanh`) of each cell of the matrix %`m##`.
#`tanh##` (%`m##`) computes the hyperbolic tangent (@`tanh`) of each cell of the matrix %`m##`.

################################################################################
"`to#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`to#` (%`n`) lists the integers 1 through %`n`.

################################################################################
"`transpose##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`transpose##` (%`m##`) flips the matrix %`m##` along its diagonal.

################################################################################
"`unicode`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`unicode` (%`c$`) returns the Unicode codepoint number that corresponds to the character %`c$`.

################################################################################
"`unicode$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`unicode$` (%`n`) returns the character that corresponds to the Unicode codepoint %`n`.

################################################################################
"`unicodeToBackslashTrigraphs$`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`unicodeToBackslashTrigraphs$` (%`string$`) converts from unicode characters to backslash trigraphs,
e.g. from “\ct” to “\bsct”.

################################################################################
"`writeFile`"
© Paul Boersma 2023

A function that writes its arguments (texts, numbers, vectors and so on) to a new file.
If the file alreasdy existed, anything that was previously present in the file is overwritten.

Examples of use:
================

################################################################################
"`writeFileLine`"
© Paul Boersma 2023

A function that writes its arguments (texts, numbers, vectors and so on, plus a newline symbol, to a new file.
If the file alreasdy existed, anything that was previously present in the file is overwritten.

Examples of use:
================

################################################################################
"`writeInfo`"
© Paul Boersma 2023

A function that writes its arguments to the Info window,
overwriting what was previously present in the Info window.

Examples of use:
================

################################################################################
"`writeInfoLine`"
© Paul Boersma 2023

A function that writes its arguments to the Info window,
overwriting what was previously present in the Info window,
and moving the output cursor to the next line.

Examples of use:
================

################################################################################
"`zero#`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`zero#` (%`n`) returns a vector with %`n` elements that are all 0.0.

################################################################################
"`zero##`"
© Paul Boersma 2023

A function that can be used in @@Formulas@.

Synopsis
========
#`zero##` (%`nrow`, %`ncol`) returns a matrix with %`nrow` \xx %`ncol` cells that are all 0.0.

################################################################################
)~~~"
MAN_PAGES_END

}

/* End of file manual_functions.cpp */
