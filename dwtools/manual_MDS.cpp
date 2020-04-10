/* manual_MDS.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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
	Configuration_draw (me.get(), g, 1, 2, -6, 4, -7, 5, 0, 1, U"", true);
}

static void drawLetterRConfigurationExample2 (Graphics g)
{
	autoConfiguration me = Configuration_createLetterRExample (2);
	Configuration_draw (me.get(), g, 1, 2, -0.8, 1.2, -0.8, 0.7, 0, 1, U"", true);
}

static void drawLetterRShepard (Graphics g)
{
	autoDissimilarity d = Dissimilarity_createLetterRExample (32.5);
	autoConfiguration c = Configuration_createLetterRExample (2);
	Dissimilarity_Configuration_drawShepardDiagram (d.get(), c.get(), g, 0, 200, 0, 2.2, 1, U"+", true);
}

static void drawLetterRRegression (Graphics g)
{
	autoDissimilarity d = Dissimilarity_createLetterRExample (32.5);
	autoConfiguration c = Configuration_createLetterRExample (2);
	Dissimilarity_Configuration_drawMonotoneRegression (d.get(), c.get(), g, kMDS_TiesHandling::PRIMARY_APPROACH, 0, 200, 0, 2.2, 1, U"+", true);
}

static void drawCarrollWishConfigurationExample (Graphics g)
{
	autoConfiguration me = Configuration_createCarrollWishExample ();
	Graphics_setWindow (g, -2, 2, -2, 2);
	Configuration_draw (me.get(), g, 1, 2, -2, 2, -2, 2, 0, 1, U"", true);
}

static void drawCarrollWishSalienceExample (Graphics g)
{
	autoSalience me = Salience_createCarrollWishExample ();
	Salience_draw (me.get(), g, 1, 2, true);
}

static void drawMsplineExample (Graphics g)
{
	drawSplines (g, 0, 1, 0, 10, kMDS_splineType::M_SPLINE, 3, U"0.3 0.5 0.6", true);
}

static void drawIsplineExample (Graphics g)
{
	drawSplines (g, 0, 1, 0, 1.5, kMDS_splineType::I_SPLINE, 3, U"0.3 0.5 0.6", true);
}

void manual_MDS_init (ManPages me);
void manual_MDS_init (ManPages me)
{

MAN_BEGIN (U"CANDECOMP", U"djmw", 19971201)
ENTRY (U"An algorithm to solve the INDSCAL problem.")
NORMAL (U"In the analysis of the INDSCAL three-way data matrix (%numberOfPoints "
	"\\xx %numberOfDimensions \\xx %numberOfSources) we seek to minimize the "
	"function: ")
FORMULA (U"%f(%X, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%i=1..%numberOfSources_ | %S__%i_ \\-- %X%W__%i_%X\\'p |^2")
NORMAL (U"where %S__%i_ is a known symmetric %numberOfPoints \\xx %numberOfPoints "
	"matrix with scalar products of distances for source %i, %X is the unknown configuration "
	"%numberOfPoints \\xx %numberOfDimensions matrix, %X\\'p its transpose, and, %W__%i_ is "
	"the diagonal %numberOfDimensions \\xx %numberOfDimensions weight matrix for source %i. The function "
	"above has no analytical solution for %X and the %W__%i_. It can be solved, however, "
	"by an iterative procedure which Carroll & Chang have christened CANDECOMP "
	"(CANonical DECOMPosition). This method minimizes, instead of the function "
	"given above, the following function:")
LIST_ITEM (U"%g(%X, %Y, %W__1_,..., %W__%numberOfSources_) = \\su__%i=1..%numberOfSources_ "
	"| %S__%i_ \\-- %X%W__%i_%Y\\'p |^2")
NORMAL (U"where %X and %Y are both %numberOfPoints \\xx %numberOfDimensions configuration matrices.")
NORMAL (U"The algorithm proceeds as follows:")
NORMAL (U"1. Initialize the $W matrices and the configuration matrix %X. This can for example be "
	"done according to a procedure given in @@Young, Takane & Lewyckyj (1978)@.")
NORMAL (U"2. An alternating least squares minimization process is started as described that "
	"sequentially updates %Y, %X an %W (@@Carroll & Chang (1970)@):")
LIST_ITEM (U"2.1. Solve for a new %Y given %X and the %W__%i_")
LIST_ITEM (U"2.2. Solve for a new %X given the %W__%i_ and the new %Y.")
LIST_ITEM (U"2.3. Solve for the %W__%i_ given the new %X and %Y.")
NORMAL (U"Evaluate the goodness-of-fit criterion and either repeat the minimization sequence "
	"(2.1\\--2.3) or continue.")
NORMAL (U"3. Done: make %Y equal to %X and solve a last time for the %W__%i_.")
NORMAL (U"Note: during the minimization the following constraints are effective:")
LIST_ITEM (U"The configuration should be centered.")
LIST_ITEM (U"The sum of squared coordinates in the configuration space is one for "
	"each dimension, i.e., the configuration always has unit variance in each dimension.")
MAN_END

MAN_BEGIN (U"Configuration", U"djmw", 20101102)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"An object of type Configuration represents the positions of a number "
	"of labelled points in a multidimensional space.")
ENTRY (U"How to create a Configuration")
NORMAL (U"From the New menu:")
LIST_ITEM (U"  \\bu @@Create Configuration...")
NORMAL (U"By multidimensional scaling:")
LIST_ITEM (U"  \\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity: To Configuration (absolute mds)...")
NORMAL (U"By multidimensional scaling with weights (@@Dissimilarity & Weight: To Configuration...@):")
LIST_ITEM (U"  \\bu ##Dissimilarity & Weight: To Configuration (monotone mds)...")
LIST_ITEM (U"  \\bu ##Dissimilarity & Weight: To Configuration (i-spline mds)...")
LIST_ITEM (U"  \\bu ##Dissimilarity & Weight: To Configuration (interval mds)...")
LIST_ITEM (U"  \\bu ##Dissimilarity & Weight: To Configuration (ratio mds)...")
LIST_ITEM (U"  \\bu ##Dissimilarity & Weight: To Configuration (absolute mds)...")
NORMAL (U"By multidimensional scaling with a start Configuration:")
LIST_ITEM (U"  \\bu @@Dissimilarity & Configuration: To Configuration (monotone mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity & Configuration: To Configuration (i-spline mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity & Configuration: To Configuration (interval mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity & Configuration: To Configuration (ratio mds)...")
LIST_ITEM (U"  \\bu @@Dissimilarity & Configuration: To Configuration (absolute mds)...")
NORMAL (U"By transforming an existing Configuration:")
LIST_ITEM (U"  \\bu @@Configuration: To Configuration (varimax)...")
LIST_ITEM (U"  \\bu @@Configuration & AffineTransform: To Configuration")
LIST_ITEM (U"  \\bu @@Configuration & Procrustes: To Configuration")
NORMAL (U"From @@Principal component analysis@:")
LIST_ITEM (U"  \\bu @@TableOfReal: To Configuration (pca)...")
LIST_ITEM (U"  \\bu @@PCA & TableOfReal: To Configuration...")
NORMAL (U"From @@Discriminant analysis@:")
LIST_ITEM (U"@@TableOfReal: To Configuration (lda)...")
LIST_ITEM (U"  \\bu @@Discriminant & TableOfReal: To Configuration...")
ENTRY (U"How to draw a Configuration")
LIST_ITEM (U"\\bu @@Configuration: Draw...")
LIST_ITEM (U"\\bu ##Configuration: Draw as numbers...")
LIST_ITEM (U"\\bu ##Configuration: Draw as squares...")
ENTRY (U"How to modify a Configuration")
LIST_ITEM (U"\\bu @@Configuration: Randomize")
LIST_ITEM (U"\\bu @@Configuration: Rotate (pc)@ (to principal directions)")
LIST_ITEM (U"\\bu @@Configuration: Rotate...@ (in a plane around the origin)")
LIST_ITEM (U"\\bu @@Configuration: Invert dimension...")
LIST_ITEM (U"\\bu @@Configuration: Normalize...")
ENTRY (U"Inside a Configuration")
NORMAL (U"With @Inspect you will see the following attributes:")
TAG (U"%numberOfRows")
DEFINITION (U"the number of points (%numberOfPoints\\>_1).")
TAG (U"%numberOfColumns")
DEFINITION (U"the dimension of the space (%numberOfDimensions\\>_1).")
TAG (U"%rowLabels")
DEFINITION (U"the names associated with the points.")
TAG (U"%columnLabels")
DEFINITION (U"the names for the dimensions.")
TAG (U"%data [1..%numberOfPoints][1..%numberOfDimensions]")
DEFINITION (U"the coordinates of the points.")
TAG (U"%metric")
DEFINITION (U"determines the way distances between points are measured. In general "
	"the distance between points #x__%i_ and #x__%j_ is:")
FORMULA (U"%d__%ij_ = "
	"(\\su__%k=1..%numberOfDimensions_ %w__%k_ |%x__%ik_ \\-- "
	"%x__%jk_|^^%metric^)^^1/%metric^")
DEFINITION (U"For Euclidean distances %metric is 2.")
TAG (U"%w [1..%numberOfDimensions]")
DEFINITION (U"weight given to each dimension in the distance calculation.")
MAN_END

MAN_BEGIN (U"Configuration: Centralize", U"djmw", 19980413)
INTRO (U"Makes the centre of the selected @Configuration equal to the origin.")
NORMAL (U"")
MAN_END

MAN_BEGIN (U"Configuration: Draw...", U"djmw", 20040407)
INTRO (U"Draws a projection of the selected @Configuration on a coordinate plane.")
ENTRY (U"Settings")
TAG (U"##X-coordinate#, ##Y-coordinate#")
DEFINITION (U"control the dimensions that will show in the plot.")
TAG (U"##xmin#, ##xmax#; ##ymin#, ##ymax#")
DEFINITION (U"range for horizontal and vertical axes, respectively.")
TAG (U"##Garnish")
DEFINITION (U"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (U"Configuration: Invert dimension...", U"djmw", 20040407)
INTRO (U"Inverts one dimension of a @Configuration.")
NORMAL (U"Setting")
TAG (U"##Dimension")
DEFINITION (U"the dimension that has to be inverted.")
ENTRY (U"Behaviour")
NORMAL (U"For all points %i=1..%numberOfPoints: if %j == %dimension then %x__%ij_ "
	"= \\--%x__%ij_.")
MAN_END

MAN_BEGIN (U"Configuration: Normalize...", U"djmw", 20040407)
INTRO (U"Normalizes the selected @Configuration.")
ENTRY (U"Settings")
TAG (U"##Sum of squares# (standard value: 0.0)")
DEFINITION (U"The desired value for the variance.")
TAG (U"##Each dimension separately")
DEFINITION (U"When on, the sum of squares in each dimension (column) will be scaled to %sumOfSquares "
	"When off, the sum of squares of all the matrix elements will equal %sumOfSquares.")
NORMAL (U"With the default value (0.0) for %sumOfSquares, and %eachDimensionSeparately chosen, "
	"an INDSCAL-like normalization is applied: the sum of squares for each column is scaled to "
	"equal 1.0. When %eachDimensionSeparately is off, a Kruskal-like normalization is applied: "
	"the sum of squares of the whole matrix is scaled equal to %numberOfRows.")
ENTRY (U"Behaviour")
NORMAL (U"Before the normalization will be applied, however, we first translate the centre of the "
	"configuration to the origin by subtracting the mean for each dimension. "
	"The sum of squares than equals variance.")
MAN_END

MAN_BEGIN (U"Configuration: Randomize", U"djmw", 19971201)
INTRO (U"Changes all coordinates of the points #%x__%i_ in the @Configuration according to:")
LIST_ITEM (U"%x__%ij_ = randomUniform (-1, 1)")
MAN_END

MAN_BEGIN (U"Configuration: Rotate...", U"djmw", 20100303)
INTRO (U"Rotates the @Configuration in a plane around the origin.")
NORMAL (U"Settings")
TAG (U"##Dimension 1#, ##Dimension 2#")
DEFINITION (U"the dimensions that span the plane. The order of dimension 1 and dimension 2 is not important: "
	"the lowest number always determines the first dimension.")
TAG (U"##Angle")
DEFINITION (U"the counter-clockwise rotation angle in degrees.")
MAN_END

MAN_BEGIN (U"Configuration: Rotate (pc)", U"djmw", 19971201)
INTRO (U"Rotates the @Configuration to principal directions. The principal directions "
	"correspond to the principal components.")
MAN_END

MAN_BEGIN (U"Configuration: To Configuration (procrustes)", U"djmw", 19971219)
INTRO (U"A command that transforms the second selected @Configuration object "
	"to match the first selected Configuration object as closely as possible."
	"This problem of fitting one configuration (testee) to another (target) "
	"as closely as possible is called the Procrustes problem. We use a "
	"special @@Procrustes transform@ algorithm that does not "
	"mutilate or distort the testee configuration.")
NORMAL (U"Both Configuration objects must have the same dimensions.")
MAN_END

MAN_BEGIN (U"Configuration: To Configuration (varimax)...", U"djmw", 20040407)
INTRO (U"A command that rotates the selected @Configuration object to a new "
	"Configuration object whose coordinates have maximum %squared variance. ")
ENTRY (U"Settings")
TAG (U"##Normalize rows")
DEFINITION (U"when selected, the distances of all points to the origin will "
	"be made equal before iteration starts. We remember these scale factors "
	"and restore the original distances after the iteration process has "
	"stopped.")
TAG (U"##Quartimax")
DEFINITION (U"when selected, the sum of fourth powers, normalized or raw, "
	"will be maximized. ")
TAG (U"##Maximum number of iterations")
DEFINITION (U"sets a limit to the number of iterations. One iteration consists "
	"of %numberOfDimensions\\.c (%numberOfDimensions\\--1)/2 planar rotations "
	"of all pairs of dimensions.")
TAG (U"##Tolerance")
DEFINITION (U"also determines when the iteration stops. This happens if "
	"|%v__%i_\\--%v__%i+1_| < %tolerance \\.c %v__%i_, where %v__%i_ is the "
	"squared variance for the %i^^th^ iteration.")
NORMAL (U"The iteration process stops when either the %%maximum number of "
	"iterations% is reached or the %tolerance criterion is met, which ever "
	"one is first.")
ENTRY (U"Algorithm")
NORMAL (U"The Varimax rotation procedure was first proposed by @@Kaiser "
	"(1958)@. Given a %numberOfPoints \\xx %numberOfDimensions configuration "
	"#A, the procedure tries to find an orthonormal rotation matrix #T such "
	"that the sum of variances of the columns of #B*#B is a maximum, where #B "
	"= #A#T and * is the element wise (Hadamard) product of matrices. A direct "
	"solution for the optimal #T is not available, except for the case when "
	"%numberOfDimensions equals two. Kaiser suggested an iterative "
	"algorithm based on planar rotations, i.e., alternate rotations of all "
	"pairs of columns of #A.")
NORMAL (U"However, this procedure is not without problems: the varimax function "
	"may have stationary points that are not even local maxima. We have "
	"incorporated an algorithm of @@Ten Berge (1995)@ "
	"that prevents this unpleasant situation from happening.")
MAN_END

MAN_BEGIN (U"Configuration: To Distance", U"djmw", 19971207)
INTRO (U"A command that computes a @Distance object for each selected "
	"@Configuration.")
ENTRY (U"Algorithm")
NORMAL (U"The distance %d__%ij_ between objects %i and %j is calculated as:")
FORMULA (U"%d__%ij_ = %d__%ji_ = (\\su__%k=1..%numberOfDimensions_ |%x__%ik_ "
	"\\-- %x__%jk_|^2)^^1/2^")
MAN_END

MAN_BEGIN (U"Configuration: To Similarity (cc)", U"djmw", 19980130)
INTRO (U"A command that create one @Similarity object from the selected "
	"@Configuration objects.")
NORMAL (U"In the Similarity object entry %s__%ij_ equals the @@congruence "
	"coefficient@ for the %i-th and %j-th selected Configuration object.")
NORMAL (U"All Configuration objects must have the same number of points and "
	"the same dimensions.")
MAN_END

MAN_BEGIN (U"Configuration & AffineTransform: To Configuration", U"djmw", 20011008)
INTRO (U"A command that transforms the selected @Configuration to a new "
	"Configuration object according to the specifications in the selected "
	"@AffineTransform object.")
MAN_END

MAN_BEGIN (U"Configuration & Procrustes: To Configuration", U"djmw", 20011008)
INTRO (U"A command that transforms the selected @Configuration to a new "
	"Configuration object according to the specifications in the selected "
	"@Procrustes object.")
MAN_END

MAN_BEGIN (U"Configurations: To AffineTransform (congruence)...", U"djmw", 20040407)
INTRO (U"A command that creates an @AffineTransform object from two selected "
	"@Configuration objects.")
NORMAL (U"We calculate the affine transform that transforms the second "
	"selected Configuration object to match the first selected Configuration "
	"object as closely as possible. "
	"The degree of proportionality is the congruence between corresponding "
	"dimensions.")
ENTRY (U"Settings")
TAG (U"##Maximum number of iterations")
DEFINITION (U"sets a limit to the number of iterations.")
TAG (U"##Tolerance")
DEFINITION (U"also determines when the iteration stops. This happens if "
	"|%f(#T__%i_)\\--%f(#T__%i+1_)| < %tolerance \\.c %f(#T__%i_), where "
	"%f(#T__%i_) is the sum of the congruences for the %i^^th^ "
	"iteration (see below).")
NORMAL (U"The iteration process stops when either the %%maximum number of "
	"iterations% is reached or the %tolerance criterion is met, which ever "
	"one is first.")
ENTRY (U"Algorithm")
NORMAL (U"Sometimes the criterion used in a @@Procrustes transform@ is too "
	"restrictive for comparing two configurations. This criterion is only "
	"zero when the positions in the rotated configuration (#A#T) equal the "
	"positions in the other configuration (#B). @@Brokken (1983)@ proposed an "
	"algorithm to maximize instead the sum of congruences between "
	"corresponding dimensions of #AT and #B. "
	"Specifically he proposed to maximize")
FORMULA (U"%f(#T) = \\su__%i=1..%numberOfDimensions_ #t\\'p__%i_#A\\'p#b__%i_ /"
	" ((#t\\'p__%i_#A\\'p#A#t__%i_)^^1/2^(#b\\'p__%i_#b__%i_)^^1/2^),")
NORMAL (U"where #t\\'p__%i_ and #b\\'p__%i_ are the %i^^th^ column of #T and "
	"#B, respectively. A direct solution for #T is not available, it can only "
	"be obtained by an iterative procedure. The implemented algorithm is from "
	"@@Kiers & Groenen (1996)@ and shows excellent convergence properties.")
MAN_END

MAN_BEGIN (U"Configuration & Configuration: To Procrustes...", U"djmw", 20011008)
INTRO (U"A command that creates a @Procrustes object from two selected "
	"@Configuration objects.")
ENTRY (U"Setting")
TAG (U"##Orthogonal transform")
DEFINITION (U"determines whether or not a translation and a scaling are allowed in the transform.")
NORMAL (U"We calculate the @@Procrustes transform@ that transforms the second "
	"selected Configuration object to match the first selected Configuration "
	"object as closely as possible.")
MAN_END

MAN_BEGIN (U"Confusion: To Dissimilarity...", U"djmw", 20040407)
INTRO (U"A command that creates a @Dissimilarity from every selected "
	"@Confusion.")
ENTRY (U"Settings")
TAG (U"##Normalize")
DEFINITION (U"when on, normalize rows by dividing each row element by the row "
	"sum. In this way you correct for unequal stimulus numbers.")
TAG (U"##No symmetrization#, #Average, #Houtgast")
DEFINITION (U"determine the symmetrization procedure. See "
	"@@Confusion: To Similarity...")
TAG (U"##Maximum dissimilarity")
DEFINITION (U"determines the maximum dissimilarity possible. When the default "
	"value, 0.0, is chosen, %maximumDissimilarity "
	"is calculated as the maximum element in the Similarity object.")
ENTRY (U"Algorithm")
NORMAL (U"We first transform the Confusion to a Similarity. See "
	"@@Confusion: To Similarity...")
NORMAL (U"To obtain dissimilarities from similarities we \"reverse\" the "
	"latter:")
FORMULA (U"%%dissimilarity__%ij_ = %maximumDissimilarity \\-- %similarity__%ij_")
MAN_END

MAN_BEGIN (U"Confusion: To Dissimilarity (pdf)...", U"djmw", 20040407)
INTRO (U"A command that creates a @Dissimilarity from every selected "
	"@Confusion.")
ENTRY (U"Settings")
TAG (U"##Symmetrize first")
DEFINITION (U"when on, the confusion matrix is symmetrized before we calculate "
	"dissimilarities.")
TAG (U"##Maximum dissimilarity (units of sigma)")
DEFINITION (U"specifies the dissimilarity from confusion matrix elements that "
	"are zero.")
ENTRY (U"Algorithm")
TAG (U"1. Normalize rows by dividing each row element by the row sum (optional).")
TAG (U"2. Symmetrize the matrix by averaging %f__%ij_ and %f__%ji_.")
TAG (U"3. Transformation of the confusion measure which is a sort of "
	"%similarity measure to the %dissimilarity measure.")
NORMAL (U"Similarity and dissimilarity have an inverse relationship: the "
	"greater the similarity, the smaller the dissimilarity and vice versa. "
	"Both have a monotonic relationship with distance. "
	"The most simple way to transform the similarities %f__%ij_ into "
	"dissimilarities is:")
FORMULA (U"%dissimilarity__%ij_ = %maximumSimilarity \\-- %similarity__%ij_")
NORMAL (U"For ordinal analyses like Kruskal this transformation is fine because "
	"only order relations are important in this analysis. However, for "
	"metrical analyses like INDSCAL this is not optimal. "
	"In INDSCAL, distance is a linear function of dissimilarity. This means "
	"that, with the transformation "
	"above, you ultimately fit an INDSCAL model in which the distance "
	"between object %i and %j will be linearly related to the confusion "
	"between %i and %j.")
NORMAL (U"For the relation between confusion and dissimilarity, the model "
	"implemented here, makes the assumption that the amount of confusion "
	"between objects %i and %j is related to the amount that their "
	"probability density functions, pdf's, overlap. Because we do not know "
	"these pdf's we make the assumption that both are normal, have equal "
	"%sigma and are one-dimensional. The parameter to be determined is the "
	"distance between the centres of both pdf's. "
	"According to formula 26.2.23 in @@Abramowitz & Stegun (1970)@, for each "
	"fraction %f__%ij_, we have to find an %x that solves:")
FORMULA (U"%f__%ij_ = 1 / \\Vr(2%\\pi) \\in__%x_^^\\oo^ e^^-%t\\.c%t/2^ %dt")
NORMAL (U"This %x will be used as the dissimilarity between %i and %j. The "
	"relation between %x and %f__%ij_ is monotonic. This means that the "
	"results for a Kruskal analysis will not change much. For INDSCAL, in "
	"general, you will note a significantly better fit.")
MAN_END

MAN_BEGIN (U"Confusion: To Similarity...", U"djmw", 20040407)
INTRO (U"A command that creates a @Similarity from every selected @Confusion.")
ENTRY (U"Settings")
TAG (U"##Normalize")
DEFINITION (U"when on, normalize rows by dividing each row element by the row "
	"sum. In this way you correct for unequal stimulus numbers.")
TAG (U"##No symmetrization#, #Average, #Houtgast")
DEFINITION (U"determine the symmetrization procedure.")
ENTRY (U"Algorithm")
NORMAL (U"The %Average procedure averages:")
FORMULA (U"%similarity__%ij_= %similarity__%ji_ = (%confusion__%ij_ + "
	"%confusion__%ji_) / 2")
NORMAL (U"The %Houtgast procedure as described in the paper by @@Klein, Plomp "
	"& Pols (1970)@, expresses similarity between stimuli %i and %j by the "
	"number of times that stimulus %i and %j have "
	"resulted in the same response, summated over all response categories.")
NORMAL (U"We use the following formula to calculate the %Houtgast "
	"dissimilarities:")
FORMULA (U"%similarity__%ij_ = \\su__%k=1..%numberOfColumns_ min "
	"(%confusion__%ik_, %confusion__%jk_)")
NORMAL (U"which is equivalent to the formula in the Klein et al. paper:")
FORMULA (U"%similarity__%ij_ = \\su__%k=1..%numberOfColumns_ (%confusion__%ik_ "
	"+ %confusion__%jk_ \\-- |%confusion__%ik_ \\-- %confusion__%jk_|)")
MAN_END

MAN_BEGIN (U"congruence coefficient", U"djmw", 20040407)
INTRO (U"The %%congruence coefficient% is a measure of similarity between two "
	"@@Configuration@s.")
NORMAL (U"The congruence coefficient %c(#X, #Y) for the configurations #X and "
	"#Y is defined as:")
FORMULA (U"%c(%X, %Y) = \\su__%i<%j_ %w__%ij_ %d__%ij_(#X) %d__%ij_(#Y) / "
	"([\\su__%i<%j_ %w__%ij_ %d^2__%ij_(#X)]^^1/2^ [\\su__%i<%j_ %w__%ij_ "
	"%d^2__%ij_(#Y)]^^1/2^),")
NORMAL (U"where %d__%ij_(#X) is the distance between the points %i and %j in "
	"configuration #X and %w__%ij_ are nonnegative weights (default: %w__%ij_"
	" = 1).")
NORMAL (U"Since distances are nonnegative, the congruence coefficient has a "
	"value between 0 and 1.")
NORMAL (U"The %%congruence coefficient% is a better measure of the similarity "
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
NORMAL (U"For further information on how well one number can assess the "
	"similarity between two configurations see @@Borg & Groenen (1997)@ "
	"section 19.7.")
MAN_END

MAN_BEGIN (U"ContingencyTable", U"djmw", 19971216)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"In a two-way contingency table, cell %f__%ij_ contains the frequency "
	"with which row category %i co-occurs with column category %j. "
	"Necessarily, all %f__%ij_ \\>_ 0.")
ENTRY (U"Commands")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu ##TableOfReal: To ##ContingencyTable")
NORMAL (U"Query")
LIST_ITEM (U"\\bu ##ContingencyTable: Get chi squared probability")
LIST_ITEM (U"\\bu ##ContingencyTable: Get Cramer's statistic")
LIST_ITEM (U"\\bu ##ContingencyTable: Get contingency coefficient")
NORMAL (U"Analysis")
LIST_ITEM (U"\\bu @@ContingencyTable: To Configuration (ca)...")
MAN_END

MAN_BEGIN (U"ContingencyTable: To Configuration (ca)...", U"djmw", 20040407)
INTRO (U"A command that creates a @Configuration object from the selected "
	"@ContingencyTable object by means of @@Correspondence analysis@.")
ENTRY (U"Settings")
TAG (U"##Number of dimensions")
DEFINITION (U"The dimensionality of the Configuration.")
TAG (U"##Scaling of the final configuration")
DEFINITION (U"determines whether row points are in the centre of gravity of "
	"column points, or, column points are in the centre of gravity of row "
	"points, or, whether roes and columns are treated symmetrically.")
ENTRY (U"Algorithm")
NORMAL (U"1. We start with the following transformation of the entries "
	"%f__%ij_:")
FORMULA (U"%h__%ij_ = %f__%ij_ / \\Vr (%f__%i+_%f__+%j_) - \\Vr "
	"(%f__%i+_%f__+%j_) / %N,")
NORMAL (U"where %h__%ij_ is the entry for a cell in the matrix #H with "
	"transformed data, %f__%i+_ "
	"is the total count for row %i, %f__+%j_ is the total count for column %j "
	"and %N is the grand total. "
	"This can be written in matrix form as:")
FORMULA (U"#H = #R^^\\--1/2^#F#C^^\\--1/2^ \\-- #R^^1/2^#uu\\'p#C^^1/2^ / %N,")
NORMAL (U"where #R and #C are diagonal matrices with the row and column totals, "
	"respectively and #u a column vector with all elements equal to 1. ")
NORMAL (U"2. Next the singular value decomposition of matrix #H is performed:")
FORMULA (U"#H = #K #\\La #L\\'p,")
NORMAL (U"where #K\\'p#K = #I, #L\\'p#L = #I, and #\\La is a diagonal matrix "
	"with singular values.")
NORMAL (U"3. Now the row (#X) and column points (#Y) can be determined. "
	"Three normalizations are possible:")
TAG (U"\\bu Scale row points in the centre of gravity of column points")
DEFINITION (U"#X = \\Vr%N #R^^\\--1/2^#K#\\La")
DEFINITION (U"#Y = \\Vr%N #C^^\\--1/2^#U")
TAG (U"\\bu Scale column points in the centre of gravity of row points")
DEFINITION (U"#X = \\Vr%N #R^^\\--1/2^#K")
DEFINITION (U"#Y = \\Vr%N #C^^\\--1/2^#L#\\La")
TAG (U"\\bu Treat row points and column points symmetrically")
DEFINITION (U"#X = \\Vr%N #R^^\\--1/2^#K#\\La^^\\--1/2^")
DEFINITION (U"#Y = \\Vr%N #C^^\\--1/2^#L\\La^^\\--1/2^")
NORMAL (U"For more details see @@Gifi (1990)@, chapter 8.")
MAN_END

MAN_BEGIN (U"Correspondence analysis", U"djmw", 19971216)
INTRO (U"Correspondence analysis provides a method for representing data in "
	"an Euclidean space so that the results can be visually examined for "
	"structure. For data in a typical two-way @ContingencyTable both the row "
	"variables and the column variables are represented in the same space. "
	"This means that one can examine relations not only among row "
	"or column variables but also between row and column variables.")
NORMAL (U"In correspondence analysis the data matrix is first transformed by "
	"dividing each cell by the square root of the corresponding row and column "
	"totals. "
	"The transformed matrix is then decomposed with singular value "
	"decomposition resulting in the singular values (which in this case are "
	"canonical correlations) and a set of row vectors and column vectors. "
	"Next the row and column vectors are rescaled with the original total "
	"frequencies to obtain optimal scores. "
	"These optimal scores are weighted by the square root of the singular "
	"values and become the coordinates of the points in the @Configuration.")
NORMAL (U"Examples can be found in the books by @@Weller & Romney (1990)@ and "
	"@@Gifi (1990)@.")
MAN_END

MAN_BEGIN (U"Create Configuration...", U"djmw", 19980413)
INTRO (U"A command to create a @Configuration with the specified number of "
	"points and number of dimensions. The location of the points will be "
	"determined by the formula (see @@Formulas@ for more "
	"information about possible formulas).")
MAN_END

MAN_BEGIN (U"Create INDSCAL Carroll & Wish example...", U"djmw", 19971201)
INTRO (U"Creates eight @Dissimilarity objects that bear names \"1\" ... \"8\".")
NORMAL (U"These objects contain the interpoint distances for a twodimensional "
	"3\\xx3 @Configuration of points, labelled A, B, C, ... I. "
	"All Dissimilarity objects are based on the following underlying configuration.")
PICTURE (4.0, 4.0, drawCarrollWishConfigurationExample)
NORMAL (U"The eight sources weigh this configuration in the following manner:")
PICTURE (4.0, 4.0, drawCarrollWishSalienceExample)
NORMAL (U"For each source, the distances were subjected to the transformation: ")
FORMULA (U"%dissimilarity__%ij_ = %distance__%ij_ + %noiseRange \\.c #u, ")
NORMAL (U"where #u is a uniform random variable between 0 and 1.")
NORMAL (U"Now you can do the following for example:")
TAG (U"Select all the Dissimilarity objects and choose @@Dissimilarity: To Distance...|"
	"To Distance...@.")
DEFINITION (U"Uncheck scale (add \"additive constant\").")
TAG (U"Select all the Distance objects and choose @@Distance: To Configuration (indscal)...|"
	"To Configuration (indscal)...@.")
DEFINITION (U"and an @@INDSCAL analysis@ will be performed. In order to reproduce the saliences, "
	"you have to uncheck the \"Normalize scalar products\" option.")
NORMAL (U"This example was adapted from @@Carroll & Wish (1974)@.")
MAN_END

MAN_BEGIN (U"Create letter R example...", U"djmw", 19971201)
INTRO (U"Creates a @Dissimilarity object that bears the name %R. The "
	"dissimilarities in this object were chosen to be a monotone "
	"transformation of the distances between the 32 two-dimensional points "
	"that make up the capital letter #R.")
PICTURE (4.0, 4.0, drawLetterRConfigurationExample)
NORMAL (U"All 32 \\.c (32-1)/2 interpoint distances were subjected to the "
	"transformation: ")
FORMULA (U"%dissimilarity__%ij_^ = %distance__%ij_^2 + 5 + %noiseRange \\.c #u, ")
NORMAL (U"where #u is a uniform random variable between 0 and 1.")
NORMAL (U"This example was chosen from @@Green, Carmone & Smith (1989)@.")
MAN_END

MAN_BEGIN (U"disparities", U"djmw", 19980111)
INTRO (U"The numbers %d\\'p__%ij_ that result from applying an admissible "
	"transformation %f on the dissimilarities %\\de__%ij_, i.e., %d\\'p__%ij_ "
	"= %f (%\\de__%ij_). Disparities have the same dimension as distances. "
	"Other names for disparities are %%pseudo distances% and %%target "
	"distances%.")
MAN_END

MAN_BEGIN (U"Dissimilarity", U"djmw", 20010327)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"It represents a one-way table with "
	"dissimilarities between \"objects\".")
ENTRY (U"Creating a Dissimilarity from data in a text file")
NORMAL (U"Suppose you have three objects A, B and C. "
	"In one way or another, you have acquired the following (symmetric) "
	"dissimilarities: %\\de__%AB_ = 2 (= %\\de__%BA_) , %\\de__%AC_ = 1 "
	"(= %\\de__%CA_), and %\\de__%BC_ = 1.4 (= %\\de__CB_), where %\\de__%AB_"
	" represents the dissimilarity between object A and object B.")
NORMAL (U"You can create a simple text file like the following:")
CODE (U"\"ooTextFile\"  ! The line by which Praat can recognize your file")
CODE (U"\"Dissimilarity\" ! The line that tells Praat about the contents")
CODE (U"3     \"A\"  \"B\"  \"C\"   ! Number of columns, and column labels")
CODE (U"3                     ! Number of rows")
CODE (U"\"A\"    0    2    1    ! Row label (A), A-B value, A-C value")
CODE (U"\"B\"    2    0  1.4    ! Row label (B), B-A value, B-C value")
CODE (U"\"C\"    1    1.4  0    ! Row label (C), C-A value, C-B value")
NORMAL (U"Notice that:")
LIST_ITEM (U"\\bu the row and column labels are identical.")
LIST_ITEM (U"\\bu the matrix elements on the diagonal are zero.")
LIST_ITEM (U"\\bu the matrix is symmetrical.")
NORMAL (U"This text file can be read with the @@Read from file...@ command. "
	"Since a Dissimilarity object has the data structure of a square "
	"symmetrical TableOfReal, you could also start from an appropriate "
	"@TableOfReal object and cast it to a Dissimilarity object.")
ENTRY (U"Commands")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu @@Confusion: To Dissimilarity...")
NORMAL (U"Drawing")
LIST_ITEM (U"\\bu ##Draw as numbers...")
LIST_ITEM (U"\\bu ##Draw as squares...")
NORMAL (U"Query")
LIST_ITEM (U"\\bu ##Get column mean (index)...")
LIST_ITEM (U"\\bu ##Get column mean (label)...")
LIST_ITEM (U"\\bu ##Get column stdev (index)...")
LIST_ITEM (U"\\bu ##Get column stdev (label)...")
LIST_ITEM (U"\\bu @@Dissimilarity: Get additive constant")
NORMAL (U"Modification")
LIST_ITEM (U"\\bu @@Formula...")
LIST_ITEM (U"\\bu ##Set value...")
LIST_ITEM (U"\\bu ##Remove column (index)...")
LIST_ITEM (U"\\bu ##Insert column (index)...")
LIST_ITEM (U"\\bu ##Set row label (index)...")
LIST_ITEM (U"\\bu ##Set row label (label)...")
LIST_ITEM (U"\\bu ##Set column label (index)...")
LIST_ITEM (U"\\bu ##Set column label (label)...")
NORMAL (U"Multidimensional scaling analysis")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (kruskal)...")
LIST_ITEM (U"Transformations")
LIST_ITEM (U"\\bu @@Dissimilarity: To Distance...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Weight")
MAN_END

MAN_BEGIN (U"Dissimilarity: Get additive constant", U"djmw", 19971201)
INTRO (U"A command that calculates the \"additive constant\" from the selected @Dissimilarity.")
NORMAL (U"Distances %d__%ij_ will be obtained from dissimilarities %\\de__%ij_ according to:")
FORMULA (U" %distance__%ij_ = %dissimilarity__%ij_ + %additiveConstant")
NORMAL (U"We use a procedure by @@Cailliez (1983)@ to solve the \"additive constant problem\", i.e. "
	"find the smallest %additiveConstant such that all %distance__%ij_ in the above equation "
	" have a Euclidean representation.")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Configuration (absolute mds)...", U"djmw", 19980105)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity object.")
NORMAL (U"The @disparities %d\\'p__%ij_ will be obtained from dissimilarities %\\de__%ij_ according to:")
FORMULA (U"%d\\'p__%ij_ = %\\de__%ij_")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Configuration (interval mds)...", U"djmw", 19980105)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity object.")
NORMAL (U"The @disparities %d\\'p__%ij_ will be obtained from dissimilarities %\\de__%ij_ according to:")
FORMULA (U"%d\\'p__%ij_ = %a + %b \\.c %\\de__%ij_")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Configuration (i-spline mds)...", U"djmw", 20040407)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity object.")
NORMAL (U"Dissimilarities %\\de__%ij_ and @disparities %d\\'p__%ij_ will be related by a @spline function:")
FORMULA (U"%d\\'p__%ij_ = \\su__%k=1..(%%numberOfInteriorKnots%+%order)_ spline__%k_ (%knots, %order, %\\de__%ij_),")
NORMAL (U"where spline__%k_ (\\.c) is the value of the %k^^th^ I-spline of order %order and knot sequence "
	"%knot evaluated at %\\de__%ij_.")
ENTRY (U"Settings")
TAG (U"##Number of dimensions")
DEFINITION (U"determines the dimensionality of the configuration.")
TAG (U"##Number of interior knots")
DEFINITION (U"determines the number of segment boundaries. Each interior knot "
	"is the boundary between two segments. The splines in each segment will "
	"be joined as continuously as possible.")
TAG (U"##Order of I-spline")
DEFINITION (U"The order of the polynomial basis of the I-spline.")
NORMAL (U"Finding the optimal Configuration involves a minimization process:")
TAG (U"##Tolerance")
DEFINITION (U"When successive values for the stress differ by less than "
	"#Tolerance, the minimization process stops.")
TAG (U"##Maximum number of iterations")
DEFINITION (U"Minimization stops after this number of iterations has been "
	"reached.")
TAG (U"##Number of repetitions")
DEFINITION (U"If chosen larger than 1, the minimization process will be "
	"repeated, each time with another random start configuration. "
	"The configuration that results in minimum stress, will be saved.")
ENTRY (U"Hints")
NORMAL (U"If %numberOfInteriorKnots is zero, polynomial regression will be "
	"performed. Therefore , the combination %numberOfInteriorKnots = 0 and "
	"%order = 1 also gives interval "
	"scaling (in fact, it is the implementation in this program).")
NORMAL (U"In the limit when %order = 0 and %numberOfInteriorKnots = "
	"%numberOfDissimilarities, monotone regression is performed.")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Configuration (kruskal)...", U"djmw", 20190510)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object.")
ENTRY (U"Settings")
TAG (U"##Number of dimensions# (standard value: 2)")
DEFINITION (U"The dimensionality of the Configuration.")
TAG (U"##Distance metric% (standard value: 2, i.e. Euclidean)")
DEFINITION (U"the general distance between points #x__%i_ and #x__%j_ (%i,%j "
	"= 1..%numberOfPoints) is:")
DEFINITION (U"(\\su__%k=1..%numberOfDimensions_ |%x__%ik_ \\--%x__%jk_|"
	"^^%metric^)^^1/%metric^")
TAG (U"##Handling of ties")
DEFINITION (U"determines the handling of ties in the data. In the %%primary approach%, whenever "
	"two or more dissimilarities are equal we do not care whether the fitted "
	"distances are equal or not. "
	"Consequently, no constraints are imposed on the fitted distances. "
	"For the %%secondary approach%, however, we impose the constaint that the fitted distances be "
	"equal whenever the dissimilarities are equal.")
NORMAL (U"For the calculation of stress:")
TAG (U"##Kruskal's stress-1 (Formula1, the default)")   // ??
FORMULA (U"%stress = \\Vr(\\su(%distance__%k_ \\-- %fittedDistance__%k_)^2 / "
	"\\su %distance__%k_^2)")
TAG (U"##Kruskal's stress-2 (Formula2)")
FORMULA (U"%stress = \\Vr(\\su(%distance__%k_ \\-- %fittedDistance__%k_)^2 / "
	"\\su (%distance__%k_ \\-- %averageDistance)^2)")
DEFINITION (U"Note that values of stress-2 are generally more than double those "
	"of stress-1 for the same degree of fit.")
NORMAL (U"Finding the optimal Configuration involves a minimization process:")
TAG (U"##Tolerance")
DEFINITION (U"When successive values for the stress differ less than %Tolerance "
	"the minimization process stops.")
TAG (U"##Maximum number of iterations")
DEFINITION (U"Minimization stops after this number of iterations has been "
	"reached.")
TAG (U"##Number of repetitions")
DEFINITION (U"When chosen larger than 1, the minimalization process will be "
	"repeated, each time with another random start configuration. "
	"The configuration that results in minimum stress will be saved.")
ENTRY (U"Precautions")
NORMAL (U"When there are few objects it is impossible to recover many "
	"dimensions. A rough rule of thumb is that there should be at least twice "
	"as many number of observations, i.e. the %numberOfPoints \\.c "
	"(%numberOfPoints - 1) / 2 (dis)similarities, than parameters "
	"to be estimated, i.e. the %numberOfPoints \\.c %numberOfDimensions "
	"position coordinates. A practical guide is:")
	LIST_ITEM (U"for %numberOfDimensions = 1 you need \\>_ 5 objects")
	LIST_ITEM (U"for %numberOfDimensions = 2 you need \\>_ 9 objects")
	LIST_ITEM (U"for %numberOfDimensions = 3 you need \\>_ 13 objects")
NORMAL (U"There is no feasible way to be certain that you have found the "
	"true global minimum. However, by using a great number of different "
	"random starting configurations to scale the same data it is often "
	"possible to obtain practical certainty. "
	"Although the procedure for obtaining an initial configuration is based "
	"on a %linear relation between distance and (dis)similarity, it gives a "
	"very good approximation of the optimal #Configuration and "
	"the #Minimizer practically always finds the global minimum from it "
	"(I guess...). A way to find out is to try the %numberOfRepetitions "
	"parameter which gives you the possibility to fit many times and each "
	"time start with another random initial configuration.")
ENTRY (U"Algorithm")
LIST_ITEM (U"1. The Dissimilarity object is converted to a Distance object in "
	"the same way as in @@Dissimilarity: To Distance...@.)")
LIST_ITEM (U"2. From the Distance object an initial Configuration is found by "
	"first transforming the Distance object to a matrix with scalar products "
	"of distances and subsequently solving for the first %numberOfDimensions "
	"eigenvectors of this matrix.")
LIST_ITEM (U"3. A minimalization algorithm is started that tries to minimize a "
	"function. In this function:")
LIST_ITEM (U"\\bu 3.1 We normalize the current Configuration from the minimizer")
LIST_ITEM (U"\\bu 3.2 Calculate a new Distance object from the configuration")
LIST_ITEM (U"\\bu 3.3 Do a monotone regression of this Distance on the "
	"Dissimilarity. This results in a new Distance object.")
LIST_ITEM (U"\\bu 3.4 Calculate stress from this Distance and the Distance "
	"obtained from Dissimilarity.")
NORMAL (U"The optimization process is ccontrolledby a conjugate gradient "
	"minimization algorithm that tries to minimize the %stress function. "
	"In @@Kruskal (1964)@, a steepest descent "
	"algorithm is used wwhichis less efficient. ")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Configuration (monotone mds)...", U"djmw", 20190510)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object.")
NORMAL (U"Dissimilarities %\\de__%ij_ and @disparities %d\\'p__%ij_ are "
	"related by:")
FORMULA (U"%d\\'p__%ij_ \\<_ %d\\'p__%kl_ if %\\de__%ij_ \\<_ %\\de__%kl_")
ENTRY (U"Settings")
TAG (U"##Number of dimensions")
DEFINITION (U"determines the number of dimensions of the configuration.")
TAG (U"##Handling of ties")
DEFINITION (U"When dissimilarities are equal, i.e., %\\de__%ij_ = %\\de__%kl_, "
	"the primary approach imposes no conditions on the corresponding "
	"@disparities %d\\'p__%ij_ and %d\\'p__%kl_, while the %secondary "
	"approach demands that also %d\\'p__%ij_ = %d\\'p__%kl_.")
NORMAL (U"Finding the optimal Configuration involves a minimization process:")
TAG (U"##Tolerance")
DEFINITION (U"When successive values for the stress differ less than %Tolerance "
	"the minimization process stops.")
TAG (U"##Maximum number of iterations")
DEFINITION (U"Minimization stops after this number of iterations has been reached.")
TAG (U"##Number of repetitions")
DEFINITION (U"When chosen larger than 1, the minimalization process will be "
	"repeated, each time with another random start configuration. "
	"The configuration that results in minimum stress will be saved.")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Configuration (ratio mds)...", U"djmw", 19980105)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object.")
NORMAL (U"The @disparities %d\\'p__%ij_ will be obtained from dissimilarities "
	"%\\de__%ij_ according to:")
FORMULA (U"%d\\'p__%ij_ = %b \\.c %\\de__%ij_")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Distance...", U"djmw", 20040407)
INTRO (U"A command that creates a @Distance object from a selected "
	"@Dissimilarity object.")
ENTRY (U"Settings")
TAG (U"##Scale")
DEFINITION (U"when on, the @@Dissimilarity: Get additive constant|"
	"additiveConstant@ is determined, when off the %additiveConstant = 0.")
NORMAL (U"dissimilarities are transformed to distances according to:")
FORMULA (U" %distance__%ij_ = %dissimilarity__%ij_ + %additiveConstant.")
MAN_END

MAN_BEGIN (U"Dissimilarity: To Weight", U"djmw", 19980108)
INTRO (U"Creates an object of type @Weight for each selected @Dissimilarity "
	"object.")
NORMAL (U"The values in the weight matrix will be:")
LIST_ITEM (U"%w__%ii_ = 0")
LIST_ITEM (U"%w__%ij_ = 1 if %\\de__%ij_ > 0")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Draw regression (absolute mds)...",
	U"djmw", 20040407)
INTRO (U"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity object versus @disparities %d\\'p__%ij_ obtained "
	"from the \"regression\" of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
FORMULA (U"%d\\'p__%ij_ = %\\de__%ij_")
ENTRY (U"Settings")
TAG (U"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (U"minimum and maximum values for the proximities (horizontal axis).")
TAG (U"##Minimum distance#, ##Maximum distance#")
DEFINITION (U"minimum and maximum values for the distances (vertical axis).")
TAG (U"##Mark size (mm)#, ##Mark string#")
DEFINITION (U"size and kind of the marks in the plot.")
TAG (U"##Garnish")
DEFINITION (U"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Draw regression (interval mds)...",
	U"djmw", 20040407)
INTRO (U"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the regression of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
FORMULA (U"%d\\'p__%ij_ = %a + %b \\.c %\\de__%ij_,")
NORMAL (U"where the values of %a and %b are determined by regression.")
ENTRY (U"Settings")
TAG (U"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (U"minimum and maximum values for the proximities (horizontal axis).")
TAG (U"##Minimum distance#, ##Maximum distance#")
DEFINITION (U"minimum and maximum values for the distances (vertical axis).")
TAG (U"##Mark size (mm)#, ##Mark string#")
DEFINITION (U"size and kind of the marks in the plot.")
TAG (U"##Garnish")
DEFINITION (U"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Draw regression (i-spline mds)...", U"djmw", 20040407)
INTRO (U"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the regression of distances %d__%ij_ from @Configuration on the "
	"@spline transformed dissimilarities %\\de__%ij_.")
ENTRY (U"Settings")
TAG (U"##Number of interior knots")
DEFINITION (U"determines the number of segments.")
TAG (U"##Order of I-spline")
DEFINITION (U"The order of the polynomial basis of the I-spline.")
TAG (U"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (U"minimum and maximum values for the proximities (horizontal axis).")
TAG (U"##Minimum distance#, ##Maximum distance#")
DEFINITION (U"minimum and maximum values for the distances (vertical axis).")
TAG (U"##Mark size (mm)#, ##Mark string#")
DEFINITION (U"size and kind of the marks in the plot.")
TAG (U"##Garnish")
DEFINITION (U"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Draw regression (monotone mds)...", U"djmw", 20190510)
INTRO (U"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the monotone regression of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
ENTRY (U"Settings")
TAG (U"##Handling of ties")
DEFINITION (U"When dissimilarities are equal, i.e., %\\de__%ij_ = %\\de__%kl_ "
	"the primary approach imposes no conditions on the corresponding distances "
	"%d__%ij_ and %d__%kl_, while the %secondary approach demands that also "
	"%d__%ij_ = %d__%kl_.")
TAG (U"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (U"minimum and maximum values for the proximities (horizontal axis).")
TAG (U"##Minimum distance#, ##Maximum distance#")
DEFINITION (U"minimum and maximum values for the distances (vertical axis).")
TAG (U"##Mark size (mm)#, ##Mark string#")
DEFINITION (U"size and kind of the marks in the plot.")
TAG (U"##Garnish")
DEFINITION (U"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Draw regression (ratio mds)...", U"djmw", 20040407)
INTRO (U"Draws a scatterplot of the dissimilarities %\\de__%ij_ from the "
	"selected @Dissimilarity versus @disparities %d\\'p__%ij_ obtained "
	"from the \"regression\" of distances %d__%ij_ "
	"from @Configuration on the dissimilarities %\\de__%ij_.")
FORMULA (U"%d\\'p__%ij_ = %b \\.c %\\de__%ij_,")
NORMAL (U"where the value of %b is determined by regression.")
ENTRY (U"Settings")
TAG (U"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (U"minimum and maximum values for the proximities (horizontal axis).")
TAG (U"##Minimum distance#, ##Maximum distance#")
DEFINITION (U"minimum and maximum values for the distances (vertical axis).")
TAG (U"##Mark size (mm)#, ##Mark string#")
DEFINITION (U"size and kind of the marks in the plot.")
TAG (U"##Garnish")
DEFINITION (U"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Draw Shepard diagram...", U"djmw", 20040407)
INTRO (U"Draws the Shepard diagram. This is a scatterplot of the "
	"dissimilarities from the @Dissimilarity object versus distances (as "
	"calculated from the @Configuration).")
ENTRY (U"Settings")
TAG (U"##Minimum proximity#, ##Maximum proximity#")
DEFINITION (U"minimum and maximum values for the proximities (horizontal axis).")
TAG (U"##Minimum distance#, ##Maximum distance#")
DEFINITION (U"minimum and maximum values for the distances (vertical axis).")
TAG (U"##Mark size (mm)#, ##Mark string#")
DEFINITION (U"size and kind of the marks in the plot.")
TAG (U"##Garnish")
DEFINITION (U"when on, draws a bounding box with decoration.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Get stress (absolute mds)...",
	U"djmw", 19980119)
INTRO (U"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (U"Behaviour")
NORMAL (U"Stress formula's are #dependent of the scale of the Configuration: "
	"you will get #another stress value if you had pre-multiplied the "
	"selected Configuration with any number greater than zero.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Get stress (interval mds)...",
	U"djmw", 19980119)
INTRO (U"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (U"Behaviour")
NORMAL (U"We use stress formula's that are independent of the scale of the "
	"Configuration: you would have got the same stress value if you had "
	"pre-multiplied the selected Configuration with any number greater "
	"than zero.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Get stress (i-spline mds)...",
	U"djmw", 19980119)
INTRO (U"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (U"Behaviour")
NORMAL (U"We use stress formula's that are independent of the scale "
	"of the Configuration: you would have got the same stress value if "
	"you had pre-multiplied the selected Configuration with any number "
	"greater than zero.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Get stress (monotone mds)...",
	U"djmw", 19980119)
INTRO (U"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (U"Behaviour")
NORMAL (U"We use stress formula's that are independent of the scale "
	"of the Configuration: you would have got the same stress value if "
	"you had pre-multiplied the selected Configuration with any number "
	"greater than zero.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: Get stress (ratio mds)...",
	U"djmw", 19980119)
INTRO (U"A command to obtain the @stress value for the selected @Dissimilarity "
	"and @Configuration object.")
ENTRY (U"Behaviour")
NORMAL (U"We use stress formula's that are independent of the scale "
	"of the Configuration: you would have got the same stress value if "
	"you had pre-multiplied the selected Configuration with any number "
	"greater than zero.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: To Configuration (absolute mds)...",
	U"djmw", 19980119)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: To Configuration (interval mds)...",
	U"djmw", 19980119)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: To Configuration (i-spline mds)...",
	U"djmw", 19980119)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: To Configuration (kruskal)...",
	U"djmw", 19971201)
INTRO (U"A command to fit an optimal @Configuration for the selected "
	"@Dissimilarity object. The selected @Configuration will be used as the "
	"starting configuration in the kruskal analysis.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: To Configuration (monotone mds)...",
	U"djmw", 19980119)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration: To Configuration (ratio mds)...",
	U"djmw", 19980119)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. The selected Configuration object serves as a starting "
	"configuration for the minimization process.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration & Weight: Get stress...", U"djmw", 20190509)
INTRO (U"A command that calculates the @stress between distances %d__%ij_ "
	"derived from the selected @Configuration object and @disparities "
	"%d\\'p__%ij_ derived from the selected @Dissimilarity object. "
	"With the selected @Weight object the evaluation of the influence "
	"of each dissimilarity on stress can be influenced.")
ENTRY (U"Settings")
LIST_ITEM (U"%%Normalized stress%, %%Kruskal's stress-1%, %%Kruskal's "
	"stress-2% or %Raw stress%")
ENTRY (U"Behaviour")
NORMAL (U"Except for %absolute %mds, we use stress formula's that are "
	"independent of the scale of the Configuration (see @stress): you would "
	"have got the same stress value if you had pre-multiplied the selected "
	"Configuration with any number greater than zero.")
MAN_END

MAN_BEGIN (U"Dissimilarity & Configuration & Weight: To Configuration...", U"djmw", 20040407)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. With the selected @Weight object the influence of each "
	"dissimilarity on @stress can be influenced. The selected Configuration "
	"object serves as a starting configuration for the minimization process.")
ENTRY (U"Settings")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
MAN_END

MAN_BEGIN (U"Dissimilarity & Weight: To Configuration...", U"djmw", 20040407)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. With the selected @Weight object the influence of each "
	"dissimilarity on @stress can be influenced.")
ENTRY (U"Settings")
NORMAL (U"May be different and depend on the representation function, i.e. "
	"the scale of measurement.")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
MAN_END

MAN_BEGIN (U"Dissimilarity & Weight: To Configuration...", U"djmw", 20040407)
INTRO (U"A command that creates a @Configuration object from a @Dissimilarity "
	"object. With the selected @Weight object the influence of each "
	"dissimilarity on @stress can be influenced.")
ENTRY (U"Settings")
NORMAL (U"May be different and depend on the representation function, i.e. the scale of measurement.")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (monotone mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (i-spline mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (interval mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (ratio mds)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (absolute mds)...")
MAN_END

MAN_BEGIN (U"Distance", U"djmw", 19971124)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"An object of type Distance represents distances between objects in a metrical space.")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu @@Confusion: To Dissimilarity (pdf)...")
LIST_ITEM (U"\\bu @@Dissimilarity: To Distance...")
MAN_END

MAN_BEGIN (U"Distance: To Configuration (indscal)...", U"djmw", 19971124)
INTRO (U"Perform an @@INDSCAL analysis@ on the selected object(s) of type @Distance that "
	"results in a @Configuration and a @Salience object.")
MAN_END

MAN_BEGIN (U"Distance: To Configuration (ytl)...", U"djmw", 19971124)
INTRO (U"A command that creates one @Configuration and one @Salience object "
	"from a collection of one or more @Distance objects.")
NORMAL (U"This Configuration and Salience object normally serve as starting points "
	"for an @@individual difference scaling@ such as an @@INDSCAL analysis@.")
NORMAL (U"The algorithm is ddescribedin @@Young, Takane & Lewyckyj (1978)@.")
MAN_END

MAN_BEGIN (U"Distance: To ScalarProduct...", U"djmw", 20040407)
INTRO (U"A command that creates a @ScalarProduct for each selected @Distance.")
ENTRY (U"Setting")
TAG (U"##Make sum of squares equal 1.0")
DEFINITION (U"when selected, the elements in the resulting matrix part will be scaled such that "
	"the sum of all the squared elements in the matrix equals 1.0.")
ENTRY (U"Algorithm")
NORMAL (U"ScalarProduct entries %b__%ij_ are created from distances %d__%ij_ bij double centering "
	"the matrix with elements \\--1/2 %d__%ij_^2, i.e.,")
FORMULA (U"%b__%ij_= \\--1/2(%d__%ij_^2 \\-- %d__%\\.cj_^2 \\-- %d__%i\\.c_^2 + %d__%\\.c\\.c_^2),")
NORMAL (U"where the dot (\\.c) means averaging over that dimension.")
MAN_END

MAN_BEGIN (U"Distance & Configuration: Draw scatter diagram...", U"djmw", 19971201)
MAN_END

MAN_BEGIN (U"Distance & Configuration: Get VAF...", U"djmw", 19971201)
INTRO (U"Calculates the \"%%variance accounted for%\" from the selected collection of @Distance objects and the "
	"selected @Configuration. The optimal @Salience necessary for the calculation "
	"will be inferred from the selected Distance and Configuration objects.")
MAN_END

MAN_BEGIN (U"Distance & Configuration: To Configuration (indscal)...", U"djmw", 19971201)
INTRO (U"Performs an @@INDSCAL analysis@ on the selected objects of type @Distance and calculates "
	"a @Configuration from them. Uses the selected Configuration object as the initial Configuration in "
	"the iteration process.")
MAN_END

MAN_BEGIN (U"Distance & Configuration & Salience: Get VAF...", U"djmw", 19971201)
INTRO (U"Calculates the \"variance accounted for\" from the selected collection "
	"of @Distance objects, the "
	"selected @Configuration and the selected #Salience.")
MAN_END

MAN_BEGIN (U"Distance & Configuration & Salience: To Configuration "
	"(indscal)...", U"djmw", 19971201)
INTRO (U"A command that creates a new @Configuration from the selected "
	"collection of @Distance objects, the selected @Configuration and the "
	"selected #Salience. The selected Configuration and Salience "
	"serve as start values for the @@INDSCAL analysis@.")
MAN_END

MAN_BEGIN (U"individual difference scaling", U"djmw", 19970502)
INTRO (U"The purpose of individual difference scaling is to represent objects, "
	"whose dissimilarities are given, as points in a metrical space. "
	"The distances in the space should be in accordance with the "
	"dissimilarities as well as is possible. Besides the configuration a "
	"@Salience matrix is calculated.")
NORMAL (U"The basic Euclidean model is:")
LIST_ITEM (U"%\\de__%ijk_ \\~~ (\\su__%s=1..%r_ %w__%ks_(%x__%is_ \\-- "
	"%x__%js_)^2)^^1/2^")
NORMAL (U"Here \\de__%ijk_ is the (known) dissimilarity between %objects %i and "
	"%j, as measured on %data %source %k. The %x's are the %coordinates of "
	"the objects in an %r-dimensional space and the %w's are weights or "
	"saliences. Because straight minimization of the expression above "
	"is difficult, one applies transformations on this expression. "
	"Squaring both sides gives the model:")
LIST_ITEM (U"%\\de^2__%ijk_ \\~~ \\su__%s=1..%r_ %w__%ks_(%x__%is_ \\-- "
	"%x__%js_)^2")
NORMAL (U"and the corresponding least squares loss function:")
LIST_ITEM (U"\\su__%k=1..%numberOfSources_ \\su__%i=1..%numberOfPoints_ "
	"\\su__%j=1..%numberOfPoints_ (%\\de^2__%ijk_ \\-- %d^2__%ijk_)^2")
NORMAL (U"This loss function is minimized in the (ratio scale option of the) "
	"#ALSCAL program of @@Takane, Young & de Leeuw (1976)@.")
NORMAL (U"The transformation used by @@Carroll & Chang (1970)@ in the INDSCAL "
	"model, transforms the data from each source into scalar products "
	"of vectors. For the dissimilarities:")
LIST_ITEM (U"%\\be__%ijk_ = \\--{ %\\de^2__%ijk_ \\-- %\\de^2__%i.%k_ \\-- "
	"%\\de^2__.%jk_ + %\\de^2__..%k_ } / 2,")
NORMAL (U"where dots replacing indices indicate averaging over the range of "
	"that index. In the same way for the distances:")
LIST_ITEM (U"%z__%ijk_ = \\--{ %d^2__%ijk_ \\--  %d^2__%i.%k_ \\--  %d"
	"^2__%.%jk_ +  %d^2__%..%k_ } / 2.")
LIST_ITEM (U"%\\be__%ijk_ \\~~ %z__%ijk_ = \\su__%s=1..%numberOfDimensions_ "
	"%w__%ks_ %x__%is_ %x__%js_")
NORMAL (U"Translated into matrix algebra, the equation above translates to:")
LIST_ITEM (U"%B__%k_ \\~~ %Z__%k_ = %X %W__%k_ %X\\'p,")
NORMAL (U"where %X is a %numberOfPoints \\xx %numberOfDimensions configuration "
	"matrix, %W__%k_, a non-negative %numberOfDimensions \\xx "
	"%numberOfDimensions matrix with weights, and %B__%k_ "
	"the %k^^th^ slab of %\\be__%ijk_.")
NORMAL (U"This translates to the following INDSCAL loss function:")
FORMULA (U"%f(%X, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%k=1..%numberOfSources_ | %B__%k_ \\-- %X%W__%k_%X\\'p |^2")
NORMAL (U"")
MAN_END

MAN_BEGIN (U"INDSCAL analysis", U"djmw", 20120306)
INTRO (U"A method for @@individual difference scaling@ analysis in P\\s{RAAT}.")
NORMAL (U"An INDSCAL analysis can be performed on objects of type Distance.")
NORMAL (U"If you start with @Dissimilarity objects you first have to transform "
	"them to Distance objects.")
LIST_ITEM (U"\\bu @@Dissimilarity: To Distance...@")
NORMAL (U"If you start with a @Confusion you can use:")
LIST_ITEM (U"\\bu @@Confusion: To Dissimilarity (pdf)...@")
ENTRY (U"Examples")
LIST_ITEM (U"\\bu @@Distance: To Configuration (indscal)...@")
DEFINITION (U"Perform an INDSCAL analysis on one or more objects of type "
	"@Distance to calculate a @Configuration.")
LIST_ITEM (U"\\bu @@Distance & Configuration: To Configuration (indscal)...@")
DEFINITION (U"Perform an INDSCAL analysis on one or more objects of type "
	"@Distance to calculate a @Configuration. Use the selected Configuration "
	"object as the initial Configuration in the iteration process.")
ENTRY (U"Algorithm")
NORMAL (U"The function to be minimized in INDSCAL is the following:")
FORMULA (U"%f(%X, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%i=1..%numberOfSources_ |%S__%i_ \\-- %XW__%i_%X\\'p|^2")
NORMAL (U"where %X an unknown %numberOfPoints x %numberOfDimensions "
	"configuration matrix, the %W__%i_ are %numberOfSources unknown "
	"diagonal %numberOfDimensions x %numberOfDimensions matrices with weights, "
	"often called saliences, and the %S__%i_ are known symmetric "
	"matrices with scalar products of dimension %numberOfPoints x "
	"%numberOfPoints.")
NORMAL (U"In the absence of an algorithm that minimizes %f, @@Carroll & "
	"Chang (1970)@ resorted to the @CANDECOMP algorithm, which instead of the "
	"function given above minimizes the following function:")
FORMULA (U"%g(%X, %Y, %W__1_,..., %W__%numberOfSources_) = "
	"\\su__%i=1..%numberOfSources_ |%S__%i_ \\-- %XW__%i_%Y\\'p|^2.")
NORMAL (U"Carroll & Chang claimed that for most practical circumstances %X "
	"and %Y converge to matrices that will be columnwise proportional. "
	"However, INDSCAL does not only require symmetry of the solution, "
	"but also non-negativity of the weights. Both these aspects cannot be "
	"guaranteed with the CANDECOMP algorithm.")
NORMAL (U"@@Ten Berge, Kiers & Krijnen (1993)@ describe an algorithm that "
	"automatically satisfies symmetry because it solves %f directly, and, "
	"also, can guarantee non-negativity of the weights. "
	"This algorithm proceeds as follows:")
NORMAL (U"Let #%x__%h_ be the %h-th column of %X. We then write the function %f above as:")
FORMULA (U"%f(#%x__%h_, %w__1%h_, ..., %w__%numberOfSources %h_) = \\su__%i=1.."
	"%numberOfSources_ |%S__%ih_ \\-- #%x__%h_%w__%ih_#%x\\'p__%h_|^2,")
NORMAL (U"with %S__%ih_ defined as:")
FORMULA (U"%S__%ih_ = (%S__%i_ - \\su__%j\\=/%h, %j=1..%numberOfDimensions_ "
	"#%x__%j_%w__%ij_#%x\\'p__%j_).")
NORMAL (U"Without loss of generality we may require that")
FORMULA (U"#%x\\'p__%h_#%x__%h_ = 1")
NORMAL (U"Minimizing %f over #%x__%h_ is equivalent to minimizing")
FORMULA (U"\\su__%i=1..%numberOfSources_ |%S__%ih_|^2 \\-- 2tr \\su "
	"%S__%ih_#%x__%h_%w__%ih_#%x\\'p__%h_ + \\su %w^2__%ih_")
NORMAL (U"This amounts to maximizing")
FORMULA (U"%g(#%x__%h_) = #%x\\'p__%h_(\\su %w__%ih_%S__%ih_)#%x__%h_")
NORMAL (U"subject to #%x\\'p__%h_#%x__%h_ = 1. The solution for #%x__%h_ is "
	"the dominant eigenvector of (\\su %w__%ih_%S__%ih_), "
	"which can be determined with the power method (see @@Golub & van Loan "
	"(1996)@). The optimal value "
	"for the %w__%ih_, given that all other parameters are fixed:")
FORMULA (U"%w__%ih_ = #%x\\'p__%h_%S__%ih_#%x__%h_")
NORMAL (U"In an alternating least squares procedure we may update columns of "
	"%X and the diagonals of the %W matrices in any sensible order.")
MAN_END

MAN_BEGIN (U"Kruskal analysis", U"djmw", 19971201)
INTRO (U"One of the @@MDS models@ in P\\s{RAAT}.")
NORMAL (U"You can perform a Kruskal-type multidimensional scaling only on "
	"objects of type @Dissimilarity. Objects of other types first have to "
	"be converted to objects of Dissimilarity type.")
ENTRY (U"Example")
NORMAL (U"Convert a @Dissimilarity object into a @Configuration object.")
LIST_ITEM (U"\\bu @@Dissimilarity: To Configuration (monotone mds)...@")
DEFINITION (U"choose appropriate parameters")
LIST_ITEM (U"\\bu @@Dissimilarity & Configuration: Get stress (monotone mds)...@")
DEFINITION (U"choose stress-1 to obtain the value for the @stress according "
	"to Kruskal.")
ENTRY (U"How to get started")
NORMAL (U"You can create an example @Dissimilarity object with the @@Create "
	"letter R example...@ button which you can find under the "
	"##Multidimensional scaling# option in the #New menu.")
MAN_END

MAN_BEGIN (U"MDS models", U"djmw", 20101109)
INTRO (U"Multidimensional scaling (MDS) models are defined by specifying "
	"how given @Dissimilarity data, %\\de__%ij_, are "
	"mapped into distances of an %m-dimensional MDS @Configuration %#X. "
	"The mapping is specified by a %%representation function%, %f : "
	"%\\de__%ij_ \\-> %d__%ij_(#X), which specifies how "
	"dissimilarities should be related to the distances. The MDS analysis "
	"tries to find the configuration (in a given dimensionality) whose "
	"distances satisfy %f as closely as possible. "
	"This closeness is quantified by a badness-of-fit measure which is often "
	"called @stress.")
ENTRY (U"Representation functions")
NORMAL (U"In the application of MDS we try to find a configuration #X such that "
	"the following relations are satisfied as well as possible:")
FORMULA (U"%f(%\\de__%ij_) \\~~ %d__%ij_(#X)")
NORMAL (U"The numbers that result from applying %f on %\\de__%ij_ are sometimes "
	"called @disparities %d\\'p__%ij_. In most applications of MDS, besides "
	"the configuration #X, also the function %f is not "
	"completely specified, i.e., the exact parameters of %f are unknown and "
	"must also be estimated during the analysis. If no particular %f can be "
	"derived from a theoretical model, one often restricts %f to a particular "
	"class of functions on the basis of the scale level of the dissimilarity "
	"data. If the disparities are related to the proximities by a specific "
	"parametric function we speak of %metric MDS otherwise we speak of "
	"%ordinal or %%non-metric% MDS.")
LIST_ITEM (U"\\bu %absolute mds: %d\\'p__%ij_ = \\de__%ij_")
DEFINITION (U"No parameters need to be estimated.")
LIST_ITEM (U"\\bu %ratio mds: %d\\'p__%ij_ = %b \\.c \\de__%ij_,")
DEFINITION (U"where the value of %b can be estimated by a linear regression of "
	"%d__%ij_ on %\\de__%ij_.")
LIST_ITEM (U"\\bu %interval mds: %d\\'p__%ij_ = %a + %b \\.c %\\de__%ij_,")
DEFINITION (U"where the values of %a and %b can be estimated by a linear "
	"regression of %d__%ij_ on %\\de__%ij_.")
LIST_ITEM (U"\\bu %i-spline mds: %d\\'p__%ij_ = %i-spline(%\\de__%ij_),")
DEFINITION (U"where %i-spline(\\.c) is a smooth monotonically increasing "
	"@spline curve. The conceptual idea is that it is not possible to map "
	"all dissimilarities into disparities by one simple function.")
LIST_ITEM (U"\\bu %monotone mds: %d\\'p__%ij_ = %monotone(\\de__%ij_),")
DEFINITION (U"where %monotone(\\.c) is restricted to be a monotonic function "
	"that preserves the order of the dissimilarities:")
FORMULA (U"if %\\de__%ij_ < %\\de__%kl_, then %d__%ij_(#X) < %d__%kl_(#X)")
DEFINITION (U"If %\\de__%ij_ = %\\de__%kl_ and no particular constraint is involved for %d__%ij_(#X) "
	"and %d__%kl_(#X) this is referred to as the %%primary approach% to ties. The %%secondary "
	"approach% to ties requires that if %\\de__%ij_ = %\\de__%kl_, then also %d__%ij_(#X) = %d__%kl_(#X).")
NORMAL (U"More information on all aspects of multidimensional scaling can be found in: "
	"@@Borg & Groenen (1997)@ and @@Ramsay (1988)@.")
NORMAL (U"The most important object types used in Praat for MDS and the conversions between these types are "
	"shown in the following figure.")
PICTURE (6, 6, drawMDSClassRelations)
MAN_END

MAN_BEGIN (U"Measurement levels", U"djmw", 20151014)
INTRO (U"According to the measurement theory of @@Stevens (1951)@, there are four measurement levels, namely "
	"#Nominal, #Ordinal, #Interval and #Ratio. In the light of multidimensional scaling, the first "
	"two levels, Nominal and Ordinal, are often called %non-%metric. The last two are %metric.")
MAN_END

MAN_BEGIN (U"Multidimensional scaling", U"djmw", 20140117)
INTRO (U"This tutorial describes how you can use P\\s{RAAT} to "
	"perform ##M#ulti ##D#imensional ##S#caling (MDS) analysis.")
NORMAL (U"MDS helps us to represent %dissimilarities between objects as "
	"%distances in a %%Euclidean space%. In effect, the more dissimilar two "
	"objects are, the larger the distance between the objects in the Euclidean "
	"space should be. The data types in P\\s{RAAT} that "
	"incorporate these notions are @Dissimilarity, @Distance and "
	"@Configuration.")
NORMAL (U"In essence, an MDS-analysis is performed when you select a "
	"Dissimilarity object and choose one of the ##To Configuration (xxx)...# "
	"commands to obtain a Configuration object. In the above, method (xxx) "
	"represents on of the possible @@MDS models|multidimensional scaling "
	"models@.")
ENTRY (U"MDS-analysis")
NORMAL (U"Let us first create a Dissimilarity object. You can for example "
	"@@Dissimilarity|create a Dissimilarity object from a file@. Here we "
	"will the use the Dissimilarity object from @@Create letter R example...|"
	"the letter R example@. We have chosen the default value (32.5) for the "
	"(uniform) noise range. Note that this may result in substantial "
	"distortions between the dissimilarities and the distances.")
NORMAL (U"Now you can do the following, for example:")
NORMAL (U"Select the Dissimilarity and choose @@Dissimilarity: To Configuration "
	"(monotone mds)...|To Configuration (monotone mds)...@, and you perform "
	"a @@Kruskal analysis|kruskal@-like multidimensional scaling which "
	"results in a new "
	"Configuration object. (This Configuration could subsequently be used as "
	"the starting Configuration for a new MDS-analysis!).")
NORMAL (U"Select the Configuration and choose @@Configuration: Draw...|Draw...@ "
	"and the following picture might result.")
PICTURE (4.0, 4.0, drawLetterRConfigurationExample2)
NORMAL (U"The following script summarizes:")
CODE (U"dissimilarity = Create letter R example: 32.5")
CODE (U"configuration = To Configuration (monotone mds): 2, \"Primary approach\", 0.00001, 50, 1")
CODE (U"Draw: 1, 2, -0.8, 1.2, -0.8, 0.7, \"yes\"")
ENTRY (U"Obtaining the stress value")
NORMAL (U"Select the Dissimilarity and the Configuration together and query for "
	"the @stress value with: "
	"@@Dissimilarity & Configuration: Get stress (monotone mds)...|"
	"Get stress (monotone mds)...@. ")
NORMAL (U"The following script summarizes:")
CODE (U"selectObject: dissimilarity, configuration")
CODE (U"Get stress (monotone mds): \"Primary approach\", \"Kruskals's "
	"stress-1\"")
ENTRY (U"The Shepard diagram")
NORMAL (U"Select the Dissimilarity and the Configuration together to "
	"@@Dissimilarity & Configuration: Draw Shepard diagram...|"
	"draw the Shepard diagram@.")
PICTURE (4.0, 4.0, drawLetterRShepard)
NORMAL (U"The following script summarizes:")
CODE (U"selectObject: dissimilarity, configuration")
CODE (U"Draw Shepard diagram: 0, 200, 0, 2.2, 1, \"+\", \"yes\"")
ENTRY (U"The (monotone) regression")
NORMAL (U"Select the Dissimilarity and the Configuration together to "
	"@@Dissimilarity & Configuration: Draw regression (monotone mds)...|"
	"draw the monotone regression@ of distances on dissimilarities.")
PICTURE (4.0, 4.0, drawLetterRRegression)
NORMAL (U"The following script summarizes:")
CODE (U"selectObject: dissimilarity, configuration")
CODE (U"Draw monotone regresion: \"Primary approach\", 0, 200, 0, 2.2, 1, \"+\", \"yes\"")
NORMAL (U"When you enter %noiseRange = 0 in the form for the letter #R, perfect "
	"reconstruction is possible. The Shepard diagram then will show "
	"a perfectly smooth monotonically increasing function.")
ENTRY (U"Weighing the dissimilarities")
NORMAL (U"When you can't have equal confidence in all the number in the "
	"Dissimilarity object, you can give different weights to these numbers by "
	"associating a @Weight object with the Dissimilarity object. "
	"An easy way to do this is to select the Dissimilarity object and first "
	"choose @@Dissimilarity: To Weight|To Weight@. Then you might change the "
	"individual weights in the Weight object with the @@TableOfReal: Set "
	"value...| Set value...@ command (remember: make %w__%ij_ = %w__%ji_).")
NORMAL (U"The following script summarizes:")
CODE (U"selectObject: dissimilarity")
CODE (U"weight = To Weight")
CODE (U"! Change [i][j] and [j][i] cells in the Weight object")
CODE (U"Set value: i, j, val")
CODE (U"Set value: j, i, val")
CODE (U"...")
CODE (U"! now we can do a weighed analysis.")
CODE (U"selectObject: dissimilarity, weight")
CODE (U"To Configuration (monotone mds): 2, \"Primary approach\", 0.00001, 50, 1)")
NORMAL (U"You can also query the @stress values with three objects selected. "
	"The following script summarizes:")
CODE (U"selectObject: dissimilarity, weight, configuration")
CODE (U"Get stress (monotone mds): \"Primary approach\", \"Kruskals's stress-1\"")
ENTRY (U"Using a start Configuration")
NORMAL (U"You could also use a Configuration object as a starting "
	"configuration in the minimization process. "
	"Let's assume that you are not satisfied with the stress value from the "
	"Configuration object that you obtained in the previous analysis. "
	"You can than use this Configuration object as a "
	"starting point for further analysis:")
NORMAL (U"The following script summarizes:")
CODE (U"selectObject: dissimilarity, configuration, weight")
CODE (U"To Configuration (monotone mds): 2, \"Primary approach\", 0.00001, 50, 1")
ENTRY (U"Multiple Dissimilarity's (INDSCAL)")
NORMAL (U"When you have multiple Dissimilarity objects you can also perform "
	"@@individual difference scaling@ (often called @@INDSCAL analysis@). ")
NORMAL (U"As an example we can use an @@Create INDSCAL Carroll & Wish "
	"example...| example taken from Carrol & Wish@. "
	"Because INDSCAL only works on metrical data, we cannot use Dissimilarity "
	"objects directly. We have to transform them first @@Dissimilarity: To "
	"Distance...|to Distance@ objects.")
NORMAL (U"This type of analysis on multiple objects results in two new objects: "
	"a Configuration and a @Salience.")
MAN_END

MAN_BEGIN (U"Procrustes", U"djmw", 20010927)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"An object of type Procrustes represents the special @@AffineTransform|"
	"affine transform@ that consists of a "
	"combination of a translation, a shape preserving transformation and a scaling (this scaling is often called %dilation). "
	"Because the transformation has to be shape preserving, only a combination of a rotation and a reflection is allowed. "
	"A configuration matrix #%X is transformed in the following way to a new configuration matrix #%Y: ")
FORMULA (U"#%Y = %s #%X #%T+ #1#%t',")
NORMAL (U"where %s is the scaling factor, #%T is the shape preserving transformation matrix, #%t is the translation vector, "
	"and #1 is the vector with only ones as its elements.")
NORMAL (U"For more information about the Procrustes transform and its algorithm "
	"see chapter 19 in @@Borg & Groenen (1997)@.")
MAN_END

MAN_BEGIN (U"Procrustes transform", U"djmw", 19980119)
INTRO (U"A transformation that only uses a combination of a translation, "
	"a scaling and a rigid transformation to transform one Configuration such that it "
	"matches as closely as possible another Configuration. ")
NORMAL (U"We speak of %%orthogonal Procrustes transform% when only the rigid "
	"transformation is allowed but no scaling or translation.")
NORMAL (U"For more information about the Procrustes transform and its algorithm "
	"see chapter 19 in @@Borg & Groenen (1997)@.")
MAN_END

MAN_BEGIN (U"Proximity", U"djmw", 19961008)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"An object of type #Proximity represents proximities between objects.")
ENTRY (U"Inside a Proximity")
NORMAL (U"With @Inspect you will see the following attributes:")
TAG (U"%numberOfRows, %numberOfColumns")
DEFINITION (U"the number of objects (%numberOfRows and %numberOfColumns are "
	"equal and \\>_1).")
TAG (U"%rowLabels, %columnLabels")
DEFINITION (U"the names associated with the objects (%rowLabels and "
	"%columnLabels are equal.")
TAG (U"%data [1..%numberOfRows][1..%numberOfColumns]")
DEFINITION (U"the proximities between the objects.")
MAN_END

MAN_BEGIN (U"Salience", U"djmw", 19980112)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"Elements %s__%ij_ in the "
	"Salience matrix represent the importance of dimension %j (in the "
	"@Configuration) for data source %i.")
ENTRY (U"Commands")
NORMAL (U"Creation, as a by-product of:")
LIST_ITEM (U"\\bu @@Distance: To Configuration (indscal)...")
LIST_ITEM (U"\\bu @@Distance: To Configuration (ytl)...")
MAN_END

MAN_BEGIN (U"ScalarProduct", U"djmw", 19980125)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"An object of type ScalarProduct represents scalar products %b__%ij_ "
	"between objects %i and %j in a metrical space.")
FORMULA (U"%b__%ij_ = \\su__%k=1..%numberOfDimensions_ %x__%ik_%x__%jk_,")
NORMAL (U"where %x__%ik_ and %x__%jk_ are the coordinates of the %k-th "
	"dimension of points %i and %j, respectively. From this definition one "
	"can see that scalar products, in contrast to distances, "
	"do change when the origin is shifted.")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu @@Distance: To ScalarProduct...@")
MAN_END

MAN_BEGIN (U"Similarity", U"djmw", 19961008)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"An object of type Similarity represent a one-way "
	"table of similarities between \"objects\".")
ENTRY (U"Commands")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu @@Confusion: To Similarity...")
NORMAL (U"Drawing")
LIST_ITEM (U"\\bu ##Draw as numbers...")
LIST_ITEM (U"\\bu ##Draw as squares...")
NORMAL (U"Query")
LIST_ITEM (U"\\bu ##Get column mean (index)...")
LIST_ITEM (U"\\bu ##Get column mean (label)...")
LIST_ITEM (U"\\bu ##Get column stdev (index)...")
LIST_ITEM (U"\\bu ##Get column stdev (label)...")
NORMAL (U"Modification")
LIST_ITEM (U"\\bu ##Formula...")
LIST_ITEM (U"\\bu ##Remove column (index)...")
LIST_ITEM (U"\\bu ##Insert column (index)...")
LIST_ITEM (U"\\bu ##Set row label (index)...")
LIST_ITEM (U"\\bu ##Set row label (label)...")
LIST_ITEM (U"\\bu ##Set column label (index)...")
LIST_ITEM (U"\\bu ##Set column label (label)...")
NORMAL (U"Analysis")
LIST_ITEM (U"\\bu @@Similarity: To Dissimilarity...")
MAN_END

MAN_BEGIN (U"Similarity: To Dissimilarity...", U"djmw", 20040407)
INTRO (U"A command that creates a @Dissimilarity from every selected "
	"@Similarity.")
ENTRY (U"Setting")
TAG (U"##Maximum dissimilarity")
DEFINITION (U"determines the maximum dissimilarity possible. When the default "
	"value, 0.0, is chchosen%maximumDissimilarity "
	"is calculated as the maximum element in the Similarity object.")
ENTRY (U"Algorithm")
NORMAL (U"To obtain dissimilarities we 'reverse' similarities:")
FORMULA (U"%%dissimilarity__%ij_ = %maximumDissimilarity \\-- %similarity__%ij_")
NORMAL (U"In this way the order of dissimilarities is the reverse of the order "
	"of the similarities.")
MAN_END

MAN_BEGIN (U"smacof", U"djmw", 19980119)
INTRO (U"Scaling by Majorizing a Complicated Function, the iterative algorithm "
	"to find an optimal Configuration.")
LIST_ITEM (U"1.  Initialize")
LIST_ITEM (U"   1.a. Get initial Configuration #Z")
LIST_ITEM (U"   1.b. Set stress %\\si__%n_^^[0]^ to a very large value.")
LIST_ITEM (U"   1.c. Set iteration counter %k = 0")
LIST_ITEM (U"2.  Increase iteration counter by one: %k = %k + 1")
LIST_ITEM (U"3.  Calculate distances %d__%ij_(#Z).")
LIST_ITEM (U"4.  Transform dissimilarities %\\de__%ij_ into disparities "
	"%d\\'p__%ij_.")
LIST_ITEM (U"5.  Standardize the disparities so that %\\et__%d\\'p_^2 = "
	"%n(%n\\--1)/2.")
LIST_ITEM (U"6.  Compute the Guttman transform #X^^[%k]^ of #Z.")
LIST_ITEM (U"7.  Compute new distances %d__%ij_(#X^^[%k]^).")
LIST_ITEM (U"8.  Compute normalized stress %\\si__%n_ (#d\\'p, #X^^[%k]^)")
LIST_ITEM (U"9.  If |%\\si__%n_^^[%k]^ \\-- %\\si__%n_^^[%k\\--1]^| / %\\si__%n_"
	"^^[%k\\--1]^ < %\\ep or %k > %maximumNumberOfIterations, then stop")
LIST_ITEM (U"10. Set #Z = #X^^[%k]^, and go to 2.")
NORMAL (U"This algorithm goes back to @@De Leeuw (1977)@.")
MAN_END

MAN_BEGIN (U"spline", U"djmw", 20121101)
INTRO (U"A spline function %f is a piecewise polynomial function defined on "
	"an interval [%x__%min_, %x__%max_] "
	"with specified continuity constraints, i.e., when the interval [%x__%min_,"
	" %x__%max_] is subdivided by points %\\xi__%i_ such that %x__%min_ = "
	"%\\xi__%1_ < ... < %\\xi__%q_ = %%x__%max_, then within each subinterval "
	"[%\\xi__%j_, %\\xi__%j+1_) the function is a polynomial %P__%j_ of "
	"specified degree %k.")
NORMAL (U"A %%knot sequence% %t = {%t__1_, ..., %t__%n+%k_}, where %n is the "
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
FORMULA (U"%n = %numberOfInteriorKnots + %order.")
NORMAL (U"With suitable basis functions, for example, the M-spline family "
	"%M__%i_(%x|%k, %t), %i=1..%n, we can write any spline %f in the form:")
FORMULA (U"%f = \\su__%i=1..%n_ %a__%i_%M__%i_,")
NORMAL (U"where the %M__%i_ are defined by the following recursive formula's:")
FORMULA (U"%M__%i_(%x|1,%t) = 1 / (%t__%i+1_ \\-- %t__%i_),           %t__%i_ "
	"\\<_ %x < %t__%i+1_, 0 otherwise")
FORMULA (U"%M__%i_(%x|%k,%t) = %k [(%x\\--%t__%i_)%M__%i_(%x|%k\\--1,%t) + "
	"(%t__%i+%k_\\--%x)%M__%i+1_(%x|%k\\--1,%t)] / "
	"((%k\\--1)(%t__%i+%k_\\--%t__%i_))")
NORMAL (U"These %M__%i_ are localized because %M__%i_(%x|%k,%t) > 0 only when "
	"%t__%i_ \\<_ %x < %t__%i+%k_ and zero otherwise. Also, we have \\in"
	" M__%i_(%x)%dx = 1. Because of this localization a change in coefficient "
	"%a__%i_ will only effect %f within this interval.")
NORMAL (U"The following picture shows an M-spline of order 3 on the interval "
	"[0, 1], with three interior knots at 0.3, 0.5 and 0.6.")
PICTURE (5.0, 5.0, drawMsplineExample)
NORMAL (U"Because the M-splines are nonnegative, %monotone splines% can be "
	"derived from them by %integration:")
FORMULA (U"%I__%i_(%x|%k,%t) = \\in__%xmin_^%x %M__%i_(%u|%k,%t) %du")
NORMAL (U"Because each %M__%i_(%x|%k, %t) is a piecewise polynomial of degree"
	" %k\\--1, each %I__%i_ will be of degree %k. Now we can write:")
FORMULA (U"%f = \\su__%i=1..%n_ %b__%i_%I__%i_(%x|%k,%t)")
NORMAL (U"We can use an M-spline of order %k+1 with a simple knot sequence %t, "
	"for which %t__%j_ \\<_ x < %t__%j+1_, to put "
	"the I-spline of order %k into a more convenient form:")
FORMULA (U"%I__%i_(%x|%k,%t) = 0,     %i > %j")
FORMULA (U"%I__%i_(%x|%k,%t) = \\su__%m=%i+1..%j_ (%t__%m+%k+1_\\--"
	"%t__%m_)%M__%m_(%x|%k+1,%t)/(%k+1),     %j\\--%k \\<_ %i \\<_ %j")
FORMULA (U"%I__%i_(%x|%k,%t) = 1,     %i < %j\\--%k")
NORMAL (U"The following figure shows the I-splines that were derived from "
	"the M-splines above.")
PICTURE (5.0, 5.0, drawIsplineExample)
NORMAL (U"These spline formula's were taken from @@Ramsay (1988)@ and the "
	"errors in his I-spline formulas were corrected.")
MAN_END

MAN_BEGIN (U"stress", U"djmw", 19980108)
INTRO (U"A badness-of-fit measure for the entire MDS representation.")
NORMAL (U"Several measures exist.")
ENTRY (U"Raw stress")
FORMULA (U"%\\si__%r_ (#d\\'p, #X) = \\su__%i<%j_ %w__%ij_(%d\\'p__%ij_ \\-- "
	"%d__%ij_(#X))^2")
FORMULA (U"= \\su__%i<%j_ %w__%ij_%d\\'p__%ij_^2 + \\su__%i<%j_ "
	"%w__%ij_%d__%ij_^2(#X) \\-- "
	"2 \\su__%i<%j_ %w__%ij_%d\\'p__%ij_%d__%ij_(#X)")
FORMULA (U"= %\\et__%d\\'p_^2 + %\\et^2(#X) \\-- 2%\\ro(#d\\'p, #X)")
NORMAL (U"where the %d\\'p__%ij_ are the @disparities that are the result "
	"from the transformation of the dissimilarities, i.e., %f(%\\de__%ij_). "
	"Raw stress can be misleading because it is dependent on the normalization "
	"of the disparities. The following measure tries to circumvent this "
	"inconvenience.")
ENTRY (U"Normalized stress")
FORMULA (U"%\\si__%n_ = \\si__%r_ / %\\et__%d\\'p_^2")
NORMAL (U"This is the stress function that we minimize by iterative "
	"majorization. It goes back to @@De Leeuw (1977)@.")
ENTRY (U"Kruskal's stress-1")
FORMULA (U"%\\si__1_ = \\Vr (\\su__%i<%j_ %w__%ij_(%d\\'p__%ij_ \\-- "
	"%d__%ij_(#X))^2 / \\su__%i<%j_ %w__%ij_%d__%ij_^2(#X))^^1/2^")
NORMAL (U"In this measure, which is due to @@Kruskal (1964)@, stress is "
	"expressed in relation to the size of #X.")
ENTRY (U"Kruskal's stress-2")
FORMULA (U"%\\si__2_ = \\Vr (\\su__%i<%j_ %w__%ij_(%d\\'p__%ij_ \\-- "
	"%d__%ij_(#X))^2 / \\su__%i<%j_ %w__%ij_(%d__%ij_(#X) - "
	"%averageDistance)^2)^^1/2^.")
NORMAL (U"In general, this measure results in a stress value that is "
	"approximately twice the value for stress-1.")
ENTRY (U"Relation between %\\si__1_ and %\\si__n_")
NORMAL (U"When we have calculated %\\si__%n_ for Configuration #X, "
	"disparities #d\\'p and Weight #W we cannot "
	"directly use #X, #d\\'p and #W to calculate %\\si__1_ because the "
	"scale of #X is not necessarily optimal "
	"for %\\si__1_. We allow therefore a scale factor %b > 0 and "
	"try to calculate  %\\si__1_ (#d\\'p, %b #X). We minimize the resulting "
	"expression for %b and substitute "
	"the result back into the formula for stress, i.e.,")
FORMULA (U"%\\si__1_^2 (#d\\'p, %b #X) = (%\\et__%d\\'p_^2 + %b^2 %\\et^2(#X) "
	"\\-- 2 %b %\\ro(#d\\'p, #X)) / %b^2 %\\et^2(#X)")
FORMULA (U"d%\\si__1_^2 (%b) / d%b == 0, gives")
FORMULA (U"%b = %\\et__%d\\'p_^2 / %\\ro")
FORMULA (U"%\\si__1_^2 =  (1 - %\\ro^2 / (%\\et__%d\\'p_^2\\.c%\\et^2(#X)))")
NORMAL (U"This means that %\\si__1_ = \\Vr %\\si__%n_.")
ENTRY (U"Relation between %\\si__2_ and %\\si__n_")
NORMAL (U"We can do the same trick as before for %\\si__2_:")
FORMULA (U"%\\si__2_^2 (#d\\'p, %b #X) = (%\\et__%d\\'p_^2 + %b^2 %\\et^2(#X) "
	"\\-- 2 %b %\\ro(#d\\'p, #X)) / "
	"(%b^2 \\su__%i<%j_ %w__%ij_(%d__%ij_(#X) - %averageDistance)^2)")
NORMAL (U"From which we derive:")
FORMULA (U"%\\si__2_ = \\Vr ((%\\et__%d\\'p_^2 \\.c %\\et^2(#X) - "
	"%\\ro^2(#d\\'p, #X)) / (%\\et__%d\\'p_^2 \\.c \\su__%i<%j_ "
	"%w__%ij_(%d__%ij_(#X) - %averageDistance)^2))")
MAN_END

MAN_BEGIN (U"TableOfReal: Centre columns", U"djmw", 19980422)
INTRO (U"A command that centres the columns in the selected @TableOfReal "
	"objects.")
ENTRY (U"Algorithm")
NORMAL (U"The new values in the table, %x\\'p__%ij_, will be:")
FORMULA (U"%x\\'p__%ij_ = %x__%ij_ \\-- %x__\\.c%j_,")
NORMAL (U"where")
FORMULA (U"%x__\\.c%j_ = \\su__%i=1..%numberOfRows_ %x__%ij_ / %numberOfRows,")
NORMAL (U"the average of column %j.")
MAN_END

MAN_BEGIN (U"TableOfReal: Centre rows", U"djmw", 19980422)
INTRO (U"A command that centres the rows in the selected @TableOfReal objects.")
ENTRY (U"Algorithm")
NORMAL (U"The new values in the table, %x\\'p__%ij_, will be:")
FORMULA (U"%x\\'p__%ij_ = %x__%ij_ \\-- %x__%i\\.c_,")
NORMAL (U"where")
FORMULA (U"%x__%i\\.c_ = \\su__%j=1..%numberOfColumns_ %x__%ij_ / "
	"%numberOfColumns,")
NORMAL (U"the average of row %i.")
MAN_END

MAN_BEGIN (U"TableOfReal: Get table norm", U"djmw", 19980422)
INTRO (U"A command that returns the norm of the selected @TableOfReal object.")
ENTRY (U"Algorithm")
NORMAL (U"Returns: sqrt (\\su__%i=1..%numberOfRows_ \\su__%j=1..%numberOfColumns"
	"_ %x__%ij_^2).")
MAN_END

MAN_BEGIN (U"TableOfReal: Normalize columns...", U"djmw", 19980422)
INTRO (U"A command that normalizes the columns in the selected @TableOfReal "
	"objects.")
ENTRY (U"Setting")
TAG (U"##Norm")
DEFINITION (U"determines the sum of the squared elements in each column after "
	"normalization.")
ENTRY (U"Algorithm")
NORMAL (U"All elements %x__%ij_ in each column %j=1..%numberOfColumns will be "
	"multiplied by sqrt (%norm / \\su__%i=1..%numberOfRows_ %x__%ij_^2).")
MAN_END

MAN_BEGIN (U"TableOfReal: Normalize rows...", U"djmw", 19980422)
INTRO (U"A command that normalizes the rows in the selected @TableOfReal "
	"objects.")
ENTRY (U"Setting")
TAG (U"##Norm")
DEFINITION (U"determines the sum of the squared elements in each row after "
	"normalization.")
ENTRY (U"Algorithm")
NORMAL (U"All elements %x__%ij_ in each row %i=1..%numberOfRows will be "
	"multiplied by sqrt (%norm / \\su__%j=1..%numberOfColumns_ %x__%ij_^2).")
MAN_END

MAN_BEGIN (U"TableOfReal: Normalize table...", U"djmw", 19980422)
INTRO (U"A command that normalizes the elements in the selected @TableOfReal "
	"objects.")
ENTRY (U"Setting")
TAG (U"##Norm")
DEFINITION (U"determines the sum of the squared elements after normalization.")
ENTRY (U"Algorithm")
NORMAL (U"All elements %x__%ij_ will be multiplied by "
	"sqrt (%norm / \\su__%i=1..%numberOfRows_ \\su__%j=1..%numberOfColumns_"
	" %x__%ij_^2.")
MAN_END

MAN_BEGIN (U"Weight", U"djmw", 19980108)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"An object of type Weight represents a matrix "
	"with weights %w__%ij_.")
NORMAL (U"An object of type Weight selected together with an onject of type"
	" @Dissimilarity can be used to make distinctions in the importance of the"
	" contribution of each individual dissimilarity %\\de__%ij_ to "
	"@stress and therefore to the final configuration. Weights can be "
	"used for instance to code for missing values, i.e., take %w__%ij_ = 0 if "
	"dissimilarity %\\de__%ij_ is missing and %w__%ij_ = 1 if %\\de__%ij_ is"
	" known.")
ENTRY (U"Commands")
NORMAL (U"Creation")
LIST_ITEM (U"\\bu @@Dissimilarity: To Weight")
NORMAL (U"Analysis")
NORMAL (U"See @@Dissimilarity & Weight: To Configuration...@ for help on the "
	"following analysis items: ")
LIST_ITEM (U"\\bu ##Dissimilarity & Weight: To Configuration (monotone mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Weight: To Configuration (i-spline mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Weight: To Configuration (interval mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Weight: To Configuration (ratio mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Weight: To Configuration (absolute mds)...")
NORMAL (U"Query")
NORMAL (U"See @@Dissimilarity & Configuration & Weight: Get stress...@ for help "
	"on the following query items: ")
LIST_ITEM (U"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(monotone mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(i-spline mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(interval mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(ratio mds)...")
LIST_ITEM (U"\\bu ##Dissimilarity & Configuration & Weight: Get stress "
	"(absolute mds)...")
MAN_END

/************ references ***********************************************/

