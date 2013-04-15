/* manual_HMM.cpp
 *
 * Copyright (C) 2011-2013 David Weenink
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
	djmw 20101009 Initial version
*/

#include "ManPagesM.h"

void manual_HMM (ManPages me);
void manual_HMM (ManPages me)
{

MAN_BEGIN (L"expectation-maximization", L"djmw", 20111130)
NORMAL (L"Expectation-maximization (EM) is an iterative method used to find maximum "
	"likelihood estimates of parameters in probabilistic models, where the model depends on "
	"unobserved, also called %%latent%, variables. EM alternates between performing an expectation (E) step, "
	"which computes an expectation of the likelihood by including the latent variables as if they "
	"were observed, and a maximization (M) step, which computes the maximum likelihood "
	"estimates of the parameters by maximizing the expected likelihood found in the E step. The "
	"parameters found on the M step are then used to start another E step, and the process is "
	"repeated until some criterion is satisfied. EM is frequently used for data clustering like for example in "
	"@@TableOfReal: To GaussianMixture...|Gaussian mixtures@ or in the @@HMM & HMM_ObservationSequences: Learn...|Baum-Welch training@ of a Hidden Markov Model.")
MAN_END

MAN_BEGIN (L"GaussianMixture", L"djmw", 20101026)
INTRO (L"A Gaussian mixture is a probability density function (p.d.f.). It is a combination of several Gaussian densities.")
NORMAL (L"The GaussianMixture's p.d.f. is defined as the weighted sum of %K multivariate Gaussian p.d.f's:")
FORMULA (L"pdf(x) = \\Si__%%i%=1_^^%K^ p__%i_ %N(%x;\\mu__%i_,\\Si__%i_),")
NORMAL (L"where each %N(%x;\\mu__%i_,\\Si__%i_) is a multivariate p.d.f. with mean  \\mu__%i_ and covariance matrix  \\Si__%i_. The coefficients %p__%i_ sum to 1. ")
NORMAL (L"For an introduction to Gaussian mixtures see for example  @@Bishop (2006)@.")
MAN_END

MAN_BEGIN (L"GaussianMixture: Draw concentration ellipses...", L"djmw", 20101101)
INTRO (L"Draws the concentration ellipse for each component in the @@GaussianMixture@. ")
NORMAL (L"The principal component plane will be determined from @@GaussianMixture: To PCA@.")
NORMAL (L"You might also use another  @PCA and to combine it with a GaussianMixture for drawing  (@@GaussianMixture & PCA: Draw concentration ellipses...@).")
ENTRY (L"Settings")
TAG (L"##Number of sigmas")
DEFINITION (L"determines the @@concentration ellipse|data coverage@.")
TAG (L"##Principal component plane")
DEFINITION (L"determines whether the principal component plane is used or not for drawing.")
MAN_END

MAN_BEGIN (L"GaussianMixture: Get probability at position", L"djmw",20101103)
INTRO (L"Evaluate the pdf of the @@GaussianMixture@ at the given position.")
MAN_END

MAN_BEGIN (L"GaussianMixture & PCA: Draw concentration ellipses...", L"djmw", 20101101)
INTRO (L"Draws the concentration ellipse for each component in the  @@GaussianMixture@ in the plane spanned by the selected  @PCA.")
ENTRY (L"Settings")
TAG (L"##Number of sigmas")
DEFINITION (L"determines the @@concentration ellipse|data coverage@.")
MAN_END

MAN_BEGIN (L"GaussianMixture: Draw marginal pdf...", L"djmw", 20101122)
INTRO (L"A command to draw the marginal p.d.f. (probability density function) of the selected  @GaussianMixture.")
NORMAL (L"A marginal distribution is the projection of the (multivariate) p.d.f. on one dimension or direction. "
	"This direction may also be externally defined by selecting a @PCA and a GaussianMixture together.")
MAN_END

MAN_BEGIN (L"GaussianMixture: Split component...", L"djmw", 20101122)
INTRO (L"Splits one component of the selected  @GaussianMixture into two components.")
NORMAL (L"The selected component is split based on a @PCA analysis of its covariance matrix. The new means are situated "
	"around the old components mean, 1\\si apart in the first principal components direction, and the new covariances are "
	"constructed with information from the old covariance matrix. ")
NORMAL (L"The details of the algorith are described in @@Zhang et al. (2003)@.")
MAN_END

MAN_BEGIN (L"GaussianMixture: To PCA", L"djmw", 20101030)
INTRO (L"Creates a  @PCA from the selected  @GaussianMixture.")
NORMAL (L"The PCA is calculated from the  @@GaussianMixture: To Covariance (total)|total covariance matrix@ of the GaussianMixture.")
MAN_END

MAN_BEGIN (L"GaussianMixture & PCA: To Matrix (density)...", L"djmw", 20101101)
INTRO (L"Represent the @@GaussianMixture@ p.d.f. on the plane spanned by @@PCA@. This makes it possible to draw "
	"the p.d.f. as grey values with one of the special @@Matrix@ image drawing methods.")
NORMAL (L"For each cell in the matrix of dimension  %%numberOfRows% \\xx %%numberOfColumns%, the p.d.f. will be evaluated.")
MAN_END

MAN_BEGIN (L"GaussianMixture: To Covariance (between)", L"djmw", 20101030)
INTRO (L"The covariance between the centers of the components of the @@GaussianMixture@ is calculated; "
	"each center is weighted according to its mixing probability.")
MAN_END

MAN_BEGIN (L"GaussianMixture: To Covariance (within)", L"djmw", 20101120)
INTRO (L"The covariances of the components of the @@GaussianMixture@ are pooled.")
MAN_END

MAN_BEGIN (L"GaussianMixture: To Covariance (total)", L"djmw", 20101030)
INTRO (L"The sum of the @@GaussianMixture: To Covariance (within)|within@ and @@GaussianMixture: To Covariance (between)|between@ covariances of the @@GaussianMixture@ is calculated.")
MAN_END

MAN_BEGIN (L"GaussianMixture: To TableOfReal (random sampling)...", L"djmw", 20101030)
INTRO (L"The selected @@GaussianMixture@ is used as a generator of data.")
ENTRY (L"Setting")
TAG (L"##Number of data points")
DEFINITION (L"determines how many random data point have to be generated.")
ENTRY (L"Algorithm")
NORMAL (L"For each data point to be generated:")
NORMAL (L"1. A random number decides to which component in the mixture the data point will belong.")
NORMAL (L"2. According to the procedure described in @@Covariance: To TableOfReal (random sampling)...@, one "
	"data point will be generated.")
MAN_END

MAN_BEGIN (L"TableOfReal: To GaussianMixture...", L"djmw", 20111130)
INTRO (L"Creates a  @@GaussianMixture@ from the selected  @@TableOfReal@ by an @@expectation-maximization|"
	"expectation-maximization@ procedure.")
ENTRY (L"Settings")
TAG (L"##Number of components")
DEFINITION (L"defines the number of Gaussians in the mixture.")
TAG (L"##Tolerance of minimizer")
DEFINITION (L"defines when to stop optimizing. If the relative difference between the likelihoods at two successive "
	"iteration steps differs by less then the tolerance we stop, i.e. when |(%L(%i-1)-%L(%i))/%L(%i)| < %%tolerance%. ")
TAG (L"##Maximum number of iterations")
DEFINITION (L"defines another stopping criterion. The EM iteration will stop when either the tolerance "
	"is reached or the maximum number of iterations. If zero is chosen, no iteration will be performed and the "
	"GaussianMixture will be initialized with the initial guess.")
TAG (L"##Stability coefficient lambda")
DEFINITION (L"defines the fraction of the total covariance that will be added to the each of the mixture "
	"covariance matrices during the EM iteration. This may prevent one or more of these matrices to become singular.")
TAG (L"##Covariance matrices are")
DEFINITION (L"defines whether the complete covariance matrices in the mixture have to be calculated or only the diagonal.")
ENTRY (L"Expectation\\--Maximization Algorithm")
NORMAL (L"The Expectation\\--Maximization (EM) algorithm is an iterative procedure to maximize the likelihood of the data given a model. For a "
	"GaussianMixture, the parameters in the model are the centers and the covariances of all components in the mixture "
	"and their mixing probabilities.")
NORMAL (L"The number of parameters depends on the number of components in the mixture and the dimension of the data. "
	"For a full covariance matrix we have to find  %dimension%(%dimension%+1)/2 matrix elements and another "
	" %dimension vector elements for its center. This makes the total number of parameters that have to be estimated "
	"for a mixture with ##Number of components# components equal to "
	"%numberOfComponents \\.c %dimension%(%dimension%+3)/2 + %numberOfComponents.")
NORMAL (L"For diagonal covariance matrices the number of parameters reduces considerably.")
NORMAL (L"The EM iteration has to start with a sensible initial guess for all the parameters. For the initial guess, "
	"we derive our centers from positions on the 1-\\si ellipse in the plane spanned by the first two principal "
	"components. We then make all covariance matrices equal to a scaled down version of the total covariance matrix "
	"where the scaling factor depends on the number of components and the quotient of the between and within variance. "
	"Initialy all mixing probabilities will be chosen equal.")
NORMAL (L"How to proceed from the initial guess with the EM to find the optimal values for all the parameters "
	"in the Gaussian mixture is explained in great detail by  @@Bishop (2006)@.")
MAN_END

#define GaussianMixture_OPTION_MENU_CRITERIA \
	OPTIONMENU (L"Criterion based on", 1) \
	OPTION (L"Maximum likelihood") \
	OPTION (L"Minimum message length") \
	OPTION (L"Bayes information") \
	OPTION (L"Akaike information") \
	OPTION (L"Akaike corrected") \
	OPTION (L"Complete-data ML")

MAN_BEGIN (L"GaussianMixture & TableOfReal: Get likelihood value...", L"djmw", 20101125)
INTRO (L"Calculates how well the @GaussianMixture model fits the data according to a criterion.")
ENTRY (L"Settings")
TAG (L"##Maximum likelihood")
FORMULA (L"ML = \\Si__%i=1..%n_ log (\\Si__%m=1..%k_ \\al__%k_ %p__%%ik%_)")
TAG (L"##Minimum message length")
FORMULA (L"DL = ML - 0.5(N\\.c\\Si__%m=1..%k_ log(%n\\al__%m_/12) -%k\\.clog(%n/12) -%k(%N+1))")
TAG (L"##Bayes information")
FORMULA (L"BIC = 2\\.cML - k\\.cN\\.clog(n)")
TAG (L"##Akaike information")
FORMULA (L"AIC = 2(ML - k\\.cN) ")
TAG (L"##Akaike corrected")
FORMULA (L"AICc = 2(ML - k\\.cN\\.cn/(n-k\\.cN-1))")
TAG (L"##Complete-data ML")
FORMULA (L"\\Si__%i=1..%n_\\Si__%m=1..%k_ \\ga__%%im%_ log (\\ga__%%im%_)")
NORMAL (L"In the formulas above %n is the number of data points, %k is the number of mixture components, %N is the "
	"number of parameters in one component, i.e. %d + %d(%d+1)/2 for a full covariance matrix of "
	"dimension %d with means. The \\al__%k_ are the mixing probabilities, the %p__%%ik%_ are the probabilities for the %i-th data vector in the %k-th component. The \\ga__%%ik%_ are defined as ")
FORMULA (L"\\ga__%%im%_= \\al__%m_\\.c%p__%%im%_ /(\\Si__%j=1..%k_ \\al__%j_\\.c%p__%%ij%_).")
MAN_END

MAN_BEGIN (L"GaussianMixture & TableOfReal: Improve likelihood...", L"djmw", 20111130)
INTRO (L"Try to improve the likelihood of the parameters in the  @@GaussianMixture@ by an @@expectation-maximization@ algorithm.")
ENTRY (L"Settings & EM Algorithm")
NORMAL (L"As decribed in  @@TableOfReal: To GaussianMixture...@.")
MAN_END

MAN_BEGIN (L"GaussianMixture & TableOfReal: To Correlation (columns)", L"djmw", 20101111)
INTRO (L"Create a @Correlation matrix from the selected @TableOfReal and the @GaussianMixture.")
NORMAL (L"We start by calculating the ClassificationTable @@GaussianMixture & TableOfReal: To ClassificationTable|from "
	"the data and the GaussianMixture@.")
NORMAL (L"Nex we calculate correlations between the %%columns% of the ClassificationTable: cell [%i,%j] of the "
	"correlation matrix will then contain the value:")
FORMULA (L"%p[%i] \\.c %p[%j] / (||%p[%i]||\\.c ||%p[%j]||), ")
NORMAL (L"where %p[%i] is the data in the %i-th column of the classification table and ||%p[%i]|| is its Euclidean norm. "
	"The index %i runs from 1 to the number of components in the mixture.")
NORMAL (L"Because all the elements in a column are positive numbers, i.e. probabilities, all correlations will be positive numbers too.")
MAN_END

MAN_BEGIN (L"GaussianMixture & TableOfReal: To ClassificationTable", L"djmw", 20101122)
INTRO (L"Create a @ClassificationTable from the selected @TableOfReal and the @GaussianMixture.")
NORMAL (L"The classification table is a matrix with the same number of rows as the selected #TableOfReal "
	"object. The number of columns equals the number of components in the mixture. "
	"Cell [%i,%j] of the classification table contains the probability "
	"that the data in row %i of the TableOfReal belongs to component %j of the mixture.")
MAN_END

MAN_BEGIN (L"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)...", L"djmw", 20101120)
INTRO (L"Find the best @@GaussianMixture@ from the data according to a iterative component-wise optimization algorithm by which components may be deleted.")
ENTRY (L"Settings")
TAG (L"##Minimum number of components")
DEFINITION (L"defines the minimum number of components that have to survive the minimization process. If a value of zero is chosen all components will survive and no deletions will take place.")
TAG (L"##Tolerance of minimizer")
DEFINITION (L"defines when to stop optimizing. If the relative difference between the likelihoods at two successive "
	"iteration steps differs by less then the tolerance we stop, i.e. when |(%L(%i-1)-%L(%i))/%L(%i)| < %%tolerance%. ")
TAG (L"##Maximum number of iterations")
DEFINITION (L"defines another stop criterion. Iteration stops whenever the number of iterations reaches this value.")
TAG (L"##Stability coefficient lambda")
DEFINITION (L"defines the fraction of the totat covariance that is added to the covariance of each component to "
	"prevent these matrices from becoming singular.")
TAG (L"##Criterion based on")
DEFINITION (L"defines whether the function to be optimized is the log likelihood or the related miminum description length.")
ENTRY (L"Algorithm")
NORMAL (L"The component-wise optimization algorithm is described in @@Figueiredo & Jain (2002)@ where the function to be optimized "
	"is the minimum description length defined as:")
FORMULA (L"%L(\\te,%Y) = %N/2 \\Si__%m=1_^^%k^ ln(%n\\al__%k_/12) + %k/2 ln(%n/12) + %k(%N+1)/2 - ln %p(%Y|\\te),")
NORMAL (L"where %k is the number of components,  %N is the number of parameters of one component, "
	"i.e. %d+%d(%d+1)/2 for a full covariance matrix of dimension %d with means and %d+%d for a diagonal "
	"matrix with means;  %n is the number of data vectors. The term ln %p(%Y|\\te) is the log likelihood of the data "
	"given the model.")
NORMAL (L"For the optimization we either optimize the complete function %L(\\te,%Y) or only the likelihood ln %p(%Y|\\te) term.")
MAN_END

MAN_BEGIN (L"GaussianMixture & TableOfReal: To TableOfReal (BHEP normality tests)...", L"djmw", 20101113)
INTRO (L"Tests the data in the @TableOfReal that belong to the components of the @GaussianMixture for normality "
	"according to an adapted version of the @@BHEP multivariate normality test@.")
ENTRY (L"Setting")
TAG (L"##Beta")
DEFINITION (L"determines the smoothing parameter of the data. If %beta equals zero the smoothing is determined "
	"automatically for each component of the mixture separately as: ")
FORMULA (L"%beta = 1/(\\Vr2) (2%p+1)^^1/(%p+4)^ n^^1/(%p+4)^,")
NORMAL (L"where %n is the effective number of elements in the component and %p the dimension of the data.")
MAN_END

MAN_BEGIN (L"TableOfReal: To GaussianMixture (row labels)...", L"djmw", 20101101)
INTRO (L"Creates a  @@GaussianMixture@ from the selected  @TableOfReal. The number of mixture components is determined by the number of different row labels.")
ENTRY (L"Setting")
TAG (L"##Covariance matrices are")
DEFINITION (L"defines whether the complete covariance matrices in the mixture have to be calculated or only the diagonal.")
MAN_END

MAN_BEGIN (L"HMM_ObservationSequence", L"djmw", 20101010)
INTRO (L"An HMM_ObservationSequence models a sequence of observations. The observation sequence can be generated "
	"by the @HMM or it can be used to train a model.")
MAN_END

MAN_BEGIN (L"HMM_StateSequence", L"djmw", 20101010)
INTRO (L"An HMM_StateSequence models the sequence of states that an @HMM has traversed.")
MAN_END

MAN_BEGIN (L"HMM", L"djmw", 20130410)
INTRO (L"A HMM is a Hidden Markov Model. Markov models are often used to model observation sequences. "
	"The fundamental assumption in a markov model is that the probability of an observation (event) can only "
	"depend on the previous observation. "
	"A HMM can be visualised as a graph with a number of %%states%. If states are connected they have line connecting them. The following picture shows a HMM with two states, labeled \"Rainy\" and \"Sunny\". Each state can emit three symbols (these are not visible in the graph).   ")
SCRIPT (5, 5,
	L"do (\"Create simple HMM...\", \"wheather\", \"no\", \"Rainy Sunny\", \"Walk Shop Clean\")\n"
	"do (\"Draw...\", \"no\")\n"
	"do (\"Remove\")\n")
INTRO (L"For an introduction into HMM's see @@Rabiner (1989)@.")
MAN_END

MAN_BEGIN (L"HMM: Create simple HMM...", L"djmw", 20101009)
INTRO (L"Creates a @@HMM|Hidden Markov Model@ from given states and observation symbols.")
ENTRY (L"Settings")
TAG (L"##Name")
DEFINITION (L"determines the name (for the list of objects).")
TAG (L"##States")
DEFINITION (L"determines the number of and the names of the states.")
TAG (L"##Symbols")
DEFINITION (L"determines the number and the names of the observation symbols.")
NORMAL (L"You can define a (not hidden) Markov model by leaving either the States or the Symbols field empty.")
NORMAL (L"The model is initialised with equal probabilities for all the transitions and emissions.")
MAN_END

MAN_BEGIN (L"HMM: Extract transition probabilities", L"djmw", 20101020)
INTRO (L"Extract the transition probabilities of the selected @@HMM@ as a @@TableOfReal@.")
NORMAL (L"The row label shows the %%from% state and the column label the %%to% state, therefore the cell element "
	"in row  %i and column  %j shows the probability of making a transition from state  %i to state  %j.")
MAN_END

MAN_BEGIN (L"HMM: Extract emission probabilities", L"djmw", 20101020)
INTRO (L"Extract the emission probabilities of the selected @@HMM@ as a @@TableOfReal@.")
NORMAL (L"The row label shows the %%from% state and the column label the %%to% symbol, therefore the cell element "
	"in row  %i and column  %j shows the probability of emitting symbol  %j from state  %i.")
MAN_END

MAN_BEGIN (L"HMM & HMM: Get cross-entropy...", L"djmw", 20101017)
INTRO (L"Calculates the cross-entropy between the two selected @@HMM@ models based on observation sequences.")
ENTRY (L"Settings")
TAG (L"##Observation length")
DEFINITION (L"defines the number of observations that have to generated.")
TAG (L"##Symmetric")
DEFINITION (L"defines whether the symmetric formula is used in the calculation.")
ENTRY (L"Algorithm")
NORMAL (L"The cross-entropy is a measure of the distance between two models  \\la__1_ and \\la__2_. It is defined as")
FORMULA (L"%D(\\la__1_,\\la__2_) = 1/%N (log %p(%O__2_|\\la__1_) - log %p(%O__2_|\\la__2_)),")
NORMAL (L"where %O__2_ is an observation sequence of length  %N generated by model  \\la__2_.")
NORMAL (L"The symmetrized version is:")
FORMULA (L"%D__%s_(\\la__1_,\\la__2_) = (%D(\\la__1_,\\la__2_) + %D(\\la__2_,\\la__1_))/2.")
MAN_END

MAN_BEGIN (L"HMM & HMM_ObservationSequence: Get cross-entropy", L"djmw", 20101017)
INTRO (L"Calculates the cross-entropy between the selected @@HMM@ model and the @@HMM_ObservationSequence@.")
NORMAL (L"The cross-entropy is a useful upper bound for the entropy of a model. An approximation to the cross-entropy for a model on a observation sequence  %O of length %N is: ")
FORMULA (L"%H(%O) = -1/%N log %p(%O),")
NORMAL (L"where %p(%O) is the probability of the observation sequence given the model.")
MAN_END

MAN_BEGIN (L"HMM & HMM & HMM_ObservationSequence: Get cross-entropy", L"djmw", 20101017)
INTRO (L"Get the (symmetric) cross-entropy for the two selected @HMM models and an observation sequence.")
NORMAL (L"See @@HMM & HMM: Get cross-entropy...@.")
MAN_END

MAN_BEGIN (L"HMM: To HMM_ObservationSequence...", L"djmw", 20101010)
INTRO (L"Use the selected @HMM as a generator for an observation sequence.")
ENTRY (L"Settings")
TAG (L"##Start state")
DEFINITION (L"defines the state in which the HMM starts. If a zero is given the start state is randomly chosen according to the start probabilities.")
TAG (L"##Number of observations")
DEFINITION (L"defines the number of observations to generate. For models of %%finite% duration such as left-to-right models, the HMM generator may stop before this number is reached.")
MAN_END

MAN_BEGIN (L"HMM: Get transition probability...", L"djmw", 20101010)
INTRO (L"For the selected @HMM get the probability to make a transition from one state to the other.")
MAN_END

MAN_BEGIN (L"HMM: Get emission probability...", L"djmw", 20101010)
INTRO (L"For the selected @HMM get the probability that in a given state a particular symbol will be emitted.")
MAN_END

MAN_BEGIN (L"HMM: Get start probability...", L"djmw", 20101010)
INTRO (L"For the selected @HMM get the probabilities that the model will start in a particular state.")
MAN_END

MAN_BEGIN (L"HMM: Get p (time, state)...", L"djmw", 20101010)
INTRO (L"For the selected @HMM get the probability that after a given number of time steps the HMM will be in "
	"a particular state.")
MAN_END

MAN_BEGIN (L"HMM: Get p (time, state, symbol)...", L"djmw", 20101010)
INTRO (L"For the selected @HMM get the probability that after a given number of time steps the HMM will be in "
	"a particular state and emits a certain symbol.")
NORMAL (L"This probability is the product of the %%probability being in particular state at that time (index)% and the %%probability of emitting a certain symbol in that state%.")
MAN_END

MAN_BEGIN (L"HMM: Get probability staying in state...", L"djmw", 20101010)
INTRO (L"For the selected @HMM get the probability that it stays exactly the given number of times in that state.")
NORMAL (L"This probability is %\\al__%%ii%_^^numberOfTimes-1^(1 - %\\al__%%ii%_),\n"
	"where \\al__%%ii%_ is the probability of staying in state %i.")
MAN_END

MAN_BEGIN (L"HMM: Get expected duration in state...", L"djmw", 20101010)
INTRO (L"For the selected @HMM get the number of time units that the system is expected to stay in that state.")
MAN_END

MAN_BEGIN (L"HMM & HMM_StateSequence: Get probability", L"djmw", 20101010)
INTRO (L"Get the natural logarithm of the probability that the selected @@HMM_StateSequence|state sequence@ was generated by the selected @HMM.")
MAN_END

MAN_BEGIN (L"HMM & HMM_ObservationSequence: Get probability", L"djmw", 20101010)
INTRO (L"Get the natural logarithm of the probability that the selected @@HMM_ObservationSequence|state sequence@ was generated by the selected @HMM.")
ENTRY (L"Algorithm")
NORMAL (L"Viterbi")
MAN_END

MAN_BEGIN (L"HMM: Set transition probabilities...", L"djmw", 20101010)
INTRO (L"Sets the probabilities for making a transition from one state to all other states.")
ENTRY (L"Settings")
TAG (L"##Probabilities")
DEFINITION (L"the list of transition probabilities from the given state. "
	"The values given will be scaled as probabilities. A zero is used to mark a transition that will never occur. ")
ENTRY (L"Examples")
NORMAL (L"For a three state model the input \"1 3 7\" will result in the same probabilities as the input \"0.1 0.3 0.7\".")
MAN_END

MAN_BEGIN (L"HMM: Set emission probabilities...", L"djmw", 20101010)
INTRO (L"Sets the probabilities for emitting the symbols from the state.")
NORMAL (L"The values given will be scaled as probabilities. ")
ENTRY (L"Examples")
NORMAL (L"For an HMM with four symbols the input \"1 3 3 3\" will result in the same probabilities as the input \"0.1 0.3 0.3 0.3\".")
MAN_END

MAN_BEGIN (L"HMM: Set start probabilities...", L"djmw", 20101010)
INTRO (L"Sets the probabilities that the model starts in the particular states.")
NORMAL (L"The values given will be scaled as probabilities. ")
MAN_END

MAN_BEGIN (L"HMM & HMM_ObservationSequence: To TableOfReal (bigrams)...", L"djmw", 20101019)
INTRO (L"Get a table with bigrams and marginals form the selected @HMM_ObservationSequence and the @HMM.")
NORMAL (L"The entry at row %i and column %j shows how often the %j-th symbol follows the %i-th symbol in the observation sequence.")
ENTRY (L"Remark")
NORMAL (L"The row and colum marginals for the first and the last element in the obsevation sequence will not be equal because there is no transition to the first and no transition from the last one.")
MAN_END

MAN_BEGIN (L"HMM_ObservationSequence: To TableOfReal (bigrams)...", L"djmw", 20101019)
INTRO (L"Get a table with bigrams form the selected @HMM_ObservationSequence.")
NORMAL (L"See also @@HMM & HMM_ObservationSequence: To TableOfReal (bigrams)...@.")
MAN_END

MAN_BEGIN (L"HMM & HMM_ObservationSequences: Learn...", L"djmw", 20111130)
INTRO (L"Train the transition and emission probabilities of the @HMM from the observations.")
ENTRY (L"Algorithm")
NORMAL (L"The Baum-Welch @@expectation-maximization@ procedure. It uses the forward and backward procedures to (re)estimate the parameters until convergence is reached.")
MAN_END

MAN_BEGIN (L"Bishop (2006)", L"djmw", 20101026)
NORMAL (L"C.M. Bishop (2006): %%Pattern recognition and machine learning%. Springer.")
MAN_END

MAN_BEGIN (L"Figueiredo & Jain (2002)",L"djmw", 20101229)
NORMAL (L"Mario A.T. Figueiredo & Anil K. Jain (2002): \"Unsupervised learning of finite mixture models.\" "
	"%%IEEE Transactions on Pattern Analysis and Machine Intelligence% ##24(3)#: 381\\--396.")
MAN_END

MAN_BEGIN (L"Rabiner (1989)", L"djmw", 20101017)
NORMAL (L"L.R. Rabiner (1989): \"A tutorial on Hidden Markov Models and selected applications in speech recognition.\" %%Proceedings of the IEEE% #77: 257\\--286.")
MAN_END

MAN_BEGIN (L"Tenreiro (2009)", L"djmw", 20101113)
NORMAL (L"C. Tenreiro (2009): \"On the choice of the smoothing parameter for the BHEP goodness-of-fit test.\" "
	"%%Computational Statistics and Data Analysis% #53: 1038\\--1053.")
MAN_END

MAN_BEGIN (L"Zhang et al. (2003)", L"djmw", 20101122)
NORMAL (L"Zhihua Zhang & Chibiao Chen & Jian Sun & Kap Luk Chan (2003): \"EM algorithms for Gaussian mixtures with "
	"split-and-merge operation.\" %%Pattern Recognition% #36: 1973\\--1983.")
MAN_END

}

/* End of file manual_HMM.cpp */
