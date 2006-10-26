/* manual_dwtools.c
 * 
 * Copyright (C) 1993-2006 David Weenink
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
 djmw 20061025 Latest modification
*/

#include "ManPagesM.h"
#include "TableOfReal_extensions.h"
#ifndef _Configuration_h_
	#include "Configuration.h"
#endif
#ifndef _Discriminant_h_
	#include "Discriminant.h"
#endif

static TableOfReal getStandardizedLogFrequencyPolsData (int includeLevels)
{
	TableOfReal me = TableOfReal_createFromPolsData_50males (includeLevels);
	long i, j;
	if (! me) return NULL;
	for (i=1; i <= my numberOfRows; i++)
	{
		for (j=1; j <= 3; j++) my data[i][j] = log10 (my data[i][j]);
	}
	TableOfReal_standardizeColumns (me);
	TableOfReal_setColumnLabel (me, 1, "standardized log (%F__1_)");
	TableOfReal_setColumnLabel (me, 2, "standardized log (%F__2_)");
	TableOfReal_setColumnLabel (me, 3, "standardized log (%F__3_)");
	if (includeLevels)
	{
		TableOfReal_setColumnLabel (me, 4, "standardized %L__1_");
		TableOfReal_setColumnLabel (me, 5, "standardized %L__1_");
		TableOfReal_setColumnLabel (me, 6, "standardized %L__3_");
	}
	return me;
}

static void drawPolsF1F2_log (Graphics g)
{
	TableOfReal me = getStandardizedLogFrequencyPolsData (0);
	if (me != NULL)
	{	
		Graphics_setWindow (g, -2.9, 2.9, -2.9, 2.9);
		TableOfReal_drawScatterPlot (me, g, 1, 2, 0, 0, -2.9, 2.9,
			-2.9, 2.9, 10, 1, "+", 1);
	}
	forget (me);
	Melder_clearError();	
}

static void drawPolsF1F2ConcentrationEllipses (Graphics g)
{
	TableOfReal me = getStandardizedLogFrequencyPolsData (0);
	if (me != NULL)
	{
		Discriminant d = TableOfReal_to_Discriminant (me);
		if (d != NULL)
		{
			Discriminant_drawConcentrationEllipses (d, g, 1, 0, NULL, 0, 1, 2, 
			-2.9, 2.9, -2.9, 2.9, 12, 1);
			forget (d);
		}
		forget (me);
	} 
	Melder_clearError();	
}

static void drawPolsDiscriminantConfiguration (Graphics g)
{
	TableOfReal me = getStandardizedLogFrequencyPolsData (0); 
	
	if (me != NULL)
	{
		Discriminant d = TableOfReal_to_Discriminant (me);
		if (d != NULL)
		{
			Configuration c = Discriminant_and_TableOfReal_to_Configuration 
				(d, me, 2);
			if (c != NULL)
			{
				Configuration_draw (c, g, 1, 2, -2.9, 2.9, -2.9, 2.9, 0, 1, "", 1);
				forget (c);
			}
			forget (d);
		}
		forget (me);
	}
	Melder_clearError();
}

static void drawBoxPlot (Graphics g)
{
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
	Graphics_text (g, x, y, "*");
	Graphics_arrow (g, xar, y, xal1, y);
	Graphics_text (g, xtl, y, "outlier > %upperInnerFence");
	y = q75 + 3.5 * hspread;
	Graphics_text (g, x, y, "o");
	Graphics_arrow (g, xar, y, xal1, y);
	Graphics_text (g, xtl, y, "outlier > %upperOuterFence");
	y = upperOuterFence;
	Graphics_setLineType (g, Graphics_DOTTED);	
	Graphics_line (g, -xtl, y, xtl, y);
	Graphics_text (g, xtl, y, "%upperOuterFence");
	y = upperInnerFence;	
	Graphics_line (g, -xtl, y, xtl, y);
	Graphics_text (g, xtl, y, "%upperInnerFence");
	Graphics_line (g, x - w, mean, x + w, mean);
	Graphics_setLineType (g, Graphics_DRAWN);
	y = upperWhisker;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, "%upperWhisker");
	y = lowerWhisker;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, "%lowerWhisker");

	y = q75;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, "%q75");
	y = q25;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, "%q25");
	y = q50;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, "%q50");		
	y = mean;
	Graphics_arrow (g, xar, y, xal2, y);
	Graphics_text (g, xtl, y, "%mean");		
	Graphics_unsetInner (g);
}

static void drawPartionedMatrix (Graphics g)
{
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
	Graphics_text (g, x1, y1, "#S__yy_");
	x1 = 3 + 7/2;
	Graphics_text (g, x1, y1, "#S__yx_");
	y1 = 7/2;
	Graphics_text (g, x1, y1, "#S__xx_");
	x1 = 1.5;
	Graphics_text (g, x1, y1, "#S__xy_");
}

void manual_dwtools_init (ManPages me);
void manual_dwtools_init (ManPages me)
{

MAN_BEGIN ("AffineTransform", "djmw", 20010927)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An affine transform is a combination of a linear transformation #%A "
	"and a translation #%t that transforms a vector #%x to a new vector #%y "
	"in the following way:")
FORMULA ("#%y = #%A x + #%t")
MAN_END

MAN_BEGIN ("AffineTransform: Invert", "djmw", 20011008)
INTRO ("Get the inverse of the selected @AffineTransform object.")
NORMAL ("The inverse from")
FORMULA ("#%y = #%A x + #%t")
NORMAL ("is:")
FORMULA ("#%x = #%A^^-1^ - #%A^^-1^#%t.")
MAN_END

MAN_BEGIN ("Band filtering in the frequency domain", "djmw", 20010404)
INTRO ("We describe how band filtering in the frequency domain is performed.")
NORMAL ("We start with a @Sound and end with a filter bank representation of "
	"this sound. We assume a standard analysis context: a sound divided into "
	"frames according to a certain %%window length% and %%time step%. We will "
	"simulate a filterbank with %N filters.")
NORMAL ("The algorithm for each sound frame proceeds in the following way:")
LIST_ITEM ("1. Apply a Gaussian window to the sound frame.")
LIST_ITEM ("2. Convert the windowed frame into a @Spectrum object.")
LIST_ITEM ("3. Convert the spectral amplitudes to %energy values by squaring "
	"the real and imaginary parts and multiplying by %df, the frequency "
	"distance between two successive frequency points in the spectrum. "
	"Since the Spectrum object only contains positive frequencies, "
	"we have to multiply all energy values, except the first and the last "
	"frequency, by another factor of 2 to compensate for negative frequencies.")
LIST_ITEM ("4. For each of the %N filters in the filter bank: determine the "
	"inner product of its filter function with the energies as determined in "
	"the previous step. The result of each inner product is the energy in the "
	"corresponding filter.")
LIST_ITEM ("5. Convert the energies in each filter to power by dividing by "
	"the %%window length%.")
LIST_ITEM ("6. Correct the power, due to the windowing of the frame, by dividing "
	"by the integral of the %squared windowing function.")
LIST_ITEM ("7. Convert all power values to %dB's according to 10 * log10 "
	"(%power / 4 10^^-10^).")
MAN_END

MAN_BEGIN ("Bonferroni correction", "djmw", 20011107)
NORMAL ("In general, if we have %k independent significance tests "
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
NORMAL ("This results in the following relation between the overall and the "
	"individual significance level:") 
FORMULA ("(1 - %\\al\\'p)^^%k%^ = 1 - %\\al.")
NORMAL ("This equation can easily be solved for %\\al\\'p:")
FORMULA ("%\\al\\'p = 1 - (1-%\\al)^^1/%k^,")
NORMAL ("which for small %\\al reduces to:")
FORMULA ("%\\al\\'p = %\\al / %k")
NORMAL ("This is a very simple recipe: If you want an overall significance "
	"level %\\al and you perform %k individual tests, simply divide %\\al "
	"by %k to obtain the significance level for the individual tests.") 
MAN_END

MAN_BEGIN ("box plot", "djmw", 20000524)
INTRO ("A box plot provides a simple graphical summary of data. These plots "
	"originate from the work of @@Tukey (1977)@.")
ENTRY ("Definitions")
NORMAL ("The following figure shows an annotated box plot.")
PICTURE (5.0, 5.0, drawBoxPlot)
NORMAL ("To understand the box plot we need the following definitions:")
LIST_ITEM ("%q25 = lower quartile, 25\\% of the data lie below this value")
LIST_ITEM ("%q50 = median, 50\\% of the data lie below this value")
LIST_ITEM ("%q75 = upper quartile, 25\\% of the data lie above this value")
NORMAL ("The following definitions all depend on these quantiles:")
LIST_ITEM ("%hspread = |%q75 \\-- %q25| (50\\% interval)")
LIST_ITEM ("%lowerOuterFence = %q25 \\-- 3.0 * %hspread (not in figure)")
LIST_ITEM ("%lowerInnerFence = %q25 \\-- 1.5 * %hspread (not in figure)")
LIST_ITEM ("%upperInnerFence = %q75 + 1.5 * %hspread")
LIST_ITEM ("%upperOuterFence = %q75 + 3.0 * %hspread")
LIST_ITEM ("%lowerWhisker = smallest data value larger then %lowerInnerFence")
LIST_ITEM ("%upperWhisker = largest data value smaller then %upperInnerFence")
NORMAL ("The box plot is a summary of the data in which:")
LIST_ITEM ("\\bu the horizontal lines of the rectangle correspond to "
	"%q25, %q50 and %q75, respectively.")
LIST_ITEM ("\\bu the dotted line corresponds to the mean.")
LIST_ITEM ("\\bu the outliers outside the %outerFences are drawn with an 'o'.")
LIST_ITEM ("\\bu the outliers in the intervals (%lowerOuterFence, %lowerInnerFence) "
	"and (%upperInnerFence, %upperOuterFence) are drawn with an '*'.")
LIST_ITEM ("\\bu the whisker lines outside the rectangle connect %q25 with %lowerWhisker, and, "
	"%q75 with %upperWhisker, respectively. With no outliers present, the "
	"whiskers mark minimum and/or maximum of the data.")
MAN_END


MAN_BEGIN ("BarkFilter", "djmw", 20010404)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type BarkFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%z, %t), expressed "
	"in dB's. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %z__%j_ (on a Bark scale).")
ENTRY ("Inside a BarkFilter")
NORMAL ("With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN ("Bootstrap", "djmw", 20031103)
INTRO ("The bootstrap data points are a random sample of size %n "
	"drawn %%with% replacement from the sample (%x__1_,...%x__n_). This "
	"means that the bootstrap data set consists of members of the original "
	"data set, some appearing zero times, some appearing once, some appearing "
	"twice, etc.")
NORMAL ("More information can be found in @@Efron & Tibshirani (1993)@.")
MAN_END

MAN_BEGIN ("canonical variate", "djmw", 20060328)
NORMAL ("A ##canonical variate# is a new variable (variate) formed by making a linear combination of two "
	"or more variates (variables) from a data set. "
	"A linear combination of variables is the same as a weighted sum of variables. "
	"Because we can in infinitely many ways choose combinations of weights between variables in a data set, "
	"there are also infinitely many canonical variates possible. ")
NORMAL ("In general additional constraints must be satisfied by the weights to get a meaningful canonical variate. "
	"For example, in @@Canonical correlation analysis|canonical correlation analyis@ a data set is split up into two parts, a %%dependent% and an %%independent% part. "
	"In both parts we can form a canonical variate and we choose weights that maximize the correlation between these canonical variates "
	"(there is an @@TableOfReal: To CCA...|algorithm@ that calculates these weights).")
MAN_END

MAN_BEGIN ("Categories", "djmw", 19960918)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type Categories represents an ordered collection of categories. Each "
	"category is a simple text string.")
ENTRY ("Categories commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu ##Create an empty Categories#")
LIST_ITEM ("\\bu @@FFNet & Pattern: To Categories...@")
NORMAL ("Viewing and editing:")
LIST_ITEM ("\\bu @CategoriesEditor")
NORMAL ("Analysis:")
LIST_ITEM ("\\bu @@Categories: To Confusion@")
LIST_ITEM ("\\bu @@Categories: Difference@")
NORMAL ("Synthesis")
LIST_ITEM ("\\bu @@Categories: Append@")
LIST_ITEM ("\\bu ##Categories: Permute...#")
LIST_ITEM ("\\bu ##Categories: To unique Categories#")
ENTRY ("Inside a Categories")
NORMAL ("With @Inspect you will see the following attributes:")
TAG ("%size")
DEFINITION ("the number of simple categories.")
TAG ("%item[]")
DEFINITION ("the categories. Each category is an object of type #SimpleString.")
MAN_END

MAN_BEGIN ("Categories: Difference", "djmw", 19960918)
INTRO ("A command to compute the difference between two selected @Categories objects.")
ENTRY ("Behaviour")
NORMAL ("Each element in the first object is compared with the corresponding "
	"object in the second object according to its compare method. "
	"The number of different %categories will be shown in the @@Info window@.")
MAN_END
 
MAN_BEGIN ("Categories: To Confusion", "djmw", 19960918)
INTRO ("A command to compute the @Confusion matrix from two selected "
	"@Categories objects.")
ENTRY ("Algorithm")
NORMAL ("A confusion matrix is constructed from both #Categories objects in "
	"the following way: The first Categories object is considered the stimulus "
	"Categories and its unique (sorted) categories "
	"form the row indices of the confusion matrix, the unique (sorted) "
	"categories of the second object form the column indices of this matrix.")
NORMAL ("Next, each element in the first #Categories object is compared with "
	"the corresponding object in the second object and the element in the "
	"confusion matrix addressed by this pair is incremented by 1.")
MAN_END

MAN_BEGIN ("Categories: Append", "djmw", 19960918)
INTRO ("You can choose this command after selecting 2 objects of type @Categories. "
	"A new object is created that contains the second object appended after the first.")
MAN_END

MAN_BEGIN ("Categories: Edit", "djmw", 19960918)
INTRO ("You can choose this command after selecting one #Categories. "
	"A @CategoriesEditor will appear on the screen, with the selected #Categories in it.")
MAN_END

MAN_BEGIN ("CategoriesEditor", "djmw", 19960918)
ENTRY ("An editor for manipulating @Categories.")
NORMAL ("To make a selection, use the left mouse button.")
NORMAL ("The Ctrl key extends a selection (discontinuously).")
NORMAL ("The Shift key extends a selection contiguously.")
MAN_END

MAN_BEGIN ("CC", "djmw", 20010219)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("Any object that represents cepstral coefficients as a function of "
	"time.")
MAN_END

MAN_BEGIN ("CCA", "djmw", 20020323)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. ")
NORMAL ("An object of type CCA represents the @@Canonical correlation "
	"analysis@ of two multivariate datasets.")
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@TableOfReal: To CCA...@")
MAN_END

MAN_BEGIN ("CCA: Get zero correlation probability...", "djmw", 20040407)
INTRO ("Get the probability that for the selected @CCA object the chosen "
	"canonical correlation coefficient is different from zero.")
ENTRY ("Arguments")
TAG ("%Index")
DEFINITION ("is the index of the canonical correlation coefficient that "
	"you want to test.")
ENTRY ("Algorithm")
NORMAL ("Wilks' statistic: the probability that coefficient \\ro__%index_ "
	"differs from zero is ")
FORMULA (" %probability = chiSquareQ (\\ci^2, %ndf),")
NORMAL ("where the %%number of degrees of freedom% parameter equals")
FORMULA ("%ndf = (%n__y_ - %index +1)(%n__x_ - %index +1)")
NORMAL ("and the chi-squared parameter is")
FORMULA ("\\ci^2 = \\--(%numberOfObservations - (%n__y_ + %n__x_ +3)/2) "
	"log (\\La__%index_),")
NORMAL ("In the formulas above the variables %n__y_ and %n__x_ are the "
	"dimensions of the dependent and the independent data sets whose "
	"canonical correlations have been "
	"obtained, and Wilks' lambda is:")
FORMULA ("\\La__index_ = \\Pi__%i=%index..min(%ny,%nx)_ (1 \\-- \\ro__%i_^^2^)")
MAN_END

MAN_BEGIN ("CCA & Correlation: To TableOfReal (loadings)", "djmw", 20020525)
INTRO ("Determine from the selected @CCA and @Correlation objects the correlations "
	"of the canonical variables with the original variables. These correlations are "
	"called %%canonical factor loadings%, or also %%structure correlation "
	"coefficients%.")
MAN_END

MAN_BEGIN ("CCA & Correlation: Get variance fraction...", "djmw", 20060323)
INTRO ("Determine from the selected @CCA and @Correlation objects the fraction of the variance "
	"explained by the selected @@canonical variate@ range.")
ENTRY ("Arguments")
TAG ("%%X or Y")
DEFINITION ("determines whether you select the dependent (y) or the independent (x) set.")
TAG ("%%Canonical variate range")
DEFINITION ("determines the canonical variates (or canonical variables).")
ENTRY ("Remarks")
NORMAL ("1. In general the variance fractions for a particular canonical variate in the "
	"dependent and in the independent set are not the same.")
NORMAL ("2. In general, the variance fractions for all canonical variates do not sum to 1.")
ENTRY ("Algorithm")
NORMAL ("The formula's can be found on page 170 of @@Cooley & Lohnes (1971)@.")
NORMAL ("For example, the fraction of the variance explained by the %i^^th^ canonical "
	"variable in the dependent set is:")
FORMULA ("%%fractionVariance% = ((#y__i_\\'p #R__yy_\\'p #R__yy_ #y__i_) / (#y__i_\\'p #R__yy_ #y__i_)) / %n__%y_,")
NORMAL ("where #y__%i_ is the eigenvector for dependent canonical variable %i and #R__%%yy%_ is the correlation matrix for the %n__%y_ variables in the dependent set.")
MAN_END

MAN_BEGIN ("CCA & Correlation: Get redundancy (sl)...", "djmw", 20060323)
INTRO ("Determine from the selected @CCA and @Correlation objects the Stewart-Love redundancy for the "
	"selected canonical variates.")
NORMAL ("The Stewart-Love redundancy for a single @@canonical variate@ is the fraction of variance explained by the selected "
	"canonical variate in a set times the fraction of shared variance between the corresponding canonical variates in the two sets.")
NORMAL ("The Stewart-Love redundancy for a canonical variate range is the sum of the individual redundancies.")
ENTRY ("Arguments")
TAG ("%%X or Y")
DEFINITION ("determines whether you select the dependent (y) or the independent (x) set.")
TAG ("%%Canonical variate range")
DEFINITION ("determines the canonical variates (or canonical variables).")
ENTRY ("Algorithm")
NORMAL ("The formula's can be found on page 170 of @@Cooley & Lohnes (1971)@.")
NORMAL ("For example, the redundancy of the dependent set (y) given the independent set (x) for the %i^^%%th%^ canonical "
	"variate can be expressed as:")
FORMULA ("%R__%i_(y) = %%varianceFraction%__%i_(y) * \\ro__%i_^2, ")
NORMAL ("where %%varianceFraction%__%i_(y) is the @@CCA & Correlation: Get variance fraction...|variance fraction@ explained "
	"by the %i^^%%th%^ canonical variate of the dependent set, and \\ro__%i_ is the %i^^%%th%^ canonical correlation coefficient.")
NORMAL ("The redundancy for the selected canonical variate in the dependent set shows what "
	"fraction of the variance in the %%dependent% set is already \"explained\" by "
	"the variance in the %%independent% set, i.e. this fraction could be considered as redundant.")
NORMAL ("In the same way we can measure the redundancy of the independent (x) set giving the dependent set (y).")
ENTRY ("Remark")
NORMAL ("In general %R__%i_(y) \\=/ %R__%i_(x).")
MAN_END

MAN_BEGIN ("CCA & TableOfReal: To TableOfReal (loadings)", "djmw", 20020525)
INTRO ("Determine from the selected @CCA and @TableOfReal objects the correlations "
	"of the canonical variables with the original variables. These correlations are "
	"called %%canonical factor loadings%, or also %%structure correlation "
	"coefficients%.")
MAN_END

MAN_BEGIN ("CCA & TableOfReal: To TableOfReal (scores)...", "djmw", 20040407)
INTRO ("Determines the scores on the dependent and the independent canonical "
	"variates from the selected @CCA and @TableOfReal objects.")
ENTRY ("Arguments")
TAG ("%%Number of canonical correlations%")
DEFINITION ("determines the dimension, i.e., the number of elements of the resulting "
	"canonical score vectors. The newly created table will have twice this number of "
	"columns because we have calculated score vectors for the dependent and the "
	"independent variates.")
ENTRY ("Behaviour")
NORMAL ("The scores on the dependent set are determined as #T__%y_ #Y, where "
	"#T__%y_ is the dependent part in the table and #Y is a matrix with "
	"%numberOfCanonicalCorrelations eigenvectors for the dependent variate.")
NORMAL ("The scores for the independent variates are then determined in an analogous "
	"way as #T__%x_ #X.")
NORMAL ("The scores for the dependent data will be in the lower numbered columns, "
	"the scores for the independent part will be in the higher numbered columns of "
	"the newly created object.")
MAN_END


MAN_BEGIN ("Canonical correlation analysis", "djmw", 20020509)
INTRO ("This tutorial will show you how to perform canonical correlation "
       "analysis with  P\\s{RAAT}.")
ENTRY ("1. Objective of canonical correlation analysis")
NORMAL ("In canonical correlation analysis we try to find the correlations between "
	"two data sets. One data set is called the %dependent set, the other the "
	"%independent set. In P\\s{RAAT} these two sets must reside into one "
	"@TableOfReal object. The lower numbered columns of this table will then be "
	"interpreted as the dependent part, the rest of the columns as the "
	"independent part. "
	"The dimension of, i.e., the number of columns in, the dependent part may not "
	"exceed the dimension of the independent part.")
NORMAL ("As an example, we will use the dataset from @@Pols et al. (1973)@ "
	"with the frequencies and levels of the first three formants from the 12 "
	"Dutch monophthongal vowels as spoken in /h_t/ context by 50 male speakers. "
	"We will try to find the canonical correlation between formant frequencies "
	"(the %dependent part) and levels (the %independent part). "
	"The dimension of both groups of variates is 3. "
	"In the introduction of the "
	"@@discriminant analysis@ tutorial you can find how to get these data, "
	"how to take the logarithm of the formant frequency values and how to "
	"standardize them. The following script summarizes:")
CODE ("Create TableOfReal (Pols 1973)... yes")
CODE ("Formula... if col < 4 then log10 (self) else self endif")
CODE ("Standardize columns")
NORMAL ("Before we start with the %canonical correlation analysis we will first have "
	"a look at the %Pearson correlations of this table and  "
	"calculate the @Correlation matrix. It is given by:")
CODE ("       F1     F2     F3     L1     L2     L3")
CODE ("F1   1     -0.338  0.191  0.384 -0.505 -0.014")
CODE ("F2  -0.338  1      0.190 -0.106  0.526 -0.568")
CODE ("F3   0.191  0.190  1      0.113 -0.038  0.019")
CODE ("L1   0.384 -0.106  0.113  1     -0.038  0.085")
CODE ("L2  -0.505  0.526 -0.038 -0.038  1      0.128")
CODE ("L3  -0.014 -0.568  0.019  0.085  0.128  1")
NORMAL ("The following script summarizes:")
CODE ("select TableOfReal pols_50males")
CODE ("To Correlation")
CODE ("Draw as numbers... 1 0 decimal 3")
NORMAL ("The correlation matrix shows that high correlations exist between some "
	"formant frequencies and some levels. For example, the correlation "
	"coefficient between F2 and L2 equals 0.526.")	
NORMAL ("In a canonical correlation analysis of the dataset above, we try "
	"to find the linear "
	"combination %u__1_ of %F__1_, %F__2_ and %F__3_ that correlates maximally "
	"with the linear combination %v__1_ of %L__1_, %L__2_ and %L__3_. "
	"When we have found these %u__1_ and %v__1_ we next try to find a new "
	"combination %u__2_ of the formant frequencies and a new combination "
	"%v__2_ of the levels that have maximum correlation. These %u__2_ and "
	"%v__2_ must be uncorrelated with %u__1_ and %v__1_. "
	"When we express the above with formulas we have:")
FORMULA ("%u__1_ = %y__11_%F__1_+%y__12_%F__2_ + %y__13_%F__3_")
FORMULA ("%v__1_ = %x__11_%L__1_+%x__12_%L__2_ + %x__13_%L__3_")
FORMULA ("\\ro(%u__1_, %v__1_) = maximum, \\ro(%u__2_, %v__2_) = submaximum, ")	
FORMULA ("\\ro(%u__2_, %u__1_) = \\ro (%u__2_, %v__1_) = \\ro (%v__2_, %v__1_) "
	"= \\ro (%v__2_, %u__1_) = 0,")
NORMAL ("where the \\ro(%u__i_, %v__i_) are the correlations between the "
	"@@canonical variate@s %u__i_ and %v__i_ and the %y__%ij_'s and %x__%ij_'s are"
	" the ##canonical coefficients# for the dependent and the independent "
	"variates, respectively.")
