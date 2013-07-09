/* manual_dwtools.cpp
 *
 * Copyright (C) 1993-2013 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020313 GPL
 djmw 20130620 Latest modification
*/

#include "ManPagesM.h"
#include "Sound_extensions.h"
#include "TableOfReal_extensions.h"
#include "Table_extensions.h"
#include "Configuration.h"
#include "Discriminant.h"


static TableOfReal getStandardizedLogFrequencyPolsData (int includeLevels) {
	autoTableOfReal me = TableOfReal_createFromPolsData_50males (includeLevels);
	for (long i = 1; i <= my numberOfRows; i++) {
		for (long j = 1; j <= 3; j++) {
			my data[i][j] = log10 (my data[i][j]);
		}
	}
	TableOfReal_standardizeColumns (me.peek());
	TableOfReal_setColumnLabel (me.peek(), 1, L"standardized log (%F__1_)");
	TableOfReal_setColumnLabel (me.peek(), 2, L"standardized log (%F__2_)");
	TableOfReal_setColumnLabel (me.peek(), 3, L"standardized log (%F__3_)");
	if (includeLevels) {
		TableOfReal_setColumnLabel (me.peek(), 4, L"standardized %L__1_");
		TableOfReal_setColumnLabel (me.peek(), 5, L"standardized %L__1_");
		TableOfReal_setColumnLabel (me.peek(), 6, L"standardized %L__3_");
	}
	return me.transfer();
}

static void drawPolsF1F2_log (Graphics g) {
	autoTableOfReal me = getStandardizedLogFrequencyPolsData (0);
	Graphics_setWindow (g, -2.9, 2.9, -2.9, 2.9);
	TableOfReal_drawScatterPlot (me.peek(), g, 1, 2, 0, 0, -2.9, 2.9, -2.9, 2.9, 10, 1, L"+", 1);
}

static void drawPolsF1F2ConcentrationEllipses (Graphics g) {
	autoTableOfReal me = getStandardizedLogFrequencyPolsData (0);
	autoDiscriminant d = TableOfReal_to_Discriminant (me.peek());
	Discriminant_drawConcentrationEllipses (d.peek(), g, 1, 0, NULL, 0, 1, 2, -2.9, 2.9, -2.9, 2.9, 12, 1);
}

static void drawPolsDiscriminantConfiguration (Graphics g) {
	autoTableOfReal me = getStandardizedLogFrequencyPolsData (0);
	autoDiscriminant d = TableOfReal_to_Discriminant (me.peek());
	autoConfiguration c = Discriminant_and_TableOfReal_to_Configuration (d.peek(), me.peek(), 2);
	Configuration_draw (c.peek(), g, 1, 2, -2.9, 2.9, -2.9, 2.9, 0, 1, L"", 1);
}

static void drawBoxPlot (Graphics g) {
	double q25 = 25, q50 = 50, q75 = 60, mean = 45;
	double hspread = q75 - q25, r = 0.05, w = 0.2;
	double lowerInnerFence = q25 - 1.5 * hspread;
	double upperInnerFence = q75 + 1.5 * hspread;
	double upperOuterFence = q75 + 3.0 * hspread;
	double lowerWhisker = lowerInnerFence + 0.1 * hspread;
	double upperWhisker = upperInnerFence - 0.5 * hspread;
	double ymin = lowerWhisker - 0.1 * hspread, ymax = q75 + 4 * hspread;
	double x = 0, dx = 0.01, xar = x + 0.7, xtl = xar + dx;
	double xal1 = x + r + dx, xal2 = x + w + r, y;

	Graphics_setWindow (g, -1, 2, ymin, ymax);
	Graphics_setInner (g);
    Graphics_setTextAlignment (g, Graphics_LEFT, Graphics_HALF);

	Graphics_line (g, x - r, lowerWhisker, x + r, lowerWhisker);
	Graphics_line (g, x, lowerWhisker, x, q25);
	Graphics_line (g, x - w, q25, x + w, q25);
	Graphics_line (g, x - w, q50, x + w, q50);
	Graphics_line (g, x - w, q75, x + w, q75);
	Graphics_line (g, x - w, q25, x - w, q75);
	Graphics_line (g, x + w, q25, x + w, q75);
	Graphics_line (g, x, q75, x, upperWhisker);
	Graphics_line (g, x - r, upperWhisker, x + r, upperWhisker);

	y = q75 + 2.5 * hspread;
	Graphics_text (g, x, y, L"*");
	Graphics_arrow (g, xar, y, xal1, y);
	Graphics_text (g, xtl, y, L"outlier > %%upperInnerFence%");
	y = q75 + 3.5 * hspread;
	Graphics_text (g, x, y, L"o");
	Graphics_arrow (g, xar, y, xal1, y);
	Graphics_text (g, xtl, y, L"outlier > %%upperOuterFence%");
	y = upperOuterFence;
	Graphics_setLineType (g, Graphics_DOTTED);
	Graphics_line (g, -xtl, y, xtl, y);
	Graphics_text (g, xtl, y, L"%%upperOuterFence%");
	y = upperInnerFence;
	Graphics_line (g, -xtl, y, xtl, y);
	Graphics_text (g, xtl, y, L"%%upperInnerFence%");
	Graphics_line (g, x - w, mean, x + w, mean);
	Graphics_setLineType (g, Graphics_DRAWN);
	y = upperWhisker;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, L"%%upperWhisker%");
	y = lowerWhisker;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, L"%%lowerWhisker%");

	y = q75;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, L"%%q75%");
	y = q25;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, L"%%q25%");
	y = q50;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, L"%%q50%");
	y = mean;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, L"%%mean%");
	Graphics_unsetInner (g);
}

static void drawPartionedMatrix (Graphics g) {
	double min = 0, max = 10, x1, x2, y1, y2;
	Graphics_setWindow (g, min, max, min, max);
	x1 = 0; x2 = max; y1 = y2 = 7;
	Graphics_setLineType (g, Graphics_DOTTED);
	Graphics_line (g, x1, y1, x2, y2);
	x1 = x2 = 3; y1 = 0; y2 = max;
	Graphics_line (g, x1, y1, x2, y2);
	Graphics_setLineType (g, Graphics_DRAWN);
	x1 = 1.5; y1 = 7+3/2;
	Graphics_setFontSize (g, 14);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
	Graphics_text (g, x1, y1, L"##S__yy_#");
	x1 = 3 + 7/2;
	Graphics_text (g, x1, y1, L"##S__yx_#");
	y1 = 7/2;
	Graphics_text (g, x1, y1, L"##S__xx_#");
	x1 = 1.5;
	Graphics_text (g, x1, y1, L"##S__xy_#");
}

void manual_dwtools_init (ManPages me);
void manual_dwtools_init (ManPages me) {

MAN_BEGIN (L"AffineTransform", L"djmw", 20010927)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An affine transform is a combination of a linear transformation #%A "
	"and a translation #%t that transforms a vector #%x to a new vector #%y "
	"in the following way:")
FORMULA (L"#%y = #%A x + #%t")
MAN_END

MAN_BEGIN (L"AffineTransform: Invert", L"djmw", 20011008)
INTRO (L"Get the inverse of the selected @AffineTransform object.")
NORMAL (L"The inverse from")
FORMULA (L"#%y = #%A x + #%t")
NORMAL (L"is:")
FORMULA (L"#%x = #%A^^-1^ - #%A^^-1^#%t.")
MAN_END

MAN_BEGIN (L"Band filtering in the frequency domain", L"djmw", 20010404)
INTRO (L"We describe how band filtering in the frequency domain is performed.")
NORMAL (L"We start with a @Sound and end with a filter bank representation of "
	"this sound. We assume a standard analysis context: a sound divided into "
	"frames according to a certain %%window length% and %%time step%. We will "
	"simulate a filterbank with %N filters.")
NORMAL (L"The algorithm for each sound frame proceeds in the following way:")
LIST_ITEM (L"1. Apply a Gaussian window to the sound frame.")
LIST_ITEM (L"2. Convert the windowed frame into a @Spectrum object.")
LIST_ITEM (L"3. Convert the spectral amplitudes to %energy values by squaring "
	"the real and imaginary parts and multiplying by %df, the frequency "
	"distance between two successive frequency points in the spectrum. "
	"Since the Spectrum object only contains positive frequencies, "
	"we have to multiply all energy values, except the first and the last "
	"frequency, by another factor of 2 to compensate for negative frequencies.")
LIST_ITEM (L"4. For each of the %N filters in the filter bank: determine the "
	"inner product of its filter function with the energies as determined in "
	"the previous step. The result of each inner product is the energy in the "
	"corresponding filter.")
LIST_ITEM (L"5. Convert the energies in each filter to power by dividing by "
	"the %%window length%.")
LIST_ITEM (L"6. Correct the power, due to the windowing of the frame, by dividing "
	"by the integral of the %squared windowing function.")
LIST_ITEM (L"7. Convert all power values to %dB's according to 10 * log10 "
	"(%power / 4 10^^-10^).")
MAN_END

MAN_BEGIN (L"Bonferroni correction", L"djmw", 20011107)
NORMAL (L"In general, if we have %k independent significance tests "
	"at the %\\al level, the probability %p that we will get no significant "
	"differences in all these tests is simply the product of the individual "
	"probabilities: (1 - %\\al)^^%k^. "
	"For example, with %\\al = 0.05 and %k = 10 we get %p = 0.95^^10^ = 0.60. "
	"This means, however, we now have a 40\\%  chance that one of these 10 "
	"tests will turn out significant, despite each individual test only being "
	"at the 5\\%  level. "
	"In order to guarantee that the overall significance test is still at the "
	"%\\al level, we have to adapt the significance level %\\al\\'p of the "
	"individual test. ")
NORMAL (L"This results in the following relation between the overall and the "
	"individual significance level:")
FORMULA (L"(1 - %\\al\\'p)^^%k%^ = 1 - %\\al.")
NORMAL (L"This equation can easily be solved for %\\al\\'p:")
FORMULA (L"%\\al\\'p = 1 - (1-%\\al)^^1/%k^,")
NORMAL (L"which for small %\\al reduces to:")
FORMULA (L"%\\al\\'p = %\\al / %k")
NORMAL (L"This is a very simple recipe: If you want an overall significance "
	"level %\\al and you perform %k individual tests, simply divide %\\al "
	"by %k to obtain the significance level for the individual tests.")
MAN_END

MAN_BEGIN (L"box plot", L"djmw", 20111010)
INTRO (L"A box plot provides a simple graphical summary of data. These plots "
	"originate from the work of @@Tukey (1977)@.")
ENTRY (L"Definitions")
NORMAL (L"The following figure shows an annotated box plot.")
PICTURE (5.0, 5.0, drawBoxPlot)
NORMAL (L"To understand the box plot we need the following definitions:")
LIST_ITEM (L"%%q25% = lower quartile, 25\\%  of the data lie below this value")
LIST_ITEM (L"%%q50% = median, 50\\%  of the data lie below this value")
LIST_ITEM (L"%%q75% = upper quartile, 25\\%  of the data lie above this value")
NORMAL (L"The following definitions all depend on these quantiles:")
LIST_ITEM (L"%%hspread% = |%%q75% \\-- %%q25%| (50\\%  interval)")
LIST_ITEM (L"%%lowerOuterFence% = %%q25% \\-- 3.0 * %%hspread% (not in figure)")
LIST_ITEM (L"%%lowerInnerFence% = %%q25% \\-- 1.5 * %%hspread% (not in figure)")
LIST_ITEM (L"%%upperInnerFence% = %%q75% + 1.5 * %%hspread%")
LIST_ITEM (L"%%upperOuterFence% = %%q75% + 3.0 * %%hspread%")
LIST_ITEM (L"%%lowerWhisker% = smallest data value larger then %%lowerInnerFence%")
LIST_ITEM (L"%%upperWhisker% = largest data value smaller then %%upperInnerFence%")
NORMAL (L"The box plot is a summary of the data in which:")
LIST_ITEM (L"\\bu the horizontal lines of the rectangle correspond to "
	" %%q25%, %%q50% and %%q75%, respectively.")
LIST_ITEM (L"\\bu the dotted line corresponds to the mean.")
LIST_ITEM (L"\\bu the outliers outside the %%outerFences% are drawn with an 'o'.")
LIST_ITEM (L"\\bu the outliers in the intervals (%%lowerOuterFence%, %%lowerInnerFence%) "
	"and (%%upperInnerFence%, %%upperOuterFence%) are drawn with an '*'.")
LIST_ITEM (L"\\bu the whisker lines outside the rectangle connect %%q25% with %%lowerWhisker%, and, "
	"%%q75% with %%upperWhisker%, respectively. With no outliers present, the "
	"whiskers mark minimum and/or maximum of the data.")
MAN_END


MAN_BEGIN (L"BarkFilter", L"djmw", 20010404)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type BarkFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%z, %t), expressed "
	"in dB's. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %z__%j_ (on a Bark scale).")
ENTRY (L"Inside a BarkFilter")
NORMAL (L"With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN (L"Bootstrap", L"djmw", 20031103)
INTRO (L"The bootstrap data points are a random sample of size %n "
	"drawn %%with% replacement from the sample (%x__1_,...%x__n_). This "
	"means that the bootstrap data set consists of members of the original "
	"data set, some appearing zero times, some appearing once, some appearing "
	"twice, etc.")
NORMAL (L"More information can be found in @@Efron & Tibshirani (1993)@.")
MAN_END

MAN_BEGIN (L"canonical variate", L"djmw", 20060328)
NORMAL (L"A ##canonical variate# is a new variable (variate) formed by making a linear combination of two "
	"or more variates (variables) from a data set. "
	"A linear combination of variables is the same as a weighted sum of variables. "
	"Because we can in infinitely many ways choose combinations of weights between variables in a data set, "
	"there are also infinitely many canonical variates possible. ")
NORMAL (L"In general additional constraints must be satisfied by the weights to get a meaningful canonical variate. "
	"For example, in @@Canonical correlation analysis|canonical correlation analyis@ a data set is split up into two parts, a %%dependent% and an %%independent% part. "
	"In both parts we can form a canonical variate and we choose weights that maximize the correlation between these canonical variates "
	"(there is an @@TableOfReal: To CCA...|algorithm@ that calculates these weights).")
MAN_END

MAN_BEGIN (L"Categories", L"djmw", 19960918)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Categories represents an ordered collection of categories. Each "
	"category is a simple text string.")
ENTRY (L"Categories commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu ##Create an empty Categories#")
LIST_ITEM (L"\\bu @@FFNet & Pattern: To Categories...@")
NORMAL (L"Viewing and editing:")
LIST_ITEM (L"\\bu @CategoriesEditor")
NORMAL (L"Analysis:")
LIST_ITEM (L"\\bu @@Categories: To Confusion@")
LIST_ITEM (L"\\bu @@Categories: Difference@")
NORMAL (L"Synthesis")
LIST_ITEM (L"\\bu @@Categories: Append@")
LIST_ITEM (L"\\bu ##Categories: Permute...#")
LIST_ITEM (L"\\bu ##Categories: To unique Categories#")
ENTRY (L"Inside a Categories")
NORMAL (L"With @Inspect you will see the following attributes:")
TAG (L"%size")
DEFINITION (L"the number of simple categories.")
TAG (L"%item[]")
DEFINITION (L"the categories. Each category is an object of type #SimpleString.")
MAN_END

MAN_BEGIN (L"Categories: Difference", L"djmw", 19960918)
INTRO (L"A command to compute the difference between two selected @Categories objects.")
ENTRY (L"Behaviour")
NORMAL (L"Each element in the first object is compared with the corresponding "
	"object in the second object according to its compare method. "
	"The number of different %categories will be shown in the @@Info window@.")
MAN_END

MAN_BEGIN (L"Categories: To Confusion", L"djmw", 19960918)
INTRO (L"A command to compute the @Confusion matrix from two selected "
	"@Categories objects.")
ENTRY (L"Algorithm")
NORMAL (L"A confusion matrix is constructed from both #Categories objects in "
	"the following way: The first Categories object is considered the stimulus "
	"Categories and its unique (sorted) categories "
	"form the row indices of the confusion matrix, the unique (sorted) "
	"categories of the second object form the column indices of this matrix.")
NORMAL (L"Next, each element in the first #Categories object is compared with "
	"the corresponding object in the second object and the element in the "
	"confusion matrix addressed by this pair is incremented by 1.")
MAN_END

MAN_BEGIN (L"Categories: Append", L"djmw", 19960918)
INTRO (L"You can choose this command after selecting 2 objects of type @Categories. "
	"A new object is created that contains the second object appended after the first.")
MAN_END

MAN_BEGIN (L"Categories: Edit", L"djmw", 19960918)
INTRO (L"You can choose this command after selecting one #Categories. "
	"A @CategoriesEditor will appear on the screen, with the selected #Categories in it.")
MAN_END

MAN_BEGIN (L"CategoriesEditor", L"djmw", 19960918)
ENTRY (L"An editor for manipulating @Categories.")
NORMAL (L"To make a selection, use the left mouse button.")
NORMAL (L"The Ctrl key extends a selection (discontinuously).")
NORMAL (L"The Shift key extends a selection contiguously.")
MAN_END

MAN_BEGIN (L"CC", L"djmw", 20010219)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"Any object that represents cepstral coefficients as a function of "
	"time.")
MAN_END

MAN_BEGIN (L"CCA", L"djmw", 20020323)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}. ")
NORMAL (L"An object of type CCA represents the @@Canonical correlation "
	"analysis@ of two multivariate datasets.")
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@TableOfReal: To CCA...@")
MAN_END

MAN_BEGIN (L"CCA: Get zero correlation probability...", L"djmw", 20040407)
INTRO (L"Get the probability that for the selected @CCA object the chosen "
	"canonical correlation coefficient is different from zero.")
ENTRY (L"Setting")
TAG (L"##Index")
DEFINITION (L"is the index of the canonical correlation coefficient that "
	"you want to test.")
ENTRY (L"Algorithm")
NORMAL (L"Wilks' statistic: the probability that coefficient \\ro__%index_ "
	"differs from zero is ")
FORMULA (L" %probability = chiSquareQ (\\ci^2, %ndf),")
NORMAL (L"where the %%number of degrees of freedom% parameter equals")
FORMULA (L"%ndf = (%n__y_ - %index +1)(%n__x_ - %index +1)")
NORMAL (L"and the chi-squared parameter is")
FORMULA (L"\\ci^2 = \\--(%numberOfObservations - (%n__y_ + %n__x_ +3)/2) "
	"log (\\La__%index_),")
NORMAL (L"In the formulas above the variables %n__y_ and %n__x_ are the "
	"dimensions of the dependent and the independent data sets whose "
	"canonical correlations have been "
	"obtained, and Wilks' lambda is:")
FORMULA (L"\\La__index_ = \\Pi__%i=%index..min(%ny,%nx)_ (1 \\-- \\ro__%i_^^2^)")
MAN_END

MAN_BEGIN (L"CCA & Correlation: To TableOfReal (loadings)", L"djmw", 20020525)
INTRO (L"Determine from the selected @CCA and @Correlation objects the correlations "
	"of the canonical variables with the original variables. These correlations are "
	"called %%canonical factor loadings%, or also %%structure correlation "
	"coefficients%.")
MAN_END

MAN_BEGIN (L"CCA & Correlation: Get variance fraction...", L"djmw", 20060323)
INTRO (L"Determine from the selected @CCA and @Correlation objects the fraction of the variance "
	"explained by the selected @@canonical variate@ range.")
ENTRY (L"Settings")
TAG (L"##%X or Y")
DEFINITION (L"determines whether you select the dependent (y) or the independent (x) set.")
TAG (L"##Canonical variate range")
DEFINITION (L"determines the canonical variates (or canonical variables).")
ENTRY (L"Remarks")
NORMAL (L"1. In general the variance fractions for a particular canonical variate in the "
	"dependent and in the independent set are not the same.")
NORMAL (L"2. In general, the variance fractions for all canonical variates do not sum to 1.")
ENTRY (L"Algorithm")
NORMAL (L"The formula's can be found on page 170 of @@Cooley & Lohnes (1971)@.")
NORMAL (L"For example, the fraction of the variance explained by the %i^^th^ canonical "
	"variable in the dependent set is:")
FORMULA (L"%%fractionVariance% = ((#y__i_\\'p #R__yy_\\'p #R__yy_ #y__i_) / (#y__i_\\'p #R__yy_ #y__i_)) / %n__%y_,")
NORMAL (L"where #y__%i_ is the eigenvector for dependent canonical variable %i and #R__%%yy%_ is the correlation matrix for the %n__%y_ variables in the dependent set.")
MAN_END

MAN_BEGIN (L"CCA & Correlation: Get redundancy (sl)...", L"djmw", 20060323)
INTRO (L"Determine from the selected @CCA and @Correlation objects the Stewart-Love redundancy for the "
	"selected canonical variates.")
NORMAL (L"The Stewart-Love redundancy for a single @@canonical variate@ is the fraction of variance explained by the selected "
	"canonical variate in a set times the fraction of shared variance between the corresponding canonical variates in the two sets.")
NORMAL (L"The Stewart-Love redundancy for a canonical variate range is the sum of the individual redundancies.")
ENTRY (L"Settings")
TAG (L"##X or Y")
DEFINITION (L"determines whether you select the dependent (y) or the independent (x) set.")
TAG (L"##Canonical variate range")
DEFINITION (L"determines the canonical variates (or canonical variables).")
ENTRY (L"Algorithm")
NORMAL (L"The formula's can be found on page 170 of @@Cooley & Lohnes (1971)@.")
NORMAL (L"For example, the redundancy of the dependent set (y) given the independent set (x) for the %i^^%%th%^ canonical "
	"variate can be expressed as:")
FORMULA (L"%R__%i_(y) = %%varianceFraction%__%i_(y) * \\ro__%i_^2, ")
NORMAL (L"where %%varianceFraction%__%i_(y) is the @@CCA & Correlation: Get variance fraction...|variance fraction@ explained "
	"by the %i^^%%th%^ canonical variate of the dependent set, and \\ro__%i_ is the %i^^%%th%^ canonical correlation coefficient.")
NORMAL (L"The redundancy for the selected canonical variate in the dependent set shows what "
	"fraction of the variance in the %%dependent% set is already \"explained\" by "
	"the variance in the %%independent% set, i.e. this fraction could be considered as redundant.")
NORMAL (L"In the same way we can measure the redundancy of the independent (x) set giving the dependent set (y).")
ENTRY (L"Remark")
NORMAL (L"In general %R__%i_(y) \\=/ %R__%i_(x).")
MAN_END

MAN_BEGIN (L"CCA & TableOfReal: To TableOfReal (loadings)", L"djmw", 20020525)
INTRO (L"Determine from the selected @CCA and @TableOfReal objects the correlations "
	"of the canonical variables with the original variables. These correlations are "
	"called %%canonical factor loadings%, or also %%structure correlation "
	"coefficients%.")
MAN_END

MAN_BEGIN (L"CCA & TableOfReal: To TableOfReal (scores)...", L"djmw", 20040407)
INTRO (L"Determines the scores on the dependent and the independent canonical "
	"variates from the selected @CCA and @TableOfReal objects.")
ENTRY (L"Settings")
TAG (L"##Number of canonical correlations")
DEFINITION (L"determines the dimension, i.e., the number of elements of the resulting "
	"canonical score vectors. The newly created table will have twice this number of "
	"columns because we have calculated score vectors for the dependent and the "
	"independent variates.")
ENTRY (L"Behaviour")
NORMAL (L"The scores on the dependent set are determined as #T__%y_ #Y, where "
	"#T__%y_ is the dependent part in the table and #Y is a matrix with "
	"%numberOfCanonicalCorrelations eigenvectors for the dependent variate.")
NORMAL (L"The scores for the independent variates are then determined in an analogous "
	"way as #T__%x_ #X.")
NORMAL (L"The scores for the dependent data will be in the lower numbered columns, "
	"the scores for the independent part will be in the higher numbered columns of "
	"the newly created object.")
MAN_END


MAN_BEGIN (L"Canonical correlation analysis", L"djmw", 20130502)
INTRO (L"This tutorial will show you how to perform canonical correlation "
       "analysis with  P\\s{RAAT}.")
ENTRY (L"1. Objective of canonical correlation analysis")
NORMAL (L"In canonical correlation analysis we try to find the correlations between "
	"two data sets. One data set is called the %dependent set, the other the "
	"%independent set. In P\\s{RAAT} these two sets must reside into one "
	"@TableOfReal object. The lower numbered columns of this table will then be "
	"interpreted as the dependent part, the rest of the columns as the "
	"independent part. "
	"The dimension of, i.e., the number of columns in, the dependent part may not "
	"exceed the dimension of the independent part.")
NORMAL (L"As an example, we will use the dataset from @@Pols et al. (1973)@ "
	"with the frequencies and levels of the first three formants from the 12 "
	"Dutch monophthongal vowels as spoken in /h_t/ context by 50 male speakers. "
	"We will try to find the canonical correlation between formant frequencies "
	"(the %dependent part) and levels (the %independent part). "
	"The dimension of both groups of variates is 3. "
	"In the introduction of the "
	"@@discriminant analysis@ tutorial you can find how to get these data, "
	"how to take the logarithm of the formant frequency values and how to "
	"standardize them. The following script summarizes:")
CODE (L"pols50m = do (\"Create TableOfReal (Pols 1973)...\", \"yes\")")
CODE (L"do (\"Formula...\", \"if col < 4 then log10 (self) else self endif\")")
CODE (L"do (\"Standardize columns\")")
NORMAL (L"Before we start with the %canonical correlation analysis we will first have "
	"a look at the %Pearson correlations of this table and  "
	"calculate the @Correlation matrix. It is given by:")
CODE (L"       F1     F2     F3     L1     L2     L3")
CODE (L"F1   1     -0.338  0.191  0.384 -0.505 -0.014")
CODE (L"F2  -0.338  1      0.190 -0.106  0.526 -0.568")
CODE (L"F3   0.191  0.190  1      0.113 -0.038  0.019")
CODE (L"L1   0.384 -0.106  0.113  1     -0.038  0.085")
CODE (L"L2  -0.505  0.526 -0.038 -0.038  1      0.128")
CODE (L"L3  -0.014 -0.568  0.019  0.085  0.128  1")
NORMAL (L"The following script summarizes:")
CODE (L"selectObject (pols50m)")
CODE (L"do (\"To Correlation\")")
CODE (L"do (\"Draw as numbers...\", 1, 0, \"decimal\", 3)")
NORMAL (L"The correlation matrix shows that high correlations exist between some "
	"formant frequencies and some levels. For example, the correlation "
	"coefficient between F2 and L2 equals 0.526.")
NORMAL (L"In a canonical correlation analysis of the dataset above, we try "
	"to find the linear "
	"combination %u__1_ of %F__1_, %F__2_ and %F__3_ that correlates maximally "
	"with the linear combination %v__1_ of %L__1_, %L__2_ and %L__3_. "
	"When we have found these %u__1_ and %v__1_ we next try to find a new "
	"combination %u__2_ of the formant frequencies and a new combination "
	"%v__2_ of the levels that have maximum correlation. These %u__2_ and "
	"%v__2_ must be uncorrelated with %u__1_ and %v__1_. "
	"When we express the above with formulas we have:")
FORMULA (L"%u__1_ = %y__11_%F__1_+%y__12_%F__2_ + %y__13_%F__3_")
FORMULA (L"%v__1_ = %x__11_%L__1_+%x__12_%L__2_ + %x__13_%L__3_")
FORMULA (L"\\ro(%u__1_, %v__1_) = maximum, \\ro(%u__2_, %v__2_) = submaximum, ")
FORMULA (L"\\ro(%u__2_, %u__1_) = \\ro (%u__2_, %v__1_) = \\ro (%v__2_, %v__1_) "
	"= \\ro (%v__2_, %u__1_) = 0,")
NORMAL (L"where the \\ro(%u__i_, %v__i_) are the correlations between the "
	"@@canonical variate@s %u__i_ and %v__i_ and the %y__%ij_'s and %x__%ij_'s are"
	" the ##canonical coefficients# for the dependent and the independent "
	"variates, respectively.")
ENTRY (L"2. How to perform a canonical correlation analysis")
NORMAL (L"Select the TableOfReal and choose from the dynamic menu the option "
	"@@TableOfReal: To CCA...|To CCA...@. This command is available in the "
	"\"Multivariate statistics\" action button. We fill out the form and supply "
	"3 for %%Dimension of dependent variate%. The resulting CCA object will bear "
	"the same name as the TableOfReal object. The following script summarizes:")
CODE (L"selectObject (pols50m)")
CODE (L"cca = do (\"To CCA...\", 3)")
ENTRY (L"3. How to get the canonical correlation coefficients")
NORMAL (L"You can get the canonical correlation coefficients by queries of the CCA "
	"object. You will find that the three canonical correlation coefficients, "
	"\\ro(%u__1_, %v__1_), \\ro(%u__2_, %v__2_) and \\ro(%u__3_, %v__3_) are "
	" approximately 0.86, 0.53 and 0.07, respectively. "
	"The following script summarizes:")
CODE (L"cc1 = do (\"Get correlation...\", 1)")
CODE (L"cc2 = do (\"Get correlation...\", 2)")
CODE (L"cc3 = do (\"Get correlation...\", 3)")
CODE (L"writeInfoLine (\"cc1 = \", cc1, \", cc2 = \", cc2, \", cc3 = \", cc3)")
ENTRY (L"4. How to obtain canonical scores")
NORMAL (L"Canonical #scores, also named @@canonical variate@s, are the linear combinations:")
FORMULA (L"%u__%i_ = %y__%i1_%F__1_+%y__%i2_%F__2_ + %y__%i3_%F__3_, and,")
FORMULA (L"%v__%i_ = %x__%i1_%L__1_+%x__%i2_%L__2_ + %x__%i3_%L__3_,")
NORMAL (L"where the index %i runs from 1 to the number of correlation coefficients.")
NORMAL (L"You can get the canonical scores by selecting a CCA object together with "
	"the TableOfReal object and choose "
	"@@CCA & TableOfReal: To TableOfReal (scores)...|To TableOfReal (scores)...@")
NORMAL (L"When we now calculate the ##Correlation# matrix of these canonical variates we "
	"get the following table:")
CODE (L"       u1     u2     u3     v1     v2     v3")
CODE (L"u1     1      .      .     0.860   .      .")
CODE (L"u2     .      1      .      .     0.531   .")
CODE (L"u3     .      .      1      .      .     0.070")
CODE (L"v1    0.860   .      .      1      .      .")
CODE (L"v2     .     0.1     .      .      1      .")
CODE (L"v3     .      .     0.070   .      .      1")
NORMAL (L"The scores with a dot are zero to numerical precision. In this table the "
	"only correlations that differ from zero are the canonical correlations. "
	"The following script summarizes:")
CODE (L"selectObject (cca, pols50m)")
CODE (L"do (\"To TableOfReal (scores)...\", 3)")
CODE (L"do (\"To Correlation\")")
CODE (L"do (\"Draw as numbers if...\", 1, 0, \"decimal\", 2, \"abs(self) > 1e-14\")")
ENTRY (L"5. How to predict one dataset from the other")
NORMAL (L"@@CCA & TableOfReal: Predict...@")
NORMAL (L"Additional information can be found in @@Weenink (2003)@.")
MAN_END

MAN_BEGIN (L"CCA & TableOfReal: Predict...", L"djmw", 20020503)
INTRO (L"")
MAN_END

MAN_BEGIN (L"Chebyshev polynomials", L"djmw", 19990620)
INTRO (L"The Chebyshev polynomials %T__%n_(%x) of degree %n are special orthogonal polynomial functions "
	"defined on the domain [-1, 1].")
NORMAL (L"Orthogonality:")
FORMULA (L"__-1_\\in^^1^ %W(%x) %T__%i_(%x) %T__%j_(%x) %dx = \\de__%ij_")
FORMULA (L"%W(%x) = (1 \\-- %x^^2^)^^\\--1/2^   (-1 < x < 1)")
NORMAL (L"They obey certain recurrence relations:")
FORMULA (L"%T__%n_(%x) = 2 %x %T__%n-1_(%x) \\-- %T__%n-2_(%x)")
FORMULA (L"%T__0_(%x) = 1")
FORMULA (L"%T__1_(%x) = %x")
MAN_END

MAN_BEGIN (L"ChebyshevSeries", L"djmw", 19990620)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type ChebyshevSeries represents a linear combination of @@Chebyshev polynomials@ "
	"%T__%k_(%x).")
FORMULA (L"ChebyshevSeries (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %T__%k_(%x)")
MAN_END

MAN_BEGIN (L"ChebyshevSeries: To Polynomial", L"djmw", 19990620)
INTRO (L"A command to transform the selected @ChebyshevSeries object into a @@Polynomial@ object.")
NORMAL (L"We find polynomial coefficients %c__%k_ such that")
FORMULA (L"\\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x^^%k^ = \\Si__%k=1.."
	"%numberOfCoefficients_ %l__%k_ %T__%k_(%x)")
NORMAL (L"We use the recurrence relation for @@Chebyshev polynomials@ to calculate these coefficients.")
MAN_END

MAN_BEGIN (L"ClassificationTable", L"djmw", 19990525)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"An object of type ClassificationTable represents the result of a classification experiment. "
	"The numbers in a row show how well a particular input matches the classes represented by the "
	"column labels. The higher the number the better the match.")
MAN_END

MAN_BEGIN (L"concentration ellipse", L"djmw", 20071113)
INTRO (L"The percentage of bivariate normally distributed data covered by an ellipse "
	"whose axes have a length of %numberOfSigmas\\.c\\si can be obtained by integration of the p.d.f. "
	"over an elliptical area. This results in the following equation as can be "
	"verified from equation 26.3.21 in @@Abramowitz & Stegun (1970)@:")
FORMULA (L"%percentage = (1 - exp (-%numberOfSigmas^^2^/2))\\.c 100\\% ,")
NORMAL (L"where the %numberOfSigmas is the radius of the \"ellipse\":")
FORMULA (L"(%x/%\\si__x_)^2 + (%y/%\\si__y_)^2 = %numberOfSigmas^2.")
NORMAL (L"The %numberOfSigmas = 1 ellipse covers 39.3\\% , "
	"the %numberOfSigmas = 2 ellipse covers 86.5\\%  and "
	"the %numberOfSigmas = 3 ellipse covers 98.9\\%  of the data.")
NORMAL (L"From the formula above we can show that if we want to cover %p percent of the data, we have to "		"chose %numberOfSigmas as:")
FORMULA (L"%numberOfSigmas = \\Vr(-2 ln(1-%p/100)).")
NORMAL (L"For covering 95\\% of the data we calculate %numberOfSigmas = 2.45.")
MAN_END

MAN_BEGIN (L"confidence interval", L"djmw", 20011105)
INTRO (L"The confidence interval gives an estimated range of values which "
	"is likely to include an unknown population parameter. "
	"The estimated range is calculated from a given set of observations.")
ENTRY (L"Examples")
NORMAL (L"At the \\al level of significance a two sided confidence interval "
	"for the true mean \\mu for normally distributed data with mean %%mean% and "
	"known standard deviation %\\si can be constructed as:")
FORMULA (L"%%mean% - %z__\\al/2_ \\si / \\Vr%N \\<_ \\mu \\<_ "
	"%%mean% + %z__\\al/2_ \\si / \\Vr%N,")
NORMAL (L"where %z__\\al/2_ = invGaussQ (\\al/2) and %N is the number of observations.")
NORMAL (L"If the standard deviation is %not known, we have to estimate its value (%s) "
	"from the data and the formula above becomes:")
FORMULA (L"%%mean% - %t__%%\\al/2;N%_ %s / \\Vr%N \\<_ \\mu \\<_ "
	"%%mean% + %t__%%\\al/2;N%_ %s / \\Vr%N,")
NORMAL (L"where %t__%%\\al/2;N%_ = invStudentQ (%\\al/2, %N-1).")
NORMAL (L"For %\\al=0.05 and %N=20 we get %z__0.025_=1.96 and %t__0.025;20_=2.039. "
	"This shows that when we have to estimate the standard deviation from the data, "
	"the confidence interval is wider than when the standard deviation is known "
	"beforehand.")
MAN_END

MAN_BEGIN (L"confidence level", L"djmw", 20011105)
NORMAL (L"The confidence level is the probability value 1-\\al associated "
	"with a @@confidence interval@, where \\al is the level of significance. "
	"It can also be expressed as a percentage 100(1-\\al)\\%  and is than "
	"sometimes called the %%confidence coefficient%.")
MAN_END

MAN_BEGIN (L"Confusion", L"djmw", 20110517)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"An object of type Confusions represents a confusion matrix, with "
	"stimuli as row labels and responses as column labels. The entry at "
	"position [%i][%j] represents the number of times response %j "
	"was given to the stimulus %i.")
ENTRY (L"Creating a Confusion from data in a text file")
NORMAL (L"Suppose you have two objects A and B. "
	"In one way or another, you have acquired the following  "
	"confusions: %\\de__%AA_ = 6, %\\de__%AB_ = 2 , %\\de__%BA_ = 1, "
	"and %\\de__%BB_ = 7.")
