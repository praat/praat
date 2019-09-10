/* manual_HMM.cpp
 *
 * Copyright (C) 2011-2019 David Weenink
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
	djmw 20101009 Initial version
*/

#include "ManPagesM.h"

void manual_HMM (ManPages me);
void manual_HMM (ManPages me)
{

MAN_BEGIN (U"expectation-maximization", U"djmw", 20111130)
NORMAL (U"Expectation-maximization (EM) is an iterative method used to find maximum "
	"likelihood estimates of parameters in probabilistic models, where the model depends on "
	"unobserved, also called %%latent%, variables. EM alternates between performing an expectation (E) step, "
	"which computes an expectation of the likelihood by including the latent variables as if they "
	"were observed, and a maximization (M) step, which computes the maximum likelihood "
	"estimates of the parameters by maximizing the expected likelihood found in the E step. The "
	"parameters found on the M step are then used to start another E step, and the process is "
	"repeated until some criterion is satisfied. EM is frequently used for data clustering like for example in "
	"@@TableOfReal: To GaussianMixture...|Gaussian mixtures@ or in the "
	"@@HMM & HMMObservationSequences: Learn...|Baum-Welch training@ of a Hidden Markov Model.")
MAN_END

MAN_BEGIN (U"GaussianMixture", U"djmw", 20101026)
INTRO (U"A Gaussian mixture is a probability density function (p.d.f.). It is a combination of several Gaussian densities.")
NORMAL (U"The GaussianMixture's p.d.f. is defined as the weighted sum of %K multivariate Gaussian p.d.f's:")
FORMULA (U"pdf(x) = \\Si__%%i%=1_^^%K^ p__%i_ %N(%x;\\mu__%i_,\\Si__%i_),")
NORMAL (U"where each %N(%x;\\mu__%i_,\\Si__%i_) is a multivariate p.d.f. with mean \\mu__%i_ and covariance matrix \\Si__%i_. The coefficients %p__%i_ sum to 1.")
NORMAL (U"For an introduction to Gaussian mixtures see for example  @@Bishop (2006)@.")
MAN_END

MAN_BEGIN (U"GaussianMixture: Draw concentration ellipses...", U"djmw", 20101101)
INTRO (U"Draws the concentration ellipse for each component in the @@GaussianMixture@. ")
NORMAL (U"The principal component plane will be determined from @@GaussianMixture: To PCA@.")
NORMAL (U"You might also use another @PCA and to combine it with a GaussianMixture for drawing (@@GaussianMixture & PCA: Draw concentration ellipses...@).")
ENTRY (U"Settings")
TAG (U"##Number of sigmas")
DEFINITION (U"determines the @@concentration ellipse|data coverage@.")
TAG (U"##Principal component plane")
DEFINITION (U"determines whether the principal component plane is used or not for drawing.")
MAN_END

MAN_BEGIN (U"GaussianMixture: Get probability at position", U"djmw",20101103)
INTRO (U"Evaluate the pdf of the @@GaussianMixture@ at the given position.")
MAN_END

MAN_BEGIN (U"GaussianMixture & PCA: Draw concentration ellipses...", U"djmw", 20101101)
INTRO (U"Draws the concentration ellipse for each component in the @@GaussianMixture@ in the plane spanned by the selected @PCA.")
ENTRY (U"Settings")
TAG (U"##Number of sigmas")
DEFINITION (U"determines the @@concentration ellipse|data coverage@.")
MAN_END

MAN_BEGIN (U"GaussianMixture: Draw marginal pdf...", U"djmw", 20101122)
INTRO (U"A command to draw the marginal p.d.f. (probability density function) of the selected @GaussianMixture.")
NORMAL (U"A marginal distribution is the projection of the (multivariate) p.d.f. on one dimension or direction. "
	"This direction may also be externally defined by selecting a @PCA and a GaussianMixture together.")
MAN_END

MAN_BEGIN (U"GaussianMixture: Split component...", U"djmw", 20101122)
INTRO (U"Splits one component of the selected  @GaussianMixture into two components.")
NORMAL (U"The selected component is split on the basis of a @PCA analysis of its covariance matrix. The new means are situated "
	"around the old components mean, 1\\si apart in the first principal components direction, and the new covariances are "
	"constructed with information from the old covariance matrix. ")
NORMAL (U"The details of the algorith are described in @@Zhang et al. (2003)@.")
MAN_END

MAN_BEGIN (U"GaussianMixture: To PCA", U"djmw", 20101030)
INTRO (U"Creates a  @PCA from the selected  @GaussianMixture.")
NORMAL (U"The PCA is calculated from the @@GaussianMixture: To Covariance (total)|total covariance matrix@ of the GaussianMixture.")
MAN_END

MAN_BEGIN (U"GaussianMixture & PCA: To Matrix (density)...", U"djmw", 20101101)
INTRO (U"Represent the @@GaussianMixture@ p.d.f. on the plane spanned by @@PCA@. This makes it possible to draw "
	"the p.d.f. as grey values with one of the special @@Matrix@ image drawing methods.")
NORMAL (U"For each cell in the matrix of dimension  %%numberOfRows% \\xx %%numberOfColumns%, the p.d.f. will be evaluated.")
MAN_END

MAN_BEGIN (U"GaussianMixture: To Covariance (between)", U"djmw", 20101030)
INTRO (U"The covariance between the centers of the components of the @@GaussianMixture@ is calculated; "
	"each center is weighted according to its mixing probability.")
MAN_END

MAN_BEGIN (U"GaussianMixture: To Covariance (within)", U"djmw", 20101120)
INTRO (U"The covariances of the components of the @@GaussianMixture@ are pooled.")
MAN_END

MAN_BEGIN (U"GaussianMixture: To Covariance (total)", U"djmw", 20101030)
INTRO (U"The sum of the @@GaussianMixture: To Covariance (within)|within@ and @@GaussianMixture: To Covariance (between)|between@ covariances of the @@GaussianMixture@ is calculated.")
MAN_END

MAN_BEGIN (U"GaussianMixture: To TableOfReal (random sampling)...", U"djmw", 20101030)
INTRO (U"The selected @@GaussianMixture@ is used as a generator of data.")
ENTRY (U"Setting")
TAG (U"##Number of data points")
DEFINITION (U"determines how many random data point have to be generated.")
ENTRY (U"Algorithm")
NORMAL (U"For each data point to be generated:")
NORMAL (U"1. A random number decides to which component in the mixture the data point will belong.")
NORMAL (U"2. According to the procedure described in @@Covariance: To TableOfReal (random sampling)...@, one "
	"data point will be generated.")
MAN_END

MAN_BEGIN (U"TableOfReal: To GaussianMixture...", U"djmw", 20150930)
INTRO (U"Creates a @@GaussianMixture@ from the selected @@TableOfReal@ by an @@expectation-maximization|"
	"expectation-maximization@ procedure.")
ENTRY (U"Settings")
TAG (U"##Number of components")
DEFINITION (U"defines the number of Gaussians in the mixture.")
TAG (U"##Tolerance of minimizer")
DEFINITION (U"defines when to stop optimizing. If the relative difference between the likelihoods at two successive "
	"iteration steps differs by less then the tolerance we stop, i.e. when |(%L(%i-1)-%L(%i))/%L(%i)| < %%tolerance%. ")
TAG (U"##Maximum number of iterations")
DEFINITION (U"defines another stopping criterion. The EM iteration will stop when either the tolerance "
	"is reached or the maximum number of iterations. If zero is chosen, no iteration will be performed and the "
	"GaussianMixture will be initialized with the initial guess.")
TAG (U"##Stability coefficient lambda")
DEFINITION (U"defines the fraction of the total covariance that will be added to the each of the mixture "
	"covariance matrices during the EM iteration. This may prevent one or more of these matrices to become singular.")
TAG (U"##Covariance matrices are")
DEFINITION (U"defines whether the complete covariance matrices in the mixture have to be calculated or only the diagonal.")
TAG (U"##Criterion based on")
DEFINITION (U"defines how the @@GaussianMixture & TableOfReal: Get likelihood value...|likelihood of the data given the model is calculated@.")
ENTRY (U"Expectation\\--Maximization Algorithm")
NORMAL (U"The Expectation\\--Maximization (EM) algorithm is an iterative procedure to maximize the likelihood of the data given a model. For a "
	"GaussianMixture, the parameters in the model are the centers and the covariances of all components in the mixture "
	"and their mixing probabilities.")
NORMAL (U"The number of parameters depends on the number of components in the mixture and the dimension of the data. "
	"For a full covariance matrix we have to find %dimension%(%dimension%+1)/2 matrix elements and another "
	"%dimension vector elements for its center. This makes the total number of parameters that have to be estimated "
	"for a mixture with ##Number of components# components equal to "
	"%numberOfComponents \\.c %dimension%(%dimension%+3)/2 + %numberOfComponents.")
NORMAL (U"For diagonal covariance matrices the number of parameters reduces considerably.")
NORMAL (U"The EM iteration has to start with a sensible initial guess for all the parameters. For the initial guess, "
	"we derive our centers from positions on the 1-\\si ellipse in the plane spanned by the first two principal "
	"components. We then make all covariance matrices equal to a scaled-down version of the total covariance matrix, "
	"where the scaling factor depends on the number of components and the quotient of the between and within variance. "
	"Initially all mixing probabilities will be chosen equal.")
NORMAL (U"How to proceed from the initial guess with the EM to find the optimal values for all the parameters "
	"in the Gaussian mixture is explained in great detail by  @@Bishop (2006)@.")
MAN_END

MAN_BEGIN (U"GaussianMixture & TableOfReal: Get likelihood value...", U"djmw", 20190711)
INTRO (U"Calculates how well the @GaussianMixture model fits the data according to a criterion.")
ENTRY (U"Settings")
TAG (U"##Maximum likelihood")
FORMULA (U"ML = \\Si__%i=1..%n_ log (\\Si__%m=1..%k_ \\al__%k_ %p__%%ik%_)")
TAG (U"##Minimum message length")
FORMULA (U"DL = ML - 0.5(N\\.c\\Si__%m=1..%k_ log(%n\\al__%m_/12) -%k\\.clog(%n/12) -%k(%N+1))")
TAG (U"##Bayes information")
FORMULA (U"BIC = 2\\.cML - k\\.cN\\.clog(n)")
TAG (U"##Akaike information")
FORMULA (U"AIC = 2(ML - k\\.cN) ")
TAG (U"##Akaike corrected")
FORMULA (U"AICc = 2(ML - k\\.cN\\.cn/(n-k\\.cN-1))")
TAG (U"##Complete-data ML")
FORMULA (U"\\Si__%i=1..%n_\\Si__%m=1..%k_ \\ga__%%im%_ log (\\ga__%%im%_)")
NORMAL (U"In these formulas, %n is the number of data points, %k is the number of mixture components, %N is the "
	"number of parameters in one component (i.e. %d + %d(%d+1)/2 for a full covariance matrix of "
	"dimension %d with means), \\al__%k_ are the %%mixing probabilities%, "
	"and %p__%%ik%_ are the %%multinormal probabilities% for the %i-th data vector in the %k-th component. "
	"The %%responsibilities% \\ga__%%im%_ are defined as ")
FORMULA (U"\\ga__%%im%_= \\al__%m_\\.c%p__%%im%_ /(\\Si__%j=1..%k_ \\al__%j_\\.c%p__%%ij%_).")
MAN_END

MAN_BEGIN (U"GaussianMixture & TableOfReal: Improve likelihood...", U"djmw", 20111130)
INTRO (U"Try to improve the likelihood of the parameters in the @@GaussianMixture@ by an @@expectation-maximization@ algorithm.")
ENTRY (U"Settings & EM Algorithm")
NORMAL (U"As decribed in @@TableOfReal: To GaussianMixture...@.")
MAN_END

MAN_BEGIN (U"GaussianMixture & TableOfReal: To Correlation (columns)", U"djmw", 20101111)
INTRO (U"Create a @Correlation matrix from the selected @TableOfReal and the @GaussianMixture.")
NORMAL (U"We start by calculating the ClassificationTable @@GaussianMixture & TableOfReal: To ClassificationTable|from "
	"the data and the GaussianMixture@.")
NORMAL (U"Nex we calculate correlations between the %%columns% of the ClassificationTable: cell [%i,%j] of the "
	"correlation matrix will then contain the value:")
FORMULA (U"%p[%i] \\.c %p[%j] / (||%p[%i]||\\.c ||%p[%j]||), ")
NORMAL (U"where %p[%i] is the data in the %i-th column of the classification table and ||%p[%i]|| is its Euclidean norm. "
	"The index %i runs from 1 to the number of components in the mixture.")
NORMAL (U"Because all the elements in a column are positive numbers, i.e. probabilities, all correlations will be positive numbers too.")
MAN_END

MAN_BEGIN (U"GaussianMixture & TableOfReal: To ClassificationTable", U"djmw", 20101122)
INTRO (U"Create a @ClassificationTable from the selected @TableOfReal and the @GaussianMixture.")
NORMAL (U"The classification table is a matrix with the same number of rows as the selected #TableOfReal "
	"object. The number of columns equals the number of components in the mixture. "
	"Cell [%i,%j] of the classification table contains the probability "
	"that the data in row %i of the TableOfReal belongs to component %j of the mixture.")
MAN_END

MAN_BEGIN (U"GaussianMixture & TableOfReal: To GaussianMixture (CEMM)...", U"djmw", 20190709)
INTRO (U"Find the best @@GaussianMixture@ from the data according to an iterative Component-wise @@expectation-maximization|Expectation-Maximization@ for Mixtures algorithm by which components may be deleted.")
ENTRY (U"Settings")
TAG (U"##Minimum number of components")
DEFINITION (U"defines the minimum number of components that have to survive the minimization process. If a value of zero is chosen, all components will survive and no deletions will take place.")
TAG (U"##Tolerance of minimizer")
DEFINITION (U"defines when to stop optimizing. If the relative difference between the likelihoods at two successive "
	"iteration steps differs by less then the tolerance we stop, i.e. when |(%L(%i-1)-%L(%i))/%L(%i)| < %%tolerance%. ")
TAG (U"##Maximum number of iterations")
DEFINITION (U"defines another stop criterion. Iteration stops whenever the number of iterations reaches this value.")
TAG (U"##Stability coefficient lambda")
DEFINITION (U"defines the fraction of the total covariance that is added to the covariance of each component to "
	"prevent these matrices from becoming singular.")
TAG (U"##Criterion based on")
DEFINITION (U"defines whether the function to be optimized is the log likelihood or the related miminum description length.")
ENTRY (U"Algorithm")
NORMAL (U"The component-wise optimization algorithm is described in @@Figueiredo & Jain (2002)@ where the function to be optimized "
	"is the minimum description length defined as:")
FORMULA (U"%L(\\te,%Y) = %N/2 \\Si__%m=1_^^%k^ ln(%n\\al__%k_/12) + %k/2 ln(%n/12) + %k(%N+1)/2 - ln %p(%Y|\\te),")
NORMAL (U"where %k is the number of components, %N is the number of parameters of one component "
	"(i.e. %d+%d(%d+1)/2 for a full covariance matrix of dimension %d with means and %d+%d for a diagonal "
	"matrix with means), and %n is the number of data vectors. The term ln %p(%Y|\\te) is the log likelihood of the data "
	"given the model.")
NORMAL (U"For the optimization we either optimize the complete function %L(\\te,%Y) or only the likelihood ln %p(%Y|\\te) term.")
MAN_END

MAN_BEGIN (U"GaussianMixture & TableOfReal: To Table (BHEP normality tests)...", U"djmw", 20190728)
INTRO (U"Tests the data in the @TableOfReal that belong to the components of the @GaussianMixture for normality "
	"according to an adapted version of the @@BHEP multivariate normality test@.")
ENTRY (U"Setting")
TAG (U"##Beta")
DEFINITION (U"determines the smoothing parameter of the data. If %beta equals zero the smoothing is determined "
	"automatically for each component of the mixture separately as: ")
FORMULA (U"%beta = 1/(\\Vr2) (2%p+1)^^1/(%p+4)^ %n^^1/(%p+4)^,")
NORMAL (U"where %n is the effective number of elements in the component, and %p is the dimension of the data.")
MAN_END

MAN_BEGIN (U"TableOfReal: To GaussianMixture (row labels)...", U"djmw", 20101101)
INTRO (U"Creates a @@GaussianMixture@ from the selected @TableOfReal. The number of mixture components is determined by the number of different row labels.")
ENTRY (U"Setting")
TAG (U"##Covariance matrices are")
DEFINITION (U"defines whether the complete covariance matrices in the mixture have to be calculated or only the diagonal.")
MAN_END

MAN_BEGIN (U"HMMObservationSequence", U"djmw", 20140117)
INTRO (U"An HMMObservationSequence models a sequence of observations. The observation sequence can be generated "
	"by the @HMM or it can be used to train a model.")
MAN_END

MAN_BEGIN (U"HMMStateSequence", U"djmw", 20101010)
INTRO (U"An HMMStateSequence models the sequence of states that an @HMM has traversed.")
MAN_END

MAN_BEGIN (U"HMM", U"djmw", 20130410)
INTRO (U"A HMM is a Hidden Markov Model. Markov models are often used to model observation sequences. "
	"The fundamental assumption in a Markov model is that the probability of an observation (event) can only "
	"depend on the previous observation. "
	"A HMM can be visualised as a graph with a number of %%states%. If two states are connected, the graph has a line that connects them. "
	"The following picture shows a HMM with two states, labeled \"Rainy\" and \"Sunny\". "
	"Each state can emit three symbols (these are not visible in the graph).")
SCRIPT (5, 5,
	U"Create simple HMM: \"weather\", \"no\", \"Rainy Sunny\", \"Walk Shop Clean\"\n"
	"Draw: \"no\"\n"
	"Remove\n")
INTRO (U"For an introduction into HMM's see @@Rabiner (1989)@.")
MAN_END

MAN_BEGIN (U"HMM: Create simple HMM...", U"djmw", 20101009)
INTRO (U"Creates a @@HMM|Hidden Markov Model@ from given states and observation symbols.")
ENTRY (U"Settings")
TAG (U"##Name")
DEFINITION (U"determines the name (for the list of objects).")
TAG (U"##States")
DEFINITION (U"determines the number of and the names of the states.")
TAG (U"##Symbols")
DEFINITION (U"determines the number and the names of the observation symbols.")
NORMAL (U"You can define a (not hidden) Markov model by leaving either the States or the Symbols field empty.")
NORMAL (U"The model is initialised with equal probabilities for all the transitions and emissions.")
MAN_END

MAN_BEGIN (U"HMM: Extract transition probabilities", U"djmw", 20101020)
INTRO (U"Extract the transition probabilities of the selected @@HMM@ as a @@TableOfReal@.")
NORMAL (U"The row label shows the %%from% state and the column label the %%to% state, therefore the cell element "
	"in row  %i and column  %j shows the probability of making a transition from state  %i to state  %j.")
MAN_END

MAN_BEGIN (U"HMM: Extract emission probabilities", U"djmw", 20101020)
INTRO (U"Extract the emission probabilities of the selected @@HMM@ as a @@TableOfReal@.")
NORMAL (U"The row label shows the %%from% state and the column label the %%to% symbol, therefore the cell element "
	"in row  %i and column  %j shows the probability of emitting symbol  %j from state  %i.")
MAN_END

MAN_BEGIN (U"HMM & HMM: Get cross-entropy...", U"djmw", 20101017)
INTRO (U"Calculates the cross-entropy between the two selected @@HMM@ models based on observation sequences.")
ENTRY (U"Settings")
TAG (U"##Observation length")
DEFINITION (U"defines the number of observations that have to generated.")
TAG (U"##Symmetric")
DEFINITION (U"defines whether the symmetric formula is used in the calculation.")
ENTRY (U"Algorithm")
NORMAL (U"The cross-entropy is a measure of the distance between two models  \\la__1_ and \\la__2_. It is defined as")
FORMULA (U"%D(\\la__1_,\\la__2_) = 1/%N (log %p(%O__2_|\\la__1_) - log %p(%O__2_|\\la__2_)),")
NORMAL (U"where %O__2_ is an observation sequence of length  %N generated by model  \\la__2_.")
NORMAL (U"The symmetrized version is:")
FORMULA (U"%D__%s_(\\la__1_,\\la__2_) = (%D(\\la__1_,\\la__2_) + %D(\\la__2_,\\la__1_))/2.")
MAN_END

MAN_BEGIN (U"HMM & HMMObservationSequence: Get cross-entropy", U"djmw", 20101017)
INTRO (U"Calculates the cross-entropy between the selected @@HMM@ model and the @@HMMObservationSequence@.")
NORMAL (U"The cross-entropy is a useful upper bound for the entropy of a model. An approximation to the cross-entropy for a model on a observation sequence  %O of length %N is: ")
FORMULA (U"%H(%O) = -1/%N log %p(%O),")
NORMAL (U"where %p(%O) is the probability of the observation sequence given the model.")
MAN_END

MAN_BEGIN (U"HMM & HMM & HMMObservationSequence: Get cross-entropy", U"djmw", 20101017)
INTRO (U"Get the (symmetric) cross-entropy for the two selected @HMM models and an observation sequence.")
NORMAL (U"See @@HMM & HMM: Get cross-entropy...@.")
MAN_END

MAN_BEGIN (U"HMM: To HMMObservationSequence...", U"djmw", 20101010)
INTRO (U"Use the selected @HMM as a generator for an observation sequence.")
ENTRY (U"Settings")
TAG (U"##Start state")
DEFINITION (U"defines the state in which the HMM starts. If a zero is given the start state is randomly chosen according to the start probabilities.")
TAG (U"##Number of observations")
DEFINITION (U"defines the number of observations to generate. For models of %%finite% duration such as left-to-right models, the HMM generator may stop before this number is reached.")
MAN_END

MAN_BEGIN (U"HMM: Get transition probability...", U"djmw", 20101010)
INTRO (U"For the selected @HMM get the probability to make a transition from one state to the other.")
MAN_END

MAN_BEGIN (U"HMM: Get emission probability...", U"djmw", 20101010)
INTRO (U"For the selected @HMM get the probability that in a given state a particular symbol will be emitted.")
MAN_END

MAN_BEGIN (U"HMM: Get start probability...", U"djmw", 20101010)
INTRO (U"For the selected @HMM get the probabilities that the model will start in a particular state.")
MAN_END

MAN_BEGIN (U"HMM: Get p (time, state)...", U"djmw", 20101010)
INTRO (U"For the selected @HMM get the probability that after a given number of time steps the HMM will be in "
	"a particular state.")
MAN_END

MAN_BEGIN (U"HMM: Get p (time, state, symbol)...", U"djmw", 20101010)
INTRO (U"For the selected @HMM get the probability that after a given number of time steps the HMM will be in "
	"a particular state and emits a certain symbol.")
NORMAL (U"This probability is the product of the %%probability being in particular state at that time (index)% and the %%probability of emitting a certain symbol in that state%.")
MAN_END

MAN_BEGIN (U"HMM: Get probability staying in state...", U"djmw", 20101010)
INTRO (U"For the selected @HMM get the probability that it stays exactly the given number of times in that state.")
NORMAL (U"This probability is %\\al__%%ii%_^^numberOfTimes-1^(1 - %\\al__%%ii%_),\n"
	"where \\al__%%ii%_ is the probability of staying in state %i.")
MAN_END

MAN_BEGIN (U"HMM: Get expected duration in state...", U"djmw", 20101010)
INTRO (U"For the selected @HMM get the number of time units that the system is expected to stay in that state.")
MAN_END

MAN_BEGIN (U"HMM & HMMStateSequence: Get probability", U"djmw", 20101010)
INTRO (U"Get the natural logarithm of the probability that the selected @@HMMStateSequence|state sequence@ was generated by the selected @HMM.")
MAN_END

MAN_BEGIN (U"HMM & HMMObservationSequence: Get probability", U"djmw", 20101010)
INTRO (U"Get the natural logarithm of the probability that the selected @@HMMObservationSequence|state sequence@ was generated by the selected @HMM.")
ENTRY (U"Algorithm")
NORMAL (U"Viterbi")
MAN_END

MAN_BEGIN (U"HMM: Set transition probabilities...", U"djmw", 20101010)
INTRO (U"Sets the probabilities for making a transition from one state to all other states.")
ENTRY (U"Settings")
TAG (U"##Probabilities")
DEFINITION (U"the list of transition probabilities from the given state. "
	"The values given will be scaled as probabilities. A zero is used to mark a transition that will never occur. ")
ENTRY (U"Examples")
NORMAL (U"For a three state model the input \"1 3 7\" will result in the same probabilities as the input \"0.1 0.3 0.7\".")
MAN_END

MAN_BEGIN (U"HMM: Set emission probabilities...", U"djmw", 20101010)
INTRO (U"Sets the probabilities for emitting the symbols from the state.")
NORMAL (U"The values given will be scaled as probabilities. ")
ENTRY (U"Examples")
NORMAL (U"For an HMM with four symbols the input \"1 3 3 3\" will result in the same probabilities as the input \"0.1 0.3 0.3 0.3\".")
MAN_END

MAN_BEGIN (U"HMM: Set start probabilities...", U"djmw", 20101010)
INTRO (U"Sets the probabilities that the model starts in the particular states.")
NORMAL (U"The values given will be scaled as probabilities. ")
MAN_END

MAN_BEGIN (U"HMM & HMMObservationSequence: To TableOfReal (bigrams)...", U"djmw", 20101019)
INTRO (U"Get a table with bigrams and marginals form the selected @HMMObservationSequence and the @HMM.")
NORMAL (U"The entry at row %i and column %j shows how often the %j-th symbol follows the %i-th symbol in the observation sequence.")
ENTRY (U"Remark")
NORMAL (U"The row and colum marginals for the first and the last element in the obsevation sequence will not be equal because there is no transition to the first and no transition from the last one.")
MAN_END

MAN_BEGIN (U"HMMObservationSequence: To TableOfReal (bigrams)...", U"djmw", 20101019)
INTRO (U"Get a table with bigrams form the selected @HMMObservationSequence.")
NORMAL (U"See also @@HMM & HMMObservationSequence: To TableOfReal (bigrams)...@.")
MAN_END

MAN_BEGIN (U"HMM & HMMObservationSequences: Learn...", U"djmw", 20111130)
INTRO (U"Train the transition and emission probabilities of the @HMM from the observations.")
ENTRY (U"Algorithm")
NORMAL (U"The Baum-Welch @@expectation-maximization@ procedure. It uses the forward and backward procedures to (re)estimate the parameters until convergence is reached.")
MAN_END

MAN_BEGIN (U"Bishop (2006)", U"djmw", 20101026)
NORMAL (U"C.M. Bishop (2006): %%Pattern recognition and machine learning%. Springer.")
MAN_END

MAN_BEGIN (U"Figueiredo & Jain (2002)", U"djmw", 20101229)
NORMAL (U"Mario A.T. Figueiredo & Anil K. Jain (2002): \"Unsupervised learning of finite mixture models.\" "
	"%%IEEE Transactions on Pattern Analysis and Machine Intelligence% ##24(3)#: 381\\--396.")
MAN_END

MAN_BEGIN (U"Rabiner (1989)", U"djmw", 20101017)
NORMAL (U"L.R. Rabiner (1989): \"A tutorial on Hidden Markov Models and selected applications in speech recognition.\" %%Proceedings of the IEEE% #77: 257\\--286.")
MAN_END

MAN_BEGIN (U"Tenreiro (2009)", U"djmw", 20101113)
NORMAL (U"C. Tenreiro (2009): \"On the choice of the smoothing parameter for the BHEP goodness-of-fit test.\" "
	"%%Computational Statistics and Data Analysis% #53: 1038\\--1053.")
MAN_END

MAN_BEGIN (U"Zhang et al. (2003)", U"djmw", 20101122)
NORMAL (U"Zhihua Zhang & Chibiao Chen & Jian Sun & Kap Luk Chan (2003): \"EM algorithms for Gaussian mixtures with "
	"split-and-merge operation.\" %%Pattern Recognition% #36: 1973\\--1983.")
MAN_END

}

/* End of file manual_HMM.cpp */
