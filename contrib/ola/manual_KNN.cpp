/* manual_KNN.c
 *
 * Copyright (C) 2007-2008 Ola Söder
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
 * os 2008/05/29 Initial release
 * os 2009/01/23 Removed irrelevant information
 * pb 2010/12/28 lay-out, spelling
 */

#include "ManPagesM.h"
#include "KNN.h"

void manual_KNN_init (ManPages me);

void manual_KNN_init (ManPages me)
{
MAN_BEGIN (U"kNN classifiers", U"Ola Söder", 20080529)
INTRO (U"This tutorial describes the use of kNN classifiers in Praat.")
NORMAL (U"@@kNN classifiers 1. What is a kNN classifier?|1. What is a kNN classifier?@")
NORMAL (U"@@kNN classifiers 1.1. Improving classification accuracy|1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting|1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1.1. Filter-based feature weighting|1.1.1.1. Filter-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1.2. Wrapper-based feature weighting|1.1.1.2. Wrapper-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.2. Model selection|1.1.2. Model selection@")
NORMAL (U"@@kNN classifiers 1.2. Improving resource efficiency|1.2. Improving resource efficiency@")
LIST_ITEM (U"@@kNN classifiers 1.2.1. Pruning|1.2.1. Pruning@")
NORMAL (U"@@kNN classifiers 2. Quick start|2. Quick start@")
NORMAL (U"@@kNN classifiers 3. Command overview|3. Command overview@")
MAN_END

MAN_BEGIN (U"KNN", U"Ola Söder", 20080529)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A #KNN classifier can %learn associations between its %input and its %output. "
	"The @@kNN classifiers@ tutorial gives you an introduction to the usage of %%k%NN learners in Praat.")
MAN_END

MAN_BEGIN (U"FeatureWeights", U"Ola Söder", 20080729)
INTRO (U"One of the @@types of objects@ in Praat.")
NORMAL (U"A @FeatureWeights object is a %d-dimensional vector containing weight values used to transform a %d-dimensional space. Feature weighting can be used to improve the classifcation accuracy of @KNN classifiers. It can also be used to generate a @Dissimilarity matrix from a @PatternList object. @Dissimilarity matrices in conjunction with @@Multidimensional scaling|MDS-analysis@ can aid the visualization of high-dimensional data.")
MAN_END

MAN_BEGIN (U"kNN classifiers 1. What is a kNN classifier?", U"Ola Söder", 20080529)
NORMAL (U"%%Instance-based% classifiers such as the %%k%NN classifier operate on the premises that classification of unknown instances "
	"can be done by relating the unknown to the known according to some @@Euclidean distance|distance/similarity function@. "
	"The intuition is that two instances far apart in the %%instance space% defined by the appropriate %%distance function% are less likely "
	"than two closely situated instances to belong to the same class.")
ENTRY (U"The learning process")
NORMAL (U"Unlike many artificial learners, %%instance-based% learners do not abstract any information from the training data during the learning phase. "
	"Learning is merely a question of encapsulating the training data. The process of %generalization is postponed until it is absolutely unavoidable, "
	"that is, at the time of %classification. This property has lead to the referring to %%instance-based% learners as %lazy learners, "
	"whereas classifiers such as @@Feedforward neural networks 1. What is a feedforward neural network?|feedforward neural networks@, "
	"where proper %abstraction is done during the learning phase, often are entitled %eager learners.")
ENTRY (U"Classification")
NORMAL (U"Classification (%generalization) using an %%instance-based% classifier can be a simple matter of locating the nearest neighbour in %%instance space% "
	"and labelling the unknown instance with the same class label as that of the located (known) neighbour. "
	"This approach is often referred to as a %%nearest neighbour classifier%. "
	"The downside of this simple approach is the lack of robustness that characterize the resulting classifiers. "
	"The high degree of local sensitivity makes %%nearest neighbour classifiers% highly susceptible to noise in the training data.")
NORMAL (U"More robust models can be achieved by locating %k, where %k > 1, neighbours and letting the majority vote decide the outcome of the class labelling. "
	"A higher value of %k results in a smoother, less locally sensitive, function. "
	"The %%nearest neighbour classifier% can be regarded as a special case of the more general %%k-nearest neighbours classifier%, "
	"hereafter referred to as a %%k%NN classifier. The drawback of increasing the value of %k is of course that as %k approaches %n, "
	"where %n is the size of the %%instance base%, the performance of the classifier will approach that of the most straightforward %%statistical baseline%, "
	"the assumption that all unknown instances belong to the class most most frequently represented in the training data.")