MAN_BEGIN (U"Abramowitz & Stegun (1970)", U"djmw", 19971201)
NORMAL (U"M. Abramowitz & I. Stegun (1970): %%Handbook of mathematical "
	"functions%. New York: Dover Publications.")
MAN_END

MAN_BEGIN (U"Borg & Groenen (1997)", U"djmw", 19971219)
NORMAL (U"I. Borg & P. Groenen (1997): %%Modern multidimensional scaling: "
	"theory and applications%. Springer.")
MAN_END

MAN_BEGIN (U"Brokken (1983)", U"djmw", 19980406)
NORMAL (U" F.B. Brokken (1983): \"Orthogonal Procrustes rotation maximizing "
	"congruence.\" %Psychometrika #48: 343\\--352.")
MAN_END

MAN_BEGIN (U"Cailliez (1983)", U"djmw", 19971201)
NORMAL (U" F. Cailliez (1983): \"The analytical solution of the additive "
	"constant problem.\" %Psychometrika #48, 305-308.")
MAN_END

MAN_BEGIN (U"Carroll & Chang (1970)", U"djmw", 19971201)
NORMAL (U"J.D. Carroll & J.-J. Chang, (1970): \"Analysis of Individual "
	"Differences in Multidimensional scaling via an N-way generalization of "
	"\"Eckart-Young\" Decomposition.\" %Psychometrika #35: 283\\--319.")