NORMAL (L"You can create a simple text file like the following:")
CODE (L"\"ooTextFile\"  ! to make Praat recognize your file")
CODE (L"\"Confusion\" ! The line that tells Praat about the contents")
CODE (L"2     \"A\"  \"B\"  ! Number of columns, and column labels")
CODE (L"2                   ! Number of rows")
CODE (L"\"A\"    6    2     ! Row label A, A-A value, A-B value")
CODE (L"\"B\"    1    7     ! Row label B, B-A value, B-B value")
NORMAL (L"This text file can be read with the @@Read from file...@ command. ")
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Categories: To Confusion")
LIST_ITEM (L"\\bu @@Create simple Confusion...")
NORMAL (L"Drawing")
LIST_ITEM (L"\\bu ##Draw as numbers...")
LIST_ITEM (L"\\bu ##Draw as squares...")
NORMAL (L"Query")
LIST_ITEM (L"\\bu @@Confusion: Get fraction correct|Get fraction correct")
LIST_ITEM (L"\\bu @@Confusion: Get stimulus sum...")
LIST_ITEM (L"\\bu @@Confusion: Get response sum...")
LIST_ITEM (L"\\bu ##Get grand sum")
NORMAL (L"Modification")
LIST_ITEM (L"\\bu ##Formula...")
LIST_ITEM (L"\\bu @@Confusion: Increase...")
NORMAL (L"Grouping")
LIST_ITEM (L"\\bu @@Confusion: Group stimuli...@")
LIST_ITEM (L"\\bu @@Confusion: Group responses...@")
NORMAL (L"Analysis:")
LIST_ITEM (L"\\bu @@Confusion: To Similarity...@")
LIST_ITEM (L"\\bu @@Confusion: To Dissimilarity (pdf)...@")
ENTRY (L"Inside a Confusion")
NORMAL (L"With @Inspect you will see the following attributes:")
TAG (L"%numberOfRows")
DEFINITION (L"the number of stimuli.")
TAG (L"%numberOfColumns")
DEFINITION (L"the number of responses.")
TAG (L"%rowLabels")
DEFINITION (L"the names of the stimuli.")
TAG (L"columnLabels")
DEFINITION (L"the names of the responses.")
MAN_END

MAN_BEGIN (L"Create simple Confusion...", L"djmw", 20130410)
INTRO (L"Creates a square @@Confusion|confusion matrix@ with equal stimulus labels and response labels.")
ENTRY (L"Example")
NORMAL (L"The command ##do (\"Create simple Confusion...\", \"simple\", \"u i a\")# results in the following Confusion:")
CODE (L"     u    i    a   ! The response labels")
CODE (L"u    0    0    0   ! Responses on stimulus  u,")
CODE (L"i    0    0    0   ! Responses on stimulus  i")
CODE (L"a    0    0    0   ! Responses on stimulus  a")
MAN_END

MAN_BEGIN (L"Confusion: Increase...", L"djmw", 20130410)
INTRO (L"Increases the contents of the corresponding cell in the selected @@Confusion@ by one.")
ENTRY (L"Settings")
TAG (L"##Stimulus# and ##Response#")
DEFINITION (L"define the cell whose value will be increased by one.")
ENTRY (L"Example")
NORMAL (L"Given the following Confusion:")
CODE (L"       u    i    a   ! The response labels")
CODE (L" u     6    2    1   ! Responses on stimulus  u,")
CODE (L" i     3    4    2   ! Responses on stimulus  i")
CODE (L" a     1    4    4   ! Responses on stimulus  a")
NORMAL (L"The command ##do (\"Increase...\", \"u\", \"i\")# results in:")
CODE (L"       u    i    a   ! The responses")
CODE (L" u     6    3    1   ! Responses on stimulus  u,")
CODE (L" i     3    4    2   ! Responses on stimulus  i")
CODE (L" a     1    4    4   ! Responses on stimulus  a")
MAN_END

MAN_BEGIN (L"Confusion: Group...", L"djmw", 20130410)
INTRO (L"Groups a number of stimuli and responses into one new category.")
ENTRY (L"Settings")
TAG (L"##Stimuli & Responses")
DEFINITION (L"defines the labels that will be grouped.")
TAG (L"##New label")
DEFINITION (L"defines the new label for the grouped labels.")
TAG (L"##New label position")
DEFINITION (L"the row/column number for the new group label.")
ENTRY (L"Example")
NORMAL (L"Given the following selected Confusion:")
CODE (L"       u    i    a   ! The response labels")
CODE (L" u     6    2    1   ! Responses on stimulus  u,")
CODE (L" i     3    4    2   ! Responses on stimulus  i")
CODE (L" a     1    4    4   ! Responses on stimulus  a")
NORMAL (L"After the command ##do (\"Group stimuli...\", \"u i\", \"high\", 0)#, the new Confusion will be:")
CODE (L"         high   a   ! The new response labels")
CODE (L" high     15    3   ! Responses on group %%high%")
CODE (L" a         5    4   ! Responses on stimulus  a")
NORMAL (L"Instead after the command ##do (\"Group stimuli...\", \"u i\", \"high\", 2)#, the new Confusion will be:")
CODE (L"        a   high  ! The new response labels")
CODE (L" a      4     5   ! Responses on stimulus  a")
CODE (L" high   3    15   ! Responses on group %%high%")
MAN_END

MAN_BEGIN (L"Confusion: Group stimuli...", L"djmw", 20130410)
INTRO (L"Groups a number of stimuli into one new category.")
ENTRY (L"Settings")
TAG (L"##Stimuli")
DEFINITION (L"defines the stimuli that will be grouped.")
TAG (L"##New label")
DEFINITION (L"defines the new label for the grouped stimuli.")
TAG (L"##New label position")
DEFINITION (L"the row number for the new group label.")
ENTRY (L"Example")
NORMAL (L"Given the following selected Confusion:")
CODE (L"       u    i    a   ! The response labels")
CODE (L" u     6    2    1   ! Responses on stimulus  u,")
CODE (L" i     3    4    2   ! Responses on stimulus  i")
CODE (L" a     1    4    4   ! Responses on stimulus  a")
NORMAL (L"After the command ##do (\"Group stimuli...\", \"u i\", \"high\", 1)#, the new Confusion will be:")
CODE (L"          u    i    a   ! The response labels")
CODE (L" high     9    6    3   ! Responses on stimulus group %%high%,")
CODE (L" a        1    4    4   ! Responses on stimulus  a")
MAN_END

MAN_BEGIN (L"Confusion: Group responses...", L"djmw", 20130410)
INTRO (L"Groups a number of responses into one new category.")
ENTRY (L"Settings")
TAG (L"##Responses")
DEFINITION (L"defines the responses that will be grouped.")
TAG (L"##New label")
DEFINITION (L"defines the new label for the grouped responses.")
TAG (L"##New label position")
DEFINITION (L"the column number for the new group label.")
ENTRY (L"Example")
NORMAL (L"Given the following selected Confusion:")
CODE (L"       u    i    a   ! The response labels")
CODE (L" u     6    2    1   ! Responses on stimulus  u,")
CODE (L" i     3    4    2   ! Responses on stimulus  i")
CODE (L" a     1    4    4   ! Responses on stimulus  a")
NORMAL (L"After the command ##do (\"Group responses...\", \"a i\", \"front\", 1)#, the new Confusion will be:")
CODE (L"   front    i    ! The new response labels")
CODE (L" u     7    2    ! Responses on stimulus  u,")
CODE (L" i     5    4    ! Responses on stimulus  i")
CODE (L" a     5    4    ! Responses on stimulus  a")
MAN_END

MAN_BEGIN (L"Confusion: Get stimulus sum...", L"djmw", 20130410)
INTRO (L"Returns the number of responses for the chosen stimulus (the sum of all the numbers in the row with this stimulus label). ")
ENTRY (L"Example")
NORMAL (L"Given the following selected Confusion:")
CODE (L"       u    i    a   ! The response labels")
CODE (L" u     6    2    1   ! Responses on stimulus  u,")
CODE (L" i     3    4    2   ! Responses on stimulus  i")
CODE (L" a     1    4    4   ! Responses on stimulus  a")
NORMAL (L"The command ##do (\"Get stimulus sum...\", \"a\")# will return the number 9.")
MAN_END

MAN_BEGIN (L"Confusion: Get response sum...", L"djmw", 20130410)
INTRO (L"Returns the number of times the chosen response was given (the sum of all the numbers in the column with this response label).")
ENTRY (L"Example")
NORMAL (L"Given the following selected Confusion:")
CODE (L"       u    i    a   ! The response labelss")
CODE (L" u     6    2    1   ! Responses on stimulus  u,")
CODE (L" i     3    4    2   ! Responses on stimulus  i")
CODE (L" a     1    4    4   ! Responses on stimulus  a")
NORMAL (L"The command ##do (\"Get response sum...\", \"a\")# will return the number 7.")
MAN_END

MAN_BEGIN (L"Confusion: Condense...", L"djmw", 20130410)
INTRO (L"Groups row and column labels of the selected @Confusion object in "
	"order to reduce its dimension. ")
ENTRY (L"Settings")
TAG (L"##Search")
DEFINITION (L"the pattern to match.")
TAG (L"##Replace")
DEFINITION (L"the pattern that replaces the match(es).")
TAG (L"##Replace limit")
DEFINITION (L"limits the maximum number of times that a match/replace cycle "
	"may occur within each label.")
TAG (L"##Search and replace are")
DEFINITION (L"defines whether the search and replace strings are taken "
	"literally or as a @@Regular expressions|regular expression@.")
ENTRY (L"Behaviour")
NORMAL (L"First all row and column labels are changed according to the search "
	"and replace specification. Next all rows or columns that have the same "
	"labels are summed. ")
MAN_END

MAN_BEGIN (L"Confusion: Get fraction correct", L"djmw", 20000225)
INTRO (L"A @@query@ to ask the selected @Confusion matrix for the fraction of "
	"correct classifications.")
NORMAL (L"The \"fraction correct\" is defined as the quotient of the number "
	"of correct classifications and the sum of the entries in the matrix.")
NORMAL (L"Correct classifications have identical row and column labels.")
MAN_END

MAN_BEGIN (L"Confusion: To TableOfReal (marginals)", L"djmw", 20011031)
INTRO (L"A new @TableOfReal object is created from the selected @Confusion "
	"object with one extra row and column. ")
NORMAL (L"The first element of the extra row will contain the sum of the "
	"confusions in the the first %column, the first element of the extra "
	"column will contain the sum of the confusions in the the first %row, "
	"etc... The bottom-right element will contain the sum of all confusions.")
MAN_END

MAN_BEGIN (L"Correlation", L"djmw", 19990105)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Correlation represents the correlation coefficients "
	"of a multivariate data set.")
MAN_END

MAN_BEGIN (L"Correlation: Confidence intervals...", L"djmw", 20040407)
INTRO (L"Calculates @@confidence interval@s for the correlation coefficients "
	"from the selected @Correlation object(s) and saves these intervals in a "
	"new @TableOfReal object.")
ENTRY (L"Settings")
TAG (L"##Confidence level")
DEFINITION (L"the @@confidence level@ you want for the confidence intervals.")
TAG (L"##Number of tests")
DEFINITION (L"determines the @@Bonferroni correction@ for the significance "
	"level. If the default value (zero) is chosen, it will be set equal "
	"to the number of correlations involved (a matrix of dimension %n "
	"has %n\\.c(%n-1)/2 correlations).")
TAG (L"##Approximation")
DEFINITION (L"defines the approximation that will be used to calculate the "
	"confidence intervals. It is either Fisher's z transformation or Ruben's "
	"transformation. According to @@Boomsma (1977)@, Ruben's approximation is "
	"more accurate than Fisher's.")
ENTRY (L"Algorithm")
NORMAL (L"We obtain intervals by the large-sample conservative multiple tests "
	"with Bonferroni inequality and the Fisher or Ruben transformation. "
	"We put the upper values of the confidence intervals in the upper "
	"triangular part of the matrix and the lower values of the confidence "
	"intervals in lower triangular part of the resulting TableOfReal object.")
NORMAL (L"In %%Fisher's approximation%, for each element %r__%ij_ of the "
	"correlation matrix the confidence interval is:")
FORMULA (L"#[ tanh (%z__%ij_ - %z__%\\al\\'p_ / \\Vr(%N - 3)) , "
	"tanh (%z__%ij_ + %z__%\\al\\'p_ / \\Vr(%N - 3)) #],")
NORMAL (L"where %z__%ij_ is the Fisher z-transform of the correlation %r__%ij_:")
FORMULA (L"%z__%ij_ = 1/2 ln ((1 + %r__%ij_) / (1 - %r__%ij_)), ")
NORMAL (L"%z__%\\al\\'p_ the Bonferroni corrected %z-value "
	"%z__%\\al/(2\\.c%numberOfTests)_, ")
FORMULA (L"%\\al = 1 - %confidenceLevel,")
NORMAL (L"and %N the number of observations that the correlation matrix is "
	"based on.")
NORMAL (L"In %%Ruben's approximation% the confidence interval for element %r__%ij_ "
	"is:")
FORMULA (L"#[ %x__1_ / \\Vr(1 - %x__1_^2), %x__2_ / \\Vr(1 - %x__2_^2) #]")
NORMAL (L"in which %x__1_ and %x__2_ are the smallest and the largest root from")
FORMULA (L"%a %x^^2^ + %b %x + %c = 0, with")
FORMULA (L"%a = 2%N - 3 - %z__%\\al\\'p_^^2^")
FORMULA (L"%b = - 2 %r\\'p \\Vr((2%N - 3)(2%N - 5))")
FORMULA (L"%c = (2%N - 5 - %z__%\\al\\'p_^^2^) %r\\'p^^2^ - 2%z__%\\al\\'p_^^2^, and")
FORMULA (L"%r\\'p = %r__%ij_ / \\Vr(1 - %r__%ij_^2),")
MAN_END

MAN_BEGIN (L"Covariance", L"djmw", 19990105)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Covariance represents the sums of squares and cross "
	"products of a multivariate data set divided by the number of observations.")
NORMAL (L"An object of type Covariance contains the same attributes as an object of "
	"type @SSCP.")
NORMAL (L"Since an object of type Covariance contains the mean values (the "
	"centroids), the covariances as well as the number of observations it has "
	"all the information necessary to be the subject of all kinds of statistical "
	"tests on means and variances.")
MAN_END

MAN_BEGIN (L"Create simple Covariance...", L"djmw", 20101125)
INTRO (L"Create a @@Covariance@ matrix with its centroid.")
ENTRY (L"Settings")
TAG (L"##Covariances")
DEFINITION (L"define the covariances. Because a covariance matrix is a symmetric matrix, only the upper triangular "
	"part of the matrix has to be input (row-wise). If your covariance matrix is "
	"of dimension %d, your input needs %d(%d+1)/2 elements. The first %d input elements are the elements of the first "
	"row of the covariance matrix, the next %d-1 input elements are for the second row, then %d-2 for the third row, etc.")
TAG (L"##Centroid")
DEFINITION (L"defines the centroid. ")
TAG (L"##Number of observations")
DEFINITION (L"defines the number of observations. ")
MAN_END

MAN_BEGIN (L"Covariance: Set value...", L"djmw", 20101124)
INTRO (L"Input @@Covariance@ matrix cell values.")
ENTRY (L"Constraints on input values")
TAG (L"A covariance matrix is a %%symmetric% matrix: values input at cell [%i,%j] will be automatically input at "
	"cell [%j,%i] too.")
TAG (L"All values on the diagonal must be positive numbers.")
TAG (L"The absolute value of an off-diagonal element at cell [%i,%j] must be smaller than the corresponding diagonal "
	"elements at cells [%i,%i] and [%j,%j].")
MAN_END

MAN_BEGIN (L"Covariance: Difference", L"djmw", 20090624)
INTRO (L"You can choose this command after selecting two objects of type @Covariance. ")
NORMAL (L"We test the hypothesis that the samples that gave rise to the two "
	"covariance matrices #%M__1_ and #%M__2_, have equal covariances. "
	"The test statistic is %L\\'p which is distributed as "
	"a \\ci^2 variate with %p(%p+1)/2 degrees of freedom.")
FORMULA (L"%L\\'p = %L \\.c (1 \\-- (2%p + 1 \\-- 2 / (%p + 1)) / (6 \\.c ( %N \\-- 1))),")
NORMAL (L"where, ")
FORMULA (L"%L = (%N \\-- 1) \\.c (ln determinant (#%M__1_) \\-- ln determinant "
	"(#%M__2_)) + trace (#%M__2_ \\.c #%M__1_^^\\--1^) \\-- %p), ")
NORMAL (L"%p is dimension of covariance matrix and "
	"%N is the number of observations underlying the covariance matrix. ")
NORMAL (L"For more details on this test, see e.g. page 292 of @@Morrison (1990)@.")
MAN_END

MAN_BEGIN (L"Covariance: Get significance of one mean...", L"djmw", 20040407)
INTRO (L"Gets the level of significance for one mean from the selected "
	"@Covariance object being different from a hypothesized mean.")
ENTRY (L"Settings")
TAG (L"##Index")
DEFINITION (L"the position of the element in the means vector (centroid) that "
	"you want to test. ")
TAG (L"##Value")
DEFINITION (L"the hypothesized mean %\\mu (see below).")
ENTRY (L"Behaviour")
NORMAL (L"This is the standard test on means when the variance is unknown. "
	"The test statistic is")
FORMULA (L"%t = (%mean - %\\mu) \\Vr%(N / %s^2),")
NORMAL (L"which has the Student %t distribution with %ndf = %N-1 degrees of freedom.")
NORMAL (L"In the formulas above, %mean is the element of the mean vector at "
	"position %index, %\\mu is the hypothesized mean, "
	"%N is the number of observations, %s^2 "
	"is the variance at position [%index][%index] in the covariance matrix.")
NORMAL (L"The returned probability %p is the %%two-sided% probability")
FORMULA (L"%p = 2 * studentQ (%t, %ndf)")
NORMAL (L"A low probability %p means that the difference is significant.")
MAN_END

MAN_BEGIN (L"Covariance: Get fraction variance...", L"djmw", 20040407)
INTRO (L"A command to ask the selected @Covariance object for the fraction "
	"of the total variance that is accounted for by the selected dimension(s).")
ENTRY (L"Settings")
TAG (L"##From dimension#, ##To dimension#")
DEFINITION (L"define the range of components. By choosing both numbers equal, "
	"you get the fraction of the variance \"explained\" by that dimension.")
ENTRY (L"Details")
NORMAL (L"The total variance is the sum of the diagonal elements of the covariance "
	"matrix #C, i.e., its trace. "
	"The fraction is defined as:")
FORMULA (L"\\Si__%i=%from..%to_ %%C__ii_% / \\Si__%i=1..%numberOfRows_ %%C__ii_%")
MAN_END

MAN_BEGIN (L"Covariance: Get significance of means difference...", L"djmw", 20040407)
INTRO (L"Gets the level of significance for the %difference of two means "
	"from the selected @Covariance object being different from a hypothesized "
	"value.")
ENTRY (L"Settings")
TAG (L"##Index1#, ##Index2#")
DEFINITION (L"the positions of the two elements of the means vector whose "
	"difference is compared to the hypothesized difference.")
TAG (L"##Value")
DEFINITION (L"the hypothesized difference (%\\mu).")
TAG (L"##Paired samples")
DEFINITION (L"determines whether we treat the two means as being dependent. ")
TAG (L"##Equal variances")
DEFINITION (L"determines whether the distribution of the difference of the means "
	"is a Student t-distribution (see below).")
ENTRY (L"Behaviour")
NORMAL (L"This is Student's t-test for the significance of a difference of means. "
	"The test statistic is:")
FORMULA (L"%t = (%mean__1_ - %mean__2_ - %\\mu) \\Vr (%N / %s^2) with %ndf "
	"degrees of freedom.")
NORMAL (L"In the formula above %mean__1_ and %mean__2_ are the elements of the "
	"means vector, %\\mu is the hypothesized difference and %N is the number of "
	"observations. The value that we use for the (combined) variance %s^2 is:")
FORMULA (L"%s^2 = %var__1_ + %var__2_ - 2 * %covar__12_,")
NORMAL (L"when the samples are %paired, and ")
FORMULA (L"%s^2 = %var__1_ + %var__2_ ")
NORMAL (L"when they are not.")
NORMAL (L"The %var__1_ and %var__2_ are the variance components for "
	"%mean__1_ and %mean__2_, respectively, and %covar__12_ is their covariance."
	" When we have %%paired samples% we assume that the two variances are "
	"not independent and their covariance is subtracted, otherwise their "
	"covariance is not taken into account. Degrees of freedom parameter %ndf "
	"usually equals 2(%N-1). ")
NORMAL (L"If the two variances are significantly different, the statistic %t "
	"above is only %approximately distributed as Student's %t with "
	"degrees of freedom equal to:")
FORMULA (L"%ndf = (%N-1) \\.c (%var__1_ + %var__2_)^2 / (%var__1_^2 + "
	"%var__2_^2).")
NORMAL (L"The returned probability %p will be the %%two-sided% probability")
FORMULA (L"%p = 2 * studentQ (%t, %ndf)")
NORMAL (L"A low probability %p means that the difference is significant.")
MAN_END

MAN_BEGIN (L"Covariance: Get significance of one variance...", L"djmw", 20040407)
INTRO (L"Gets the probability for one variance from the selected "
	"@Covariance object being different from a hypothesized variance.")
ENTRY (L"Settings")
TAG (L"##Index")
DEFINITION (L"the position of the variance element.")
TAG (L"##Hypothesized variance")
DEFINITION (L"the hypothesized variance %\\si^2")
ENTRY (L"Behaviour")
NORMAL (L"The test statistic")
FORMULA (L"%\\ci^2 = (%N-1)%s^2 / %\\si^2,")
NORMAL (L"is distributed as a chi-squared variate with %ndf = %N-1 degrees of freedom.")
NORMAL (L"The returned probability %p will be ")
FORMULA (L"%p = chiSquareQ (%\\ci^2, %ndf)")
MAN_END

MAN_BEGIN (L"Covariance: Get significance of variance ratio...", L"djmw", 20040407)
INTRO (L"Gets the probability for the ratio of two variances "
	"from the selected @Covariance object being different from a hypothesized "
	"ratio.")
ENTRY (L"Settings")
TAG (L"##Index1#, ##Index2#")
DEFINITION (L"determine the variances.")
TAG (L"##Hypothesized ratio")
DEFINITION (L"the hypothesized ratio %F.")
ENTRY (L"Behaviour")
NORMAL (L"The test statistic")
FORMULA (L"%f = %s__1_^2 / %s__2_^2 / %ratio")
NORMAL (L"is distributed as Fisher's F distribution with %ndf__1_ = %N-1 and "
	"%ndf__2_ = %N-1 degrees of freedom for the numerator and denominator terms, "
	"respectively.")
NORMAL (L"The returned probability %p will be the %%two-sided% probability")
FORMULA (L"%p = 2 * fisherQ (%f, %ndf__1_, %ndf__2_)")
NORMAL (L"If %s__2_^2 > %s__1_^2 we use 1/%f to determine the probability.")
MAN_END

MAN_BEGIN (L"Covariances: Report multivariate mean difference...", L"djmw", 20090627)
INTRO (L"Reports the probability that the two multivariate means of the selected @@Covariance@s are equal.")
ENTRY (L"Settings")
TAG (L"##Covariances are equal")
DEFINITION (L"determines whether the test is performed as if the two covariance matrices are equal or not.")
ENTRY (L"Algorithm")
NORMAL (L"For equal covariance matrices the test is via Hotelling's T^^2^ as described in @@Morrison (1990)|Morrison (1990,@ page 141). "
	"The test statistic is %F = (%N__1_+%N__2_-%p-1)/((%N__1_+%N__2_-2)%p)\\.c T^^2^, with %p and %N__1_+%N__2_-%p-1 degrees of freedom.")
NORMAL (L"If the covariance matrices are not equal, we apply a correction on the number of degrees of freedom as "
	"proposed by @@Krishnamoorthy & Yu (2004)@. The test statistic in this case is %F = (\\nu-%p+1)/(%p\\nu)\\.c T^^2^, "
	"with %p and \\nu degrees of freedom. Here \\nu is a corrected number of degrees of freedom. ")
NORMAL (L"(The test for unequal covariances simplifies to Welch's approximate solution for the univariate t-test with unequal variances.) ")
MAN_END

MAN_BEGIN (L"Covariances: Report equality", L"djmw", 20090701)
INTRO (L"Reports the probability that the selected @@Covariance@ matrices are equal.")
NORMAL (L"We use the Bartlett test and the Wald test. According to @@Schott (2001)@, "
	"both tests are overly sensitive to violations of normality.")
MAN_END

MAN_BEGIN (L"Covariance: To TableOfReal (random sampling)...", L"djmw", 20101101)
INTRO (L"Generate a @TableOfReal object by random sampling from a multi-variate "
	"normal distribution whose @Covariance matrix is the selected object.")
ENTRY (L"Setting")
TAG (L"##Number of data points")
DEFINITION (L"determines the number of data points that will be generated. Each "
	"data point occupies one row in the generated table.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm proceeds as follows:")
LIST_ITEM (L"1. Diagonalize the covariance matrix: calculate the eigenvalues $v__%i_ and "
	"eigenvectors %#e__%i_ of the %m \\xx %m Covariance matrix. "
	"In general there will also be %m of these. Let #%E be the %m \\xx %m matrix "
	"with eigenvector %#e__%j_ in column %j (%j=1..%m).")
LIST_ITEM (L"2. Generate a vector #x whose elements %x__%k_ equal %x__%k_ = "
	"randomGauss (0, \\Vr (%v__%k_)). "
	"Each  %x__%k_ is a random deviate drawn from a Gaussian distribution with "
	"mean zero and standard deviation equal to the square root of the corresponding "
	"eigenvalue %v__%k_.")
LIST_ITEM (L"3. Rotate back: calculate the vector #y = #%E #x, obtained by multiplying the vector "
	"#x with the matrix #%E.")
LIST_ITEM (L"4. Add the centroid to #y and copy the elements of #y to the corresponding row of "
	"the TableOfReal object.")
LIST_ITEM (L"5. Repeat steps 2, 3 and 4 until the desired number of data points "
	"has been reached.")
LIST_ITEM (L"6. Copy the column labels from the Covariance object to the "
	"TableOfReal object.")
NORMAL (L"In case the covariance matrix is diagonal, the algorithm is much simpler: we can skip "
	"the first and third step.")
MAN_END

MAN_BEGIN (L"Covariance & TableOfReal: Extract quantile range...", L"djmw", 20040225)
INTRO (L"Extract those rows from the selected @TableOfReal object whose Mahalanobis "
	"distance, with respect to the selected @Covariance object, are within the "
	"quantile range.")
MAN_END

MAN_BEGIN (L"Covariance & TableOfReal: To TableOfReal (mahalanobis)...", L"djmw", 20130502)
INTRO (L"Calculate Mahalanobis distance for the selected @TableOfReal with respect to the "
	"selected @Covariance object.")
ENTRY (L"Setting")
TAG (L"##Use table centroid")
DEFINITION (L"Use the mean vector calculated from the columns in the selected TableOfReal instead of the means in the selected Covariance.")
ENTRY (L"Explanation")
NORMAL (L"The Mahalanobis distance is defined as")
FORMULA (L"%d = \\Vr((#%x - #mean)\\'p #S^^-1^ (#%x - #mean)),")
NORMAL (L"where #%x is a vector, #mean is the average and #S is the covariance matrix. ")
NORMAL (L"It is the multivariate form of the distance measured in units of standard deviation.")
ENTRY (L"Example")
NORMAL (L"Count the number of items that are within 1, 2, 3, 4 and 5 standard deviations from the mean.")
NORMAL (L"We first create a table with only one column and 10000 rows and fill it with numbers drawn from "
	"a normal distribution with mean zero and standard deviation one. Its covariance matrix, of course, is "
	"one dimensional. We next create a table with Mahalanobis distances.")
CODE (L"n = 100000")
CODE (L"t0 = do (\"Create TableOfReal...\", \"table\", n, 1)")
CODE (L"do (\"Formula...\",  randomGauss(0,1))")
CODE (L"c = do (\"To Covariance\")")
CODE (L"selectObject (c, t0)")
CODE (L"ts = do (\"To TableOfReal (mahalanobis)...\", \"no\")")
CODE (L"")
CODE (L"for nsigma to 5")
CODE1 (L"  selectObject (ts)")
CODE1 (L"  extraction = do (\"Extract rows where...\",  \"self < nsigma\")")
CODE1 (L"  nr = do (\"Get number of rows\")")
CODE1 (L"  nrp = nr / n * 100")
CODE1 (L"  expect = (1 - 2 * gaussQ (nsigma)) * 100")
CODE1 (L"  writeInfoLine (nsigma, \"-sigma: \", nrp, \"%, \", expect, \"%\")")
CODE1 (L"  removeObject (extraction)")
CODE (L"endfor")
MAN_END

MAN_BEGIN (L"Create ChebyshevSeries...", L"djmw", 20040407)
INTRO (L"A command to create a @ChebyshevSeries from a list of coefficients.")
ENTRY (L"Settings")
TAG (L"##Xmin# and ##Xmax#")
DEFINITION (L"define the domain of the polynomials.")
TAG (L"%Coefficients")
DEFINITION (L"define the coefficients of each @@Chebyshev polynomials|Chebyshev polynomial@. "
	"The coefficient of the polynomial with the highest degree comes last.")
MAN_END

MAN_BEGIN (L"Create ISpline...", L"djmw", 20040407)
INTRO (L"A command to create an @ISpline from a list of coefficients.")
ENTRY (L"Settings")
TAG (L"##Xmin# and ##Xmax#")
DEFINITION (L"define the domain of the polynomial @spline.")
TAG (L"%Degree")
DEFINITION (L"defines the degree of the polynomial @spline.")
TAG (L"%Coefficients")
DEFINITION (L"define the coefficients of the basis polynomials.")
TAG (L"%%Interior knots")
DEFINITION (L"define the positions in the domain where continuity conditions are defined.")
ENTRY (L"Behaviour")
NORMAL (L"The number of coefficients and the number of interior knots must satisfy "
	"the following relation:")
FORMULA (L"%numberOfCoefficients = %numberOfInteriorKnots + %degree")
NORMAL (L"")
MAN_END

MAN_BEGIN (L"Create MSpline...", L"djmw", 20040407)
INTRO (L"A command to create an @MSpline from a list of coefficients.")
ENTRY (L"Settings")
TAG (L"##Xmin# and ##Xmax#")
DEFINITION (L"define the domain of the polynomial @spline.")
TAG (L"##Degree")
DEFINITION (L"defines the degree of the polynomial @spline.")
TAG (L"##Coefficients")
DEFINITION (L"define the coefficients of the basis polynomials.")
TAG (L"##Interior knots")
DEFINITION (L"define the positions in the domain where continuity conditions are defined.")
ENTRY (L"Behaviour")
NORMAL (L"The number of coefficients and the number of interior knots must satisfy "
	"the following relation:")
FORMULA (L"%numberOfCoefficients = %numberOfInteriorKnots + %degree + 1")
NORMAL (L"")
MAN_END

MAN_BEGIN (L"Create Polynomial...", L"djmw", 20040407)
INTRO (L"A command to create an @Polynomial from a list of coefficients.")
ENTRY (L"Settings")
TAG (L"##Xmin# and ##Xmax#")
DEFINITION (L"define the domain of the polynomial.")
TAG (L"##Degree")
DEFINITION (L"defines the degree of the basis polynomials.")
TAG (L"##Coefficients")
DEFINITION (L"define the coefficients of the polynomial. The coefficient of the "
	"highest power of %x comes last.")
MAN_END

MAN_BEGIN (L"Create LegendreSeries...", L"djmw", 20040407)
INTRO (L"A command to create a @LegendreSeries from a list of coefficients.")
ENTRY (L"Settings")
TAG (L"##Xmin# and ##Xmax#")
DEFINITION (L"define the domain of the polynomials.")
TAG (L"##Coefficients")
DEFINITION (L"define the coefficients of each @@Legendre polynomials|Legendre polynomial@. "
	"The coefficient of the polynomial with the highest degree comes last.")
MAN_END

MAN_BEGIN (L"Create Sound from gammatone...", L"djmw", 20100517)
INTRO (L"A command to create a @Sound as a @@gammatone@.")
ENTRY (L"Settings")
TAG (L"##Name")
DEFINITION (L"the name of the resulting Sound object.")
TAG (L"##Minimum time (s)# and ##Maximum time (s)#")
DEFINITION (L"the start and end time of the resulting Sound.")
TAG (L"##Sampling frequency (Hz)")
DEFINITION (L"the @@sampling frequency@ of the resulting Sound.")
TAG (L"##Gamma")
DEFINITION (L"determines the exponent of the polynomial.")
TAG (L"##Frequency (Hz)# and ##Bandwidth (Hz)#")
DEFINITION (L"determine the frequency and damping of the cosine wave in the gammatone.")
TAG (L"##Initial phase (radians)")
DEFINITION (L"the initial phase of the cosine wave.")
TAG (L"##Addition factor# (standard value: 0)")
DEFINITION (L"determines the degree of asymmetry in the spectrum of the gammatone. "
	"The zero default value gives a gammatone. A value unequal to zero results in a "
	"so called %gammachirp. A negative value is used in auditory filter modeling to "
	"guarantee the usual direction of filter asymmetry, which corresponds to an upward "
	"glide in instantaneous frequency.")
TAG (L"##Scale amplitudes")
DEFINITION (L"determines whether the amplitudes will be scaled to fit in the range (-1, 1).")
ENTRY (L"Purpose")
NORMAL (L"to create a Sound according to the following formula:")
FORMULA (L"%t^^%\\ga\\--1^ e^^\\--2%\\pi\\.c%bandwidth\\.c%t^ "
	"cos (2%%\\pi\\.cfrequency\\.ct% + %additionFactor\\.cln(%t) + %initialPhase),")
NORMAL (L"The %gammachirp function has a monotonically frequency-modulated carrier (the chirp) with "
	"instantaneous frequency ")
FORMULA (L"%instantaneousFrequency(%t) = %frequency + %additionFactor / (2\\.c\\pi\\.c%t)")
NORMAL (L"and an envelope that is a gamma distribution function. It is a theoretically optimum "
	"auditory filter, in the sense that it leads to minimal uncertainty in the joint time and "
	"scale representation of auditory signal analysis.")
NORMAL (L"For faithful modelling of the inner ear, "
	"@@Irino & Patterson (1997)@ conclude that a value of approximately 1.5 * ERB (%frequency) "
	"is appropriate for %bandwidth. "
	"ERB stands for @@equivalent rectangular bandwidth@. Their formula for ERB is:")
FORMULA (L"ERB(%f) = 6.23 10^^\\--6^ %f^2 + 93.39 10^^\\--3^ %f + 28.52.")
NORMAL (L"To avoid @aliasing in the chirp sound, a sound is only generated during times where the "
	"instantaneous frequency is greater than zero and smaller than the @@Nyquist frequency@.")
MAN_END

MAN_BEGIN (L"Create Sound from Shepard tone...", L"djmw", 20130410)
INTRO (L"One of the commands that create a @Sound.")
ENTRY (L"Settings")
TAG (L"##Name")
DEFINITION (L"the name of the resulting Sound object.")
TAG (L"##Minimum time (s)# and ##Maximum time (s)")
DEFINITION (L"the start and end time of the resulting Sound.")
TAG (L"##Sampling frequency (Hz)")
DEFINITION (L"the @@sampling frequency@ of the resulting Sound.")
TAG (L"##Lowest frequency (Hz)")
DEFINITION (L"the frequency of the lowest component in the tone complex.")
TAG (L"##Number of components")
DEFINITION (L"the number of frequency components in the tone complex.")
TAG (L"##Frequency change (semitones/s)")
DEFINITION (L"determines how many semitones the frequency of each component will change in one second. "
	"The number of seconds needed to change one octave will then be 12 divided by ##Frequency change#. "
	"You can make rising, falling and monotonous tone complexes by chosing a positive, negative or zero value.")
TAG (L"##Amplitude range% (dB)")
DEFINITION (L"determines the relative size in decibels of the maximum and the minimum amplitude of the components in a tone complex. These relative amplitudes will then be 10^^\\--%amplitudeRange/20^. ")
TAG (L"##Octave shift fraction (0-1)")
DEFINITION (L"shifts all frequency components by this fraction at the start. You will probably only need this "
	"if you want to generate static tone complexes as the example script below shows.")
ENTRY (L"Purpose")
NORMAL (L"To create a Sound that is a continuous variant of the sound sequences "
	"used by @@Shepard (1964)@ in his "
	"experiment about the circularity in judgments of relative pitch.")
NORMAL (L"The tone consists of many sinusoidal components whose frequencies "
	"might increase exponentially in time. "
	"All frequencies are always at successive intervals of an octave and sounded simultaneously. "
	"Thus the frequency of each component above the lowest is at each moment in time exactly twice "
	"the frequency of the one just below. The amplitudes are large for the components of intermediate "
	"frequency only, and tapered off gradually to subthreshold levels for the components at the "
	"highest and lowest extremes of frequency.")
NORMAL (L"For a rising tone complex, the Sound is generated according to the following specification:")
FORMULA (L"%s(%t) = \\su__%i=1..%numberOfComponents_ %A__%i_(%t) sin (arg__%i_(%t)), where")
FORMULA (L"arg__%i_(%t) = \\in 2%\\pi f__%i_(%\\ta) %d\\ta , and")
FORMULA (L"f__%i_(%t) = %lowestFrequency \\.c 2^^(%i \\-- 1 + octaveShiftFraction + %t/(12/%frequencyChange_st)^, with")
FORMULA (L"%A__%i_(%t) = 10^^((%L__min_ + (%L__max_ \\-- %L__min_) (1 \\-- cos 2%\\pi%\\te__%i_(%t)) / 2) / 20)^, where,")
FORMULA (L"%L__max_ = 0, %L__min_ = 10^^\\--%amplitudeRange/20^, and,")
FORMULA (L"%\\te__%i_(%t) = 2\\pi log2 (%f(%t) / %lowestFrequency) / %numberOfComponents.")
NORMAL (L"The maximum frequency that can be reached during a sweep by any single tone is:")
FORMULA (L"%maximumFrequency = %lowestFrequency\\.c2^^%numberOfComponents^.")
NORMAL (L"A component that reaches the maximum frequency falls instantaneously to the lowest frequency and then starts rising again.")
NORMAL (L"The absolute @@sound pressure level@ of the resulting sound will not be set, it is only guaranteed that the peak value "
	"is just below 1. You can always scale the intensity with the ##Scale Intensity...# command.")
