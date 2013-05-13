/* manual_MDS.cpp
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
 djmw 20020422 GPL + removed "C syntax" part of manpage
 djmw 20030317 Latest modification.
 djmw 20030825 Spelling corrections.
 djmw 20040118 Procrustes modifications.
 djmw 20040214 Removed last link to @praat program@.
 djmw 20040407 Change Arguments -> Settings.
 djmw 20040513 Spelling.
 djmw 20070108 Latest modification.
*/

#include "ManPagesM.h"
#include "MDS.h"

static void drawLetterRConfigurationExample (Graphics g)
{
	autoConfiguration me = Configuration_createLetterRExample (1);
	Graphics_setWindow (g, -6, 4, -7, 5);
	Configuration_draw (me.peek(), g, 1, 2, -6, 4, -7, 5, 0, 1, L"", 1);
}

static void drawLetterRConfigurationExample2 (Graphics g)
{
	autoConfiguration me = Configuration_createLetterRExample (2);
	Configuration_draw (me.peek(), g, 1, 2, -0.8, 1.2, -0.8, 0.7, 0, 1, L"", 1);
}

static void drawLetterRShepard (Graphics g)
{
	autoDissimilarity d = Dissimilarity_createLetterRExample (32.5);
	autoConfiguration c = Configuration_createLetterRExample (2);
	Dissimilarity_Configuration_drawShepardDiagram (d.peek(), c.peek(), g, 0, 200, 0, 2.2, 1, L"+", 1);
}

static void drawLetterRRegression (Graphics g)
{
	autoDissimilarity d = Dissimilarity_createLetterRExample (32.5);
	autoConfiguration c = Configuration_createLetterRExample (2);
	Dissimilarity_Configuration_drawMonotoneRegression (d.peek(), c.peek(), g, MDS_PRIMARY_APPROACH, 0, 200, 0, 2.2, 1, L"+", 1);
}

static void drawCarrollWishConfigurationExample (Graphics g)
{
	autoConfiguration me = Configuration_createCarrollWishExample ();
	Graphics_setWindow (g, -2, 2, -2, 2);
	Configuration_draw (me.peek(), g, 1, 2, -2, 2, -2, 2, 0, 1, L"", 1);
}

static void drawCarrollWishSalienceExample (Graphics g)
{
	autoSalience me = Salience_createCarrollWishExample ();
	Salience_draw (me.peek(), g, 1, 2, 1);
}

static void drawMsplineExample (Graphics g)
{
	drawSplines (g, 0, 1, 0, 10, 1, 3, L"0.3 0.5 0.6", 1);
}

static void drawIsplineExample (Graphics g)
{
	drawSplines (g, 0, 1, 0, 1.5, 2, 3, L"0.3 0.5 0.6", 1);
}

void manual_MDS_init (ManPages me);
void manual_MDS_init (ManPages me)
{

MAN_BEGIN (L"CANDECOMP", L"djmw", 19971201)
ENTRY (L"An algorithm to solve the INDSCAL problem.")
NORMAL (L"In the analysis of the INDSCAL three-way data matrix (%numberOfPoints "
	"\\xx %numberOfDimensions \\xx %numberOfSources) we seek to minimize the "
	"function: ")
FORMULA (L"%f(%X, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%i=1..%numberOfSources_ | %S__%i_ \\-- %X%W__%i_%X\\'p |^2")
NORMAL (L"where %S__%i_ is a known symmetric %numberOfPoints \\xx %numberOfPoints "
	"matrix with scalar products of distances for source %i, %X is the unknown configuration "
	"%numberOfPoints \\xx %numberOfDimensions matrix, %X\\'p its transpose, and, %W__%i_ is "
	"the diagonal %numberOfDimensions \\xx %numberOfDimensions weight matrix for source %i. The function "
	"above has no analytical solution for %X and the %W__%i_. It can be solved, however, "
	"by an iterative procedure which Carroll & Chang have christened CANDECOMP "
	"(CANonical DECOMPosition).  This method minimizes, instead of the function "
	"given above, the following function:")
LIST_ITEM (L"%g(%X, %Y, %W__1_,..., %W__%numberOfSources_) = \\su__%i=1..%numberOfSources_ "
	"| %S__%i_ \\-- %X%W__%i_%Y\\'p |^2")
NORMAL (L"where %X and %Y are both %numberOfPoints \\xx %numberOfDimensions configuration matrices.")
NORMAL (L"The algorithm proceeds as follows:")
NORMAL (L"1. Initialize the $W matrices and the configuration matrix %X. This can for example be "
	"done according to a procedure given in @@Young, Takane & Lewyckyj (1978)@.")
NORMAL (L"2. An alternating least squares minimization process is started as described that "
	"sequentially updates %Y, %X an %W (@@Carroll & Chang (1970)@):")
LIST_ITEM (L"2.1. Solve for a new %Y given %X and the %W__%i_")
LIST_ITEM (L"2.2. Solve for a new %X given the %W__%i_ and the new %Y.")
LIST_ITEM (L"2.3. Solve for the %W__%i_ given the new %X and %Y.")
NORMAL (L"Evaluate the goodness-of-fit criterion and either repeat the minimization sequence "
	"(2.1\\--2.3) or continue.")
NORMAL (L"3. Done: make %Y equal to %X and solve a last time for the %W__%i_.")
NORMAL (L"Note: during the minimization the following constraints are effective:")
LIST_ITEM (L"The configuration must be centered.")
LIST_ITEM (L"The sum of squared coordinates in the configuration space is one for "
	"each dimension, i.e., the configuration always has unit variance in each dimension.")
MAN_END

MAN_BEGIN (L"Configuration", L"djmw", 20101102)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Configuration represents the positions of a number "
	"of labelled points in a multidimensional space.")
ENTRY (L"How to create a Configuration")
NORMAL (L"From the New menu:")
LIST_ITEM (L"  \\bu @@Create Configuration...")
NORMAL (L"By multidimensional scaling:")
LIST_ITEM (L"  \\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity: To Configuration (absolute mds)...")
NORMAL (L"By multidimensional scaling with weights (@@Dissimilarity & Weight: To Configuration...@):")
LIST_ITEM (L"  \\bu ##Dissimilarity & Weight: To Configuration (monotone mds)...")
LIST_ITEM (L"  \\bu ##Dissimilarity & Weight: To Configuration (i-spline mds)...")
LIST_ITEM (L"  \\bu ##Dissimilarity & Weight: To Configuration (interval mds)...")
LIST_ITEM (L"  \\bu ##Dissimilarity & Weight: To Configuration (ratio mds)...")
LIST_ITEM (L"  \\bu ##Dissimilarity & Weight: To Configuration (absolute mds)...")
NORMAL (L"By multidimensional scaling with a start Configuration:")
LIST_ITEM (L"  \\bu @@Dissimilarity & Configuration: To Configuration (monotone mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity & Configuration: To Configuration (i-spline mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity & Configuration: To Configuration (interval mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity & Configuration: To Configuration (ratio mds)...")
LIST_ITEM (L"  \\bu @@Dissimilarity & Configuration: To Configuration (absolute mds)...")
NORMAL (L"By transforming an existing Configuration:")
LIST_ITEM (L"  \\bu @@Configuration: To Configuration (varimax)...")
LIST_ITEM (L"  \\bu @@Configuration & AffineTransform: To Configuration")
LIST_ITEM (L"  \\bu @@Configuration & Procrustes: To Configuration")
NORMAL (L"From @@Principal component analysis@:")
LIST_ITEM (L"  \\bu @@TableOfReal: To Configuration (pca)...")
LIST_ITEM (L"  \\bu @@PCA & TableOfReal: To Configuration...")
NORMAL (L"From @@Discriminant analysis@:")
LIST_ITEM (L"@@TableOfReal: To Configuration (lda)...")
LIST_ITEM (L"  \\bu @@Discriminant & TableOfReal: To Configuration...")
ENTRY (L"How to draw a Configuration")
LIST_ITEM (L"\\bu @@Configuration: Draw...")
LIST_ITEM (L"\\bu ##Configuration: Draw as numbers...")
LIST_ITEM (L"\\bu ##Configuration: Draw as squares...")
ENTRY (L"How to modify a Configuration")
LIST_ITEM (L"\\bu @@Configuration: Randomize")
LIST_ITEM (L"\\bu @@Configuration: Rotate (pc)@ (to principal directions)")
LIST_ITEM (L"\\bu @@Configuration: Rotate...@ (in a plane around the origin)")
LIST_ITEM (L"\\bu @@Configuration: Invert dimension...")
LIST_ITEM (L"\\bu @@Configuration: Normalize...")
ENTRY (L"Inside a Configuration")
NORMAL (L"With @Inspect you will see the following attributes:")
TAG (L"%numberOfRows")
DEFINITION (L"the number of points (%numberOfPoints\\>_1).")
TAG (L"%numberOfColumns")
DEFINITION (L"the dimension of the space (%numberOfDimensions\\>_1).")
TAG (L"%rowLabels")
DEFINITION (L"the names associated with the points.")
TAG (L"%columnLabels")
DEFINITION (L"the names for the dimensions.")
TAG (L"%data [1..%numberOfPoints][1..%numberOfDimensions]")
DEFINITION (L"the coordinates of the points.")
TAG (L"%metric")
DEFINITION (L"determines the way distances between points are measured. In general "
	"the distance between points #x__%i_ and #x__%j_ is:")
FORMULA (L"%d__%ij_ = "
	"(\\su__%k=1..%numberOfDimensions_ %w__%k_ |%x__%ik_ \\-- "
	"%x__%jk_|^^%metric^)^^1/%metric^")
DEFINITION (L"For Euclidean distances %metric is 2.")
TAG (L"%w [1..%numberOfDimensions]")
DEFINITION (L"weight given to each dimension in the distance calculation.")
MAN_END

MAN_BEGIN (L"Configuration: Centralize", L"djmw", 19980413)
INTRO (L"Makes the centre of the selected @Configuration equal to the origin.")
NORMAL (L"")
MAN_END