ENTRY ("2. How to perform a canonical correlation analysis")
NORMAL ("Select the TableOfReal and choose from the dynamic menu the option "
	"@@TableOfReal: To CCA...|To CCA...@. This command is available in the "
	"\"Multivariate statistics\" action button. We fill out the form and supply "
	"3 for %%Dimension of dependent variate%. The resulting CCA object will bear "
	"the same name as the TableOfReal object. The following script summarizes:")
CODE ("select TableOfReal pols_50males")
CODE ("To CCA... 3")
ENTRY ("3. How to get the canonical correlation coefficients")
NORMAL ("You can get the canonical correlation coefficients by queries of the CCA "
	"object. You will find that the three canonical correlation coefficients, "
	"\\ro(%u__1_, %v__1_), \\ro(%u__2_, %v__2_) and \\ro(%u__3_, %v__3_) are "
	" approximately 0.86, 0.53 and 0.07, respectively. "
	"The following script summarizes:")
CODE ("cc1 = Get correlation... 1")
CODE ("cc2 = Get correlation... 2")
CODE ("cc3 = Get correlation... 3")
CODE ("printline cc1 = 'cc1', cc2 = 'cc2', cc3 = 'cc3'")
ENTRY ("4. How to obtain canonical scores")
NORMAL ("Canonical #scores, also named @@canonical variate@s, are the linear combinations:")
FORMULA ("%u__%i_ = %y__%i1_%F__1_+%y__%i2_%F__2_ + %y__%i3_%F__3_, and,")
FORMULA ("%v__%i_ = %x__%i1_%L__1_+%x__%i2_%L__2_ + %x__%i3_%L__3_,")
NORMAL ("where the index %i runs from 1 to the number of correlation coefficients.")
NORMAL ("You can get the canonical scores by selecting a CCA object together with "
	"the TableOfReal object and choose "
	"@@CCA & TableOfReal: To TableOfReal (scores)...|To TableOfReal (scores)...@")
NORMAL ("When we now calculate the ##Correlation# matrix of these canonical variates we "
	"get the following table:")
CODE ("       u1     u2     u3     v1     v2     v3")
CODE ("u1     1      .      .     0.860   .      .")
CODE ("u2     .      1      .      .     0.531   .")
CODE ("u3     .      .      1      .      .     0.070")
CODE ("v1    0.860   .      .      1      .      .")
CODE ("v2     .     0.1     .      .      1      .")
CODE ("v3     .      .     0.070   .      .      1")
NORMAL ("The scores with a dot are zero to numerical precision. In this table the "
	"only correlations that differ from zero are the canonical correlations. "
	"The following script summarizes:")
CODE ("select CCA pols_50males")
CODE ("plus TableOfReal pols_50males")
CODE ("To TableOfReal (scores)... 3")
CODE ("To Correlation")
CODE ("Draw as numbers if... 1 0 decimal 2 abs(self) > 1e-14")	 
ENTRY ("5. How to predict one dataset from the other")
NORMAL ("@@CCA & TableOfReal: Predict...@")
NORMAL ("Additional information can be found in @@Weenink (2003)@.")
MAN_END

MAN_BEGIN ("CCA & TableOfReal: Predict...", "djmw", 20020503)
INTRO ("")
MAN_END

MAN_BEGIN ("Chebyshev polynomials", "djmw", 19990620)
INTRO ("The Chebyshev polynomials %T__%n_(%x) of degree %n are special orthogonal polynomial functions "
	"defined on the domain [-1, 1].")
NORMAL ("Orthogonality:")
FORMULA ("__-1_\\in^^1^ %W(%x) %T__%i_(%x) %T__%j_(%x) %dx = \\de__%ij_")
FORMULA ("%W(%x) = (1 \\-- %x^^2^)^^\\--1/2^   (-1 < x < 1)")
NORMAL ("They obey certain recurrence relations:")
FORMULA ("%T__%n_(%x) = 2 %x %T__%n-1_(%x) \\-- %T__%n-2_(%x)")
FORMULA ("%T__0_(%x) = 1")
FORMULA ("%T__1_(%x) = %x")
MAN_END

MAN_BEGIN ("ChebyshevSeries", "djmw", 19990620)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type ChebyshevSeries represents a linear combination of @@Chebyshev polynomials@ "
	"%T__%k_(%x).")
FORMULA ("ChebyshevSeries (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %T__%k_(%x)")
MAN_END

MAN_BEGIN ("ChebyshevSeries: To Polynomial", "djmw", 19990620)
INTRO ("A command to transform the selected @ChebyshevSeries object into a @@Polynomial@ object.")
NORMAL ("We find polynomial coefficients %c__%k_ such that")
FORMULA ("\\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x^^%k^ = \\Si__%k=1.."
	"%numberOfCoefficients_ %l__%k_ %T__%k_(%x)")
NORMAL ("We use the recurrence relation for @@Chebyshev polynomials@ to calculate these coefficients.")
MAN_END

MAN_BEGIN ("ClassificationTable", "djmw", 19990525)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type ClassificationTable represents the result of a classification experiment. "
	"The numbers in a row show how well a particular input matches the classes represented by the "
	"column labels. The higher the number the better the match.")
MAN_END

MAN_BEGIN ("concentration ellipse", "djmw", 20000531)
NORMAL ("The percentage of bivariate normally distributed data covered by an ellipse "
	"whose axes have a length of %numberOfSigmas\\.c\\si can be obtained by integration of the p.d.f. "
	"over an elliptical area. This results in the following equation as can be "
	"verified from equation 26.3.21 in @@Abramowitz & Stegun (1970)@:")
FORMULA ("%percentage = (1 - exp (-%numberOfSigmas^^2^/2))\\.c 100\\% ,")
NORMAL ("where the %numberOfSigmas is the radius of the \"ellipse\":")
FORMULA ("(%x/%\\si__x_)^2 + (%y/%\\si__y_)^2 = %numberOfSigmas^2.")
NORMAL ("The %numberOfSigmas = 1 ellipse covers 39.3\\% , "
	"the %numberOfSigmas = 2 ellipse covers 86.5\\%  and "
	"the %numberOfSigmas = 3 ellipse covers 98.9\\%  of the data.")
MAN_END

MAN_BEGIN ("confidence interval", "djmw", 20011105)
NORMAL ("The confidence interval gives an estimated range of values which "
	"is likely to include an unknown population parameter. "
	"The estimated range is calculated from a given set of observations.")
ENTRY ("Examples")
NORMAL ("At the \\al level of significance a two sided confidence interval "
	"for the true mean \\mu for normally distributed data with mean %%mean% and "
	"known standard deviation %\\si can be constructed as:")
FORMULA ("%%mean% - %z__\\al/2_ \\si / \\Vr%N \\<_ \\mu \\<_ "
	"%%mean% + %z__\\al/2_ \\si / \\Vr%N,")
NORMAL ("where %z__\\al/2_ = invGaussQ (\\al/2) and %N is the number of observations.")
NORMAL ("If the standard deviation is %not known, we have to estimate its value (%s) "
	"from the data and the formula above becomes:")
FORMULA ("%%mean% - %t__%%\\al/2;N%_ %s / \\Vr%N \\<_ \\mu \\<_ "
	"%%mean% + %t__%%\\al/2;N%_ %s / \\Vr%N,")
NORMAL ("where %t__%%\\al/2;N%_ = invStudentQ (%\\al/2, %N-1).")
NORMAL ("For %\\al=0.05 and %N=20 we get %z__0.025_=1.96 and %t__0.025;20_=2.039. "
	"This shows that when we have to estimate the standard deviation from the data, "
	"the confidence interval is wider than when the standard deviation is known "
	"beforehand.")
MAN_END

MAN_BEGIN ("confidence level", "djmw", 20011105)
NORMAL ("The confidence level is the probability value 1-\\al associated "
	"with a @@confidence interval@, where \\al is the level of significance. "
	"It can also be expressed as a percentage 100(1-\\al)\\%  and is than "
	"sometimes called the %%confidence coefficient%.") 
MAN_END

MAN_BEGIN ("Confusion", "djmw", 20010501)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type Confusions represents a confusion matrix, with "
	"stimuli as row labels and responses as column labels. The entry at "
	"position [%i][%j] represents the number of times response %j "
	"was given to the stimulus %i.")
ENTRY ("Creating a Confusion from data in a text file")
NORMAL ("Suppose you have two objects A and B. "
	"In one way or another, you have acquired the following  "
	"confusions: %\\de__%AA_ = 6, %\\de__%AB_ = 2 , %\\de__%BA_ = 1, "
	"and %\\de__%BB_ = 7.")
NORMAL ("You can create a simple text file like the following:")
CODE ("\"ooTextFile\"  ! to make Praat recognize your file")
CODE ("\"Confusion\" ! The line that tells Praat about the contents")
CODE ("2     \"A\"  \"B\"  ! Number of columns, and column labels")
CODE ("2                   ! Number of rows")
CODE ("\"A\"    6    2     ! Row label A, A-A value, A-B value")
CODE ("\"B\"    1    7     ! Row label B, B-A value, B-B value")
NORMAL ("This text file can be read with the @@Read from file...@ command. ")
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Categories: To Confusion")
NORMAL ("Drawing")
LIST_ITEM ("\\bu ##Draw as numbers...")
LIST_ITEM ("\\bu ##Draw as squares...")
NORMAL ("Query")
LIST_ITEM ("\\bu ##Get column mean (index)...")
LIST_ITEM ("\\bu ##Get column mean (label)...")
LIST_ITEM ("\\bu ##Get column stdev (index)...")
LIST_ITEM ("\\bu ##Get column stdev (label)...")
LIST_ITEM ("\\bu @@Confusion: Get fraction correct|Get fraction correct")
NORMAL ("Modification")
LIST_ITEM ("\\bu ##Formula...")
LIST_ITEM ("\\bu ##Remove column (index)...")
LIST_ITEM ("\\bu ##Insert column (index)...")
LIST_ITEM ("\\bu ##Set row label (index)...")
LIST_ITEM ("\\bu ##Set row label (label)...")
LIST_ITEM ("\\bu ##Set column label (index)...")
LIST_ITEM ("\\bu ##Set column label (label)...")
NORMAL ("Analysis:")
LIST_ITEM ("\\bu @@Confusion: To Similarity...@")
LIST_ITEM ("\\bu @@Confusion: To Dissimilarity (pdf)...@")
ENTRY ("Inside a Confusion")
NORMAL ("With @Inspect you will see the following attributes:")
TAG ("%numberOfRows")
DEFINITION ("the number of stimuli.")
TAG ("%numberOfColumns")
DEFINITION ("the number of responses.")
TAG ("%rowLabels")
DEFINITION ("the names of the stimuli.")
TAG ("columnLabels")
DEFINITION ("the names of the responses.")
MAN_END

MAN_BEGIN ("Confusion: Condense...", "djmw", 20040407)
INTRO ("Groups row and column labels of the selected @Confusion object in "
	"order to reduce its dimension.")
ENTRY ("Arguments")
TAG ("%Search")
DEFINITION ("the pattern to match")
TAG ("%Replace")
DEFINITION ("the pattern that replaces the match(es)")
TAG ("%%Replace limit%")
DEFINITION ("limits the maximum number of times that a match/replace cycle "
	"may occur within each label.")
TAG ("%Search and replace are")
DEFINITION ("defines whether the search and replace strings are taken "
	"litterally or as a @@Regular expressions|regular expression@.")
ENTRY ("Behaviour")
NORMAL ("First all row and column labels are changed according to the search "
	"and replace specification. Next all rows or columns that have the same "
	"labels are summed. ")
MAN_END

MAN_BEGIN ("Confusion: Get fraction correct", "djmw", 20000225)
INTRO ("A @@query@ to ask the selected @Confusion matrix for the fraction of "
	"correct classifications.")
NORMAL ("The \"fraction correct\" is defined as the quotient of the number "
	"of correct classifications and the sum of the entries in the matrix.")
NORMAL ("Correct classifications have identical row and column labels.")
MAN_END

MAN_BEGIN ("Confusion: To TableOfReal (marginals)", "djmw", 20011031)
INTRO ("A new @TableOfReal object is created from the selected @Confusion "
	"object with one extra row and column. ")
NORMAL ("The first element of the extra row will contain the sum of the "
	"confusions in the the first %column, the first element of the extra "
	"column will contain the sum of the confusions in the the first %row, "
	"etc... The bottom-right element will contain the sum of all confusions.")
MAN_END

MAN_BEGIN ("Correlation", "djmw", 19990105)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type Correlation represents the correlation coefficients "
	"of a multivariate data set.")
MAN_END

MAN_BEGIN ("Correlation: Confidence intervals...", "djmw", 20040407)
INTRO ("Calculates @@confidence interval@s for the correlation coefficients "
	"from the selected @Correlation object(s) and saves these intervals in a "
	"new @TableOfReal object.")
ENTRY ("Arguments")
TAG ("%%Confidence level%")
DEFINITION ("the @@confidence level@ you want for the confidence intervals.")
TAG ("%%Number of tests%")
DEFINITION ("determines the @@Bonferroni correction@ for the significance "
	"level. If the default value (zero) is chosen, it will be set equal "
	"to the number of correlations involved (a matrix of dimension %n "
	"has %n\\.c(%n-1)/2 correlations).")
TAG ("%%Approximation%")
DEFINITION ("defines the approximation that will be used to calculate the "
	"confidence intervals. It is either Fisher's z transformation or Ruben's "
	"transformation. According to @@Boomsma (1977)@, Ruben's approximation is "
	"more accurate than Fisher's.")
ENTRY ("Algorithm")
NORMAL ("We obtain intervals by the large-sample conservative multiple tests " 
	"with Bonferroni inequality and the Fisher or Ruben transformation. "
	"We put the upper values of the confidence intervals in the upper "
	"triangular part of the matrix and the lower values of the confidence "
	"intervals in lower triangular part of the resulting TableOfReal object.")
NORMAL ("In %%Fisher's approximation%, for each element %r__%ij_ of the "
	"correlation matrix the confidence interval is:")
FORMULA ("#[ tanh (%z__%ij_ - %z__%\\al\\'p_ / \\Vr(%N - 3)) , "
	"tanh (%z__%ij_ + %z__%\\al\\'p_ / \\Vr(%N - 3)) #],")
NORMAL ("where %z__%ij_ is the Fisher z-transform of the correlation %r__%ij_:")
FORMULA ("%z__%ij_ = 1/2 ln ((1 + %r__%ij_) / (1 - %r__%ij_)), ")
NORMAL ("%z__%\\al\\'p_ the Bonferroni corrected %z-value "
	"%z__%\\al/(2\\.c%numberOfTests)_, ")
FORMULA ("%\\al = 1 - %confidenceLevel,")
NORMAL ("and %N the number of observations that the correlation matrix is "
	"based on.")
NORMAL ("In %%Ruben's approximation% the confidence interval for element %r__%ij_ "
	"is:")
FORMULA ("#[ %x__1_ / \\Vr(1 - %x__1_^2), %x__2_ / \\Vr(1 - %x__2_^2) #]")
NORMAL ("in which %x__1_ and %x__2_ are the smallest and the largest root from")
FORMULA ("%a %x^^2^ + %b %x + %c = 0, with")
FORMULA ("%a = 2%N - 3 - %z__%\\al\\'p_^^2^")
FORMULA ("%b = - 2 %r\\'p \\Vr((2%N - 3)(2%N - 5))")
FORMULA ("%c = (2%N - 5 - %z__%\\al\\'p_^^2^) %r\\'p^^2^ - 2%z__%\\al\\'p_^^2^, and") 
FORMULA ("%r\\'p = %r__%ij_ / \\Vr(1 - %r__%ij_^2),")
MAN_END

MAN_BEGIN ("Covariance", "djmw", 19990105)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type Covariance represents the sums of squares and cross "
	"products of a multivariate data set divided by the number of observations.")
NORMAL ("An object of type Covariance contains the same attributes as an object of "
	"type @SSCP.")
NORMAL ("Since an object of type Covariance contains the mean values (the "
	"centroids), the covariances as well as the number of observations it has "
	"all the information necessary to be the subject of all kinds of statistical "
	"tests on means and variances.")
MAN_END

MAN_BEGIN ("Covariance: Difference", "djmw", 19981222)
INTRO ("You can choose this command after selecting two objects of type @Covariance. ")
NORMAL ("We test the hypothesis that the samples that gave rise to the two "
	"covariance matrices #%M__1_ and #%M__2_, were drawn from the same "
	"distribution. The test statistic is %L\\'p which is distributed as "
	"a \\ci^2 variate with %p(%p+1)/2 degrees of freedom.")
FORMULA ("%L\\'p = %L \\.c (1 \\-- (2%p + 1 \\-- 2 / (%p + 1)) / (6 \\.c ( %N \\-- 1))),")
NORMAL ("where, ")
FORMULA ("%L = (%N \\-- 1) \\.c (ln determinant (#%M__1_) \\-- ln determinant "
	"(#%M__2_)) + trace (#%M__2_ \\.c #%M__1_^^\\--1^) \\-- %p), ")
NORMAL ("%p is dimension of covariance matrix and "
	"%N is the number of observations underlying the covariance matrix. ")
NORMAL ("For more details on this test, see e.g. page 292 of @@Morrison (1990)@.")
MAN_END

MAN_BEGIN ("Covariance: Get significance of one mean...", "djmw", 20040407)
INTRO ("Gets the level of significance for one mean from the selected "
	"@Covariance object being different from a hypothesized mean.")
ENTRY ("Arguments")
TAG ("%Index")
DEFINITION ("the position of the element in the means vector (centroid) that "
	"you want to test. ")
TAG ("%%Value")
DEFINITION ("the hypothesized mean %\\mu (see below).")
ENTRY ("Behaviour")
NORMAL ("This is the standard test on means when the variance is unknown. "
	"The test statistic is")
FORMULA ("%t = (%mean - %\\mu) \\Vr%(N / %s^2),")
NORMAL ("which has the Student %t distribution with %ndf = %N-1 degrees of freedom.")
NORMAL ("In the formulas above, %mean is the element of the mean vector at "
	"position %index, %\\mu is the hypothesized mean, "
	"%N is the number of observations, %s^2 "
	"is the variance at position [%index][%index] in the covariance matrix.")
NORMAL ("The returned probability %p is the %%two-sided% probability")
FORMULA ("%p = 2 * studentQ (%t, %ndf)")
NORMAL ("A low probability %p means that the difference is significant.")
MAN_END

MAN_BEGIN ("Covariance: Get fraction variance...", "djmw", 20040407)
INTRO ("A command to ask the selected @Covariance object for the fraction "
	"of the total variance that is accounted for by the selected dimension(s).")
ENTRY ("Arguments")
TAG ("%%From dimension%, %%To dimension%")
DEFINITION ("define the range of components. By choosing both numbers equal, "
	"you get the fraction of the variance \"explained\" by that dimension.")
ENTRY ("Details")
NORMAL ("The total variance is the sum of the diagonal elements of the covariance "
	"matrix #C, i.e., its trace. "
	"The fraction is defined as:")
FORMULA ("\\Si__%i=%from..%to_ %%C__ii_% / \\Si__%i=1..%numberOfRows_ %%C__ii_%")
MAN_END

MAN_BEGIN ("Covariance: Get significance of means difference...", "djmw", 20040407)
INTRO ("Gets the level of significance for the %difference of two means "
	"from the selected @Covariance object being different from a hypothesized "
	"value.")
ENTRY ("Arguments")
TAG ("%%Index1, Index2")
DEFINITION ("the positions of the two elements of the means vector whose "
	"difference is compared to the hypothesized difference.")
TAG ("%%Value")
DEFINITION ("the hypothesized difference (%\\mu).")
TAG ("%%Paired samples")
DEFINITION ("determines whether we treat the two means as being dependent. ")
TAG ("%%Equal variances")
DEFINITION ("determines whether the distribution of the difference of the means "
	"is a Student t-distribution (see below).")
ENTRY ("Behaviour")
NORMAL ("This is Student's t-test for the significance of a difference of means. "
	"The test statistic is:")
FORMULA ("%t = (%mean__1_ - %mean__2_ - %\\mu) \\Vr (%N / %s^2) with %ndf "
	"degrees of freedom.")
NORMAL ("In the formula above %mean__1_ and %mean__2_ are the elements of the "
	"means vector, %\\mu is the hypothesized difference and %N is the number of "
	"observations. The value that we use for the (combined) variance %s^2 is:")
FORMULA ("%s^2 = %var__1_ + %var__2_ - 2 * %covar__12_,")
NORMAL ("when the samples are %paired, and ")
FORMULA ("%s^2 = %var__1_ + %var__2_ ")
NORMAL ("when they are not.")
NORMAL ("The %var__1_ and %var__2_ are the variance components for "
	"%mean__1_ and %mean__2_, respectively, and %covar__12_ is their covariance."
	" When we have %%paired samples% we assume that the two variances are "
	"not independent and their covariance is subtracted, otherwise their "
	"covariance is not taken into account. Degrees of freedom parameter %ndf "
	"usually equals 2(%N-1). ")
NORMAL ("If the two variances are significantly different, the statistic %t "
	"above is only %approximately distributed as Student's %t with "
	"degrees of freedom equal to:")
FORMULA ("%ndf = (%N-1) \\.c (%var__1_ + %var__2_)^2 / (%var__1_^2 + "
	"%var__2_^2).")
NORMAL ("The returned probability %p will be the %%two-sided% probability")
FORMULA ("%p = 2 * studentQ (%t, %ndf)")
NORMAL ("A low probability %p means that the difference is significant.")
MAN_END

MAN_BEGIN ("Covariance: Get significance of one variance...", "djmw", 20040407)
INTRO ("Gets the probability for one variance from the selected "
	"@Covariance object being different from a hypothesized variance.")
ENTRY ("Arguments")
TAG ("%Index")
DEFINITION ("the position of the variance element.")
TAG ("%%Hypothesized variance")
DEFINITION ("the hypothesized variance \\si^2")
ENTRY ("Behaviour")
NORMAL ("The test statistic")
FORMULA ("%\\ci^2 = (%N-1)%s^2 / \\si^2,")
NORMAL ("is distributed as a chi-squared variate with %ndf = %N-1 degrees of freedom.")
NORMAL ("The returned probability %p will be ")
FORMULA ("%p = chiSquareQ (%\\ci^2, %ndf)")
MAN_END

MAN_BEGIN ("Covariance: Get significance of variance ratio...", "djmw", 20040407)
INTRO ("Gets the probability for the ratio of two variances "
	"from the selected @Covariance object being different from a hypothesized "
	"ratio.")
ENTRY ("Arguments")
TAG ("%%Index1, index2")
DEFINITION ("determine the variances")
TAG ("%%Hypothesized ratio")
DEFINITION ("the hypothesized ratio %F")
ENTRY ("Behaviour")
NORMAL ("The test statistic")
FORMULA ("%f = %s__1_^2 / %s__2_^2 / %ratio")
NORMAL ("is distributed as Fisher's F distribution with %ndf__1_ = %N-1 and "
	"%ndf__2_ = %N-1 degrees of freedom for the numerator and denominator terms, "
	"respectively.")
NORMAL ("The returned probability %p will be the %%two-sided% probability")
FORMULA ("%p = 2 * fisherQ (%f, %ndf__1_, %ndf__2_)")
NORMAL ("If %s__2_^2 > %s__1_^2 we use 1/%f to determine the probability.")
MAN_END


MAN_BEGIN ("Covariance: To TableOfReal (random sampling)...", "djmw", 20040407)
INTRO ("Generate a @TableOfReal object by random sampling from a multi-variate "
	"normal distribution whose @Covariance matrix is the selected object.")
ENTRY ("Arguments")
TAG ("%%Number of data points")
DEFINITION ("determines the number of data points that will be generated. Each "
	"data point occupies one row in the generated table.")
ENTRY ("Algorithm")
NORMAL ("The algorithm proceeds as follows:")
LIST_ITEM ("1. Calculate the eigenvalues $v__%i_ and "
	"eigenvectors %#e__%i_ of the %m \\xx %m Covariance matrix. "
	"In general there will also be %m of these. Let #%E be the %m \\xx %m matrix "
	"with eigenvector %#e__%j_ in column %j (%j=1..%m).")
LIST_ITEM ("2. Generate a vector #x whose elements %x__%k_ equal %x__%k_ = "
	"randomGauss (0, \\Vr (%v__%k_)). "
	"Each  %x__%k_ is a random deviate drawn from a Gaussian distribution with "
	"mean zero and standard deviation equal to the square root of the corresponding "
	"eigenvalue %v__%k_.")
LIST_ITEM ("3. Calculate the vector #y = #%E #x, obtained by multiplying the vector "
	"#x with the matrix #%E.")
LIST_ITEM ("4. Add the centroid to #y and copy the elements of #y to the corresponding row of "
	"the TableOfReal object.")	
LIST_ITEM ("5. Repeat steps 2, 3 and 4 until the desired number of data points "
	"has been reached.")
LIST_ITEM ("6. Copy the column labels from the Covariance object to the "
	"TableOfReal object.") 
MAN_END