ENTRY (L"Example")
NORMAL (L"The following script generates 12 static Shepard tone complexes, 1 semitone 'apart', "
	"with a cosine window to temper the abrupt start and finish.")
CODE (L"fadeTime = 0.010")
CODE (L"for i to 12")
CODE1 (L"fraction = (i-1)/12")
CODE1 (L"do (\"Create Sound from Shepard tone...\", \"s\" + string\\$  (i), 0, 0.1, 22050, 4.863, 10, 0, 34, fraction)")
CODE1 (L"do (\"Fade in...\", 0, 0, fadeTime, \"no\")")
CODE1 (L"do (\"Fade out...\", 0, 0.1, -fadeTime, \"no\")")
CODE (L"endfor")
MAN_END

MAN_BEGIN (L"Create formant table (Peterson & Barney 1952)", L"djmw", 20080509)
INTRO (L"A command to create a @Table object filled with the "
	"fundamental frequency and the first three formant frequency values from 10 "
	"American-English monophthongal vowels as spoken in a /h_d/ context by 76 speakers "
	"(33 men, 28 women and 15 children). Every vowel was pronounced twice, so that there are "
	"1520 recorded vowels in total.")
ENTRY (L"Table layout")
NORMAL (L"The created table will contain 9 columns:")
TAG (L"Column 1, labelled as %Type")
DEFINITION (L"speaker type: \"m\", \"w\" or \"c\" (for %man, %women or %child).")
TAG (L"Column 2, labelled as %Sex")
DEFINITION (L"speaker sex: either \"m\" or \"f\" (for %male or %female).")
TAG (L"Column 3, labelled as %Speaker")
DEFINITION (L"speaker id: a number from 1 to 76.")
TAG (L"Column 4, labelled as %Vowel")
DEFINITION (L"the vowel name. The following list gives the vowel in a %h_d context word "
	"together with its representation in this column: (%heed, iy), (%hid, ih), "
	"(%head, eh), (%had, ae), (%hod, aa), (%hawed, ao), (%hood, uh), (%%who'd%, uw), "
	"(%hud, ah), (%heard, er).")
TAG (L"Column 5, labelled as %IPA")
DEFINITION (L"the IPA notation for the vowels as defined in @@Peterson & Barney (1952)@. ")
TAG (L"Column 6, labelled as %F0")
DEFINITION (L"the fundamental frequency in Hertz.")
TAG (L"Column 7, 8 and 9, labelled as %F1, %F2 and %F3")
DEFINITION (L"the frequencies in Hertz of the first three formants.")
ENTRY (L"Remarks")
NORMAL (L"We originally downloaded the data from the University of Pennsylvania FTP site, "
	"where they were reportedly based on a printed version supplied by Ignatius Mattingly. ")
NORMAL (L"About the IPA notation. We used the original notation from the Peterson & Barney article. "
	"The notation in @@Watrous (1991)@ differs for three vowels: Watrous uses /e, o, \\er/ where Peterson & Barney use /\\ef, \\ct, \\er\\hr/.")
NORMAL (L"More details about these data and how they were measured can be found in the articles"
	"@@Watrous (1991)@ and in @@Peterson & Barney (1952)@.")
MAN_END

MAN_BEGIN (L"Create formant table (Pols & Van Nierop 1973)", L"djmw", 20020620)
INTRO (L"A command to create a @Table object filled with the frequencies and the levels "
	"of the first three formants from the 12 Dutch monophthong "
	"vowels as spoken in /h_t/ context by 50 male and 25 female speakers.")
ENTRY (L"Table layout")
NORMAL (L"The created table will contain 10 columns")
TAG (L"Column 1, labeled as %Sex")
DEFINITION (L"speaker sex: Either \"m\" or \"f\" (for %male or %female).")
TAG (L"Column 2, labeled as %Speaker")
DEFINITION (L"speaker id: a number from 1 to 75.")
TAG (L"Column 3, labeled as %Vowel")
DEFINITION (L"the vowel name. The following list gives the vowel in p_t context word "
	"together with its representation in this column: (%poet, oe), (%paat, aa), "
	"(%poot, oo), (%pat, a), (%peut, eu), (%piet, ie), (%puut, uu), (%peet, ee), "
	"(%put, u), (%pet, e), (%pot, o), (%pit, i).")
TAG (L"Column 4, labeled as %IPA")
DEFINITION (L"the IPA-notation for the vowels")
TAG (L"Column 5, 6 and 7, labeled as %F1, %F2 and %F3")
DEFINITION (L"the frequencies in Hertz of the first three formants.")
TAG (L"Column 8, 9 and 10, labeled as %L1, %L2 and %L3")
DEFINITION (L"the levels in decibel below overall SPL of the first three formants.")
NORMAL (L"More details about these data and how they were measured can be found "
	"in @@Pols et al. (1973)@ and @@Van Nierop et al. (1973)@.")
MAN_END

MAN_BEGIN (L"Create formant table (Weenink 1985)", L"djmw", 20041217)
INTRO (L"A command to create a @Table object filled with the "
	"fundamental frequency and the first three formant frequency values from 12 "
	"Dutch monophthongal vowels as spoken in isolation by 30 speakers "
	"(10 men, 10 women and 10 children). Every vowel was pronounced only once, so that there are "
	"360 recorded vowels in total. A reduced form, with only the formant frequecy values, is also available "
	"as a @@Create TableOfReal (Weenink 1985)...|TableOfReal@.")
ENTRY (L"Table layout")
NORMAL (L"The created table will contain 9 columns:")
TAG (L"Column 1, labelled as %Type")
DEFINITION (L"speaker type: \"m\", \"w\" or \"c\" (for %man, %women or %child).")
TAG (L"Column 2, labelled as %Sex")
DEFINITION (L"speaker sex: either \"m\" or \"f\" (for %male or %female).")
TAG (L"Column 3, labelled as %Speaker")
DEFINITION (L"speaker id: a number from 1 to 76.")
TAG (L"Column 4, labelled as %Vowel")
DEFINITION (L"the vowel name. The following list gives the vowel in Dutch p_t context words "
	"together with its representation in this column: (%poet, oe), (%paat, aa), "
	"(%poot, oo), (%pat, a), (%peut, eu), (%piet, ie), (%puut, uu), (%peet, ee), "
	"(%put, u), (%pet, e), (%pot, o), (%pit, i).")
TAG (L"Column 5, labelled as %IPA")
DEFINITION (L"the IPA notation for the vowels.")
TAG (L"Column 6, labelled as %F0")
DEFINITION (L"the fundamental frequency in Hertz.")
TAG (L"Column 7, 8 and 9, labelled as %F1, %F2 and %F3")
DEFINITION (L"the frequencies in Hertz of the first three formants. ")
NORMAL (L"The formant frequency values have been determined by means of LPC analysis with a "
	"varying prediction order. See @@Weenink (1985)@.")
MAN_END

MAN_BEGIN (L"Create TableOfReal (Pols 1973)...", L"djmw", 19990426)
INTRO (L"A command to create a @TableOfReal filled with the first three formant "
	"frequency values and (optionally) the levels from the 12 Dutch monophthongal "
	"vowels as spoken in /h_t/ context by 50 male speakers.")
NORMAL (L"The first three columns will contain the frequencies in Hz, the next three columns "
	"the levels in decibels below the overall SPL of the measured vowel segment. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL (L"More details about these data and how they were measured can be found in the paper of "
	"@@Pols et al. (1973)@.")
MAN_END

MAN_BEGIN (L"Create TableOfReal (Van Nierop 1973)...", L"djmw", 20041217)
INTRO (L"A command to create a @TableOfReal filled with the first three formant "
	"frequency values and (optionally) the levels from the 12 Dutch monophthongal "
	"vowels as spoken in /h_t/ context by 25 female speakers.")
NORMAL (L"The first three columns will contain the frequencies in Hz, the next three columns "
	"the levels in decibels below the overall SPL of the measured vowel segment. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL (L"More details about these data and how they were measured can be found in the paper of "
	"@@Van Nierop et al. (1973)@.")
MAN_END

MAN_BEGIN (L"Create TableOfReal (Weenink 1985)...", L"djmw", 19990426)
INTRO (L"A command to create a @TableOfReal filled with the first three formant "
	"frequency values from the 12 Dutch monophthongal "
	"vowels as spoken in isolation by either 10 men or 10 women or 10 children.")
NORMAL (L"The three columns will contain the formant frequencies in Hz. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL (L"More details about these data and how they were measured can be found in the paper of "
	"@@Weenink (1985)@.")
MAN_END

MAN_BEGIN (L"Discriminant", L"djmw", 19981103)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Discriminant represents the discriminant structure of a multivariate "
	"data set with several groups. This discriminant structure consists of a number of orthogonal "
	"directions in space, along which maximum separability of the groups can occur.")
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Discriminant analysis@ tutorial")
LIST_ITEM (L"\\bu @@TableOfReal: To Discriminant@")
NORMAL (L"Drawing")
LIST_ITEM (L"\\bu Draw eigenvalues...")
LIST_ITEM (L"\\bu Draw eigenvector...")
LIST_ITEM (L"\\bu @@Discriminant: Draw sigma ellipses...|Draw sigma ellipses...@")
MAN_END

MAN_BEGIN (L"Discriminant analysis", L"djmw", 20130502)
INTRO (L"This tutorial will show you how to perform discriminant analysis with P\\s{RAAT}")
NORMAL (L"As an example, we will use the dataset from @@Pols et al. (1973)@ "
	"with the frequencies and levels of the first three formants from the 12 "
	"Dutch monophthongal vowels as spoken in /h_t/ context by 50 male speakers. "
	"This data set has been incorporated into "
	"Praat and can be called into play with the @@Create TableOfReal "
	"(Pols 1973)...@ command that can be found in the \"New / "
	"TableOfReal\" menu.")
NORMAL (L"In the list of objects a new TableOfReal object will appear with 6 "
	"columns and 600 rows "
	"(50 speakers \\xx 12 vowels). The first three columns contain "
	"the formant frequencies in Hz, the last three columns contain the levels "
	"of the first three formants "
	"given in decibels below the overall sound pressure level of the measured "
	"vowel segment. Each row is labelled with a vowel label.")
NORMAL (L"Pols et al. use logarithms of frequency values, we will too. Because "
	"the measurement units in the first three columns are in Hz and in the last "
	"three columns in dB, it is probably better to standardize the columns. "
	"The following script summarizes our achievements up till now:")
CODE (L"table = do (\"Create TableOfReal (Pols 1973)...\", \"yes\"")
CODE (L"do (\"Formula...\", \"if col < 4 then log10 (self) else self fi\"")
CODE (L"do (\"Standardize columns\"")
CODE (L"\\#  change the column labels too, for nice plot labels.")
CODE (L"do (\"Set column label (index)...\", 1, \"standardized log (\\% F\\_ \\_ 1\\_ )\")")
CODE (L"do (\"Set column label (index)...\", 2, \"standardized log (\\% F\\_ \\_ 2\\_ )\")")
CODE (L"do (\"Set column label (index)...\", 3, \"standardized log (\\% F\\_ \\_ 3\\_ )\")")
CODE (L"do (\"Set column label (index)...\", 4, \"standardized \\% L\\_ \\_ 1\\_ \")")
CODE (L"do (\"Set column label (index)...\", 5, \"standardized \\% L\\_ \\_ 2\\_ \")")
CODE (L"do (\"Set column label (index)...\", 6, \"standardized \\% L\\_ \\_ 3\\_ \")")
NORMAL (L"To get an indication of what these data look like, we make a scatter "
	"plot of the "
	"first standardized log-formant-frequency against the second standardized "
	"log-formant-frequency. With the next script fragment you can reproduce the "
	"following picture.")
CODE (L"do (\"Viewport...\", 0, 5, 0, 5)")
CODE (L"selectObject (table)")
CODE (L"do (\"Draw scatter plot...\", 1, 2, 0, 0, -2.9, 2.9, -2.9, 2.9, 10, \"yes\", \"+\", \"yes\")")
PICTURE (5, 5, drawPolsF1F2_log)
NORMAL (L"Apart from a difference in scale this plot is the same as fig. 3 in the "
	"Pols et al. article.")
ENTRY (L"1. How to perform a discriminant analysis")
NORMAL (L"Select the TableOfReal and choose from the dynamic menu the option "
	"@@TableOfReal: To Discriminant|To Discriminant@. This command is available "
	"in the \"Multivariate statistics\" action button. The resulting Discriminant "
	"object will bear the same name as the TableOfReal object. The following "
	"script summarizes:")
CODE (L"selectObject (table)")
CODE (L"discrimimant = do (\"To Discriminant\")")
ENTRY (L"2. How to project data on the discriminant space")
NORMAL (L"You select a TableOfReal and a Discriminant object together and choose: "
	"@@Discriminant & TableOfReal: To Configuration...|To Configuration...@. "
	"One of the options of the newly created Configuration object is to draw it. "
	"The following picture shows how the data look in the plane spanned by the "
	"first two dimensions of this Configuration. The directions in this "
	"configuration are the eigenvectors from the Discriminant.")
PICTURE (5, 5, drawPolsDiscriminantConfiguration)
NORMAL (L"The following script summarizes:")
CODE (L"selectObject (table, discriminant)")
CODE (L"do (\"To Configuration...\", 0)")
CODE (L"do (\"Viewport...\", 0, 5, 0, 5)")
CODE (L"do (\"Draw...\", 1, 2, -2.9, 2.9, -2.9, 2.9, 12, \"yes\", \"+\", \"yes\")")
NORMAL (L"If you are only interested in this projection, there also is a short cut "
	"without an intermediate Discriminant object:  "
	"select the TableOfReal object and choose @@TableOfReal: To Configuration "
	"(lda)...|To Configuration (lda)...@.")
ENTRY (L"3. How to draw concentration ellipses")
NORMAL (L"Select the Discriminant object and choose @@Discriminant: Draw sigma "
	"ellipses...|Draw sigma ellipses...@. In the form you can fill out the "
	"coverage of the ellipse by way of the %%Number of sigmas% parameter. "
	"You can also select the projection "
	"plane. The next figure shows the 1-%\\si concentration ellipses in the "
	"standardized log %F__1_ vs log %F__2_ plane. When the data are multinormally distributed, "
	"a 1-%\\si ellipse will cover approximately 39.3\\%  of the data. "
	"The following code summarizes:")
CODE (L"selectObject (discriminant)")
CODE (L"do (\"Draw sigma ellipses...\", 1.0, \"no\", 1, 2, -2.9, 2.9, -2.9, 2.9, 12, \"yes\")")
PICTURE (5, 5, drawPolsF1F2ConcentrationEllipses)
ENTRY (L"4. How to classify")
NORMAL (L"Select together the Discriminant object (the classifier), and "
	"a TableOfReal object (the data to be classified). Next you choose "
	"@@Discriminant & TableOfReal: To ClassificationTable...|To "
	"ClassificationTable@. "
	"Normally you will enable the option %%Pool covariance matrices% and "
	"the pooled covariance matrix will be used for classification.")
NORMAL (L"The ClassificationTable can be converted to a @Confusion object "
	"and its fraction correct can be queried with: "
	"@@Confusion: Get fraction correct@.")
NORMAL (L"In general you would separate your data into two independent sets, "
	"\\s{TRAIN} and \\s{TEST}. You would use \\s{TRAIN} to train the "
	"discriminant classifier and \\s{TEST} to test how well it classifies. "
	"Several possibilities for splitting a dataset into two sets exist. "
	"We mention the @@Jackknife|jackknife@ (\"leave-one-out\") and the "
	"@@Bootstrap|bootstrap@ methods (\"resampling\").")
MAN_END

MAN_BEGIN (L"Discriminant: Draw sigma ellipses...", L"djmw", 20040407)
INTRO (L"A command to draw for each group from the selected @Discriminant an ellipse "
	"that covers part of the multivariate data.")