MAN_BEGIN (L"Configuration: Draw...", L"djmw", 20040407)
INTRO (L"Draws a projection of the selected @Configuration on a coordinate plane.")
ENTRY (L"Settings")
TAG (L"##X-coordinate#, ##Y-coordinate#")
DEFINITION (L"control the dimensions that will show in the plot.")
TAG (L"##xmin#, ##xmax#; ##ymin#, ##ymax#")
DEFINITION (L"range for horizontal and vertical axes, respectively.")
TAG (L"##Garnish")
DEFINITION (L"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (L"Configuration: Invert dimension...", L"djmw", 20040407)
INTRO (L"Inverts one dimension of a @Configuration.")
NORMAL (L"Setting")
TAG (L"##Dimension")
DEFINITION (L"the dimensions that has to be inverted.")
ENTRY (L"Behaviour")
NORMAL (L"For all points %i=1..%numberOfPoints: if %j == %dimension then %x__%ij_ "
	"= \\--%x__%ij_.")
MAN_END

MAN_BEGIN (L"Configuration: Normalize...", L"djmw", 20040407)
INTRO (L"Normalizes the selected @Configuration.")
ENTRY (L"Settings")
TAG (L"##Sum of squares# (standard value: 0.0)")
DEFINITION (L"The desired value for the variance.")
TAG (L"##Each dimension separately")
DEFINITION (L"When on, the sum of squares in each dimension (column) will be scaled to %sumOfSquares "
	"When off, the sum of squares of all the matrix elements will equal %sumOfSquares.")
NORMAL (L"With the default value (0.0) for %sumOfSquares, and %eachDimensionSeparately chosen, "
	"an INDSCAL-like normalization is applied: the sum of squares for each column is scaled to "
	"equal 1.0. When %eachDimensionSeparately is off, a Kruskal-like normalization is applied: "
	"the sum of squares of the whole matrix is scaled equal to %numberOfRows.")
ENTRY (L"Behaviour")
NORMAL (L"Before the normalization will be applied, however, we first translate the centre of the "
	"configuration to the origin by subtracting the mean for each dimension. "
	"The sum of squares than equals variance.")
MAN_END

MAN_BEGIN (L"Configuration: Randomize", L"djmw", 19971201)
INTRO (L"Changes all coordinates of the points #%x__%i_ in the @Configuration according to:")
LIST_ITEM (L"%x__%ij_ = randomUniform (-1, 1)")
MAN_END

MAN_BEGIN (L"Configuration: Rotate...", L"djmw", 20100303)
INTRO (L"Rotates the @Configuration in a plane around the origin.")
NORMAL (L"Settings")
TAG (L"##Dimension 1#, ##Dimension 2#")
DEFINITION (L"the dimensions that span the plane. The order of dimension 1 and dimension 2 is not important: "
	"the lowest number always determines the first dimension.")
TAG (L"##Angle")
DEFINITION (L"the counter-clockwise rotation angle in degrees.")
MAN_END

MAN_BEGIN (L"Configuration: Rotate (pc)", L"djmw", 19971201)
INTRO (L"Rotates the @Configuration to principal directions. The principal directions "
	"correspond to the principal components.")
MAN_END

MAN_BEGIN (L"Configuration: To Configuration (procrustes)", L"djmw", 19971219)
INTRO (L"A command that transforms the second selected @Configuration object "
	"to match the first selected Configuration object as closely as possible."
	"This problem of fitting one configuration (testee) to another (target) "
	"as closely as possible is called the Procrustes problem. We use a "
	"special @@Procrustes transform@ algorithm that does not "
	"mutilate or distort the testee configuration.")
NORMAL (L"Both Configuration objects must have the same dimensions.")
MAN_END

MAN_BEGIN (L"Configuration: To Configuration (varimax)...", L"djmw", 20040407)
INTRO (L"A command that rotates the selected @Configuration object to a new "
	"Configuration object whose coordinates have maximum %squared variance. ")
ENTRY (L"Settings")
TAG (L"##Normalize rows")
DEFINITION (L"when selected, the distances of all points to the origin will "
	"be made equal before iteration starts. We remember these scale factors "
	"and restore the original distances after the iteration process has "
	"stopped.")
TAG (L"##Quartimax")
DEFINITION (L"when selected, the sum of fourth powers, normalized or raw, "
	"will be maximized. ")
TAG (L"##Maximum number of iterations")
DEFINITION (L"sets a limit to the number of iterations. One iteration consists "
	"of %numberOfDimensions\\.c (%numberOfDimensions\\--1)/2 planar rotations "
	"of all pairs of dimensions.")
TAG (L"##Tolerance")
DEFINITION (L"also determines when the iteration stops. This happens if "
	"|%v__%i_\\--%v__%i+1_| < %tolerance \\.c %v__%i_, where %v__%i_ is the "
	"squared variance for the %i^^th^ iteration.")
NORMAL (L"The iteration process stops when either the %%maximum number of "
	"iterations% is reached or the %tolerance criterion is met, which ever "
	"one is first.")
ENTRY (L"Algorithm")
NORMAL (L"The Varimax rotation procedure was first proposed by @@Kaiser "
	"(1958)@. Given a %numberOfPoints \\xx %numberOfDimensions configuration "
	"#A, the procedure tries to find an orthonormal rotation matrix #T such "
	"that the sum of variances of the columns of #B*#B is a maximum, where #B "
	"= #A#T and * is the element wise (Hadamard) product of matrices. A direct "
	"solution for the optimal #T is not available, except for the case when "
	"%numberOfDimensions equals two. Kaiser suggested an iterative "
	"algorithm based on planar rotations, i.e., alternate rotations of all "
	"pairs of columns of #A.")
NORMAL (L"However, this procedure is not without problems: the varimax function "
	"may have stationary points that are not even local maxima. We have "
	"incorporated an algorithm of @@Ten Berge (1995)@ "
	"that prevents this unpleasant situation from happening.")
MAN_END

MAN_BEGIN (L"Configuration: To Distance", L"djmw", 19971207)
INTRO (L"A command that computes a @Distance object for each selected "
	"@Configuration.")
ENTRY (L"Algorithm")
NORMAL (L"The distance %d__%ij_ between objects %i and %j is calculated as:")
FORMULA (L"%d__%ij_ = %d__%ji_ = (\\su__%k=1..%numberOfDimensions_ |%x__%ik_ "
	"\\-- %x__%jk_|^2)^^1/2^")
MAN_END

MAN_BEGIN (L"Configuration: To Similarity (cc)", L"djmw", 19980130)
INTRO (L"A command that create one @Similarity object from the selected "
	"@Configuration objects.")
NORMAL (L"In the Similarity object entry %s__%ij_ equals the @@congruence "
	"coefficient@ for the %i-th and %j-th selected Configuration object.")
NORMAL (L"All Configuration objects must have the same number of points and "
	"the same dimensions.")
MAN_END

MAN_BEGIN (L"Configuration & AffineTransform: To Configuration", L"djmw", 20011008)
INTRO (L"A command that transforms the selected @Configuration to a new "
	"Configuration object according to the specifications in the selected "
	"@AffineTransform object.")
MAN_END

MAN_BEGIN (L"Configuration & Procrustes: To Configuration", L"djmw", 20011008)
INTRO (L"A command that transforms the selected @Configuration to a new "
	"Configuration object according to the specifications in the selected "
	"@Procrustes object.")
MAN_END

MAN_BEGIN (L"Configurations: To AffineTransform (congruence)...", L"djmw", 20040407)
INTRO (L"A command that creates an @AffineTransform object from two selected "
	"@Configuration objects.")
NORMAL (L"We calculate the affine transform that transforms the second "
	"selected Configuration object to match the first selected Configuration "
	"object as closely as possible. "
	"The degree of proportionality is the congruence between corresponding "
	"dimensions.")
ENTRY (L"Settings")
TAG (L"##Maximum number of iterations")
DEFINITION (L"sets a limit to the number of iterations.")
TAG (L"##Tolerance")
DEFINITION (L"also determines when the iteration stops. This happens if "
	"|%f(#T__%i_)\\--%f(#T__%i+1_)| < %tolerance \\.c %f(#T__%i_), where "
	"%f(#T__%i_) is the sum of the congruences for the %i^^th^ "
	"iteration (see below).")
NORMAL (L"The iteration process stops when either the %%maximum number of "
	"iterations% is reached or the %tolerance criterion is met, which ever "
	"one is first.")
ENTRY (L"Algorithm")
NORMAL (L"Sometimes the criterion used in a @@Procrustes transform@ is too "
	"restrictive for comparing two configurations. This criterion is only "
	"zero when the positions in the rotated configuration (#A#T) equal the "
	"positions in the other configuration (#B). @@Brokken (1983)@ proposed an "
	"algorithm to maximize instead the sum of congruences between "
	"corresponding dimensions of #AT and #B. "
	"Specifically he proposed to maximize")
FORMULA (L"%f(#T) = \\su__%i=1..%numberOfDimensions_ #t\\'p__%i_#A\\'p#b__%i_ /"
	" ((#t\\'p__%i_#A\\'p#A#t__%i_)^^1/2^(#b\\'p__%i_#b__%i_)^^1/2^),")
NORMAL (L"where #t\\'p__%i_ and #b\\'p__%i_ are the %i^^th^ column of #T and "
	"#B, respectively.  A direct solution for #T is not available, it can only "
	"be obtained by an iterative procedure. The implemented algorithm is from "
	"@@Kiers & Groenen (1996)@ and shows excellent convergence properties.")
MAN_END

MAN_BEGIN (L"Configuration & Configuration: To Procrustes...", L"djmw", 20011008)
INTRO (L"A command that creates a @Procrustes object from two selected "
	"@Configuration objects.")
ENTRY (L"Setting")
TAG (L"##Orthogonal transform")
DEFINITION (L"determines whether or not a translation and a scaling are allowed in the transform.")
NORMAL (L"We calculate the @@Procrustes transform@ that transforms the second "
	"selected Configuration object to match the first selected Configuration "
	"object as closely as possible.")
MAN_END

MAN_BEGIN (L"Confusion: To Dissimilarity...", L"djmw", 20040407)
INTRO (L"A command that creates a @Dissimilarity from every selected "
	"@Confusion.")
ENTRY (L"Settings")
TAG (L"##Normalize")
DEFINITION (L"when on, normalize rows by dividing each row element by the row "
	"sum. In this way you correct for unequal stimulus numbers.")
TAG (L"##No symmetrization#, #Average, #Houtgast")
DEFINITION (L"determine the symmetrization procedure. See "
	"@@Confusion: To Similarity...")
TAG (L"##Maximum dissimilarity")
DEFINITION (L"determines the maximum dissimilarity possible. When the default "
	"value, 0.0, is chosen, %maximumDissimilarity "
	"is calculated as the maximum element in the Similarity object.")
ENTRY (L"Algorithm")
NORMAL (L"We first transform the Confusion to a Similarity. See "
	"@@Confusion: To Similarity...")
NORMAL (L"To obtain dissimilarities from similarities we \"reverse\" the "
	"latter:")
FORMULA (L"%%dissimilarity__%ij_ = %maximumDissimilarity \\-- %similarity__%ij_")
MAN_END

MAN_BEGIN (L"Confusion: To Dissimilarity (pdf)...", L"djmw", 20040407)
INTRO (L"A command that creates a @Dissimilarity from every selected "
	"@Confusion.")
ENTRY (L"Settings")
TAG (L"##Symmetrize first")
DEFINITION (L"when on, the confusion matrix is symmetrized before we calculate "
	"dissimilarities.")
TAG (L"##Maximum dissimilarity (units of sigma)")
DEFINITION (L"specifies the dissimilarity from confusion matrix elements that "
	"are zero.")
ENTRY (L"Algorithm")
TAG (L"1. Normalize rows by dividing each row element by the row sum (optional).")
TAG (L"2. Symmetrize the matrix by averaging %f__%ij_ and %f__%ji_.")
TAG (L"3. Transformation of the confusion measure which is a sort of "
	"%similarity measure to the %dissimilarity measure.")
NORMAL (L"Similarity and dissimilarity have an inverse relationship: the "
	"greater the similarity, the smaller the dissimilarity and vice versa. "
	"Both have a monotonic relationship with distance. "
	"The most simple way to transform the similarities %f__%ij_ into "
	"dissimilarities is:")
FORMULA (L"%dissimilarity__%ij_ = %maximumSimilarity \\-- %similarity__%ij_")
NORMAL (L"For ordinal analyses like Kruskal this transformation is fine because "
	"only order relations are important in this analysis. However, for "
	"metrical analyses like INDSCAL this is not optimal. "
	"In INDSCAL, distance is a linear function of dissimilarity. This means "
	"that, with the transformation "
	"above, you ultimately fit an INDSCAL model in which the distance "
	"between object %i and %j will be linearly related to the confusion "
	"between %i and %j.")
NORMAL (L"For the relation between confusion and dissimilarity, the model "
	"implemented here, makes the assumption that the amount of confusion "
	"between objects %i and %j is related to the amount that their "
	"probability density functions, pdf's, overlap. Because we do not know "
	"these pdf's we make the assumption that both are normal, have equal "
	"%sigma and are one-dimensional. The parameter to be determined is the "
	"distance between the centres of both pdf's. "
	"According to formula 26.2.23 in @@Abramowitz & Stegun (1970)@, for each "
	"fraction %f__%ij_, we have to find an %x that solves:")
FORMULA (L"%f__%ij_ = 1 / \\Vr(2%\\pi) \\in__%x_^^\\oo^ e^^-%t\\.c%t/2^ %dt")
NORMAL (L"This %x will be used as the dissimilarity between %i and %j. The "
	"relation between %x and %f__%ij_ is monotonic. This means that the "
	"results for a Kruskal analysis will not change much. For INDSCAL, in "
	"general, you will note a significantly better fit.")
MAN_END

MAN_BEGIN (L"Confusion: To Similarity...", L"djmw", 20040407)
INTRO (L"A command that creates a @Similarity from every selected @Confusion.")
ENTRY (L"Settings")
TAG (L"##Normalize")
DEFINITION (L"when on, normalize rows by dividing each row element by the row "
	"sum. In this way you correct for unequal stimulus numbers.")
TAG (L"##No symmetrization#, #Average, #Houtgast")
DEFINITION (L"determine the symmetrization procedure.")
ENTRY (L"Algorithm")
NORMAL (L"The %Average procedure averages:")
FORMULA (L"%similarity__%ij_= %similarity__%ji_ = (%confusion__%ij_ + "
	"%confusion__%ji_) / 2")
NORMAL (L"The %Houtgast procedure as described in the paper by @@Klein, Plomp "
	"& Pols (1970)@, expresses similarity between stimuli %i and %j by the "
	"number of times that stimulus %i and %j have "
	"resulted in the same response, summated over all response categories.")
NORMAL (L"We use the following formula to calculate the %Houtgast "
	"dissimilarities:")
FORMULA (L"%similarity__%ij_ = \\su__%k=1..%numberOfColumns_ min "
	"(%confusion__%ik_, %confusion__%jk_)")
NORMAL (L"which is equivalent to the formula in the Klein et al. paper:")
FORMULA (L"%similarity__%ij_ = \\su__%k=1..%numberOfColumns_ (%confusion__%ik_ "
	"+ %confusion__%jk_ \\-- |%confusion__%ik_ \\-- %confusion__%jk_|)")
MAN_END

MAN_BEGIN (L"congruence coefficient", L"djmw", 20040407)
INTRO (L"The %%congruence coefficient% is a measure of similarity between two "
	"@@Configuration@s.")
NORMAL (L"The congruence coefficient %c(#X, #Y) for the configurations #X and "
	"#Y is defined as:")
FORMULA (L"%c(%X, %Y) = \\su__%i<%j_ %w__%ij_ %d__%ij_(#X) %d__%ij_(#Y) / "
	"([\\su__%i<%j_ %w__%ij_ %d^2__%ij_(#X)]^^1/2^ [\\su__%i<%j_ %w__%ij_ "
	"%d^2__%ij_(#Y)]^^1/2^),")
NORMAL (L"where %d__%ij_(#X) is the distance between the points %i and %j in "
	"configuration #X and %w__%ij_ are nonnegative weights (default: %w__%ij_"
	" = 1).")
NORMAL (L"Since distances are nonnegative, the congruence coefficient has a "
	"value between 0 and 1.")
NORMAL (L"The %%congruence coefficient% is a better measure of the similarity "
	"between configurations than the %%correlation coefficient% of the "
	"distances. @@Borg & Groenen (1997)@ give a simple example where things "
	"go wrong with correlation coefficients: two configurations #X and #Y with three points each, have "
	"distances %d__12_(#X) = 1, %d__13_(#X) = 2, %d__23_(#X) = 3 and "
	"%d__12_(#Y) = 2, %d__13_(#Y) = 3, %d__23_(#Y) = 4. "
	"These distances have a correlation coefficient of 1. "
	"However, in #X the three points lie on a straight line and in #Y the "
	"points form a triangle. This unwanted situation occurs because "
	"in the calculation of the correlation coefficient the mean is subtracted "
	"from the distances and the resulting values are no longer distances "
	"(they may become negative). In calculating the correlation "
	"between the distances we should not subtract the mean. "
	"In fact, the congruence coefficient is exactly this correlation "
	"coefficient calculated with respect to the origin and "
	"not with respect to the centroid position (the \"mean\").")
NORMAL (L"For further information on how well one number can assess the "
	"similarity between two configurations see @@Borg & Groenen (1997)@ "
	"section 19.7.")
MAN_END

MAN_BEGIN (L"ContingencyTable", L"djmw", 19971216)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"In a two-way contingency table, cell %f__%ij_ contains the frequency "
	"with which row category %i co-occurs with column category %j. "
	"Necessarily, all %f__%ij_ \\>_ 0.")