MAN_BEGIN ("Covariance & TableOfReal: Extract quantile range...", "djmw", 20040225)
INTRO ("Extract those rows from the selected @TableOfReal object whose Mahalanobis "
	"distance, with respect to the selected @Covariance object, are within the "
	"quantile range.")
MAN_END

MAN_BEGIN ("Create ChebyshevSeries...", "djmw", 20040407)
INTRO ("A command to create a @ChebyshevSeries from a list of coefficients.")
ENTRY ("Arguments")
TAG ("%Xmin, %Xmax")
DEFINITION ("define the domain of the polynomials.")
TAG ("%Coefficients")
DEFINITION ("define the coefficients of each @@Chebyshev polynomials|Chebyshev polynomial@. "
	"The coefficient of the polynomial with the highest degree comes last.")
MAN_END

MAN_BEGIN ("Create ISpline...", "djmw", 20040407)
INTRO ("A command to create an @ISpline from a list of coefficients.")
ENTRY ("Arguments")
TAG ("%Xmin, %Xmax")
DEFINITION ("define the domain of the polynomial @spline.")
TAG ("%Degree")
DEFINITION ("defines the degree of the polynomial @spline.")
TAG ("%Coefficients")
DEFINITION ("define the coefficients of the basis polynomials.")
TAG ("%%Interior knots")
DEFINITION ("define the positions in the domain where continuity conditions are defined.")
ENTRY ("Behaviour")
NORMAL ("The number of coefficients and the number of interior knots must satisfy "
	"the following relation:")
FORMULA ("%numberOfCoefficients = %numberOfInteriorKnots + %degree")
NORMAL ("")
MAN_END

MAN_BEGIN ("Create MSpline...", "djmw", 20040407)
INTRO ("A command to create an @MSpline from a list of coefficients.")
ENTRY ("Arguments")
TAG ("%Xmin, %Xmax")
DEFINITION ("define the domain of the polynomial @spline.")
TAG ("%Degree")
DEFINITION ("defines the degree of the polynomial @spline.")
TAG ("%Coefficients")
DEFINITION ("define the coefficients of the basis polynomials.")
TAG ("%%Interior knots")
DEFINITION ("define the positions in the domain where continuity conditions are defined.")
ENTRY ("Behaviour")
NORMAL ("The number of coefficients and the number of interior knots must satisfy "
	"the following relation:")
FORMULA ("%numberOfCoefficients = %numberOfInteriorKnots + %degree + 1")
NORMAL ("")
MAN_END

MAN_BEGIN ("Create Polynomial...", "djmw", 20040407)
INTRO ("A command to create an @Polynomial from a list of coefficients.")
ENTRY ("Arguments")
TAG ("%Xmin, %Xmax")
DEFINITION ("define the domain of the polynomial.")
TAG ("%Degree")
DEFINITION ("defines the degree of the basis polynomials.")
TAG ("%Coefficients")
DEFINITION ("define the coefficients of the polynomial. The coefficient of the "
	"highest power of %x comes last.")
MAN_END

MAN_BEGIN ("Create LegendreSeries...", "djmw", 20040407)
INTRO ("A command to create a @LegendreSeries from a list of coefficients.")
ENTRY ("Arguments")
TAG ("%Xmin, %Xmax")
DEFINITION ("define the domain of the polynomials.")
TAG ("%Coefficients")
DEFINITION ("define the coefficients of each @@Legendre polynomials|Legendre polynomial@. "
	"The coefficient of the polynomial with the highest degree comes last.")
MAN_END

MAN_BEGIN ("Create Sound from gamma-tone...", "djmw", 20040407)
INTRO ("A command to create a @Sound as a @@gamma-tone@.")
ENTRY ("Arguments")
TAG ("%Name")
DEFINITION ("the name of the resulting Sound object.")
TAG ("%%Minimum time%, %%Maximum time% (s)")
DEFINITION ("the start and end time of the resulting Sound.")
TAG ("%%Sampling frequency% (Hz)")
DEFINITION ("the @@sampling frequency@ of the resulting Sound.")
TAG ("%Gamma")
DEFINITION ("the exponent of the polynomial.")
TAG ("%Frequency (Hz), %Bandwidth (Hz)")
DEFINITION ("determine the frequency and damping of the cosine wave in the gamma-tone.")
TAG ("%%Initial phase% (radians)")
DEFINITION ("the initial phase of the cosine wave.")
TAG ("%%Addition factor% (default: 0)")
DEFINITION ("determines the degree of asymmetry in the spectrum of the gamma-tone. "
	"The zero default value gives a gamma-tone. A value unequal to zero results in a "
	"so called %gamma-chirp. A negative value is used in auditory filter modeling to "
	"guarantee the usual direction of filter asymmetry, which corresponds to an upward "
	"glide in instantaneous frequency.")
TAG ("%%Scale amplitudes")
DEFINITION ("determines whether the amplitudes will be scaled to fit in the range (-1, 1).")
ENTRY ("Purpose")
NORMAL ("to create a Sound according to the following formula:")
FORMULA ("%t^^%\\ga\\--1^ e^^\\--2%\\pi\\.c%bandwidth\\.c%t^ "
	"cos (2%%\\pi\\.cfrequency\\.ct% + %additionFactor\\.cln(%t) + %initialPhase),")
NORMAL ("The %gamma %chirp function has a monotonically frequency-modulated carrier (the chirp) with "
	"instantaneous frequency ")
FORMULA ("%instantaneousFrequency(%t) = %frequency + %additionFactor / (2\\.c\\pi\\.c%t)")
NORMAL ("and an envelope that is a gamma distribution function. It is a theoretically optimum "
	"auditory filter, in the sense that it leads to minimal uncertainty in the joint time and "
	"scale representation of auditory signal analysis.")
NORMAL ("For faithful modelling of the inner ear, "
	"@@Irino & Patterson (1996)@ conclude that a value of approximately 1.5 * ERB (%frequency) "
	"is appropriate for %bandwidth. "
	"ERB stands for @@equivalent rectangular bandwidth@. Their formula for ERB is:")
FORMULA ("ERB(%f) = 6.23 10^^\\--6^ %f^2 + 93.39 10^^\\--3^ %f + 28.52.")
NORMAL ("To avoid @aliasing in the chirp sound, a sound is only generated during times where the "
	"instantaneous frequency is greater than zero and smaller than the @@Nyquist frequency@.")
MAN_END

MAN_BEGIN ("Create Sound from Shepard tone...", "djmw", 20050629)
INTRO ("One of the commands that create a @Sound.")
ENTRY ("Arguments")
TAG ("%Name")
DEFINITION ("the name of the resulting Sound object.")
TAG ("%%Minimum time%, %%Maximum time% (s)")
DEFINITION ("the start and end time of the resulting Sound.")
TAG ("%%Sampling frequency% (Hz)")
DEFINITION ("the @@sampling frequency@ of the resulting Sound.")
TAG ("%%Lowest frequency% (Hz)")
DEFINITION ("the frequency of the lowest component in the tone complex.")
TAG ("%%Number of components%")
DEFINITION ("the number of frequency components in the tone complex.")
TAG ("%%Frequency change% (semitones/s)")
DEFINITION ("determines how many semitones the frequency of each component will change in one second. "
	"The number of seconds needed to change one octave will then be 12 divided by this number. "
	"You can make rising, falling and monotonous tone complexes by chosing a positive, negative or zero value.")
TAG ("%%Amplitude range% (dB)")
DEFINITION ("determines the relative size in decibels of the maximum and the minimum amplitude of the components in a tone complex. These relative amplitudes will then be 10^^\\--%amplitudeRange/20^. ")
TAG ("%%Octave shift fraction% [0,1)]")
DEFINITION ("shifts all frequency components by this fraction at the start. You will probably only need this "
	"if you want to generate static tone complexes as the example script below shows.")
ENTRY ("Purpose")
NORMAL ("To create a Sound that is a continuous variant of the sound sequences "
	"used by @@Shepard (1964)@ in his "
	"experiment about the circularity in judgments of relative pitch.")
NORMAL ("The tone consists of many sinusoidal components whose frequencies "
	"might increase exponentially in time. "
	"All frequencies are always at successive intervals of an octave and sounded simultaneously. "
	"Thus the frequency of each component above the lowest is at each moment in time exactly twice "
	"the frequency of the one just below. The amplitudes are large for the components of intermediate "
	"frequency only, and tapered off gradually to subthreshold levels for the components at the "
	"highest and lowest extremes of frequency.")
NORMAL ("For a rising tone complex, the Sound is generated according to the following specification:")
FORMULA ("%s(%t) = \\su__%i=1..%numberOfComponents_ %A__%i_(%t) sin (arg__%i_(%t)), where")
FORMULA ("arg__%i_(%t) = \\in 2%\\pi f__%i_(%\\ta) %d\\ta , and")
FORMULA ("f__%i_(%t) = %lowestFrequency \\.c 2^^(%i \\-- 1 + octaveShiftFraction + %t/(12/%frequencyChange_st)^, with")
FORMULA ("%A__%i_(%t) = 10^^((%L__min_ + (%L__max_ \\-- %L__min_) (1 \\-- cos 2%\\pi%\\te__%i_(%t)) / 2) / 20)^, where,")
FORMULA ("%L__max_ = 0, %L__min_ = 10^^\\--%amplitudeRange/20^, and,")
FORMULA ("%\\te__%i_(%t) = 2\\pi log2 (%f(%t) / %lowestFrequency) / %numberOfComponents.")
NORMAL ("The maximum frequency that can be reached during a sweep by any single tone is:")
FORMULA ("%maximumFrequency = %lowestFrequency\\.c2^^%numberOfComponents^.")
NORMAL ("A component that reaches the maximum frequency falls instantaneously to the lowest frequency and then starts rising again.")
NORMAL ("The absolute @@sound pressure level@ of the resulting sound will not be set, it is only guaranteed that the peak value "
	"is just below 1. You can always scale the intensity with the ##Scale Intensity...# command.")
ENTRY ("Example")
NORMAL ("The following script generates 12 static Shepard tone complexes, 1 semitone 'apart', "
	"with a cosine window to temper the abrupt start and finish.")
CODE ("twin = 0.010")
CODE ("fwin = 1 / (2 * twin)")
CODE ("for i to 12")
CODE1 ("fraction = (i-1)/12")
CODE1 ("Create Sound from Shepard tone... s'i' 0 0.1 22050 4.863 10 0 34 'fraction'")
CODE1 ("Formula... if x<twin then self*(1+cos(2*pi*fwin*(x+twin)))/2 else self fi")
CODE1 ("Formula... if x>xmax-twin then self*(1+cos(2*pi*fwin*(x-xmax+twin)))/2 else self fi")
CODE ("endfor")
MAN_END

MAN_BEGIN ("Create formant table (Peterson & Barney 1952)", "djmw", 20040512)
INTRO ("A command to create a @Table object filled with the "
	"fundamental frequency and the first three formant frequency values from 10 "
	"American-English monophthongal vowels as spoken in a /h_d/ context by 76 speakers "
	"(33 men, 28 women and 15 children). Every vowel was pronounced twice, so that there are "
	"1520 recorded vowels in total.")
ENTRY ("Table layout")
NORMAL ("The created table will contain 9 columns:")
TAG ("Column 1, labelled as %Type")
DEFINITION ("speaker type: \"m\", \"w\" or \"c\" (for %man, %women or %child).")
TAG ("Column 2, labelled as %Sex")
DEFINITION ("speaker sex: either \"m\" or \"f\" (for %male or %female).")
TAG ("Column 3, labelled as %Speaker")
DEFINITION ("speaker id: a number from 1 to 76.")
TAG ("Column 4, labelled as %Vowel")
DEFINITION ("the vowel name. The following list gives the vowel in a %h_d context word "
	"together with its representation in this column: (%heed, iy), (%hid, ih), "
	"(%head, eh), (%had, ae), (%hod, aa), (%hawed, ao), (%hood, uh), (%%who'd%, uw), "
	"(%hud, ah), (%heard, er).")
TAG ("Column 5, labelled as %IPA")
DEFINITION ("the IPA notation for the vowels.")
TAG ("Column 6, labelled as %F0")
DEFINITION ("the fundamental frequency in Hertz.")
TAG ("Column 7, 8 and 9, labelled as %F1, %F2 and %F3")
DEFINITION ("the frequencies in Hertz of the first three formants.")
NORMAL ("We originally downloaded the data from a University of Pennsylvania FTP site, "
	"where they were reportedly based on a printed version supplied by Ignatius Mattingly. "
	"More details about these data and how they were measured can be found in "
	"@@Peterson & Barney (1952)@.")
MAN_END

MAN_BEGIN ("Create formant table (Pols & Van Nierop 1973)", "djmw", 20020620)
INTRO ("A command to create a @Table object filled with the frequencies and the levels "
	"of the first three formants from the 12 Dutch monophthong "
	"vowels as spoken in /h_t/ context by 50 male and 25 female speakers.")
ENTRY ("Table layout")
NORMAL ("The created table will contain 10 columns")
TAG ("Column 1, labeled as %Sex")
DEFINITION ("speaker sex: Either \"m\" or \"f\" (for %male or %female).")
TAG ("Column 2, labeled as %Speaker")
DEFINITION ("speaker id: a number from 1 to 75.")
TAG ("Column 3, labeled as %Vowel")
DEFINITION ("the vowel name. The following list gives the vowel in p_t context word "
	"together with its representation in this column: (%poet, oe), (%paat, aa), "
	"(%poot, oo), (%pat, a), (%peut, eu), (%piet, ie), (%puut, uu), (%peet, ee), "
	"(%put, u), (%pet, e), (%pot, o), (%pit, i).")
TAG ("Column 4, labeled as %IPA")
DEFINITION ("the IPA-notation for the vowels")
TAG ("Column 5, 6 and 7, labeled as %F1, %F2 and %F3")
DEFINITION ("the frequencies in Hertz of the first three formants.")
TAG ("Column 8, 9 and 10, labeled as %L1, %L2 and %L3")
DEFINITION ("the levels in decibel below overall SPL of the first three formants.")
NORMAL ("More details about these data and how they were measured can be found "
	"in @@Pols et al. (1973)@ and @@Van Nierop et al. (1973)@.")
MAN_END

MAN_BEGIN ("Create formant table (Weenink 1985)", "djmw", 20041217)
INTRO ("A command to create a @Table object filled with the "
	"fundamental frequency and the first three formant frequency values from 12 "
	"Dutch monophthongal vowels as spoken in isolation by 30 speakers "
	"(10 men, 10 women and 10 children). Every vowel was pronounced only once, so that there are "
	"360 recorded vowels in total. A reduced form, with only the formant frequecy values, is also available "
	"as a @@Create TableOfReal (Weenink 1985)...|TableOfReal@.")
ENTRY ("Table layout")
NORMAL ("The created table will contain 9 columns:")
TAG ("Column 1, labelled as %Type")
DEFINITION ("speaker type: \"m\", \"w\" or \"c\" (for %man, %women or %child).")
TAG ("Column 2, labelled as %Sex")
DEFINITION ("speaker sex: either \"m\" or \"f\" (for %male or %female).")
TAG ("Column 3, labelled as %Speaker")
DEFINITION ("speaker id: a number from 1 to 76.")
TAG ("Column 4, labelled as %Vowel")
DEFINITION ("the vowel name. The following list gives the vowel in Dutch p_t context words "
	"together with its representation in this column: (%poet, oe), (%paat, aa), "
	"(%poot, oo), (%pat, a), (%peut, eu), (%piet, ie), (%puut, uu), (%peet, ee), "
	"(%put, u), (%pet, e), (%pot, o), (%pit, i).")
TAG ("Column 5, labelled as %IPA")
DEFINITION ("the IPA notation for the vowels.")
TAG ("Column 6, labelled as %F0")
DEFINITION ("the fundamental frequency in Hertz.")
TAG ("Column 7, 8 and 9, labelled as %F1, %F2 and %F3")
DEFINITION ("the frequencies in Hertz of the first three formants. ")
NORMAL ("The formant frequency values have been determined by means of LPC analysis with a "
	"varying prediction order. See @@Weenink (1985)@.")
MAN_END

MAN_BEGIN ("Create TableOfReal (Pols 1973)...", "djmw", 19990426)
INTRO ("A command to create a @TableOfReal filled with the first three formant "
	"frequency values and (optionally) the levels from the 12 Dutch monophthongal "
	"vowels as spoken in /h_t/ context by 50 male speakers.")
NORMAL ("The first three columns will contain the frequencies in Hz, the next three columns "
	"the levels in decibels below the overall SPL of the measured vowel segment. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL ("More details about these data and how they were measured can be found in the paper of "
	"@@Pols et al. (1973)@.")
MAN_END

MAN_BEGIN ("Create TableOfReal (Van Nierop 1973)...", "djmw", 20041217)
INTRO ("A command to create a @TableOfReal filled with the first three formant "
	"frequency values and (optionally) the levels from the 12 Dutch monophthongal "
	"vowels as spoken in /h_t/ context by 25 female speakers.")
NORMAL ("The first three columns will contain the frequencies in Hz, the next three columns "
	"the levels in decibels below the overall SPL of the measured vowel segment. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL ("More details about these data and how they were measured can be found in the paper of "
	"@@Van Nierop et al. (1973)@.")
MAN_END

MAN_BEGIN ("Create TableOfReal (Weenink 1985)...", "djmw", 19990426)
INTRO ("A command to create a @TableOfReal filled with the first three formant "
	"frequency values from the 12 Dutch monophthongal "
	"vowels as spoken in isolation by either 10 men or 10 women or 10 children.")
NORMAL ("The three columns will contain the formant frequencies in Hz. Each row will "
	"be labelled with its corresponding vowel symbol.")
NORMAL ("More details about these data and how they were measured can be found in the paper of "
	"@@Weenink (1985)@.")
MAN_END

MAN_BEGIN ("Discriminant", "djmw", 19981103)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type Discriminant represents the discriminant structure of a multivariate "
	"data set with several groups. This discriminant structure consists of a number of orthogonal "
	"directions in space, along which maximum separability of the groups can occur.")
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Discriminant analysis@ tutorial")
LIST_ITEM ("\\bu @@TableOfReal: To Discriminant@")
NORMAL ("Drawing")
LIST_ITEM ("\\bu Draw eigenvalues...")
LIST_ITEM ("\\bu Draw eigenvector...")
LIST_ITEM ("\\bu @@Discriminant: Draw sigma ellipses...|Draw sigma ellipses...@")
MAN_END

MAN_BEGIN ("Discriminant analysis", "djmw", 19990227)
INTRO ("This tutorial will show you how to perform discriminant analysis with P\\s{RAAT}")
NORMAL ("As an example, we will use the dataset from @@Pols et al. (1973)@ "
	"with the frequencies and levels of the first three formants from the 12 "
	"Dutch monophthongal vowels as spoken in /h_t/ context by 50 male speakers. "
	"This data set has been incorporated into "
	"P\\s{RAAT} and can be called into play with the @@Create TableOfReal "
	"(Pols 1973)...@ command that can be found in the \"New / "
	"TableOfReal\" menu.")
NORMAL ("In the list of objects a new TableOfReal object will appear with 6 "
	"columns and 600 rows "
	"(50 speakers \\xx 12 vowels). The first three columns contain "
	"the formant frequencies in Hz, the last three columns contain the levels "
	"of the first three formants "
	"given in decibels below the overall sound pressure level of the measured "
	"vowel segment. Each row is labelled with a vowel label.")
NORMAL ("Pols et al. use logarithms of frequency values, we will too. Because "
	"the measurement units in the first three columns are in Hz and in the last "
	"three columns in dB, it is probably better to standardize the columns. "
	"The following script summarizes our achievements up till now:")
CODE ("Create TableOfReal (Pols 1973)... yes")
CODE ("Formula... if col < 4 then log10 (self) else self fi")
CODE ("Standardize columns")
CODE ("\\#  change the column labels too, for nice plot labels.")
CODE ("Set column label (index)... 1 standardized log (\\% F\\_ \\_ 1\\_ )")
CODE ("Set column label (index)... 2 standardized log (\\% F\\_ \\_ 2\\_ )")
CODE ("Set column label (index)... 3 standardized log (\\% F\\_ \\_ 3\\_ )")
CODE ("Set column label (index)... 4 standardized \\% L\\_ \\_ 1\\_ ")
CODE ("Set column label (index)... 5 standardized \\% L\\_ \\_ 2\\_ ")
CODE ("Set column label (index)... 6 standardized \\% L\\_ \\_ 3\\_ ")
NORMAL ("To get an indication of what these data look like, we make a scatter "
	"plot of the "
	"first standardized log-formant-frequency against the second standardized "
	"log-formant-frequency. With the next script fragment you can reproduce the "
	"following picture.")
CODE ("Viewport... 0 5 0 5")
CODE ("select TableOfReal pols_50males")
CODE ("Draw scatter plot... 1 2 0 0 -2.9 2.9 -2.9 2.9 10 y + y")
PICTURE (5, 5, drawPolsF1F2_log)
NORMAL ("Apart from a difference in scale this plot is the same as fig. 3 in the "
	"Pols et al. article.")
ENTRY ("1. How to perform a discriminant analysis")
NORMAL ("Select the TableOfReal and choose from the dynamic menu the option "
	"@@TableOfReal: To Discriminant|To Discriminant@. This command is available "
	"in the \"Multivariate statistics\" action button. The resulting Discriminant "
	"object will bear the same name as the TableOfReal object. The following "
	"script summarizes:")
CODE ("select TableOfReal pols_50males")
CODE ("To Discriminant")
ENTRY ("2. How to project data on the discriminant space")
NORMAL ("You select a TableOfReal and a Discriminant object together and choose: "
	"@@Discriminant & TableOfReal: To Configuration...|To Configuration...@. "
	"One of the options of the newly created Configuration object is to draw it. "
	"The following picture shows how the data look in the plane spanned by the "
	"first two dimensions of this Configuration. The directions in this "
	"configuration are the eigenvectors from the Discriminant.")
PICTURE (5, 5, drawPolsDiscriminantConfiguration)		
NORMAL ("The following script summarizes:")
CODE ("select TableOfReal pols_50males")
CODE ("plus Discriminant pols_50males")
CODE ("To Configuration... 0")
CODE ("Viewport... 0 5 0 5")
CODE ("Draw... 1 2 -2.9 2.9 -2.9 2.9 12 y + y")
NORMAL ("If you are only interested in this projection, there also is a short cut "
	"without an intermediate Discriminant object:  "
	"select the TableOfReal object and choose @@TableOfReal: To Configuration "
	"(lda)...|To Configuration (lda)...@.")
ENTRY ("3. How to draw concentration ellipses")
NORMAL ("Select the Discriminant object and choose @@Discriminant: Draw sigma "
	"ellipses...|Draw sigma ellipses...@. In the form you can fill out the "
	"coverage of the ellipse by way of the %%Number of sigmas% parameter. "
	"You can also select the projection "
	"plane. The next figure shows the 1-%\\si concentration ellipses in the "
	"standardized log %F__1_ vs log %F__2_ plane. When the data are multinormally distributed, "
	"a 1-%\\si ellipse will cover approximately 39.3\\%  of the data. "
	"The following code summarizes:")
CODE ("select Discriminant pols_50males")
CODE ("Draw sigma ellipses... 1.0 no 1 2 -2.9 2.9 -2.9 2.9 12 yes")
PICTURE (5, 5, drawPolsF1F2ConcentrationEllipses)
ENTRY ("4. How to classify")
NORMAL ("Select together the Discriminant object (the classifier), and "
	"a TableOfReal object (the data to be classified). Next you choose "
	"@@Discriminant & TableOfReal: To ClassificationTable...|To "
	"ClassificationTable@. "
	"Normally you will enable the option %%Pool covariance matrices% and "
	"the pooled covariance matrix will be used for classification.")
NORMAL ("The ClassificationTable can be converted to a @Confusion object "
	"and its fraction correct can be queried with: "
	"@@Confusion: Get fraction correct@.")
NORMAL ("In general you would separate your data into two independent sets, "
	"\\s{TRAIN} and \\s{TEST}. You would use \\s{TRAIN} to train the "
	"discriminant classifier and \\s{TEST} to test how well it classifies. "
	"Several possibilities for splitting a dataset into two sets exist. "
	"We mention the @@Jackknife|jackknife@ (\"leave-one-out\") and the "
	"@@Bootstrap|bootstrap@ methods (\"resampling\").")
MAN_END

MAN_BEGIN ("Discriminant: Draw sigma ellipses...", "djmw", 20040407)
INTRO ("A command to draw for each group from the selected @Discriminant an ellipse "
	"that covers part of the multivariate data.")