ENTRY (L"Settings")
TAG (L"##Number of sigmas")
DEFINITION (L"determines the @@concentration ellipse|data coverage@.")
TAG (L"##Discriminant plane")
DEFINITION (L"When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be drawn. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
TAG (L"##Xmin#, ##Xmax#, ##Ymin#, ##Ymax#")
DEFINITION (L"determine the limits of the drawing area.")
TAG (L"##Label size")
DEFINITION (L"determines the size of the labels at the centre of the ellipse. No "
	"labels will be drawn when a value less than or equal to zero is chosen.")
MAN_END

MAN_BEGIN (L"Discriminant: Extract within-group SSCP...", L"djmw", 20020314)
INTRO (L"Extract the @SSCP for group %%index% from the selected @Discriminant "
	"object.")
MAN_END

MAN_BEGIN (L"Discriminant: Extract pooled within-groups SSCP", L"djmw", 20020314)
INTRO (L"Extract the pooled within-group @SSCP from the selected @Discriminant "
	"object.")
MAN_END

MAN_BEGIN (L"Discriminant: Get partial discrimination probability...", L"djmw", 19981102)
INTRO (L"A command to test the selected @Discriminant for the significance of "
	"discrimination afforded by the remaining %n\\--%k eigenvectors after the acceptance of "
	"the first %k eigenvectors.")
ENTRY (L"Details")
NORMAL (L"The test statistic is:")
FORMULA (L"%\\ci^2 = \\--(%degreesOfFreedom\\--(%numberOfGroups+%dimension)/2) ln \\La\\'p, where")
FORMULA (L"%degreesOfFreedom = (%dimension\\--%k)(%numberOfGroups\\--%k\\--1), and, ")
FORMULA (L"\\La\\'p = \\Pi__%j=%k+1..%numberOfEigenvalues_ 1 / (1 + %%eigenvalue[j]%)")
MAN_END

MAN_BEGIN (L"Discriminant: Get contribution of component...", L"djmw", 19981106)
INTRO (L"A command to ask the selected @Discriminant for the contribution of the %j^^th^ "
	"discriminant function (component) to the total variance.")
ENTRY (L"Details")
NORMAL (L"The contribution is defined as:")
FORMULA (L"%%eigenvalue[j]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (L"Discriminant: Get Wilks' lambda...", L"djmw", 20040407)
INTRO (L"A command to ask the selected @Discriminant for the value of Wilks' lamda (a "
	"multivariate measure of group differences over several variables).")
ENTRY (L"Settings")
TAG (L"##From")
DEFINITION (L"the first eigenvalue number from which the value for lambda has to be calculated.")
ENTRY (L"Details")
NORMAL (L"Wilks' lambda is defined as:")
FORMULA (L"%\\La = \\Pi__%i=%from..%numberOfEigenvalues_ 1 / (1 + %eigenvalue[%i])")
NORMAL (L"Because lambda is a kind of %inverse measure, values of lambda which are near zero "
	"denote high discrimination between groups.")
MAN_END

MAN_BEGIN (L"Discriminant: Get concentration ellipse area...", L"djmw", 20040407)
INTRO (L"A command to query the @Discriminant object for the area of the concentration "
	"ellipse of one of its groups.")
ENTRY (L"Settings")
TAG (L"##Number of sigmas")
DEFINITION (L"determines the @@concentration ellipse|data coverage@.")
TAG (L"##Discriminant plane")
DEFINITION (L"When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the area of the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be calculated. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
ENTRY (L"Algorithm")
NORMAL (L"See @@SSCP: Get sigma ellipse area...")
MAN_END

MAN_BEGIN (L"Discriminant: Get confidence ellipse area...", L"djmw", 20040407)
INTRO (L"A command to query the @Discriminant object for the area of the confidence "
	"ellipse of one of its groups.")
ENTRY (L"Settings")
TAG (L"##Discriminant plane")
DEFINITION (L"When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the area of the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be calculated. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
ENTRY (L"Algorithm")
NORMAL (L"See @@SSCP: Get confidence ellipse area...")
MAN_END

MAN_BEGIN (L"Discriminant & Pattern: To Categories...", L"djmw", 20040422)
INTRO (L"A command to use the selected @Discriminant to classify each pattern from the "
	"selected @Pattern into a category.")
NORMAL (L"Arguments as in @@Discriminant & TableOfReal: To ClassificationTable...@.")
MAN_END

MAN_BEGIN (L"Discriminant & SSCP: Project", L"djmw", 20020313)
INTRO (L"A command to project the selected @SSCP object on the eigenspace "
	"defined by the selected @Discriminant object.")
NORMAL (L"Further details can be found in @@Eigen & SSCP: Project@")
MAN_END

MAN_BEGIN (L"Discriminant & TableOfReal: To ClassificationTable...", L"djmw", 20040407)
INTRO (L"A command to use the selected @Discriminant to classify each row from the "
	"selected @TableOfReal. The newly created @ClassificationTable will then contain the posterior "
	"probabilities of group membership.")
ENTRY (L"Settings")
TAG (L"##Pool covariance matrices")
DEFINITION (L"when on, all group covariance matrices are pooled and distances will be determined "
	"on the basis of only this pooled covariance matrix (see below).")
ENTRY (L"Details")
NORMAL (L"The posterior probabilities of group membership %p__%j_ for a vector #x are defined as:")
FORMULA (L"%p__%j_ = %p(%j\\| #%x) = exp (\\--%d__%j_^^2^(#%x) / 2) / "
	"\\su__%k=1..%numberOfGroups_ exp (\\--%d__%k_^^2^(#%x) / 2),")
NORMAL (L"where %d__%i_^^2^ is the generalized squared distance function:")
FORMULA (L"%d__%i_^^2^(#%x) = ((#%x\\--#%\\mu__%i_)\\'p #\\Si__%i_^^-1^ (#%x\\--#%\\mu__%i_) + "
	"ln determinant (#\\Si__%i_)) / 2 \\-- ln %aprioriProbability__%i_")
NORMAL (L"that depends on the individual covariance matrix #\\Si__%i_ and the mean "
	"#%\\mu__%i_ for group %i.")
NORMAL (L"When the covariances matrices are %pooled, the squared distance function can be reduced to:")
FORMULA (L"%d__%i_^^2^(#%x) = ((#%x\\--#%\\mu__%i_)\\'p #\\Si^^-1^ (#%x\\--#%\\mu__%i_) "
	"\\-- ln %aprioriProbability__%i_,")
NORMAL (L"and #\\Si is now the pooled covariance matrix.")
NORMAL (L"The a priori probabilities normally will have values that are related to the number of "
	"%training vectors %n__%i_ in each group:")
FORMULA (L"%aprioriProbability__%i_ = %n__%i_ / \\Si__%k=1..%numberOfGroups_ %n__%k_")
MAN_END

MAN_BEGIN (L"Discriminant & TableOfReal: To Configuration...", L"djmw", 20040407)
INTRO (L"A command to project each row in the selected @TableOfReal onto "
	"a space spanned by the eigenvectors of the selected @Discriminant. ")
ENTRY (L"Settings")
TAG (L"##Number of dimensions")
DEFINITION (L"specifies the number of eigenvectors taken into account, i.e., determines "
	"the dimension of the resulting @Configuration. When the default value (0) is "
	"given the resulting Configuration will have the maximum dimension as allowed by "
	"the number of eigenvectors in the selected Discriminant.")
ENTRY (L"Precondition")
NORMAL (L"The number of columns in the TableOfReal must equal the dimension of the "
	"eigenvectors in the Discriminant.")
NORMAL (L"See also @@Eigen & TableOfReal: Project...@.")
MAN_END

MAN_BEGIN (L"Discriminant & TableOfReal: To TableOfReal (mahalanobis)...", L"djmw", 20130502)
INTRO (L"Calculate Mahalanobis distances for the selected @TableOfReal with respect to one group in the "
	"selected @Discriminant object.")
ENTRY (L"Settings")
TAG (L"##Group label")
DEFINITION (L"defines which group mean to use for the distance calculation.")
TAG (L"##Pool covariance matrices")
DEFINITION (L"when on use a pooled covariance matrix instead of the group covariance matrix.")
ENTRY (L"Algorithm")
NORMAL (L"See @@Covariance & TableOfReal: To TableOfReal (mahalanobis)...@.")
ENTRY (L"Example")
NORMAL (L"Calculate the number of datapoints that are within the one-sigma elipses of two different groups, i.e. "
	"the number of data points that are in the overlapping area. ")
NORMAL (L"Suppose the group labels are \\o/ and \\yc.")
CODE (L"pols50m = do (\"Create TableOfReal (Pols 1973)...\", \"no\")")
CODE (L"do (\"Formula...\", \"log10(self)\")")
CODE (L"discriminant = do (\"To Discriminant\")")
CODE (L"selectObject (pols50m, discriminant)")
CODE (L"t1 = do (\"To TableOfReal (mahalanobis)...\", \"\\bso/\", \"no\")")
CODE (L"selectObject (pols50m, discriminant)")
CODE (L"t2 = do (\"To TableOfReal (mahalanobis)...\", \"\\bsyc\", \"no\")")
NORMAL (L"Now we count when both the t1 and t2 values are smaller than 1 (sigma):")
CODE (L"do (\"Copy...\", \"tr\")")
CODE (L"do (\"Formula...\", \"Object_'t1'[] < 1 and Object_'t2'[] < 1\")")
CODE (L"do (\"Extract rows where column...\", 1, \"equal to\", 1)")
CODE (L"no = do (\"Get number of rows\")")

MAN_END

MAN_BEGIN (L"DTW", L"djmw", 20110603)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type DTW represents the dynamic time warp structure of "
	"two objects.")
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@CC: To DTW...@ (from 2 objects with cepstral coefficients)")
LIST_ITEM (L"\\bu ##Spectrogram: To DTW...# (from 2 Spectrogram objects)")
NORMAL (L"Query:")
LIST_ITEM (L"\\bu @@DTW: Get y time from x time...@")
LIST_ITEM (L"\\bu @@DTW: Get x time from y time...@")
MAN_END

MAN_BEGIN (L"DTW: Draw warp (x)...", L"djmw", 20071204)
INTRO (L"Draws the warp given a time on the \"x-direction\"")
MAN_END

MAN_BEGIN (L"DTW: Find path (band & slope)...", L"djmw", 20120223)
INTRO (L"Finds the optimal path for the selected @DTW that lies within the union of the sakoe-chiba band and local slope limits.")
ENTRY (L"Settings")
TAG (L"##Sakoe-Chiba band (s)#,")
DEFINITION (L"The maximum distance from the start/end of the sound where a path may start/finish.")
TAG (L"##Slope constraint#,")
DEFINITION (L"determines the maximum and minimum local slopes in the optimal path. For example, the constraint "
    "1/3 < slope < 3 forces the path locally after having taken three steps in the same direction direction to take the next step in the other direction, or after having taken two steps in the same direction to take the next step in the diagonal direction. At the same time the global consequences of the \"1/3 < slope < 3\" constraint mandates that the durations of the two domains do not differ by more than a factor of three. ")
NORMAL (L"For more information see the article of @@Sakoe & Chiba (1978)@.")
MAN_END

MAN_BEGIN (L"DTW: Get maximum consecutive steps...", L"djmw", 20050307)
INTRO (L"Get the maximum number of consecutive steps in the chosen direction along the optimal path from the selected @DTW.")
MAN_END

MAN_BEGIN (L"DTW: Get distance (weighted)", L"djmw", 20100628)
INTRO (L"Queries the selected @DTW object for the weighted distance along the minimum path.")
ENTRY (L"Algorithm")
NORMAL (L"If the distance matrix has %%nx% cells along the %%x%-direction, %%ny% cells along the %%y%-direction and the "
	"sum of the distances along the minimum path is %%S%, the weighted distance is given by %%S%/(%nx+%ny). ")
MAN_END

MAN_BEGIN (L"DTW: Get time along path...", L"djmw", 20110603)
INTRO (L"Queries the selected @DTW object for the time along the minimal path "
	"given the time along the \"%x-direction\". This command is deprecated, the new commands for querying are "
	"@@DTW: Get y time from x time...@ and @@DTW: Get x time from y time...@.")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time along the %x-direction.")
ENTRY (L"Behaviour")
NORMAL (L"When the %input time is in the interval [%xmin, %xmax], the %returned "
	"time will be in the interval [%ymin, %ymax], where [%xmin, %xmax] and "
	"[%ymin, %ymax] are the domains of the two \"objects\" from which the "
	"DTW-object was constructed."
	"For all other input times we assume that the two object are aligned.")
NORMAL (L"We like to have a \"continuous\" interpretation of time for the quantized times in the %x and "
	"%y direction; we make the path piecewise linear. There are two special cases:")
NORMAL (L"1. The local path is horizontal. We calculate the %y-time from the line that connects the "
	"lower-left position of the leftmost horizontal time block to the upper-right position of the "
	"rightmost horizontal  time block.")
NORMAL (L"2. The local path is vertical. We calculate the %y-time from the line that connects the "
	"lower-left position of the bottommost vertical time block to the upper-right position of the "
	"topmost horizontal time block.")
MAN_END

MAN_BEGIN (L"DTW: Get y time from x time...", L"djmw", 20110603)
INTRO (L"Queries the selected @DTW object for the time along the %y-direction "
	"given the time along the \"%x-direction\". ")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time along the %x-direction.")
ENTRY (L"Behaviour")
NORMAL (L"When the %input time is in the interval [%xmin, %xmax], the %returned "
	"time will be in the interval [%ymin, %ymax], where [%xmin, %xmax] and "
	"[%ymin, %ymax] are the domains of the two \"objects\" from which the "
	"DTW-object was constructed."
	"For input times outside the domain we assume that the two object were aligned.")
NORMAL (L"We like to have a \"continuous\" interpretation of time for the quantized times in the %x and "
	"%y direction; we make the path a piecewise linear monotonically increasing function. "
	"There are special cases:")
NORMAL (L"1. The local path is in the %x-direction only. We calculate the %y-time from the line "
	"that connects the lower-left position of the begin cell of this path to the "
	"upper-right position of the last cell.")
NORMAL (L"2. The local path is in the x-direction only. We calculate the %y-time from the line "
	"that connects the lower-left position of lowest cell to the upper-right position of the "
	"highest cell.")
NORMAL (L"3. A cell is both part of a path in the %x- and the %y-direction. "
	"We then calculate the intersection point of the paths in the %x- and the %y-directions. "
	"The %y-times in this cell are now calculated from the two line segments that originate "
	"from this intersection point.")
MAN_END

MAN_BEGIN (L"DTW: Get x time from y time...", L"djmw", 20110603)
INTRO (L"Queries the selected @DTW object for the time along the %x-direction "
	"given the time along the \"%y-direction\". ")
ENTRY (L"Setting")
TAG (L"##Time (s)")
DEFINITION (L"the time along the %y-direction.")
ENTRY (L"Behaviour")
NORMAL (L"The behaviour is like @@DTW: Get y time from x time...@")
MAN_END

MAN_BEGIN (L"DTW: Swap axes", L"djmw", 20050306)
INTRO (L"Swap the x and y-axes of the selected @DTW.")
MAN_END

MAN_BEGIN (L"DTW: To Polygon...", L"djmw", 20120223)
INTRO (L"A command to convert for a selected @DTW the Sakoe-Chiba band and the local slope constraint "
	"to a @Polygon object. The polygon will show the boundaries of the search domain for the optimal path.")
MAN_END

MAN_BEGIN (L"DTW & TextGrid: To TextGrid (warp times)", L"djmw", 20110603)
INTRO (L"Create a new TextGrid from the selected @DTW and @TextGrid by warping the "
	"times from the selected TextGrid to the newly created TextGrid.")
ENTRY (L"Algorithm")
NORMAL (L"First we check whether the y-domain of the DTW and the domain of the TextGrid are "
	"equal. If they are, a new TextGrid is created by copying the selected one. "
	"We then change its domain and make it equal to the x-domain of the DTW. "
	"Then for each tier we change the domain and @@DTW: Get x time from y time...|calculate new times@ by using the path.")
MAN_END

MAN_BEGIN (L"DTW & Sounds: Draw...", L"djmw", 20071204)
INTRO (L"Draws the distances, the path and the sounds for the selected @DTW and the two selected @@Sound|Sounds@.")
MAN_END

MAN_BEGIN (L"DTW & Sounds: Draw warp (x)...", L"djmw", 20071204)
INTRO (L"Draws the warp given a time on the \"x-direction\" for the selected @DTW and the two selected @@Sound|Sounds@.")
MAN_END

MAN_BEGIN (L"Create empty EditCostsTable...", L"djmw", 20120524)
INTRO (L"Creates an empty @@EditCostsTable@.")
ENTRY (L"Settings")
TAG (L"##Name#")
DEFINITION (L"the name of the resulting EditCostsTable object.")
TAG (L"##Number of target symbols#")
DEFINITION (L"the number of different symbols in the target symbol set that you want to give special edit cost values in the EditCostTable. "
	"The number you specify may be smaller than the actual target symbol set size because the EditCostTable has an entry for target symbols "
	"that fall in a %%rest% category. If you don't want to treat any target symbol is a special way you may set this value to 0.")
TAG (L"##Number of source symbols#")
DEFINITION (L"the number of different symbols in the source symbol set that you want to give special edit cost values in the EditCostTable. "
	"The number you specify may be smaller than the actual source symbol set size because the EditCostTable has an entry for source symbols "
	"that fall in a %rest% category. If you don't want to treat any source symbol is a special way you may set this value 0.")
MAN_END

MAN_BEGIN (L"EditCostsTable", L"djmw", 20130502)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"The EditCostsTable determines the %%string edit costs%, i.e. the costs involved in changing one string of "
	"symbols (the %%source%) into another one (the %%target%). "
	"String edit costs are generally divided into %%insertion%, %%deletion% and %%substitution% costs. "
	"The latter terms refer to the operations that may be performed on a source string to transform it to a target "
	"string. For example, to change the source string \"execution\" to the target string \"intention\" we would need "
	"one insertion (i), one deletion (d) and three substitutions (s) as the following figure shows.")
SCRIPT (4, 1.0,  L"target = do (\"Create Strings as characters...\", \"intention\")\n"
"source = do (\"Create Strings as characters...\", \"execution\")\n"
	"selectObject (source, target)\n"
	"edt = do (\"To EditDistanceTable\")\n"
	"do (\"Draw edit operations\")\n"
	"removeObject (edt, target, source)\n")
NORMAL (L"The figure above was produced with default values for the costs, i.e. the insertion and deletion costs were 1.0 while the "
	"substitution cost was 2.0. The actual edit distance between the target and source strings is calculated by the @@EditDistanceTable@ "
	"which uses an EditCostsTable to access the specific string edit costs. The figure above was produced by the following commands:")
CODE (L"target = do (\"Create Strings as characters...\", \"intention\")")
CODE (L"source = do (\"Create Strings as characters...\", \"execution\")")
CODE (L"plusObject (target)")
CODE (L"edt = do (\"To EditDistanceTable\")")
CODE (L"do (\"Draw edit operations\")")
NORMAL (L"The default EditCostsTable which is in every new EditDistanceTable object has only two rows and two columns, "
	"where the cells in this EditCostsTable have the following interpretation:\n")
TAG (L"Cell [1][2]:")
DEFINITION (L"defines the cost for the insertion of a target symbol in the source string. The default insertion cost is 1.0.")
TAG (L"Cell [2][1]:")
DEFINITION (L"defines the cost of the deletion of a source symbol. The default value is 1.0.")
TAG (L"Cell [1][1]:")
DEFINITION (L"defines the cost of substituting a target symbol for a source symbol where the target and source symbols don't match. The default substitution cost is 2.0.")
TAG (L"Cell [2][2]:")
DEFINITION (L"defines the cost of substituting a target symbol for a source symbol where the target and source symbols do match. The deault value is 0.0.")
ENTRY (L"How to create a non-default EditCostsTable")
NORMAL (L"In general we can define a table for %%numberOfTargets% target symbols and %%numberOfSources% source symbols. These numbers "
	"do not necessarily have to be equal to the number of different symbols that may occur in the target and source strings. They only represent the number of symbols that you like to give special edit costs. "
	"The EditCostTable will provide one extra dimension to accommodate target symbol insertion costs and source symbol deletion costs and another extra dimension to represent other target and source symbols that don't have separate entries and can therefore be treated as one group. "
	"The actual dimension of the table will therefore be (%%numberOfTargets% + 2) \\xx (%%numberOfSources% + 2). This is what the cells in the non-default table mean: ")
LIST_ITEM (L"\\bu The upper matrix part of dimension %%numberOfTargets% \\xx %%numberOfSources% will show at cell [%i][%j] the costs "
	"of substituting the %i-th target symbol for the %j-th source symbol.")
LIST_ITEM (L"\\bu The first %%numberOfSources% values in row (%%numberOfTargets% + 1) represent the costs of substituting one of the target "
	"symbols from the target %%rest% category for the source symbol in the corresponding column.  The target rest category is the group of "
	"targets that do not belong to the %%numberOfTargets% targets represented in the upper part of the matrix.")
LIST_ITEM (L"\\bu The first %%numberOfTargets% values in the column (%%numberOfSources% + 1) represent the costs of substituting the target "
	"symbol in the corresponding row for one of the source symbols from the source %%rest% category.  The source rest category is the group "
	"of source symbols that do not belong to the %%numberOfSources% source symbols represented in the upper part of the matrix.")
LIST_ITEM (L"\\bu The first %%numberOfSources% cells in the last row represent the deletion cost of the corresponding source symbols.")
LIST_ITEM (L"\\bu The first %%numberOfTargets% cells in the last column represent the insertion costs of the corresponding target symbols.")
LIST_ITEM (L"\\bu Finally the four numbers in the cells at the bottom-right corner have an interpretation analogous to the four numbers in "
	"the basic EditCostTable we discussed above (but now for the %%rest% symbols).")
ENTRY (L"Example")
NORMAL (L"If we extend the basic table with one extra target and one extra source symbol, then the EditCostTable will "
	"be a 3 by 3 table. The numbers in the following table have been chosen to be distinctive and therefore probably "
	"will not correspond to any practical situation.")
CODE (L"   s         ")
CODE (L"t 1.1 1.2 1.3")
CODE (L"  1.4 1.5 1.6")
CODE (L"  1.7 1.8 0.0")
NORMAL (L"By issuing the following series of commands this particular table can be created:")
CODE (L"do (\"Create empty EditCostsTable...\", \"editCosts\", 1, 1)")
CODE (L"do (\"Set target symbol (index)...\", 1, \"t\")")
CODE (L"do (\"Set source symbol (index)...\", 1, \"s\")")
CODE (L"do (\"Set insertion costs...\", \"t\", 1.3)")
CODE (L"do (\"Set deletion costs...\", \"s\", 1.7)")
CODE (L"do (\"Set substitution costs...\", \"t\", \"s\", 1.1)")
CODE (L"do (\"Set substitution costs...\", \"\", \"s\", 1.4)")
CODE (L"do (\"Set substitution costs...\", \"t\", \"\", 1.2)")
CODE (L"do (\"Set costs (others)...\", 1.6, 1.8, 0, 1.5)")
NORMAL (L"In the first line we create the (empty) table, we name it %%editCosts% and it creates space for one target "
	"and one source symbol. The next line defines the target symbol which becomes the label of the first row of the table. "
	"Line 3 defines the source symbol which will become the label of the first column of the table. "
	"We next define the insertion and deletion costs, they fill cells [1][3] and [3][1], respectively. "
	"Cell [1][1] is filled by the command in line 6.  The command in line 7 fills cell [2][1] which defines the cost "
	"of substituting any target symbol unequal to \"t\" for \"s\". The next line fills cell [1][2] which defines "
	"the substitution costs of \"t\" for any source symbol unequal to \"s\". "
	"Finally, the command in the last line defines the little 2\\xx2 matrix at the bottom-right that "
	"is analogous to the default cost matrix explained above. Therefore cell [2][2] defines the cost of substituting a "
	"target symbol unequal to \"t\" for a source symbol unequal to \"s\" where the target and source symbols don't match, while cell [3][3] "
	"defines the costs when they do match. "
	"Cell [3][2] defines the cost of the deletion of a source symbol unequal \"s\", while cell [2][3] defines the cost "
	"for  the insertion of a target symbol unequal \"t\" in the source string. ")
ENTRY (L"How to use a special EditCostsTable")
NORMAL (L"After creating the special EditCostsTable you select it together with the EditDistanceTable and issue the command @@EditDistanceTable & EditCostsTable: Set new edit costs|Set new edit costs@. The EditDistanceTable will then find the minimum edit distance based on the new cost values.")
MAN_END

MAN_BEGIN (L"EditDistanceTable", L"djmw", 20130502)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"An EditDistanceTable shows the accumulated distances between a target string and a source string. "
	"For example, the accumulated distances between the target string \"intention\" and the source string "
	"\"execution\" can be expressed by the following EditDistanceTable:")
SCRIPT (5, 3.5, L"target = do (\"Create Strings as characters...\", \"intention\")\n"
	"source = do (\"Create Strings as characters...\", \"execution\")\n"
	"selectObject (source, target)\n"
	"edt = do (\"To EditDistanceTable\")\n"
	"do (\"Draw...\", \"decimal\", 1, 0)\n"
	"removeObject (edt, target, source)\n")
NORMAL (L"This figure was created by issuing the following commands:")
CODE (L"target = do (\"Create Strings as characters...\", \"intention\")")
CODE (L"source = do (\"Create Strings as characters...\", \"execution\")")
CODE (L"plusObject (target)")
CODE (L"edt = do (\"To EditDistanceTable\")")
CODE (L"do (\"Draw...\", \"decimal\", 1, 0)")
NORMAL (L"The target string is always displayed vertically while the source string is displayed horizontally and the origin is at the bottom-left corner of the table. "
	"Each cell of this table, dist[%i, %j], contains the accumulated distance between the first %i characters of the target and the first %j characters of the source. The cells on the path through this table which have the "
	"minimum accumulated cost are shown with boxes around them. Below we will explain how this path is calculated.")
NORMAL (L"The local directional steps in this path show which %%edit operations% we have to perform on the source string symbols to obtain the target string symbols. "
	"Three edit operations exist: (1) %%insertion% of a target symbol in the source string. This happens each time we take a step in the vertical direction along the path. (2) %%deletion% of a symbol in the source string. This happens each time we take a step in horizontal direction along the path. (3) %%substitution% of a source symbol by a target symbol happens at each diagonal step along the path.")
NORMAL (L"If we trace the path from its start at the origin to its end, we see that it first moves up, indicating the insertion of an \"i\" symbol in the source string. "
	"In the next step which is in the diagonal direction, the \"n\" target is substituted for the \"e\" source symbol. Next follows another substitution, \"t\" for \"x\". "
	"The next diagonal step substitutes \"e\" for an identical \"e\". This step is followed by a horizontal step in which the source symbol \"c\" is deleted. "
	"The next diagonal step substitutes an \"n\" for a \"u\". The path now continues in the diagonal direction until the end point and only identical substitutions occur in the last part. The following figure shows these operations more explicitly.")
SCRIPT (4, 1.5,  L"target = do (\"Create Strings as characters...\", \"intention\")\n"
	"source = do (\"Create Strings as characters...\", \"execution\")\n"
	"plusObject (target)\n"
	"edt = do (\"To EditDistanceTable\")\n"
	"do (\"Draw edit operations\")\n"
	"removeObject (edt, target, source)\n")
NORMAL (L"The value of the accumulated costs in a cell of the table is computed by taking the minimum of the accumulated distances from three possible paths that end in the current cell, i.e. the paths that come from the %%left%, from the %%diagonal% and from %%below%.")
CODE (L"dist[i,j] = min (d__left_, d__diag_, d__below_), ")
NORMAL (L"where ")
CODE (L" d__left _ = dist[i-1,j]   + insertionCost(target[i])")
CODE (L" d__diag _ = dist[i-1,j-1] + substitutionCost(source[j],target[i])")
CODE (L" d__below_ = dist[i,j-1]   + deletionCost(source[j])")
NORMAL (L"Since the calculation is recursive we start at the origin. After calculating the accumulative distances for each cell in the table as based on the algorithm above, the cell at the top-right position will contain the accumulated edit distance. "
	"This distance happens to be 8 for the given example. The value 8 results from using the target-indepent value of 1.0 for the insertion cost, the source-independent value of 1.0 for the deletion costs and a constant value of 2.0 for the substitution costs. "
	"If target and source symbol happen to be equal no costs are assigned, or, equivalently the substitution costs are zero if target and source symbol match. If you want more control over these costs you can create an @@EditCostsTable@ and specify your special costs and then @@EditDistanceTable & EditCostsTable: Set new edit costs|set the new edit costs@.")
NORMAL (L"If during the calculations we also keep track of which of the three cells resulted in the local minimum accumulated distance, we can use this directional "
	"information to backtrack from the cell at the top-right position to the cell at the bottom-right position and obtain the minimum path.")
MAN_END

MAN_BEGIN (L"EditDistanceTable & EditCostsTable: Set new edit costs", L"djmw", 20120522)
INTRO (L"A command available in the dynamic menu if an @@EditDistanceTable@ and an @@EditCostsTable@ are chosen together.")
NORMAL (L"New accumulated cost values will be calculated and a new path based on these values will be calculated.")
MAN_END

MAN_BEGIN (L"Eigen", L"djmw", 19981102)
INTRO (L"One of the @@types of objects@ in Praat.")
NORMAL (L"An object of type Eigen represents the eigen structure of "
	"a matrix whose eigenvalues and eigenvectors are real.")
ENTRY (L"Inside an Eigen")
NORMAL (L"With @Inspect you will see the following attributes:")
TAG (L"%numberOfEigenvalues")
DEFINITION (L"the number of eigenvalues and eigenvectors")
TAG (L"%dimension")
DEFINITION (L"the dimension of an eigenvector.")
TAG (L"%eigenvalues[1..%numberOfEigenvalues]")
DEFINITION (L"the real eigenvalues.")
TAG (L"%eigenvectors[1..%numberOfEigenvalues][1..%dimension]")
DEFINITION (L"the real eigenvectors, stored by row.")
MAN_END

MAN_BEGIN (L"Eigen: Draw eigenvalues...", L"djmw", 20040407)
INTRO (L"A command to draw the eigenvalues of the selected @Eigen object(s).")
ENTRY (L"Settings")
TAG (L"##Fraction of eigenvalues summed")
DEFINITION (L"defines whether or not fractions are plotted. Fractions %f__%i_ "
	"will be calculated for each number %e__%i_ by dividing this number by the sum of all "
	"numbers %e__%j_: %f__%i_ = %e__%i_ / \\su__%j=1..%numberOfEigenvalues_ %e__%j_.")
TAG (L"##Cumulative")
DEFINITION (L"defines whether or not cumulative values are plotted. Cumulative "
	"values %c__%i_ will be calculated for each number %e__%i_ by summing the first %i "
	"numbers %e__%j_: %c__%i_ = \\su__%j=1..%i_ %e__%j_).")
NORMAL (L"A @@Scree plot|scree@ plot can be obtained if both %%Fraction of eigenvalues summed% "
	"and %%Cumulative% are unchecked.")
MAN_END

MAN_BEGIN (L"Eigen: Draw eigenvector...", L"djmw", 20040407)
INTRO (L"A command to draw an eigenvector from the selected @Eigen.")
ENTRY (L"Settings")
TAG (L"##Eigenvector number")
DEFINITION (L"determines the eigenvector to be drawn.")
TAG (L"%Component %loadings")
DEFINITION (L"when on, the eigenvector is multiplied with the square root of the corresponding "
	"eigenvalue. (For @@PCA@-analysis this means that you will draw the so called "
	"%%component loading vector%. You will be able to compare "
	"quantitatively the elements in different component loading vectors because "
	"the %i-th element in the %j-th component loading vector gives the covariance between the %i-th "
	"original variable and the %j-th principal component.)")
TAG (L"##Element rang%")
DEFINITION (L"determine the first and last element of the vector that must be drawn.")
TAG (L"##Minimum# and ##Maximum#")
DEFINITION (L"determine the lower and upper bounds of the plot (choosing #Maximum smaller than #Minimum "
	"will draw the %%inverted% eigenvector). ")
TAG (L"##Mark size#, ##Mark string#")
DEFINITION (L"determine size and type of the marks that will be drawn.")
TAG (L"##Garnish")
DEFINITION (L"determines whether a bounding box and margins will be drawn.")
MAN_END

MAN_BEGIN (L"Eigen: Get contribution of component...", L"djmw", 19981109)
INTRO (L"A command to ask the selected @Eigen for the contribution of the %j^^th^ "
	"eigenvalue to the total sum of eigenvalues.")
ENTRY (L"Details")
NORMAL (L"The contribution is defined as:")
FORMULA (L"%%eigenvalue[j]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (L"Eigen: Get cumulative contribution of components...", L"djmw", 19981109)
INTRO (L"A command to ask the selected @Eigen for the contribution of the sum of the "
	"eigenvalues[%from..%to] to the total sum of eigenvalues.")
ENTRY (L"Details")
NORMAL (L"The contribution is defined as:")
FORMULA (L"\\Si__%i=%from..%to_ %%eigenvalue[i]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (L"Eigen: Get eigenvalue...", L"djmw", 20040225)
INTRO (L"A command to query the selected @Eigen for the %i^^th^ "
	"eigenvalue.")
MAN_END

MAN_BEGIN (L"Eigen: Get eigenvector element...", L"djmw", 20040225)
INTRO (L"A command to query the selected @Eigen for the %j^^th^ element of the "
	"%i^^th^ eigenvector.")
MAN_END

MAN_BEGIN (L"Eigen & Matrix: Project...", L"djmw", 20040407)
INTRO (L"A command to project the columns of the @Matrix object onto the "
	"eigenspace of the @Eigen object.")
ENTRY (L"Setting")
TAG (L"##Number of dimensions")
DEFINITION (L"defines the dimension, i.e., the number of rows, of the "
	"resulting object.")
ENTRY (L"Algorithm")
NORMAL (L"Project each column of the Matrix on the coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
FORMULA (L"%y__%ji_ = \\Si__%k=1..%numberOfColums_ %e__jk_ %x__%ki_, where")
NORMAL (L"%y__%ji_ is the %j-th element of the %i-th column of the resulting "
	"(matrix) object, %e__%jk_ is the %k-th element of the %j-th eigenvector "
	"and, %x__%ki_ is the %k-th element of the %i-th column of the selected "
	"matrix object.")
MAN_END

MAN_BEGIN (L"Eigen & SSCP: Project", L"djmw", 20020328)
INTRO (L"A command to project the @SSCP object onto the eigenspace of "
	"the @Eigen object.")
ENTRY (L"Behaviour")
NORMAL (L"Transform the SSCP object as if it was calculated in a coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
FORMULA (L"#%S__%t_ = #%E\\'p #%S #%E, where")
NORMAL (L"where #%E\\'p is the transpose of the matrix with eigenvectors #%E, "
	"#%S is the square matrix with sums of squares and crossproducts, and "
	"#%S__%t_ the newly created square matrix. The dimension of #%S__%t_ may "
	"be smaller than the dimension of #%S.")
MAN_END

MAN_BEGIN (L"Eigen & TableOfReal: Project...", L"djmw", 20040407)
INTRO (L"A command to project the rows of the @TableOfReal object onto the "
	"eigenspace of the @Eigen object.")
ENTRY (L"Setting")
TAG (L"##Number of dimensions")
DEFINITION (L"defines the number of dimensions, i.e., the number of columns, of the "
	"resulting object.")
ENTRY (L"Algorithm")
NORMAL (L"Project each row of the TableOfReal on the coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
FORMULA (L"%y__%ij_ = \\Si__%k=1..%numberOfColums_ %e__jk_ %x__%ik_, where")
NORMAL (L"%e__%jk_ is the %k-th element of the %j-th eigenvector, %x__%ik_ is "
	"the %k-th element of the %i-th row and %y__%ij_ is the %j-th element at "
	"the %i-th row of the matrix part of the resulting object.")
MAN_END

MAN_BEGIN (L"equivalent rectangular bandwidth", L"djmw", 19980713)
INTRO (L"The %%equivalent rectangular bandwidth% (ERB) of a filter is defined "
	"as the width of a rectangular filter whose height equals the peak gain of "
	"the filter and which passes the same total power as the filter (given a flat "
	"spectrum input such as white noise or an impulse).")
MAN_END

MAN_BEGIN (L"Excitations", L"djmw", 19960918)
INTRO (L"A collection of objects of type @Excitation. "
	"You can create an #Excitations by selecting one or more #Excitation's and "
	"selecting ##To Excitations#. You can add one or more #Excitation's to an "
	"#Excitations by selecting one #Excitations and one or more "
	"#Excitation's and selecting ##Add to Excitations# (the #Excitation's will "
	"be removed from the list of objects).")
MAN_END

MAN_BEGIN (L"Excitations: Append", L"djmw", 19960918)
INTRO (L"You can choose this command after selecting two objects of type @Excitations. ")
NORMAL (L"A new object is created that contains the second object appended after the first.")
MAN_END

MAN_BEGIN (L"Excitations: To Pattern...", L"djmw", 19960918)
INTRO (L"A command to convert every selected @Excitations to a @Pattern object.")
ENTRY (L"Setting")
TAG (L"##Join")
DEFINITION (L"the number of subsequent @Excitation objects to combine into one row of @Pattern. "
	"E.g. if an #Excitation has length 26 and %join = 2 then each row of #Pattern "
	"contains 52 elements. The number of rows in #Pattern will be %%my size% / 2. "
	"In the conversion process the elements of an #Excitation will be divided by 100.0 in order "
	"to guarantee that all patterns have values between 0 and 1.")
MAN_END

MAN_BEGIN (L"FilterBank: Draw filter functions...", L"djmw", 20030901)
INTRO (L"")
MAN_END

MAN_BEGIN (L"FilterBank: Draw frequency scales...", L"djmw", 20030901)
MAN_END

MAN_BEGIN (L"FilterBank: Get frequency in Hertz...", L"djmw", 20030901)
INTRO (L"A @query to the selected FilterBank object.")
ENTRY (L"Return value")
NORMAL (L"a frequency value in Hertz.")
MAN_END

MAN_BEGIN (L"FilterBank: Get frequency in Bark...", L"djmw", 20030901)
MAN_END

MAN_BEGIN (L"FilterBank: Get frequency in mel...", L"djmw", 20030901)
MAN_END

MAN_BEGIN (L"FormantFilter", L"djmw", 20010404)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type FormantFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%f, %t), expressed "
	"in dB's. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %f__%j_ (on a linear frequency scale).")
ENTRY (L"Inside a FormantFilter")
NORMAL (L"With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN (L"gammatone", L"djmw", 20100517)
INTRO (L"A gammatone is the product of a rising polynomial, a decaying exponential function, and a "
	"cosine wave.")
NORMAL (L"It can be described with the following formula:")
FORMULA (L"gammaTone (%t) = %a %t^^%\\ga\\--1^ e^^\\--2%\\pi\\.c%bandwidth\\.c%t^ "
	"cos (2%%\\pi\\.cfrequency\\.ct% + %initialPhase),")
NORMAL (L"where %\\ga determines the order of the gammatone.")
NORMAL (L"The gammatone function has a monotone carrier (the tone) with an "
	"envelope that is a gamma distribution function. The amplitude spectrum is essentially "
	"symmetric on a linear frequency scale. This function is used in some time-domain "
	"auditory models to simulate the spectral analysis performed by the basilar membrane. "
	"It was popularized in auditory modeling by @@Johannesma (1972)@. @@Flanagan (1960)@ "
	"already used it to model basilar membrane motion.")
MAN_END

MAN_BEGIN (L"generalized singular value decomposition", L"djmw", 19981007)
INTRO (L"For %m > %n, the %%generalized singular value decomposition% (gsvd) of an %m \\xx %n matrix #%A and "
	"a %p \\xx %n matrix #%B is given by the pair of factorizations")
FORMULA (L"#%A = #%U #%\\Si__1_ [#%0, #%R] #%Q\\'p and #%B = #%V #%\\Si__2_ [#%0, #%R] #%Q\\'p")
NORMAL (L"The matrices in these factorizations have the following properties:")
TAG (L"\\bu #%U [%m \\xx %m], #%V [%p \\xx %p] and #%Q [%n \\xx %n]")
DEFINITION (L" are orthogonal matrices. In the reconstruction formula's above we maximally need "
	"only the first %n columns of matrices #%U and #%V (when %m and/or %p are greater than %n).")
TAG (L"\\bu #%R [%r \\xx %r],")
DEFINITION (L"is an upper triangular nonsingular matrix. %r is the rank of [#%A\\'p, #%B\\'p]\\'p "
	"and %r \\<_ %n. The matrix [#%0, #%R] is %r \\xx %n and its first %n \\xx (%n \\-- %r) part "
	"is a zero matrix.")
TAG (L"\\bu #%\\Si__1_ [%m \\xx %r] and #%\\Si__2_ [%p \\xx %r]")
DEFINITION (L"are real, nonnegative and \"diagonal\".")
NORMAL (L"In practice, the matrices #%\\Si__1_ and #%\\Si__2_ are never used. Instead a shorter "
	"representation with numbers %\\al__%i_ and %\\be__%i_ is used. These numbers obey "
	"0 \\<_ \\al__%i_ \\<_ 1 and \\al__%i_^^2^ + \\be__%i_^^2^ = 1. The following relations exist:")
FORMULA (L"#%\\Si__1_\\'p #%\\Si__1_ + #%\\Si__2_\\'p #%\\Si__2_ = #%I, ")
FORMULA (L"#%\\Si__1_\\'p #%\\Si__1_ = #diag (%\\al__1_^^2^, ..., %\\al__%r_^^2^), and, ")
FORMULA (L"#%\\Si__2_\\'p #%\\Si__2_ = #diag (%\\be__1_^^2^, ..., %\\be__%r_^^2^).")
NORMAL (L"The ratios \\al__%i_ / \\be__%i_ are called the %%generalized singular values% of the "
	"pair #%A, #%B. Let %l be the rank of #%B and %k + %l (= %r) the rank of [#%A\\'p, #%B\\'p]\\'p. "
	"Then the first %k generalized singular values are infinite and the remaining %l are finite. "
	"(When %#B is of full rank then, of course, %k = 0).")
ENTRY (L"Special cases")
NORMAL (L"\\bu If #%B is a square nonsingular matrix, the gsvd of #%A and #%B is equivalent to the "
	"singular value decomposition of #%A #%B^^\\--1^.")
NORMAL (L"\\bu The generalized eigenvalues and eigenvectors of #%A\\'p #%A - %\\la  #%B\\'p #%B "
	"can be expressed in terms of the gsvd. The columns of the matrix #%X, constructed as")
CODE (L"X = Q*( I   0    )")
CODE (L"      ( 0 inv(R) ),")
NORMAL (L"form the eigenvectors. The important eigenvectors, of course, correspond "
	"to the positions where the %l eigenvalues are not infinite.")
MAN_END

MAN_BEGIN (L"invFisherQ", L"djmw", 20000525)
INTRO (L"$$invFisherQ$ (%q, %df1, %df2) returns the value %f for which "
	"$$@fisherQ (%f, %df1, %df2) = %q.")
MAN_END

MAN_BEGIN (L"fisherQ", L"djmw", 20000525)
INTRO (L"$$fisherQ$ (%f, %df1, %df2) returns the area under Fisher's F-distribution "
	"from %f to +\\oo.")
MAN_END

MAN_BEGIN (L"ISpline", L"djmw", 19990627)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}. ")
NORMAL (L"An object of type ISpline represents a linear combination of basis "
	"i@spline functions. Each basis %ispline is a monotonically increasing "
	"polynomial function of degree %p.")
FORMULA (L"ISpline (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %ispline__%k_(%x)")
MAN_END

MAN_BEGIN (L"Jackknife", L"djmw", 20031103)
INTRO (L"A technique for estimating the bias and standard deviation of an "
	"estimate.")
NORMAL (L"Suppose we have a sample #%x = (%x__1_, %x__2_,...%x__n_) and wish to estimate "
	"the bias and standard error of an estimator \\Te. The jackknife "
	"focuses on the samples that leave out one observation at a time: "
	"the %i-th jackknife sample consists of the data with the %i-th observation "
	"removed.")
MAN_END

MAN_BEGIN (L"Kirshenbaum phonetic encoding", L"djmw", 20120413)
INTRO (L"The Kirshenbaum phonetic encoding represents International Phonetic Alphabet symbols using ascii characters. See: http://www.kirshenbaum.net/IPA/ascii-ipa.pdf. The @@espeak@ speech synthesizer on which our synthesizer is based accepts this encoding as text input. ")
MAN_END

MAN_BEGIN (L"Legendre polynomials", L"djmw", 19990620)
INTRO (L"The Legendre polynomials %P__%n_(%x) of degree %n are special "
	"orthogonal polynomial functions defined on the domain [-1, 1].")
NORMAL (L"Orthogonality:")
FORMULA (L"__-1_\\in^^1^ %W(%x) %P__%i_(%x) %P__%j_(%x) %dx = \\de__%ij_")
FORMULA (L"%W(%x) = 1    (-1 < x < 1)")
NORMAL (L"They obey certain recurrence relations:")
FORMULA (L"%n %P__%n_(%x) = (2%n \\-- 1) %x %P__%n-1_(%x) \\-- (%n \\-- 1) %P__%n-2_(%x)")
FORMULA (L"%P__0_(%x) = 1")
FORMULA (L"%P__1_(%x) = %x")
NORMAL (L"We may %change the domain of these polynomials to [%xmin, %xmax] by "
	"using the following transformation:")
FORMULA (L"%x\\'p = (2%x \\-- (%xmax + %xmin)) / (%xmax - %xmin).")
NORMAL (L"We subsequently use %P__%k_(%x\\'p) instead of %P__%k_(%x).")
MAN_END

MAN_BEGIN (L"LegendreSeries", L"djmw", 19990620)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type LegendreSeries represents a linear combination of @@Legendre polynomials@ "
	"%P__%k_(%x).")
FORMULA (L"LegendreSeries (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %P__%k_(%x)")
MAN_END

MAN_BEGIN (L"LegendreSeries: To Polynomial", L"djmw", 19990620)
INTRO (L"A command to transform the selected @LegendreSeries object into a @@Polynomial@ object.")
NORMAL (L"We find polynomial coefficients %c__%k_ such that")
FORMULA (L"\\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x^^%k^ = "
	"\\Si__%k=1..%numberOfCoefficients_ %l__%k_ %P__%k_(%x)")
NORMAL (L"We use the recurrence relation for @@Legendre polynomials@ to calculate these coefficients.")
MAN_END

MAN_BEGIN (L"Matrix: Draw distribution...", L"djmw", 20041110)
INTRO (L"A command to draw the distribution histogram of the values in the selected part of a @Matrix.")
ENTRY (L"Settings")
TAG (L"##Horizontal range#, ##Vertical range#")
DEFINITION (L"determine the part of the matrix whose distribution will be drawn.")
TAG (L"##Minimum value#, ##Maximum value#")
DEFINITION (L"determine the range of values that will be considered in the distribution. "
	"To treat all bin widths equally, the range will include the %%Minimum value% and exclude the "
	"%%Maximum value% (see below).")
TAG (L"##Number of bins")
DEFINITION (L"determines the number of bars in the distribution histogram.")
TAG (L"##Minimum frequency#, ##Maximum frequency#")
DEFINITION (L"determine the limits of the vertical axis.")
ENTRY (L"Algorithm")
NORMAL (L"For a particular matrix element %z, the histogram bin number %%i% that will be incremented obeys the following relation:")
FORMULA (L"%%lowerBinBorder%__%i_ \\<_ %z <  %%lowerBinBorder%__%i_+ %%binWidth%,")
NORMAL (L"where")
FORMULA (L"%%binWidth% = (%%maximumValue% - %%minimumValue%)/%%numberOfBins%,")
NORMAL (L"and")
FORMULA (L"%%lowerBinBorder%__%i_ = %%minimumValue% + (%i - 1)\\.c%%binWidth%.")
NORMAL (L"In this way all bins will be based on exactly the same width, as each binning interval includes its lower border "
	"and excludes its upper border "
	"(i.e., each interval is closed to the left and open to the right). ")
MAN_END

MAN_BEGIN (L"Matrix: Solve equation...", L"djmw", 19961006)
INTRO (L"Solve the general matrix equation #A #x = #b for #x.")
NORMAL (L"The matrix #A can be any general %m \\xx %n matrix, #b is a %m-dimensional "
	"and #x a %n-dimensional vector. The @Matrix contains #A as its first %n columns "
	"and #b as its last column. The %n-dimensional solution is returned as a #Matrix "
	"with %n columns.")
NORMAL (L"When the number of equations (%m) is %greater than the number of unknowns (%n) the "
	"algorithm gives the best least-squares solution. If on the contrary you "
	"have %fewer equations than unknowns the solution will not be unique.")
ENTRY (L"Method")
NORMAL (L"Singular value decomposition with backsubstitution. "
	"Zero will be substituted for eigenvalues smaller than %tolerance \\.c "
	"%%maximum_eigenvalue% (when the user-supplied %tolerance equals 0.0 a "
	"value of 2.2 \\.c 10^^-16^ \\.c %%number_of_unknowns% "
	"will be used as %tolerance).")
NORMAL (L"See for more details: @@Golub & van Loan (1996)@ chapters 2 and 3.")
MAN_END

MAN_BEGIN (L"MelFilter", L"djmw", 20120724)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type MelFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%f, %t), "
	"expressed in dB's. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %f__%j_ (on a Mel frequency scale).")
NORMAL (L"The frequency in mels is:")
FORMULA (L"mels = 2595 * log10 (1 + hertz / 700),")
NORMAL (L"and its inverse is:")
FORMULA (L"hertz = 700 * (10.0^^mel / 2595.0^ - 1).")
ENTRY (L"Inside a MelFilter")
NORMAL (L"With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN (L"MelFilter: To MFCC...", L"djmw", 20130221)
INTRO (L"A command to create a @MFCC object from each selected @MelFilter "
	"object.")
NORMAL (L"Mel frequency cepstral coefficients result from the Discrete Cosine "
	"Transform of the filterbank spectrum (in dB). The following formula "
	"shows the relation:")
FORMULA (L"%c__%k-1_ = \\Si__%j=1_^^%N^ %P__%j_ cos (\\pi(%k-1)(%j-0.5)/%N)),")
NORMAL (L"where %N represents the number of filters and %P__%j_ the power in dB "
	"in the %j^^%th^ filter (%k runs from 1 to %N).")
NORMAL (L"This transformation was first used by @@Davis & Mermelstein (1980)@.")
MAN_END

MAN_BEGIN (L"MFCC: To TableOfReal...", L"djmw", 20120504)
INTRO (L"Convert the selected @@MFCC@ object to a @@TableOfReal@ object. Each MFCC frame results "
	"in one row in the TableOfReal. If the \"Include energy\" option is chosen, the zeroth MFCC "
	"coefficient will be saved in the first column.")
MAN_END

MAN_BEGIN (L"MSpline", L"djmw", 19990627)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}. ")
NORMAL (L"An object of type MSpline represents a linear combination of basis "
	"m@spline functions. Each basis %mspline is a positive polynomial function "
	"of degree %p.")
FORMULA (L"MSpline (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %mspline__%k_(%x)")
MAN_END

MAN_BEGIN (L"Pattern", L"djmw", 20041201)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
INTRO (L"An object of type Pattern represents a sequence of patterns that can serve as "
	"inputs for a neural net. All elements in a Pattern have to be in the interval [0,1].")
ENTRY (L"Pattern commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu ##Create Pattern with zeroes...#")
LIST_ITEM (L"\\bu @@TableOfReal: To Pattern and Categories...@")
NORMAL (L"Synthesis:")
LIST_ITEM (L"\\bu @@FFNet & Pattern: To Categories...@")
LIST_ITEM (L"\\bu @@Pattern & Categories: To FFNet...@")
ENTRY (L"Inside a Pattern")
NORMAL (L"With @Inspect you will see that this type contains the same "
	"attributes as a @Matrix.")
MAN_END

MAN_BEGIN (L"PCA", L"djmw", 19990323)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}. "
	"See the @@Principal component analysis@ tutorial.")
NORMAL (L"An object of type PCA represents the principal components analysis "
	"of a multivariate dataset.")
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Principal component analysis@ tutorial")
LIST_ITEM (L"\\bu @@TableOfReal: To PCA@")
ENTRY (L"Inside a PCA")
NORMAL (L"With @Inspect you will see that this type contains the same "
	"attributes as an @Eigen with the following extras:")
TAG (L"%numberOfObservations")
DEFINITION (L"the number of observations in the multivariate dataset that originated the PCA, "
	"usually equal to the dataset's number of rows.")
TAG (L"%labels[1..%dimension]")
DEFINITION (L"the label that corresponds to each dimension.")
TAG (L"%centroid")
DEFINITION (L"the centroids of the originating multivariate data set.")
MAN_END

MAN_BEGIN (L"PCA: Get fraction variance accounted for...", L"djmw", 19990106)
INTRO (L"A command to query the selected @PCA for the fraction %%variance "
	"accounted for% by the selected components.")
ENTRY (L"Setting")
TAG (L"##Principal component range")
DEFINITION (L"defines the range of the principal components. If you choose both numbers equal, "
	"you get the fraction of the \"variance\" explained by that one component.")
ENTRY (L"Details")
NORMAL (L"The contribution is defined as:")
FORMULA (L"\\Si__%i=%from..%to_ %%eigenvalue[i]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN (L"PCA: Get eigenvalue...", L"djmw", 20040225)
INTRO (L"A command to query the selected @PCA for the %i^^th^ "
	"eigenvalue.")
MAN_END

MAN_BEGIN (L"PCA: Get eigenvector element...", L"djmw", 20040225)
INTRO (L"A command to query the selected @PCA for the %j^^th^ element of the "
	"%i^^th^ eigenvector.")
MAN_END

MAN_BEGIN (L"PCA: Get equality of eigenvalues...",L"djmw", 19981102)
INTRO (L"A command to get the probability that some of the eigenvalues of the "
	"selected @PCA object are equal. A low probability means that it is not "
	"very likely that that these numbers are equal.")
NORMAL (L"We test the hypothesis %H__0_: %\\la__%from_ = ... = %\\la__%to_ "
	"that %r (= %to\\--%from+1) of the eigenvalues \\la of the covariance "
	"matrix are equal. The remaining eigenvalues are unrestricted as to their "
	"values and multiplicities. The alternative hypothesis to %H__0_ is that "
	"some of the eigenvalues in the set are distinct.")
ENTRY (L"Settings")
TAG (L"##Eigenvalue range")
DEFINITION (L"define the range of eigenvalues to be tested for equality.")
TAG (L"##Conservative test")
DEFINITION (L"when on, a more conservative estimate for %n is chosen (see below).")
ENTRY (L"Details")
NORMAL (L"The test statistic is:")
FORMULA (L"\\ci^2 = \\--%n \\Si__%j=%from..%to_ ln %eigenvalue[%j] + %n %r "
	"ln (\\Si__%j=%from..%to_ %eigenvalue[%j] / %r),")
NORMAL (L"with %r(%r+1)/2 \\--1 degrees of freedom. Here %n = %totalNumberOfCases \\-- 1.")
NORMAL (L"A special case occurs when the variation in the last %r dimensions is spherical. In a "
	"slightly more conservative test we may replace %n by %n\\--%from\\--(2%r^2+%r+2)/6%r.")
NORMAL (L"Also see @@Morrison (1990)@, page 336.")
MAN_END

MAN_BEGIN (L"PCA: Get number of components (VAF)...", L"djmw", 19990111)
INTRO (L"A command to ask the selected @PCA for the minimum number of "
	"components that are necessary "
	"to explain the given fraction %%variance accounted for%.")
ENTRY (L"Setting")
TAG (L"##Variance accounted for (fraction)")
DEFINITION (L"the fraction variance accounted for that must be explained.")
MAN_END

MAN_BEGIN (L"PCA: To TableOfReal (reconstruct 1)...", L"djmw", 20030108)
INTRO (L"A command to reconstruct a single data item. The result is stored as "
	"a @TableOfReal with only one row.")
ENTRY (L"Settings")
TAG (L"##Coefficients")
DEFINITION (L"the weight for the eigenvectors.")
NORMAL (L"The algorithm is explained in @@PCA & Configuration: To TableOfReal "
	"(reconstruct)@.")
MAN_END

MAN_BEGIN (L"PCA & Configuration: To TableOfReal (reconstruct)", L"djmw", 20030108)
INTRO (L"A command to reconstruct a @TableOfReal from the selected @Configuration"
	" and @PCA.")
NORMAL (L"The TableOfReal is reconstructed from the eigenvectors of the PCA and "
	"elements of the Configuration are the weight factors: ")
FORMULA (L"%#t__%i_ = \\Si__%k_ %c__%ik_ #%e__%k_,")
NORMAL (L"where %#t__%i_ is the %i-th row in the resulting TableOfReal object, %c__%ik_ is "
	"the element at row %i and column %k in the Configuration object and #%e__%k_ "
	"the %k-th eigenvector from the PCA object.")
MAN_END

MAN_BEGIN (L"PCA & PCA: Get angle between pc1-pc2 planes", L"djmw", 20041028)
INTRO (L"A command to calculate the angle between two planes. Each plane is spanned by the first "
	"two eigenvectors from the corresponding @@PCA@.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm is described in section 12.4.3 of @@Golub & van Loan (1996)@:")
NORMAL (L"First we form the projection of one set of eigenvectors on the other set. "
	"This results in a 2\\xx2 matrix #C:")
FORMULA (L"#C = #E__1_\\'p #E__2_,")
NORMAL (L"where #E__1_ and #E__2_ are 2\\xx%%dimension% and %%dimension%\\xx2 matrices "
	"that contain the first two eigenvectors of the PCA's, respectively.")
NORMAL (L"Next, we compute the @@singular value decomposition@ of #C:")
FORMULA (L"#C = #U #\\Si #V\\'p")
NORMAL (L"Now the cosine of the angle between the two planes is given by \\si__2_ and "
	"the angle in degrees is therefore:")
FORMULA (L"arccos (\\si__2_)\\.c180/\\pi")
MAN_END

MAN_BEGIN (L"PCA & PCA: To Procrustes...", L"djmw", 20041028)
INTRO (L"A command to calculate a @Procrustes from the two selected @@PCA@'s.")
NORMAL (L"Determines the orthogonal @@Procrustes transform@.")
NORMAL (L"Algorithm 12.4.1 in @@Golub & van Loan (1996)@.")
MAN_END

MAN_BEGIN (L"PCA & TableOfReal: To Configuration...", L"djmw", 19990111)
INTRO (L"A command to construct a @Configuration from the selected @TableOfReal"
	" and @PCA.")
ENTRY (L"Setting")
TAG (L"##Number of dimensions")
DEFINITION (L"determines the number of dimensions of the resulting Configuration.")
ENTRY (L"Algorithm")
NORMAL (L"The TableOfReal is projected on the eigenspace of the PCA, i.e., "
	"each row of the TableOfReal is treated as a vector, and the inner product "
	"with the eigenvectors of the PCA determine its coordinates in the Configuration.")
NORMAL (L"Because the algorithm performs a projection, the resulting Configuration will "
	"##only be centered#, i.e., its centroid will be at ##0#, if the data in the "
	"TableOfReal object are centered too. ")
NORMAL (L"See also @@Eigen & TableOfReal: Project...@.")
MAN_END

MAN_BEGIN (L"PCA & TableOfReal: To TableOfReal (z-scores)...", L"djmw", 20120510)
INTRO (L"A command to construct a @TableOfReal with z-scores from the selected @TableOfReal"
	" and @PCA.")
ENTRY (L"Setting")
TAG (L"##Number of dimensions")
DEFINITION (L"determines the number of dimensions of the resulting Configuration.")
ENTRY (L"Algorithm")
NORMAL (L"The values %d__%ij_ in the new TableOfReal are calculated as")
FORMULA (L"%d__%ij_ = ##eigenvector#__j_\\.c ##z#__%i_,")
NORMAL (L"which is the inproduct of the %j-th eigenvector and the z-score vector ##z#__%i_ of the %i-th row whose elements %z__%ij_ are defined as")
FORMULA (L"%z__%ij_ = (data__%ij_ - mean__%j_) / sqrt (eigenvalue__%j_),")
NORMAL (L"in which data__%ij_ is the data value at row %i and column %j of the selected TableOfReal and mean__%j_ is the "
	"%j-th centroid value of the PCA. The square root of the %j-th eigenvalue is the standard deviation in "
	" the %j-th principal direction.")
MAN_END

MAN_BEGIN (L"PCA & TableOfReal: Get fraction variance...", L"djmw", 20040324)
INTRO (L"A command to query the selected @PCA and @TableOfReal object for the explained "
	"fraction of the variance if the TableOfReal object were projected onto the PCA space.")
ENTRY (L"Algorithm")
LIST_ITEM (L"1. The TableOfReal is @@TableOfReal: To Covariance|converted@ to a "
	"Covariance object.")
LIST_ITEM (L"2. The Covariance object is @@PCA & Covariance: Project|projected@ on "
	"the PCA eigenspace and the newly obtained projected Covariance object is "
	"@@Covariance: Get fraction variance...|queried@ for the fraction variance.")
MAN_END

MAN_BEGIN (L"PitchTier: To Pitch...", L"djmw", 20061128)
INTRO (L"Synthesizes a new @Pitch from the selected @PitchTier.")
MAN_END

MAN_BEGIN (L"Polygon: Rotate...", L"djmw", 20100418)
INTRO (L"Rotates the selected @@Polygon@ counterclockwise with respect to the given coordinates.")
MAN_END

MAN_BEGIN (L"Create simple Polygon...", L"djmw", 20130409)
INTRO (L"Creates a @@Polygon@ from user  supplied x/y pairs.")
ENTRY (L"Settings")
TAG (L"##Name")
DEFINITION (L"defines the name of the resulting Polygon.")
TAG (L"##Vertices as X-Y pairs#,")
DEFINITION (L"defines the x-y values of the vertices of the Polygon. The Polygon will be automatically closed, i.e., the first and the last point will be connected.")
ENTRY (L"Example")
NORMAL (L"The command ##Create simple Polygon... p 0.0 0.0  0.0 1.0  1.0 0.0# defines  a Polygon with three points. In the figure the three points are indicated with open circles while the Polygon is drawn as a closed figure.")
SCRIPT (4,4, L"do (\"Create simple Polygon...\", \"p\", \"0.0 0.0 0.0 1.0 1.0 0.0\")\n"
	"do (\"Draw circles...\", 0, 1, 0, 1, 3)\n"
	"do (\"Draw closed...\", 0, 1, 0, 1)\n"
	"do (\"Remove\")\n")
MAN_END

MAN_BEGIN (L"Polygon: Get location of point...", L"djmw", 20120220)
INTRO (L"Determines whether a given point is on the ##I#nside, the ##O#utside, on an ##E#dge or on a ##V#ertex of the selected Polygon.")
ENTRY (L"Algorithm")
NORMAL (L"We determine how often a horizontal line extending from the point crosses the polygon. If the number of crossings is even, the point is on the outside, else on the inside. Special care is taken to be able to detect if a point is on the boundary of the polygon. The used algorithm is from @@Hormann & Agathos (2001)@")
MAN_END

MAN_BEGIN (L"Polygon: Simplify", L"djmw", 20130502)
INTRO (L"Removes collinear vertices from a @@Polygon@.")
ENTRY (L"Example")
SCRIPT (4, 4,
	L"p1 = do (\"Create simple Polygon...\", \"p\", \"0.0 0.0 0.0 1.0 0.5 0.5 1.0 0.0 0.5 0 0 -0.5 0 -0.25\")\n"
	"do (\"Draw closed...\", 0, 0, 0, 0)\n"
	"do (\"Colour...\", \"Red\")\n"
	"do (\"Draw circles...\", 0, 0, 0, 0, 3)\n"
	"p2 = do (\"Simplify\")\n"
	"do (\"Colour...\", \"Black\")\n"
	"do (\"Paint circles...\", 0, 0, 0, 0, 1.5)\n"
	"removeObject (p1, p2)\n"
)
NORMAL (L"Given the Polygon with the seven vertices indicated by the red open circles, the Simplify action results in the Polygon with four vertices indicated by the filled black circles.")
MAN_END

MAN_BEGIN (L"Polygon: Translate...", L"djmw", 20100418)
INTRO (L"Translates the selected @@Polygon@ over the given vector.")
NORMAL (L"Given the old coordinates (x__i_, y__i_) and the translation (x__t_,y__t_), the new coordinates are:")
FORMULA (L"x__i_\\'p = x__i_ + x__t_")
FORMULA (L"y__i_\\'p = y__i_ + y__t_")
MAN_END

MAN_BEGIN (L"Polynomial", L"djmw", 19990608)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type "
	"Polynomial represents a polynomial function on a domain.")