NORMAL (U"This problem can be avoided by limiting the influence of distant instances. "
	"One way of doing so is to assign a weight to each vote, where the weight is a function of the distance between the unknown and the known instance. "
	"By letting each weight be defined by the inversed squared distance between the known and unknown instances votes cast by distant instances "
	"will have very little influence on the decision process compared to instances in the near neighbourhood. "
	"%%Distance weighted voting% usually serves as a good middle ground as far as local sensitivity is concerned.")
MAN_END

MAN_BEGIN (U"kNN classifiers 1.1. Improving classification accuracy", U"Ola Söder", 20080529)
NORMAL (U"This tutorial describes possible ways of improving the classification accuracy of a %%k%NN classifier.")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting|1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.2. Model selection|1.1.2. Model selection@")
MAN_END

MAN_BEGIN (U"kNN classifiers 1.1.1. Feature weighting", U"Ola Söder", 20080529)
NORMAL (U"A %%k%NN classifier in its most basic form operates under the implicit assumption that all features are of equal value as far as the classification problem at hand is concerned. When irrelevant and noisy features influence the neighbourhood search to the same degree as highly relevant features, the accuracy of the model is likely to deteriorate. %%Feature weighting% is a technique used to approximate the optimal degree of influence of individual features using a training set. When successfully applied relevant features are attributed a high weight value, whereas irrelevant features are given a weight value close to zero. %%Feature weighting% can be used not only to improve classification accuracy but also to discard features with weights below a certain threshold value and thereby increase the resource efficiency of the classifier.")
NORMAL (U"Two fundamentally different approaches to this optimization problem can be identified, the %%filter-based% and the %%wrapper-based%. The class of %%filter-based% methods contains algorithms that use no input other than the training data itself to calculate the %%feature weights%, whereas %%wrapper-based% algorithms use feedback from a classifier to guide the search. %%Wrapper-based% algorithms are inherently more powerful than their filter-based counterpart as they implicitly take the %%inductive bias% of the classifier into account. This power comes at a price however; the usage of %%wrapper-based% algorithms increases the risk of %%overfitting% the training data.")
NORMAL (U"In @@kNN classifiers 1.1.1.1. Filter-based feature weighting|section 1.1.1.1.@ the %%filter-based% feature weighting algorithm implemented in Praat is presented. @@kNN classifiers 1.1.1.2. Wrapper-based feature weighting|Section 1.1.1.2.@ contains an account of the implemented %%wrapper-based% feature weighting algorithm.")
MAN_END

MAN_BEGIN (U"kNN classifiers 1.1.1.1. Filter-based feature weighting", U"Ola Söder", 20080529)
NORMAL (U"The %%filter-based% %%feature weighting% algorithm implemented in Praat is an extended version of the well known R\\s{ELIEF} algorithm, R\\s{ELIEF-F}, such as it is presented (with one minor exception, see below) in @@Igor Kononenko (1994)|Kononenko (1994)@. Unlike the original R\\s{ELIEF} algorithm, the R\\s{ELIEF-F} algorithm copes with multi class (as in more than 2 classes) data sets. The simple intuition behind the R\\s{ELIEF-F} algorithm is that a good feature is a feature with little within class variance and generous amounts of between-class variance. A bad feature is characterized by within-class and between-class variances of magnitudes roughly equal.")
NORMAL (U"The computation of the weight vector is done in an iterative fashion, with all weights initially set to 0. All features in the training set are normalized (all values are set within the range [0 ... 1]) and thereafter used to update the weight vector as follows: On each iteration a random instance is chosen. The nearest hit is located, where hit is an instance of the same class as that of the randomly chosen instance. The nearest misses of all the classes but that of the randomly chosen instance are located, where a miss is an instance of a class different from that of the randomly chosen instance. Each weight is updated by subtracting the difference between the given attribute of the randomly chosen instance and that of the nearest hit and adding the corresponding difference between the chosen instance and all the nearest misses weighted by the prior probabilities of their classes. If the distance between the attribute of the randomly chosen instance and the nearest hit equals the corresponding value for the nearest miss(es) then the weight value will not change, it will thus remain 0 given that the current iteration is the first one. If the difference between the attribute of the chosen instance and the nearest hit is lower than the corresponding value for the miss(es) then the weight value will be increased. On average highly significant attributes will result in absolute values distinct from 0, leading to an absolute increase of the weight, and insignificant attributes will on average result in values near 0, retaining the pessimistic view that all features are of no value as predictors.")
NORMAL (U"The implementation of R\\s{ELIEF-F} found in Praat differs slightly from the algorithm described. Instances are not randomly chosen, instead all instances are used to update the weight vector. This simplification is of no concern unless massive data sets are used, in which case the Praat approach would be no less precise, but needlessly slow.")
MAN_END