ENTRY ("Arguments")
TAG ("%%Number of sigmas")
DEFINITION ("determines the @@concentration ellipse|data coverage@.")
TAG ("%%Discriminant plane")
DEFINITION ("When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be drawn. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
TAG ("%Xmin, %Xmax, %Ymin, %Ymax")
DEFINITION ("determine the limits of the drawing area.")
TAG ("%%Label size")
DEFINITION ("determines the size of the labels at the centre of the ellipse. No "
	"labels will be drawn when a value less than or equal to zero is chosen.")
MAN_END

MAN_BEGIN ("Discriminant: Extract within-group SSCP...", "djmw", 20020314)
INTRO ("Extract the @SSCP for group %%index% from the selected @Discriminant "
	"object.")
MAN_END

MAN_BEGIN ("Discriminant: Extract pooled within-groups SSCP", "djmw", 20020314)
INTRO ("Extract the pooled within-group @SSCP from the selected @Discriminant "
	"object.")
MAN_END

MAN_BEGIN ("Discriminant: Get partial discrimination probability...", "djmw", 19981102)
INTRO ("A command to test the selected @Discriminant for the significance of "
	"discrimination afforded by the remaining %n\\--%k eigenvectors after the acceptance of "
	"the first %k eigenvectors.")
ENTRY ("Details")
NORMAL ("The test statistic is:")
FORMULA ("%\\ci^2 = \\--(%degreesOfFreedom\\--(%numberOfGroups+%dimension)/2) ln \\La\\'p, where")
FORMULA ("%degreesOfFreedom = (%dimension\\--%k)(%numberOfGroups\\--%k\\--1), and, ")
FORMULA ("\\La\\'p = \\Pi__%j=%k+1..%numberOfEigenvalues_ 1 / (1 + %%eigenvalue[j]%)")
MAN_END

MAN_BEGIN ("Discriminant: Get contribution of component...", "djmw", 19981106)
INTRO ("A command to ask the selected @Discriminant for the contribution of the %j^^th^ "
	"discriminant function (component) to the total variance.")
ENTRY ("Details")
NORMAL ("The contribution is defined as:")
FORMULA ("%%eigenvalue[j]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN ("Discriminant: Get Wilks' lambda...", "djmw", 20040407)
INTRO ("A command to ask the selected @Discriminant for the value of Wilks' lamda (a "
	"multivariate measure of group differences over several variables).")
ENTRY ("Arguments")
TAG ("%From")
DEFINITION ("the first eigenvalue number from which the value for lambda has to be calculated.")
ENTRY ("Details")
NORMAL ("Wilks' lambda is defined as:")
FORMULA ("%\\La = \\Pi__%i=%from..%numberOfEigenvalues_ 1 / (1 + %eigenvalue[%i])")
NORMAL ("Because lambda is a kind of %inverse measure, values of lambda which are near zero "
	"denote high discrimination between groups.")
MAN_END

MAN_BEGIN ("Discriminant: Get concentration ellipse area...", "djmw", 20040407)
INTRO ("A command to query the @Discriminant object for the area of the concentration "
	"ellipse of one of its groups.")
ENTRY ("Arguments")
TAG ("%%Number of sigmas")
DEFINITION ("determines the @@concentration ellipse|data coverage@.")
TAG ("%%Discriminant plane")
DEFINITION ("When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the area of the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be calculated. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
ENTRY ("Algorithm") 
NORMAL ("See @@SSCP: Get sigma ellipse area...")
MAN_END

MAN_BEGIN ("Discriminant: Get confidence ellipse area...", "djmw", 20040407)
INTRO ("A command to query the @Discriminant object for the area of the confidence "
	"ellipse of one of its groups.")
ENTRY ("Arguments")
TAG ("%%Discriminant plane")
DEFINITION ("When on, the selected %X and %Y-dimension will refer to the eigenvectors "
	"of the discriminant space, and, consequently, the area of the projection of the hyper ellipsoid "
	"onto the space spanned by these eigenvectors will be calculated. When off, the selected "
	"%X and Y-dimension will refer to the original dimensions.")
ENTRY ("Algorithm") 
NORMAL ("See @@SSCP: Get confidence ellipse area...")
MAN_END

MAN_BEGIN ("Discriminant & Pattern: To Categories...", "djmw", 20040422)
INTRO ("A command to use the selected @Discriminant to classify each pattern from the "
	"selected @Pattern into a category.")
NORMAL ("Arguments as in @@Discriminant & TableOfReal: To ClassificationTable...@.")
MAN_END

MAN_BEGIN ("Discriminant & SSCP: Project", "djmw", 20020313)
INTRO ("A command to project the selected @SSCP object on the eigenspace "
	"defined by the selected @Discriminant object.")
NORMAL ("Further details can be found in @@Eigen & SSCP: Project@")
MAN_END

MAN_BEGIN ("Discriminant & TableOfReal: To ClassificationTable...", "djmw", 20040407)
INTRO ("A command to use the selected @Discriminant to classify each row from the "
	"selected @TableOfReal. The newly created @ClassificationTable will then contain the posterior "
	"probabilities of group membership.")
ENTRY ("Arguments")
TAG ("%%Pool covariance matrices")
DEFINITION ("when on, all group covariance matrices are pooled and distances will be determined "
	"on the basis of only this pooled covariance matrix (see below).")
ENTRY ("Details")
NORMAL ("The posterior probabilities of group membership %p__%j_ for a vector #x are defined as:")
FORMULA ("%p__%j_ = %p(%j\\| #%x) = exp (\\--%d__%j_^^2^(#%x) / 2) / "
	"\\su__%k=1..%numberOfGroups_ exp (\\--%d__%k_^^2^(#%x) / 2),")
NORMAL ("where %d__%i_^^2^ is the generalized squared distance function:")
FORMULA ("%d__%i_^^2^(#%x) = ((#%x\\--#%\\mu__%i_)\\'p #\\Si__%i_^^-1^ (#%x\\--#%\\mu__%i_) + "
	"ln determinant (#\\Si__%i_)) / 2 \\-- ln %aprioriProbability__%i_")
NORMAL ("that depends on the individual covariance matrix #\\Si__%i_ and the mean "
	"#%\\mu__%i_ for group %i.") 
NORMAL ("When the covariances matrices are %pooled, the squared distance function can be reduced to:")
FORMULA ("%d__%i_^^2^(#%x) = ((#%x\\--#%\\mu__%i_)\\'p #\\Si^^-1^ (#%x\\--#%\\mu__%i_) "
	"\\-- ln %aprioriProbability__%i_,")
NORMAL ("and #\\Si is now the pooled covariance matrix.")
NORMAL ("The a priori probabilities normally will have values that are related to the number of "
	"%training vectors %n__%i_ in each group:")
FORMULA ("%aprioriProbability__%i_ = %n__%i_ / \\Si__%k=1..%numberOfGroups_ %n__%k_")
MAN_END

MAN_BEGIN ("Discriminant & TableOfReal: To Configuration...", "djmw", 20040407)
INTRO ("A command to project each row in the selected @TableOfReal onto "
	"a space spanned by the eigenvectors of the selected @Discriminant. ")
ENTRY ("Arguments")
TAG ("%%Number of dimensions")
DEFINITION ("specifies the number of eigenvectors taken into account, i.e., determines "
	"the dimension of the resulting @Configuration. When the default value (0) is "
	"given the resulting Configuration will have the maximum dimension as allowed by "
	"the number of eigenvectors in the selected Discriminant.")
ENTRY ("Precondition")
NORMAL ("The number of columns in the TableOfReal must equal the dimension of the "
	"eigenvectors in the Discriminant.")
NORMAL ("See also @@Eigen & TableOfReal: Project...@.") 
MAN_END

MAN_BEGIN ("DTW", "djmw", 20000525)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type DTW represents the dynamic time warp structure of "
	"two objects.")
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@CC: To DTW...@ (from 2 objects with cepstral coefficients)")
LIST_ITEM ("\\bu ##Spectrogram: To DTW...# (from 2 Spectrogram objects)")
NORMAL ("Query:")
LIST_ITEM ("\\bu @@DTW: Get time along path...@")
MAN_END

MAN_BEGIN ("DTW: Find path (band)...", "djmw", 20050306)
INTRO ("Finds the optimal path for the selected @DTW satisfying constraints within a Sakoe-Chiba band.")
ENTRY ("Arguments")
TAG ("%%Adjustment window duration")
DEFINITION ("The maximum distance from the start of the sound that a path may start. This is the parameter "
	"that determines the @@Sakoe & Chiba (1978)@ band.")
TAG ("%%Adjustment window includes end")
DEFINITION ("determines whether the adjustment window includes the endpoint. When off and when the durations of "
	"the two objects in the DTW differ by more than the adjustment window duration, the last part of the longest object "
	"cannot be matched at all.")
TAG ("%%X weight, Y weight, Diagonal weight")
DEFINITION ("determine the weights in the cumulative distance calculation.")
NORMAL ("To visualize the Sakoe-Chiba band, select a DTW and chose @@DTW: To Polygon (band)...|To Polygon (band)...@. "
	"Next select ##Draw...# (don't forget to make the drawing domains equal to the domains of the two objects in the DTW).")
NORMAL ("For more information see the article of @@Sakoe & Chiba (1978)@.")
MAN_END

MAN_BEGIN ("DTW: Find path (slopes)...", "djmw", 20050306)
INTRO ("Finds the optimal path for the selected @DTW satisfying slope constraints.")
TAG ("%%Non-diagonal steps, Diagonal steps")
DEFINITION ("implement a slope constraint. Every %%nonDiagonalSteps% steps in a X or Y direction must be followed by "
	"at least %%diagonalSteps% steps in the diagonal direction. By chosing %nonDiagonalSteps = 0 you explicitly forbid "
	"non-diagonal steps, while %diagonalSteps = 0 does not impose any slope constraints at all.")
TAG ("%%X weight, Y weight, Diagonal weight")
DEFINITION ("determine the weights in the cumulative distance calculation.")
ENTRY ("Examples")
NORMAL ("For the constraint 1/2 <= slope <= 2, set %nonDiagonalSteps = 2 and %diagonalSteps = 1.")
NORMAL ("For the constraint 1/3 <= slope <= 3, set %nonDiagonalSteps = 3 and %diagonalSteps = 1.")
NORMAL ("To visualize the slope constraints, select a DTW and chose @@DTW: To Polygon (slopes)...|To Polygon (slopes)...@. "
	"Next select ##Draw...# (don't forget to make the drawing domains equal to the domains of the two objects in the DTW).")
NORMAL ("For more information see the article of @@Sakoe & Chiba (1978)@.")
MAN_END

MAN_BEGIN ("DTW: Get maximum consecutive steps...", "djmw", 20050307)
INTRO ("Get the maximum number of consecutive steps in the chosen direction along the optimal path from the selected @DTW.")
MAN_END

MAN_BEGIN ("DTW: Get time along path...", "djmw", 20040407)
INTRO ("Queries the selected @DTW object for the time along the minimal path "
	"given the time along the \"%x-direction\". ")
ENTRY ("Argument")
TAG ("%Time")
DEFINITION ("the time along the %x-direction.")
ENTRY ("Behaviour")
NORMAL ("When the %input time is in the interval [%xmin, %xmax], the %returned "
	"time will be in the interval [%ymin, %ymax], where [%xmin, %xmax] and "
	"[%ymin, %ymax] are the domains of the two \"objects\" from which the "
	"DTW-object was constructed."
	"For all other input times we assume that the two object are aligned.")
NORMAL ("We like to have a \"continuous\" interpretation of time for the quantized times in the %x and "
	"%y direction; we make the path piecewise linear. There are two special cases:")
NORMAL ("1. The local path is horizontal. We calculate the %y-time from the line that connects the "
	"lower-left position of the leftmost horizontal time block to the upper-right position of the "
	"rightmost horizontal  time block.")
NORMAL ("2. The local path is vertical. We calculate the %y-time from the line that connects the "
	"lower-left position of the bottommost vertical time block to the upper-right position of the "
	"topmost horizontal time block.")
MAN_END

MAN_BEGIN ("DTW: Swap axes", "djmw", 20050306)
INTRO ("Swap the x and y-axes of the selected @DTW.")
MAN_END

MAN_BEGIN ("DTW: To Polygon (band)...", "djmw", 20050307)
INTRO ("A command to convert for a selected @DTW the Sakoe-Chiba band, as implemented by the window adjustment duration parameter, "
	"to a @Polygon object. The polygon shows the boundaries of the search domain for the optimal path.")
MAN_END

MAN_BEGIN ("DTW: To Polygon (slopes)...", "djmw", 20050307)
INTRO ("A command to convert for a selected @DTW the slope constraints "
	"to a @Polygon object. The polygon shows the boundaries of the search domain for the optimal path.")
MAN_END

MAN_BEGIN ("DTW & TextGrid: To TextGrid (warp times)", "djmw", 20060909)
INTRO ("Create a new TextGrid from the selected @DTW and @TextGrid by warping the "
	"times from the selected textGrid to the newly created TextGrid.")
MAN_END

MAN_BEGIN ("Eigen", "djmw", 19981102)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type Eigen represents the eigen structure of "
	"a matrix whose eigenvalues and eigenvectors are real.")
ENTRY ("Inside an Eigen")
NORMAL ("With @Inspect you will see the following attributes:")
TAG ("%numberOfEigenvalues")
DEFINITION ("the number of eigenvalues and eigenvectors")
TAG ("%dimension")
DEFINITION ("the dimension of an eigenvector.")
TAG ("%eigenvalues[1..%numberOfEigenvalues]")
DEFINITION ("the real eigenvalues.")
TAG ("%eigenvectors[1..%numberOfEigenvalues][1..%dimension]")
DEFINITION ("the real eigenvectors, stored by row.")
MAN_END

MAN_BEGIN ("Eigen: Draw eigenvalues...", "djmw", 20040407)
INTRO ("A command to draw the eigenvalues of the selected @Eigen object(s).")
ENTRY ("Arguments")
TAG ("%%Fraction of eigenvalues summed%")
DEFINITION ("defines whether or not fractions are plotted. Fractions %f__%i_ "
	"will be calculated for each number %e__%i_ by dividing this number by the sum of all "
	"numbers %e__%j_: %f__%i_ = %e__%i_ / \\su__%j=1..%numberOfEigenvalues_ %e__%j_.")
TAG ("%%Cumulative%")
DEFINITION ("defines whether or not cumulative values are plotted. Cumulative "
	"values %c__%i_ will be calculated for each number %e__%i_ by summing the first %i "
	"numbers %e__%j_: %c__%i_ = \\su__%j=1..%i_ %e__%j_).")
NORMAL ("A @@Scree plot|scree@ plot can be obtained if both %%Fraction of eigenvalues summed% "
	"and %%Cumulative% are unchecked.")
MAN_END

MAN_BEGIN ("Eigen: Draw eigenvector...", "djmw", 20040407)
INTRO ("A command to draw an eigenvector from the selected @Eigen.")
ENTRY ("Arguments")
TAG ("%Eigenvector %number")
DEFINITION ("determines the eigenvector to be drawn.")
TAG ("%Component %loadings")
DEFINITION ("when on, the eigenvector is multiplied with the square root of the corresponding "
	"eigenvalue. (For @@PCA@-analysis this means that you will draw the so called "
	"%%component loading vector%. You will be able to compare "
	"quantitatively the elements in different component loading vectors because "
	"the %i-th element in the %j-th component loading vector gives the covariance between the %i-th "
	"original variable and the %j-th principal component.)")
TAG ("%%Element range%")
DEFINITION ("determine the first and last element of the vector that must be drawn.")
TAG ("%Minimum, %Maximum")
DEFINITION ("determine the lower and upper bounds of the plot (choosing  Maximum < Minimum "
	"will draw the %%inverted% eigenvector). ")
TAG ("%Mark %size, %Mark %string")
DEFINITION ("determine size and type of the marks that will be drawn.")
TAG ("%Garnish")
DEFINITION ("determines whether a bounding box and margins will be drawn.")
MAN_END

MAN_BEGIN ("Eigen: Get contribution of component...", "djmw", 19981109)
INTRO ("A command to ask the selected @Eigen for the contribution of the %j^^th^ "
	"eigenvalue to the total sum of eigenvalues.")
ENTRY ("Details")
NORMAL ("The contribution is defined as:")
FORMULA ("%%eigenvalue[j]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN ("Eigen: Get cumulative contribution of components...", "djmw", 19981109)
INTRO ("A command to ask the selected @Eigen for the contribution of the sum of the "
	"eigenvalues[%from..%to] to the total sum of eigenvalues.")
ENTRY ("Details")
NORMAL ("The contribution is defined as:")
FORMULA ("\\Si__%i=%from..%to_ %%eigenvalue[i]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN ("Eigen: Get eigenvalue...", "djmw", 20040225)
INTRO ("A command to query the selected @Eigen for the %i^^th^ "
	"eigenvalue.")
MAN_END

MAN_BEGIN ("Eigen: Get eigenvector element...", "djmw", 20040225)
INTRO ("A command to query the selected @Eigen for the %j^^th^ element of the "
	"%i^^th^ eigenvector.")
MAN_END

MAN_BEGIN ("Eigen & Matrix: Project...", "djmw", 20040407)
INTRO ("A command to project the columns of the @Matrix object onto the "
	"eigenspace of the @Eigen object.")
ENTRY ("Arguments")
TAG ("%%Number of dimensions%,")
DEFINITION ("defines the dimension, i.e., the number of rows, of the "
	"resulting object.")
ENTRY ("Algorithm")
NORMAL ("Project each column of the Matrix on the coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
FORMULA ("%y__%ji_ = \\Si__%k=1..%numberOfColums_ %e__jk_ %x__%ki_, where")
NORMAL ("%y__%ji_ is the %j-th element of the %i-th column of the resulting "
	"(matrix) object, %e__%jk_ is the %k-th element of the %j-th eigenvector "
	"and, %x__%ki_ is the %k-th element of the %i-th column of the selected "
	"matrix object.")
MAN_END

MAN_BEGIN ("Eigen & SSCP: Project", "djmw", 20020328)
INTRO ("A command to project the @SSCP object onto the eigenspace of "
	"the @Eigen object.")
ENTRY ("Behaviour")
NORMAL ("Transform the SSCP object as if it was calculated in a coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
FORMULA ("#%S__%t_ = #%E\\'p #%S #%E, where")
NORMAL ("where #%E\\'p is the transpose of the matrix with eigenvectors #%E, "
	"#%S is the square matrix with sums of squares and crossproducts, and "
	"#%S__%t_ the newly created square matrix. The dimension of #%S__%t_ may "
	"be smaller than the dimension of #%S.")
MAN_END

MAN_BEGIN ("Eigen & TableOfReal: Project...", "djmw", 20040407)
INTRO ("A command to project the rows of the @TableOfReal object onto the "
	"eigenspace of the @Eigen object.")
ENTRY ("Arguments")
TAG ("%%Number of dimensions%,")
DEFINITION ("defines the dimension, i.e., the number of columns, of the "
	"resulting object.")
ENTRY ("Algorithm")
NORMAL ("Project each row of the TableOfReal on the coordinate "
	"system given by the eigenvectors of the Eigen object. This can be done "
	"as follows:")
FORMULA ("%y__%ij_ = \\Si__%k=1..%numberOfColums_ %e__jk_ %x__%ik_, where")
NORMAL ("%e__%jk_ is the %k-th element of the %j-th eigenvector, %x__%ik_ is "
	"the %k-th element of the %i-th row and %y__%ij_ is the %j-th element at "
	"the %i-th row of the matrix part of the resulting object.")
MAN_END

MAN_BEGIN ("equivalent rectangular bandwidth", "djmw", 19980713)
INTRO ("The %%equivalent rectangular bandwidth% (ERB) of a filter is defined "
	"as the width of a rectangular filter whose height equals the peak gain of "
	"the filter and which passes the same total power as the filter (given a flat "
	"spectrum input such as white noise or an impulse).")
MAN_END

MAN_BEGIN ("Excitations", "djmw", 19960918)
INTRO ("A collection of objects of type @Excitation. "
	"You can create an #Excitations by selecting one or more #Excitation's and "
	"selecting ##To Excitations#. You can add one or more #Excitation's to an "
	"#Excitations by selecting one #Excitations and one or more "
	"#Excitation's and selecting ##Add to Excitations# (the #Excitation's will "
	"be removed from the list of objects).")
MAN_END

MAN_BEGIN ("Excitations: Append", "djmw", 19960918)
INTRO ("You can choose this command after selecting two objects of type @Excitations. ")
NORMAL ("A new object is created that contains the second object appended after the first.")
MAN_END

MAN_BEGIN ("Excitations: To Pattern...", "djmw", 19960918)
INTRO ("A command to convert every selected @Excitations to a @Pattern object.")
ENTRY ("Arguments")
TAG ("%Join")
DEFINITION ("the number of subsequent @Excitation objects to combine into one row of @Pattern. " 
	"E.g. if an #Excitation has length 26 and %join = 2 then each row of #Pattern "
	"contains 52 elements. The number of rows in #Pattern will be %%my size% / 2. "
	"In the conversion process the elements of an #Excitation will be divided by 100.0 in order "
	"to guarantee that all patterns have values between 0 and 1.")
MAN_END


MAN_BEGIN ("FilterBank: Draw filter functions...", "djmw", 20030901)
INTRO ("")
MAN_END

MAN_BEGIN ("FilterBank: Draw frequency scales...", "djmw", 20030901)
MAN_END

MAN_BEGIN ("FilterBank: Get frequency in Hertz...", "djmw", 20030901)
INTRO ("A @query to the selected FilterBank object.")
ENTRY ("Return value")
NORMAL ("a frequency value in Hertz.")
MAN_END

MAN_BEGIN ("FilterBank: Get frequency in Bark...", "djmw", 20030901)
MAN_END

MAN_BEGIN ("FilterBank: Get frequency in mel...", "djmw", 20030901)
MAN_END

MAN_BEGIN ("FormantFilter", "djmw", 20010404)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type FormantFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%f, %t), expressed "
	"in dB's. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %f__%j_ (on a linear frequency scale).")
ENTRY ("Inside a FormantFilter")
NORMAL ("With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN ("gamma-tone", "djmw", 19980713)
INTRO ("A gamma-tone is the product of a rising polynomial, a decaying exponential function, and a "
	"cosine wave.")
NORMAL ("It can be described with the following formula:")
FORMULA ("gammaTone (%t) = %a %t^^%\\ga\\--1^ e^^\\--2%\\pi\\.c%bandwidth\\.c%t^ "
	"cos (2%%\\pi\\.cfrequency\\.ct% + %initialPhase),")
NORMAL ("where %\\ga determines the order of the gamma-tone.")
NORMAL ("The gammatone function has a monotone carrier (the tone) with an "
	"envelope that is a gamma distribution function. The amplitude spectrum is essentially "
	"symmetric on a linear frequency scale. This function is used in some time-domain "
	"auditory models to simulate the spectral analysis performed by the basilar membrane. "
	"It was popularized in auditory modeling by @@Johannesma (1972)@. @@Flanagan (1960)@ "
	"already used it to model basilar membrane motion.")
MAN_END

MAN_BEGIN ("generalized singular value decomposition", "djmw", 19981007)
INTRO ("For %m > %n, the %%generalized singular value decomposition% (gsvd) of an %m \\xx %n matrix #%A and "
	"a %p \\xx %n matrix #%B is given by the pair of factorizations")
FORMULA ("#%A = #%U #%\\Si__1_ [#%0, #%R] #%Q\\'p and #%B = #%V #%\\Si__2_ [#%0, #%R] #%Q\\'p")
NORMAL ("The matrices in these factorizations have the following properties:")
TAG ("\\bu #%U [%m \\xx %m], #%V [%p \\xx %p] and #%Q [%n \\xx %n]")
DEFINITION (" are orthogonal matrices. In the reconstruction formula's above we maximally need "
	"only the first %n columns of matrices #%U and #%V (when %m and/or %p are greater than %n).")
TAG ("\\bu #%R [%r \\xx %r],")
DEFINITION ("is an upper triangular nonsingular matrix. %r is the rank of [#%A\\'p, #%B\\'p]\\'p "
	"and %r \\<_ %n. The matrix [#%0, #%R] is %r \\xx %n and its first %n \\xx (%n \\-- %r) part "
	"is a zero matrix.")
TAG ("\\bu #%\\Si__1_ [%m \\xx %r] and #%\\Si__2_ [%p \\xx %r]")
DEFINITION ("are real, nonnegative and \"diagonal\".")
NORMAL ("In practice, the matrices #%\\Si__1_ and #%\\Si__2_ are never used. Instead a shorter "
	"representation with numbers %\\al__%i_ and %\\be__%i_ is used. These numbers obey "
	"0 \\<_ \\al__%i_ \\<_ 1 and \\al__%i_^^2^ + \\be__%i_^^2^ = 1. The following relations exist:")
FORMULA ("#%\\Si__1_\\'p #%\\Si__1_ + #%\\Si__2_\\'p #%\\Si__2_ = #%I, ")
FORMULA ("#%\\Si__1_\\'p #%\\Si__1_ = #diag (%\\al__1_^^2^, ..., %\\al__%r_^^2^), and, ")
FORMULA ("#%\\Si__2_\\'p #%\\Si__2_ = #diag (%\\be__1_^^2^, ..., %\\be__%r_^^2^).")
NORMAL ("The ratios \\al__%i_ / \\be__%i_ are called the %%generalized singular values% of the "
	"pair #%A, #%B. Let %l be the rank of #%B and %k + %l (= %r) the rank of [#%A\\'p, #%B\\'p]\\'p. "
	"Then the first %k generalized singular values are infinite and the remaining %l are finite. "
	"(When %#B is of full rank then, of course, %k = 0).")
ENTRY ("Special cases")
NORMAL ("\\bu If #%B is a square nonsingular matrix, the gsvd of #%A and #%B is equivalent to the "
	"singular value decomposition of #%A #%B^^\\--1^.")
NORMAL ("\\bu The generalized eigenvalues and eigenvectors of #%A\\'p #%A - %\\la  #%B\\'p #%B "
	"can be expressed in terms of the gsvd. The columns of the matrix #%X, constructed as")
CODE ("X = Q*( I   0    )")
CODE ("      ( 0 inv(R) ),")
NORMAL ("form the eigenvectors. The important eigenvectors, of course, correspond "
	"to the positions where the %l eigenvalues are not infinite.")
MAN_END

MAN_BEGIN ("invFisherQ", "djmw", 20000525)
INTRO ("$$invFisherQ$ (%q, %df1, %df2) returns the value %f for which "
	"$$@fisherQ (%f, %df1, %df2) = %q.")
MAN_END

MAN_BEGIN ("fisherQ", "djmw", 20000525)
INTRO ("$$fisherQ$ (%f, %df1, %df2) returns the area under Fisher's F-distribution "
	"from %f to +\\oo.")
MAN_END

MAN_BEGIN ("ISpline", "djmw", 19990627)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. ")
NORMAL ("An object of type ISpline represents a linear combination of basis "
	"i@spline functions. Each basis %ispline is a monotonically increasing "
	"polynomial function of degree %p.")
FORMULA ("ISpline (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %ispline__%k_(%x)")
MAN_END

MAN_BEGIN ("Jackknife", "djmw", 20031103)
INTRO ("A technique for estimating the bias and standard deviation of an "
	"estimate.")
NORMAL ("Suppose we have a sample #%x = (%x__1_, %x__2_,...%x__n_) and wish to estimate "
	"the bias and standard error of an estimator \\Te. The jackknife "
	"focuses on the samples that leave out one observation at a time: "
	"the %i-th jackknife sample consists of the data with the %i-th observation "
	"removed.")
MAN_END

MAN_BEGIN ("Legendre polynomials", "djmw", 19990620)
INTRO ("The Legendre polynomials %P__%n_(%x) of degree %n are special "
	"orthogonal polynomial functions defined on the domain [-1, 1].")
NORMAL ("Orthogonality:")
FORMULA ("__-1_\\in^^1^ %W(%x) %P__%i_(%x) %P__%j_(%x) %dx = \\de__%ij_")
FORMULA ("%W(%x) = 1    (-1 < x < 1)")
NORMAL ("They obey certain recurrence relations:")
FORMULA ("%n %P__%n_(%x) = (2%n \\-- 1) %x %P__%n-1_(%x) \\-- (%n \\-- 1) %P__%n-2_(%x)")
FORMULA ("%P__0_(%x) = 1")
FORMULA ("%P__1_(%x) = %x")
NORMAL ("We may %change the domain of these polynomials to [%xmin, %xmax] by "
	"using the following transformation:")
FORMULA ("%x\\'p = (2%x \\-- (%xmax + %xmin)) / (%xmax - %xmin).")
NORMAL ("We subsequently use %P__%k_(%x\\'p) instead of %P__%k_(%x).")
MAN_END

MAN_BEGIN ("LegendreSeries", "djmw", 19990620)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type LegendreSeries represents a linear combination of @@Legendre polynomials@ "
	"%P__%k_(%x).")
FORMULA ("LegendreSeries (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %P__%k_(%x)")
MAN_END

MAN_BEGIN ("LegendreSeries: To Polynomial", "djmw", 19990620)
INTRO ("A command to transform the selected @LegendreSeries object into a @@Polynomial@ object.")
NORMAL ("We find polynomial coefficients %c__%k_ such that")
FORMULA ("\\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x^^%k^ = "
	"\\Si__%k=1..%numberOfCoefficients_ %l__%k_ %P__%k_(%x)")
NORMAL ("We use the recurrence relation for @@Legendre polynomials@ to calculate these coefficients.")
MAN_END

MAN_BEGIN ("Matrix: Draw distribution...", "djmw", 20041110)
INTRO ("A command to draw the distribution histogram of the values in the selected part of a @Matrix.")
ENTRY ("Arguments")
TAG ("%%Horizontal range%, %%Vertical range%")
DEFINITION ("determine the part of the matrix whose distribution will be drawn.")
TAG ("%%Minimum value%, %%Maximum value%")
DEFINITION ("determine the range of values that will be considered in the distribution. "
	"To treat all bin widths equally, the range will include the %%Minimum value% and exclude the "
	"%%Maximum value% (see below).")
TAG ("%%Number of bins%")
DEFINITION ("determines the number of bars in the distribution histogram.")
TAG ("%%Minimum frequency%, %%Maximum frequency%")
DEFINITION ("determine the limits of the vertical axis.")
ENTRY ("Algorithm")
NORMAL ("For a particular matrix element %z, the histogram bin number %%i% that will be incremented obeys the following relation:")
FORMULA ("%%lowerBinBorder%__%i_ \\<_ %z <  %%lowerBinBorder%__%i_+ %%binWidth%,")
NORMAL ("where")
FORMULA ("%%binWidth% = (%%maximumValue% - %%minimumValue%)/%%numberOfBins%,")
NORMAL ("and")
FORMULA ("%%lowerBinBorder%__%i_ = %%minimumValue% + (%i - 1)\\.c%%binWidth%.")
NORMAL ("In this way all bins will be based on exactly the same width, as each binning interval includes its lower border "
	"and excludes its upper border "
	"(i.e., each interval is closed to the left and open to the right). ")
MAN_END

MAN_BEGIN ("Matrix: Solve equation...", "djmw", 19961006)
INTRO ("Solve the general matrix equation #A #x = #b for #x.")
NORMAL("The matrix #A can be any general %m \\xx %n matrix, #b is a %m-dimensional "
	"and #x a %n-dimensional vector. The @Matrix contains #A as its first %n columns "
	"and #b as its last column. The %n-dimensional solution is returned as a #Matrix "
	"with %n columns.")
NORMAL ("When the number of equations (%m) is %greater than the number of unknowns (%n) the "
	"algorithm gives the best least-squares solution. If on the contrary you "
	"have %fewer equations than unknowns the solution will not be unique.")
ENTRY ("Method")
NORMAL ("Singular value decomposition with backsubstitution. "
	"Zero will be substituted for eigenvalues smaller than %tolerance \\.c "
	"%%maximum_eigenvalue% (when the user-supplied %tolerance equals 0.0 a "
	"value of 2.2 \\.c 10^^-16^ \\.c %%number_of_unknowns% "
	"will be used as %tolerance).")
NORMAL ("See for more details: @@Golub & van Loan (1996)@ chapters 2 and 3.")
MAN_END

MAN_BEGIN ("MelFilter", "djmw", 20010404)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type MelFilter represents an acoustic time-frequency "
	"representation of a sound: the power spectral density %P(%f, %t), "
	"expressed in dB's. "
	"It is sampled into a number of points around equally spaced times %t__%i_ "
	"and frequencies %f__%j_ (on a Mel frequency scale).")
ENTRY ("Inside a MelFilter")
NORMAL ("With @Inspect you will see that this type contains the same "
	"attributes a @Matrix.")
MAN_END

MAN_BEGIN ("MelFilter: To MFCC...", "djmw", 20010404)
INTRO ("A command to create a @MFCC object from each selected @MelFilter "
	"object.")
NORMAL ("Mel frequency cepstral coefficients result from the Discrete Cosine "
	"Transform of the filterbank spectrum (in dB). The following formula "
	"shows the relation:")
FORMULA ("%c__%i_ = \\Si__%j=1_^^%N^ %P__%j_ cos (%i\\pi/%N (%j-0.5))),")
NORMAL ("where %N represents the number of filters and %P__%j_ the power in dB "
	"in the %j^^%th^ filter.")
NORMAL ("This transformation was first used by @@Davis & Mermelstein (1980)@.")
MAN_END

MAN_BEGIN ("MSpline", "djmw", 19990627)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. ")
NORMAL ("An object of type MSpline represents a linear combination of basis "
	"m@spline functions. Each basis %mspline is a positive polynomial function "
	"of degree %p.")
FORMULA ("MSpline (%x) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %mspline__%k_(%x)")
MAN_END

MAN_BEGIN ("Pattern", "djmw", 20041201)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
INTRO ("An object of type Pattern represents a sequence of patterns that can serve as "
	"inputs for a neural net. All elements in a Pattern have to be in the interval [0,1].")
ENTRY ("Pattern commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu ##Create Pattern with zeroes...#")
LIST_ITEM ("\\bu @@TableOfReal: To Pattern and Categories...@")
NORMAL ("Synthesis:")
LIST_ITEM ("\\bu @@FFNet & Pattern: To Categories...@")
LIST_ITEM ("\\bu @@Pattern & Categories: To FFNet...@")
ENTRY ("Inside a Pattern")
NORMAL ("With @Inspect you will see that this type contains the same "
	"attributes as a @Matrix.")
MAN_END

MAN_BEGIN ("PCA", "djmw", 19990323)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. "
	"See the @@Principal component analysis@ tutorial.")
NORMAL ("An object of type PCA represents the principal components analysis "
	"of a multivariate dataset.")
ENTRY ("Commands")
NORMAL ("Creation:")
LIST_ITEM ("\\bu @@Principal component analysis@ tutorial")
LIST_ITEM ("\\bu @@TableOfReal: To PCA@")	
ENTRY ("Inside a PCA")
NORMAL ("With @Inspect you will see that this type contains the same "
	"attributes as an @Eigen with the following extras:")
TAG ("%numberOfObservations")
DEFINITION ("the number of observations in the multivariate dataset that originated the PCA, "
	"usually equal to the dataset's number of rows.")
TAG ("%labels[1..%dimension]")
DEFINITION ("the label that corresponds to each dimension.")
TAG ("%centroid")
DEFINITION ("the centroids of the originating multivariate data set.")
MAN_END

MAN_BEGIN ("PCA: Get fraction variance accounted for...", "djmw", 19990106)
INTRO ("A command to query the selected @PCA for the fraction %%variance "
	"accounted for% by the selected components.")
ENTRY ("Arguments")
TAG ("%%Principal component range%")
DEFINITION ("defines the range of the principal components. If you choose both numbers equal, "
	"you get the fraction of the \"variance\" explained by that one component.")
ENTRY ("Details")
NORMAL ("The contribution is defined as:")
FORMULA ("\\Si__%i=%from..%to_ %%eigenvalue[i]% / \\Si__%i=1..%numberOfEigenvalues_ %%eigenvalue[i]%")
MAN_END

MAN_BEGIN ("PCA: Get eigenvalue...", "djmw", 20040225)
INTRO ("A command to query the selected @PCA for the %i^^th^ "
	"eigenvalue.")
MAN_END

MAN_BEGIN ("PCA: Get eigenvector element...", "djmw", 20040225)
INTRO ("A command to query the selected @PCA for the %j^^th^ element of the "
	"%i^^th^ eigenvector.")
MAN_END

MAN_BEGIN ("PCA: Get equality of eigenvalues...","djmw", 19981102)
INTRO ("A command to get the probability that some of the eigenvalues of the "
	"selected @PCA object are equal. A low probability means that it is not "
	"very likely that that these numbers are equal.")
NORMAL ("We test the hypothesis %H__0_: %\\la__%from_ = ... = %\\la__%to_ "
	"that %r (= %to\\--%from+1) of the eigenvalues \\la of the covariance "
	"matrix are equal. The remaining eigenvalues are unrestricted as to their "
	"values and multiplicities. The alternative hypothesis to %H__0_ is that "
	"some of the eigenvalues in the set are distinct.")
ENTRY ("Arguments")
TAG ("%%Eigenvalue range%")
DEFINITION ("define the range of eigenvalues to be tested for equality.")
TAG ("%%Conservative test")
DEFINITION ("when on, a more conservative estimate for %n is chosen (see below).")
ENTRY ("Details")
NORMAL ("The test statistic is:")
FORMULA ("\\ci^2 = \\--%n \\Si__%j=%from..%to_ ln %eigenvalue[%j] + %n %r "
	"ln (\\Si__%j=%from..%to_ %eigenvalue[%j] / %r),")
NORMAL ("with %r(%r+1)/2 \\--1 degrees of freedom. Here %n = %totalNumberOfCases \\-- 1.")
NORMAL ("A special case occurs when the variation in the last %r dimensions is spherical. In a "
	"slightly more conservative test we may replace %n by %n\\--%from\\--(2%r^2+%r+2)/6%r.")
NORMAL ("Also see @@Morrison (1990)@, page 336.")
MAN_END

MAN_BEGIN ("PCA: Get number of components (VAF)...", "djmw", 19990111)
INTRO ("A command to ask the selected @PCA for the minimum number of "
	"components that are necessary "
	"to explain the given fraction %%variance accounted for%.")
ENTRY ("Arguments")
TAG ("%%Variance accounted for% (fraction)")
DEFINITION ("the fraction variance accounted for that must be explained.")
MAN_END

MAN_BEGIN ("PCA: To TableOfReal (reconstruct 1)...", "djmw", 20030108)
INTRO ("A command to reconstruct a single data item. The result is stored as "
	"a @TableOfReal with only one row.")
ENTRY ("Arguments")
TAG ("%Coefficients")
DEFINITION ("the weight for the eigenvectors.")
NORMAL ("The algorithm is explained in @@PCA & Configuration: To TableOfReal "
	"(reconstruct)@.")
MAN_END

MAN_BEGIN ("PCA & Configuration: To TableOfReal (reconstruct)", "djmw", 20030108)
INTRO ("A command to reconstruct a @TableOfReal from the selected @Configuration"
	" and @PCA.")
NORMAL ("The TableOfReal is reconstructed from the eigenvectors of the PCA and "
	"elements of the Configuration are the weight factors: ")
FORMULA ("%#t__%i_ = \\Si__%k_ %c__%ik_ #%e__%k_,")
NORMAL ("where %#t__%i_ is the %i-th row in the resulting TableOfReal object, %c__%ik_ is "
	"the element at row %i and column %k in the Configuration object and #%e__%k_ "
	"the %k-th eigenvector from the PCA object.")
MAN_END

MAN_BEGIN ("PCA & PCA: Get angle between pc1-pc2 planes", "djmw", 20041028)
INTRO ("A command to calculate the angle between two planes. Each plane is spanned by the first "
	"two eigenvectors from the corresponding @@PCA@.")
ENTRY ("Algorithm")
NORMAL ("The algorithm is described in section 12.4.3 of @@Golub & van Loan (1996)@:")
NORMAL ("First we form the projection of one set of eigenvectors on the other set. "
	"This results in a 2\\xx2 matrix #C:")
FORMULA ("#C = #E__1_\\'p #E__2_,")
NORMAL ("where #E__1_ and #E__2_ are 2\\xx%%dimension% and %%dimension%\\xx2 matrices "
	"that contain the first two eigenvectors of the PCA's, respectively.")
NORMAL ("Next, we compute the @@singular value decomposition@ of #C:")
FORMULA ("#C = #U #\\Si #V\\'p")
NORMAL ("Now the cosine of the angle between the two planes is given by \\si__2_ and "
	"the angle in degrees is therefore:")
FORMULA ("arccos (\\si__2_)\\.c180/\\pi")
MAN_END

MAN_BEGIN ("PCA & PCA: To Procrustes...", "djmw", 20041028)
INTRO ("A command to calculate a @Procrustes from the two selected @@PCA@'s.")
NORMAL ("Determines the orthogonal @@Procrustes transform@.")
NORMAL ("Algorithm 12.4.1 in @@Golub & van Loan (1996)@.") 
MAN_END

MAN_BEGIN ("PCA & TableOfReal: To Configuration...", "djmw", 19990111)
INTRO ("A command to construct a @Configuration from the selected @TableOfReal"
	" and @PCA.")
ENTRY ("Arguments")
TAG ("%%Number of dimensions")
DEFINITION ("determines the dimension of the resulting Configuration.")
ENTRY ("Algorithm")
NORMAL ("The TableOfReal is projected on the eigenspace of the PCA, i.e., "
	"each row of the TableOfReal is treated as a vector, and the inner product "
	"with the eigenvectors of the PCA determine its coordinates in the Configuration.")
NORMAL ("Because the algorithm performs a projection, the resulting Configuration will "
	"##only be centered#, i.e., its centroid will be at ##0#, if the data in the "
	"TableOfReal object are centered too. ")
NORMAL ("See also @@Eigen & TableOfReal: Project...@.")
MAN_END

MAN_BEGIN ("PCA & TableOfReal: Get fraction variance...", "djmw", 20040324)
INTRO ("A command to query the selected @PCA and @TableOfReal object for the explained "
	"fraction of the variance if the TableOfReal object were projected onto the PCA space.")
ENTRY ("Algorithm")
LIST_ITEM ("1. The TableOfReal is @@TableOfReal: To Covariance|converted@ to a "
	"Covariance object.")
LIST_ITEM ("2. The Covariance object is @@PCA & Covariance: Project|projected@ on "
	"the PCA eigenspace and the newly obtained projected Covariance object is "
	"@@Covariance: Get fraction variance...|queried@ for the fraction variance.")
MAN_END

MAN_BEGIN ("Polynomial", "djmw", 19990608)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type "
	"Polynomial represents a polynomial function on a domain.")