NORMAL (L"A polynomial of degree %n is defined as:")
FORMULA (L"%p(%x) = %c__1_ + %c__2_ %x + %c__3_ %x^^2^ + ... c__%n+1_ %x^^%n^.")
NORMAL (L"The real numbers %c__%k_ are called the polynomial %coefficients.")
ENTRY (L"Commands")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu @@Create Polynomial...@ (in the ##New menu#)")
LIST_ITEM (L"\\bu @@LPC: To Polynomial (slice)...@ (from prediction coefficients)")
LIST_ITEM (L"\\bu @@LegendreSeries: To Polynomial@")
LIST_ITEM (L"\\bu @@ChebyshevSeries: To Polynomial@")
NORMAL (L"Drawing")
LIST_ITEM (L"\\bu ##Draw...#")
NORMAL (L"Queries")
LIST_ITEM (L"\\bu @@Polynomial: Get function value...|Get function value...@: get %p(%x)")
LIST_ITEM (L"\\bu ##Get coefficient value...#: get %c__%i_")
LIST_ITEM (L"\\bu @@Polynomial: Get minimum...|Get minimum...@: minimum of %p(%x) on an interval")
LIST_ITEM (L"\\bu @@Polynomial: Get x of minimum...|Get x of minimum...@")
LIST_ITEM (L"\\bu @@Polynomial: Get maximum...|Get maximum...@: maximum of %p(%x) on an interval")
LIST_ITEM (L"\\bu @@Polynomial: Get x of maximum...|Get x of maximum...@")
LIST_ITEM (L"\\bu @@Polynomial: Get area...|Get area...@")
NORMAL (L"Modification")
LIST_ITEM (L"\\bu ##Set domain...#: new domain")
LIST_ITEM (L"\\bu ##Set coefficient value...#: change one coefficient")
NORMAL (L"Conversion")
LIST_ITEM (L"\\bu @@Polynomial: To Spectrum...|To Spectrum...@ (evaluation over unit-circle)")
LIST_ITEM (L"\\bu @@Polynomial: To Polynomial (derivative)|To Polynomial (derivative)@")
LIST_ITEM (L"\\bu @@Polynomial: To Polynomial (primitive)|To Polynomial (primitive)@")
LIST_ITEM (L"\\bu @@Polynomial: To Roots|To Roots@: roots of polynomial")
MAN_END

MAN_BEGIN (L"Polynomial: Get area...", L"djmw", 19990610)
INTRO (L"A command to compute the area below the selected @Polynomial object.")
ENTRY (L"Settings")
TAG (L"##Xmin#, ##Xmax#")
DEFINITION (L"define the interval.")
NORMAL (L"The area is defined as __%xmin_\\in^^xmax^ %p(%x) %dx.")
MAN_END

MAN_BEGIN (L"Polynomial: Get function value...", L"djmw", 19990610)
INTRO (L"A command to compute %p(%x) for the selected @Polynomial object.")
MAN_END

MAN_BEGIN (L"Polynomial: Get maximum...", L"djmw", 19990610)
INTRO (L"A command to compute, on a specified interval, the maximum value of the selected "
	"@Polynomial object.")
MAN_END

MAN_BEGIN (L"Polynomial: Get x of maximum...", L"djmw", 19990610)
INTRO (L"A command to compute, on a specified interval, the location of the maximum of the "
	"selected @Polynomial object.")
MAN_END

MAN_BEGIN (L"Polynomial: Get minimum...", L"djmw", 19990610)
INTRO (L"A command to compute, on a specified interval, the minimum value of the selected "
	"@Polynomial object.")
MAN_END

MAN_BEGIN (L"Polynomial: Get x of minimum...", L"djmw", 19990610)
INTRO (L"A command to compute, on a specified interval, the location of the minimum of the "
	"selected @Polynomial object.")
MAN_END

MAN_BEGIN (L"Polynomials: Multiply", L"djmw", 19990616)
INTRO (L"A command to multiply two @@Polynomial|polynomials@ with each other.")
NORMAL (L"The result of multiplying 1 + 2 %x and 2 \\-- %x^2 will be the polynomial:")
FORMULA (L"2 + 4 %x \\-- %x^2 \\-- 2 %x^3.")
MAN_END

MAN_BEGIN (L"Polynomial: To Polynomial (derivative)", L"djmw", 19990610)
INTRO (L"A command to compute the derivative of the selected @Polynomial object.")
MAN_END

MAN_BEGIN (L"Polynomial: To Polynomial (primitive)", L"djmw", 19990610)
INTRO (L"A command to compute the primitive of the selected @Polynomial object.")
MAN_END

MAN_BEGIN (L"Polynomial: Scale x...", L"djmw", 19990610)
INTRO (L"A command to transform the selected @Polynomial object to a new domain.")
TAG (L"##Xmin# and ##Xmax#")
DEFINITION (L"define the new domain.")
ENTRY (L"Behaviour")
NORMAL (L"The polynomial is transformed from domain [%x__min_, %x__max_] to "
	"domain [%Xmin, %Xmax] in such a way that its form stays the same. "
	"This is accomplished by first calculating:")
FORMULA (L"%f(%x\\'p) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x\\'p^^%k^, where")
FORMULA (L"%x\\'p = %a %x + %b,")
NORMAL (L"and then collecting terms of equal degree. The %a and %b are defined as")
FORMULA (L"%a = (%x__min_ \\-- %x__max_) / (%Xmin \\-- %Xmax)")
FORMULA (L"%b = %x__min_ \\-- %a %Xmin")
MAN_END

MAN_BEGIN (L"Polynomial: To Roots", L"djmw", 19990608)
INTRO (L"A command to compute the @@Roots|roots@ of the selected @Polynomial objects.")
ENTRY (L"Algorithm")
NORMAL (L"The roots are found from the polished eigenvalues of a special companion matrix. "
	"For further explanation on these methods see @@Press et al. (1992)@.")
MAN_END

MAN_BEGIN (L"Polynomial: To Spectrum...", L"djmw", 19990616)
INTRO (L"A command to compute the @@Spectrum|spectrum@ of the selected @Polynomial objects.")
ENTRY (L"Settings")
TAG (L"##Nyquist frequency (Hz)")
DEFINITION (L"defines the highest frequency in the spectrum. The lowest frequency of the spectrum "
	"will be 0 Hz.")
TAG (L"##Number of frequencies")
DEFINITION (L"defines the number of frequencies in the spectrum.")
ENTRY (L"Algorithm")
NORMAL (L"We calculate the spectrum by evaluating the polynomial at regularly spaced points %z__%k_ "
	"on the upper half of a circle with radius %r = 1 in the complex plane. The upperhalf of the "
	"unit circle, where %k\\.c%\\fi is in the interval [0, %\\pi], will be mapped to frequencies "
	"[0, @@Nyquist frequency@] in the spectrum. ")
NORMAL (L"The complex values %z__%k_ (%k=1..%numberOfFrequencies) are defined as:")
FORMULA (L"%z__%k_ = %r e^^%i %k %\\fi^, where,")
FORMULA (L"%\\fi = \\pi / (%numberOfFrequencies \\-- 1) and %r = 1.")
MAN_END

MAN_BEGIN (L"Principal component analysis", L"djmw", 20120510)
INTRO (L"This tutorial describes how you can perform principal component "
       "analysis with P\\s{RAAT}.")
NORMAL (L"Principal component analysis (PCA) involves a mathematical procedure "
	"that transforms a number of (possibly) correlated variables into a "
	"(smaller) number of uncorrelated variables called %%principal "
	"components%. The first principal component accounts for as much of the "
	"variability in the data as possible, and each succeeding component "
	"accounts for as much of the remaining variability as possible.")
ENTRY (L"1. Objectives of principal component analysis")
LIST_ITEM (L"\\bu To discover or to reduce the dimensionality of the data set.")
LIST_ITEM (L"\\bu To identify new meaningful underlying variables.")
ENTRY (L"2. How to start")
NORMAL (L"We assume that the multi-dimensional data have been collected in a @TableOfReal data matrix, "
	"in which the rows are associated with the cases and the columns with the variables.")
NORMAL (L"Traditionally, principal component analysis is performed on the "
	"symmetric @@Covariance|Covariance@ matrix or on the symmetric @@correlation|Correlation@ matrix. "
	"These matrices can be calculated from the data matrix. "
	"The covariance matrix contains scaled @@SSCP|sums of squares and cross products@. "
	"A correlation matrix is like a covariance matrix but first the variables, i.e. the columns, have been standardized.  "
	"We will have to standardize the data first if the variances of "
	"variables differ much, or if the units of measurement of the "
	"variables differ. You can standardize the data in the TableOfReal by choosing @@TableOfReal: Standardize columns|Standardize columns@.")
NORMAL (L"To perform the analysis, we select the TabelOfReal data matrix in the list of objects and choose "
	"@@TableOfReal: To PCA|To PCA@. This results in a new PCA object in the "
	"list of objects.")
NORMAL (L"We can now make a @@Scree plot|scree@ plot of the eigenvalues, @@Eigen: Draw "
	"eigenvalues...|Draw eigenvalues...@ "
	"to get an indication of the importance of each eigenvalue. The exact "
	"contribution of each eigenvalue (or a range of eigenvalues) to the "
	"\"explained variance\" can also be queried: "
	"@@PCA: Get fraction variance accounted for...|Get fraction variance "
	"accounted for...@. You might also check for the equality of a "
	"number of eigenvalues: @@PCA: Get equality of eigenvalues...|Get equality "
	"of eigenvalues...@.")
ENTRY (L"3. Determining the number of components")
NORMAL (L"There are two methods to help you to choose the number of components. "
	"Both methods are based on relations between the eigenvalues.")
LIST_ITEM (L"\\bu Plot the eigenvalues, @@Eigen: Draw eigenvalues...|"
	"Draw eigenvalues...@. If the points on the graph tend to level out (show an \"elbow\"), "
	"these eigenvalues are usually close enough to zero that they can be "
	"ignored.")
LIST_ITEM (L"\\bu Limit the number of components to that number that accounts for a certain fraction of the total variance. For example, if you are satisfied with 95% of the total variance explained then use the number you get by the query ##Get number of components (VAF)... 0.95#.")
ENTRY (L"4. Getting the principal components")
NORMAL (L"Principal components are obtained by projecting the multivariate "
	"datavectors on the space spanned by the eigenvectors. This can be done "
	"in two ways:")
LIST_ITEM (L"1. Directly from the TableOfReal without first forming a "
	"@PCA object: "
	"@@TableOfReal: To Configuration (pca)...|To Configuration (pca)...@. "
	"You can then draw the Configuration or display its numbers. ")
LIST_ITEM (L"2. Select a PCA and a TableOfReal object together and choose "
	"@@PCA & TableOfReal: To Configuration...|To Configuration...@. "
	"In this way you project the TableOfReal onto the PCA's eigenspace.")
ENTRY (L"5. Mathematical background on principal component analysis")
NORMAL (L"The mathematical technique used in PCA is called eigen analysis: "
	"we solve for the eigenvalues and eigenvectors of a square symmetric "
	"matrix with sums of squares and cross products. "
	"The eigenvector associated with the largest eigenvalue has the same "
	"direction as the first principal component. The eigenvector associated "
	"with the second largest eigenvalue determines the direction of the second "
	"principal component. "
	"The sum of the eigenvalues equals the trace of the square matrix and the "
	"maximum number of eigenvectors equals the number of rows (or columns) of "
	"this matrix.")
ENTRY (L"6. Algorithms")
NORMAL (L"If our starting point happens to be a symmetric matrix like the covariance matrix, "
	"we solve for the eigenvalue and eigenvectors "
	"by first performing a Householder reduction to tridiagonal form, followed"
	" by the QL algorithm with implicit shifts.")
NORMAL (L"If, conversely, our starting point is the data matrix #%A , "
	"we do not have to form explicitly the matrix with sums of squares and "
	"cross products, #%A\\'p#%A. Instead, we proceed by a numerically more "
	"stable method, and form the @@singular value decomposition@ of #%A, "
	"#%U #%\\Si #%V\\'p. The matrix #%V then contains the eigenvectors, "
	"and the squared diagonal elements of #%\\Si contain the eigenvalues.")
MAN_END

MAN_BEGIN (L"PCA & Covariance: Project", L"djmw", 20040225)
INTRO (L"A command to project the @Covariance object onto the eigenspace of "
	"the @PCA object. ")
NORMAL (L"Further details can be found in @@Eigen & SSCP: Project@.")
MAN_END

MAN_BEGIN (L"PCA & SSCP: Project", L"djmw", 20040225)
INTRO (L"A command to project the @SSCP object onto the eigenspace of "
	"the @PCA object. ")
NORMAL (L"Further details can be found in @@Eigen & SSCP: Project@.")
MAN_END

MAN_BEGIN (L"Regular expressions", L"djmw", 20010706)
INTRO (L"This tutorial describes the %syntax of regular expressions in P\\s{RAAT} ")
ENTRY (L"Introduction")
NORMAL (L"A %%regular expression% is a text string that describes a %set "
	"of strings. Regular expressions (regex) are useful as a way to search "
	"for patterns in text strings and, optionally, replace them by another "
	"pattern.")
NORMAL (L"Some regex match only one string, i.e., the set they describe has "
	"only one member. For example, the regex \"ab\" matches the string \"ab\" "
	"and no others. Other regex match more than one string, i.e., the set "
	"they describe has more than one member. For example, the regex \"a*\" "
	"matches the string made up of any number (including zero) of \"a\"s. "
	"As you can see, some characters match themselves (such as \"a\" and "
	"\"b\") and these characters are called %ordinary characters. The "
	"characters that don't match themselves, such as \"*\", are called "
	"%special characters or %meta characters. Many special characters are only "
	"special characters in the %search regex and are ordinary characters in "
	"the substitution regex. ")
NORMAL (L"You can read the rest of this tutorial sequentially with the help of "
	"the \"<1\" and \">1\" buttons.")
LIST_ITEM (L"1. @@Regular expressions 1. Special characters|Special characters@ "
	"(\\bs \\^  \\$ { } [ ] ( ) . + ? \\|  - &)")
LIST_ITEM (L"2. @@Regular expressions 2. Quantifiers|Quantifiers@ "
	"(how often do we match).")
LIST_ITEM (L"3. @@Regular expressions 3. Anchors|Anchors@ (where do we match)")
LIST_ITEM (L"4. @@Regular expressions 4. Special constructs with parenthesis|"
	"Special constructs with parenthesis@ (grouping constructs)")
LIST_ITEM (L"5. @@Regular expressions 5. Special control characters|"
	"Special control characters@ (difficult-to-type characters like \\bsn)")
LIST_ITEM (L"6. @@Regular expressions 6. Convenience escape sequences|"
	"Convenience escape sequences@ "
	"(\\bsd \\bsD \\bsl \\bsL \\bss \\bsS \\bsw \\bsW \\bsB)")
LIST_ITEM (L"7. @@Regular expressions 7. Octal and hexadecimal escapes|"
	"Octal and hexadecimal escapes@ (things like \\bs053 or \\bsX2B)")
LIST_ITEM (L"8. @@Regular expressions 8. Substitution special characters|"
	"Substitution special characters@ (\\bs1..\\bs9 \\bsU \\bsu \\bsL \\bsl &)")
NORMAL (L"More in depth coverage of regular expressions can be found in "
	"@@Friedl (1997)@.")

MAN_END

MAN_BEGIN (L"Regular expressions 1. Special characters", L"djmw", 20010718)
INTRO (L"The following characters are the %meta characters that give special "
	"meaning to the regular expression search syntax:")
TAG (L"#\\bs#   the backslash %escape character.")
DEFINITION (L"The backslash gives special meaning to the character "
	"following it. For example, the combination \"\\bsn\" stands for the "
	"%newline, one of the @@Regular expressions 5. Special control characters"
	"|control characters@. The combination \"\\bsw\" stands for a \"word\" "
	"character, one of the @@Regular expressions 6. Convenience escape "
	"sequences|"
	"convenience escape sequences@ while \"\\bs1\" is one of the @@Regular "
	"expressions 8. Substitution special characters|substitution special "
	"characters@.")
LIST_ITEM1 (L"Example: The regex \"aa\\bsn\" tries to match two consecutive "
	"\"a\"s at the end of a line, inclusive the newline character itself.")
LIST_ITEM1 (L"Example: \"a\\bs+\" matches \"a+\" and not a series of one or "
	"\"a\"s.")
TAG (L"##\\^ #    the caret is the start of line @@Regular expressions 3. "
	"Anchors|anchor@ or the negate symbol.")
LIST_ITEM1 (L"Example: \"\\^ a\" matches \"a\" at the start of a line.")
LIST_ITEM1 (L"Example: \"[\\^ 0-9]\" matches any non digit.")
TAG (L"##\\$ #    the dollar is the end of line @@Regular expressions 3. "
	"Anchors|anchor@.")
LIST_ITEM1 (L"Example: \"b\\$ \" matches a \"b\" at the end of a line.")
LIST_ITEM1 (L"Example: \"\\^ b\\$ \" matches the empty line.")
TAG (L"##{ }#    the open and close curly bracket are used as range @@Regular "
	"expressions 2. Quantifiers|quantifiers@.")
LIST_ITEM1 (L"Example: \"a{2,3}\" matches \"aa\" or \"aaa\".")
TAG (L"##[ ]#    the open and close square bracket define a character class to "
	"match a %single character.")
DEFINITION (L"The \"\\^ \" as the first character following the \"[\" negates "
	"and the match is for the characters %not listed. "
	"The \"-\" denotes a range of characters. Inside a \"[  ]\" character "
	"class construction most special characters are interpreted as ordinary "
	"characters. ")
LIST_ITEM1 (L"Example: \"[d-f]\" is the same as \"[def]\" and matches \"d\", "
	"\"e\" or \"f\".")
LIST_ITEM1 (L"Example: \"[a-z]\" matches any lowercase characters in the "
	"alfabet.")
LIST_ITEM1 (L"Example: \"[\\^ 0-9]\" matches any character that is not a digit.")
LIST_ITEM1 (L"Example: A search for \"[][()?<>$^.*?^]\" in the string "
	"\"[]()?<>$^.*?^\" followed by a replace string \"r\" has the result "
	"\"rrrrrrrrrrrrr\". Here the search string is %one character class and "
	"all the meta characters are interpreted as ordinary characters without "
	"the need to escape them.")
TAG (L"##( )#    the open and close parenthesis are used for grouping "
	"characters (or other regex).")
DEFINITION (L"The groups can be referenced in "
	"both the search and the @@Regular expressions 8. Substitution special "
	"characters|substitution@ phase. There also exist some @@Regular "
	"expressions 4. Special constructs with parenthesis|special constructs "
	"with parenthesis@.")
LIST_ITEM1 (L"Example: \"(ab)\\bs1\" matches \"abab\".")
TAG (L"##.#    the dot matches any character except the newline.")
LIST_ITEM1 (L"Example: \".a\" matches two consecutive characters where "
	"the last one is \"a\".")
LIST_ITEM1 (L"Example: \".*\\bs.txt\\$ \" matches all strings that end in "
	"\".txt\".")
TAG (L"##*#    the star is the match-zero-or-more @@Regular expressions 2. "
	"Quantifiers|quantifier@.")
LIST_ITEM1 (L"Example: \"\\^ .*\\$ \" matches an entire line. ")
TAG (L"##+#    the plus is the match-one-or-more quantifier.")
TAG (L"##?#    the question mark is the match-zero-or-one "
	"quantifier. The question mark is also used in  "
	"@@Regular expressions 4. Special constructs with parenthesis|special "
	"constructs with parenthesis@ and in @@Regular expressions 2. "
	"Quantifiers|changing match behaviour@.")
TAG (L"##\\| #    the vertical pipe separates a series of alternatives.")
LIST_ITEM1 (L"Example: \"(a|b|c)a\" matches \"aa\" or \"ba\" or \"ca\".")
TAG (L"##< >#    the smaller and greater signs are @@Regular expressions 3. "
	"Anchors|anchors@ that specify a left or right word boundary.")
TAG (L"##-#    the minus indicates a range in a character class (when it is "
	"not at the first position after the \"[\" opening bracket or the last "
	"position before the \"]\" closing bracket.")
LIST_ITEM1 (L"Example: \"[A-Z]\" matches any uppercase character.")
LIST_ITEM1 (L"Example: \"[A-Z-]\" or \"[-A-Z]\" match any uppercase character "
	"or \"-\".")
TAG (L"##&#    the and is the \"substitute complete match\" symbol.")
MAN_END

MAN_BEGIN (L"Regular expressions 2. Quantifiers", L"djmw", 20010708)
INTRO (L"Quantifiers specify how often the preceding @@Regular expressions|"
	"regular expression@ should match.")
TAG (L"##*#   Try to match the preceding regular expression zero or more times.")
LIST_ITEM1 (L"Example: \"(ab)c*\" matches \"ab\" followed by zero or more "
	"\"c\"s, i.e., \"ab\", \"abc\", \"abcc\", \"abccc\" ...")
TAG (L"##+#   Try to match the preceding regular expression one or more times.")
LIST_ITEM1 (L"Example: \"(ab)c+\" matches \"ab\" followed by one or more "
	"\"c\"s, i.e., \"abc\", \"abcc\", \"abccc\" ...")
TAG (L"##{%m, %n}#   Try to match the preceding regular expression between %m "
	"and %n times.")
DEFINITION (L"If you leave %m out, it is assumed to be zero. If you leave "
	"%n out it is assumed to be infinity. I.e., \"{,%n}\" matches from %zero "
	"to %n times, \"{%m,}\" matches a minimum of %m times, \"{,}\" matches "
	"the same as \"*\" and \"{n}\" is shorthand for \"{n, n\"} and matches "
	"exactly %n times.")
LIST_ITEM1 (L"Example: \"(ab){1,2}\" matches \"ab\" and \"abab\".")
TAG (L"##?#   Try to match zero or one time.")
ENTRY (L"Changing match behaviour")
NORMAL (L"Default the quantifiers above try to match as much as possible, they "
	"are %greedy. "
	"You can change greedy behaviour to %lazy behaviour by adding an "
	"extra \"?\" after the quantifier.")
LIST_ITEM1 (L"Example: In the string \"cabddde\", the search \"abd{1,2}\" "
	"matches \"abdd\", while the search for \"abd{1,2}?\" matches \"abd\".")
LIST_ITEM1 (L"Example: In the string \"cabddde\", the search \"abd+\" "
	"matches \"abddd\", while the search for \"abd+?\" matches \"abd\".")
MAN_END

MAN_BEGIN (L"Regular expressions 3. Anchors", L"djmw", 20010708)
INTRO (L"Anchors let you specify a very specific position within the search "
	"text.")
TAG (L"##\\^ #   Try to match the (following) regex at the beginning of a line.")
LIST_ITEM1 (L"Example: \"\\^ ab\" matches \"ab\" only at the beginning of a "
	"line and not, for example, in the line \"cab\".")
TAG (L"##\\$ #   Try to match the (following) regex at the end of a line.")
TAG (L"##<#    Try to match the regex at the %start of a word.")
DEFINITION (L"The character class that defines a %word can be found at the "
	"@@Regular expressions 6. Convenience escape sequences|convenience escape "
	"sequences@ page.")
TAG (L"##>#    Try to match the regex at the %end of a word.")
TAG (L"##\\bsB#   Not a word boundary")
DEFINITION (L"")
MAN_END

MAN_BEGIN (L"Regular expressions 4. Special constructs with parenthesis", L"djmw",
	20010710)
INTRO (L"Some special constructs exist with parenthesis. ")
TAG (L"##(?:#%regex#)#   is a grouping-only construct.")
DEFINITION (L"They exist merely for efficiency reasons and facilitate grouping.")
TAG (L"##(?=#%regex#)#   is a positive look-ahead.")
DEFINITION (L"A match of the regular expression contained in the positive "
	"look-ahead construct is attempted. If the match succeeds, control is "
	"passed to the regex following this construct and the text consumed by "
	"this look-ahead construct is first unmatched. ")
TAG (L"##(?!#%regex#)#   is a negative look-ahead.")
DEFINITION (L"Functions like a positive look-ahead, only the "
	"%regex must %not match.")
LIST_ITEM (L"Example: \"abc(?!.*abc.*)\" searches for the %last "
	"occurrence of \"abc\" in a string.")
TAG (L"##(?i#%regex#)#   is a case insensitive regex.")
TAG (L"##(?I#%regex#)#   is a case sensitive regex.")
DEFINITION (L"Default a regex is case sensitive. ")
LIST_ITEM1 (L"Example: \"(?iaa)\" matches \"aa\", \"aA\", \"Aa\" and \"AA\".")
TAG (L"##(?n#%regex#)#   matches newlines.")
TAG (L"##(?N#%regex#)#   doesn't match newlines.")
NORMAL (L"All the constructs above do not capture text and cannot be "
	"referenced, i.e., the parenthesis are not counted. However, you "
	"can make them capture text by surrounding them with %ordinary "
	"parenthesis.")
MAN_END

MAN_BEGIN (L"Regular expressions 5. Special control characters", L"djmw", 20010708)
INTRO (L"Special control characters in a @@Regular expressions|regular "
	"expression@ specify characters that are difficult to type.")
TAG (L"#\\bsa  alert (bell).")
TAG (L"#\\bsb  backspace.")
TAG (L"#\\bse  ASCII escape character.")
TAG (L"#\\bsf  form feed (new page).")
TAG (L"#\\bsn  newline.")
TAG (L"#\\bsr  carriage return.")
LIST_ITEM1 (L"Example : a search for \"\\bsr\\bsn\" followed by a replace "
	"\"\\bsr\" changes Windows text files to Macintosh text files.")
LIST_ITEM1 (L"Example : a search for \"\\bsr\" followed by a replace "
	"\"\\bsn\" changes Macintosh text files to Unix text files.")
LIST_ITEM1 (L"Example : a search for \"\\bsr\\bsn\" followed by a replace "
	"\"\\bsn\" changes Windows text files to Unix text files.")
TAG (L"#\\bst  horizontal tab.")
TAG (L"#\\bsv  vertical tab.")
MAN_END

MAN_BEGIN (L"Regular expressions 6. Convenience escape sequences", L"djmw", 20010708)
INTRO (L"Convenience escape sequences in a @@Regular expressions|regular "
	"expression@ present a shorthand for some character classes.")
TAG (L"#\\bsd  matches a digit: [0-9].")
LIST_ITEM1 (L"Example: \"-?\\bsd+\" matches any integer.")
TAG (L"#\\bsD  %not a digit: [\\^ 0-9].")
TAG (L"#\\bsl  a letter: [a-zA-Z].")
TAG (L"#\\bsL  %not a letter: [\\^ a-zA-Z].")
TAG (L"#\\bss  whitespace: [ \\bst\\bsn\\bsr\\bsf\\bsv].")
TAG (L"#\\bsS  %not whitespace: [\\^  \\bst\\bsn\\bsr\\bsf\\bsv].")
TAG (L"#\\bsw  \"word\" character: [a-zA-Z0-9\\_ ].")
LIST_ITEM1 (L"Example: \"\\bsw+\" matches a \"word\", i.e., a string of one "
	"or more characters that may consist of letters, digits and underscores.")
TAG (L"#\\bsW  %not a \"word\" character: [\\^ a-zA-Z0-9\\_ ].")
TAG (L"#\\bsB  any character that is %not a word-delimiter.")
MAN_END

MAN_BEGIN (L"Regular expressions 7. Octal and hexadecimal escapes", L"djmw", 20010709)
INTRO (L"An octal number can be represented by the octal escape \"\\bs0\" "
	"and maximally three digits from the digit class [0-7]. "
	"The octal number should not exceed \\bs0377. ")
NORMAL (L"A hexadecimal number can be represented by the octal escape "
	"\"\\bsx\" or \"\\bsX\"and maximally two characters from the class "
	"[0-9A-F]. The maximum hexadecimal number should not exceed \\bsxFF. ")
LIST_ITEM1 (L"Example: \\bs053 and \\bsX2B both specify the \"+\" character.")
MAN_END

MAN_BEGIN (L"Regular expressions 8. Substitution special characters", L"djmw", 20010708)
INTRO (L"The substitution string is mostly interpreted as ordinary text except "
	"for the @@Regular expressions 5. Special control characters|"
	"special control characters@, the @@Regular expressions 7. Octal and "
	"hexadecimal escapes|octal and hexadecimal escapes@ and the following "
	"character combinations:")
TAG (L"#\\bs1 ... #\\bs9    are backreferences at sub-expressions 1 ... 9 in the match.")
DEFINITION (L"Any of the first nine sub-expressions of the match string can "
	"be inserted into the replacement string by inserting a `\\bs' followed "
	"by a digit from 1 to 9 that represents the string matched by a "
	"parenthesized expression within the regular expression. The numbering "
	"is left to right.")
LIST_ITEM1 (L"Example: A search for \"(a)(b)\" in the string \"abc\", "
	"followed by a replace \"\\bs2\\bs1\" results in \"bac\".")
TAG (L"#&    reference at entire match.")
DEFINITION (L"The entire string that was matched by the search operation will "
	"be substituted.")
LIST_ITEM1 (L"Example: a search for \".\" in the string \"abcd\" followed by "
	"the replace \"&&\" doubles every character in the result "
	"\"aabbccdd\".")
TAG (L"#\\bsU #\\bsu    to uppercase.")
DEFINITION (L"The text inserted by \"&\" or \"\\bs1\" ... \"\\bs9\" is "
	"converted to %uppercase (\"\\bsu\" only changes the %first character to "
	"uppercase).")
LIST_ITEM1 (L"Example: A search for \"(aa)\" in the string \"aabb\", "
	"followed by a replace \"\\bsU\\bs1bc\" results in the string \"AAbcbb\".")
TAG (L"#\\bsL #\\bsl    to lowercase.")
DEFINITION (L"The text inserted by \"&\" or \"\\bs1\" ... \"\\bs9\" is "
	"converted to %lowercase (\"\\bsl\" only changes the %first character to "
	"lowercase).")
LIST_ITEM1 (L"Example: A search for \"(AA)\" with a replace \"\\bsl\\bs1bc\" "
	"in the string \"AAbb\" results in the string \"aAbcbb\".")
MAN_END

MAN_BEGIN (L"Roots", L"djmw", 19990608)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Roots "
	"represents the (complex) roots of a @@Polynomial|polynomial@ function.")
ENTRY (L"Commands")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu @@Polynomial: To Roots@")
NORMAL (L"Drawing")
LIST_ITEM (L"\\bu ##Draw...# (in the complex plane)")
NORMAL (L"Queries")
LIST_ITEM (L"\\bu ##Get root...#: get complex root")
LIST_ITEM (L"\\bu ##Get real part of root...#")
LIST_ITEM (L"\\bu ##Get imaginary part of root...#")
MAN_END

MAN_BEGIN (L"Scree plot", L"djmw", 20040331)
NORMAL (L"A scree plot shows the sorted eigenvalues, from large to "
	"small, as a function of the eigenvalue index.")
MAN_END

MAN_BEGIN (L"singular value decomposition", L"djmw", 20120510)
INTRO (L"The %%singular value decomposition% (SVD) is a matrix factorization algorithm.")
NORMAL (L"For %m > %n, the singular value decomposition of a real %m \\xx %n matrix #A is the "
	"factorization")
FORMULA (L"#A = #U #\\Si #V\\'p,")
NORMAL (L"The matrices in this factorization have the following properties:")
TAG (L"#U [%m \\xx %n] and #V [%n \\xx %n]")
DEFINITION (L"are orthogonal matrices. The columns #u__%i_ of #U =[#u__1_, ..., #u__%n_] "
	"are the %%left singular vectors%, and the columns #v__%i_ of #V [#v__1_, ..., #v__%n_] "
	"are the %%right singular vectors%.")
TAG (L"#\\Si [%n \\xx %n] = diag (%\\si__1_, ..., %\\si__%n_)")
DEFINITION (L"is a real, nonnegative, and diagonal matrix. Its diagonal contains the so called "
	"%%singular values% %\\si__%i_, where %\\si__1_ \\>_ ... \\>_ %\\si__%n_ \\>_ 0.")
NORMAL (L"If %m < %n, the decomposition results in #U [%m \\xx %m] and #V [%n \\xx %m].")
MAN_END

MAN_BEGIN (L"Sound & Pitch: Change speaker...", L"djmw", 20070722)
INTRO (L"A command to create a new Sound object with manipulated characteristics "
	"from the selected @Sound and @Pitch.")