MAN_BEGIN (U"kNN classifiers 1.1.1.2. Wrapper-based feature weighting", U"Ola Söder", 20090123)
NORMAL (U"The %%wrapper-based% %%feature weighting% algorithm implemented in Praat attempts to find the globally optimal feature weights by means of a greedy local search. The local neighbourhood is defined by a number of random seeds centered around a pivot seed. For each iteration of the algorithm the best performing seed is chosen to be the pivot of the next iteration. At the same time the maximum allowed distance between a seed and the pivot is decreased by a constant factor. Given a sufficient number of seeds and a small shrinkage factor the algorithm will on average home in on a good approximation of the global maximum.")
NORMAL (U"Given reasonable parameter settings, the search algorithm on average gives good results without huge memory and CPU requirements. The strong point of the method, the ability to efficiently do a rather thorough search of the weight space, can, as mentioned in @@kNN classifiers 1.1.1. Feature weighting|section 1.1.1.@, lead to overfitting. One way of minimizing this problem is to constrain the algorithm by means of %%early stopping%. In Praat this is done in a very naive fashion, by simply not allowing the algorithm to explore weights achieving an accuracy higher than a specified threshold value.")
MAN_END

MAN_BEGIN (U"kNN classifiers 1.1.2. Model selection", U"Ola Söder", 20080529)
NORMAL (U"%%Model selection% is the process of choosing classifier parameters suitable for the classification task at hand. In most cases this is done manually in an experimental fashion.")
NORMAL (U"The search for the optimal model can also be automated. In Praat this is done by means of the same @@kNN classifiers 1.1.1.2. Wrapper-based feature weighting|greedy local search algorithm@ used to search the weight space for feature weights. The %%model selection% search implementation in Praat lets the user limit the search space with respect to the parameter %k. By setting a maximum allowed value of %k the search space can be shrunk considerably.")
NORMAL (U"Due to its discrete (%k) and nominal (vote weighting) nature, the size of the search space is normally of no concern, making an experimental/manual search tractable. The %%model selection% feature of Praat becomes an essential tool only when applied to huge instance bases where the expected optimal value of %k is high. In most cases however, manual experimenting will suffice.")
MAN_END

MAN_BEGIN (U"kNN classifiers 1.2. Improving resource efficiency", U"Ola Söder", 20080529)
NORMAL (U"%%Instance-based% learners such as the %%k%NN classifier implemented in Praat are fairly inefficient as far as CPU and memory usage is concerned. The performance can be improved upon by making sure that only those instances that are vital for the accuracy of the classifier are stored and that non-vital instances are disposed of. Praat does give the user the possibility to prune non-vital or harmful instances, making the resulting classifier less memory and CPU hungry and in some cases more accurate even though that is not the primary objective of the pruning algorithm.")
MAN_END

MAN_BEGIN (U"kNN classifiers 1.2.1. Pruning", U"Ola Söder", 20080529)
NORMAL (U"Pruning is the process of discarding instances that do not improve upon the classification accuracy of the classifier. This group of instances includes noisy instances that, at best, make no difference as far as model accuracy is concerned, at worst, induces classification errors. It also includes instances that are redundant; instances that are implied by the defined neighbourhood.")

ENTRY (U"The C-Pruner algorithm")

NORMAL (U"The C-Pruner algorithm such as it is presented in @@Ke-Ping Zhao et al. (2003)|Zhao et al. (2003)@ identifies pruning candidates and computes the order in which these candidates shall be removed. The ordering is of vital importance since the removal of one candidate might disqualify other candidates, making them non-prunable. In order to understand how the C-Pruner algorithm operates a few definitions are necessary:")

LIST_ITEM (U"\\bu The %k-reachability set of an instance %p consists of the %k nearest neighbours of %p")

LIST_ITEM (U"\\bu The %k-coverage set of an instance %p consists of those instances of the same class as that of %p having %p as one of their %k nearest neighbours.")

LIST_ITEM (U"\\bu An instance %p is superfluous if it is implied by %k-reachability, that is, if it can be correctly classified using %k-reachability as instance base.")

LIST_ITEM (U"\\bu An instance %p is critical if at least one instance in the %k-coverage set of %p is not implied by the %k-reachability set of %p or, after %p is deleted, atleast one instance in the %k-coverage set of %p is not implied by the %k-reachability set of %p.")

LIST_ITEM (U"\\bu An instance %p is noisy if %p isn't superfluous and the %k-reachability set of %p is bigger than the %k-coverage set of %p.")

NORMAL (U"Given these definitions, an instance is tagged for pruning if one of the following conditions hold: It is noisy, or it is superfluous but not critical. This translates to the discarding of instances that are bad class predictors (noise) and of instances that are highly typical of their class and thus are located close to the center of the cluster defining the given class. Instances located close to the class center are very likely implied by the surrounding border instances and thus redundant. In order to avoid destructive domino effects it is important that the pruning starts close to the center of the cluster and works its way out and not the other way around. To impose this ordering the C-Pruner algorithm uses the following heuristics to determine the order of removal of two superfluous instances %p__i_ and %p__j_:")