NORMAL ("A polynomial of degree %n is defined as:")
FORMULA ("%p(%x) = %c__1_ + %c__2_ %x + %c__3_ %x^^2^ + ... c__%n+1_ %x^^%n^.")
NORMAL ("The real numbers %c__%k_ are called the polynomial %coefficients.")
ENTRY ("Commands")
NORMAL ("Creation")
LIST_ITEM ("\\bu @@Create Polynomial...@ (in the ##New menu#)")
LIST_ITEM ("\\bu @@LPC: To Polynomial (slice)...@ (from prediction coefficients)")
LIST_ITEM ("\\bu @@LegendreSeries: To Polynomial@")
LIST_ITEM ("\\bu @@ChebyshevSeries: To Polynomial@")
NORMAL ("Drawing")
LIST_ITEM ("\\bu ##Draw...#")
NORMAL ("Queries")
LIST_ITEM ("\\bu @@Polynomial: Get function value...|Get function value...@: get %p(%x)")
LIST_ITEM ("\\bu ##Get coefficient value...#: get %c__%i_")
LIST_ITEM ("\\bu @@Polynomial: Get minimum...|Get minimum...@: minimum of %p(%x) on an interval")
LIST_ITEM ("\\bu @@Polynomial: Get x of minimum...|Get x of minimum...@")
LIST_ITEM ("\\bu @@Polynomial: Get maximum...|Get maximum...@: maximum of %p(%x) on an interval")
LIST_ITEM ("\\bu @@Polynomial: Get x of maximum...|Get x of maximum...@")
LIST_ITEM ("\\bu @@Polynomial: Get area...|Get area...@")
NORMAL ("Modification")
LIST_ITEM ("\\bu ##Set domain...#: new domain")
LIST_ITEM ("\\bu ##Set coefficient value...#: change one coefficient")
NORMAL ("Conversion")
LIST_ITEM ("\\bu @@Polynomial: To Spectrum...|To Spectrum...@ (evaluation over unit-circle)")
LIST_ITEM ("\\bu @@Polynomial: To Polynomial (derivative)|To Polynomial (derivative)@")
LIST_ITEM ("\\bu @@Polynomial: To Polynomial (primitive)|To Polynomial (primitive)@")
LIST_ITEM ("\\bu @@Polynomial: To Roots|To Roots@: roots of polynomial")
MAN_END

MAN_BEGIN ("Polynomial: Get area...", "djmw", 19990610)
INTRO ("A command to compute the area below the selected @Polynomial object.")
ENTRY ("Arguments")
TAG ("%Xmin, %Xmax")
DEFINITION ("define the interval.")
NORMAL ("The area is defined as __%xmin_\\in^^xmax^ %p(%x) %dx.")
MAN_END

MAN_BEGIN ("Polynomial: Get function value...", "djmw", 19990610)
INTRO ("A command to compute %p(%x) for the selected @Polynomial object.")
MAN_END

MAN_BEGIN ("Polynomial: Get maximum...", "djmw", 19990610)
INTRO ("A command to compute, on a specified interval, the maximum value of the selected "
	"@Polynomial object.")
MAN_END

MAN_BEGIN ("Polynomial: Get x of maximum...", "djmw", 19990610)
INTRO ("A command to compute, on a specified interval, the location of the maximum of the "
	"selected @Polynomial object.")
MAN_END

MAN_BEGIN ("Polynomial: Get minimum...", "djmw", 19990610)
INTRO ("A command to compute, on a specified interval, the minimum value of the selected "
	"@Polynomial object.")
MAN_END

MAN_BEGIN ("Polynomial: Get x of minimum...", "djmw", 19990610)
INTRO ("A command to compute, on a specified interval, the location of the minimum of the "
	"selected @Polynomial object.")
MAN_END

MAN_BEGIN ("Polynomials: Multiply", "djmw", 19990616)
INTRO ("A command to multiply two @@Polynomial|polynomials@ with each other.")
NORMAL ("The result of multiplying 1 + 2 %x and 2 \\-- %x^2 will be the polynomial:")
FORMULA ("2 + 4 %x \\-- %x^2 \\-- 2 %x^3.")
MAN_END

MAN_BEGIN ("Polynomial: To Polynomial (derivative)", "djmw", 19990610)
INTRO ("A command to compute the derivative of the selected @Polynomial object.")
MAN_END

MAN_BEGIN ("Polynomial: To Polynomial (primitive)", "djmw", 19990610)
INTRO ("A command to compute the primitive of the selected @Polynomial object.")
MAN_END

MAN_BEGIN ("Polynomial: Scale x...", "djmw", 19990610)
INTRO ("A command to transform the selected @Polynomial object to a new domain.")
TAG ("%Xmin, %Xmax")
DEFINITION ("define the new domain")
ENTRY ("Behaviour")
NORMAL ("The polynomial is transformed from domain [%x__min_, %x__max_] to "
	"domain [%Xmin, %Xmax] in such a way that its form stays the same. "
	"This is accomplished by first calculating:")
FORMULA ("%f(%x\\'p) = \\Si__%k=1..%numberOfCoefficients_ %c__%k_ %x\\'p^^%k^, where")
FORMULA ("%x\\'p = %a %x + %b,")
NORMAL ("and then collecting terms of equal degree. The %a and %b are defined as")
FORMULA ("%a = (%x__min_ \\-- %x__max_) / (%Xmin \\-- %Xmax)")
FORMULA ("%b = %x__min_ \\-- %a %Xmin")
MAN_END

MAN_BEGIN ("Polynomial: To Roots", "djmw", 19990608)
INTRO ("A command to compute the @@Roots|roots@ of the selected @Polynomial objects.")
ENTRY ("Algorithm")
NORMAL ("The roots are found from the polished eigenvalues of a special companion matrix. "
	"For further explanation on these methods see @@Press et al. (1992)@.")
MAN_END

MAN_BEGIN ("Polynomial: To Spectrum...", "djmw", 19990616)
INTRO ("A command to compute the @@Spectrum|spectrum@ of the selected @Polynomial objects.")
ENTRY ("Arguments")
TAG ("%%Nyquist frequency% (Hz)")
DEFINITION ("defines the highest frequency in the spectrum. The lowest frequency of the spectrum "
	"will be 0 Hz.")
TAG ("%%Number of frequencies")
DEFINITION ("defines the number of frequencies in the spectrum.")
ENTRY ("Algorithm")
NORMAL ("We calculate the spectrum by evaluating the polynomial at regularly spaced points %z__%k_ "
	"on the upper half of a circle with radius %r = 1 in the complex plane. The upperhalf of the "
	"unit circle, where %k\\.c%\\fi is in the interval [0, %\\pi], will be mapped to frequencies "
	"[0, @@Nyquist frequency@] in the spectrum. ")
NORMAL ("The complex values %z__%k_ (%k=1..%numberOfFrequencies) are defined as:")
FORMULA ("%z__%k_ = %r e^^%i %k %\\fi^, where,")
FORMULA ("%\\fi = \\pi / (%numberOfFrequencies \\-- 1) and %r = 1.")
MAN_END

MAN_BEGIN ("Principal component analysis", "djmw", 19990323)
INTRO ("This tutorial describes how you can perform principal component "
       "analysis with the P\\s{RAAT}.")
NORMAL ("Principal component analysis (PCA) involves a mathematical procedure "
	"that transforms a number of (possibly) correlated variables into a "
	"(smaller) number of uncorrelated variables called %%principal "
	"components%. The first principal component accounts for as much of the "
	"variability in the data as possible, and each succeeding component "
	"accounts for as much of the remaining variability as possible.")
ENTRY ("1. Objectives of principal component analysis")
LIST_ITEM ("\\bu To discover or to reduce the dimensionality of the data set.")
LIST_ITEM ("\\bu To identify new meaningful underlying variables.")
ENTRY ("2. How to start")
NORMAL ("We assume that the multi-dimensional data have been collected in a @TableOfReal data matrix, "
	"in which the rows are associated with the cases and the columns with the variables.")