NORMAL (L"With this command you can have finer grained control over the "
	"pitch than with the @@Sound: Change speaker...@ command. "
	"Accurate pitch measurement determines the quality of the "
	"@@overlap-add@ synthesis." )
ENTRY (L"Settings")
NORMAL (L"The settings are described in @@Sound: Change speaker...@. ")
MAN_END

MAN_BEGIN (L"Sound & Pitch: Change gender...", L"djmw", 20070722)
/* INTRO (L"Deprecated: use @@Sound & Pitch: Change speaker...@") */
NORMAL (L"A command to create a new Sound object with manipulated characteristics "
	"from the selected @Sound and @Pitch.")
NORMAL (L"With this command you can have finer grained control over the "
	"pitch than with the @@Sound: Change gender...@ command. "
	"Accurate pitch measurement determines the quality of the "
	"@@overlap-add@ synthesis." )
ENTRY (L"Settings")
NORMAL (L"The settings are described in @@Sound: Change gender...@. ")
MAN_END

MAN_BEGIN (L"Sound: Change gender...", L"djmw", 20030205)
/* INTRO (L"Deprecated: use @@Sound: Change speaker...@") */
NORMAL (L"A command to create a new @Sound with manipulated characteristics.")
ENTRY (L"Settings")
NORMAL (L"The quality of the @@overlap-add|manipulation@ depends on the pitch measurement.")
NORMAL (L"The arguments that control the pitch measurement are:")
TAG (L"##Minimum pitch (Hz)# (standard value: 75 Hz)")
DEFINITION (L"pitch candidates below this frequency will not be considered.")
TAG (L"##%Maximum pitch (Hz)# (standard value: 600 Hz)")
DEFINITION (L"pitch candidates above this frequency will be ignored.")
NORMAL (L"The arguments that control the manipulation are:")
TAG (L"##Formant shift ratio")
DEFINITION (L"determines the frequencies of the formants in the newly created "
	"Sound. If this ratio equals 1 no frequency shift will occur and "
	"the formant frequencies will not change. A ratio of 1.1 will change "
	"a male voice to a voice with approximate female formant characteristics. "
	"A ratio of 1/1.1 will change a female voice to a voice with approximate male formant "
	"characteristics.")
TAG (L"##New pitch median (Hz)# (standard value: 0.0 Hz, i.e. same as original)")
DEFINITION (L"determines what the median pitch of the new Sound will be. "
	"The pitch values in the newly created Sound will be calculated from the pitch "
	"values in the selected Sound by multiplying them by a factor "
	"%%newPitchMedian / oldPitchMedian%. This factor equals 1.0 if the default "
	"value for the new pitch median (0.0) is chosen. ")
TAG (L"##Pitch range factor# (standard value: 1.0)")
DEFINITION (L"determines an %extra% scaling of the new pitch values around the %new% "
	"pitch median. A factor of 1.0 means that no additional pitch modification will occur "
	"(except the obvious one described above). A factor of 0.0 monotonizes the new "
	"sound to the new pitch median.")
TAG (L"##Duration factor# (standard value: 1.0)")
DEFINITION (L"The factor with which the sound will be lengthened. The default is 1.0. "
	"If you take a value less than 1.0, the resulting sound will be shorter than "
	"the original. A value larger than 3.0 will not work.")
NORMAL (L"If you want more control over the synthesis you can supply your own "
	"Pitch object and use the @@Sound & Pitch: Change gender...@ command. ")
ENTRY (L"Algorithm")
NORMAL (L"The shifting of frequencies is done via manipulation of the sampling frequency. "
	"Pitch and duration changes are generated with @@overlap-add@ synthesis.")
NORMAL (L"The new pitch values are calculated in a two step process. We first multiply all "
	"the pitches with the factor %%newPitchMedian / oldPitchMedian% according to:")
FORMULA (L"%newPitch = %pitch * %newPitchMedian  / %oldPitchMedian.")
NORMAL (L"It follows that if the %newPitchMedian equals the %oldPitchMedian no "
	"change in pitch values will occur in the first step.")
NORMAL (L"Subsequently, the pitch range scale factor determines the final pitch values "
	"in the following linear manner:")
FORMULA (L"%finalPitch = %newPitchMedian + (%newPitch \\-- %newPitchMedian) * %pitchRangeScaleFactor")
NORMAL (L"Hence, it follows that no further scaling occurs if %pitchRangeScaleFactor "
	"equals 1.0.")
MAN_END

MAN_BEGIN (L"Sound: Change speaker...", L"djmw", 20080515)
INTRO (L"A command to create a new @Sound with manipulated characteristics.")
ENTRY (L"Settings")
NORMAL (L"The quality of the @@overlap-add|manipulation@ depends on the pitch measurement.")
NORMAL (L"The arguments that control the pitch measurement are:")
TAG (L"##Pitch floor (Hz)# (standard value: 75 Hz)")
DEFINITION (L"pitch candidates below this frequency will not be considered.")
TAG (L"##Pitch ceiling (Hz)# (standard value: 600 Hz)")
DEFINITION (L"pitch candidates above this frequency will be ignored.")
NORMAL (L"The arguments that control the manipulation are:")
TAG (L"##Multiply formants by")
DEFINITION (L"determines the formant frequencies of the newly created sound. "
	"The formant frequency of the new sound will equal the formant frequencies of the selected sound multiplied by this number. "
	"If this number equals 1, formant frequencies will not change. A number of 1.1 will change "
	"a male voice to a voice with approximate female formant characteristics. "
	"A ratio of 1/1.1 will change a female voice to a voice with approximate male formant "
	"characteristics.")
TAG (L"##Multiply pitch by")
DEFINITION (L"determines what the pitch of the new Sound will be. "
	"The pitch values of the new sound will equal the pitch values of the selected sound multiplied by this number. A value of 1.8 will  approximately change a male's pitch to a female's pitch.")
TAG (L"##Multiply pitch range by# (standard value: 1.0)")
DEFINITION (L"determines the pitch range of the newly created sound. "
	"A factor of 1.0 means that no additional pitch modification will occur "
	"(except the obvious one described above). A factor of 0.0 monotonizes the new "
	"sound to the new pitch median. A negative number inverses the pitch range with respect to the median.")
TAG (L"##Multiply duration by# (standard value: 1.0)")
DEFINITION (L"determines how to modify the duration of the newly created sound. "
	"A value of 1.0 means that the new sound will have the same duration as the selected sound. "
	"A value less than 1.0 will result in a shortened new sound. A value larger than 2.5 will not work.")
NORMAL (L"If you want more control over the synthesis you can supply your own "
	"Pitch object and use the @@Sound & Pitch: Change speaker...@ command. ")
ENTRY (L"Algorithm")
NORMAL (L"The shifting of formant frequencies is done via manipulation of the sampling frequency. "
	"To multiply all formants by a factor of 1.10 (i.e. raising them by 10 percent), a sampling "
	"frequency of 44100 Hz is first raised to 48510 Hz (without changing "
	"the samples). After this, the sound is lengthened by a factor of 1.10 "
	"and the pitch is lowered by a factor of 1.10, so that the original "
	"duration and pitch are restored. After this, the sound is resampled "
	"to 44100 Hz (by sinc interpolation)."
	"Pitch and duration changes are generated with @@overlap-add@ synthesis.")
MAN_END

MAN_BEGIN (L"Sound: Remove noise...", L"djmw", 20121122)
INTRO (L"A command to suppress noise in the selected @Sound.")
ENTRY (L"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (5), L""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Remove noise...", 5)
	Manual_DRAW_SETTINGS_WINDOW_RANGE(L"Noise time range (s)", L"0.0", L"0.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD(L"Window length (s)", L"0.025")
	Manual_DRAW_SETTINGS_WINDOW_RANGE(L"Filter frequency range (Hz)", L"80.0", L"10000.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD(L"Smoothing (Hz)", L"40.0")
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU(L"Noise reduction method", L"Spectral subtraction")
)
TAG (L"##Noise time range (s)")
DEFINITION (L"the start and end time of a noise part in the sound whose characteristics will be used in the denoising. "
	"If the end time is chosen before the start time, the noise fragment will be chosen automatically around a position "
	"where the intensity is minimal. For good noise suppression it is important that the noise fragment's duration is chosen "
	"several times the length of the window.")
TAG (L"##Window length (s)")
DEFINITION (L"denoising takes place in (overlapping) windows of this length.")
TAG (L"##Filter frequency range (Hz)")
DEFINITION (L"before denoising the sound will be @@Sound: Filter (pass Hann band)...|band-pass filtered@. ")
TAG (L"##Noise reduction method")
DEFINITION (L"The method of %%spectral subtraction% was defined in @@Boll (1979)@. The variant implemented is modeled "
	"after a script by Ton Wempe.")
MAN_END

MAN_BEGIN (L"Sound: Draw where...", L"djmw", 20130409)
INTRO (L"A command to draw only those parts of a @Sound where a condition holds.")
ENTRY (L"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (5), L""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Draw where...", 5)
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range (s)", "0.0", "0.0 (=all)")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (=all)")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
	Manual_DRAW_SETTINGS_WINDOW_OPTIONMENU("Drawing method", "Curve")
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("Draw only those parts where the following condition holds",
		"x < xmin + (xmax - xmin) / 2; first half")
)
TAG (L"##Time range (s)")
DEFINITION (L"selects the time domain for the drawing.")
TAG (L"##Vertical range")
DEFINITION (L"defines the vertical limits; larger amplitudes will be clipped.")
TAG (L"##Draw only those parts where the following condition holds#")
DEFINITION (L"determines the part of the sound that will be drawn. All parts where the formula evaluates to true will be drawn. "
	"This formula may ##not# contain references to the sampling of the sound, i.e. don't use 'col', 'x1', 'dx' and 'ncol' in it.")
ENTRY (L"Example 1")
NORMAL (L"The following script draws all amplitudes larger than one in red.")
CODE (L"do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 2000, \"1.8*sin(2*pi*5*x)+randomGauss(0,0.1)\")")
CODE (L"do (\"Colour...\", \"Red\")")
CODE (L"do (\"Draw where...\", 0, 0, -2, 2, \"no\", \"Curve\", \"abs(self)>1\")")
CODE (L"do (\"Colour...\", \"Black\")")
CODE (L"do (\"Draw where...\", 0, 0, -2, 2, \"yes\", \"Curve\", \"not (abs(self)>1)\")")
SCRIPT (8, 3,
	L"do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 2000, \"1.8*sin(2*pi*5*x)+randomGauss(0,0.1)\")\n"
	"do (\"Colour...\", \"Red\")\n"
	"do (\"Draw where...\", 0, 0, -2, 2, \"no\", \"Curve\", \"abs(self)>1\")\n"
	"do (\"Colour...\", \"Black\")\n"
	"do (\"Draw where...\",  0, 0, -2, 2, \"yes\", \"Curve\", \"not (abs(self)>1)\")\n"
	"do (\"Remove\")\n"
)
ENTRY (L"Example 2")
NORMAL (L"Draw the second half of a sound:")
CODE (L"do (\"Draw where...\", 0, 0, -1, 1, \"no\", \"Curve\", \"x > xmin + (xmax - xmin) / 2\")")
ENTRY (L"Example 3")
NORMAL (L"Draw only positive amplitudes:")
CODE (L"do (\"Draw where...\", 0, 0, -1, 1, \"no\", \"Curve\", \"self>0\")")
ENTRY (L"Example 4")
NORMAL (L"Draw parts where pitch is larger than 300 Hz in red:")
CODE (L"s = selected (\"Sound\")")
CODE (L"p = do (\"To Pitch...\", 0, 75, 600)")
CODE (L"pt = do (\"Down to PitchTier\")")
CODE (L"selectObject (s)")
CODE (L"do (\"Colour...\", \"Red\")")
CODE (L"do (\"Draw where...\", 0, 0, -1, 1, \"yes\", \"Curve\", \"Object_'pt'(x) > 300\")")
CODE (L"do (\"Colour...\", \"Black\")")
CODE (L"do (\"Draw where...\", 0, 0, -1, 1, \"yes\", \"Curve\", \"not (Object_'pt'(x) > 300)\")")
MAN_END

MAN_BEGIN (L"Sound: Fade in...", L"djmw", 20130410)
INTRO (L"A command to gradually increase the amplitude of a selected @Sound.")
ENTRY (L"Settings")
TAG (L"##Channel")
DEFINITION (L"determines whether you want to fade all channels or only a selected channel.")
TAG (L"##Time (s)")
DEFINITION (L"determines where the fade-in will take place. If %time is earlier than the start time of the sound, the start time of the sound wil be used.")
TAG (L"##Fade time (s)")
DEFINITION (L"determines the start point and the endpoint of the fade-in with respect to the %time argument. Depending on the sign of %%fadeTime%, %time is either the start or the end position of the fade-in. If %%fadeTime% is positive, fade-in will take place between %%time% and %%time+fadeTime%. If %%fadeTime% is negative, fade-in wil take place between %%time+fadeTime% and %time.")
TAG (L"##Silent from start")
DEFINITION (L"when on, makes the sound silent before the fade-in starts. "
	"When off, the sound before the fade-in starts will not be changed. ")
ENTRY (L"Algorithm")
NORMAL (L"Multiplication with the first half period of a (1-cos(x))/2 function. ")
ENTRY (L"Cross-fading two sounds")
NORMAL (L"The following script cross-fades two sounds s1 and s2 at time 1 second and leaves the result in s2.")
CODE1 (L"crossFTime = 0.5")
CODE1 (L"t = 1")
CODE1 (L"do (\"Create Sound from formula...\", \"s1\", \"Mono\", 0, 2, 44100, \"sin(2*pi*500*x)\")")
CODE1 (L"do (\"Fade out...\", 0, t-crossFTime/2, crossFTime, \"yes\")")
CODE1 (L"do (\"Create Sound from formula...\", \"s2\", \"Mono\", 0, 2, 44100, \"sin(2*pi*1000*x)\")")
CODE1 (L"do (\"Fade in...\", 0, t-crossFTime/2, crossFTime, \"yes\")")
CODE1 (L"do (\"Formula...\", \"self+Sound_s1[]\")")
MAN_END

MAN_BEGIN (L"Sound: Fade out...", L"djmw", 20121010)
INTRO (L"A command to gradually decrease the amplitude of a selected @Sound.")
ENTRY (L"Settings")
TAG (L"##Channel")
DEFINITION (L"determines whether you want to fade all channels or only a selected channel.")
TAG (L"##Time (s)")
DEFINITION (L"determines where the fade-out will take place. If %time is later than the end time of the sound, the end time of the sound wil be used.")
TAG (L"##Fade time (s)")
DEFINITION (L"determines the start point and the endpoint of the fade-out with respect to the %time argument. Depending on the sign of %%fadeTime%, %time is either the start or the end position of the fade-out. If %%fadeTime% is positive, fade-out will take place between %%time% and %%time+fadeTime%. If %%fadeTime% is negative, fade-out wil take place between %%time+fadeTime% and %time.")
TAG (L"##Silent to end")
DEFINITION (L"Make the sound silent after the fade-out finishes. ")
ENTRY (L"Algorithm")
NORMAL (L"Multiplication with the first half period of a (1+cos(%%x%))/2 function.")
MAN_END

MAN_BEGIN (L"Sound: Filter (gammatone)...", L"djmw", 19980712)
INTRO (L"A command to filter a Sound by a fourth order gammatone bandpass filter.")
ENTRY (L"Settings")
TAG (L"##Centre frequency (Hz)#, ##Bandwidth (Hz)#")
DEFINITION (L"determine the passband of the filter.")
ENTRY (L"Algorithm")
NORMAL (L"The impulse response of the filter is a 4-th order @@gammatone@. This "
	"filter is implemented as a simple 8-th order recursive digital filter with "
	"4 zeros and 8 poles (these 8 poles consist of one conjugate pole pair to the "
	"4-th power). In the Z-domain its formula is: ")
FORMULA (L"%#H (%z) = (1 + \\su__%i=1..4_ %a__%i_%z^^%\\--i^) / "
	"(1 + \\su__%j=1..8_ %b__%j_%z^^%\\--j^)")
NORMAL (L"The derivation of the filter coefficients %a__%i_ and %b__%j_ is "
	"according to @@Slaney (1993)@. "
	"The gain of the filter is scaled to unity at the centre frequency.")
MAN_END

MAN_BEGIN (L"Sound: Play as frequency shifted...", L"djmw", 20121025)
INTRO (L"Plays the selected @Sound with all frequencies shifted by the same amount. This command can be used to make "
	"audible those sounds that are normally not audible by human beings, like ultrasound or infrasound.")
ENTRY (L"Settings")
TAG (L"##Shift by (Hz)")
DEFINITION (L"the amount by which frequencies are shifted. A positive number shifts frequencies up, a negative number "
	"shifts frequencies down. ")
ENTRY (L"##Example")
NORMAL (L"Rodents produce sounds with frequencies far outside the human audible range. Some meaningfull sqeeks "
	"are present in the frequency range from 54 kHz up to sometimes 100kHz. By choosing a shift value of -54000 all " "frequencies will be shifted down by 54000 Hz. This means that now the rodents frequencies from 54000 Hz and up fall "
	"in your audible domain. If the highest frequency you can hear is, say, 14000 Hz than you can now hear rodent's "
	"frequencies between 54000 and 68000 Hz (=54000+14000).")
MAN_END

MAN_BEGIN (L"Sound: To BarkFilter...", L"djmw", 20010404)
INTRO (L"A command that creates a @BarkFilter object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of filters.")
NORMAL (L"The filter functions used are:")
FORMULA (L"10 log %#H(%z) = 7 - 7.5 * (%z__%c_ - %z - 0.215) - 17.5 * \\Vr "
	"(0.196 + (%z__%c_ - %z - 0.215)^2)")
NORMAL (L"where %z__%c_ is the central (resonance) frequency of the filter in Bark. "
	"The bandwidths of these filters are constant and equal 1 Bark.")
MAN_END

MAN_BEGIN (L"Sound: To FormantFilter...", L"djmw", 20010404)
INTRO (L"A command that creates a @FormantFilter object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of filters whose bandwidths depend on the pitch of the signal.")
NORMAL (L"The analysis proceeds in two steps:")
LIST_ITEM (L"1. We perform a pitch analysis (see @@Sound: To Pitch...@ for "
	"details).")
LIST_ITEM (L"2. We perform a filter bank analysis on a linear frequency scale. "
	"The bandwidth of the filters depends on the measured pitch (see @@Sound & "
	"Pitch: To FormantFilter...@ for details).")
MAN_END

MAN_BEGIN (L"Sound: Paint where...", L"djmw", 20130502)
INTRO (L"A command to paint only those parts of a @Sound where a condition holds. The painted area is the area "
	"between the Sound and a horizontal line at a certain level.")
ENTRY (L"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (6), L""
	Manual_DRAW_SETTINGS_WINDOW ("Sound: Paint where...", 6)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Colour (0-1, name, {r,g,b})", "0.5")
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range (s)", "0.0", "0.0 (=all)")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (=all)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Fill from level", "0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
	Manual_DRAW_SETTINGS_WINDOW_TEXT ("Paint only those parts where the following condition holds",
		"1; always")
)
TAG (L"##Colour")
DEFINITION (L"defines the @@Colour|colour@ of the paint.")
TAG (L"##Time range (s)")
DEFINITION (L"selects the time domain for the drawing.")
TAG (L"##Vertical range")
DEFINITION (L"defines the vertical limits; larger amplitudes will be clipped.")
TAG (L"##Fill from level")
DEFINITION (L"defines the level of the horizontal line. ")
TAG (L"##Formula")
DEFINITION (L"determines the part of the sound that will be painted. All parts where the formula evaluates to true will be painted. "
"This formula may ##not# contain references to the sampling of the sound, i.e. don't use 'col', 'x1', 'dx' and 'ncol' in it.")
ENTRY (L"Example 1")
NORMAL (L"The following script paints the area under a sine curve in red and the area above in green."
	"For the first paint the horizontal line is at y=-1, for the second paint the line is at y=+1. "
	"The formula always evaluates to true.")
CODE (L"s = do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 10000, \"0.5*sin(2*pi*5*x)\")")
CODE (L"do (\"Paint where...\", \"Red\", 0, 0, -1, 1, -1, \"yes\", \"1\")")
CODE (L"do (\"Paint where...\", \"Green\", 0, 0, -1, 1, 1, \"no\", \"1\")")
SCRIPT (8, 5,
	L"s = do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 10000, \"0.5*sin(2*pi*5*x)\")\n"
	"do (\"Paint where...\", \"Red\", 0, 0, -1, 1, -1, \"no\", \"1\")\n"
	"do (\"Paint where...\", \"Green\", 0, 0, -1, 1, 1, \"yes\", \"1\")\n"
	"do (\"Remove\")\n")
ENTRY (L"Example 2")
NORMAL (L"The following script paints the area below zero in red and the area above in green."
	"The horizontal line is now always at y=0 and we use the formula to differentiate the areas.")
CODE (L"s = do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 10000, \"0.5*sin(2*pi*5*x)\")")
CODE (L"do (\"Paint where...\", \"Red\", 0, 0, -1, 1, 0, \"no\", \"self>0\")")
CODE (L"do (\"Paint where...\", \"Green\", 0, 0, -1, 1, 0, \"yes\", \"self<0\")")
SCRIPT (8, 5,
	L"s = do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 10000, \"0.5*sin(2*pi*5*x)\")\n"
	"do (\"Paint where...\", \"Red\", 0, 0, -1, 1, 0, \"no\", \"self<0\")\n"
	"do (\"Paint where...\", \"Green\", 0, 0, -1, 1, 0, \"yes\", \"self>0\")\n"
	"removeObject (s)\n")
ENTRY (L"Example 3")
NORMAL (L"To give an indication that the area under a 1/x curve between the points %a and %b and the area "
	"between %c and %d are equal if %b/%a = %d/%c. For example, for %a=1, %b=2, %c=4 and %d=8: ")
CODE (L"do (\"Create Sound from formula...\", \"1dx\", \"Mono\", 0, 20, 100, \"1/x\")")
CODE (L"do (\"Draw...\", 0, 20, 0, 1.5, \"yes\", \"Curve\")")
CODE (L"do (\"Paint where...\", \"Grey\", 0, 20, 0, 1.5, 0, \"yes\", \"(x >= 1 and x <2) or (x>=4 and x<8)\")")
CODE (L"do (\"One mark bottom...\", 1, \"yes\", \"yes\", \"no\", \"\")")
CODE (L"do (\"One mark bottom...\", 2, \"yes\", \"yes\", \"no\", \"\")")
CODE (L"do (\"One mark bottom...\", 4, \"yes\", \"yes\", \"no\", \"\")")
CODE (L"do (\"One mark bottom...\", 8, \"yes\", \"yes\", \"no\", \"\")")
SCRIPT (8, 5,
	L"s = do (\"Create Sound from formula...\", \"1dx\", \"Mono\", 0, 20, 100, \"1/x\")\n"
	"do (\"Draw...\", 0, 20, 0, 1.5, \"yes\", \"Curve\")\n"
	"do (\"Paint where...\", \"Grey\", 0, 20, 0, 1.5, 0, \"yes\", \"(x >= 1 and x <2) or (x>=4 and x<8)\")\n"
	"do (\"One mark bottom...\", 1, \"yes\", \"yes\", \"no\", \"\")\n"
	"do (\"One mark bottom...\", 2, \"yes\", \"yes\", \"no\", \"\")\n"
	"do (\"One mark bottom...\", 4, \"yes\", \"yes\", \"no\", \"\")\n"
	"do (\"One mark bottom...\", 8, \"yes\", \"yes\", \"no\", \"\")\n"
	"removeObject (s)\n")
MAN_END

MAN_BEGIN (L"Sounds: Paint enclosed...", L"djmw", 20130502)
INTRO (L"Paints the area between the two selected @@Sound@s. ")
ENTRY (L"Settings")
SCRIPT (5.4, Manual_SETTINGS_WINDOW_HEIGHT (4), L""
	Manual_DRAW_SETTINGS_WINDOW ("Sounds: Paint enclosed", 4)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Colour (0-1, name, {r,g,b})", "0.5")
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Time range (s)", "0.0", "0.0 (=all)")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (=all)")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
)
TAG (L"##Colour")
DEFINITION (L"defines the @@Colour|colour@ of the paint.")
TAG (L"##Time range (s)")
DEFINITION (L"selects the time domain for the drawing.")
TAG (L"##Vertical range")
DEFINITION (L"defines the vertical limits, larger amplitudes will be clipped.")
ENTRY (L"Example")
NORMAL (L"The following script paints the area enclosed between a sine tone of 5 Hz and the straight line %y = %x/2.")
CODE (L"s1 = do (\"Create Sound from formula...\", \"sine\", \"Mono\", 0, 1, 10000, \"1/2 * sin(2*pi*5*x)\")")
CODE (L"s2 = do (\"Create Sound from formula...\", \"line\", \"Mono\", 0, 1, 10000, \"x / 2\")")
CODE (L"plusObject (s1)")
CODE (L"do (\"Paint enclosed...\", \"Grey\", 0, 0, -1, 1, \"yes\")")
SCRIPT ( 4, 2,
	 L"s1 = do (\"Create Sound from formula...\", \"sine\", \"Mono\", 0, 1, 10000, \"1/2 * sin(2*pi*5*x)\")\n"
	"s2 = do (\"Create Sound from formula...\", \"line\", \"Mono\", 0, 1, 10000, \"x / 2\")\n"
	"selectObject (s1, s2)\n"
	"do (\"Paint enclosed...\", \"Grey\", 0, 0, -1, 1, \"yes\")\n"
	"removeObject (s1, s2)\n")
MAN_END

MAN_BEGIN (L"Sound: To Polygon...", L"djmw", 20130502)
INTRO (L"A command that creates a @@Polygon@ from a selected @@Sound@, where the Polygon's "
	" points are defined by the (%time, %amplitude) pairs of the sound. ")
ENTRY (L"Settings")
TAG (L"##Channel")
DEFINITION (L"defines which channel of the sound is used.")
TAG (L"##Time range (s)")
DEFINITION (L"defines the part of the sound whose (%time, %amplitude) pairs have to be included.")
TAG (L"##Vertical range")
DEFINITION (L"defines the vertical limits, larger amplitudes will be clipped.")
TAG (L"##Connection y-value")
DEFINITION (L"defines the y-value of the first and last point of the Polygon. This gives the opportunity to "
	" draw a closed Polygon with the horizontal connection line at any position you like. ")
ENTRY (L"Example")
NORMAL (L"The following script paints the area under a sound curve in red and the area above in green.")
CODE (L"s = do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 10000, \"0.5*sin(2*pi*5*x)\")")
CODE (L"\\# Connection y-value is at amplitude -1: area under the curve.")
CODE (L"p1 = do (\"To Polygon...\", 1, 0, 0, -1, 1, -1)")
CODE (L"do (\"Paint...\", \"{1,0,0}\", 0, 0, -1, 1)")
CODE (L"selectObject (s)")
CODE (L"\\# Connection y-value is now at amplitude 1: area above the curve.")
CODE (L"p2 = do (\"To Polygon...\", 1, 0, 0, -1, 1, 1)")
CODE (L"do (\"Paint...\", \"{0,1,0}\", 0, 0, -1, 1)")
SCRIPT (4.5, 2,
	L"s = do (\"Create Sound from formula...\", \"s\", \"Mono\", 0, 1, 10000, \"0.5*sin(2*pi*5*x)\")\n"
	"p1 = do (\"To Polygon...\", 1, 0, 0, -1, 1, -1)\n"
	"do (\"Paint...\", \"{1,0,0}\", 0, 0, -1, 1)\n"
	"selectObject (s)\n"
	"p2 = do (\"To Polygon...\", 1, 0, 0, -1, 1, 1)\n"
	"do (\"Paint...\", \"{0,1,0}\", 0, 0, -1, 1)\n"
	"removeObject (p2, p1, s)\n"
)
MAN_END

#define xxx_to_TextGrid_detectSilences_COMMON_PARAMETERS_HELP \
TAG (L"##Silence threshold (dB)") \
DEFINITION (L"determines the maximum silence intensity value in dB with respect to the maximum " \
	"intensity. For example, if %imax is the maximum intensity in dB then the maximum silence " \
	"intensity is calculated as %%imax - silenceThreshold%; intervals with an intensity smaller " \
	"than this value are considered as silent intervals.") \
TAG (L"##Minimum silent interval duration (s)") \
DEFINITION (L"determines the minimum duration for an interval to be considered as silent. " \
	"If you don't want the closure for a plosive to count as silent then use a large enough value.") \
TAG (L"##Minimum sounding interval duration (s)") \
DEFINITION (L"determines the minimum duration for an interval to be ##not# considered as silent. " \
	"This offers the possibility to filter out small intense bursts of relatively short duration.") \
TAG (L"##Silent interval label") \
DEFINITION (L"determines the label for a silent interval in the TextGrid.") \
TAG (L"##Sounding interval label") \
DEFINITION (L"determines the label for a sounding interval in the TextGrid.")

MAN_BEGIN (L"Sound: To TextGrid (silences)...", L"djmw", 20061205)
INTRO (L"A command that creates a @TextGrid in which the silent and sounding intervals of the selected @Sound are marked.")
ENTRY (L"Settings")
xxx_to_TextGrid_detectSilences_COMMON_PARAMETERS_HELP
ENTRY (L"Algorithm")
NORMAL (L"First the intensity is determined according to the @@Sound: To Intensity...@ command. "
	"Next the silent and sounding intervas are determined as in the @@Intensity: To TextGrid (silences)...@ command.")
MAN_END

MAN_BEGIN (L"Intensity: To TextGrid (silences)...", L"djmw", 20061201)
INTRO (L"A command that creates a @TextGrid in which the silent and sounding intervals of the selected @Intensity are marked.")
ENTRY (L"Settings")
xxx_to_TextGrid_detectSilences_COMMON_PARAMETERS_HELP
ENTRY (L"Algorithm")
NORMAL (L"First the intensity contour is evaluated and the intervals above and below the silence threshold are marked as "
	"%sounding and %silent. "
	"We then remove sounding intervals with a duration smaller than the %%Minimum sounding interval duration%. "
	"This step is followed by joining the neighbouring silent intervals that resulted because of this removal. "
	"Finally we remove silent intervals with a duration smaller than the %%Minimum silent interval duration%. "
	"This is followed by joining the neighbouring sounding intervals that resulted because of this removal.")
NORMAL (L"Experience showed that first removing short intensity bursts instead of short silences gave better results than doing it the other way around.")
ENTRY (L"Important")
NORMAL (L"The effectiveness of the %%Minimum silent interval duration% and %%Minimum sounding interval duration% "
	"depends on the effective analysis window duration that was used to determine the intensity contour. "
	"For example, if you have chosen 100 Hz for the \"Minimum pitch\" parameter in the @@Sound: To Intensity...@ analysis, "
	"the effective analysis window duration was 32 ms. Don't expect to find sounding "
	"or silent intervals with a duration smaller than this effective analysis window duration.")
MAN_END

MAN_BEGIN (L"Sound: Trim silences...", L"djmw", 20120323)
INTRO (L"A command that creates from the selected @Sound a new sound in which all silence durations are not longer than a specified value.")
ENTRY (L"Settings")
TAG (L"%%Trim duration (s)%,")
DEFINITION (L"specifies the maximum allowed silence duration.")
TAG (L"%%Minimum pitch (Hz)%, and, %Time step (s)%,")
DEFINITION (L"determine how we measure the intensities on which the determination of silent intervals is based. See @@Sound: To Intensity...@ for more info.")
TAG (L"%%Silence threshold (dB)%, %%Minimum silent interval duration (s)%, and %%Minimum sounding interval duration%,")
DEFINITION (L"determine how the silent intervals will be determined. See @@Intensity: To TextGrid (silences)...@ for more info.")
TAG (L"%%Save trimming info as TextGrid%,")
DEFINITION (L"determines if a TextGrid with trimming information will also be created. The TextGrid will have one tier where interval of the %%originating% sound that were trimmed have been labeled. ")
TAG (L"%%Trim label%,")
DEFINITION (L"determines the label that the trimmed intervals in the TextGrid will get.")
MAN_END

MAN_BEGIN (L"Sound & Pitch: To FormantFilter...", L"djmw", 20010404)
INTRO (L"A command that creates a @FormantFilter object from the selected "
	"@Sound and @Pitch objects by @@band filtering in the frequency domain@ with a "
	"bank of filters whose bandwidths depend on the Pitch.")
NORMAL (L"The filter functions used are:")
FORMULA (L"%#H(%f, %F__0_) = 1 / (((%f__%c_^^2^ - %f^2) /%f\\.c%B(%F__0_)))^2 + 1),")
NORMAL (L"where %f__%c_ is the central (resonance) frequency of the filter. "
	"%B(%F__0_) is the bandwidth in Hz and determined as")
FORMULA (L"%B(%F__0_) = %relativeBandwidth\\.c%F__0_, ")
NORMAL (L"where %F__0_ is the fundamental frequency as determined from the Pitch "
	"object. Whenever the value of %F__0_ is undefined, a value of 100 Hz is taken.")
MAN_END

MAN_BEGIN (L"Sound: To MelFilter...", L"djmw", 20010404)
INTRO (L"A command that creates a @MelFilter object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of filters.")
NORMAL (L"The filter functions used are triangular in shape on a %linear "
	"frequency scale. The filter function depends on three parameters, the "
	"lower frequency %f__%l_, the central frequency %f__%c_ and the higher "
	"frequency %f__%h_. "
	"On a %mel scale, the distances %f__%c_-%f__%l_ and %f__%h_-%f__%c_ "
	"are the same for each filter and are equal to the distance between the "
	"%f__%c_'s of successive filters. The filter function is:" )
FORMULA (L"%#H(%f) = 0 for %f \\<_ %f__%l_ and %f \\>_ %f__%h_")
FORMULA (L"%#H(%f) = (%f - %f__%l_) / (%f__%c_ - %f__%l_) for %f__%l_ \\<_ %f \\<_ %f__%c_")
FORMULA (L"%#H(%f) = (%f__%h_ - %f) / (%f__%h_ - %f__%c_) for %f__%c_ \\<_ %f \\<_ %f__%h_")
MAN_END

MAN_BEGIN (L"Sound: To Pitch (shs)...", L"djmw", 19970402)
INTRO (L"A command that creates a @Pitch object from every selected @Sound object.")
ENTRY (L"Purpose")
NORMAL (L"to perform a pitch analysis based on a spectral compression model. "
	"The concept of this model is that each spectral component not only activates "
	"those elements of the central pitch processor that are most sensitive to the "
	"component's frequency, but also elements that have a lower harmonic "
	"relation with this component. Therefore, when a specific element of the "
	"central pitch processor is most sensitive at a frequency %f__0_, it receives "
	"contributions from spectral components in the "
	"signal at integral multiples of %f__0_.")
ENTRY (L"Algorithm")
NORMAL (L"The spectral compression consists of the summation of a sequence of "
	"harmonically compressed spectra. "
	"The abscissa of these spectra is compressed by an integral factor, the rank "
	"of the compression. The maximum of the resulting sum spectrum is the "
	"estimate of the pitch. Details of the algorithm can be "
	"found in @@Hermes (1988)@")
ENTRY (L"Settings")
TAG (L"##Time step (s)# (standard value: 0.01 s)")
DEFINITION (L"the measurement interval (frame duration), in seconds.")
TAG (L"##Minimum pitch (Hz)# (standard value: 50 Hz)")
DEFINITION (L"candidates below this frequency will not be recruited. This parameter "
	"determines the length of the analysis window.")
TAG (L"##Max. number of candidates# (standard value: 15)")
DEFINITION (L"The maximum number of candidates that will be recruited.")
TAG (L"##Maximum frequency (Hz)# (standard value: 1250 Hz)")
DEFINITION (L"higher frequencies will not be considered.")
TAG (L"##Max. number of subharmonics# (standard value: 15)")
DEFINITION (L"the maximum number of harmonics that add up to the pitch.")
TAG (L"##Compression factor# (standard value: 0.84)")
DEFINITION (L"the factor by which successive compressed spectra are multiplied before the summation.")
TAG (L"##Number of points per octave# (standard value: 48)")
DEFINITION (L"determines the sampling of the logarithmic frequency scale.")
TAG (L"##Ceiling (Hz)# (standard value: 500 Hz)")
DEFINITION (L"candidates above this frequency will be ignored.")
MAN_END

MAN_BEGIN (L"Spectra: Multiply", L"djmw", 20100318)
INTRO (L"Returns a new Spectrum object that is the product of the two selected "
	"@Spectrum objects.")
MAN_END

MAN_BEGIN (L"Spectrum: Conjugate", L"djmw", 20031023)
INTRO (L"Reverses the sign of the complex part of the selected @Spectrum object(s).")
NORMAL (L"For real signals, conjugation in the spectral domain amounts to time-inversion in the time domain.")
MAN_END

MAN_BEGIN (L"Spectrum: Shift frequencies...", L"djmw", 20121028)
INTRO (L"Creates a new @Spectrum by shifting all frequencies of the selected Spectrum upwards or downwards.")
ENTRY (L"Settings")
TAG (L"##Shift by (Hz)")
DEFINITION (L"a positive value shifts the spectrum towards higher frequencies, a negative value shifts the spectrum "
	"towards lower frequencies.")
TAG (L"##New maximum frequency (Hz)")
DEFINITION (L"the maximum frequency in the new Spectrum.")
TAG (L"##Precision")
DEFINITION (L"the number of neighbouring frequency points that are used in the calculation of the new frequency points. "
	"The precision relates linearly to the amount of computing time needed to get the new shifted spectrum.")
MAN_END