ENTRY (L"Commands")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu ##TableOfReal: To ##ContingencyTable")
NORMAL (L"Query")
LIST_ITEM (L"\\bu ##ContingencyTable: Get chi squared probability")
LIST_ITEM (L"\\bu ##ContingencyTable: Get Cramer's statistic")
LIST_ITEM (L"\\bu ##ContingencyTable: Get contingency coefficient")
NORMAL (L"Analysis")
LIST_ITEM (L"\\bu @@ContingencyTable: To Configuration (ca)...")
MAN_END

MAN_BEGIN (L"ContingencyTable: To Configuration (ca)...", L"djmw", 20040407)
INTRO (L"A command that creates a @Configuration object from the selected "
	"@ContingencyTable object by means of @@Correspondence analysis@.")
ENTRY (L"Settings")
TAG (L"##Number of dimensions")
DEFINITION (L"The dimensionality of the Configuration.")
TAG (L"##Scaling of the final configuration")
DEFINITION (L"determines whether row points are in the centre of gravity of "
	"column points, or, column points are in the centre of gravity of row "
	"points, or, whether roes and columns are treated symmetrically.")
ENTRY (L"Algorithm")
NORMAL (L"1. We start with the following transformation of the entries "
	"%f__%ij_:")
FORMULA (L"%h__%ij_ = %f__%ij_ / \\Vr (%f__%i+_%f__+%j_) - \\Vr "
	"(%f__%i+_%f__+%j_) / %N,")
NORMAL (L"where %h__%ij_ is the entry for a cell in the matrix #H with "
	"transformed data, %f__%i+_ "
	"is the total count for row %i, %f__+%j_ is the total count for column %j "
	"and %N is the grand total. "
	"This can be written in matrix form as:")
FORMULA (L"#H = #R^^\\--1/2^#F#C^^\\--1/2^ \\-- #R^^1/2^#uu\\'p#C^^1/2^ / %N,")
NORMAL (L"where #R and #C are diagonal matrices with the row and column totals, "
	"respectively and #u a column vector with all elements equal to 1. ")
NORMAL (L"2. Next the singular value decomposition of matrix #H is performed:")
FORMULA (L"#H = #K #\\La #L\\'p,")
NORMAL (L"where #K\\'p#K = #I, #L\\'p#L = #I, and #\\La is a diagonal matrix "
	"with singular values.")
NORMAL (L"3. Now the row (#X) and column points (#Y) can be determined. "
	"Three normalizations are possible:")
TAG (L"\\bu Scale row points in the centre of gravity of column points")
DEFINITION (L"#X = \\Vr%N #R^^\\--1/2^#K#\\La")
DEFINITION (L"#Y = \\Vr%N #C^^\\--1/2^#L")
TAG (L"\\bu Scale column points in the centre of gravity of row points")
DEFINITION (L"#X = \\Vr%N #R^^\\--1/2^#K")
DEFINITION (L"#Y = \\Vr%N #C^^\\--1/2^#L#\\La")
TAG (L"\\bu Treat row points and column points symmetrically")
DEFINITION (L"#X = \\Vr%N #R^^\\--1/2^#K#\\La^^\\--1/2^")
DEFINITION (L"#Y = \\Vr%N #C^^\\--1/2^#L\\La^^\\--1/2^")
NORMAL (L"For more details see @@Gifi (1990)@, chapter 8.")
MAN_END

MAN_BEGIN (L"Correspondence analysis", L"djmw", 19971216)
INTRO (L"Correspondence analysis provides a method for representing data in "
	"an Euclidean space so that the results can be visually examined for "
	"structure. For data in a typical two-way @ContingencyTable both the row "
	"variables and the column variables are represented in the same space. "
	"This means that one can examine relations not only among row "
	"or column variables but also between row and column variables.")
NORMAL (L"In correspondence analysis the data matrix is first transformed by "
	"dividing each cell by the square root of the corresponding row and column "
	"totals. "
	"The transformed matrix is then decomposed with singular value "
	"decomposition resulting in the singular values (which in this case are "
	"canonical correlations) and a set of row vectors and column vectors. "
	"Next the row and column vectors are rescaled with the original total "
	"frequencies to obtain optimal scores. "
	"These optimal scores are weighted by the square root of the singular "
	"values and become the coordinates of the points in the @Configuration.")
NORMAL (L"Examples can be found in the books by @@Weller & Romney (1990)@ and "
	"@@Gifi (1990)@.")
MAN_END

MAN_BEGIN (L"Create Configuration...", L"djmw", 19980413)
INTRO (L"A command to create a @Configuration with the specified number of "
	"points and number of dimensions. The location of the points will be "
	"determined by the formula (see @@Formulas@ for more "
	"information about possible formulas).")
MAN_END

MAN_BEGIN (L"Create INDSCAL Carroll & Wish example...", L"djmw", 19971201)
INTRO (L"Creates eight @Dissimilarity objects that bear names \"1\" ... \"8\".")
NORMAL (L"These objects contain the interpoint distances for a twodimensional "
	"3\\xx3 @Configuration of points, labelled A, B, C, ... I. "
	"All Dissimilarity objects are based on the following underlying configuration.")
PICTURE (4.0, 4.0, drawCarrollWishConfigurationExample)
NORMAL (L"The eight sources weigh this configuration in the following manner:")
PICTURE (4.0, 4.0, drawCarrollWishSalienceExample)
NORMAL (L"For each source, the distances were subjected to the transformation: ")
FORMULA (L"%dissimilarity__%ij_ = %distance__%ij_ + %noiseRange \\.c #u, ")
NORMAL (L"where #u is a uniform random variable between 0 and 1.")
NORMAL (L"Now you can do the following for example:")
TAG (L"Select all the Dissimilarity objects and choose @@Dissimilarity: To Distance...|"
	"To Distance...@.")
DEFINITION (L"Uncheck scale (add \"additive constant\").")
TAG (L"Select all the Distance objects and choose @@Distance: To Configuration (indscal)...|"
	"To Configuration (indscal)...@.")
DEFINITION (L"and an @@INDSCAL analysis@ will be performed. In order to reproduce the saliences, "
	"you have to uncheck the \"Normalize scalar products\" option.")
NORMAL (L"This example was adapted from @@Carroll & Wish (1974)@.")
MAN_END

MAN_BEGIN (L"Create letter R example...", L"djmw", 19971201)
INTRO (L"Creates a @Dissimilarity object that bears the name %R. The "
	"dissimilarities in this object were chosen to be a monotone "
	"transformation of the distances between the 32 two-dimensional points "
	"that make up the capital letter #R.")
PICTURE (4.0, 4.0, drawLetterRConfigurationExample)
NORMAL (L"All 32 \\.c (32-1)/2 interpoint distances  were subjected to the "
	"transformation: ")
FORMULA (L"%dissimilarity__%ij_^ = %distance__%ij_^2 + 5 + %noiseRange \\.c #u, ")
NORMAL (L"where #u is a uniform random variable between 0 and 1.")
NORMAL (L"This example was chosen from @@Green, Carmone & Smith (1989)@.")
MAN_END

MAN_BEGIN (L"disparities", L"djmw", 19980111)
INTRO (L"The numbers %d\\'p__%ij_ that result from applying an admissible "
	"transformation %f on the dissimilarities %\\de__%ij_, i.e., %d\\'p__%ij_ "
	"= %f (%\\de__%ij_). Disparities have the same dimension as distances. "
	"Other names for disparities are %%pseudo distances% and %%target "
	"distances%.")
MAN_END

MAN_BEGIN (L"Dissimilarity", L"djmw", 20010327)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"It represents a one-way table with "
	"dissimilarities between \"objects\".")
ENTRY (L"Creating a Dissimilarity from data in a text file")
NORMAL (L"Suppose you have three objects A, B and C. "
	"In one way or another, you have acquired the following (symmetric) "
	"dissimilarities: %\\de__%AB_ = 2 (= %\\de__%BA_) , %\\de__%AC_ = 1 "
	"(= %\\de__%CA_), and %\\de__%BC_ = 1.4 (= %\\de__CB_), where %\\de__%AB_"
	" represents the dissimilarity between object A and object B.")
NORMAL (L"You can create a simple text file like the following:")
CODE (L"\"ooTextFile\"  ! The line by which Praat can recognize your file")
CODE (L"\"Dissimilarity\" ! The line that tells Praat about the contents")
CODE (L"3     \"A\"  \"B\"  \"C\"   ! Number of columns, and column labels")
CODE (L"3                     ! Number of rows")
CODE (L"\"A\"    0    2    1    ! Row label (A), A-B value, A-C value")
CODE (L"\"B\"    2    0  1.4    ! Row label (B), B-A value, B-C value")
CODE (L"\"C\"    1    1.4  0    ! Row label (C), C-A value, C-B value")
NORMAL (L"Notice that:")
LIST_ITEM (L"\\bu the row and column labels are identical.")
LIST_ITEM (L"\\bu the matrix elements on the diagonal are zero.")
LIST_ITEM (L"\\bu the matrix is symmetrical.")
NORMAL (L"This text file can be read with the @@Read from file...@ command. "
	"Since a Dissimilarity object has the data structure of a square "
	"symmetrical TableOfReal, you could also start from an appropriate "
	"@TableOfReal object and cast it to a Dissimilarity object.")
ENTRY (L"Commands")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu @@Confusion: To Dissimilarity...")
NORMAL (L"Drawing")
LIST_ITEM (L"\\bu ##Draw as numbers...")
LIST_ITEM (L"\\bu ##Draw as squares...")
NORMAL (L"Query")
LIST_ITEM (L"\\bu ##Get column mean (index)...")
LIST_ITEM (L"\\bu ##Get column mean (label)...")
LIST_ITEM (L"\\bu ##Get column stdev (index)...")
LIST_ITEM (L"\\bu ##Get column stdev (label)...")
LIST_ITEM (L"\\bu @@Dissimilarity: Get additive constant")
NORMAL (L"Modification")
LIST_ITEM (L"\\bu @@Formula...")
LIST_ITEM (L"\\bu ##Set value...")
LIST_ITEM (L"\\bu ##Remove column (index)...")
LIST_ITEM (L"\\bu ##Insert column (index)...")
LIST_ITEM (L"\\bu ##Set row label (index)...")
LIST_ITEM (L"\\bu ##Set row label (label)...")
LIST_ITEM (L"\\bu ##Set column label (index)...")
LIST_ITEM (L"\\bu ##Set column label (label)...")
NORMAL (L"Multidimensional scaling analysis")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (kruskal)...")
LIST_ITEM (L"Transformations")
LIST_ITEM (L"\\bu @@Dissimilarity: To Distance...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Weight")
MAN_END

MAN_BEGIN (L"Dissimilarity: Get additive constant", L"djmw", 19971201)
INTRO (L"A command that calculates the \"additive constant\" from the selected @Dissimilarity.")
NORMAL (L"Distances %d__%ij_ will be obtained from dissimilarities %\\de__%ij_ according to:")
FORMULA (L" %distance__%ij_ = %dissimilarity__%ij_ + %additiveConstant")
NORMAL (L"We use a procedure by @@Cailliez (1983)@ to solve the \"additive constant problem\", i.e. "
	"find the smallest %additiveConstant such that all %distance__%ij_ in the above equation "
	" have a Euclidean representation.")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Configuration (absolute mds)...", L"djmw", 19980105)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity object.")
NORMAL (L"The @disparities %d\\'p__%ij_ will be obtained from dissimilarities %\\de__%ij_ according to:")
FORMULA (L"%d\\'p__%ij_ = %\\de__%ij_")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Configuration (interval mds)...", L"djmw", 19980105)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity object.")
NORMAL (L"The @disparities %d\\'p__%ij_ will be obtained from dissimilarities %\\de__%ij_ according to:")
FORMULA (L"%d\\'p__%ij_ = %a + %b \\.c %\\de__%ij_")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Configuration (i-spline mds)...", L"djmw", 20040407)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity object.")
NORMAL (L"Dissimilarities %\\de__%ij_ and @disparities %d\\'p__%ij_ will be related by a @spline function:")
FORMULA (L"%d\\'p__%ij_ = \\su__%k=1..(%%numberOfInteriorKnots%+%order)_ spline__%k_ (%knots, %order, %\\de__%ij_),")
NORMAL (L"where spline__%k_ (\\.c) is the value of the %k^^th^ I-spline of order %order and knot sequence "
	"%knot evaluated at %\\de__%ij_.")
ENTRY (L"Settings")
TAG (L"##Number of dimensions")
DEFINITION (L"determines the dimensionality of the configuration.")
TAG (L"##Number of interior knots")
DEFINITION (L"determines the number of segment boundaries. Each interior knot "
	"is the boundary between two segments. The splines in each segment will "
	"be joined as continuously as possible.")
TAG (L"##Order of I-spline")
DEFINITION (L"The order of the polynomial basis of the I-spline.")
NORMAL (L"Finding the optimal Configuration involves a minimization process:")
TAG (L"##Tolerance")
DEFINITION (L"When successive values for the stress differ by less than "
	"#Tolerance, the minimization process stops.")
TAG (L"##Maximum number of iterations")
DEFINITION (L"Minimization stops after this number of iterations has been "
	"reached.")