NORMAL ("Traditionally, principal component analysis is performed on the "
	"symmetric @@Covariance|Covariance@ matrix or on the symmetric @@correlation|Correlation@ matrix. "
	"These matrices can be calculated from the data matrix. "
	"The covariance matrix contains scaled @@SSCP|sums of squares and cross products@. "
	"A correlation matrix is like a covariance matrix but first the variables, i.e. the columns, have been standardized.  "
	"We will have to standardize the data first if the variances of "
	"variables differ much, or if the units of measurement of the "
	"variables differ. You can standardize the data in the TableOfReal by choosing @@TableOfReal: Standardize columns|Standardize columns@.")
NORMAL ("To perform the analysis, we select the TabelOfReal data matrix in the list of objects and choose "
	"@@TableOfReal: To PCA|To PCA@. This results in a new PCA object in the "
	"list of objects.")
NORMAL ("We can now make a @@Scree plot|scree@ plot of the eigenvalues, @@Eigen: Draw "
	"eigenvalues...|Draw eigenvalues...@ "
	"to get an indication of the importance of each eigenvalue. The exact "
	"contribution of each eigenvalue (or a range of eigenvalues) to the "
	"\"explained variance\" can also be queried: "
	"@@PCA: Get fraction variance accounted for...|Get fraction variance "
	"accounted for...@. You might also check for the equality of a "
	"number of eigenvalues: @@PCA: Get equality of eigenvalues...|Get equality "
	"of eigenvalues...@.")
ENTRY ("3. Determining the number of components")
NORMAL ("There are two methods to help you to choose the number of components. "
	"Both methods are based on relations between the eigenvalues.")
LIST_ITEM ("\\bu Plot the eigenvalues, @@Eigen: Draw eigenvalues...|"
	"Draw eigenvalues...@")
LIST_ITEM1 ("If the points on the graph tend to level out (show an \"elbow\"), "
	"these eigenvalues are usually close enough to zero that they can be "
	"ignored.")
LIST_ITEM ("\\bu Limit variance accounted for, @@PCA: Get number of components "
	"(VAF)...|Get number of components (VAF)...@.")
ENTRY ("4. Getting the principal components")
NORMAL ("Principal components are obtained by projecting the multivariate "
	"datavectors on the space spanned by the eigenvectors. This can be done "
	"in two ways:")
LIST_ITEM ("1. Directly from the TableOfReal without first forming a "
	"@PCA object: "
	"@@TableOfReal: To Configuration (pca)...|To Configuration (pca)...@. "
	"You can then draw the Configuration or display its numbers. ")
LIST_ITEM ("2. Select a PCA and a TableOfReal object together and choose "
	"@@PCA & TableOfReal: To Configuration...|To Configuration...@. "
	"In this way you project the TableOfReal onto the PCA's eigenspace.")
ENTRY ("5. Mathematical background on principal component analysis")
NORMAL ("The mathematical technique used in PCA is called eigen analysis: "
	"we solve for the eigenvalues and eigenvectors of a square symmetric "
	"matrix with sums of squares and cross products. "
	"The eigenvector associated with the largest eigenvalue has the same "
	"direction as the first principal component. The eigenvector associated "
	"with the second largest eigenvalue determines the direction of the second "
	"principal component. "
	"The sum of the eigenvalues equals the trace of the square matrix and the "
	"maximum number of eigenvectors equals the number of rows (or columns) of "
	"this matrix.")
ENTRY ("6. Algorithms")
NORMAL ("If our starting point happens to be a symmetric matrix like the covariance matrix, "
	"we solve for the eigenvalue and eigenvectors "
	"by first performing a Householder reduction to tridiagonal form, followed"
	" by the QL algorithm with implicit shifts.")
NORMAL ("If, conversely, our starting point is the data matrix #%A , "
	"we do not have to form explicitly the matrix with sums of squares and "
	"cross products, #%A\\'p#%A. Instead, we proceed by a numerically more "
	"stable method, and form the @@singular value decomposition@ of #%A, "
	"#%U #%\\Si #%V\\'p. The matrix #%V then contains the eigenvectors, "
	"and the squared diagonal elements of #%\\Si contain the eigenvalues.")
MAN_END

MAN_BEGIN ("PCA & Covariance: Project", "djmw", 20040225)
INTRO ("A command to project the @Covariance object onto the eigenspace of "
	"the @PCA object. ")
NORMAL ("Further details can be found in @@Eigen & SSCP: Project@.")
MAN_END

MAN_BEGIN ("PCA & SSCP: Project", "djmw", 20040225)
INTRO ("A command to project the @SSCP object onto the eigenspace of "
	"the @PCA object. ")
NORMAL ("Further details can be found in @@Eigen & SSCP: Project@.")
MAN_END

MAN_BEGIN ("Regular expressions", "djmw", 20010706)
INTRO ("This tutorial describes the %syntax of regular expressions in P\\s{RAAT} ")
ENTRY ("Introduction")
NORMAL ("A %%regular expression% is a text string that describes a %set "
	"of strings. Regular expressions (regex) are useful as a way to search "
	"for patterns in text strings and, optionally, replace them by another "
	"pattern.")
NORMAL ("Some regex match only one string, i.e., the set they describe has "
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
NORMAL ("You can read the rest of this tutorial sequentially with the help of "
	"the \"<1\" and \">1\" buttons.")
LIST_ITEM ("1. @@Regular expressions 1. Special characters|Special characters@ "
	"(\\bs \\^  \\$ { } [ ] ( ) . + ? \\|  - &)")
LIST_ITEM ("2. @@Regular expressions 2. Quantifiers|Quantifiers@ "
	"(how often do we match).")
LIST_ITEM ("3. @@Regular expressions 3. Anchors|Anchors@ (where do we match)")
LIST_ITEM ("4. @@Regular expressions 4. Special constructs with parenthesis|"
	"Special constructs with parenthesis@ (grouping constructs)")
LIST_ITEM ("5. @@Regular expressions 5. Special control characters|"
	"Special control characters@ (difficult-to-type characters like \\bsn)")
LIST_ITEM ("6. @@Regular expressions 6. Convenience escape sequences|"
	"Convenience escape sequences@ "
	"(\\bsd \\bsD \\bsl \\bsL \\bss \\bsS \\bsw \\bsW \\bsB)")
LIST_ITEM ("7. @@Regular expressions 7. Octal and hexadecimal escapes|"
	"Octal and hexadecimal escapes@ (things like \\bs053 or \\bsX2B)")
LIST_ITEM ("8. @@Regular expressions 8. Substitution special characters|"
	"Substitution special characters@ (\\bs1..\\bs9 \\bsU \\bsu \\bsL \\bsl &)")
NORMAL ("More in depth coverage of regular expressions can be found in "
	"@@Friedl (1997)@.")

MAN_END
	
MAN_BEGIN ("Regular expressions 1. Special characters", "djmw", 20010718)
INTRO ("The following characters are the %meta characters that give special "
	"meaning to the regular expression search syntax:")
TAG ("#\\bs#   the backslash %escape character.")
DEFINITION ("The backslash gives special meaning to the character "
	"following it. For example, the combination \"\\bsn\" stands for the "
	"%newline, one of the @@Regular expressions 5. Special control characters"
	"|control characters@. The combination \"\\bsw\" stands for a \"word\" "
	"character, one of the @@Regular expressions 6. Convenience escape "
	"sequences|"
	"convenience escape sequences@ while \"\\bs1\" is one of the @@Regular "
	"expressions 8. Substitution special characters|substitution special "
	"characters@.")	
LIST_ITEM1("Example: The regex \"aa\\bsn\" tries to match two consecutive "
	"\"a\"s at the end of a line, inclusive the newline character itself.")
LIST_ITEM1 ("Example: \"a\\bs+\" matches \"a+\" and not a series of one or "
	"\"a\"s.")
TAG ("##\\^ #    the caret is the start of line @@Regular expressions 3. "
	"Anchors|anchor@ or the negate symbol.")
LIST_ITEM1 ("Example: \"\\^ a\" matches \"a\" at the start of a line.")
LIST_ITEM1 ("Example: \"[\\^ 0-9]\" matches any non digit.")	
TAG ("##\\$ #    the dollar is the end of line @@Regular expressions 3. "
	"Anchors|anchor@.")	
LIST_ITEM1 ("Example: \"b\\$ \" matches a \"b\" at the end of a line.")
LIST_ITEM1 ("Example: \"\\^ b\\$ \" matches the empty line.")
TAG ("##{ }#    the open and close curly bracket are used as range @@Regular "
	"expressions 2. Quantifiers|quantifiers@.")
LIST_ITEM1 ("Example: \"a{2,3}\" matches \"aa\" or \"aaa\".")	
TAG ("##[ ]#    the open and close square bracket define a character class to "
	"match a %single character.")	
DEFINITION ("The \"\\^ \" as the first character following the \"[\" negates "
	"and the match is for the characters %not listed. "
	"The \"-\" denotes a range of characters. Inside a \"[  ]\" character "
	"class construction most special characters are interpreted as ordinary "
	"characters. ")
LIST_ITEM1 ("Example: \"[d-f]\" is the same as \"[def]\" and matches \"d\", "
	"\"e\" or \"f\".")
LIST_ITEM1 ("Example: \"[a-z]\" matches any lowercase characters in the "
	"alfabet.")
LIST_ITEM1 ("Example: \"[\\^ 0-9]\" matches any character that is not a digit.")
LIST_ITEM1 ("Example: A search for \"[][()?<>$^.*?^]\" in the string "
	"\"[]()?<>$^.*?^\" followed by a replace string \"r\" has the result "
	"\"rrrrrrrrrrrrr\". Here the search string is %one character class and "
	"all the meta characters are interpreted as ordinary characters without "
	"the need to escape them.")	
TAG ("##( )#    the open and close parenthesis are used for grouping "
	"characters (or other regex).")	
DEFINITION ("The groups can be referenced in "
	"both the search and the @@Regular expressions 8. Substitution special "
	"characters|substitution@ phase. There also exist some @@Regular "
	"expressions 4. Special constructs with parenthesis|special constructs "
	"with parenthesis@.")
LIST_ITEM1 ("Example: \"(ab)\\bs1\" matches \"abab\".")	
TAG ("##.#    the dot matches any character except the newline.")
LIST_ITEM1 ("Example: \".a\" matches two consecutive characters where "
	"the last one is \"a\".")
LIST_ITEM1 ("Example: \".*\\bs.txt\\$ \" matches all strings that end in "
	"\".txt\".")	
TAG ("##*#    the star is the match-zero-or-more @@Regular expressions 2. "
	"Quantifiers|quantifier@.")
LIST_ITEM1 ("Example: \"\\^ .*\\$ \" matches an entire line. ")	
TAG ("##+#    the plus is the match-one-or-more quantifier.")
TAG ("##?#    the question mark is the match-zero-or-one "
	"quantifier. The question mark is also used in  "
	"@@Regular expressions 4. Special constructs with parenthesis|special "
	"constructs with parenthesis@ and in @@Regular expressions 2. "
	"Quantifiers|changing match behaviour@.")	
TAG ("##\\| #    the vertical pipe separates a series of alternatives.")	
LIST_ITEM1 ("Example: \"(a|b|c)a\" matches \"aa\" or \"ba\" or \"ca\".")	
TAG ("##< >#    the smaller and greater signs are @@Regular expressions 3. "
	"Anchors|anchors@ that specify a left or right word boundary.")	
TAG ("##-#    the minus indicates a range in a character class (when it is "
	"not at the first position after the \"[\" opening bracket or the last "
	"position before the \"]\" closing bracket.")
LIST_ITEM1 ("Example: \"[A-Z]\" matches any uppercase character.")
LIST_ITEM1 ("Example: \"[A-Z-]\" or \"[-A-Z]\" match any uppercase character "
	"or \"-\".")	
TAG ("##&#    the and is the \"substitute complete match\" symbol.")
MAN_END

MAN_BEGIN ("Regular expressions 2. Quantifiers", "djmw", 20010708)
INTRO ("Quantifiers specify how often the preceding @@Regular expressions|"
	"regular expression@ should match.")
TAG ("##*#   Try to match the preceding regular expression zero or more times.")
LIST_ITEM1 ("Example: \"(ab)c*\" matches \"ab\" followed by zero or more "
	"\"c\"s, i.e., \"ab\", \"abc\", \"abcc\", \"abccc\" ...")
TAG ("##+#   Try to match the preceding regular expression one or more times.")
LIST_ITEM1 ("Example: \"(ab)c+\" matches \"ab\" followed by one or more "
	"\"c\"s, i.e., \"abc\", \"abcc\", \"abccc\" ...")
TAG ("##{%m, %n}#   Try to match the preceding regular expression between %m "
	"and %n times.")
DEFINITION ("If you leave %m out, it is assumed to be zero. If you leave "
	"%n out it is assumed to be infinity. I.e., \"{,%n}\" matches from %zero "
	"to %n times, \"{%m,}\" matches a minimum of %m times, \"{,}\" matches "
	"the same as \"*\" and \"{n}\" is shorthand for \"{n, n\"} and matches "
	"exactly %n times.")
LIST_ITEM1 ("Example: \"(ab){1,2}\" matches \"ab\" and \"abab\".")
TAG ("##?#   Try to match zero or one time.")
ENTRY ("Changing match behaviour")
NORMAL ("Default the quantifiers above try to match as much as possible, they "
	"are %greedy. "
	"You can change greedy behaviour to %lazy behaviour by adding an "
	"extra \"?\" after the quantifier.")
LIST_ITEM1 ("Example: In the string \"cabddde\", the search \"abd{1,2}\" "
	"matches \"abdd\", while the search for \"abd{1,2}?\" matches \"abd\".")
LIST_ITEM1 ("Example: In the string \"cabddde\", the search \"abd+\" "
	"matches \"abddd\", while the search for \"abd+?\" matches \"abd\".")
MAN_END

MAN_BEGIN ("Regular expressions 3. Anchors", "djmw", 20010708)
INTRO ("Anchors let you specify a very specific position within the search "
	"text.")
TAG ("##\\^ #   Try to match the (following) regex at the beginning of a line.")
LIST_ITEM1 ("Example: \"\\^ ab\" matches \"ab\" only at the beginning of a "
	"line and not, for example, in the line \"cab\".")
TAG ("##\\$ #   Try to match the (following) regex at the end of a line.")
TAG ("##<#    Try to match the regex at the %start of a word.")
DEFINITION ("The character class that defines a %word can be found at the "
	"@@Regular expressions 6. Convenience escape sequences|convenience escape "
	"sequences@ page.")
TAG ("##>#    Try to match the regex at the %end of a word.")
TAG ("##\\bsB#   Not a word boundary")
DEFINITION ("")
MAN_END

MAN_BEGIN ("Regular expressions 4. Special constructs with parenthesis", "djmw",
	20010710)
INTRO ("Some special constructs exist with parenthesis. ")
TAG ("##(?:#%regex#)#   is a grouping-only construct.")
DEFINITION ("They exist merely for efficiency reasons and facilitate grouping.")
TAG ("##(?=#%regex#)#   is a positive look-ahead.")
DEFINITION ("A match of the regular expression contained in the positive "
	"look-ahead construct is attempted. If the match succeeds, control is "
	"passed to the regex following this construct and the text consumed by "
	"this look-ahead construct is first unmatched. ")
TAG ("##(?!#%regex#)#   is a negative look-ahead.")
DEFINITION ("Functions like a positive look-ahead, only the "
	"%regex must %not match.")
LIST_ITEM ("Example: \"abc(?!.*abc.*)\" searches for the %last "
	"occurrence of \"abc\" in a string.") 
TAG ("##(?i#%regex#)#   is a case insensitive regex.")
TAG ("##(?I#%regex#)#   is a case sensitive regex.")
DEFINITION ("Default a regex is case sensitive. ")
LIST_ITEM1 ("Example: \"(?iaa)\" matches \"aa\", \"aA\", \"Aa\" and \"AA\".")
TAG ("##(?n#%regex#)#   matches newlines.")
TAG ("##(?N#%regex#)#   doesn't match newlines.") 
NORMAL ("All the constructs above do not capture text and can not be "
	"referenced, i.e., the parenthesis are not counted. However, you "
	"can make them capture text by surrounding them with %ordinary "
	"parenthesis.")
MAN_END

MAN_BEGIN ("Regular expressions 5. Special control characters", "djmw", 20010708)
INTRO ("Special control characters in a @@Regular expressions|regular "
	"expression@ specify characters that are difficult to type.")
TAG ("#\\bsa  alert (bell).")
TAG ("#\\bsb  backspace.")
TAG ("#\\bse  ASCII escape character.")
TAG ("#\\bsf  form feed (new page).")
TAG ("#\\bsn  newline.")
TAG ("#\\bsr  carriage return.")
LIST_ITEM1 ("Example : a search for \"\\bsr\\bsn\" followed by a replace "
	"\"\\bsr\" changes Windows text files to Macintosh text files.")
LIST_ITEM1 ("Example : a search for \"\\bsr\" followed by a replace "
	"\"\\bsn\" changes Macintosh text files to Unix text files.")
LIST_ITEM1 ("Example : a search for \"\\bsr\\bsn\" followed by a replace "
	"\"\\bsn\" changes Windows text files to Unix text files.")
TAG ("#\\bst  horizontal tab.")
TAG ("#\\bsv  vertical tab.")
MAN_END

MAN_BEGIN ("Regular expressions 6. Convenience escape sequences", "djmw", 20010708)
INTRO ("Convenience escape sequences in a @@Regular expressions|regular "
	"expression@ present a shorthand for some character classes.")
TAG ("#\\bsd  matches a digit: [0-9].")
LIST_ITEM1 ("Example: \"-?\\bsd+\" matches any integer.")
TAG ("#\\bsD  %not a digit: [\\^ 0-9].")
TAG ("#\\bsl  a letter: [a-zA-Z].")
TAG ("#\\bsL  %not a letter: [\\^ a-zA-Z].")
TAG ("#\\bss  whitespace: [ \\bst\\bsn\\bsr\\bsf\\bsv].")
TAG ("#\\bsS  %not whitespace: [\\^  \\bst\\bsn\\bsr\\bsf\\bsv].")
TAG ("#\\bsw  \"word\" character: [a-zA-Z0-9\\_ ].")
LIST_ITEM1 ("Example: \"\\bsw+\" matches a \"word\", i.e., a string of one "
	"or more characters that may consist of letters, digits and underscores.")
TAG ("#\\bsW  %not a \"word\" character: [\\^ a-zA-Z0-9\\_ ].")
TAG ("#\\bsB  any character that is %not a word-delimiter.")
MAN_END

MAN_BEGIN ("Regular expressions 7. Octal and hexadecimal escapes", "djmw", 
	20010709)
NORMAL ("An octal number can be represented by the octal escape \"\\bs0\" "
	"and maximally three digits from the digit class [0-7]. "
	"The octal number should not exceed \\bs0377. ")
NORMAL ("A hexadecimal number can be represented by the octal escape "
	"\"\\bsx\" or \"\\bsX\"and maximally two characters from the class "
	"[0-9A-F]. The maximum hexadecimal number should not exceed \\bsxFF. ")
LIST_ITEM1 ("Example: \\bs053 and \\bsX2B both specify the \"+\" character.") 
MAN_END

MAN_BEGIN ("Regular expressions 8. Substitution special characters", "djmw", 
	20010708)
INTRO ("The substitution string is mostly interpreted as ordinary text except "
	"for the @@Regular expressions 5. Special control characters|"
	"special control characters@, the @@Regular expressions 7. Octal and "
	"hexadecimal escapes|octal and hexadecimal escapes@ and the following "
	"character combinations:")
TAG ("#\\bs1 ... #\\bs9    are backreferences at sub-expressions 1 ... 9 in the match.")
DEFINITION ("Any of the first nine sub-expressions of the match string can "
	"be inserted into the replacement string by inserting a `\\bs' followed "
	"by a digit from 1 to 9 that represents the string matched by a "
	"parenthesized expression within the regular expression. The numbering "
	"is left to right.")
LIST_ITEM1("Example: A search for \"(a)(b)\" in the string \"abc\", "
	"followed by a replace \"\\bs2\\bs1\" results in \"bac\".")
TAG ("#&    reference at entire match.")
DEFINITION ("The entire string that was matched by the search operation will "
	"be substituted.")
LIST_ITEM1 ("Example: a search for \".\" in the string \"abcd\" followed by "
	"the replace \"&&\" doubles every character in the result "
	"\"aabbccdd\".")
TAG ("#\\bsU #\\bsu    to uppercase.")
DEFINITION ("The text inserted by \"&\" or \"\\bs1\" ... \"\\bs9\" is "
	"converted to %uppercase (\"\\bsu\" only changes the %first character to "
	"uppercase).")
LIST_ITEM1 ("Example: A search for \"(aa)\" in the string \"aabb\", "
	"followed by a replace \"\\bsU\\bs1bc\" results in the string \"AAbcbb\".")
TAG ("#\\bsL #\\bsl    to lowercase.")
DEFINITION ("The text inserted by \"&\" or \"\\bs1\" ... \"\\bs9\" is "
	"converted to %lowercase (\"\\bsl\" only changes the %first character to "
	"lowercase).")
LIST_ITEM1 ("Example: A search for \"(AA)\" with a replace \"\\bsl\\bs1bc\" "
	"in the string \"AAbb\" results in the string \"aAbcbb\".")
MAN_END

MAN_BEGIN ("Roots", "djmw", 19990608)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type Roots "
	"represents the (complex) roots of a @@Polynomial|polynomial@ function.")
ENTRY ("Commands")
NORMAL ("Creation")
LIST_ITEM ("\\bu @@Polynomial: To Roots@")
NORMAL ("Drawing")
LIST_ITEM ("\\bu ##Draw...# (in the complex plane)")
NORMAL ("Queries")
LIST_ITEM ("\\bu ##Get root...#: get complex root")
LIST_ITEM ("\\bu ##Get real part of root...#")
LIST_ITEM ("\\bu ##Get imaginary part of root...#")
MAN_END

MAN_BEGIN ("Scree plot", "djmw", 20040331)
NORMAL ("A scree plot shows the sorted eigenvalues, from large to "
	"small, as a function of the eigenvalue index.")
MAN_END

MAN_BEGIN ("singular value decomposition", "djmw", 19981007)
INTRO ("For %m > %n, the %%singular value decomposition% (svd) of a real %m \\xx %n matrix #A is the "
	"factorization")
FORMULA ("#A = #U #\\Si #V\\'p,")
NORMAL ("The matrices in this factorization have the following properties:")
TAG ("#U [%m \\xx %n] and #V [%n \\xx %n]")
DEFINITION ("are orthogonal matrices. The columns #u__%i_ of #U =[#u__1_, ..., #u__%n_] "
	"are the %%left singular vectors%, and the columns #v__%i_ of #V [#v__1_, ..., #v__%n_] "
	"are the %%right singular vectors%.")
TAG ("#\\Si [%n \\xx %n] = diag (%\\si__1_, ..., %\\si__%n_)")
DEFINITION ("is a real, nonnegative, and diagonal matrix. Its diagonal contains the so called "
	"%%singular values% %\\si__%i_, where %\\si__1_ \\>_ ... \\>_ %\\si__%n_ \\>_ 0.")
NORMAL ("If %m < %n, the decomposition results in #U [%m \\xx %m] and #V [%n \\xx %m].")
MAN_END

MAN_BEGIN ("Sound & Pitch: Change gender...", "djmw", 20030208)
INTRO ("A command to create a new Sound object with manipulated characteristics "
	"from the selected @Sound and @Pitch.")
NORMAL ("With this command you can have finer grained control over the "
	"pitch than with the @@Sound: Change gender...@ command. "
	"Accurate pitch measurement determines the quality of the "
	"@PSOLA synthesis." )
ENTRY ("Arguments")
NORMAL ("The settings are described in @@Sound: Change gender...@ "
	"(except that we don't need the %%Maximum pitch% here, since it will be "
	"determined from the selected Pitch.")
MAN_END

MAN_BEGIN ("Sound: Change gender...", "djmw", 20030205)
INTRO ("A command to create a new @Sound with manipulated characteristics.")
ENTRY ("Arguments")
NORMAL ("The quality of the @@PSOLA|manipulation@ depends on the pitch measurement.")
NORMAL ("The arguments that control the pitch measurement are:")
TAG ("%%Minimum pitch% (default 75 Hz)")
DEFINITION ("pitch candidates below this frequency will not be considered.")
TAG ("%%Maximum pitch% (default 600 Hz)")
DEFINITION ("pitch candidates above this frequency will be ignored.")
NORMAL ("The arguments that control the manipulation are:")
TAG ("%%Formant shift ratio%")
DEFINITION ("determines the frequencies of the formants in the newly created "
	"Sound. If this ratio equals 1 no frequency shift will occur and "
	"the formant frequencies will not change. A ratio of 1.2 will change "
	"a male voice to a voice with approximate female formant characteristics. "
	"A ratio of 1/1.2 will change a female voice to a voice with approximate male formant "
	"characteristics.")