LIST_ITEM (U"\\bu If the %k-reachability set of %p__i_ contains more instances of the same class as that of %p__i_ than the corresponding value for %p__j_ then %p__i_ should be removed before %p__j_.")

LIST_ITEM (U"\\bu If the %k-reachability set of %p__i_ contains the same number of instances of the same class as that of %p__i_ as the corresponding value for %p__j_ and the distance between %p__i_ and the closest instance of a class different from the class of %p__i_ is greater than the corresponding value for %p__j_ then %p__i_ should be removed before %p__j_.")

LIST_ITEM (U"\\bu If the %k-reachability set of %p__i_ contains the same number of instances of the same class as that of %p__i_ as the corresponding value for %p__j_ and the distance between %p__i_ and the closest instance of a class different from the class of %p__i_ equals the corresponding value for %p__j_ then the order of removal is decided randomly.")

NORMAL (U"In order to gain control of the degree of pruning the Praat implementation of the C-Pruner algorithm decides whether to prune or not prune a given instance tagged for pruning on a probabilistic basis. This makes it possible for the user to specify the hardness of the pruning process (e.g. 100 percent (exp.) noise, 50 percent (exp.) redundancy) to be able to find a good compromise between model accuracy and resource requirements.")
MAN_END

MAN_BEGIN (U"kNN classifiers 2. Quick start", U"Ola Söder", 20080809)
ENTRY (U"An example: Learning the Iris data set")
NORMAL (U"In the @@Feedforward neural networks|the feedforward neural networks tutorial@ a description of how the @FFNet classifier in Praat can be applied to @@iris data set|the Iris data set@ can be found.")
NORMAL (U"The same data can be used to test the %%k%NN feature of Praat. To do so create an example data set using the @@Create iris example...@ command found in the ##Feedforward neural networks# submenu. The form prompting for network topology settings can be ignored by selecting OK. Select the newly created @PatternList and @Categories objects and click ##To KNN Classifier...#. A form prompting for a name of the classifier to be created will be shown. The ordering in which instances are to be inserted into the instance base can also be specified, make sure that #Random is selected and thereafter close the form by selecting OK. The newly created and trained classifier will be shown in the list of objects.")
NORMAL (U"To estimate how well the classifier can be expected to classify new samples of Irises select ##Query -# \\=> ##Get accuracy estimate...#. A form prompting for %%k%NN parameter settings and evaluation method will be shown. Experiment with the parameter settings until satisfactory results are achieved. If everything worked out the estimate will likely end up somewhere in the range of 94-98 percent.")
NORMAL (U"An alternative to manually experimenting with model parameters is to let the computer do the job. This is done be choosing the @KNN object and thereafter selecting ##Query -# \\=> ##Get optimized parameters...#. The form shown prompts for a selection of parameters controlling the search. The default values will in most cases, including this, be appropriate.")
NORMAL (U"Another way of improving classification accuracy is to transform the instance space in which the individual instances, in this case Irises, are stored as to maximize the distance between instances of different classes and minimize the distance between instances of the same class. This can be done by means of feature weighting. To do so select the @KNN object and choose ##To FeatureWeights...#. Adjust the %%k%NN settings according to the ones found by the model search algorithm and let the remaining options retain the default values. Click OK. A @FeatureWeights object will be added to the objects list. The feature weights contained within the newly created object can be used by selecting named object in conjunction with the @KNN classifier and thereafter choosing the desired action.")
MAN_END

MAN_BEGIN (U"k-means clustering", U"Ola Söder", 20080529)
INTRO (U"This tutorial describes the use of %%k%-means clustering in Praat. ")
NORMAL (U"@@k-means clustering 1. How does k-means clustering work?|1. How does k-means clustering work?@")
NORMAL (U"@@k-means clustering 2. Quick start|2. Quick start@")
MAN_END

MAN_BEGIN (U"k-means clustering 1. How does k-means clustering work?", U"Ola Söder", 20080529)
NORMAL (U"The %%k%-means clustering algorithm attempts to split a given anonymous data set (a set containing no information as to class identity) into a fixed number (%k) of clusters.")
NORMAL (U"Initially %k number of so called %centroids are chosen. A %centroid is a data point (imaginary or real) at the center of a cluster. In Praat each centroid is an existing data point in the given input data set, picked at random, such that all %centroids are unique (that is, for all %centroids %c__%i_ and %c__%j_, %c__%i_ \\=/ %c__%j_). These %centroids are used to train a @@kNN classifiers 1. What is a kNN classifier?|kNN classifier@. The resulting classifier is used to classify (using %k = 1) the data and thereby produce an initial randomized set of clusters. Each %centroid is thereafter set to the arithmetic mean of the cluster it defines. The process of classification and %centroid adjustment is repeated until the values of the %centroids stabilize. The final %centroids will be used to produce the final classification/clustering of the input data, effectively turning the set of initially anonymous data points into a set of data points, each with a class identity.")
MAN_END