TAG (L"##Number of repetitions")
DEFINITION (L"If chosen larger than 1, the minimization process will be "
	"repeated, each time with another random start configuration. "
	"The configuration that results in minimum stress, will be saved.")
ENTRY (L"Hints")
NORMAL (L"If %numberOfInteriorKnots is zero, polynomial regression will be "
	"performed. Therefore , the combination %numberOfInteriorKnots = 0 and "
	"%order = 1 also gives interval "
	"scaling (in fact, it is the implementation in this program).")
NORMAL (L"In the limit when %order = 0 and %numberOfInteriorKnots = "
	"%numberOfDissimilarities, monotone regression is performed.")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Configuration (kruskal)...", L"djmw", 20040407)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object.")
ENTRY (L"Settings")
TAG (L"##Number of dimensions# (standard value: 2)")
DEFINITION (L"The dimensionality of the Configuration.")
TAG (L"##Distance metric% (standard value: 2, i.e. Euclidean)")
DEFINITION (L"the general distance between points #x__%i_ and #x__%j_ (%i,%j "
	"= 1..%numberOfPoints) is:")
DEFINITION (L"(\\su__%k=1..%numberOfDimensions_ |%x__%ik_ \\--%x__%jk_|"
	"^^%metric^)^^1/%metric^")
TAG (L"##Sort distances")
DEFINITION (L"determines the handling of ties in the data. When off, whenever "
	"two or more dissimilarities are equal we do not care whether the fitted "
	"distances are equal or not. "
	"Consequently, no constraints are imposed on the fitted distances. "
	"When on, however, we impose the constaint that the fitted distances be "
	"equal whenever the dissimilarities are equal.")
NORMAL (L"For the calculation of stress:")
TAG (L"##Formula1 (default)")   // ??
FORMULA (L"%stress = \\Vr(\\su(%distance__%k_ \\-- %fittedDistance__%k_)^2 / "
	"\\su %distance__%k_^2)")
TAG (L"##Formula2")
FORMULA (L"%stress = \\Vr(\\su(%distance__%k_ \\-- %fittedDistance__%k_)^2 / "
	"\\su (%distance__%k_ \\-- %averageDistance)^2)")
DEFINITION (L"Note that values of stress 2 are generally more than double those "
	"of stress 1 for the same degree of fit.")
NORMAL (L"Finding the optimal Configuration involves a minimization process:")
TAG (L"##Tolerance")
DEFINITION (L"When successive values for the stress differ less than %Tolerance "
	"the minimization process stops.")
TAG (L"##Maximum number of iterations")
DEFINITION (L"Minimization stops after this number of iterations has been "
	"reached.")
TAG (L"##Number of repetitions")
DEFINITION (L"When chosen larger than 1, the minimalization process will be "
	"repeated, each time with another random start configuration. "
	"The configuration that results in minimum stress will be saved.")
ENTRY (L"Precautions")
NORMAL (L"When there are few objects it is impossible to recover many "
	"dimensions. A rough rule of thumb is that there should be at least twice "
	"as many number of observations, i.e. the %numberOfPoints \\.c "
	"(%numberOfPoints - 1) / 2 (dis)similarities, than parameters "
	"to be estimated, i.e. the %numberOfPoints \\.c %numberOfDimensions "
	"position coordinates. A practical guide is:")
	LIST_ITEM (L"for %numberOfDimensions = 1 you need \\>_ 5 objects")
	LIST_ITEM (L"for %numberOfDimensions = 2 you need \\>_ 9 objects")
	LIST_ITEM (L"for %numberOfDimensions = 3 you need \\>_ 13 objects")
NORMAL (L"There is no feasible way to be certain that you have found the "
	"true global minimum. However, by using a great number of different "
	"random starting configurations to scale the same data it is often "
	"possible to obtain practical certainty. "
	"Although the procedure for obtaining an initial configuration is based "
	"on a %linear relation between distance and (dis)similarity, it gives a "
	"very good approximation of the optimal #Configuration and "
	"the #Minimizer practically always finds the global minimum from it "
	"(I guess...).  A way to find out is to try the %numberOfRepetitions "
	"parameter which gives you the possibility to fit many times and each "
	"time start with another random initial configuration.")
ENTRY (L"Algorithm")
LIST_ITEM (L"1. The Dissimilarity object is converted to a Distance object in "
	"the same way as in @@Dissimilarity: To Distance...@.)")
LIST_ITEM (L"2. From the Distance object an initial Configuration is found by "
	"first transforming the Distance object to a matrix with scalar products "
	"of distances and subsequently solving for the first %numberOfDimensions "
	"eigenvectors of this matrix.")
LIST_ITEM (L"3. A minimalization algorithm is started that tries to minimize a "
	"function. In this function:")
LIST_ITEM (L"\\bu 3.1 We normalize the current Configuration from the minimizer")
LIST_ITEM (L"\\bu 3.2 Calculate a new Distance object from the configuration")
LIST_ITEM (L"\\bu 3.3 Do a monotone regression of this Distance on the "
	"Dissimilarity. This results in a new Distance object.")
LIST_ITEM (L"\\bu 3.4 Calculate stress from this Distance and the Distance "
	"obtained from Dissimilarity.")
NORMAL (L"The optimization process is ccontrolledby a conjugate gradient "
	"minimization algorithm that tries to minimize the %stress function. "
	"In @@Kruskal (1964)@, a steepest descent "
	"algorithm is used wwhichis less efficient. ")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Configuration (monotone mds)...", L"djmw", 20040407)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object.")
NORMAL (L"Dissimilarities %\\de__%ij_ and @disparities %d\\'p__%ij_ are "
	"related by:")
FORMULA (L"%d\\'p__%ij_ \\<_ %d\\'p__%kl_ if %\\de__%ij_ \\<_ %\\de__%kl_")
ENTRY (L"Settings")
TAG (L"##Number of dimensions")
DEFINITION (L"determines the number of dimensions of the configuration.")
TAG (L"##Primary or secondary approach to ties")
DEFINITION (L"When dissimilarities are equal, i.e., %\\de__%ij_ = %\\de__%kl_, "
	"the primary approach imposes no conditions on the corresponding "
	"@disparities %d\\'p__%ij_ and %d\\'p__%kl_, while the %secondary "
	"approach demands that also %d\\'p__%ij_ = %d\\'p__%kl_.")
NORMAL (L"Finding the optimal Configuration involves a minimization process:")
TAG (L"##Tolerance")
DEFINITION (L"When successive values for the stress differ less than %Tolerance "
	"the minimization process stops.")
TAG (L"##Maximum number of iterations")
DEFINITION (L"Minimization stops after this number of iterations has been reached.")
TAG (L"##Number of repetitions")
DEFINITION (L"When chosen larger than 1, the minimalization process will be "
	"repeated, each time with another random start configuration. "
	"The configuration that results in minimum stress will be saved.")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Configuration (ratio mds)...", L"djmw", 19980105)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object.")
NORMAL (L"The @disparities %d\\'p__%ij_ will be obtained from dissimilarities "
	"%\\de__%ij_ according to:")
FORMULA (L"%d\\'p__%ij_ = %b \\.c %\\de__%ij_")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Distance...", L"djmw", 20040407)
INTRO (L"A command that creates a @Distance object from a selected "
	"@Dissimilarity object.")
ENTRY (L"Settings")
TAG (L"##Scale")
DEFINITION (L"when on, the @@Dissimilarity: Get additive constant|"
	"additiveConstant@ is determined, when off the %additiveConstant = 0.")
NORMAL (L"dissimilarities are transformed to distances according to:")
FORMULA (L" %distance__%ij_ = %dissimilarity__%ij_ + %additiveConstant.")
MAN_END

MAN_BEGIN (L"Dissimilarity: To Weight", L"djmw", 19980108)
INTRO (L"Creates an object of type @Weight for each selected @Dissimilarity "
	"object.")
NORMAL (L"The values in the weight matrix will be:")
LIST_ITEM (L"%w__%ii_ = 0")
LIST_ITEM (L"%w__%ij_ = 1 if %\\de__%ij_ > 0")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Draw regression (absolute mds)...",
	L"djmw", 20040407)