TAG ("%%New pitch median% (default 0.0 Hz: same as original)")
DEFINITION ("determines what the median pitch of the new Sound will be. "
	"The pitch values in the newly created Sound will be calculated from the pitch "
	"values in the selected Sound by multiplying them by a factor "
	"%%newPitchMedian / oldPitchMedian%. This factor equals 1.0 if the default "
	"value for the new pitch median (0.0) is chosen. ")
TAG ("%%Pitch range factor% (default 1.0)")
DEFINITION ("determines an %extra% scaling of the new pitch values around the %new% "
	"pitch median. A factor of 1.0 means that no additional pitch modification will occur "
	"(except the obvious one described above). A factor of 0.0 monotonizes the new "
	"sound to the new pitch median.")
TAG ("%%Duration factor% (default 1.0)")
DEFINITION ("The factor with which the sound will be lengthened. The default is 1.0. "
	"If you take a value less than 1.0, the resulting sound will be shorter than "
	"the original. A value larger than 3.0 will not work.")
NORMAL ("If you want more control over the synthesis you can supply your own "
	"Pitch object and use the @@Sound & Pitch: Change gender...@ command. ")
ENTRY ("Algorithm")
NORMAL ("The shifting of frequencies is done via manipulation of the sampling frequency. "
	"Pitch and duration changes are generated with @PSOLA synthesis.")
NORMAL ("The new pitch values are calculated in a two step process. We first multiply all "
	"the pitches with the factor %%newPitchMedian / oldPitchMedian% according to:")
FORMULA ("%newPitch = %pitch * %newPitchMedian  / %oldPitchMedian.")
NORMAL ("It follows that if the %newPitchMedian equals the %oldPitchMedian no "
	"change in pitch values will occur in the first step.")
NORMAL ("Subsequently, the pitch range scale factor determines the final pitch values "
	"in the following linear manner:")
FORMULA ("%finalPitch = %newPitchMedian + (%newPitch \\-- %newPitchMedian) * %pitchRangeScaleFactor")
NORMAL ("Hence, it follows that no further scaling occurs if %pitchRangeScaleFactor "
	"equals 1.0.")
MAN_END

MAN_BEGIN ("Sound: Filter (gammatone)...", "djmw", 19980712)
INTRO ("A command to filter a Sound by a fourth order gammatone bandpass filter.")
ENTRY ("Arguments")
TAG ("%%Centre frequency%, %Bandwidth")
DEFINITION ("determine the passband of the filter.")
ENTRY ("Algorithm")
NORMAL ("The impulse response of the filter is a 4-th order @@gamma-tone@. This "
	"filter is implemented as a simple 8-th order recursive digital filter with "
	"4 zeros and 8 poles (these 8 poles consist of one conjugate pole pair to the "
	"4-th power). In the Z-domain its formula is: ")
FORMULA ("%#H (%z) = (1 + \\su__%i=1..4_ %a__%i_%z^^%\\--i^) / "
	"(1 + \\su__%j=1..8_ %b__%j_%z^^%\\--j^)")
NORMAL ("The derivation of the filter coefficients %a__%i_ and %b__%j_ is "
	"according to @@Slaney (1993)@. "
	"The gain of the filter is scaled to unity at the centre frequency.")
MAN_END

MAN_BEGIN ("Sound: To BarkFilter...", "djmw", 20010404)
INTRO ("A command that creates a @BarkFilter object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of filters.")
NORMAL ("The filter functions used are:")
FORMULA ("10 log %#H(%z) = 7 - 7.5 * (%z__%c_ - %z - 0.215) - 17.5 * \\Vr "
	"(0.196 + (%z__%c_ - %z - 0.215)^2)")
NORMAL ("where %z__%c_ is the central (resonance) frequency of the filter in Bark. "
	"The bandwidths of these filters are constant and equal 1 Bark.")
MAN_END

MAN_BEGIN ("Sound: To FormantFilter...", "djmw", 20010404)
INTRO ("A command that creates a @FormantFilter object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of filters whose bandwidths depend on the pitch of the signal.")
NORMAL ("The analysis proceeds in two steps:")
LIST_ITEM ("1. We perform a pitch analysis (see @@Sound: To Pitch...@ for "
	"details).")
LIST_ITEM ("2. We perform a filter bank analysis on a linear frequency scale. "
	"The bandwidth of the filters depends on the measured pitch (see @@Sound & "
	"Pitch: To FormantFilter...@ for details).")
MAN_END

MAN_BEGIN ("Sound: To IntervalTier (silence)...", "djmw", 20060921)
INTRO ("A command that creates an @IntervalTier in which the silence intervals of the selected @Sound are marked.")
ENTRY ("Arguments")
TAG ("%%Silence threshold%")
DEFINITION ("determines the maximum silence intensity value (as a fraction of the intensity range in dB). "
	"If the local intensity is below this value the frame is considered as silence. "
	"For example, if %imax and %imin are the maximum and minimum intensity of the sound then the maximum silence intensity "
	"is calculated as %%imin + silenceThreshold * (imax - imin)%.")
TAG ("%%Minimum silence interval (s)%")
DEFINITION ("determines the minimum duration for an interval to be considered as silence. "
	"If you don't want the closure for a plosive to count as silence then use a large enough value. ")
TAG ("%%Minimum non-silence interval (s)%")
DEFINITION ("determines the minimum duration for an interval to be ##not# considered as silence. "
	"This offers the possibility to filter out small intense bursts of relative short duration.")
TAG ("%%Silence label%")
DEFINITION ("determines the label for a silence interval in the IntervalTier.")
MAN_END

MAN_BEGIN ("Sound & Pitch: To FormantFilter...", "djmw", 20010404)
INTRO ("A command that creates a @FormantFilter object from the selected "
	"@Sound and @Pitch objects by @@band filtering in the frequency domain@ with a "
	"bank of filters whose bandwidths depend on the Pitch.")
NORMAL ("The filter functions used are:")
FORMULA ("%#H(%f, %F__0_) = 1 / (((%f__%c_^^2^ - %f^2) /%f\\.c%B(%F__0_)))^2 + 1),")
NORMAL ("where %f__%c_ is the central (resonance) frequency of the filter. "
	"%B(%F__0_) is the bandwidth in Hz and determined as")
FORMULA ("%B(%F__0_) = %relativeBandwidth\\.c%F__0_, ")
NORMAL ("where %F__0_ is the fundamental frequency as determined from the Pitch "
	"object. Whenever the value of %F__0_ is undefined, a value of 100 Hz is taken.")
MAN_END

MAN_BEGIN ("Sound: To MelFilter...", "djmw", 20010404)
INTRO ("A command that creates a @MelFilter object from every selected "
	"@Sound object by @@band filtering in the frequency domain@ with a "
	"bank of filters.")
NORMAL ("The filter functions used are triangular in shape on a %linear "
	"frequency scale. The filter function depends on three parameters, the "
	"lower frequency %f__%l_, the central frequency %f__%c_ and the higher "
	"frequency %f__%h_. "
	"On a %mel scale, the distances %f__%c_-%f__%l_ and %f__%h_-%f__%c_ "
	"are the same for each filter and are equal to the distance between the "
	"%f__%c_'s of successive filters. The filter function is:" )
FORMULA ("%#H(%f) = 0 for %f \\<_ %f__%l_ and %f \\>_ %f__%h_")
FORMULA ("%#H(%f) = (%f - %f__%l_) / (%f__%c_ - %f__%l_) for %f__%l_ \\<_ %f \\<_ %f__%c_")
FORMULA ("%#H(%f) = (%f__%h_ - %f) / (%f__%h_ - %f__%c_) for %f__%c_ \\<_ %f \\<_ %f__%h_")
MAN_END

MAN_BEGIN ("Sound: To Pitch (shs)...", "djmw", 19970402)
INTRO ("A command that creates a @Pitch object from every selected @Sound object.")
ENTRY ("Purpose")
NORMAL ("to perform a pitch analysis based on a spectral compression model. "
	"The concept of this model is that each spectral component not only activates "
	"those elements of the central pitch processor that are most sensitive to the "
	"component's frequency, but also elements that have a lower harmonic "
	"relation with this component. Therefore, when a specific element of the "
	"central pitch processor is most sensitive at a frequency %f__0_, it receives "
	"contributions from spectral components in the "
	"signal at integral multiples of %f__0_.") 
ENTRY ("Algorithm")
NORMAL ("The spectral compression consists of the summation of a sequence of "
	"harmonically compressed spectra. "
	"The abscissa of these spectra is compressed by an integral factor, the rank "
	"of the compression. The maximum of the resulting sum spectrum is the "
	"estimate of the pitch. Details of the algorithm can be "
	"found in @@Hermes (1988)@")
ENTRY ("Arguments")
TAG ("%%Time step% (default 0.01 s)")
DEFINITION ("the measurement interval (frame duration), in seconds.")
TAG ("%%Minimum pitch% (default 50 Hz)")
DEFINITION ("candidates below this frequency will not be recruited. This parameter "
	"determines the length of the analysis window.")
TAG ("%%Max. number of candidates% (default 15)")
DEFINITION ("The maximum number of candidates that will be recruited.")	
TAG ("%%Maximum frequency% (default 1250 Hz)")
DEFINITION ("higher frequencies will not be considered.")
TAG ("%%Max. number of subharmonics% (default 15)")
DEFINITION ("the maximum number of harmonics that add up to the pitch.")
TAG ("%%Compression factor% (default 0.84)")
DEFINITION ("the factor by which successive compressed spectra are multiplied before the summation.")
TAG ("%%Number of points per octave% (default 48)")
DEFINITION ("determines the sampling of the logarithmic frequency scale.")
TAG ("%Ceiling (default 500 Hz)")
DEFINITION ("candidates above this frequency will be ignored.")
MAN_END

MAN_BEGIN ("Spectra: Multiply", "djmw", 20031023)
INTRO ("Returns a new Spectrum object that is the product of the two selected "
	"@Spectrum objects.")
MAN_END

MAN_BEGIN ("Spectrum: Conjugate", "djmw", 20031023)
INTRO ("Reverses the sign of the complex part of the selected @Spectrum object(s).")
MAN_END

MAN_BEGIN ("SSCP", "djmw", 19981103)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("An object of type SSCP represents the sums of squares and cross products of "
	"a multivariate data set.")
NORMAL ("Besides the matrix part, an object of type SSCP also contains a "
	"vector with centroids.")
ENTRY ("Inside a SSCP")
NORMAL ("With @Inspect you will see that this type contains the same "
	"attributes as a @TableOfReal with the following extras:")
TAG ("%numberOfObservations")
TAG ("%centroid")
MAN_END

MAN_BEGIN ("SSCP: Draw sigma ellipse...", "djmw", 19990222)
INTRO ("A command to draw for the selected @SSCP an ellipse that "
	"covers a part of the multivariate data.")
ENTRY ("Arguments")
TAG ("%%Number of sigmas")
DEFINITION ("determines the @@concentration ellipse|data coverage@.")
MAN_END

MAN_BEGIN ("SSCP: Get sigma ellipse area...", "djmw", 20000525)
INTRO ("A command to query the selected @SSCP object for the area of a "
	"sigma ellipse.")
ENTRY ("Algorithm")
NORMAL ("The algorithm proceeds as follows:")
LIST_ITEM ("1. The four array elements in the SSCP-matrix that correspond to the chosen dimensions "
	"are copied into a two-dimensional matrix #%S (symmetric of course).")
LIST_ITEM ("2. The eigenvalues of #%S are determined, call them %s__1_ and %s__2_.")
LIST_ITEM ("3. The lengths %l__%i_  of the axes of the ellipse can be obtained as the  "
	"square root of the %s__i_ multiplied by a scale factor: %l__%i_ = %scaleFactor \\.c "
	"\\Vr (%s__%i_ ), "
	"where %scaleFactor = %numberOfSigmas / \\Vr(%numberOfObservations \\-- 1).")
LIST_ITEM ("4. The area of the ellipse will be %\\pi\\.c%l__1_\\.c%l__2_.") 
MAN_END

MAN_BEGIN ("SSCP: Get confidence ellipse area...", "djmw", 20000525)
INTRO ("A command to query the selected @SSCP object for the area of a "
	"confidence ellipse.")
ENTRY ("Algorithm")
NORMAL ("The algorithm proceeds as follows:")
LIST_ITEM ("1. The four array elements in the SSCP-matrix that correspond to the chosen dimensions "
	"are copied into a two-dimensional matrix #%S (symmetric of course).")
LIST_ITEM ("2. The eigenvalues of #%S are determined, call them %s__1_ and %s__2_.")
LIST_ITEM ("3. The lengths %l__1_ and %l__2_ of the two axes of the ellipse can be obtained as "
	"(see for example @@Johnson (1998)@, page 410): ")
FORMULA ("    %l__%i_ = %scaleFactor \\.c \\Vr (%s__%i_ ),")
LIST_ITEM ("     where")
FORMULA ("%scaleFactor = \\Vr (%f \\.c %p \\.c (%n \\-- 1) / (%n \\.c (%n \\-- %p))),")
LIST_ITEM ("     in which %f = $$@@invFisherQ@$ (1 \\-- %confidenceLevel, %p, %n \\-- %p), "
	"where %p is the numberOfRows from the SSCP object and %n the %numberOfObservations.")
LIST_ITEM ("4. The area of the ellipse will be %\\pi\\.c%l__1_\\.c%l__2_.") 
MAN_END

MAN_BEGIN ("SSCP: Get diagonality (bartlett)...", "djmw", 20011111)
INTRO ("Tests the hypothesis that the selected @SSCP matrix object is "
	"diagonal.")
ENTRY ("Arguments")
TAG ("%%Number of constraints")
DEFINITION ("modifies the number of independent observations. "
	"The default value is 1.")
ENTRY ("Algorithm")
NORMAL ("The test statistic is |#R|^^N/2^, the N/2-th power of the determinant"
	" of the correlation matrix. @@Bartlett (1954)@ developed the following "
	"approximation to the limiting distribution:")
FORMULA ("\\ci^2 = -(%N - %numberOfConstraints - (2%p + 5) /6) ln |#R|")
NORMAL ("In the formula's above, %p is the dimension of the correlation "
	"matrix, %N-%numberOfConstraints is the number of independent "
	"observations. Normally %numberOfConstraints would "
	"equal 1, however, if the matrix has been computed in some other way, "
	"e.g., from within-group sums of squares and cross-products of %k "
	"independent groups, %numberOfConstraints would equal %k.")
NORMAL ("We return the probability %\\al as ")
FORMULA ("%\\al = chiSquareQ (\\ci^2 , %p(%p-1)/2).")
NORMAL ("A very low %\\al indicates that it is very improbable that the "
	"matrix is diagonal.")
MAN_END

MAN_BEGIN ("SSCP: Get fraction variation...", "djmw", 20040210)
INTRO ("A command to ask the selected @SSCP object for the fraction "
	"of the total variation that is accounted for by the selected dimension(s).")
NORMAL ("Further details can be found in @@Covariance: Get fraction variance...@.")
MAN_END

MAN_BEGIN ("SSCP: To CCA...", "djmw", 20031103)
INTRO ("A command that creates a @@CCA|canonical correlation@ object from the "
	"selected @SSCP object.")
ENTRY ("Arguments")
TAG ("%%Dimension of dependent variate (ny)%")
DEFINITION ("defines a partition of the square %n x %n SSCP matrix S into the parts S__yy_ of "
	"dimension %ny x %ny, S__xx_ of dimension %nx x %nx, and the parts "
	"S__xy_ and S__yx_ of dimensions %nx x %ny and %ny x %nx, respectively.")
ENTRY ("Algorithm")
NORMAL ("The partition for the square SSCP-matrix is as follows:")
PICTURE (2.0, 2.0, drawPartionedMatrix)
NORMAL ("The canonical correlation equations we have to solve are:")
FORMULA ("(1)    (#S__%yx_ #S__%xx_^^-1^ #S__%yx_\\'p -\\la #S__%yy_)#y = #0")
FORMULA ("(2)    (#S__%yx_\\'p #S__%yy_^^-1^ #S__%yx_ -\\la #S__%xx_)#x = #0")
NORMAL ("where #S__%yy_ [%ny \\xx %ny] and #S__%xx_ [%nx \\xx %nx] are "
	"symmetric, positive definite matrices belonging to the dependent and the "
	"independent variables, respectively. ")
NORMAL ("These two equations are not independent and we will show that both "
	"equations have the same eigenvalues and that the eigenvectors #x for "
	"equation (2) can be obtained from the eigenvectors #y of equation (1).")
NORMAL ("We can solve equation (1) in several ways, however, the numerically "
	"stablest algorithm is probably by performing first a Cholesky decomposition "
	"of #S__xx_ and #S__yy_, followed by a @@generalized singular value "
	"decomposition@. The algorithm goes as follows:")
NORMAL ("The Cholesky decompositions (\"square roots\") of #S__yy_ and #S__xx_ are:")
FORMULA ("#S__yy_ = #U\\'p #U and #S__xx_ = #H\\'p #H,")
NORMAL ("where #U and H are upper triangular matrices. From these decompositions, "
	"the inverse for #S__xx_^^-1^ is easily computed. Let #K be the inverse of #H, "
	"then we can write: ")
FORMULA ("#S__xx_^^-1^ = #K #K\\'p.")
NORMAL ("We next substitute in equation (1) and rewrite as:")
FORMULA ("((#K\\'p#S__yx_\\'p)\\'p (#K\\'p#S__yx_\\'p) - \\la #U\\'p #U)#x = 0")
NORMAL ("This equation can be solved for eigenvalues and eigenvectors by the "
	"generalized singular value decomposition because it is of the form "
	"#A\\'p#A -\\la #B\\'p#B.")
NORMAL ("Now, given the solution for equation (1) we can find the solution "
	"for equation (2) by first multiplying (1) from the left with "
	"#S__yx_\\'p#S__yy_^^-1^, resulting in:")
FORMULA ("(#S__yx_\\'p#S__yy_^^-1^#S__yx_#S__xx_^^-1^#S__yx_\\'p -\\la #S__yx_\\'p) #y = 0")
NORMAL ("Now we split of the term #S__xx_^^-1^#S__yx_\\'p and obtain:")
FORMULA ("(#S__yx_\\'p#S__yy_^^-1^#S__yx_ - \\la #S__xx_) #S__xx_^^-1^#S__yx_\\'p #y = 0")
NORMAL ("This equation is like equation (2) and it has therefore the same eigenvalues "
	"and eigenvectors. (We also proved this fact in the algorithmic section of "
	"@@TableOfReal: To CCA...@.)")
NORMAL ("The eigenvectors #x is now")
FORMULA ("#x = #S__xx_^^-1^#S__yx_\\'p #y.")
MAN_END

MAN_BEGIN ("SSCP: To Covariance...", "djmw", 19990524)
INTRO ("A command that creates a @Covariance object from each selected @SSCP object.")
ENTRY ("Arguments")
TAG ("%%Number of constraints")
DEFINITION ("determines the factor by which each entry in the "
	"SSCP-matrix is scaled to obtain the Covariance matrix.")
ENTRY ("Details")
NORMAL ("The relation between the numbers %c__%ij_ in the covariance matrix and the numbers %s__%ij_ in "
	"the originating SSCP matrix is:")
FORMULA ("%c__%ij_ = %s__%ij_ / (%numberOfObservations - %numberOfConstraints)")
NORMAL ("Normally %numberOfConstraints will equal 1. However, when the originating SSCP was the "
	"result of summing %g SSCP objects, as is, for example, the case when you obtained the total "
	"within-groups SSCP from the individual group SSCP's, %numberOfConstraints  will equal %g.")
MAN_END

MAN_BEGIN ("SSCP & TableOfReal: Extract quantile range...", "djmw", 20040225)
INTRO ("Extract those rows from the selected @TableOfReal object whose Mahalanobis "
	"distance, with respect to the selected @SSCP object, are within the "
	"quantile range.")
MAN_END

MAN_BEGIN ("T-test", "djmw", 20020117)
INTRO ("A test on the mean of a normal variate when the variance is unknown.")
NORMAL ("In Praat, the t-test is used to query a @Covariance object and:")
LIST_ITEM ("1. get the significance of one mean. See @@Covariance: Get "
	"significance of one mean...@.")
LIST_ITEM ("2. get the significance of the %difference between two means. "
	"See @@Covariance: Get significance of means difference...@.")
NORMAL ("You should use a t-test when you want to test a hypothesis about "
	"the mean of one column in a @TableOfReal object, or, if you want to test "
	"a hypothesis about the difference between the means of two columns in "
	"this object.")
NORMAL ("You can perform these t-tests in Praat by first transforming the "
	"TableOfReal object into a Covariance object (see @@TableOfReal: To "
	"Covariance@) and then choosing the appropriate query method on the "
	"latter object.")
MAN_END

MAN_BEGIN ("TableOfReal: Change row labels...", "djmw", 20010822)
INTRO ("Changes the row labels of the selected @TableOfReal object according "
	"to the specification in the search and replace fields.")
NORMAL ("Both search and replace fields may contain @@regular expressions|"
	"Regular expressions@. The %%Replace limit% parameter limits the number of "
	"replaces that may occur within each label.")
MAN_END

MAN_BEGIN ("TableOfReal: Change column labels...", "djmw", 20010822)
INTRO ("Changes the column labels of the selected @TableOfReal object according "
	"to the specification in the search and replace fields.")
NORMAL ("Both search and replace fields may contain @@regular expressions|"
	"Regular expressions@. The %%Replace limit% parameter limits the number of "
	"replaces that may occur within each label.")
MAN_END

MAN_BEGIN ("TableOfReal: Draw biplot...", "djmw", 20020603)
INTRO ("A command to draw a biplot for each column in the selected "
	"@TableOfReal object.")
ENTRY ("Arguments")
TAG ("%%Xmin%, %%Xmax%, %%Ymin%, %%Ymax%")
DEFINITION ("determine the drawing boundaries.")
TAG ("%Split factor")
DEFINITION ("determines the weighing of the row and column structure "
	"(see below).")
ENTRY ("Behaviour")
LIST_ITEM ("1. Get the @@singular value decomposition@ #U #\\Si #V\\'p of the "
	"table.")
LIST_ITEM ("2. Calculate weighing factors %\\la for row and columns")
FORMULA ("%\\la__r,1_ = %\\si__1_^^%splitFactor^")
FORMULA ("%\\la__c,1_ = %\\si__1_^^1-%splitFactor^")
FORMULA ("%\\la__r,2_ = %\\si__2_^^%splitFactor^")
FORMULA ("%\\la__c,2_ = %\\si__2_^^1-%splitFactor^")
DEFINITION ("where %\\si__1_ and %\\si__2_ are the first and the second singular values")
LIST_ITEM ("3. For the rows (%i from 1..%numberOfRows) form:")
FORMULA ("%xr__%i_ = %U__%i1_  %\\la__%r,1_")
FORMULA ("%yr__%i_ = %U__%i2_  %\\la__%r,2_")
LIST_ITEM ("4. For the columns (%i from 1..%numberOfColumns) form:")
FORMULA ("%xc__%i_ = %V__%i1_  %\\la__%c,1_")
FORMULA ("%yc__%i_ = %V__%i2_  %\\la__%c,2_")
LIST_ITEM ("5. Plot the points (%xr__%i_, yr__%i_) and (%xc__%i_, yc__%i_) in the "
	"same figure with the corresponding row and column labels.")
MAN_END

MAN_BEGIN ("TableOfReal: Draw box plots...", "djmw", 20000523)
INTRO ("A command to draw a @@box plot@ for each column in the selected "
	"@TableOfReal object.")
ENTRY ("Arguments")
TAG ("%%From row%, %%To row%, %%From column%, %%To column%")
DEFINITION ("determine the part of the table that you want to analyse.")
TAG ("%Ymin and %Ymax")
DEFINITION ("determine the drawing boundaries.")
MAN_END

MAN_BEGIN ("TableOfReal: Draw rows as histogram...", "djmw", 20030619)
INTRO ("A command to draw a histogram from the rows in the selected "
	"@TableOfReal object.")
NORMAL ("The histogram will consist of %groups of bars. The number of groups will "
	"be determined by the number of selected columns from the table, while the "
	"number of bars within each group will be determined from the number of "
	"selected rows.")
ENTRY ("Arguments")
TAG ("%%Row numbers% and %%Column range%,")
DEFINITION ("determine the part of the table that you want to draw. "
	"The column range determines the number of bars that you want to draw for "
	"each row selected by the %%Row numbers% argument.")
TAG ("%Ymin and %Ymax")
DEFINITION ("the drawing boundaries.")
NORMAL ("The following arguments are all relative to the width of a bar "
	"in the histogram. ")
TAG ("%%Horizontal offset%")
DEFINITION ("the offset from the left and right margin.")
TAG ("%%Distance between bar groups%")
DEFINITION ("the distance between each group, i.e., the distance "
	"between the right side of the last bar in a group to the left side of "
	"the first bar in the next group.")
TAG ("%%Distance between bars%")
DEFINITION ("the distance between the bars in a group.")
TAG ("%%Grey values%")
DEFINITION ("the grey values of the bars in a group.")
ENTRY ("Bar positioning")
NORMAL ("If you want to put the labels yourself you will need the following information.")
NORMAL ("The width of a bar is determined as follows:")
FORMULA ("%width = 1 / (%nc \\.c %nr + 2 \\.c %hoffset + (%nc - 1)\\.c %intergroup +"
		"%nc\\.c(%nr -1)\\.c %interbar),")