MAN_BEGIN (U"k-means clustering 2. Quick start", U"Ola Söder", 20080529)
NORMAL (U"Clustering using the %%k%-means clustering algorithm in Praat is done by selecting a @PatternList and choosing ##To Categories...#. In the appearing requester the number of sought after clusters (unique categories) can be specified. The cluster size ratio constraint (%z) imposes a constraint on the output such that %cluster size(%x) / %cluster size(%y) > %z for all clusters %x and %y in the resulting set of clusters. Valid values of %z are 0 < %z <= 1 where values near 0 imposes practically no constraints on the cluster sizes and a value of 1 tells the algorithm to attempt to create clusters of equal size. The size ratio constraint is enforced in a very naive fashion, by random reseeding. Since this can be a rather time consuming process it is possible to set an upper bound on the number of reseeds done by the algorithm. This upper bound is defined by the parameter ##Maximum number of reseeds#. It should be noted however that normally there's no need to use the size ratio constraint, selecting the desired number of clusters will, on average, result in clusters of roughly equal size, given well distributed data.")
MAN_END

MAN_BEGIN (U"PatternList to Dissimilarity", U"Ola Söder", 20080529)
NORMAL (U"A @Dissimilarity matrix can be used in conjunction with @@Multidimensional scaling|Multidimensional scaling@ to aid visualization of high-dimensional data. A @Dissimilarity object is a matrix of the distances, according to the chosen @@Euclidean distance|distance function@, between all the data points in the @PatternList object.")
NORMAL (U"A @Dissimilarity object can be created by selecting a @PatternList object and choosing ##To Dissimilarity#. The dissimilarity matrix can also be computed using feature weights. This is done by selecting a @PatternList object, an @FeatureWeights object and choosing ##To Dissimilarity#.")
MAN_END

MAN_BEGIN (U"Euclidean distance", U"Ola Söder", 20080529)
NORMAL (U"The Euclidean distance between the %n-dimensional vectors %a and %b can be calculated as follows:")
FORMULA (U"\\De__Euclidean_(%x,%y) = \\Vr(\\su__%i=1..%n_(%a__%i_ - %b__%I_)^2)")
MAN_END

MAN_BEGIN (U"kNN classifiers 3. Command overview", U"Ola Söder", 20080809 )
INTRO (U"KNN commands")
ENTRY (U"Creation:")
LIST_ITEM (U"\\bu @@PatternList & Categories: To KNN classifier...@")
LIST_ITEM (U"\\bu @@Create KNN...@")
ENTRY (U"Learning:")
LIST_ITEM (U"\\bu @@KNN & PatternList & Categories: Learn...@")
ENTRY (U"Classification:")
LIST_ITEM (U"\\bu @@KNN & PatternList: To Categories...@")
LIST_ITEM (U"\\bu @@KNN & PatternList: To TabelOfReal...@")
LIST_ITEM (U"\\bu @@KNN & PatternList & FeatureWeights: To Categories...@")
LIST_ITEM (U"\\bu @@KNN & PatternList & FeatureWeights: To TableOfReal...@")
ENTRY (U"Evaluation:")
LIST_ITEM (U"\\bu @@KNN & PatternList & Categories: Evaluate...@")
LIST_ITEM (U"\\bu @@KNN & PatternList & Categories & FeatureWeights: Evaluate...@")
ENTRY (U"Queries")
LIST_ITEM (U"\\bu @@KNN: Get optimized parameters...@")
LIST_ITEM (U"\\bu @@KNN: Get accuracy estimate...@")
LIST_ITEM (U"\\bu @@KNN & FeatureWeights: Get accuracy estimate...@")
LIST_ITEM (U"\\bu @@KNN: Get size of instance base@")
ENTRY (U"Extractions")
LIST_ITEM (U"\\bu @@KNN: Extract input Pattern@")
LIST_ITEM (U"\\bu @@KNN: Extract output Categories@")
ENTRY (U"Modification:")
LIST_ITEM (U"\\bu @@KNN: Shuffle@")
LIST_ITEM (U"\\bu @@KNN: Prune...@")
LIST_ITEM (U"\\bu @@KNN: Reset...@")
ENTRY (U"Miscellaneous:")
LIST_ITEM (U"\\bu @@KNN: To FeatureWeights...@")
LIST_ITEM (U"\\bu @@KNN & PatternList & Categories: To FeatureWeights...@") 
LIST_ITEM (U"\\bu @@PatternList & Categories: To FeatureWeights...@")
ENTRY (U"Pre/post processing:")
LIST_ITEM (U"\\bu @@PatternList: To Categories...@")
LIST_ITEM (U"\\bu @@PatternList & FeatureWeights: To Categories...@")
LIST_ITEM (U"\\bu @@PatternList: To Dissimilarity...@")
LIST_ITEM (U"\\bu @@PatternList & FeatureWeights: To Dissimilarity...@")
MAN_END