INTRO (L"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity object versus @disparities %d\\'p__%ij_ obtained "
	"from the \"regression\" of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
FORMULA (L"%d\\'p__%ij_ = %\\de__%ij_")
ENTRY (L"Settings")
TAG (L"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (L"minimum and maximum values for the proximities (horizontal axis).")
TAG (L"##Minimum distance#, ##Maximum distance#")
DEFINITION (L"minimum and maximum values for the distances (vertical axis).")
TAG (L"##Mark size (mm)#, ##Mark string#")
DEFINITION (L"size and kind of the marks in the plot.")
TAG (L"##Garnish")
DEFINITION (L"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Draw regression (interval mds)...",
	L"djmw", 20040407)
INTRO (L"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the regression of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
FORMULA (L"%d\\'p__%ij_ = %a + %b \\.c %\\de__%ij_,")
NORMAL (L"where the values of %a and %b are determined by regression.")
ENTRY (L"Settings")
TAG (L"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (L"minimum and maximum values for the proximities (horizontal axis).")
TAG (L"##Minimum distance#, ##Maximum distance#")
DEFINITION (L"minimum and maximum values for the distances (vertical axis).")
TAG (L"##Mark size (mm)#, ##Mark string#")
DEFINITION (L"size and kind of the marks in the plot.")
TAG (L"##Garnish")
DEFINITION (L"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Draw regression (i-spline mds)...", L"djmw", 20040407)
INTRO (L"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the regression of distances %d__%ij_ from @Configuration on the "
	"@spline transformed dissimilarities %\\de__%ij_.")
ENTRY (L"Settings")
TAG (L"##Number of interior knots")
DEFINITION (L"determines the number of segments.")
TAG (L"##Order of I-spline")
DEFINITION (L"The order of the polynomial basis of the I-spline.")
TAG (L"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (L"minimum and maximum values for the proximities (horizontal axis).")
TAG (L"##Minimum distance#, ##Maximum distance#")
DEFINITION (L"minimum and maximum values for the distances (vertical axis).")
TAG (L"##Mark size (mm)#, ##Mark string#")
DEFINITION (L"size and kind of the marks in the plot.")
TAG (L"##Garnish")
DEFINITION (L"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Draw regression (monotone mds)...", L"djmw", 20040407)
INTRO (L"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the monotone regression of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
ENTRY (L"Settings")
TAG (L"##Primary or secondary approach to ties")
DEFINITION (L"When dissimilarities are equal, i.e., %\\de__%ij_ = %\\de__%kl_ "
	"the primary approach imposes no conditions on the corresponding distances "
	"%d__%ij_ and %d__%kl_, while the %secondary approach demands that also "
	"%d__%ij_ = %d__%kl_.")
TAG (L"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (L"minimum and maximum values for the proximities (horizontal axis).")
TAG (L"##Minimum distance#, ##Maximum distance#")
DEFINITION (L"minimum and maximum values for the distances (vertical axis).")
TAG (L"##Mark size (mm)#, ##Mark string#")
DEFINITION (L"size and kind of the marks in the plot.")
TAG (L"##Garnish")
DEFINITION (L"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Draw regression (ratio mds)...", L"djmw", 20040407)
INTRO (L"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the \"regression\" of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
FORMULA (L"%d\\'p__%ij_ = %b \\.c %\\de__%ij_,")
NORMAL (L"where the value of %b is determined by regression.")
ENTRY (L"Settings")
TAG (L"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (L"minimum and maximum values for the proximities (horizontal axis).")
TAG (L"##Minimum distance#, ##Maximum distance#")
DEFINITION (L"minimum and maximum values for the distances (vertical axis).")
TAG (L"##Mark size (mm)#, ##Mark string#")
DEFINITION (L"size and kind of the marks in the plot.")
TAG (L"##Garnish")
DEFINITION (L"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Draw Shepard diagram...", L"djmw", 20040407)
INTRO (L"Draws the Shepard diagram. This is a scatterplot of the "
	"dissimilarities from the @Dissimilarity object versus distances (as "
	"calculated from the @Configuration).")
ENTRY (L"Settings")
TAG (L"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (L"minimum and maximum values for the proximities (horizontal axis).")
TAG (L"##Minimum distance#, ##Maximum distance#")
DEFINITION (L"minimum and maximum values for the distances (vertical axis).")
TAG (L"##Mark size (mm)#, ##Mark string#")
DEFINITION (L"size and kind of the marks in the plot.")
TAG (L"##Garnish")
DEFINITION (L"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Get stress (absolute mds)...",
	L"djmw", 19980119)
INTRO (L"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (L"Behaviour")
NORMAL (L"Stress formula's are #dependent of the scale of the Configuration: "
	"you will get #another stress value if you had pre-multiplied the "
	"selected Configuration with any number greater than zero.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Get stress (interval mds)...",
	L"djmw", 19980119)
INTRO (L"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (L"Behaviour")
NORMAL (L"We use stress formula's that are independent of the scale of the "
	"Configuration: you would have got the same stress value if you had "
	"pre-multiplied the selected Configuration with any number greater "
	"than zero.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Get stress (i-spline mds)...",
	L"djmw", 19980119)
INTRO (L"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (L"Behaviour")
NORMAL (L"We use stress formula's that are independent of the scale "
	"of the Configuration: you would have got the same stress value if "
	"you had pre-multiplied the selected Configuration with any number "
	"greater than zero.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Get stress (monotone mds)...",
	L"djmw", 19980119)
INTRO (L"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (L"Behaviour")
NORMAL (L"We use stress formula's that are independent of the scale "
	"of the Configuration: you would have got the same stress value if "
	"you had pre-multiplied the selected Configuration with any number "
	"greater than zero.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: Get stress (ratio mds)...",
	L"djmw", 19980119)
INTRO (L"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (L"Behaviour")
NORMAL (L"We use stress formula's that are independent of the scale "
	"of the Configuration: you would have got the same stress value if "
	"you had pre-multiplied the selected Configuration with any number "
	"greater than zero.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: To Configuration (absolute mds)...",
	L"djmw", 19980119)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: To Configuration (interval mds)...",
	L"djmw", 19980119)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: To Configuration (i-spline mds)...",
	L"djmw", 19980119)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: To Configuration (kruskal)...",
	L"djmw", 19971201)
INTRO (L"A command to fit an optimal @Configuration for the selected "
	"@Dissimilarity object. The selected @Configuration will be used as the "
	"starting configuration in the kruskal analysis.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: To Configuration (monotone mds)...",
	L"djmw", 19980119)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration: To Configuration (ratio mds)...",
	L"djmw", 19980119)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration & Weight: Get stress...", L"djmw", 20040407)
INTRO (L"A command that calculates the @stress between distances %d__%ij_ "
	"derived from the selected @Configuration object and @disparities "
	"%d\\'p__%ij_ derived from the selected @Dissimilarity object. "
	"With the selected @Weight object the evaluation of the influence "
	"of each dissimilarity on stress can be influenced.")
ENTRY (L"Settings")
LIST_ITEM (L"%%Normalized stress%, %%Kruskal's stress-1%, %%Kruskal's "
	"stress-2% or %Raw stress%")
ENTRY (L"Behaviour")
NORMAL (L"Except for %absolute %mds, we us stress formula's that are "
	"independent of the scale of the Configuration (see @stress): you would "
	"have got the same stress value if you had pre-multiplied the selected "
	"Configuration with any number greater than zero.")
MAN_END

MAN_BEGIN (L"Dissimilarity & Configuration & Weight: To Configuration...", L"djmw", 20040407)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. With the selected @Weight object the influence of each "
	"dissimilarity on @stress can be influenced. The selected Configuration "
	"object serves as a starting configuration for the minimization process.")
ENTRY (L"Settings")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
MAN_END

MAN_BEGIN (L"Dissimilarity & Weight: To Configuration...", L"djmw", 20040407)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. With the selected @Weight object the influence of each "
	"dissimilarity on @stress can be influenced.")
ENTRY (L"Settings")
NORMAL (L"May be different and depend on the representation function, i.e. "
	"the scale of measurement.")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
MAN_END

MAN_BEGIN (L"Dissimilarity & Weight: To Configuration...", L"djmw", 20040407)
INTRO (L"A command that creates a @Configuration object from a @Dissimilarity "
	"object. With the selected @Weight object the influence of each "
	"dissimilarity on @stress can be influenced.")
ENTRY (L"Settings")
NORMAL (L"May be different and depend on the representation function, i.e. the scale of measurement.")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
MAN_END

MAN_BEGIN (L"Distance", L"djmw", 19971124)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Distance represents distances between objects in a metrical space.")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu @@Confusion: To Dissimilarity (pdf)...")
LIST_ITEM (L"\\bu @@Dissimilarity: To Distance...")
MAN_END

MAN_BEGIN (L"Distance: To Configuration (indscal)...", L"djmw", 19971124)
INTRO (L"Perform an @@INDSCAL analysis@ on the selected object(s) of type @Distance that "
	"results in a @Configuration and a @Salience object.")
MAN_END

MAN_BEGIN (L"Distance: To Configuration (ytl)...", L"djmw", 19971124)
INTRO (L"A command that creates one @Configuration and one @Salience object "
	"from a collection of one or more @Distance objects.")
NORMAL (L"This Configuration and Salience object normally serve as starting points "
	"for an @@individual difference scaling@ such as an @@INDSCAL analysis@.")
NORMAL (L"The algorithm is ddescribedin @@Young, Takane & Lewyckyj (1978)@.")
MAN_END

MAN_BEGIN (L"Distance: To ScalarProduct...", L"djmw", 20040407)
INTRO (L"A command that creates a @ScalarProduct for each selected @Distance.")
ENTRY (L"Setting")
TAG (L"##Make sum of squares equal 1.0")
DEFINITION (L"when selected, the elements in the resulting matrix part will be scaled such that "
	"the sum of all the squared elements in the matrix equals 1.0.")
ENTRY (L"Algorithm")
NORMAL (L"ScalarProduct entries %b__%ij_ are created from distances %d__%ij_ bij double centering "
	"the matrix with elements \\--1/2 %d__%ij_^2, i.e.,")
FORMULA (L"%b__%ij_= \\--1/2(%d__%ij_^2 \\-- %d__%\\.cj_^2 \\-- %d__%i\\.c_^2 + %d__%\\.c\\.c_^2),")
NORMAL (L"where the dot (\\.c) means averaging over that dimension.")
MAN_END

MAN_BEGIN (L"Distance & Configuration: Draw scatter diagram...", L"djmw", 19971201)
MAN_END

MAN_BEGIN (L"Distance & Configuration: Get VAF...", L"djmw", 19971201)
INTRO (L"Calculates the \"%%variance accounted for%\" from the selected collection of @Distance objects and the "
	"selected @Configuration. The optimal @Salience necessary for the calculation "
	"will be inferred from the selected Distance and Configuration objects.")
MAN_END

MAN_BEGIN (L"Distance & Configuration: To Configuration (indscal)...", L"djmw", 19971201)
INTRO (L"Performs an @@INDSCAL analysis@ on the selected objects of type @Distance and calculates "
	"a @Configuration from them. Uses the selected Configuration object as the initial Configuration in "
	"the iteration process.")
MAN_END

MAN_BEGIN (L"Distance & Configuration & Salience: Get VAF...", L"djmw", 19971201)
INTRO (L"Calculates the \"variance accounted for\" from the selected collection "
	"of @Distance objects, the "
	"selected @Configuration and the selected #Salience.")
MAN_END

MAN_BEGIN (L"Distance & Configuration & Salience: To Configuration "
	"(indscal)...", L"djmw", 19971201)
INTRO (L"A command that creates a new @Configuration from the selected "
	"collection of @Distance objects, the selected @Configuration and the "
	"selected #Salience. The selected Configuration and Salience "
	"serve as start values for the @@INDSCAL analysis@.")
MAN_END

MAN_BEGIN (L"individual difference scaling", L"djmw", 19970502)
INTRO (L"The purpose of individual difference scaling is to represent objects, "
	"whose dissimilarities are given, as points in a metrical space. "
	"The distances in the space should be in accordance with the "
	"dissimilarities as well as is possible. Besides the configuration a "
	"@Salience matrix is calculated.")
NORMAL (L"The basic Euclidean model is:")
LIST_ITEM (L"%\\de__%ijk_ \\~~ (\\su__%s=1..%r_ %w__%ks_(%x__%is_ \\-- "
	"%x__%js_)^2)^^1/2^")
NORMAL (L"Here \\de__%ijk_ is the (known) dissimilarity between %objects %i and "
	"%j, as measured on %data %source %k. The %x's are the %coordinates of "
	"the objects in an %r-dimensional space and the %w's are weights or "
	"saliences. Because straight minimization of the expression above "
	"is difficult, one applies transformations on this expression. "
	"Squaring both sides gives the model:")
LIST_ITEM (L"%\\de^2__%ijk_ \\~~ \\su__%s=1..%r_ %w__%ks_(%x__%is_ \\-- "
	"%x__%js_)^2")
NORMAL (L"and the corresponding least squares loss function:")
LIST_ITEM (L"\\su__%k=1..%numberOfSources_ \\su__%i=1..%numberOfPoints_ "
	"\\su__%j=1..%numberOfPoints_ (%\\de^2__%ijk_ \\-- %d^2__%ijk_)^2")
NORMAL (L"This loss function is minimized in the (ratio scale option of the) "
	"#ALSCAL program of @@Takane, Young & de Leeuw (1976)@.")
NORMAL (L"The transformation used by @@Carroll & Chang (1970)@ in the INDSCAL "
	"model, transforms the data from each source into scalar products "
	"of vectors. For the dissimilarities:")
LIST_ITEM (L"%\\be__%ijk_ = \\--{ %\\de^2__%ijk_ \\-- %\\de^2__%i.%k_ \\-- "
	"%\\de^2__.%jk_ + %\\de^2__..%k_ } / 2,")
NORMAL (L"where dots replacing indices indicate averaging over the range of "
	"that index. In the same way for the distances:")
LIST_ITEM (L"%z__%ijk_ = \\--{ %d^2__%ijk_ \\--  %d^2__%i.%k_ \\--  %d"
	"^2__%.%jk_ +  %d^2__%..%k_ } / 2.")
LIST_ITEM (L"%\\be__%ijk_ \\~~ %z__%ijk_ = \\su__%s=1..%numberOfDimensions_ "
	"%w__%ks_ %x__%is_ %x__%js_")
NORMAL (L"Translated into matrix algebra, the equation above translates to:")
LIST_ITEM (L"%B__%k_ \\~~ %Z__%k_ = %X %W__%k_ %X\\'p,")
NORMAL (L"where %X is a %numberOfPoints \\xx %numberOfDimensions configuration "
	"matrix, %W__%k_, a non-negative %numberOfDimensions \\xx "
	"%numberOfDimensions matrix with weights, and %B__%k_ "
	"the %k^^th^ slab of %\\be__%ijk_.")
NORMAL (L"This translates to the following INDSCAL loss function:")
FORMULA (L"%f(%X, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%k=1..%numberOfSources_ | %B__%k_ \\-- %X%W__%k_%X\\'p |^2")
NORMAL (L"")
MAN_END

MAN_BEGIN (L"INDSCAL analysis", L"djmw", 20120306)
INTRO (L"A method for @@individual difference scaling@ analysis in P\\s{RAAT}.")
NORMAL (L"An INDSCAL analysis can be performed on objects of type Distance.")
NORMAL (L"If you start with @Dissimilarity objects you first have to transform "
	"them to Distance objects.")
LIST_ITEM (L"\\bu @@Dissimilarity: To Distance...@")
NORMAL (L"If you start with a @Confusion you can use:")
LIST_ITEM (L"\\bu @@Confusion: To Dissimilarity (pdf)...@")
ENTRY (L"Examples")
LIST_ITEM (L"\\bu @@Distance: To Configuration (indscal)...@")
DEFINITION (L"Perform an INDSCAL analysis on one or more objects of type "
	"@Distance to calculate a @Configuration.")
LIST_ITEM (L"\\bu @@Distance & Configuration: To Configuration (indscal)...@")
DEFINITION (L"Perform an INDSCAL analysis on one or more objects of type "
	"@Distance to calculate a @Configuration. Use the selected Configuration "
	"object as the initial Configuration in the iteration process.")
ENTRY (L"Algorithm")
NORMAL (L"The function to be minimized in INDSCAL is the following:")
FORMULA (L"%f(%X, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%i=1..%numberOfSources_ |%S__%i_ \\-- %XW__%i_%X\\'p|^2")
NORMAL (L"where %X an unknown %numberOfPoints x %numberOfDimensions "
	"configuration matrix, the %W__%i_ are %numberOfSources unknown "
	"diagonal %numberOfDimensions x %numberOfDimensions matrices with weights, "
	"often called saliences, and the %S__%i_ are known symmetric "
	"matrices with scalar products of dimension %numberOfPoints x "
	"%numberOfPoints.")
NORMAL (L"In the absence of an algorithm that minimizes %f, @@Carroll & "
	"Chang (1970)@ resorted to the @CANDECOMP algorithm, which instead of the "
	"function given above minimizes the following function:")
FORMULA (L"%g(%X, %Y, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%i=1..%numberOfSources_ |%S__%i_ \\-- %XW__%i_%Y\\'p|^2.")
NORMAL (L"Carroll & Chang claimed that for most practical circumstances %X "
	"and %Y converge to matrices that will be columnwise proportional. "
	"However, INDSCAL does not only require symmetry of the solution, "
	"but also non-negativity of the weights. Both these aspects cannot be "
	"guaranteed with the CANDECOMP algorithm.")
NORMAL (L"@@Ten Berge, Kiers & Krijnen (1993)@ describe an algorithm that "
	"automatically satisfies symmetry because it solves %f directly, and, "
	"also, can guarantee non-negativity of the weights. "
	"This algorithm proceeds as follows:")
NORMAL (L"Let #%x__%h_ be the %h-th column of %X. We then write the function %f above as:")
FORMULA (L"%f(#%x__%h_, %w__1%h_, ..., %w__%numberOfSources %h_) = \\su__%i=1.."
	"%numberOfSources_ |%S__%ih_ \\-- #%x__%h_%w__%ih_#%x\\'p__%h_|^2,")
NORMAL (L"with %S__%ih_ defined as:")
FORMULA (L"%S__%ih_ = (%S__%i_ - \\su__%j\\=/%h, %j=1..%numberOfDimensions_ "
	"#%x__%j_%w__%ij_#%x\\'p__%j_).")
NORMAL (L"Without loss of generality we may require that")
FORMULA (L"#%x\\'p__%h_#%x__%h_ = 1")
NORMAL (L"Minimizing %f over #%x__%h_ is equivalent to minimizing")
FORMULA (L"\\su__%i=1..%numberOfSources_ |%S__%ih_|^2 \\-- 2tr \\su "
	"%S__%ih_#%x__%h_%w__%ih_#%x\\'p__%h_ + \\su %w^2__%ih_")
NORMAL (L"This amounts to maximizing")
FORMULA (L"%g(#%x__%h_) = #%x\\'p__%h_(\\su %w__%ih_%S__%ih_)#%x__%h_")
NORMAL (L"subject to #%x\\'p__%h_#%x__%h_ = 1. The solution for #%x__%h_ is "
	"the dominant eigenvector of (\\su %w__%ih_%S__%ih_), "
	"which can be determined with the power method (see @@Golub & van Loan "
	"(1996)@). The optimal value "
	"for the %w__%ih_, given that all other parameters are fixed:")
FORMULA (L"%w__%ih_ = #%x\\'p__%h_%S__%ih_#%x__%h_")
NORMAL (L"In an alternating least squares procedure we may update columns of "
	"%X and the diagonals of the %W matrices in any sensible order.")
MAN_END

MAN_BEGIN (L"Kruskal analysis", L"djmw", 19971201)
INTRO (L"One of the @@MDS models@ in P\\s{RAAT}.")
NORMAL (L"You can perform a Kruskal-type multidimensional scaling only on "
	"objects of type @Dissimilarity. Objects of other types first have to "
	"be converted to objects of Dissimilarity type.")
ENTRY (L"Example")
NORMAL (L"Convert a @Dissimilarity object into a @Configuration object.")
LIST_ITEM (L"\\bu @@Dissimilarity: To Configuration (monotone mds)...@")
DEFINITION (L"choose appropriate parameters")
LIST_ITEM (L"\\bu @@Dissimilarity & Configuration: Get stress (monotone mds)...@")
DEFINITION (L"choose stress-1 to obtain the value for the @stress according "
	"to Kruskal.")
ENTRY (L"How to get started")
NORMAL (L"You can create an example @Dissimilarity object with the @@Create "
	"letter R example...@ button which you can find under the "
	"##Multidimensional scaling# option in the #New menu.")
MAN_END

MAN_BEGIN (L"MDS models", L"djmw", 20101109)
INTRO (L"Multidimensional scaling (MDS) models are defined by specifying "
	"how given @Dissimilarity data, %\\de__%ij_, are "
	"mapped into distances of an %m-dimensional MDS @Configuration %#X. "
	"The mapping is specified by a %%representation function%, %f : "
	"%\\de__%ij_ \\-> %d__%ij_(#X), which specifies how "
	"dissimilarities should be related to the distances. The MDS analysis "
	"tries to find the configuration (in a given dimensionality) whose "
	"distances satisfy %f as closely as possible. "
	"This closeness is quantified by a badness-of-fit measure which is often "
	"called @stress.")
ENTRY (L"Representation functions")
NORMAL (L"In the application of MDS we try to find a configuration #X such that "
	"the following relations are satisfied as well as possible:")
FORMULA (L"%f(%\\de__%ij_) \\~~ %d__%ij_(#X)")
NORMAL (L"The numbers that result from applying  %f on  %\\de__%ij_ are sometimes "
	"called @disparities %d\\'p__%ij_. In most applications of MDS, besides "
	"the configuration #X, also the function %f is not "
	"completely specified, i.e., the exact parameters of %f are unknown and "
	"must also be estimated during the analysis. If no particular %f can be "
	"derived from a theoretical model, one often restricts %f to a particular "
	"class of functions on the basis of the scale level of the dissimilarity "
	"data. If the disparities are related to the proximities by a specific "
	"parametric function we speak of %metric MDS otherwise we speak of "
	"%ordinal or %%non-metric% MDS.")
LIST_ITEM (L"\\bu %absolute mds: %d\\'p__%ij_ = \\de__%ij_")
DEFINITION (L"No parameters need to be estimated.")
LIST_ITEM (L"\\bu %ratio mds: %d\\'p__%ij_ = %b \\.c \\de__%ij_,")
DEFINITION (L"where the value of %b can be estimated by a linear regression of "
	"%d__%ij_ on %\\de__%ij_.")
LIST_ITEM (L"\\bu %interval mds: %d\\'p__%ij_ = %a + %b \\.c %\\de__%ij_,")
DEFINITION (L"where the values of %a and %b can be estimated by a linear "
	"regression of %d__%ij_ on %\\de__%ij_.")
LIST_ITEM (L"\\bu %i-spline mds: %d\\'p__%ij_ = %i-spline(%\\de__%ij_),")
DEFINITION (L"where %i-spline(\\.c) is a smooth monotonically increasing "
	"@spline curve. The conceptual idea is that it is not possible to map "
	"all dissimilarities into disparities by one simple function.")
LIST_ITEM (L"\\bu %monotone mds: %d\\'p__%ij_ = %monotone(\\de__%ij_),")
DEFINITION (L"where %monotone(\\.c) is restricted to be a monotonic function "
	"that preserves the order of the dissimilarities:")
FORMULA (L"if %\\de__%ij_ < %\\de__%kl_, then %d__%ij_(#X) < %d__%kl_(#X)")
DEFINITION (L"If %\\de__%ij_ = %\\de__%kl_ and no particular constraint is involved for %d__%ij_(#X) "
	"and %d__%kl_(#X) this is referred to as the %%primary approach% to ties. The %%secondary "
	"approach% to ties requires that if %\\de__%ij_ = %\\de__%kl_, then also %d__%ij_(#X) = %d__%kl_(#X).")
NORMAL (L"More information on all aspects of multidimensional scaling can be found in: "
	"@@Borg & Groenen (1997)@ and @@Ramsay (1988)@.")
NORMAL (L"The most important object types used in Praat for MDS and the conversions between these types are "
	"shown in the following figure.")
PICTURE (6, 6, drawMDSClassRelations)
MAN_END

MAN_BEGIN (L"Measurement levels", L"djmw", 19970502)
INTRO (L"According to the measurement theory of @@Stevens (1951)@, there are four measurement levels, namely "
	"#Nominal, #Ordinal, #Interval and #Ratio. In the light of multidimensional scaling, the first "
	"two levels, Nominal and Ordinal, are often called %non-%metric. The last two are %metric.")
MAN_END

MAN_BEGIN (L"Multidimensional scaling", L"djmw", 20130502)
INTRO (L"This tutorial describes how you can use P\\s{RAAT} to "
	"perform ##M#ulti##D#imensional ##S#caling (MDS) analysis.")
NORMAL (L"MDS helps us to represent %dissimilarities between objects as "
	"%distances in a %%Euclidean space%. In effect, the more dissimilar two "
	"objects are, the larger the distance between the objects in the Euclidean "
	"space should be. The data types in P\\s{RAAT} that "
	"incorporate these notions are @Dissimilarity, @Distance and "
	"@Configuration.")
NORMAL (L"In essence, an MDS-analysis is performed when you select a "
	"Dissimilarity object and choose one of the ##To Configuration (xxx)...# "
	"commands to obtain a Configuration object. In the above, method (xxx) "
	"represents on of the possible @@MDS models|multidimensional scaling "
	"models@.")
ENTRY (L"MDS-analysis")
NORMAL (L"Let us first create a Dissimilarity object. You can for example "
	"@@Dissimilarity|create a Dissimilarity object from a file@. Here we "
	"will the use the Dissimilarity object from @@Create letter R example...|"
	"the letter R example@. We have chosen the default value (32.5) for the "
	"(uniform) noise range. Note that this may result in substantial "
	"distortions between the dissimilarities and the distances.")
NORMAL (L"Now you can do the following, for example:")
NORMAL (L"Select the Dissimilarity and choose @@Dissimilarity: To Configuration "
	"(monotone mds)...|To Configuration (monotone mds)...@, and  you perform "
	"a @@Kruskal analysis|kruskal@-like multidimensional scaling which "
	"results in a new "
	"Configuration object. (This Configuration could subsequently be used as "
	"the starting Configuration for a new MDS-analysis!).")
NORMAL (L"Select the Configuration and choose @@Configuration: Draw...|Draw...@ "
	"and the following picture will result")
PICTURE (4.0, 4.0, drawLetterRConfigurationExample2)
NORMAL (L"The following script summarizes:")
CODE (L"dissimilarity = do (\"Create letter R example...\", 32.5)")
CODE (L"configuration = do (\"To Configuration (monotone mds)...\", 2, \"Primary approach\", 0.00001, 50, 1)")
CODE (L"do (\"Draw...\", 1, 2, -0.8, 1.2, -0.8, 0.7, \"yes\")")
ENTRY (L"Obtaining the stress value")
NORMAL (L"Select the Dissimilarity and the Configuration together and query for "
	"the @stress value with: "
	"@@Dissimilarity & Configuration: Get stress (monotone mds)...|"
	"Get stress (monotone mds)...@. ")
NORMAL (L"The following script summarizes:")
CODE (L"selectObject (dissimilarity, configuration)")
CODE (L"do (\"Get stress (monotone mds)...\", \"Primary approach\", \"Kruskals's "
	"stress-1\")")
ENTRY (L"The Shepard diagram")
NORMAL (L"Select the Dissimilarity and the Configuration together to "
	"@@Dissimilarity & Configuration: Draw Shepard diagram...|"
	"draw the Shepard diagram@.")
PICTURE (4.0, 4.0, drawLetterRShepard)
NORMAL (L"The following script summarizes:")
CODE (L"selectObject (dissimilarity, configuration)")
CODE (L"do (\"Draw Shepard diagram...\", 0, 200, 0, 2.2, 1, \"+\", \"yes\")")
ENTRY (L"The (monotone) regression")
NORMAL (L"Select the Dissimilarity and the Configuration together to "
	"@@Dissimilarity & Configuration: Draw regression (monotone mds)...|"
	"draw the monotone regression@ of distances on dissimilarities.")
PICTURE (4.0, 4.0, drawLetterRRegression)
NORMAL (L"The following script summarizes:")
CODE (L"selectObject (dissimilarity, configuration)")
CODE (L"do (\"Draw monotone regresion...\", \"Primary approach\", 0, 200, 0, 2.2, 1, \"+\", \"yes\")")
NORMAL (L"When you enter %noiseRange = 0 in the form for the letter #R, perfect "
	"reconstruction is possible. The Shepard diagram then will show "
	"a perfectly smooth monotonically increasing function.")
ENTRY (L"Weighing the dissimilarities")
NORMAL (L"When you can't have equal confidence in all the number in the "
	"Dissimilarity object, you can give different weights to these numbers by "
	"associating a @Weight object with the Dissimilarity object. "
	"An easy way to do this is to select the Dissimilarity object and first "
	"choose @@Dissimilarity: To Weight|To Weight@. Then you might change the "
	"individual weights in the Weight object with the @@TableOfReal: Set "
	"value...| Set value...@ command (remember: make %w__%ij_ = %w__%ji_).")
NORMAL (L"The following script summarizes:")
CODE (L"selectObject (dissimilarity)")
CODE (L"weight = do (\"To Weight\")")
CODE (L"! Change [i][j] and [j][i] cells in the Weight object")
CODE (L"do (\"Set value...\", i, j, val)")
CODE (L"do (\"Set value...\", j, i, val)")
CODE (L"...")
CODE (L"! now we can do a weighed analysis.")
CODE (L"selectObject (dissimilarity, weight)")
CODE (L"do (\"To Configuration (monotone mds)...\", 2, \"Primary approach\", 0.00001, 50, 1)")
NORMAL (L"You can also query the @stress values with three objects selected. "
	"The following script summarizes:")
CODE (L"selectObject (dissimilarity, weight, configuration)")
CODE (L"do (\"Get stress (monotone mds)...\", \"Primary approach\", \"Kruskals's "
	"stress-1\")")
ENTRY (L"Using a start Configuration")
NORMAL (L"You could also use a Configuration object as a starting "
	"configuration in the minimization process. "
	"Lets assume that you are not satisfied with the stress value from the "
	"Configuration object that you obtained in the previous analysis. "
	"You can than use this Configuration object as a "
	"starting point for further analysis:")
NORMAL (L"The following script summarizes:")
CODE (L"selectObject (dissimilarity, configuration, weight)")
CODE (L"do (\"To Configuration (monotone mds)...\", 2, \"Primary approach\", 0.00001, 50, 1)")
ENTRY (L"Multiple Dissimilarity's (INDSCAL)")
NORMAL (L"When you have multiple Dissimilarity objects you can also perform "
	"@@individual difference scaling@ (often called @@INDSCAL analysis@). ")
NORMAL (L"As an example we can use an @@Create INDSCAL Carroll & Wish "
	"example...| example taken from Carrol & Wish@. "
	"Because INDSCAL only works on metrical data, we cannot use Dissimilarity "
	"objects directly. We have to transform them first @@Dissimilarity: To "
	"Distance...|to Distance@ objects.")
NORMAL (L"This type of analysis on multiple objects results in two new objects: "
	"a Configuration and a @Salience.")
MAN_END

MAN_BEGIN (L"Procrustes", L"djmw", 20010927)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Procrustes represents the special @@AffineTransform|"
	"affine transform@ that consists of a "
	"combination of a translation, a shape preserving transformation and a scaling (this scaling is often called %dilation). "
	"Because the transformation has to be shape preserving, only a combination of a rotation and a reflection is allowed. "
	"A configuration matrix #%X is transformed in the following way to a new configuration matrix #%Y: ")
FORMULA (L"#%Y = %s #%X #%T+ #1#%t',")
NORMAL (L"where %s is the scaling factor, #%T is the shape preserving transformation matrix, #%t is the translation vector, "
	"and #1 is the vector with only ones as its elements.")
NORMAL (L"For more information about the Procrustes transform and its algorithm "
	"see chapter 19 in @@Borg & Groenen (1997)@.")
MAN_END

MAN_BEGIN (L"Procrustes transform", L"djmw", 19980119)
INTRO (L"A transformation that only uses a combination of a translation, "
	"a scaling and a rigid transformation to transform one Configuration such that it "
	"matches as closely as possible another Configuration. ")
NORMAL (L"We speak of %%orthogonal Procrustes transform% when only the rigid "
	"transformation is allowed but no scaling or translation.")
NORMAL (L"For more information about the Procrustes transform and its algorithm "
	"see chapter 19 in @@Borg & Groenen (1997)@.")
MAN_END

MAN_BEGIN (L"Proximity", L"djmw", 19961008)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type #Proximity represents proximities between objects.")
ENTRY (L"Inside a Proximity")
NORMAL (L"With @Inspect you will see the following attributes:")
TAG (L"%numberOfRows, %numberOfColumns")
DEFINITION (L"the number of objects (%numberOfRows and %numberOfColumns are "
	"equal and \\>_1).")
TAG (L"%rowLabels, %columnLabels")
DEFINITION (L"the names associated with the objects (%rowLabels and "
	"%columnLabels are equal.")
TAG (L"%data [1..%numberOfRows][1..%numberOfColumns]")
DEFINITION (L"the proximities between the objects.")
MAN_END

MAN_BEGIN (L"Salience", L"djmw", 19980112)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"Elements %s__%ij_ in the "
	"Salience matrix represent the importance of dimension %j (in the "
	"@Configuration) for data source %i.")
ENTRY (L"Commands")
NORMAL (L"Creation, as a by-product of:")
LIST_ITEM (L"\\bu @@Distance: To Configuration (indscal)...")
LIST_ITEM (L"\\bu @@Distance: To Configuration (ytl)...")
MAN_END

MAN_BEGIN (L"ScalarProduct", L"djmw", 19980125)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type ScalarProduct represents scalar products %b__%ij_ "
	"between objects %i and %j in a metrical space.")
FORMULA (L"%b__%ij_ = \\su__%k=1..%numberOfDimensions_ %x__%ik_%x__%jk_,")
NORMAL (L"where %x__%ik_ and %x__%jk_ are the coordinates of the %k-th "
	"dimension of points %i and %j, respectively. From this definition one "
	"can see that scalar products, in contrast to distances, "
	"do change when the origin is shifted.")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu @@Distance: To ScalarProduct...@")
MAN_END

MAN_BEGIN (L"Similarity", L"djmw", 19961008)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Similarity represent a one-way "
	"table of similarities between \"objects\".")
ENTRY (L"Commands")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu @@Confusion: To Similarity...")
NORMAL (L"Drawing")
LIST_ITEM (L"\\bu ##Draw as numbers...")
LIST_ITEM (L"\\bu ##Draw as squares...")
NORMAL (L"Query")
LIST_ITEM (L"\\bu ##Get column mean (index)...")
LIST_ITEM (L"\\bu ##Get column mean (label)...")
LIST_ITEM (L"\\bu ##Get column stdev (index)...")
LIST_ITEM (L"\\bu ##Get column stdev (label)...")
NORMAL (L"Modification")
LIST_ITEM (L"\\bu ##Formula...")
LIST_ITEM (L"\\bu ##Remove column (index)...")
LIST_ITEM (L"\\bu ##Insert column (index)...")
LIST_ITEM (L"\\bu ##Set row label (index)...")
LIST_ITEM (L"\\bu ##Set row label (label)...")
LIST_ITEM (L"\\bu ##Set column label (index)...")
LIST_ITEM (L"\\bu ##Set column label (label)...")
NORMAL (L"Analysis")
LIST_ITEM (L"\\bu @@Similarity: To Dissimilarity...")
MAN_END

MAN_BEGIN (L"Similarity: To Dissimilarity...", L"djmw", 20040407)
INTRO (L"A command that creates a @Dissimilarity from every selected "
	"@Similarity.")
ENTRY (L"Setting")
TAG (L"##Maximum dissimilarity")
DEFINITION (L"determines the maximum dissimilarity possible. When the default "
	"value, 0.0, is chchosen%maximumDissimilarity "
	"is calculated as the maximum element in the Similarity object.")
ENTRY (L"Algorithm")
NORMAL (L"To obtain dissimilarities we 'reverse' similarities:")
FORMULA (L"%%dissimilarity__%ij_ = %maximumDissimilarity \\-- %similarity__%ij_")
NORMAL (L"In this way the order of dissimilarities is the reverse of the order "
	"of the similarities.")
MAN_END

MAN_BEGIN (L"smacof", L"djmw", 19980119)
INTRO (L"Scaling by Majorizing a Complicated Function, the iterative algorithm "
	"to find an optimal Configuration.")
LIST_ITEM (L"1.  Initialize")
LIST_ITEM (L"   1.a. Get initial Configuration #Z")
LIST_ITEM (L"   1.b. Set stress %\\si__%n_^^[0]^ to a very large value.")
LIST_ITEM (L"   1.c. Set iteration counter %k = 0")
LIST_ITEM (L"2.  Increase iteration counter by one: %k = %k + 1")
LIST_ITEM (L"3.  Calculate distances %d__%ij_(#Z).")
LIST_ITEM (L"4.  Transform dissimilarities %\\de__%ij_ into disparities "
	"%d\\'p__%ij_.")
LIST_ITEM (L"5.  Standardize the disparities so that %\\et__%d\\'p_^2 = "
	"%n(%n\\--1)/2.")
LIST_ITEM (L"6.  Compute the Guttman transform #X^^[%k]^ of #Z.")
LIST_ITEM (L"7.  Compute new distances %d__%ij_(#X^^[%k]^).")
LIST_ITEM (L"8.  Compute normalized stress %\\si__%n_ (#d\\'p, #X^^[%k]^)")
LIST_ITEM (L"9.  If |%\\si__%n_^^[%k]^ \\-- %\\si__%n_^^[%k\\--1]^| / %\\si__%n_"
	"^^[%k\\--1]^ < %\\ep or %k > %maximumNumberOfIterations, then stop")
LIST_ITEM (L"10. Set #Z = #X^^[%k]^, and go to 2.")
NORMAL (L"This algorithm goes back to @@De Leeuw (1977)@.")
MAN_END

MAN_BEGIN (L"spline", L"djmw", 20121101)
INTRO (L"A spline function %f is a piecewise polynomial function defined on "
	"an interval [%x__%min_, %x__%max_] "
	"with specified continuity constraints, i.e., when the interval [%x__%min_,"
	" %x__%max_] is subdivided by points %\\xi__%i_ such that %x__%min_ = "
	"%\\xi__%1_ < ... < %\\xi__%q_ = %%x__%max_, then within each subinterval "
	"[%\\xi__%j_, %\\xi__%j+1_) the function is a polynomial %P__%j_ of "
	"specified degree %k.")
NORMAL (L"A %%knot sequence% %t = {%t__1_, ..., %t__%n+%k_}, where %n is the "
	"number of free parameters that specify the spline function, is derived "
	"from the %\\xi__%i_ by placing knots at the boundary values %\\xi__%i_ "
	"according to the order of continuity at that boundary. The most common "
	"continuity characteristics imposed on %f request that for adjacent "
	"polynomials the derivatives up to order %k\\--2 match. For example, "
	"the knot sequence of a spline of order %k for a partition of [%x__%min_,"
	" %%x__%max_] into three intervals (%q = 4) will be %t__1_ = ... = "
	"%t__%k_ = %x__%min_ (=%\\xi__1_), %t__%k+1_ = %\\xi__2_, %t__%k+2_ = "
	"%\\xi__3_ , %t__%k+3_ = ... = %t__%k+%k+2_ = %x__%max_ (= %\\xi__4_). "
	"This is called a %simple knot sequence, because all interior knots are "
	"simple. The number of free parameters %n for this case obeys a simple "
	"formula:")
FORMULA (L"%n = %numberOfInteriorKnots + %order.")
NORMAL (L"With suitable basis functions, for example, the M-spline family "
	"%M__%i_(%x|%k, %t), %i=1..%n, we can write any spline %f in the form:")
FORMULA (L"%f = \\su__%i=1..%n_ %a__%i_%M__%i_,")
NORMAL (L"where the %M__%i_ are defined by the following recursive formula's:")
FORMULA (L"%M__%i_(%x|1,%t) = 1 / (%t__%i+1_ \\-- %t__%i_),           %t__%i_ "
	"\\<_ %x < %t__%i+1_, 0 otherwise")
FORMULA (L"%M__%i_(%x|%k,%t) = %k [(%x\\--%t__%i_)%M__%i_(%x|%k\\--1,%t) + "
	"(%t__%i+%k_\\--%x)%M__%i+1_(%x|%k\\--1,%t)] / "
	"((%k\\--1)(%t__%i+%k_\\--%t__%i_))")
NORMAL (L"These %M__%i_ are localized because %M__%i_(%x|%k,%t) > 0 only when "
	"%t__%i_ \\<_ %x < %t__%i+%k_ and zero otherwise. Also, we have \\in"
	" M__%i_(%x)%dx = 1. Because of this localization a change in coefficient "
	"%a__%i_ will only effect %f within this interval.")
NORMAL (L"The following picture shows an M-spline of order 3 on the interval "
	"[0, 1], with three interior knots at 0.3, 0.5 and 0.6.")
PICTURE (5.0, 5.0, drawMsplineExample)
NORMAL (L"Because the M-splines are nonnegative, %monotone splines% can be "
	"derived from them by %integration:")
FORMULA (L"%I__%i_(%x|%k,%t) = \\in__%xmin_^%x %M__%i_(%u|%k,%t) %du")
NORMAL (L"Because each %M__%i_(%x|%k, %t) is a piecewise polynomial of degree"
	" %k\\--1, each %I__%i_ will be of degree %k. Now we can write:")
FORMULA (L"%f = \\su__%i=1..%n_ %b__%i_%I__%i_(%x|%k,%t)")
NORMAL (L"We can use an M-spline of order %k+1 with a simple knot sequence %t, "
	"for which %t__%j_ \\<_ x < %t__%j+1_, to put "
	"the I-spline of order %k into a more convenient form:")
FORMULA (L"%I__%i_(%x|%k,%t) = 0,     %i > %j")
FORMULA (L"%I__%i_(%x|%k,%t) = \\su__%m=%i+1..%j_ (%t__%m+%k+1_\\--"
	"%t__%m_)%M__%m_(%x|%k+1,%t)/(%k+1),     %j\\--%k \\<_ %i \\<_ %j")
FORMULA (L"%I__%i_(%x|%k,%t) = 1,     %i < %j\\--%k")
NORMAL (L"The following figure shows the I-splines that were derived from "
	"the M-splines above.")
PICTURE (5.0, 5.0, drawIsplineExample)
NORMAL (L"These spline formula's were taken from @@Ramsay (1988)@ and the "
	"errors in his I-spline formulas were corrected.")
MAN_END

MAN_BEGIN (L"stress", L"djmw", 19980108)
INTRO (L"A badness-of-fit measure for the entire MDS representation.")
NORMAL (L"Several measures exist.")
ENTRY (L"Raw stress")
FORMULA (L"%\\si__%r_ (#d\\'p, #X) = \\su__%i<%j_ %w__%ij_(%d\\'p__%ij_ \\-- "
	"%d__%ij_(#X))^2")
FORMULA (L"= \\su__%i<%j_ %w__%ij_%d\\'p__%ij_^2 + \\su__%i<%j_ "
	"%w__%ij_%d__%ij_^2(#X) \\-- "
	"2 \\su__%i<%j_ %w__%ij_%d\\'p__%ij_%d__%ij_(#X)")
FORMULA (L"= %\\et__%d\\'p_^2 + %\\et^2(#X) \\-- 2%\\ro(#d\\'p, #X)")
NORMAL (L"where the %d\\'p__%ij_ are the @disparities that are the result "
	"from the transformation of the dissimilarities, i.e., %f(%\\de__%ij_). "
	"Raw stress can be misleading because it is dependent on the normalization "
	"of the disparities. The following measure tries to circumvent this "
	"inconvenience.")
ENTRY (L"Normalized stress")
FORMULA (L"%\\si__%n_ = \\si__%r_ / %\\et__%d\\'p_^2")
NORMAL (L"This is the stress function that we minimize by iterative "
	"majorization. It goes back to @@De Leeuw (1977)@.")
ENTRY (L"Kruskal's stress-1")
FORMULA (L"%\\si__1_ = \\Vr (\\su__%i<%j_ %w__%ij_(%d\\'p__%ij_ \\-- "
	"%d__%ij_(#X))^2 / \\su__%i<%j_ %w__%ij_%d__%ij_^2(#X))^^1/2^")
NORMAL (L"In this measure, which is due to @@Kruskal (1964)@, stress is "
	"expressed in relation to the size of #X.")
ENTRY (L"Kruskal's stress-2")
FORMULA (L"%\\si__2_ = \\Vr (\\su__%i<%j_ %w__%ij_(%d\\'p__%ij_ \\-- "
	"%d__%ij_(#X))^2 / \\su__%i<%j_ %w__%ij_(%d__%ij_(#X) - "
	"%averageDistance)^2)^^1/2^.")
NORMAL (L"In general, this measure results in a stress value that is "
	"approximately twice the value for stress-1.")
ENTRY (L"Relation between %\\si__1_ and %\\si__n_")
NORMAL (L"When we have calculated %\\si__%n_ for Configuration #X, "
	"disparities #d\\'p and Weight #W we cannot "
	"directly use #X, #d\\'p and #W to calculate %\\si__1_ because the "
	"scale of #X is not necessarily optimal "
	"for %\\si__1_. We allow therefore a scale factor %b > 0 and "
	"try to calculate  %\\si__1_ (#d\\'p, %b #X). We minimize the resulting "
	"expression for %b and substitute "
	"the result back into the formula for stress, i.e.,")
FORMULA (L"%\\si__1_^2 (#d\\'p, %b #X) = (%\\et__%d\\'p_^2 + %b^2 %\\et^2(#X) "
	"\\-- 2 %b %\\ro(#d\\'p, #X)) / %b^2 %\\et^2(#X)")
FORMULA (L"d%\\si__1_^2 (%b) / d%b == 0, gives")
FORMULA (L"%b = %\\et__%d\\'p_^2 / %\\ro")
FORMULA (L"%\\si__1_^2 =  (1 - %\\ro^2 / (%\\et__%d\\'p_^2\\.c%\\et^2(#X)))")
NORMAL (L"This means that %\\si__1_ = \\Vr %\\si__%n_.")
ENTRY (L"Relation between %\\si__2_ and %\\si__n_")
NORMAL (L"We can do the same trick as before for %\\si__2_:")
FORMULA (L"%\\si__2_^2 (#d\\'p, %b #X) = (%\\et__%d\\'p_^2 + %b^2 %\\et^2(#X) "
	"\\-- 2 %b %\\ro(#d\\'p, #X)) / "
	"(%b^2 \\su__%i<%j_ %w__%ij_(%d__%ij_(#X) - %averageDistance)^2)")
NORMAL (L"From which we derive:")
FORMULA (L"%\\si__2_ = \\Vr ((%\\et__%d\\'p_^2 \\.c %\\et^2(#X) - "
	"%\\ro^2(#d\\'p, #X)) / (%\\et__%d\\'p_^2 \\.c \\su__%i<%j_ "
	"%w__%ij_(%d__%ij_(#X) - %averageDistance)^2))")
MAN_END

MAN_BEGIN (L"TableOfReal: Centre columns", L"djmw", 19980422)
INTRO (L"A command that centres the columns in the selected @TableOfReal "
	"objects.")
ENTRY (L"Algorithm")
NORMAL (L"The new values in the table, %x\\'p__%ij_, will be:")
FORMULA (L"%x\\'p__%ij_ = %x__%ij_ \\-- %x__\\.c%j_,")
NORMAL (L"where")
FORMULA (L"%x__\\.c%j_ = \\su__%i=1..%numberOfRows_ %x__%ij_ / %numberOfRows,")
NORMAL (L"the average of column %j.")
MAN_END

MAN_BEGIN (L"TableOfReal: Centre rows", L"djmw", 19980422)
INTRO (L"A command that centres the rows in the selected @TableOfReal objects.")
ENTRY (L"Algorithm")
NORMAL (L"The new values in the table, %x\\'p__%ij_, will be:")
FORMULA (L"%x\\'p__%ij_ = %x__%ij_ \\-- %x__%i\\.c_,")
NORMAL (L"where")
FORMULA (L"%x__%i\\.c_ = \\su__%j=1..%numberOfColumns_ %x__%ij_ / "
	"%numberOfColumns,")
NORMAL (L"the average of row %i.")
MAN_END

MAN_BEGIN (L"TableOfReal: Get table norm", L"djmw", 19980422)
INTRO (L"A command that returns the norm of the selected @TableOfReal object.")
ENTRY (L"Algorithm")
NORMAL (L"Returns: sqrt (\\su__%i=1..%numberOfRows_ \\su__%j=1..%numberOfColumns"
	"_ %x__%ij_^2).")
MAN_END

MAN_BEGIN (L"TableOfReal: Normalize columns...", L"djmw", 19980422)
INTRO (L"A command that normalizes the columns in the selected @TableOfReal "
	"objects.")
ENTRY (L"Setting")
TAG (L"##Norm")
DEFINITION (L"determines the sum of the squared elements in each column after "
	"normalization.")
ENTRY (L"Algorithm")
NORMAL (L"All elements %x__%ij_ in each column %j=1..%numberOfColumns will be "
	"multiplied by sqrt (%norm / \\su__%i=1..%numberOfRows_ %x__%ij_^2).")
MAN_END

MAN_BEGIN (L"TableOfReal: Normalize rows...", L"djmw", 19980422)
INTRO (L"A command that normalizes the rows in the selected @TableOfReal "
	"objects.")
ENTRY (L"Setting")
TAG (L"##Norm")
DEFINITION (L"determines the sum of the squared elements in each row after "
	"normalization.")
ENTRY (L"Algorithm")
NORMAL (L"All elements %x__%ij_ in each row %i=1..%numberOfRows will be "
	"multiplied by sqrt (%norm / \\su__%j=1..%numberOfColumns_ %x__%ij_^2).")
MAN_END

MAN_BEGIN (L"TableOfReal: Normalize table...", L"djmw", 19980422)
INTRO (L"A command that normalizes the elements in the selected @TableOfReal "
	"objects.")
ENTRY (L"Setting")
TAG (L"##Norm")
DEFINITION (L"determines the sum of the squared elements after normalization.")
ENTRY (L"Algorithm")
NORMAL (L"All elements %x__%ij_ will be multiplied by "
	"sqrt (%norm / \\su__%i=1..%numberOfRows_ \\su__%j=1..%numberOfColumns_"
	" %x__%ij_^2.")
MAN_END

MAN_BEGIN (L"Weight", L"djmw", 19980108)
INTRO (L"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (L"An object of type Weight represents a matrix "
	"with weights %w__%ij_.")
NORMAL (L"An object of type Weight selected together with an onject of type"
	" @Dissimilarity can be used to make distinctions in the importance of the"
	" contribution of each individual dissimilarity %\\de__%ij_ to "
	"@stress and therefore to the final configuration. Weights can be "
	"used for instance to code for missing values, i.e., take %w__%ij_ = 0 if "
	"dissimilarity %\\de__%ij_ is missing and %w__%ij_ = 1 if %\\de__%ij_ is"
	" known.")
ENTRY (L"Commands")
NORMAL (L"Creation")
LIST_ITEM (L"\\bu @@Dissimilarity: To Weight")
NORMAL (L"Analysis")
NORMAL (L"See @@Dissimilarity & Weight: To Configuration...@ for help on the "
	"following analysis items: ")
LIST_ITEM (L"\\bu ##Dissimilarity & Weight: To Configuration (monotone mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Weight: To Configuration (i-spline mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Weight: To Configuration (interval mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Weight: To Configuration (ratio mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Weight: To Configuration (absolute mds)...")
NORMAL (L"Query")
NORMAL (L"See @@Dissimilarity & Configuration & Weight: Get stress...@ for help "
	"on the following query items: ")
LIST_ITEM (L"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(monotone mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(i-spline mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(interval mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(ratio mds)...")
LIST_ITEM (L"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(absolute mds)...")
MAN_END

/************ references ***********************************************/

MAN_BEGIN (L"Abramowitz & Stegun (1970)", L"djmw", 19971201)
NORMAL (L"M. Abramowitz & I. Stegun (1970): %%Handbook of mathematical "
	"functions%. New York: Dover Publications.")
MAN_END

MAN_BEGIN (L"Borg & Groenen (1997)", L"djmw", 19971219)
NORMAL (L"I. Borg & P. Groenen (1997): %%Modern multidimensional scaling: "
	"theory and applications%. Springer.")
MAN_END

MAN_BEGIN (L"Brokken (1983)", L"djmw", 19980406)
NORMAL (L" F.B. Brokken (1983): \"Orthogonal Procrustes rotation maximizing "
	"congruence.\" %Psychometrika #48: 343\\--352.")
MAN_END

MAN_BEGIN (L"Cailliez (1983)", L"djmw", 19971201)
NORMAL (L" F. Cailliez (1983): \"The analytical solution of the additive "
	"constant problem.\" %Psychometrika #48, 305-308.")
MAN_END

MAN_BEGIN (L"Carroll & Chang (1970)", L"djmw", 19971201)
NORMAL (L"J.D. Carroll & J.-J. Chang, (1970): \"Analysis of Individual "
	"Differences in Multidimensional scaling via an N-way generalization of "
	"\"Eckart-Young\" Decomposition.\" %Psychometrika #35: 283\\--319.")
MAN_END

MAN_BEGIN (L"Carroll & Wish (1974)", L"djmw", 19971201)
NORMAL (L"J.D. Carroll & M. Wish, (1974): \"Models and methods for three-way "
	"multidimensional scaling.\" In D.H. Krantz, R.C. Atkinson, R.D. Luce & "
	"P. Suppes (eds.): %%Contemporary developments in mathematical psychology: "
	"Vol. 2 Measurement, psychophysics, and neural "
	"information processing%, 283\\--319. New York: Academic Press.")
MAN_END

MAN_BEGIN (L"De Leeuw (1977)", L"djmw", 19971201)
NORMAL (L"J. de Leeuw (1977): \"Applications of convex analysis to "
	"multidimensional scaling.\" In J.R. Barra, F. Brodeau, G. Romier & "
	"B. van Cutsem (eds.): %%Recent developments in statistics%. Amsterdam: "
	"North-Holland. 133\\--145.")
MAN_END

MAN_BEGIN (L"De Leeuw & Pruzansky (1978)", L"djmw", 19971201)
NORMAL (L"J. de Leeuw & S. Pruzansky (1978): \"A new computational method to "
	"fit the weighted Euclidean distance model.\" %Psychometrika #43: 479\\--490.")
MAN_END

MAN_BEGIN (L"Gifi (1990)", L"djmw", 19971207)
NORMAL (L"A. Gifi (1990): %%Nonlinear multivariate analysis%. John Wiley & "
	"Sons Ltd., reprint 1996.")
MAN_END

MAN_BEGIN (L"Golub & van Loan (1996)", L"djmw", 19971207)
NORMAL (L"G. Golub & C. van Loan (1996): %%Matrix computations%. Third edition. "
	"London: The Johns Hopkins University Press.")   // ??
MAN_END

MAN_BEGIN (L"Green, Carmone & Smith (1989)", L"djmw", 19971201)
NORMAL (L"P. Green, F. Carmone, S. Smith (1989): "
	"%%Multidimensional scaling: concepts and applications%. Section 3. Allyn and Bacon.")
MAN_END

MAN_BEGIN (L"Kaiser (1958)", L"djmw", 19980404)
NORMAL (L" H.F. Kaiser (1958): \"The varimax criterion for analytic rotation "
	"in factor analysis.\" %Psychometrika #23: 187\\--200.")
MAN_END

MAN_BEGIN (L"Kiers & Groenen (1996)", L"djmw", 19971219)
NORMAL (L"H.A.L. Kiers & P. Groenen (1996): \"A monotonically convergent "
	"algorithm for orthogonal congruence rotation.\" %Psychometrika #61: "
	"375\\--389.")
MAN_END

MAN_BEGIN (L"Klein, Plomp & Pols (1970)", L"djmw", 19971201)
NORMAL (L" W. Klein, R. Plomp, & L.C.W. Pols (1970): \"Vowel Spectra, "
	"Vowel Spaces, and Vowel Identification.\" %%Journal of the Acoustical Society of America% #48: 999\\--1009.")
MAN_END

MAN_BEGIN (L"Kruskal (1964)", L"djmw", 19971201)
NORMAL (L"J.B. Kruskal (1964): \"Nonmetric multidimensional scaling: a "
	"numerical method.\" %Psychometrika #29: 115\\--129.")
MAN_END

MAN_BEGIN (L"Ramsay (1988)", L"djmw", 19980106)
NORMAL (L"J.O. Ramsay (1988): \"Monotone regression splines in action.\" "
	"%%Statistical Science% #3: 425\\--461.")
MAN_END

MAN_BEGIN (L"Stevens (1951)", L"djmw", 19971201)
NORMAL (L"S.S. Stevens (1951): \"Mathematics, measurement, and psychophysics.\" "
	"In S.S. Stevens (ed.): %%Handbook of experimental psychology%. New York: "
	"Wiley.")
MAN_END

MAN_BEGIN (L"Takane, Young & de Leeuw (1976)", L"djmw", 19971201)
NORMAL (L"Y. Takane, F. Young, J. de Leeuw (1976): \"Non-metric individual "
	"differences multidimensional scaling: an alternating least squares method "
	"with optimal scaling features.\" %Psychometrika #42: 7\\--67.")
MAN_END

MAN_BEGIN (L"Ten Berge (1995)", L"djmw", 19980404)
NORMAL (L"J.M.F. ten Berge (1995): \"Suppressing permutations or rigid planar "
	"rotations: a remedy against nonoptimal varimax rotations.\" "
	"%Psychometrika #60, 437\\--446.")
MAN_END

MAN_BEGIN (L"Ten Berge, Kiers & Krijnen (1993)", L"djmw", 19971207)
NORMAL (L"J.M.F. ten Berge, H.A.L. Kiers & W.P. Krijnen (1993): \"Computational "
	"solutions for the problem of negative saliences and nonsymmetry in "
	"INDSCAL.\" %%Journal of Classification% #10: 115\\--124.")
MAN_END

MAN_BEGIN (L"Torgerson (1958)", L"djmw", 19971201)
NORMAL (L"W.S. Torgerson (1958): %%Theory and methods of scaling%. New York: Wiley.")
MAN_END

MAN_BEGIN (L"Young, Takane & Lewyckyj (1978)", L"djmw", 19971201)
NORMAL (L"F.W. Young, Y. Takane & R. Lewyckyj (1978): "
	"\"Three notes on ALSCAL.\" %Psychometrika #43: 433\\--435.")
MAN_END

MAN_BEGIN (L"Weller & Romney (1990)", L"djmw", 19971216)
NORMAL (L"S.C. Weller & A.K. Romney (1990): %%Metric Scaling: "
	"correspondence analysis%. Sage University Paper Series on Quantitative "
	"Applications in the Social Sciences 07-075. Newbury Park, CA: Sage.")
MAN_END

}

/* End of file manual_MDS.cpp */
