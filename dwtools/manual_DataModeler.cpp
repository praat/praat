/* manual_DataModeler.cpp
 *
 * Copyright (C) 2014-2025 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
	djmw 20101009 Initial version
*/

#include "ManPagesM.h"

void manual_DataModeler (ManPages me);
void manual_DataModeler (ManPages me) {

MAN_PAGES_BEGIN
R"~~~(
################################################################################
"DataModeler"
© David Weenink, 2024

One of the @@types of objects@ in Praat. A #DataModeler tries to fit %N data points (%x_%i, %y_%i) as lying on a particular kind 
of model function f(%x; #%p) that depends on %M %parameters #%p. 
The parameters #%p are determined by minimizing the chi squared function
%\ci^^2^(#p%)=\Si ((%y__%i_ - %f(%x__%i_, #%p))/%\si__%i_)^^2^,
where the sum is over all %N data points. 

If the individual uncertainties %\si__%i_of the data are not known before hand, the model parameter are determined by assuming that
all uncertainties %\si__$i_ are equal. However an independent assessment of the goodness-of-fit cannot be determined in this case. 
A good guess for %\si^^2^ for this case could be %\si^^2^=\Si  (%y__%i_ - %f(%x__%i_, #%p))^^2^.

Drawing
=======

################################################################################
"DataModeler: Speckle..."
© David Weenink, 2024

Draws the data as (x_i, y_i) points.

################################################################################
"DataModeler: Draw model..."
© David Weenink, 2024

Draws the model function as a continuous curve.


################################################################################
"DataModeler: Draw estimated track..."
© David Weenink, 2024

Draws the model function as a sequence of straight line segments between the estimated values at the data %x_%i values.
If there are many data this curve is almost indistinguishable from a the curve drawn by @@DataModeler: Draw model...|Draw model...@.


################################################################################
"DataModeler: Get coefficient of determination"
© David Weenink, 2024

Get the coefficient of determination (%R^2) of the model.
The %R^2 goodness of fit is a number between 0 and 1 that measures how well a model predicts the data.
A value of 1 means a perfect fit.
If all dependent data, the %y values, have the same variance the %R^2 is simply 1 - %SS__model_/%SS__total_,
where %SS__model_ is the sum of the squared differences between %y's predicted by the model and the actual %y values from the data,
and %SS__total_ the sum of the squared differences between the actual data %y values and the mean of the data.

Because our data points can have different weights we use adapted formula from @@Buse (1973)@.


################################################################################
"DataModeler: Get residual sum of squares"
© David Weenink, 2024

Get the residual sum of squares which is defined as 
EQUATION (\Si (y[i] - ))

################################################################################
"DataModeler: To Table (z-scores)"
© David Weenink, 2024

For each data point get the difference between the model and the data as a z-score.


################################################################################
)~~~"
MAN_PAGES_END

MAN_BEGIN (U"FormantModeler: Get residual sum of squares...", U"djmw", 20140421)
INTRO (U"Get the residual sum of squares for a formant in the selected FormantModeler.")
NORMAL (U"The residual sum of squares, RSS,  is defined as follows")
EQUATION (U"RSS = \\su__i=1_^^n^ (%f__%i_ - %F__%i_)^^2^,")
NORMAL (U"where %f__%i_ is the frequency value of the %i-ith data point, %F__%i_ is the frequency at the i-data point as estimated by the model and %n is the number of data points.")
MAN_END

MAN_BEGIN (U"Formants: Extract smoothest part (constrained)...", U"djmw", 20140424)
NORMAL (U"Extracts the best matching part from the slected @@formant|Formant@s.")
ENTRY (U"Settings")
TERM (U"##Minimum F1 (Hz)")
DEFINITION (U"suppresses models whose average first formant frequency, %f1, is below %%minimumF1% by a factor sqrt (%minimumF1 - %f1 + 1). You can use this constraint to disfavour models with a low average first formant. Sometimes due to the generally high frequency of the /a/ a lower harmonic of the fundamental frequency is taken as a candidate for the first formant. You can suppress these models with this constraint.")
TERM (U"##Maximum F1 (Hz)")
DEFINITION (U"suppresses models whose average first formant frequency, %f1, is above %%maximumF1% by a factor sqrt (%f1 - %%maximumF1% + 1).You can use this constraint to disfavour models in which the first formant is missing. ")
TERM (U"##Minimum F2 (Hz)")
DEFINITION (U"suppresses models whose average second formant frequency, %f2, is below %%minimumF2% by a factor sqrt (%minimumF2 - %f2 + 1). This constraint might be used for high front vowels that normally have a large distance between the first and second formant. ")
TERM (U"##Maximum F2 (Hz)")
DEFINITION (U"suppresses models whose average second formant frequency, %f2, is above %%maximumF2% by a factor sqrt (%f2 - %%maximumF2% + 1). This factor is sometimes necessary to suppress models for high back vowels where the second formant is \"missing\", i.e. where the third formant is playing the role of the second. ")
TERM (U"##Minimum F3 (Hz)")
DEFINITION (U"suppress models whose average third formant frequency, %%f3%, is below %%minimumF3% by a factor sqrt (%%minimumF3% - %%f3% + 1). This constraint might sometimes be usefull to suppress low lying third formants that can occur for /a/-like vowels.")
MAN_END


MAN_BEGIN (U"Buse (1973)", U"djmw", 20140328)
NORMAL (U"A. Buse (1973): \"Goodness of fit in generalized least squares estimation.\", %%The American Statistician% #27: 106\\--108.")
MAN_END

}

/* End of file manual_HMM.cpp */