NORMAL ("where %nc is the number of columns (groups) to draw, %nr is the number of "
	"rows to draw (the number of bars within a group), %hoffset is the horizontal "
	"offset, %intergroup the distance between each group and %interbar "
	"the distance between the bars within a group.")
NORMAL ("The spacing between the bars drawn from a row:")
FORMULA ("%dx = (%intergroup + %nr + (%nr -1) \\.c %interbar) *% width")
NORMAL ("The first bar for the %k-th row starts at:")
FORMULA ("%x1 = %hoffset \\.c %width + (%k - 1) \\.c (1 + %interbar) \\.c %width")
MAN_END

MAN_BEGIN ("TableOfReal: Select columns where row...", "djmw", 20020502)
INTRO ("Copy columns from the selected @TableOfReal object to a new "
	"TableOfReal object.")
ENTRY ("Arguments")
TAG ("%Columns")
DEFINITION ("defines the indices of the columns to be selected. Ranges can be "
	"defined with a colon \":\". Columns will be selected in the specified "
	"order.")
TAG ("%%Row condition")
DEFINITION ("specifies a condition for the selection of rows. If the "
	"condition evaluates as %true for a particular row, the selected elements "
	"in this row will be copied. See @@Matrix: Formula...@ for the kind of "
	"expressions that can be used here.")
ENTRY ("Examples")
CODE ("Select columns where row... \"1 2 3\" 1")
CODE ("Select columns where row... \"1 : 3\" 1")
NORMAL ("Two alternative expressions to copy the first three columns to a new table "
	"with the same number of rows.")
CODE ("Select columns where row... \"3 : 1\" 1")
NORMAL ("Copy the first three columns to a new table with the same number of "
	"rows. The new table will have the 3 columns reversed.")
CODE ("Select columns where row... \"1:6 9:11\" self[row,8]>0")
NORMAL ("Copy the first six columns and columns 9, 10, and 11 to a new table. "
	"Copy only elements from rows where the element in column 8 is greater "
	"than zero.") 
MAN_END

MAN_BEGIN ("TableOfReal: Standardize columns", "djmw", 19990428)
INTRO ("Standardizes each column of the selected @TableOfReal.")
NORMAL ("The entries %x__%ij_ in the TableOfReal will change to:")
FORMULA ("(%x__%ij_ \\-- %\\mu__%j_) / %\\si__%j_, ")
NORMAL ("where %\\mu__%j_ and %\\si__%j_ are the mean and the standard deviation as calculated "
	"from the %j^^th^ column, respectively. After standardization all column means will equal zero "
	"and all column standard deviations will equal one.")
MAN_END

MAN_BEGIN ("TableOfReal: To Configuration (lda)...", "djmw", 19981103)
INTRO ("Calculates a @Configuration based on the @Discriminant scores obtained "
	"from the selected @TableOfReal. Row labels in the table indicate group membership.")
ENTRY ("Parameters")
TAG ("%%Number of dimensions")
DEFINITION ("determines the number of dimensions of the resulting Configuration.")
ENTRY ("Algorithm")
NORMAL ("First we calculate the Discriminant from the data in the TableOfReal. "
	"See @@TableOfReal: To Discriminant@ for details.")
NORMAL ("The eigenvectors of the Discriminant determine the directions that "
	"the data in the TableOfReal will be projected unto.")
MAN_END

MAN_BEGIN ("TableOfReal: To Configuration (pca)...", "djmw", 19980909)
INTRO ("Calculates a @Configuration based on the principal components from the "
	"selected @TableOfReal.")
ENTRY ("Parameters")
TAG ("%%Number of dimensions")
DEFINITION ("determines the number of dimensions of the resulting Configuration.")
ENTRY ("Algorithm")
NORMAL ("We form principal components without explicitly calculating the covariance matrix "
	"#C = #M\\'p\\.c#M, where #M is the matrix part of the TableOfReal. ")
LIST_ITEM ("1. Make the singular value decomposition of #M. This results in "
	"#M = #U\\.c#d\\.c#V\\'p.")
LIST_ITEM ("2. Sort singular values #d and corresponding row vectors in #V (descending).")
LIST_ITEM ("3. The principalComponent__%ij_ = \\su__%k=1..%numberOfColumns_ %M__%ik_ \\.c %V__%jk_.")
ENTRY ("Remark")
NORMAL ("The resulting configuration is unique up to reflections along the new principal directions.")
MAN_END

MAN_BEGIN ("TableOfReal: To Correlation", "djmw", 20020105)
INTRO ("A command that creates a (%Pearson) @Correlation object from every "
	"selected @TableOfReal object. The correlations are calculated between "
	"columns.")
ENTRY ("Algorithm")
NORMAL ("The linear correlation coefficient %r__%ij_ (also called the %%product"
	" moment correlation coefficient% or %%Pearson's correlation coefficient%) "
	" between the elements of columns %i and %j is calculated as:")
FORMULA ("%r__%ij_ = \\Si__%k_ (%x__%ki_ - %mean__%i_)(%x__%kj_ - %mean__%j_)/"
	"(\\Vr (\\Si__%k_(%x__%ki_ - %mean__%i_)^2) \\Vr (\\Si__%k_(%x__%kj_ -"
	" %mean__%j_)^2)),")
NORMAL ("where %x__%mn_ is the element %m in column %n, and %mean__%n_ "
	"is the mean of column %n.")
MAN_END

MAN_BEGIN ("TableOfReal: To Correlation (rank)", "djmw", 20020105)
INTRO ("A command that creates a (%%Spearman rank-order%) @Correlation object "
	"from every selected @TableOfReal object. The correlations are calculated "
	"between columns.")
ENTRY ("Algorithm")
NORMAL ("The Spearman rank-order correlation coefficient %r__%ij_ between "
	"the elements of columns %i and %j is calculated as the linear correlation"
	" of the ranks:")
FORMULA ("%r__%ij_ = \\Si__%k_ (%R__%ki_ - %Rmean__%i_) "
	"(%R__%kj_ - %Rmean__%j_) / (\\Vr (\\Si__%k_(%R__%ki_ - %Rmean__%i_)^2) "
	"\\Vr (\\Si__%k_(%R__%kj_ - %Rmean__%j_)^2)),")
NORMAL ("where %R__%mn_ is the rank of element %m in column %n, "
	"and %Rmean__%n_ is the mean of the ranks in column %n.")
MAN_END

MAN_BEGIN ("TableOfReal: To Covariance", "djmw", 20020117)
INTRO ("A command that creates a @Covariance object from every "
	"selected @TableOfReal object. The covariances are calculated between "
	"columns.")
ENTRY ("Algorithm")
NORMAL ("The covariance coefficients %s__%ij_ "
	" between the elements of columns %i and %j are defined as:")
FORMULA ("%s__%ij_ = \\Si__%k_ (%x__%ki_ - %mean__%i_)(%x__%kj_ - %mean__%j_)/"
	"(%numberOfObservations - %numberOfConstraints),")
NORMAL ("where %x__%ki_ is the element %k in column %i, %mean__%i_ "
	"is the mean of column %i, %numberOfObservations equals the number of rows in "
	"the table, and %numberOfConstraints equals 1.")
NORMAL ("The actual calculation goes as follows")
LIST_ITEM ("1. Centralize each column (subtract the mean).")
LIST_ITEM ("2. Get its @@singular value decomposition@ #U #\\Si #V\\'p.")
LIST_ITEM ("3. Form #S = #V #\\Si #V\\'p.")
LIST_ITEM ("4. Divide all elements in #S by (%numberOfObservations - 1).")
MAN_END

MAN_BEGIN ("TableOfReal: To Discriminant", "djmw", 19990104)
INTRO ("A command that creates a @Discriminant object from every selected "
	"@TableOfReal object. Row labels in the table indicate group membership.")
ENTRY ("Algorithm")
NORMAL ("We solve for directions #x that are eigenvectors of the generalized "
	"eigenvalue equation:")
FORMULA ("#%B #x - %\\la #%W #x = 0,")
NORMAL ("where #%B and #%W are the between-groups and the within-groups sums of "
	"squares and cross-products matrices, respectively. Both #%B and #%W are symmetric "
	"matrices. Standard formula show that both matrices can also "
	"be written as a matrix product. The formula above then transforms to:")
FORMULA ("#%B__1_\\'p#%B__1_ #x - %\\la #%W__1_\\'p#%W__1_ #x = 0")
NORMAL ("The equation can be solved with the @@generalized singular value decomposition@. "
	"This procedure is numerically very stable and can even cope with cases when both "
	"matrices are singular.")
NORMAL ("The a priori probabilities in the Discriminant will be calculated from the number of "
	"%training vectors %n__%i_ in each group:")
FORMULA ("%aprioriProbability__%i_ = %n__%i_ / \\Si__%k=1..%numberOfGroups_ %n__%k_")
MAN_END

MAN_BEGIN ("TableOfReal: To PCA", "djmw", 19980106)
INTRO ("A command that creates a @PCA object from every selected "
	"@TableOfReal object.")
MAN_END

MAN_BEGIN ("TableOfReal: To SSCP...", "djmw", 19990218)
INTRO ("Calculates Sums of Squares and Cross Products (@SSCP) from the selected @TableOfReal.")
ENTRY ("Algorithm")
NORMAL ("The sums of squares and cross products %s__%ij_ "
	" between the elements of columns %i and %j are calculated as:")
FORMULA ("%s__%ij_ = \\Si__%k_ (%x__%ki_ - %mean__%i_)(%x__%kj_ - %mean__%j_),")
NORMAL ("where %x__%mn_ is the element %m in column %n and %mean__%n_ "
	"is the mean of column %n.")
MAN_END

MAN_BEGIN ("TableOfReal: To Pattern and Categories...", "djmw", 20040429)
INTRO ("Extracts a @Pattern and a @Categories from the selected @TableOfReal.")
NORMAL ("The selected rows and columns are copied into the Pattern and "
	"the corresponding row labels are copied into a Categories. ")
MAN_END

MAN_BEGIN ("TableOfReal: To CCA...", "djmw", 20020424)
INTRO ("A command that creates a @CCA object from the selected "
	"@TableOfReal object.")
ENTRY ("Arguments")
TAG ("%%Dimension of dependent variate (ny)")
DEFINITION ("defines the partition of the table into the two parts whose "
	"correlations will be determined. The first %ny columns must be the "
	"dependent part, the rest of the columns will be interpreted as the "
	"independent part (%nx columns). In general %nx must be larger than or "
	"equal to %ny.")
ENTRY ("Behaviour")
NORMAL ("Calculates canonical correlations between the %dependent and the "
	"%independent parts of the table. The corresponding "
	"canonical coefficients are also determined.")
ENTRY ("Algorithm")
NORMAL ("The canonical correlation equations for two data sets #T__%y_ "
	"[%n \\xx %p] and #T__%x_ [n \\xx %q] are:")
FORMULA ("(1)    (#S__%yx_ #S__%xx_^^-1^ #S__%yx_\\'p -\\la #S__%yy_)#y = #0")
FORMULA ("(2)    (#S__%yx_\\'p #S__%yy_^^-1^ #S__%yx_ -\\la #S__%xx_)#x = #0")
NORMAL ("where #S__%yy_ [%p \\xx %p] and #S__%xx_ [%q \\xx %q] are the "
	"covariance matrices of data sets #T__%y_ and  #T__%x_, respectively, "
	"#S__%yx_ [%p \\xx %q] is the matrix of covariances between data sets "
	"#T__%y_ and #T__%x_, and the vectors #y and #x are the %%canonical "
	"weights% or the %%canonical function coefficients% for the dependent and "
	"the independent data, respectively. "
	"In terms of the (dependent) data set #T__%y_ and the (independent) data set "
	"#T__%x_, these covariances can be written as:")
FORMULA ("#S__%yy_ =  #T__%y_\\'p #T__%y_,  #S__%yx_ = #T__%y_\\'p #T__%x_ and "
	"#S__%xx_ =  #T__%x_\\'p #T__%x_.")
NORMAL ("The following @@singular value decomposition@s ")
FORMULA ("#T__%y_ = #U__%y_ #D__%y_ #V__%y_\\'p and #T__%x_ = #U__%x_ #D__%x_ "
	"#V__%x_\\'p ")
NORMAL ("transform equation (1) above into:")
FORMULA ("(3)    (#V__%y_ #D__%y_ #U__%y_\\'p#U__%x_ #U__%x_\\'p #U__%y_ #D__%y_ "
	"#V__%y_\\'p - \\la #V__%y_ #D__%y_ #D__%y_ #V__%y_\\'p)#y = 0 ")
NORMAL ("where we used the fact that:")
FORMULA ("#S__%xx_^^-1^ = #V__%x_ #D__%x_^^-2^ #V__%x_\\'p.")
NORMAL ("Equation (3) can be simplified by multiplication from the left by "
	"#D__%y_^^-1^ #V__%y_' to:")
FORMULA (" (4)   ((#U__%x_\\'p #U__%y_)\\'p (#U__%x_\\'p #U__%y_) - \\la #I)#D__%y_ "
	"#V__%y_\\'p #y = #0")
NORMAL ("This equation can, finally, be solved by a substitution of the s.v.d "
	"of  #U__%x_\\'p #U__%y_ = #U #D #V\\'p  into (4). This results in")
FORMULA ("(5)  (#D^^2^ - \\la #I) #V\\'p #D__%y_ #V__%y_\\'p #y = #0")
NORMAL ("In an analogous way we can reduce eigenequation (2) to:")
FORMULA ("(6)  (#D^^2^ - \\la #I) #U\\'p #D__%x_ #V__%x_\\'p #x = #0")
NORMAL ("From (5) and (6) we deduce that the eigenvalues in both equations "
	"are equal to the squared singular values of the product matrix "
	"#U__%x_\\'p#U__%y_. "
	"These singular values are also called %%canonical "
	"correlation coefficients%. The eigenvectors #y and #x can be obtained "
	"from the columns of the following matrices #Y and #X:")
FORMULA ("#Y = #V__%y_ #D__%y_^^-1^ #V")
FORMULA ("#X = #V__%x_ #D__%x_^^-1^ #U")
NORMAL ("For example, when the vector #y equals the first column of #Y and "
	"the vector #x equals "
	"the first column of #X, then the vectors #u = #T__%y_#y and #v = #T__%x_#x "
	"are the linear combinations from #T__%y_ and #T__%x_ that have maximum "
	"correlation. Their correlation coefficient equals the first canonical "
	"correlation coefficient.")
MAN_END

MAN_BEGIN ("TableOfReal: To TableOfReal (means by row labels)...", "djmw", 20050221)
INTRO ("A command that appears in the ##Multivariate statistics# menu if you select a @@TableOfReal@. "
	"It calculates the multivariate means for the different row labels from the selected TableOfReal.")
ENTRY ("Argument")
TAG ("%%Expand")
DEFINITION ("when %off, then for a table with %n rows and %m different labels (%m\\<_%n), the resulting table will have %m rows. "
	"When %on, the dimensions of the resulting table will be the same as the originating, and corresponding means substituded "
	"in each row.")
ENTRY ("Example")
NORMAL ("The following commands")
CODE ("@@Create TableOfReal (Pols 1973)...@ 0")
CODE ("To TableOfReal (means by row labels)... 0")
NORMAL ("will result in a new TableOfReal that has 12 rows. Each row will contain the mean F1, F2 and F3 values for a particular vowel. These means "
	" were obtained from 50 representations of that vowel.")
NORMAL ("If we had chosen the %expansion:")
CODE ("To TableOfReal (means by row labels)... 1")
NORMAL ("the resulting TableOfReal would have had 600 rows. This representation  comes in handy when, for example, you have to calculate deviations from the mean.")
MAN_END

MAN_BEGIN ("TextGrid: Extend time...", "djmw", 20020702)
INTRO ("Extends the domain of the selected @TextGrid object.")
ENTRY ("Arguments")
TAG ("%%Extend domain by")
DEFINITION ("defines the amount of time by which the domain will be extended.")
TAG ("%At")
DEFINITION ("defines whether starting times or finishing times will be "
	"modified.")
ENTRY ("Behaviour")
NORMAL ("We add an extra (empty) interval into each %%interval tier%. "
	"This is necessary to keep original intervals intact. According to the "
	"value of the second argument, the new interval will be added at the "
	"beginning or at the end of the tier.")
NORMAL ("For %%point tiers% only the domain will be changed.")
MAN_END

MAN_BEGIN ("TIMIT acoustic-phonetic speech corpus", "djmw", 19970320)
INTRO ("A large American-English speech corpus that resulted from the joint efforts "
	"of several American research sites.")
NORMAL ("The TIMIT corpus contains a total of 6300 sentences, 10 sentences spoken by "
	"630 speakers selected from 8 major dialect regions of the USA. 70\\%  of "
	"the speakers are male, 30\\%  are female.")
NORMAL ("The text corpus design was done by the Massachusetts Institute of "
	"Technology (MIT), Stanford Research Institute and Texas Instruments (TI). "
	"The speech was recorded at TI, transcribed at MIT, and has been maintained, "
	"verified and prepared for CDROM production by the American National Institute "
	"of Standards and Technology (NIST) (@@Lamel et al. (1986)@).")
MAN_END

/********************* References **************************************/

MAN_BEGIN ("Bai & Demmel (1993)", "djmw", 19981007)
NORMAL ("Z. Bai & J. Demmel (1993), \"Computing the generalized singular value "
	"decomposition\", %%SIAM J. Sci. Comput.% #14, 1464-1486.")
MAN_END

MAN_BEGIN ("Bartlett (1954)", "djmw", 20011111)
NORMAL ("M.S. Bartlett(1954), \"A note on multiplying factors for various "
	"chi-squared approximations\", %%Joural of the Royal Statistical Society, "
	"Series B%, vol. 16, 296-298")
MAN_END

MAN_BEGIN ("Boomsma (1977)", "djmw", 20020524)
NORMAL ("A. Boomsma (1977), \"Comparing approximations of confidence intervals "
	"for the product-moment correlation coefficient\", %%Statistica Neerlandica% "
	"#31, 179-186.")
MAN_END

MAN_BEGIN ("Cooley & Lohnes (1971)", "djmw", 20060322)
NORMAL ("W.W. Colley & P.R. Lohnes (1971), %%Multivariate data analysis%, "
	"John Wiley & Sons.")
MAN_END

MAN_BEGIN ("Davis & Mermelstein (1980)", "djmw", 20010419)
NORMAL ("S.B. Davis & P. Mermelstein (1980), \"Comparison of parametric "
	"representations for monosyllabic word recognition in continuously "
	"spoken sentences\", "
	"%%IEEE Trans. on ASSP% #28, 357-366.")
MAN_END

MAN_BEGIN ("Efron & Tibshirani (1993)", "djmw", 20031103)
NORMAL ("B. Efron & R.J. Tibshirani (1993), %%An introduction "
	"to the bootstrap%, Chapman & Hall.")
MAN_END

MAN_BEGIN ("Flanagan (1960)", "djmw", 19980713)
NORMAL ("J.L. Flanagan (1960), \"Models for approximating basilar membrane "
	"displacement\", %%Bell Sys. Tech. J.% #39, 1163-1191.")
MAN_END

MAN_BEGIN ("Friedl (1997)", "djmw", 20010710)
NORMAL ("J.E.F. Friedl (1997), %%Mastering Regular Expressions%, "
	"O'Reilly & Associates.")
MAN_END

MAN_BEGIN ("Heath et al. (1986)", "djmw", 19981007)
NORMAL ("M.T. Heath, J.A. Laub, C.C. Paige & R.C. Ward (1986), \"Computing the "
	"singular value decomposition of a product of two matrices\", "
	"%%SIAM J. Sci. Statist. Comput.% #7, 1147-1159.")
MAN_END

MAN_BEGIN ("Hermes (1988)", "djmw", 19980123)
NORMAL ("D.J. Hermes (1988), \"Measurement of pitch by subharmonic "
	"summation\", %%J.Acoust.Soc.Am.% #83, 257-264.")
MAN_END

MAN_BEGIN ("Irino & Patterson (1996)", "djmw", 19980123)
NORMAL ("T. Irino & R.D. Patterson (1996), \"A time-domain, level-dependent "
	"auditory filter: The gammachirp\", %%J.Acoust.Soc.Am.% #101, 412-419.")
MAN_END

MAN_BEGIN ("Johannesma (1972)", "djmw", 19980123)
NORMAL ("P.I.M. Johannesma (1972): \"The pre-response stimulus ensemble of "
	"neurons in the cochlear nucleus\", in %%Symposium on Hearing Theory% "
	"(IPO, Eindhoven, Holland), 58-69.")
MAN_END

MAN_BEGIN ("Johnson (1998)", "djmw", 20000525)
NORMAL ("D.E. Johnson (1998), %%Applied Multivariate methods%")
MAN_END

MAN_BEGIN ("Lamel et al. (1986)", "djmw", 19980123)
NORMAL ("L.F. Lamel, R.H. Kassel & S. Sennef (1986): \"Speech Database "
	"Development: Design and Analysis of the Acoustic-Phonetic Corpus\", "
	"%%Proc. DARPA Speech Recognition Workshop%\", Report No. SAIC-86/1546, "
	"100-19.")
MAN_END

MAN_BEGIN ("Morrison (1990)", "djmw", 19980123)
NORMAL ("D.F. Morrison (1990), %%Multivariate Statistical Methods%, "
	"McGraw-Hill, New York.") 
MAN_END

MAN_BEGIN ("Peterson & Barney (1952)", "djmw", 20020620)
NORMAL ("G.E. Peterson & H.L. Barney (1952), \"Control methods used in a study "
	"of the vowels\", %%J.Acoust.Soc.Am.% #24, 175-184")
MAN_END

MAN_BEGIN ("Pols et al. (1973)", "djmw", 19990426)
NORMAL ("L.C.W. Pols, H.R.C. Tromp & R. Plomp (1973), "
	"\"Frequency analysis of Dutch vowels from 50 male speakers\", "
	"%%J.Acoust.Soc.Am.% #53, 1093-1101.")
MAN_END

MAN_BEGIN ("Press et al. (1992)", "djmw", 19980114)
NORMAL ("W.H. Press, S.A. Teukolsky, W.T. Vetterling & B.P. Flannery (1992), "
	"%%Numerical Recipes in C: the art of scientific computing%, "
	"Second Edition, Cambridge University Press.")
MAN_END

MAN_BEGIN ("Sakoe & Chiba (1978)", "djmw", 20050302)
NORMAL ("H. Sakoe & S. Chiba (1978), \"Dynamic programming algorithm optimization for spoken word recognition\", "
	"%%Trans. on ASSP% #26, 43-49.")
MAN_END

MAN_BEGIN ("Shepard (1964)", "djmw", 19980114)
NORMAL ("R.N. Shepard (1964), \"Circularity in Judgments of Relative Pitch\", "
	"%%J.Acoust.Soc.Am.% #36, 2346-2353.")
MAN_END

MAN_BEGIN ("Slaney (1993)", "djmw", 19980712)
NORMAL ("M. Slaney (1993), \"An efficient implementation of the "
	"Patterson-Holdsworth auditory filterbank\", " 
	"%%Apple Computer Technical Report% #35, 41 pages.")
MAN_END

MAN_BEGIN ("Tribolet et al. (1979)", "djmw", 20010114)
NORMAL ("J.M. Tribolet & T.F. Quatieri (1979), \"Computation of the Complex "
	"Cepstrum\", in: %%Programs for Digital Signal Processing%, "
	"Digital Signal Processing Committee (eds.), IEEE Press.")
MAN_END

MAN_BEGIN ("Tukey (1977)", "djmw", 20000524)
NORMAL ("J.W. Tukey (1977), %%Exploratory data analysis%, "
	"Addison-Wesley, Reading, Mass.")
MAN_END

MAN_BEGIN ("Van Nierop et al. (1973)", "djmw", 20020620)
NORMAL ("D.J.P.J. Van Nierop, L.C.W. Pols & R. Plomp (1973), \"Frequency "
	"analysis of Dutch vowels from 25 female speakers\", %%Acustica% #29, 110-118")
MAN_END

MAN_BEGIN ("Weenink (1985)", "djmw", 20040225)
NORMAL ("D.J.M. Weenink (1999), \"Formant analysis of Dutch vowels from 10 children\", "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #25, 81-99.")
MAN_END

MAN_BEGIN ("Weenink (1985)", "djmw", 20041217)
NORMAL ("D.J.M. Weenink (1985), \"Accurate algorithms for performing "
 		"principal component analysis  and discriminant analysis\", "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #19, 45-52.")
MAN_END

MAN_BEGIN ("Weenink (2003)", "djmw", 20040225)
NORMAL ("D.J.M. Weenink (1999), \"Canonical correlation analysis\", "
		"%%Proceedings of the Institute of Phonetic Sciences of the "
		"University of Amsterdam% #25, 81-99.")
MAN_END

}

/* End of file manual_dwtools.c */