MAN_BEGIN (L"SpeechSynthesizer", L"djmw", 20120413)
INTRO (L"The SpeechSynthesizer is one of the @@types of objects@ in Praat. It creates a speech sound from text. The actual text-to-speech synthesis is performed by the @@Espeak@ speech synthsizer and therefore our SpeechSynthsizer is merely an interface to Espeak.")
ENTRY (L"Commands")
NORMAL (L"Creation:")
LIST_ITEM (L"\\bu @@Create SpeechSynthesizer...@")
NORMAL (L"Playing:")
LIST_ITEM (L"\\bu @@SpeechSynthesizer: Play text...|Play text...@")
LIST_ITEM (L"\\bu @@SpeechSynthesizer: To Sound...|To Sound...@")
NORMAL (L"Modification:")
LIST_ITEM (L"\\bu @@SpeechSynthesizer: Set text input settings...|Set text input settings...@")
LIST_ITEM (L"\\bu @@SpeechSynthesizer: Set speech output settings...|Set speech output settings...@")
MAN_END

MAN_BEGIN (L"Create SpeechSynthesizer...", L"djmw", 20120221)
INTRO (L"Creates the @@Espeak@ speech synthesizer.")
ENTRY (L"Settings")
TAG (L"##Language#")
DEFINITION (L"determines the language of the synthesizer.")
TAG (L"##Voice variant#")
DEFINITION (L"determines which voice type the synthesizer uses (male, female or whispered voices).")
MAN_END

MAN_BEGIN (L"SpeechSynthesizer: Play text...", L"djmw", 20120413)
INTRO (L"The selected @@SpeechSynthesizer@ plays a text")
ENTRY (L"Settings")
TAG (L"##Text#")
DEFINITION (L"is the text to be played. Text within [[ ]] is treated as phonemes codes in @@Kirshenbaum phonetic encoding@. For example, besides a text like \"This is text\", you might also input \"This [[Iz]] text\".")
MAN_END

MAN_BEGIN (L"SpeechSynthesizer: To Sound...", L"djmw", 20120414)
INTRO (L"The selected @@SpeechSynthesizer@ converts a text to the corresponding speech sound.")
ENTRY (L"Settings")
TAG (L"##Text#")
DEFINITION (L"is the text to be played. Text within [[ ]] is treated as phonemes codes in @@Kirshenbaum phonetic encoding@. For example, besides a text like \"This is text\", you might also input \"This [[Iz]] text\".")
TAG (L"##Create TextGrid with annotations#")
DEFINITION (L"determines whether, besides the sound, a TextGrid with multiple-tier annotations will appear.")
MAN_END

MAN_BEGIN (L"SpeechSynthesizer: Set text input settings...", L"djmw", 20120414)
INTRO (L"A command available in the ##Modify# menu when you select a @@SpeechSynthesizer@.")
ENTRY (L"Settings")
TAG (L"##Input text format is#")
DEFINITION (L"determines how the input text will be synthesized.")
TAG (L"##Input phoneme codes are#")
DEFINITION (L"")
MAN_END

MAN_BEGIN (L"SpeechSynthesizer: Set speech output settings...", L"djmw", 20120414)
INTRO (L"A command available in the ##Modify# menu when you select a @@SpeechSynthesizer@.")
ENTRY (L"Settings")
TAG (L"##Sampling frequency#")
DEFINITION (L"determines how the sampling frequency of the sound.")
TAG (L"##Gap between words#")
DEFINITION (L"determines the amount of silence between words.")
TAG (L"##Pitch adjustment#")
DEFINITION (L"")
TAG (L"##Pitch range#")
DEFINITION (L"")
TAG (L"##Words per minute#")
DEFINITION (L"determines the speaking rate in words per minute.")
TAG (L"##estimate words per minute from data#")
DEFINITION (L"")
TAG (L"##Output phoneme codes are#")
MAN_END

MAN_BEGIN (L"SSCP", L"djmw", 19981103)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type SSCP represents the sums of squares and cross products of "
	"a multivariate data set.")
NORMAL (L"Besides the matrix part, an object of type SSCP also contains a "
	"vector with centroids.")
ENTRY (L"Inside a SSCP")
NORMAL (L"With @Inspect you will see that this type contains the same "
	"attributes as a @TableOfReal with the following extras:")
TAG (L"%numberOfObservations")
TAG (L"%centroid")
MAN_END

MAN_BEGIN (L"SSCP: Draw sigma ellipse...", L"djmw", 19990222)
INTRO (L"A command to draw for the selected @SSCP an ellipse that "
	"covers a part of the multivariate data.")
ENTRY (L"Setting")
TAG (L"##Number of sigmas")
DEFINITION (L"determines the @@concentration ellipse|data coverage@.")
MAN_END

MAN_BEGIN (L"SSCP: Get sigma ellipse area...", L"djmw", 20000525)
INTRO (L"A command to query the selected @SSCP object for the area of a "
	"sigma ellipse.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm proceeds as follows:")
LIST_ITEM (L"1. The four array elements in the SSCP-matrix that correspond to the chosen dimensions "
	"are copied into a two-dimensional matrix #%S (symmetric of course).")
LIST_ITEM (L"2. The eigenvalues of #%S are determined, call them %s__1_ and %s__2_.")
LIST_ITEM (L"3. The lengths %l__%i_  of the axes of the ellipse can be obtained as the  "
	"square root of the %s__i_ multiplied by a scale factor: %l__%i_ = %scaleFactor \\.c "
	"\\Vr (%s__%i_ ), "
	"where %scaleFactor = %numberOfSigmas / \\Vr(%numberOfObservations \\-- 1).")
LIST_ITEM (L"4. The area of the ellipse will be %\\pi\\.c%l__1_\\.c%l__2_.")
MAN_END

MAN_BEGIN (L"SSCP: Get confidence ellipse area...", L"djmw", 20000525)
INTRO (L"A command to query the selected @SSCP object for the area of a "
	"confidence ellipse.")
ENTRY (L"Algorithm")
NORMAL (L"The algorithm proceeds as follows:")
LIST_ITEM (L"1. The four array elements in the SSCP-matrix that correspond to the chosen dimensions "
	"are copied into a two-dimensional matrix #%S (symmetric of course).")
LIST_ITEM (L"2. The eigenvalues of #%S are determined, call them %s__1_ and %s__2_.")
LIST_ITEM (L"3. The lengths %l__1_ and %l__2_ of the two axes of the ellipse can be obtained as "
	"(see for example @@Johnson (1998)@, page 410): ")
FORMULA (L"    %l__%i_ = %scaleFactor \\.c \\Vr (%s__%i_ ),")
LIST_ITEM (L"     where")
FORMULA (L"%scaleFactor = \\Vr (%f \\.c %p \\.c (%n \\-- 1) / (%n \\.c (%n \\-- %p))),")
LIST_ITEM (L"     in which %f = $$@@invFisherQ@$ (1 \\-- %confidenceLevel, %p, %n \\-- %p), "
	"where %p is the numberOfRows from the SSCP object and %n the %numberOfObservations.")
LIST_ITEM (L"4. The area of the ellipse will be %\\pi\\.c%l__1_\\.c%l__2_.")
MAN_END

MAN_BEGIN (L"SSCP: Get diagonality (bartlett)...", L"djmw", 20011111)
INTRO (L"Tests the hypothesis that the selected @SSCP matrix object is "
	"diagonal.")
ENTRY (L"Setting")
TAG (L"##Number of constraints")
DEFINITION (L"modifies the number of independent observations. "
	"The default value is 1.")
ENTRY (L"Algorithm")
NORMAL (L"The test statistic is |#R|^^N/2^, the N/2-th power of the determinant"
	" of the correlation matrix. @@Bartlett (1954)@ developed the following "
	"approximation to the limiting distribution:")
FORMULA (L"\\ci^2 = -(%N - %numberOfConstraints - (2%p + 5) /6) ln |#R|")
NORMAL (L"In the formula's above, %p is the dimension of the correlation "
	"matrix, %N-%numberOfConstraints is the number of independent "
	"observations. Normally %numberOfConstraints would "
	"equal 1, however, if the matrix has been computed in some other way, "
	"e.g., from within-group sums of squares and cross-products of %k "
	"independent groups, %numberOfConstraints would equal %k.")
NORMAL (L"We return the probability %\\al as ")
FORMULA (L"%\\al = chiSquareQ (\\ci^2 , %p(%p-1)/2).")
NORMAL (L"A very low %\\al indicates that it is very improbable that the "
	"matrix is diagonal.")
MAN_END

MAN_BEGIN (L"SSCP: Get fraction variation...", L"djmw", 20040210)
INTRO (L"A command to ask the selected @SSCP object for the fraction "
	"of the total variation that is accounted for by the selected dimension(s).")
NORMAL (L"Further details can be found in @@Covariance: Get fraction variance...@.")
MAN_END

MAN_BEGIN (L"SSCP: To CCA...", L"djmw", 20031103)
INTRO (L"A command that creates a @@CCA|canonical correlation@ object from the "
	"selected @SSCP object.")
ENTRY (L"Setting")
TAG (L"##Dimension of dependent variate (ny)")
DEFINITION (L"defines a partition of the square %n x %n SSCP matrix S into the parts S__yy_ of "
	"dimension %ny x %ny, S__xx_ of dimension %nx x %nx, and the parts "
	"S__xy_ and S__yx_ of dimensions %nx x %ny and %ny x %nx, respectively.")
ENTRY (L"Algorithm")
NORMAL (L"The partition for the square SSCP-matrix is as follows:")
PICTURE (2.0, 2.0, drawPartionedMatrix)
NORMAL (L"The canonical correlation equations we have to solve are:")
FORMULA (L"(1)    (#S__%yx_ #S__%xx_^^-1^ #S__%yx_\\'p -\\la #S__%yy_)#y = #0")
FORMULA (L"(2)    (#S__%yx_\\'p #S__%yy_^^-1^ #S__%yx_ -\\la #S__%xx_)#x = #0")
NORMAL (L"where #S__%yy_ [%ny \\xx %ny] and #S__%xx_ [%nx \\xx %nx] are "
	"symmetric, positive definite matrices belonging to the dependent and the "
	"independent variables, respectively. ")
NORMAL (L"These two equations are not independent and we will show that both "
	"equations have the same eigenvalues and that the eigenvectors #x for "
	"equation (2) can be obtained from the eigenvectors #y of equation (1).")
NORMAL (L"We can solve equation (1) in several ways, however, the numerically "
	"stablest algorithm is probably by performing first a Cholesky decomposition "
	"of #S__xx_ and #S__yy_, followed by a @@generalized singular value "
	"decomposition@. The algorithm goes as follows:")
NORMAL (L"The Cholesky decompositions (\"square roots\") of #S__yy_ and #S__xx_ are:")
FORMULA (L"#S__yy_ = #U\\'p #U and #S__xx_ = #H\\'p #H,")
NORMAL (L"where #U and H are upper triangular matrices. From these decompositions, "
	"the inverse for #S__xx_^^-1^ is easily computed. Let #K be the inverse of #H, "
	"then we can write: ")
FORMULA (L"#S__xx_^^-1^ = #K #K\\'p.")
NORMAL (L"We next substitute in equation (1) and rewrite as:")
FORMULA (L"((#K\\'p#S__yx_\\'p)\\'p (#K\\'p#S__yx_\\'p) - \\la #U\\'p #U)#x = 0")
NORMAL (L"This equation can be solved for eigenvalues and eigenvectors by the "
	"generalized singular value decomposition because it is of the form "
	"#A\\'p#A -\\la #B\\'p#B.")
NORMAL (L"Now, given the solution for equation (1) we can find the solution "
	"for equation (2) by first multiplying (1) from the left with "
	"#S__yx_\\'p#S__yy_^^-1^, resulting in:")
FORMULA (L"(#S__yx_\\'p#S__yy_^^-1^#S__yx_#S__xx_^^-1^#S__yx_\\'p -\\la #S__yx_\\'p) #y = 0")
NORMAL (L"Now we split of the term #S__xx_^^-1^#S__yx_\\'p and obtain:")
FORMULA (L"(#S__yx_\\'p#S__yy_^^-1^#S__yx_ - \\la #S__xx_) #S__xx_^^-1^#S__yx_\\'p #y = 0")
NORMAL (L"This equation is like equation (2) and it has therefore the same eigenvalues "
	"and eigenvectors. (We also proved this fact in the algorithmic section of "
	"@@TableOfReal: To CCA...@.)")
NORMAL (L"The eigenvectors #x is now")
FORMULA (L"#x = #S__xx_^^-1^#S__yx_\\'p #y.")
MAN_END

MAN_BEGIN (L"SSCP: To Covariance...", L"djmw", 20090624)
INTRO (L"A command that creates a @Covariance object from each selected @SSCP object.")
ENTRY (L"Setting")
TAG (L"##Number of constraints")
DEFINITION (L"determines the factor by which each entry in the "
	"SSCP-matrix is scaled to obtain the Covariance matrix.")
ENTRY (L"Details")
NORMAL (L"The relation between the numbers %c__%ij_ in the covariance matrix and the numbers %s__%ij_ in "
	"the originating SSCP matrix is:")
FORMULA (L"%c__%ij_ = %s__%ij_ / (%numberOfObservations - %numberOfConstraints)")
NORMAL (L"Normally %numberOfConstraints will equal 1. However, when the SSCP was the "
	"result of summing %g SSCP objects, as is, for example, the case when you obtained the total "
	"within-groups SSCP from pooling the individual group SSCP's, %numberOfConstraints will equal the number of pooled SSCP's,  %g.")
MAN_END

MAN_BEGIN (L"SSCP & TableOfReal: Extract quantile range...", L"djmw", 20040225)
INTRO (L"Extract those rows from the selected @TableOfReal object whose Mahalanobis "
	"distance, with respect to the selected @SSCP object, are within the "
	"quantile range.")
MAN_END

MAN_BEGIN (L"T-test", L"djmw", 20020117)
INTRO (L"A test on the mean of a normal variate when the variance is unknown.")
NORMAL (L"In Praat, the t-test is used to query a @Covariance object and:")
LIST_ITEM (L"1. get the significance of one mean. See @@Covariance: Get "
	"significance of one mean...@.")
LIST_ITEM (L"2. get the significance of the %difference between two means. "
	"See @@Covariance: Get significance of means difference...@.")
NORMAL (L"You should use a t-test when you want to test a hypothesis about "
	"the mean of one column in a @TableOfReal object, or, if you want to test "
	"a hypothesis about the difference between the means of two columns in "
	"this object.")
NORMAL (L"You can perform these t-tests in Praat by first transforming the "
	"TableOfReal object into a Covariance object (see @@TableOfReal: To "
	"Covariance@) and then choosing the appropriate query method on the "
	"latter object.")
MAN_END

MAN_BEGIN (L"BHEP multivariate normality test", L"djmw", 20101124)
INTRO (L"The Baringhaus–Henze–Epps–Pulley multivariate normality test.")
NORMAL (L"According to @@Henze & Wagner (1997)@ the test has:")
LIST_ITEM (L"\\bu affine invariance,")
LIST_ITEM (L"\\bu consistency against each fixed nonnormal alternative distribution,")
LIST_ITEM (L"\\bu asymptotic power against contiguous alternatives of order \\Vr (%n),")
LIST_ITEM (L"\\bu feasibility for any dimension and any sample size.")
NORMAL (L"The test depends on a smoothing parameter %%h% that can be chosen in various ways:")
NORMAL (L"@@Henze & Wagner (1997)@ recommend %h = 1.41, while")
NORMAL (L"@@Tenreiro (2009)@ recommends  %h__%%s% _= 0.448 + 0.026\\.c%d for short tailed alternatives and "
" %h__%%l%_ = 0.928 + 0.049\\.c%d for long tailed alternatives.")
MAN_END

MAN_BEGIN (L"Table: Normal probability plot...", L"djmw", 20130619)
NORMAL (L"In a normal probability plot, the data in the selected column of the @Table are plotted "
	"against a normal distribution in such a way that the points should form approximately a straight line. "
	"Departures from a straight line indicate departures from normality.")
ENTRY (L"Settings")
TAG (L"##Number of quantiles#")
DEFINITION (L"the number of quantile points, %n, in the plot. From this number %n, the quantile points are "
	"determined as follows: the last quantile point is %q__%n_ = 0.5^^1/%n^ and the first quantile point is "
	"%q__1_=1\\--%q__%n_. The intermediate quantile points %q__%i_ are determined according to "
	"%q__%i_=(%i \\-- 0.3175)/(%n + 0.365), where %i runs from 2 to %n\\--1.")
TAG (L"##Number of sigmas#")
DEFINITION (L"determines the horizontal and vertical drawing ranges in units of standard deviations. ")
MAN_END

MAN_BEGIN (L"Table: Quantile-quantile plot...", L"djmw", 20130619)
NORMAL (L"In a quantile-quantile plot the quantiles of the data in the first selected column of the @Table are plotted against "
	"the quantiles of the data in the second selected column.  If the two sets come from a population with the "
	"same distribution, the points should fall approximately along the reference line.")
MAN_END

MAN_BEGIN (L"Table: Bar plot where...", L"djmw", 20130624)
INTRO (L"Draws a bar plot from data in one or more columns of the selected @Table. In a bar plot the horizontal axis has nominal values (labels). ")
ENTRY (L"Settings")
SCRIPT (6, Manual_SETTINGS_WINDOW_HEIGHT (10), L""
	Manual_DRAW_SETTINGS_WINDOW ("Table: Bar plot where", 10)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Vertical column(s)", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE("Vertical range", "0.0", "0.0 (=autoscaling)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Column with labels", "")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Distance of first bar from border", "1.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Distance between bar groups", "1.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Distance between bars within group", "0.0")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Colours (0-1, name, {r,g,b})", "Grey")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Label text angle (degrees)", "0.0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
	Manual_DRAW_SETTINGS_WINDOW_TEXT("Formula:", "row>1 and row < 10")
)
TAG (L"##Vertical column(s)")
DEFINITION (L"you list the table columns that you want to represent in the bar plot. The number of selected columns is the group size.")
TAG (L"##Vertical range")
DEFINITION (L"determine the lower and upper limit of the display.")
TAG (L"##Column with labels")
DEFINITION (L"determines the column whose labels will be put at the bottom of the plot.")
TAG (L"##Distance of first bar from border")
DEFINITION (L"determines how far the first (and last) bar wil be positioned from the borders (in units of the width of one bar).")
TAG (L"##Distance between bar groups")
DEFINITION (L"determines how far groups of bars are from each other. ")
TAG (L"##Distance between bars within group")
DEFINITION (L"determines the distance between the bars within each group.")
TAG (L"##Colours")
DEFINITION (L"determines the colours of the bars in a group.")
TAG (L"##Label text angle (degrees)")
DEFINITION (L"determines the angle of the labels written below the plot. If you have very long label texts you can prevent the label texts from overlapping.")
TAG (L"##Formula:")
DEFINITION (L"can be used to supply an expression to select only those rows for plotting where the expression evaluates to %%true%. A 1 value always evaluates to %%true%.")
ENTRY (L"Examples")
NORMAL (L"@@Keating & Esposito (2006)@ present a bar plot in their fig. 3 from which we estimate the following data table")
CODE (L"Language        Modal  Breathy")
CODE (L"Chong            -1.5    5")
CODE (L"Fuzhou            2     10")
CODE (L"Green Hmong       3     12")
CODE (L"White Hmong       2     11")
CODE (L"Mon              -1.5    0")
CODE (L"SADV Zapotec     -6     -4")
CODE (L"SLQ Zapotec       3.5   14")
CODE (L"Tlacolula Zapotec 3     13")
CODE (L"Tamang            1      1")
CODE (L"!Xoo              1     14")
NORMAL (L"Given that we have these data in a Table with the three columns labeled \"Language\", \"Modal\" and \"Breathy\", "
	"respectively, we can first try to reproduce their figure 3 (a bar plot with both Modal and Breathy columns displayed) ")
NORMAL (L"As you can see the labels in the first column are very long texts and they will surely overlap if "
	"plotted at the bottom of a plot. We therefore use a value of 15 degrees for the \"Label text angle\" " "parameter. This "
	"will make the label texts nonoverlapping. We cannot make this angle much larger because then the label texts will run out of "
	"the viewport. ")
NORMAL (L"Sometimes you need to plot only a part of the Table and for the selection of this part, the \"Formula\" field can be "
	"used. Since we only have a small table we put a \"1\" in this field which always evaluates to true. In effect, all the rows will be selected. The following script line will produce the picture below.")
CODE (L"do (\"Bar plot where...\", \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"0.9 0.5\", 15.0, \"yes\", \"1\")")
SCRIPT (5, 3,  L"h1h2 = do (\"Create H1H2 table (Esposito 2006)\")\n"
	"do (\"Font size...\", 10)\n"
	"do (\"Bar plot where...\", \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"0.9 0.5\", 15.0, \"yes\", \"1\")\n"
	"removeObject (h1h2)\n")
NORMAL (L"The essentials of the bart plot in their paper are perfectly reproduced in the figure above. If you want the bars within a group to be placed somewhat more apart say 0.2 (times the bar width) you can set the \"Distance between bars in a group\" to a value of 0.2:")
CODE (L"do (\"Bar plot where...\", \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.2, \"0.9 0.5\", 15.0, \"yes\", \"1\")")
SCRIPT (5, 3,  L"h1h2 = do (\"Create H1H2 table (Esposito 2006)\")\n"
	"do (\"Font size...\", 10)\n"
	"do (\"Bar plot where...\", \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.2, \"0.9 0.5\", 15.0, \"yes\", \"1\")\n"
	"removeObject (h1h2)\n")
NORMAL (L"Of course we can also work with colours and we can add vertical marks as the following sriptlet shows")
CODE (L"do (\"Bar plot where...\", \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"Green Red\", 15.0, \"yes\", \"1\")")
CODE (L"do (\"Marks left every...\", 1, 5, 1, 1, 1)")
CODE (L"do (\"Text left...\", 1, \"H__1_-H__2_ (dB)\")")
SCRIPT (5, 3,  L"h1h2 = do (\"Create H1H2 table (Esposito 2006)\")\n"
	"do (\"Font size...\", 10)\n"
	"do (\"Bar plot where...\", \"Modal Breathy\", -10, 20, \"Language\", 1.0, 1.0, 0.0, \"Green Red\", 15.0, \"yes\", \"1\")\n"
	"do (\"Marks left every...\", 1, 5, 1, 1, 1)\n"
	"do (\"Text left...\", 1, \"H__1_-H__2_ (dB)\")\n"
	"removeObject (h1h2)\n")
MAN_END

MAN_BEGIN (L"Table: Line graph where...", L"djmw", 20130624)
INTRO (L"Draws a line graph from the data in a column of the selected @Table. In a line plot the horizontal axis can have a nominal scale or a numeric scale. The data point are connected by line segments.")
ENTRY (L"Settings")
SCRIPT (6, Manual_SETTINGS_WINDOW_HEIGHT (8), L""
	Manual_DRAW_SETTINGS_WINDOW ("Table: Line graph where", 8)
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Vertical column", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Vertical range", "0.0", "0.0 (=autoscaling)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Horizontal column", "")
	Manual_DRAW_SETTINGS_WINDOW_RANGE ("Horizontal range", "0.0", "0.0 (=autoscaling)")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Text", "+")
	Manual_DRAW_SETTINGS_WINDOW_FIELD ("Label text angle (degrees)", "0.0")
	Manual_DRAW_SETTINGS_WINDOW_BOOLEAN("Garnish", 1)
	Manual_DRAW_SETTINGS_WINDOW_TEXT("Formula:", "1; (=everything)")
)
TAG (L"##Vertical column")
DEFINITION (L"The column whose data points you want to plot.")
TAG (L"##Vertical range")
DEFINITION (L"determine the lower and upper limit of the plot.")
TAG (L"##Horizontal column")
DEFINITION (L"determines the horizontal scale. If you leave it empty, or, if the (selected part of the) selected column contains nominal values, i.e. the values are not numeric but text, the horizontal "
	"distance between the data points will be constant (i.e. 1) and the nominal values (texts) will be put as labels at the bottom of the horizontal axis. "
	"On the other hand, if this column contains only numerical values, the horizontal position of the data points will be determined by the values in this column.")
TAG (L"##Horizontal range")
DEFINITION (L"determines the left and right limit of the plot.")
TAG (L"##Text")
DEFINITION (L"The text to put at the position of the data point in the plot.")
TAG (L"##Label text angle (degrees)")
DEFINITION (L"determines the angle of the labels written %%below% the plot. If you have very long label texts in the \"Horizontal column\", you can prevent the label texts from overlapping. This only has effect for a horizontal column with nominal values.")
TAG (L"##Formula")
DEFINITION (L"can be used to supply an expression to select only those rows for plotting where the expression evaluates to %%true%. A 1 value always evaluates to %%true%.")
ENTRY (L"Examples")
NORMAL (L"The following table was estimated from fig. 3 in @@Ganong (1980)@ and represents the fraction /d/ responses as a function of a "
	"voice onset time (VOT) continuum. The second column shows the responses in a word - nonword continuum, while the third column shows "
	"the responses to a nonword - word continuum.")
CODE (L"VOT dash-tash dask-task")
CODE (L"-17.5   0.98      0.92")
CODE (L" -7.5   0.95      0.83")
CODE (L" -2.5   0.71      0.33")
CODE (L"  2.5   0.29      0.10")
CODE (L"  7.5   0.12      0.02")
CODE (L" 17.5   0.10      0.02")
NORMAL (L"We can reproduce fig. 3 from Ganong (1980) with the following script, where we labeled the word - nonword curve with \"wn\" and the nonword - word curve with \"nw\". We deselect \"Garnish\" because we want to put special marks at the bottom.")
CODE (L"do (\"Dotted line\")\n")
CODE (L"do (\"Line graph where...\", \"dash-tash\", 0, 1, \"VOT\", -20, 20, \"wn\", 0, 0, \"1\")")
CODE (L"do (\"Dashed line\")\n")
CODE (L"do (\"Line graph where...\", \"dask-task\", 0, 1, \"VOT\", -20, 20, \"nw\", 0, 0, \"1\")")
CODE (L"do (\"Draw inner box\")")
CODE (L"do (\"One mark bottom...\", 2.5, 0, 1, 0, \"+2.5\")")
CODE (L"do (\"One mark bottom...\", -2.5, 1, 1, 0, \"\")")
CODE (L"do (\"One mark bottom...\", -7.5,1, 1, 0, \"\")")
CODE (L"do (\"One mark bottom...\", 7.5, 0, 1, 0, \"+7.5\")")
CODE (L"do (\"One mark bottom...\", 2.5, 0, 0, 0, \"+2.5\")")
CODE (L"do (\"One mark bottom...\", -20, 0, 0, 0, \"Short VOT\")")
CODE (L"do (\"One mark bottom...\", 20, 0, 0, 0, \"Long VOT\")")
CODE (L"do (\"Text bottom...\", 1, \"VOT (ms)\")")
CODE (L"do (\"Marks left every...\", 1, 0.2, 1, 1, 0)")
CODE (L"do (\"Text left...\", 1, \"Prop. of voiced responses\")")

SCRIPT (5,3, L"ganong = do (\"Create Table (Ganong 1980)\")\n"
	"do (\"Dotted line\")\n"
	"do (\"Line graph where...\", \"dash-tash\", 0, 1, \"VOT\", -20, 20, \"wn\", 0, 0, \"1\")\n"
	"do (\"Dashed line\")\n"
	"do (\"Line graph where...\", \"dask-task\", 0, 1, \"VOT\", -20, 20, \"nw\", 0, 0, \"1\")\n"
	"do (\"Draw inner box\")\n"
	"do (\"One mark bottom...\", 2.5, 0, 1, 0, \"+2.5\")\n"
	"do (\"One mark bottom...\", -2.5, 1, 1, 0, \"\")\n"
	"do (\"One mark bottom...\", -7.5,1, 1, 0, \"\")\n"
	"do (\"One mark bottom...\", 7.5, 0, 1, 0, \"+7.5\")\n"
	"do (\"One mark bottom...\", 2.5, 0, 0, 0, \"+2.5\")\n"
	"do (\"One mark bottom...\", -20, 0, 0, 0, \"Short VOT\")\n"
	"do (\"One mark bottom...\", 20, 0, 0, 0, \"Long VOT\")\n"
	"do (\"Text bottom...\", 1, \"VOT (ms)\")\n"
	"do (\"Marks left every...\", 1, 0.2, 1, 1, 0)\n"
	"do (\"Text left...\", 1, \"Prop. of voiced responses\")\n"
	"removeObject (ganong)\n"
)
NORMAL (L"As an example of what happens if you don't supply an argument for the \"Horizontal column\" we will use the same table as for the previous plot. However the resulting plot may not be as meaningful (note that the horizontal nominal scale makes all points equidistant in the horizontal direction.)")
CODE (L"do (\"Dotted line\")\n")
CODE (L"do (\"Line graph where...\", \"dash-tash\", 0, 1, \"\", 0, 0, \"wn\", 0, 1, \"1\")")
CODE (L"do (\"One mark bottom...\", 1, 0, 1, 0, \"Short VOT\")")
SCRIPT (5,3, L"ganong = do (\"Create Table (Ganong 1980)\")\n"
	"do (\"Dotted line\")\n"
	"do (\"Line graph where...\", \"dash-tash\", 0, 1, \"\", 0, 0, \"wn\", 0, 1, \"1\")\n"
	"do (\"One mark bottom...\", 1, 0, 1, 0, \"Short VOT\")\n"
	"removeObject (ganong)\n"
)
MAN_END


MAN_BEGIN (L"Table: Get median absolute deviation...", L"djmw", 20120405)
INTRO (L"Get the median absolute deviation (MAD) of the column in the selected @@Table@ (adjusted by a scale factor).")
ENTRY (L"Algorithm")
NORMAL (L"From the %n numbers %x__1_, %x__2_, ..., %x__%n_ in the selected column we first calculate the @@quantile algorithm|median@ "
	"value %x__median_. Next we calculate the %n absolute deviations from this median: %d__1_, %d__2_, ..., %d__%n_, "
	"where %d__%j_=|%x__%j_ - %x__median_|. "
	"Then we calculate the MAD value, which is the median of the %n values %d__1_, %d__2_, ..., %d__%n_. Finally we multiply the MAD "
	"value by the scale factor 1.4826. This last multiplication makes the result comparable with the value of the standard deviation if "
	"the %x values are normally distributed.")
MAN_END

MAN_BEGIN (L"Table: Report one-way anova...", L"djmw", 20120617)
INTRO (L"Performs a one-way analysis of variance on the data in one column of a selected @@Table@ and reports the fixed-effects anova table results in the info window.")
ENTRY (L"Settings")
TAG (L"##Column with data#")
DEFINITION (L"the label of the column who's data will be analyzed.")
TAG (L"##Factor")
DEFINITION (L"the label of the column with the names of the levels.")
TAG (L"##Table with means")
DEFINITION (L"if checked, a Table with the mean values of the levels will be created.")
TAG (L"##Table with differences between means")
DEFINITION (L"if checked, a Table with the differences between the mean values of the levels will be created.")
TAG (L"##Table with Tukey's post-hoc test")
DEFINITION (L"if checked, a Table with Tukey's HSD tests will be created. Each value in this Table measures the probability that the corresponding difference between the level means happened by chance. The test compares all possible level means and is based on the studentized range distribution.")
MAN_END

MAN_BEGIN (L"Table: Report two-way anova...", L"djmw", 20130410)
INTRO (L"Performs a two-way analysis of variance on the data in one column of a selected %%fully factorial% @@Table@ and reports the fixed-effects anova table in the info window. ")
ENTRY (L"Settings")
TAG (L"##Column with data#")
DEFINITION (L"the label of the column who's data will be analyzed.")
TAG (L"##First factor")
DEFINITION (L"the label of the column with the names of the levels for the first factor.")
TAG (L"##Second factor")
DEFINITION (L"the label of the column with the names of the levels for the second factor.")
TAG (L"##Table with means")
DEFINITION (L"if checked, a Table with the mean values of all the levels will be created.")
ENTRY (L"Example")
NORMAL (L"Suppose you want to check if fundamental frequency depends on the type of vowel and speaker type. We will use the "
	"@@Create formant table (Peterson & Barney 1952)|Peterson & Barney@ vowel data set to illustrate this. "
	"The following script will first create the data set and then produce the two-way anova report." )
CODE (L"do (\"Create formant table (Peterson & Barney 1952)\")")
CODE (L"do (\"Report two-way anova...\", \"F0\", \"Vowel\", \"Type\")")
NORMAL (L"This will produce the following anova table in the info window:")
CODE (L"Two-way analysis of \"F0\" by \"Vowel\" and \"Type\".")
CODE (L"")
CODE (L"      Source             SS        Df             MS         F         P")
CODE (L"       Vowel        73719.4         9        8191.05    7.62537    5.25258e-11")
CODE (L"        Type    4.18943e+06         2    2.09471e+06    1950.05              0")
CODE (L"Vowel x Type        6714.34        18        373.019   0.347258       0.994969")
CODE (L"       Error    1.60053e+06      1490        1074.18")
CODE (L"       Total    5.87039e+06      1519")
NORMAL (L"The analysis shows that F0 strongly depends on the vowel and also on the speaker type and, luckily, we do not have any "
	"interaction between the vowel and the speaker type. Besides the anova table there is also shown a table with the mean F0 "
	"values for each Vowel-Type combination which looks like:")
CODE (L"                   c         m         w      Mean")
CODE (L"        aa       258       124       212       198")
CODE (L"        ae       248       125       208       194")
CODE (L"        ah       263       129       223       205")
CODE (L"        ao       259       127       217       201")
CODE (L"        eh       259       128       220       202")
CODE (L"        er       264       133       219       205")
CODE (L"        ih       270       136       232       213")
CODE (L"        iy       270       136       231       212")
CODE (L"        uh       273       136       234       214")
CODE (L"        uw       278       139       235       218")
CODE (L"      Mean       264       131       223       206")
NORMAL (L"The first column of this table shows the vowel codes while the first row shows the speaker types (child, man, women). "
	"The last row and the last column of the table shows the averages for the factors Type and Vowel, respectively. The actual "
	"data are unbalanced because we have 300, 660 and 560 replications per column respectively (for each speaker we have two replcations of the data).")
ENTRY (L"Algorithm")
NORMAL (L"The formula's to handle unbalanced designs come from @@Khuri (1998)@.")
MAN_END

MAN_BEGIN (L"Table: Report one-way Kruskal-Wallis...", L"djmw", 20120617)
INTRO (L"Performs a one-way Kruskal-Wallis analysis on the data in one column of a selected @@Table@ and reports the results in the info window. This test is sometimes refered to as a one-way analysis of variance for %%non-normally distributed% data.")
ENTRY (L"Settings")
TAG (L"##Column with data#")
DEFINITION (L"the label of the column who's data will be analyzed.")
TAG (L"##Factor")
DEFINITION (L"the label of the column with the names of the levels.")
ENTRY (L"Algorithm")
NORMAL (L"The analysis is done on the ranked data and consists of the following steps:")
LIST_ITEM (L"1. Rank all the %N data points together, i.e. rank the data from 1 to %N.")
LIST_ITEM (L"2. The test statistic is:")
FORMULA (L"%K = (12 / (%N(%N+1)) \\Si__%i=1_^^%g^ %n__%i_ (meanRank__%i_)^^2^ - 3(%N+1),")
DEFINITION (L"where %g is the number of levels, %n__%i_ the number of data in %i-th level and meanRank__%i_ "
	"the average rank of the %i-th level.")
LIST_ITEM (L"3. The %p value is %%approximated by the \\ci^^2^ (%K, %g - 1) distribution.")
MAN_END

MAN_BEGIN (L"TableOfReal: Report multivariate normality (BHEP)...", L"djmw", 20090701)
INTRO (L"Report about multivariate normality according to the @@BHEP multivariate normality test@.")
ENTRY (L"Settings")
TAG (L"##Smoothing parameter")
DEFINITION (L"determines the smoothing parameter %h.")
MAN_END

MAN_BEGIN (L"TableOfReal: Change row labels...", L"djmw", 20010822)
INTRO (L"Changes the row labels of the selected @TableOfReal object according "
	"to the specification in the search and replace fields.")
NORMAL (L"Both search and replace fields may contain @@regular expressions|"
	"Regular expressions@. The ##Replace limit# parameter limits the number of "
	"replaces that may occur within each label.")
MAN_END

MAN_BEGIN (L"TableOfReal: Change column labels...", L"djmw", 20010822)
INTRO (L"Changes the column labels of the selected @TableOfReal object according "
	"to the specification in the search and replace fields.")
NORMAL (L"Both search and replace fields may contain @@regular expressions|"
	"Regular expressions@. The %%Replace limit% parameter limits the number of "
	"replaces that may occur within each label.")
MAN_END

MAN_BEGIN (L"TableOfReal: Draw biplot...", L"djmw", 20020603)
INTRO (L"A command to draw a biplot for each column in the selected "
	"@TableOfReal object.")
ENTRY (L"Settings")
TAG (L"##Xmin#, ##Xmax#, ##Ymin#, ##Ymax#")
DEFINITION (L"determine the drawing boundaries.")
TAG (L"##Split factor")
DEFINITION (L"determines the weighing of the row and column structure "
	"(see below).")
ENTRY (L"Behaviour")
LIST_ITEM (L"1. Get the @@singular value decomposition@ #U #\\Si #V\\'p of the "
	"table.")