MAN_END

MAN_BEGIN (U"Carroll & Wish (1974)", U"djmw", 19971201)
NORMAL (U"J.D. Carroll & M. Wish, (1974): \"Models and methods for three-way "
	"multidimensional scaling.\" In D.H. Krantz, R.C. Atkinson, R.D. Luce & "
	"P. Suppes (eds.): %%Contemporary developments in mathematical psychology: "
	"Vol. 2 Measurement, psychophysics, and neural "
	"information processing%, 283\\--319. New York: Academic Press.")
MAN_END

MAN_BEGIN (U"De Leeuw (1977)", U"djmw", 19971201)
NORMAL (U"J. de Leeuw (1977): \"Applications of convex analysis to "
	"multidimensional scaling.\" In J.R. Barra, F. Brodeau, G. Romier & "
	"B. van Cutsem (eds.): %%Recent developments in statistics%. Amsterdam: "
	"North-Holland. 133\\--145.")
MAN_END

MAN_BEGIN (U"De Leeuw & Pruzansky (1978)", U"djmw", 19971201)
NORMAL (U"J. de Leeuw & S. Pruzansky (1978): \"A new computational method to "
	"fit the weighted Euclidean distance model.\" %Psychometrika #43: 479\\--490.")
MAN_END

MAN_BEGIN (U"Gifi (1990)", U"djmw", 19971207)
NORMAL (U"A. Gifi (1990): %%Nonlinear multivariate analysis%. John Wiley & "
	"Sons Ltd., reprint 1996.")