MAN_BEGIN (U"PatternList & Categories: To KNN classifier...", U"Ola Söder", 20080726 )
INTRO (U"Create and train a @KNN classifier using the selected @PatternList and @Categories objects as training data.")
ENTRY (U"Settings")
TAG (U"##Name")\
DEFINITION (U"The name of the @KNN classifier.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
MAN_END

MAN_BEGIN (U"Create KNN...", U"Ola Söder", 20080718 )
INTRO (U"Create a new @KNN classifier.")
ENTRY (U"Settings")
TAG (U"##Name")
DEFINITION (U"The name of the classifier.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN & PatternList & Categories: Learn...", U"Ola Söder", 20080726 )
INTRO (U"Train the selected @KNN classifier using the chosen @PatternList and @Categories objects as training data.")
ENTRY (U"Settings")
TAG (U"##Name")
DEFINITION (U"The name of the classifier.")
TAG (U"##Ordering")
DEFINITION (U"The order in which the training instances are to be inserted into the instance base.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
MAN_END

MAN_BEGIN (U"KNN & PatternList: To Categories...", U"Ola Söder", 20080726 )
INTRO (U"Use the selected @KNN classifier to classify the chosen @PatternList. A @Categories object containing the result will be created.")
ENTRY (U"Settings")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
MAN_END

MAN_BEGIN (U"KNN & PatternList: To TabelOfReal...", U"Ola Söder", 20080718 )
INTRO (U"Use the selected @KNN classifier to classify the chosen @PatternList. A @TableOfReal object containing verbose information on the decision process  will be created.")
ENTRY (U"Settings")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
TAG (U"##Output")
DEFINITION (U"Specifies the degree of verbosity, where %%winners only% generates a @TableOfReal containing information on the instances of the winning category only whereas %%All candidates% results in a @TableOfReal with verbose information on all unique categories in the neighbourhood defined by the parameter %k.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
MAN_END

MAN_BEGIN (U"KNN & PatternList & FeatureWeights: To Categories...", U"Ola Söder", 20080726 )
INTRO (U"Use the selected @KNN classifier and @FeatureWeights object to classify the chosen @PatternList. A @Categories object containing the result will be created.")
ENTRY (U"Settings")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1.1. Filter-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1.2. Wrapper-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN & PatternList & FeatureWeights: To TableOfReal...", U"Ola Söder", 20080718 )
INTRO (U"Use the selected @KNN classifier and the feature weights, @FeatureWeights, to classify the chosen @PatternList. A @TableOfReal object containing verbose information on the decision process will be created.")
ENTRY (U"Settings")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
TAG (U"##Output")
DEFINITION (U"Specifies the degree of verbosity, where %%winners only% generates a @TableOfReal containing information on the instances of the winning category only whereas %%All candidates% results in a @TableOfReal with verbose information on all unique categories in the neighbourhood defined by the parameter %k.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1.1. Filter-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1.2. Wrapper-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Get optimized parameters...", U"Ola Söder", 20080718 )
INTRO (U"Perform an automated search for the optimal @KNN parameter settings.")
ENTRY (U"Settings")
TAG (U"##Evaluation method")
DEFINITION (U"The method to be used for estimating the classification accuracy, which in turn is used to guide the model search. "
	"Supported methods are 10-fold cross-validation and leave-one-out.")
TAG (U"##k max")
DEFINITION (U"The maximum value of the parameter %k.")
TAG (U"##Number of seeds")
DEFINITION (U"The size of the parameter neighbourhood to be searched.")
TAG (U"##Learning rate")
DEFINITION (U"The rate at which the parameter neighbourhood is shrunk (per iteration).")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers 1.1.2. Model selection@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Get accuracy estimate...", U"Ola Söder", 20080718 )
INTRO (U"Estimate the classification accuracy of the selected @KNN classifier.")
ENTRY (U"Settings")
TAG (U"##Evaluation method")
DEFINITION (U"The method to be used for estimating the classification accuracy. Supported methods are 10-fold cross-validation and leave-one-out.")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@KNN & FeatureWeights: Get accuracy estimate...@")
LIST_ITEM (U"@@KNN & PatternList & Categories: Evaluate...@")
LIST_ITEM (U"@@KNN & PatternList & Categories & FeatureWeights: Evaluate...@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.2. Model selection@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END  

MAN_BEGIN (U"KNN & FeatureWeights: Get accuracy estimate...", U"Ola Söder", 20080809)
INTRO (U"Estimate the classification accuracy of the @KNN classifier using the chosen @FeatureWeights.")
ENTRY (U"Settings")
TAG (U"##Evaluation method")
DEFINITION (U"The method to be used for estimating the classification accuracy. "
	"Supported methods are 10-fold cross-validation and leave-one-out.")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@KNN & PatternList & Categories: Evaluate...@")
LIST_ITEM (U"@@KNN & PatternList & Categories & FeatureWeights: Evaluate...@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.2. Model selection@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END 

MAN_BEGIN (U"KNN & PatternList & Categories: Evaluate...", U"Ola Söder", 20080718)
INTRO (U"Estimate the classification accuracy of the chosen @KNN classifier using the selected @PatternList and @Categories objects as test set.")
ENTRY (U"Settings")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@KNN & PatternList & Categories & FeatureWeights: Evaluate...@")
LIST_ITEM (U"@@KNN: Get accuracy estimate...@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.2. Model selection@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN & PatternList & Categories & FeatureWeights: Evaluate...", U"Ola Söder", 20080718)
INTRO (U"Estimate the classification accuracy of the chosen @KNN classifier using the selected @PatternList and @Categories objects as test set. The selected @FeatureWeights object will be used in the classification process.")
ENTRY (U"Settings")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@KNN & PatternList & Categories: Evaluate...@")
LIST_ITEM (U"@@KNN: Get accuracy estimate...@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.2. Model selection@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Get size of instance base", U"Ola Söder", 20080718)
INTRO (U"Queries the selected @KNN for the number of instances in the instance base. ")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Extract input Pattern", U"Ola Söder", 20080726)
INTRO (U"Create a new @PatternList object identical to the one in the instance base of the selected @KNN classifier.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Extract output Categories", U"Ola Söder", 20080726)
INTRO (U"Create a new @Categories object identical to the one in the instance base of the selected @KNN classifier.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Shuffle", U"Ola Söder", 20080718)
INTRO (U"Shuffle the instance base of the selected classifier.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Prune...", U"Ola Söder", 20080718)
INTRO (U"Prune the instance base of the selected classifier.")
ENTRY (U"Settings")
TAG (U"##Noise pruning degree")
DEFINITION (U"A value in the range [0 ... 1] specifying the probability that instances tagged as noisy by @@kNN classifiers 1.2.1. Pruning|the C-Pruner algorithm@ will be pruned.")
TAG (U"##Redundancy pruning degree")
DEFINITION (U"The probability that instances tagged as redundant by @@kNN classifiers 1.2.1. Pruning|the C-Pruner algorithm@ will be pruned.")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers 1.2.1. Pruning@")
LIST_ITEM (U"@@kNN classifiers 1.2. Improving resource efficiency@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: Reset...", U"Ola Söder", 20080718)
INTRO (U"Empty the instance base of the selected classifier.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN: To FeatureWeights...", U"Ola Söder", 20080728)
INTRO (U"Wrap the selected @KNN and use its feedback to guide the search for the optimal feature weights. A @FeatureWeights object will be created.")
ENTRY (U"Settings")
TAG (U"##Learning rate")
DEFINITION (U"The rate at which the maximum distance between the pivot and a random seed is decremented.")
TAG (U"##Number of seeds")
DEFINITION (U"The size of the feature weight neighbourhood.")
TAG (U"##Stop at")
DEFINITION (U"A value specifying a stopping criterion. When feature weights yielding accuracy estimates higher than the specified value the search will stop. A value of 1 imposes no constraints whereas a value of 0.5 will result in the termination of the search algorithm once feature weights resulting in an classification accuracy of 50 percent or better are found.")
TAG (U"##Optimization")
DEFINITION (U"Specifies whether to search for all features simultaneously or one at a time.")
TAG (U"##Evaluation method")
DEFINITION (U"The method to be used for estimating the classification accuracy. Supported methods are 10-fold cross-validation and leave-one-out.")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood used for feedback classification.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers 1.1.1.2. Wrapper-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@KNN & PatternList & Categories & FeatureWeights: Evaluate...@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"KNN & PatternList & Categories: To FeatureWeights...", U"Ola Söder", 20080809)
INTRO (U"Wrap the selected @KNN and use its classification accuracy on the test set constituted by the @PatternList and @Categories objects as feedback to guide the search for the optimal feature weights. A @FeatureWeights object will be created.")
ENTRY (U"Settings")
TAG (U"##Learning rate")
DEFINITION (U"The rate at which the maximum distance between the pivot and a random seed is decremented.")
TAG (U"##Number of seeds")
DEFINITION (U"The size of the feature weight neighbourhood.")
TAG (U"##Stop at")
DEFINITION (U"A value specifying a stopping criterion. When feature weights yielding accuracy estimates higher than the specified value the search will stop. A value of 1 imposes no constraints whereas a value of 0.5 will result in the termination of the search algorithm once feature weights resulting in an classification accuracy of 50 percent or better are found.")
TAG (U"##Optimization")
DEFINITION (U"Specifies whether to search for all features simultaneously or one at a time.")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood used for feedback classification.")
TAG (U"##Vote weighting")
DEFINITION (U"The type of vote weighting to be used.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers 1.1.1.2. Wrapper-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1. Improving classification accuracy@")
LIST_ITEM (U"@@KNN & PatternList & Categories & FeatureWeights: Evaluate...@")
LIST_ITEM (U"@@kNN classifiers 1. What is a kNN classifier?@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"PatternList & Categories: To FeatureWeights...", U"Ola Söder", 20080809) 
INTRO (U"Compute an estimate of the optimal feature weights using the @@kNN classifiers 1.1.1.1. Filter-based feature weighting|RELIEF-F algorithm@.")
ENTRY (U"Setting")
TAG (U"##k neighbours")
DEFINITION (U"The size of the neighbourhood.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers 1.1.1.1. Filter-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers 1.1.1.2. Wrapper-based feature weighting@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"PatternList: To Categories...", U"Ola Söder", 20080728)
INTRO (U"Split the given @PatternList into a fixed number of clusters using the @@k-means clustering|%%k%-means clustering algorithm@. A @Categories object containing numbered categories corresponding to the generated clusters will be created.")
ENTRY (U"Settings")
TAG (U"##k clusters")
DEFINITION (U"The number of clusters to be generated.")
TAG (U"##Cluster size ratio constraint")
DEFINITION (U"The minimum allowed ratio between the smallest and the biggest cluster.")
TAG (U"##Maximum number of reseeds")
DEFINITION (U"The maximum allowed number of reseeds used to enforce the cluster size ratio constraint.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"PatternList & FeatureWeights: To Categories...", U"Ola Söder", 20080728)
INTRO (U"Split the given @PatternList into a fixed number of clusters using the @@k-means clustering|%%k%-means clustering algorithm@ and the feature weights contained within the selected @FeatureWeights object. A @Categories object containing numbered categories corresponding to the generated clusters will be created.")
ENTRY (U"Settings")
TAG (U"##k clusters")
DEFINITION (U"The number of clusters to be generated.")
TAG (U"##Cluster size ratio constraint")
DEFINITION (U"The minimum allowed ratio between the smallest and the biggest cluster.")
TAG (U"##Maximum number of reseeds")
DEFINITION (U"The maximum allowed number of reseeds used to enforce the cluster size ratio constraint.")
ENTRY (U"See also:")
LIST_ITEM (U"@@kNN classifiers@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
MAN_END

MAN_BEGIN (U"PatternList: To Dissimilarity...", U"Ola Söder", 20080718)
INTRO (U"Generate a @Dissimilarity matrix from the selected @PatternList. Dissimilarities are computed using the @@Euclidean distance@")
ENTRY (U"See also:")
LIST_ITEM (U"@@Multidimensional scaling@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"PatternList & FeatureWeights: To Dissimilarity...", U"Ola Söder", 20080718)
INTRO (U"Generate a @Dissimilarity matrix from the selected @PatternList using the feature weights contained within the selected @FeatureWeights object. Dissimilarities are computed using the @@Euclidean distance@.")
ENTRY (U"See also:")
LIST_ITEM (U"@@Multidimensional scaling@")
LIST_ITEM (U"@@kNN classifiers 1.1.1. Feature weighting@")
LIST_ITEM (U"@@kNN classifiers@")
MAN_END

MAN_BEGIN (U"Ke-Ping Zhao et al. (2003)", U"Ola Söder", 20080718)
NORMAL (U"Ke-Ping Zhao et al. (2003): \"C-Pruner: An improved instance pruning algorithm.\" "
	"In %%Proceedings of the Second International Conference on Machine Learning and Cybernetics%, November 2003, 94\\--99.")
MAN_END

MAN_BEGIN (U"Igor Kononenko (1994)", U"Ola Söder", 20080718)
NORMAL (U"Igor Kononenko (1994): \"Estimating attributes: Analysis and extensions of relief.\" "
	"In %%ECML-94: Proceedings of the European Conference on Machine Learning%, Secaucus, NJ, USA, 171\\--182. "
	"New York: Springer.")
MAN_END

}

/* End of file manual_KNN.c */