LIST_ITEM (L"2. Calculate weighing factors %\\la for row and columns")
FORMULA (L"%\\la__r,1_ = %\\si__1_^^%splitFactor^")
FORMULA (L"%\\la__c,1_ = %\\si__1_^^1-%splitFactor^")
FORMULA (L"%\\la__r,2_ = %\\si__2_^^%splitFactor^")
FORMULA (L"%\\la__c,2_ = %\\si__2_^^1-%splitFactor^")
DEFINITION (L"where %\\si__1_ and %\\si__2_ are the first and the second singular values")
LIST_ITEM (L"3. For the rows (%i from 1..%numberOfRows) form:")
FORMULA (L"%xr__%i_ = %U__%i1_  %\\la__%r,1_")
FORMULA (L"%yr__%i_ = %U__%i2_  %\\la__%r,2_")
LIST_ITEM (L"4. For the columns (%i from 1..%numberOfColumns) form:")
FORMULA (L"%xc__%i_ = %V__%i1_  %\\la__%c,1_")
FORMULA (L"%yc__%i_ = %V__%i2_  %\\la__%c,2_")
LIST_ITEM (L"5. Plot the points (%xr__%i_, yr__%i_) and (%xc__%i_, yc__%i_) in the "
	"same figure with the corresponding row and column labels.")
MAN_END

MAN_BEGIN (L"TableOfReal: Draw box plots...", L"djmw", 20000523)
INTRO (L"A command to draw a @@box plot@ for each column in the selected "
	"@TableOfReal object.")
ENTRY (L"Settings")
TAG (L"##From row#, ##To row#, ##From column#, ##To column#")
DEFINITION (L"determine the part of the table that you want to analyse.")
TAG (L"%Ymin and %Ymax")
DEFINITION (L"determine the drawing boundaries.")
MAN_END

MAN_BEGIN (L"TableOfReal: Draw rows as histogram...", L"djmw", 20030619)
INTRO (L"A command to draw a histogram from the rows in the selected "
	"@TableOfReal object.")
NORMAL (L"The histogram will consist of %groups of bars. The number of groups will "
	"be determined by the number of selected columns from the table, while the "
	"number of bars within each group will be determined from the number of "
	"selected rows.")
ENTRY (L"Settings")
TAG (L"##Row numbers# and ##Column range#")
DEFINITION (L"determine the part of the table that you want to draw. "
	"The column range determines the number of bars that you want to draw for "
	"each row selected by the %%Row numbers% argument.")
TAG (L"##Ymin# and ##Ymax#")
DEFINITION (L"the drawing boundaries.")
NORMAL (L"The following arguments are all relative to the width of a bar "
	"in the histogram. ")
TAG (L"##Horizontal offset")
DEFINITION (L"the offset from the left and right margin.")
TAG (L"##Distance between bar groups")
DEFINITION (L"the distance between each group, i.e., the distance "
	"between the right side of the last bar in a group to the left side of "
	"the first bar in the next group.")
TAG (L"##Distance between bars")
DEFINITION (L"the distance between the bars in a group.")
TAG (L"##Grey values")
DEFINITION (L"the grey values of the bars in a group.")
ENTRY (L"Bar positioning")
NORMAL (L"If you want to put the labels yourself you will need the following information.")
NORMAL (L"The width of a bar is determined as follows:")
FORMULA (L"%width = 1 / (%nc \\.c %nr + 2 \\.c %hoffset + (%nc - 1)\\.c %intergroup +"
		"%nc\\.c(%nr -1)\\.c %interbar),")
NORMAL (L"where %nc is the number of columns (groups) to draw, %nr is the number of "
	"rows to draw (the number of bars within a group), %hoffset is the horizontal "
	"offset, %intergroup the distance between each group and %interbar "
	"the distance between the bars within a group.")
NORMAL (L"The spacing between the bars drawn from a row:")
FORMULA (L"%dx = (%intergroup + %nr + (%nr -1) \\.c %interbar) *% width")
NORMAL (L"The first bar for the %k-th row starts at:")
FORMULA (L"%x1 = %hoffset \\.c %width + (%k - 1) \\.c (1 + %interbar) \\.c %width")
MAN_END

MAN_BEGIN (L"TableOfReal: Select columns where row...", L"djmw", 20130410)
INTRO (L"Copy columns from the selected @TableOfReal object to a new "
	"TableOfReal object.")
ENTRY (L"Settings")
TAG (L"##Columns")
DEFINITION (L"defines the indices of the columns to be selected. Ranges can be "
	"defined with a colon \":\". Columns will be selected in the specified "
	"order.")
TAG (L"##Row condition")
DEFINITION (L"specifies a condition for the selection of rows. If the "
	"condition evaluates as %true for a particular row, the selected elements "
	"in this row will be copied. See @@Matrix: Formula...@ for the kind of "
	"expressions that can be used here.")
ENTRY (L"Examples")
CODE (L"do (\"Select columns where row...\", \"1 2 3\", \"1\")")
CODE (L"do (\"Select columns where row...\", \"1 : 3\", \"1\")")
NORMAL (L"Two alternative expressions to copy the first three columns to a new table "
	"with the same number of rows.")
CODE (L"do (\"Select columns where row...\", \"3 : 1\", \"1\")")
NORMAL (L"Copy the first three columns to a new table with the same number of "
	"rows. The new table will have the 3 columns reversed.")
CODE (L"do (\"Select columns where row...\", \"1:6 9:11\", \"self[row,8]>0\")")
NORMAL (L"Copy the first six columns and columns 9, 10, and 11 to a new table. "
	"Copy only elements from rows where the element in column 8 is greater "
	"than zero.")
MAN_END

MAN_BEGIN (L"TableOfReal: Standardize columns", L"djmw", 19990428)
INTRO (L"Standardizes each column of the selected @TableOfReal.")
NORMAL (L"The entries %x__%ij_ in the TableOfReal will change to:")
FORMULA (L"(%x__%ij_ \\-- %\\mu__%j_) / %\\si__%j_, ")
NORMAL (L"where %\\mu__%j_ and %\\si__%j_ are the mean and the standard deviation as calculated "
	"from the %j^^th^ column, respectively. After standardization all column means will equal zero "
	"and all column standard deviations will equal one.")
MAN_END

MAN_BEGIN (L"TableOfReal: To Configuration (lda)...", L"djmw", 19981103)
INTRO (L"Calculates a @Configuration based on the @Discriminant scores obtained "
	"from the selected @TableOfReal. Row labels in the table indicate group membership.")
ENTRY (L"Setting")
TAG (L"##Number of dimensions")
DEFINITION (L"determines the number of dimensions of the resulting Configuration.")
ENTRY (L"Algorithm")
NORMAL (L"First we calculate the Discriminant from the data in the TableOfReal. "
	"See @@TableOfReal: To Discriminant@ for details.")
NORMAL (L"The eigenvectors of the Discriminant determine the directions that "
	"the data in the TableOfReal will be projected unto.")
MAN_END

MAN_BEGIN (L"TableOfReal: To Configuration (pca)...", L"djmw", 19980909)
INTRO (L"Calculates a @Configuration based on the principal components from the "
	"selected @TableOfReal.")
ENTRY (L"Setting")
TAG (L"##Number of dimensions")
DEFINITION (L"determines the number of dimensions of the resulting Configuration.")
ENTRY (L"Algorithm")
NORMAL (L"We form principal components without explicitly calculating the covariance matrix "
	"#C = #M\\'p\\.c#M, where #M is the matrix part of the TableOfReal. ")
LIST_ITEM (L"1. Make the singular value decomposition of #M. This results in "
	"#M = #U\\.c#d\\.c#V\\'p.")
LIST_ITEM (L"2. Sort singular values #d and corresponding row vectors in #V (descending).")
LIST_ITEM (L"3. The principalComponent__%ij_ = \\su__%k=1..%numberOfColumns_ %M__%ik_ \\.c %V__%jk_.")
ENTRY (L"Remark")
NORMAL (L"The resulting configuration is unique up to reflections along the new principal directions.")
MAN_END

MAN_BEGIN (L"TableOfReal: To Correlation", L"djmw", 20020105)
INTRO (L"A command that creates a (%Pearson) @Correlation object from every "
	"selected @TableOfReal object. The correlations are calculated between "
	"columns.")
ENTRY (L"Algorithm")
NORMAL (L"The linear correlation coefficient %r__%ij_ (also called the %%product"
	" moment correlation coefficient% or %%Pearson's correlation coefficient%) "
	" between the elements of columns %i and %j is calculated as:")
FORMULA (L"%r__%ij_ = \\Si__%k_ (%x__%ki_ - %mean__%i_)(%x__%kj_ - %mean__%j_)/"
	"(\\Vr (\\Si__%k_(%x__%ki_ - %mean__%i_)^2) \\Vr (\\Si__%k_(%x__%kj_ -"
	" %mean__%j_)^2)),")
NORMAL (L"where %x__%mn_ is the element %m in column %n, and %mean__%n_ "
	"is the mean of column %n.")
MAN_END

MAN_BEGIN (L"TableOfReal: To Correlation (rank)", L"djmw", 20020105)
INTRO (L"A command that creates a (%%Spearman rank-order%) @Correlation object "
	"from every selected @TableOfReal object. The correlations are calculated "
	"between columns.")
ENTRY (L"Algorithm")
NORMAL (L"The Spearman rank-order correlation coefficient %r__%ij_ between "
	"the elements of columns %i and %j is calculated as the linear correlation"
	" of the ranks:")
FORMULA (L"%r__%ij_ = \\Si__%k_ (%R__%ki_ - %Rmean__%i_) "
	"(%R__%kj_ - %Rmean__%j_) / (\\Vr (\\Si__%k_(%R__%ki_ - %Rmean__%i_)^2) "
	"\\Vr (\\Si__%k_(%R__%kj_ - %Rmean__%j_)^2)),")
NORMAL (L"where %R__%mn_ is the rank of element %m in column %n, "
	"and %Rmean__%n_ is the mean of the ranks in column %n.")
MAN_END

MAN_BEGIN (L"TableOfReal: To Covariance", L"djmw", 20020117)
INTRO (L"A command that creates a @Covariance object from every "
	"selected @TableOfReal object. The covariances are calculated between "
	"columns.")
ENTRY (L"Algorithm")
NORMAL (L"The covariance coefficients %s__%ij_ "
	" between the elements of columns %i and %j are defined as:")
FORMULA (L"%s__%ij_ = \\Si__%k_ (%x__%ki_ - %mean__%i_)(%x__%kj_ - %mean__%j_)/"
	"(%numberOfObservations - %numberOfConstraints),")
NORMAL (L"where %x__%ki_ is the element %k in column %i, %mean__%i_ "
	"is the mean of column %i, %numberOfObservations equals the number of rows in "
	"the table, and %numberOfConstraints equals 1.")
NORMAL (L"The actual calculation goes as follows")
LIST_ITEM (L"1. Centralize each column (subtract the mean).")
LIST_ITEM (L"2. Get its @@singular value decomposition@ #U #\\Si #V\\'p.")
LIST_ITEM (L"3. Form #S = #V #\\Si #V\\'p.")
LIST_ITEM (L"4. Divide all elements in #S by (%numberOfObservations - 1).")
MAN_END

MAN_BEGIN (L"TableOfReal: To Discriminant", L"djmw", 19990104)
INTRO (L"A command that creates a @Discriminant object from every selected "
	"@TableOfReal object. Row labels in the table indicate group membership.")
ENTRY (L"Algorithm")
NORMAL (L"We solve for directions #x that are eigenvectors of the generalized "
	"eigenvalue equation:")
FORMULA (L"#%B #x - %\\la #%W #x = 0,")
NORMAL (L"where #%B and #%W are the between-groups and the within-groups sums of "
	"squares and cross-products matrices, respectively. Both #%B and #%W are symmetric "
	"matrices. Standard formula show that both matrices can also "
	"be written as a matrix product. The formula above then transforms to:")
FORMULA (L"#%B__1_\\'p#%B__1_ #x - %\\la #%W__1_\\'p#%W__1_ #x = 0")
NORMAL (L"The equation can be solved with the @@generalized singular value decomposition@. "
	"This procedure is numerically very stable and can even cope with cases when both "
	"matrices are singular.")
NORMAL (L"The a priori probabilities in the Discriminant will be calculated from the number of "
	"%training vectors %n__%i_ in each group:")
FORMULA (L"%aprioriProbability__%i_ = %n__%i_ / \\Si__%k=1..%numberOfGroups_ %n__%k_")
MAN_END

MAN_BEGIN (L"TableOfReal: To PCA", L"djmw", 19980106)
INTRO (L"A command that creates a @PCA object from every selected "
	"@TableOfReal object.")
MAN_END

MAN_BEGIN (L"TableOfReal: To SSCP...", L"djmw", 19990218)
INTRO (L"Calculates Sums of Squares and Cross Products (@SSCP) from the selected @TableOfReal.")
ENTRY (L"Algorithm")
NORMAL (L"The sums of squares and cross products %s__%ij_ "
	" between the elements of columns %i and %j are calculated as:")
FORMULA (L"%s__%ij_ = \\Si__%k_ (%x__%ki_ - %mean__%i_)(%x__%kj_ - %mean__%j_),")
NORMAL (L"where %x__%mn_ is the element %m in column %n and %mean__%n_ "
	"is the mean of column %n.")
MAN_END

MAN_BEGIN (L"TableOfReal: To Pattern and Categories...", L"djmw", 20040429)
INTRO (L"Extracts a @Pattern and a @Categories from the selected @TableOfReal.")
NORMAL (L"The selected rows and columns are copied into the Pattern and "
	"the corresponding row labels are copied into a Categories. ")
MAN_END

MAN_BEGIN (L"TableOfReal: To CCA...", L"djmw", 20020424)
INTRO (L"A command that creates a @CCA object from the selected "
	"@TableOfReal object.")
ENTRY (L"Settings")
TAG (L"%%Dimension of dependent variate (ny)")
DEFINITION (L"defines the partition of the table into the two parts whose "
	"correlations will be determined. The first %ny columns must be the "
	"dependent part, the rest of the columns will be interpreted as the "
	"independent part (%nx columns). In general %nx must be larger than or "
	"equal to %ny.")
ENTRY (L"Behaviour")
NORMAL (L"Calculates canonical correlations between the %dependent and the "
	"%independent parts of the table. The corresponding "
	"canonical coefficients are also determined.")
ENTRY (L"Algorithm")
NORMAL (L"The canonical correlation equations for two data sets #T__%y_ "
	"[%n \\xx %p] and #T__%x_ [n \\xx %q] are:")
FORMULA (L"(1)    (#S__%yx_ #S__%xx_^^-1^ #S__%yx_\\'p -\\la #S__%yy_)#y = #0")
FORMULA (L"(2)    (#S__%yx_\\'p #S__%yy_^^-1^ #S__%yx_ -\\la #S__%xx_)#x = #0")
NORMAL (L"where #S__%yy_ [%p \\xx %p] and #S__%xx_ [%q \\xx %q] are the "
	"covariance matrices of data sets #T__%y_ and  #T__%x_, respectively, "
	"#S__%yx_ [%p \\xx %q] is the matrix of covariances between data sets "
	"#T__%y_ and #T__%x_, and the vectors #y and #x are the %%canonical "
	"weights% or the %%canonical function coefficients% for the dependent and "
	"the independent data, respectively. "
	"In terms of the (dependent) data set #T__%y_ and the (independent) data set "
	"#T__%x_, these covariances can be written as:")
FORMULA (L"#S__%yy_ =  #T__%y_\\'p #T__%y_,  #S__%yx_ = #T__%y_\\'p #T__%x_ and "
	"#S__%xx_ =  #T__%x_\\'p #T__%x_.")
NORMAL (L"The following @@singular value decomposition@s ")
FORMULA (L"#T__%y_ = #U__%y_ #D__%y_ #V__%y_\\'p and #T__%x_ = #U__%x_ #D__%x_ "
	"#V__%x_\\'p ")
NORMAL (L"transform equation (1) above into:")
FORMULA (L"(3)    (#V__%y_ #D__%y_ #U__%y_\\'p#U__%x_ #U__%x_\\'p #U__%y_ #D__%y_ "
	"#V__%y_\\'p - \\la #V__%y_ #D__%y_ #D__%y_ #V__%y_\\'p)#y = 0 ")
NORMAL (L"where we used the fact that:")
FORMULA (L"#S__%xx_^^-1^ = #V__%x_ #D__%x_^^-2^ #V__%x_\\'p.")
NORMAL (L"Equation (3) can be simplified by multiplication from the left by "
	"#D__%y_^^-1^ #V__%y_' to:")
FORMULA (L" (4)   ((#U__%x_\\'p #U__%y_)\\'p (#U__%x_\\'p #U__%y_) - \\la #I)#D__%y_ "
	"#V__%y_\\'p #y = #0")
NORMAL (L"This equation can, finally, be solved by a substitution of the s.v.d "
	"of  #U__%x_\\'p #U__%y_ = #U #D #V\\'p  into (4). This results in")
FORMULA (L"(5)  (#D^^2^ - \\la #I) #V\\'p #D__%y_ #V__%y_\\'p #y = #0")
NORMAL (L"In an analogous way we can reduce eigenequation (2) to:")
FORMULA (L"(6)  (#D^^2^ - \\la #I) #U\\'p #D__%x_ #V__%x_\\'p #x = #0")
NORMAL (L"From (5) and (6) we deduce that the eigenvalues in both equations "
	"are equal to the squared singular values of the product matrix "
	"#U__%x_\\'p#U__%y_. "
	"These singular values are also called %%canonical "
	"correlation coefficients%. The eigenvectors #y and #x can be obtained "
	"from the columns of the following matrices #Y and #X:")
FORMULA (L"#Y = #V__%y_ #D__%y_^^-1^ #V")
FORMULA (L"#X = #V__%x_ #D__%x_^^-1^ #U")
NORMAL (L"For example, when the vector #y equals the first column of #Y and "
	"the vector #x equals "
	"the first column of #X, then the vectors #u = #T__%y_#y and #v = #T__%x_#x "
	"are the linear combinations from #T__%y_ and #T__%x_ that have maximum "
	"correlation. Their correlation coefficient equals the first canonical "
	"correlation coefficient.")
MAN_END

MAN_BEGIN (L"TableOfReal: To TableOfReal (means by row labels)...", L"djmw", 20130410)
INTRO (L"A command that appears in the ##Multivariate statistics# menu if you select a @@TableOfReal@. "
	"It calculates the multivariate means for the different row labels from the selected TableOfReal.")
ENTRY (L"Setting")
TAG (L"##Expand")
DEFINITION (L"when %off, then for a table with %n rows and %m different labels (%m\\<_%n), the resulting table will have %m rows. "
	"When %on, the dimensions of the resulting table will be the same as the originating, and corresponding means substituded "
	"in each row.")
ENTRY (L"Example")
NORMAL (L"The following commands")
CODE (L"do (\"@@Create TableOfReal (Pols 1973)...@\", \"no\")")
CODE (L"do (\"To TableOfReal (means by row labels)...\", 0)")
NORMAL (L"will result in a new TableOfReal that has 12 rows. Each row will contain the mean F1, F2 and F3 values for a particular vowel. These means "
	" were obtained from 50 representations of that vowel.")
NORMAL (L"If we had chosen the %expansion:")
CODE (L"do (\"To TableOfReal (means by row labels)...\", \"yes\")")
NORMAL (L"the resulting TableOfReal would have had 600 rows. This representation  comes in handy when, for example, you have to calculate deviations from the mean.")
MAN_END

MAN_BEGIN (L"TextGrid: Extend time...", L"djmw", 20020702)
INTRO (L"Extends the domain of the selected @TextGrid object.")
ENTRY (L"Settings")
TAG (L"##Extend domain by")
DEFINITION (L"defines the amount of time by which the domain will be extended.")
TAG (L"##At")
DEFINITION (L"defines whether starting times or finishing times will be "
	"modified.")
ENTRY (L"Behaviour")
NORMAL (L"We add an extra (empty) interval into each %%interval tier%. "
	"This is necessary to keep original intervals intact. According to the "
	"value of the second argument, the new interval will be added at the "
	"beginning or at the end of the tier.")
NORMAL (L"For %%point tiers% only the domain will be changed.")
MAN_END

MAN_BEGIN (L"TIMIT acoustic-phonetic speech corpus", L"djmw", 19970320)
INTRO (L"A large American-English speech corpus that resulted from the joint efforts "
	"of several American research sites.")
NORMAL (L"The TIMIT corpus contains a total of 6300 sentences, 10 sentences spoken by "
	"630 speakers selected from 8 major dialect regions of the USA. 70\\%  of "
	"the speakers are male, 30\\%  are female.")
NORMAL (L"The text corpus design was done by the Massachusetts Institute of "
	"Technology (MIT), Stanford Research Institute and Texas Instruments (TI). "
	"The speech was recorded at TI, transcribed at MIT, and has been maintained, "
	"verified and prepared for CDROM production by the American National Institute "
	"of Standards and Technology (NIST) (@@Lamel et al. (1986)@).")
MAN_END

MAN_BEGIN (L"VowelEditor", L"djmw", 20111124)
INTRO (L"An Editor for generating vowel-like @@sound|Sound@s from mouse movements.")
ENTRY (L"How to get a sound")
NORMAL (L"With the mouse button down, you can move the mouse cursor around in the plane "
	"spanned by the first two formants. While you move the cursor around, the positions you trace will be "
	"indicated by  blue dots. After you release the mouse button, the color of the trajectory will change "
	"to black and you will hear the vowel-like sound whose "
	"first two formants follow this trajectory. The small bars on the trajectory are time markers. With "
	"default settings time markers are at 50 milliseconds apart and they may give you an indication of the speed you traversed the trajectory.")
ENTRY (L"The interface")
NORMAL (L"In the lower part of the editor a number of buttons and fields are displayed.")
TAG (L"##Play")
DEFINITION (L"will play the trajectory.")
TAG (L"##Reverse")
DEFINITION (L"will reverse the trajectory and play it.")
TAG (L"##Publish")
DEFINITION (L"will publish the sound in the list of objects.")
TAG (L"##Duration (s)")
DEFINITION (L"allows to modify the duration of the current trajectory. ")
TAG (L"##Extend (s)")
DEFINITION (L"determines the duration of the straight line trajectory that connects the endpoint of the current trajectory with the startpoint of a new trajectory. You may extend the current trajectory by starting a new trajectory with the shift button pressed. After you finished the new trajectory, three trajectories will be appended: the current one, the straight line one and the new one.")
TAG (L"##Start F0 (Hz)")
DEFINITION (L"determines the fundamental frequency at the start of the trajectory.")
TAG (L"##F0 slope (oct/s)")
DEFINITION (L"determines how many octaves the pitch will changes during the course of the trajectory.")
NORMAL (L"The bottom line in the Editor displays the first and second formant frequency and the fundamental frequency at the start point and the endpoint of the trajectory.")
ENTRY (L"Edit menu")
TAG (L"##Set F0...")
DEFINITION (L"Set pitch and slope.")
TAG (L"##Set F3 & F4...")
DEFINITION (L"Set the frequencies and bandwidths for the third and fourth formant.")
TAG (L"##Reverse trajectory")
DEFINITION (L"Reverses the trajectory (like editor button).")   // ??
TAG (L"##Modify trajectory duration...")
DEFINITION (L"Modifies trajectory duration (like editor field).")   // ??
TAG (L"##New trajectory...")
DEFINITION (L"Set startpoint, endpoint and duration of a new trajectory.")
TAG (L"##Extend trajectory...")
DEFINITION (L"Extend current trajectory to...")
TAG (L"##Shift trajectory...")
DEFINITION (L"Shift current trajectory.")
ENTRY (L"View menu")
TAG (L"##F1 & F2 range...#")
DEFINITION (L"Modify the horizontal and vertical scales.")
TAG (L"##Show vowel marks from fixed set...#")
DEFINITION (L"Show the vowel marks in the editor from a fixed set of vowel inventories.")
TAG (L"##Show vowel marks from Table file...#")
DEFINITION (L"Put your own marks in the editor. The Table needs to have at least three mandatory columns "
	"labeled \"Vowel\", \"F1\" and  \"F2\" and "
	"one optional column labeled \"Size\". The Vowel column contains the vowel marker labels, the F1 and "
	"F2 columns have the first and second formant frequencies in Hertz. The optional Size column contains "
	"the font size of the vowel markers.")
TAG (L"##Show trajectory time markers every...")
DEFINITION (L"Shows time markers as small bars orthogonal to the trajectory. ")
ENTRY (L"Publishing")
TAG (L"##Publish Sound")
TAG (L"##Extract FormantTier")
TAG (L"##Extract PitchTier")
DEFINITION (L"Publish the Sound, the PitchTier and the FormantTier from the trajectory.")
TAG (L"##Draw trajectory...")
DEFINITION (L"Draws the trajectory in the picture window")
MAN_END

MAN_BEGIN (L"VowelEditor: Show vowel marks from Table file...", L"djmw", 20111124)
INTRO (L"A command in the @@VowelEditor@ that lets you set your own vowel marks. ")
ENTRY (L"Layout of the Table")
NORMAL (L"The Table needs at least three mandatory columns labeled \"Vowel\", \"F1\" and  \"F2\" and "
	"one optional column labeled \"Size\". The Vowel column contains the vowel marker labels, the F1 and "
	"F2 columns have the first and second formant frequencies in Hertz. The optional Size column contains "
	"the font size of the vowel markers.")
MAN_END

/********************** GSL ********************************************/
MAN_BEGIN (L"incompleteBeta", L"djmw", 20071024)
TAG (L"##incompleteBeta (%a, %b, %x)")
DEFINITION (L"I__x_(%a,%b) = 1/beta(%a,%b)\\in__0_^%x %t^^%a-1^(1-%t)^^%b-1^ dt,")
NORMAL (L"for 0 \\<_ %x \\<_ 1 and %a and %b and %a+%b not equal to a negative integer.")
//double incompleteBeta (double a, double b, double x);
//Pre: 0<= x <= 1; a> 0, b>0
//Def: $I_x(a,b)=B_x(a,b)/B(a,b)=1/B(a,b) \int_0^x t^{a-1}(1-t)^{b-1)dt$
//Limiting values: $I_0(a,b)=0 I_1(a,b)=1$
//Symmetry: $I_x(a,b) = 1 - I_{1-x}(b,a)$
MAN_END

MAN_BEGIN (L"incompleteGammaP", L"djmw", 20071024)
TAG (L"##incompleteGammaP (%a, %x)")
DEFINITION (L"incompleteGammaP = 1/\\Ga(%a)\\in__0_^%x e^^-%t^%t^^%a-1^ dt,")
NORMAL (L"For %x\\>_ 0 and %a not a negative integer.")
MAN_END

MAN_BEGIN (L"lnBeta", L"djmw", 20071024)
TAG (L"##lnBeta (%a, %b)")
DEFINITION (L"Computes the logarithm of the #beta function, subject to %a and %b and %a+%b not being negative integers.")
MAN_END

/********************* References **************************************/

MAN_BEGIN (L"Bai & Demmel (1993)", L"djmw", 19981007)
NORMAL (L"Z. Bai & J. Demmel (1993): \"Computing the generalized singular value "
	"decomposition.\" %%SIAM J. Sci. Comput.% #14: 1464\\--1486.")
MAN_END

MAN_BEGIN (L"Bartlett (1954)", L"djmw", 20011111)
NORMAL (L"M.S. Bartlett (1954): \"A note on multiplying factors for various "
	"chi-squared approximations.\", %%Joural of the Royal Statistical Society, "
	"Series B% #16: 296\\--298")
MAN_END

MAN_BEGIN (L"Boll (1979)", L"djmw", 20121021)
NORMAL (L"S.F. Boll (1979): \"Suppression of acoustic noise in speech using spectral subtraction.\""
	"%%IEEE Transactions on ASSP% #27: 113\\--120.")
MAN_END

MAN_BEGIN (L"Boomsma (1977)", L"djmw", 20020524)
NORMAL (L"A. Boomsma (1977): \"Comparing approximations of confidence intervals "
	"for the product-moment correlation coefficient.\" %%Statistica Neerlandica% "
	"#31: 179-186.")
MAN_END

MAN_BEGIN (L"Cooley & Lohnes (1971)", L"djmw", 20060322)
NORMAL (L"W.W. Colley & P.R. Lohnes (1971): %%Multivariate data analysis%. "
	"John Wiley & Sons.")
MAN_END

MAN_BEGIN (L"Davis & Mermelstein (1980)", L"djmw", 20010419)
NORMAL (L"S.B. Davis & P. Mermelstein (1980), \"Comparison of parametric "
	"representations for monosyllabic word recognition in continuously "
	"spoken sentences.\" "
	"%%IEEE Transactions on ASSP% #28: 357\\--366.")
MAN_END

MAN_BEGIN (L"Efron & Tibshirani (1993)", L"djmw", 20031103)
NORMAL (L"B. Efron & R.J. Tibshirani (1993): %%An introduction "
	"to the bootstrap%. Chapman & Hall.")
MAN_END

MAN_BEGIN (L"Espeak", L"djmw", 20111217)
NORMAL (L"Jonathan Duddington's Espeak speech synthesizer, available via http://espeak.sourceforge.net/")
MAN_END

MAN_BEGIN (L"Flanagan (1960)", L"djmw", 19980713)
NORMAL (L"J.L. Flanagan (1960): \"Models for approximating basilar membrane "
	"displacement.\" %%Bell System Technical Journal% #39: 1163\\--1191.")
MAN_END

MAN_BEGIN (L"Friedl (1997)", L"djmw", 20010710)
NORMAL (L"J.E.F. Friedl (1997): %%Mastering Regular Expressions%. "
	"O'Reilly & Associates.")
MAN_END

MAN_BEGIN (L"Ganong (1980)", L"djmw", 20130622)
NORMAL (L"W.F. Ganong III (1980): \"Phonetic categorization in auditory word perception.\" %%Journal of Experimental Psychology: Human Perception and Performance% #6: 110\\--125.") 
MAN_END

MAN_BEGIN (L"Greiner & Hormann (1998)", L"djmw", 20110617)
NORMAL (L"G. Greiner & K. Hormann (1998): \"Efficient clipping of arbitrary polygons.\" %%ACM Transactions on Graphics% #17: 71\\--83.")
MAN_END

MAN_BEGIN (L"Heath et al. (1986)", L"djmw", 19981007)
NORMAL (L"M.T. Heath, J.A. Laub, C.C. Paige & R.C. Ward (1986): \"Computing the "
	"singular value decomposition of a product of two matrices.\" "
	"%%SIAM J. Sci. Statist. Comput.% #7: 1147\\--1159.")
MAN_END

MAN_BEGIN (L"Hermes (1988)", L"djmw", 19980123)
NORMAL (L"D.J. Hermes (1988): \"Measurement of pitch by subharmonic "
	"summation.\" %%Journal of the Acoustical Society of America% #83: 257\\--264.")
MAN_END

MAN_BEGIN (L"Henze & Wagner (1997)", L"djmw", 20090630)
NORMAL (L"N. Henze & T. Wagner (1997): \"A new npproach to the BHEP Tests for Multivariate Normality.\" "
	"%%Journal of Multivariate Analysis% #62: 1\\--23.")
MAN_END

MAN_BEGIN (L"Hormann & Agathos (2001)", L"djmw", 20110617)
NORMAL (L"K. Hormann & A. Agathos (2001): \"The point in polygon problem for arbitrary polygons.\" "
	"%%Computational Geometry% #20: 131\\--144.")
MAN_END

MAN_BEGIN (L"Irino & Patterson (1997)", L"djmw", 20100517)
NORMAL (L"T. Irino & R.D. Patterson (1997): \"A time-domain, level-dependent "
	"auditory filter: The gammachirp.\" %%Journal of the Acoustical Society of America% #101: 412\\--419.")
MAN_END

MAN_BEGIN (L"Johannesma (1972)", L"djmw", 19980123)
NORMAL (L"P.I.M. Johannesma (1972): \"The pre-response stimulus ensemble of "
	"neurons in the cochlear nucleus.\" In %%Symposium on Hearing Theory% "
	"(IPO, Eindhoven, Holland), 58\\--69.")
MAN_END

MAN_BEGIN (L"Johnson (1998)", L"djmw", 20000525)
NORMAL (L"D.E. Johnson (1998): %%Applied Multivariate methods%.")
MAN_END

MAN_BEGIN (L"Keating & Esposito (2006)", L"djmw", 20130620)
NORMAL (L"P.A. Keating & C. Esposito (2006): \"Linguistic voice quality.\" %%UCLA Working Papers in Phonetics% #105: 85\\--91.")
MAN_END

MAN_BEGIN (L"Khuri (1998)", L"djmw", 20120702)
NORMAL (L"A. Khuri (1998): \"Unweighted sums of squares in unbalanced analysis of variance.\", %%Journal of Statistical Planning "
	"and Inference% #74: 135\\--147.")
MAN_END

MAN_BEGIN (L"Kim & Kim (2006)", L"djmw", 20110617)
NORMAL (L"D.H. Kim & M.-J. Kim (2006): \"An extension of polygon clipping to resolve degenerate cases.\" %%Computer-Aided Design & Applications% #3: 447\\--456.")
MAN_END

MAN_BEGIN (L"Krishnamoorthy & Yu (2004)", L"djmw", 20090813)
NORMAL (L"K. Krishnamoortht & J. Yu (2004): \"Modified Nel and Van der Merwe test for multivariate "
	"Behrens-Fisher problem.\" %%Statistics & Probability Letters% #66: 161\\--169.")
MAN_END

MAN_BEGIN (L"Lamel et al. (1986)", L"djmw", 19980123)
NORMAL (L"L.F. Lamel, R.H. Kassel & S. Sennef (1986): \"Speech Database "
	"Development: Design and Analysis of the Acoustic-Phonetic Corpus.\" "
	"%%Proc. DARPA Speech Recognition Workshop%, Report No. SAIC-86/1546, "
	"100\\--119.")
MAN_END

MAN_BEGIN (L"Morrison (1990)", L"djmw", 19980123)
NORMAL (L"D.F. Morrison (1990): %%Multivariate Statistical Methods%. "
	"New York: McGraw-Hill.")
MAN_END

MAN_BEGIN (L"Peterson & Barney (1952)", L"djmw", 20020620)
NORMAL (L"G.E. Peterson & H.L. Barney (1952): \"Control methods used in a study "
	"of the vowels.\" %%Journal of the Acoustical Society of America% #24: 175\\--184")
MAN_END

MAN_BEGIN (L"Pols et al. (1973)", L"djmw", 19990426)
NORMAL (L"L.C.W. Pols, H.R.C. Tromp & R. Plomp (1973): "
	"\"Frequency analysis of Dutch vowels from 50 male speakers.\" "
	"%%Journal of the Acoustical Society of America% #53: 1093\\--1101.")
MAN_END

MAN_BEGIN (L"Press et al. (1992)", L"djmw", 19980114)
NORMAL (L"W.H. Press, S.A. Teukolsky, W.T. Vetterling & B.P. Flannery (1992): "
	"%%Numerical recipes in C: The art of scientific computing%. "
	"Second Edition. Cambridge University Press.")
MAN_END

MAN_BEGIN (L"Sakoe & Chiba (1978)", L"djmw", 20050302)
NORMAL (L"H. Sakoe & S. Chiba (1978): \"Dynamic programming algorithm optimization for spoken word recognition.\" "
	"%%Transactions on ASSP% #26: 43\\--49.")
MAN_END

MAN_BEGIN (L"Schott (2001)", L"djmw", 20090629)
NORMAL (L"J. R. Schott (2001): \"Some tests for the equality of covariance matrices.\" "
	"%%Journal of Statistical Planning and Inference% #94: 25\\-–36.")
MAN_END

MAN_BEGIN (L"Shepard (1964)", L"djmw", 19980114)
NORMAL (L"R.N. Shepard (1964): \"Circularity in judgments of relative pitch.\" "
	"%%Journal of the Acoustical Society of America% #36: 2346\\--2353.")
MAN_END

MAN_BEGIN (L"Slaney (1993)", L"djmw", 19980712)
NORMAL (L"M. Slaney (1993): \"An efficient implementation of the "
	"Patterson-Holdsworth auditory filterbank.\" "
	"%%Apple Computer Technical Report% #35, 41 pages.")
MAN_END

MAN_BEGIN (L"Tribolet et al. (1979)", L"djmw", 20010114)
NORMAL (L"J.M. Tribolet & T.F. Quatieri (1979): \"Computation of the Complex "
	"Cepstrum.\" In %%Programs for Digital Signal Processing%, "
	"Digital Signal Processing Committee (eds.), IEEE Press.")
MAN_END

MAN_BEGIN (L"Tukey (1977)", L"djmw", 20000524)
NORMAL (L"J.W. Tukey (1977): %%Exploratory data analysis%. Reading, MA: Addison-Wesley.")
MAN_END

MAN_BEGIN (L"Van Nierop et al. (1973)", L"djmw", 20020620)
NORMAL (L"D.J.P.J. Van Nierop, L.C.W. Pols & R. Plomp (1973): \"Frequency "
	"analysis of Dutch vowels from 25 female speakers.\" %%Acustica% #29: 110\\--118")
MAN_END

MAN_BEGIN (L"Weenink (1985)", L"djmw", 20111010)
NORMAL (L"D.J.M. Weenink (1985), \"Formant analysis of Dutch vowels from 10 children\", "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #9, 45\\--52.")
MAN_END

MAN_BEGIN (L"Watrous (1991)", L"djmw", 20080125)
NORMAL (L"R.L. Watrous (1991): \"Current status of Peterson-Barney vowel formant data.\" "
	"%%Journal of the Acoustical Society of America% #89: 2459\\--2460.")
MAN_END

MAN_BEGIN (L"Weenink (1999)", L"djmw", 20041217)
NORMAL (L"D.J.M. Weenink (1999): \"Accurate algorithms for performing "
 		"principal component analysis and discriminant analysis.\" "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #23: 77\\--89.")
MAN_END

MAN_BEGIN (L"Weenink (2003)", L"djmw", 20040225)
NORMAL (L"D.J.M. Weenink (2003): \"Canonical correlation analysis.\" "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #25: 81\\--99.")
MAN_END

}

/* End of file manual_dwtools.cpp */