MAN_END

MAN_BEGIN (U"Golub & van Loan (1996)", U"djmw", 19971207)
NORMAL (U"G. Golub & C. van Loan (1996): %%Matrix computations%. Third edition. "
	"London: The Johns Hopkins University Press.")   // ??
MAN_END

MAN_BEGIN (U"Green, Carmone & Smith (1989)", U"djmw", 19971201)
NORMAL (U"P. Green, F. Carmone, S. Smith (1989): "
	"%%Multidimensional scaling: concepts and applications%. Section 3. Allyn and Bacon.")
MAN_END

MAN_BEGIN (U"Kaiser (1958)", U"djmw", 19980404)
NORMAL (U" H.F. Kaiser (1958): \"The varimax criterion for analytic rotation "
	"in factor analysis.\" %Psychometrika #23: 187\\--200.")
MAN_END

MAN_BEGIN (U"Kiers & Groenen (1996)", U"djmw", 19971219)
NORMAL (U"H.A.L. Kiers & P. Groenen (1996): \"A monotonically convergent "
	"algorithm for orthogonal congruence rotation.\" %Psychometrika #61: "
	"375\\--389.")
MAN_END

MAN_BEGIN (U"Klein, Plomp & Pols (1970)", U"djmw", 19971201)
NORMAL (U" W. Klein, R. Plomp, & L.C.W. Pols (1970): \"Vowel Spectra, "
	"Vowel Spaces, and Vowel Identification.\" %%Journal of the Acoustical Society of America% #48: 999\\--1009.")
MAN_END

MAN_BEGIN (U"Kruskal (1964)", U"djmw", 19971201)
NORMAL (U"J.B. Kruskal (1964): \"Nonmetric multidimensional scaling: a "
	"numerical method.\" %Psychometrika #29: 115\\--129.")
MAN_END

MAN_BEGIN (U"Ramsay (1988)", U"djmw", 19980106)
NORMAL (U"J.O. Ramsay (1988): \"Monotone regression splines in action.\" "
	"%%Statistical Science% #3: 425\\--461.")
MAN_END

MAN_BEGIN (U"Stevens (1951)", U"djmw", 19971201)
NORMAL (U"S.S. Stevens (1951): \"Mathematics, measurement, and psychophysics.\" "
	"In S.S. Stevens (ed.): %%Handbook of experimental psychology%. New York: "
	"Wiley.")
MAN_END

MAN_BEGIN (U"Takane, Young & de Leeuw (1976)", U"djmw", 19971201)
NORMAL (U"Y. Takane, F. Young, J. de Leeuw (1976): \"Non-metric individual "
	"differences multidimensional scaling: an alternating least squares method "
	"with optimal scaling features.\" %Psychometrika #42: 7\\--67.")
MAN_END

MAN_BEGIN (U"Ten Berge (1995)", U"djmw", 19980404)
NORMAL (U"J.M.F. ten Berge (1995): \"Suppressing permutations or rigid planar "
	"rotations: a remedy against nonoptimal varimax rotations.\" "
	"%Psychometrika #60, 437\\--446.")
MAN_END

MAN_BEGIN (U"Ten Berge, Kiers & Krijnen (1993)", U"djmw", 19971207)
NORMAL (U"J.M.F. ten Berge, H.A.L. Kiers & W.P. Krijnen (1993): \"Computational "
	"solutions for the problem of negative saliences and nonsymmetry in "
	"INDSCAL.\" %%Journal of Classification% #10: 115\\--124.")
MAN_END

MAN_BEGIN (U"Torgerson (1958)", U"djmw", 19971201)
NORMAL (U"W.S. Torgerson (1958): %%Theory and methods of scaling%. New York: Wiley.")
MAN_END

MAN_BEGIN (U"Young, Takane & Lewyckyj (1978)", U"djmw", 19971201)
NORMAL (U"F.W. Young, Y. Takane & R. Lewyckyj (1978): "
	"\"Three notes on ALSCAL.\" %Psychometrika #43: 433\\--435.")
MAN_END

MAN_BEGIN (U"Weller & Romney (1990)", U"djmw", 19971216)
NORMAL (U"S.C. Weller & A.K. Romney (1990): %%Metric Scaling: "
	"correspondence analysis%. Sage University Paper Series on Quantitative "
	"Applications in the Social Sciences 07-075. Newbury Park, CA: Sage.")
MAN_END

}

/* End of file manual_MDS.cpp */
