/* manual_FFNet.c
 *
 * Copyright (C) 1994-2019 David Weenink
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
 djmw 20020408 GPL
*/

#include "ManPagesM.h"

#include "FFNet.h"

static void drawFFNet_345 (Graphics g)
{
	autoFFNet me = FFNet_create (3, 4, 0, 5, false);
	FFNet_drawTopology (me.get(), g);
}

void manual_FFNet_init (ManPages me);
void manual_FFNet_init (ManPages me)
{

MAN_BEGIN (U"epoch", U"djmw", 20040428)
INTRO (U"A term that is often used in the context of machine learning. An epoch is one complete "
	"presentation of the %%data set to be learned% to a learning machine.")
NORMAL (U"Learning machines like @@FFNet|feedforward neural nets@ that use iterative algorithms "
	"often need many epochs during their learning phase.")
NORMAL (U"A @@Discriminant|discriminant classifier@ is also a learning machine. "
	"However, in contrast with neural nets a discriminant classifier only needs one epoch to learn.")
MAN_END

MAN_BEGIN (U"Feedforward neural networks", U"djmw", 20040511)
INTRO (U"This tutorial describes the use of @FFNet feedforward neural networks in P\\s{RAAT}. ")
NORMAL (U"@@Feedforward neural networks 1. What is a feedforward neural network?|1. What is a feedforward neural network?@")
LIST_ITEM (U"  @@Feedforward neural networks 1.1. The learning phase|1.1 The learning phase")
LIST_ITEM (U"  @@Feedforward neural networks 1.2. The classification phase|1.2 The classification phase")
NORMAL (U"@@Feedforward neural networks 2. Quick start|2. Quick start@")
NORMAL (U"@@Feedforward neural networks 3. FFNet versus discriminant classifier|3. FFNet versus discriminant classifier@")
NORMAL (U"@@Feedforward neural networks 4. Command overview|4. Command overview@")
MAN_END

MAN_BEGIN (U"Feedforward neural networks 1. What is a feedforward neural network?", U"djmw", 20040426)
INTRO (U"A feedforward neural network is a biologically inspired classification algorithm. "
	"It consist of a (possibly large) number of simple neuron-like processing %units, organized in %layers. "
	"Every unit in a layer is connected with all the units in the previous layer. "
	"These connections are not all equal: each connection may have a different strength or %weight. "
	"The weights on these connections encode the knowledge of a network. "
	"Often the units in a neural network are also called %nodes.")
NORMAL (U"Data enters at the inputs and passes through the network, layer by layer, until it arrives at the outputs. "
	"During normal operation, that is when it acts as a classifier, there is no feedback between layers. "
	"This is why they are called %%feedforward% neural networks. ")
NORMAL (U"In the following figure we see an example of a 2-layered network with, from top to bottom: "
	"an output layer with 5 units, a %hidden layer with 4 units, respectively. The network has 3 input units.")
PICTURE (5, 5, drawFFNet_345)
NORMAL (U"The 3 inputs are shown as circles and these do not belong to any layer of the network (although the inputs "
	"sometimes are considered as a virtual layer with layer number 0). Any layer that is not an output layer is a "
	"%hidden layer. This network therefore has 1 hidden layer and 1 output layer. The figure also shows all the "
	"connections between the units in different layers. A layer only connects to the previous layer. ")
NORMAL (U"The operation of this network can be divided into two phases:")
NORMAL (U"@@Feedforward neural networks 1.1. The learning phase|1. The learning phase")
NORMAL (U"@@Feedforward neural networks 1.2. The classification phase|2. The classification phase")
MAN_END

MAN_BEGIN (U"Feedforward neural networks 1.1. The learning phase", U"djmw", 20040428)
INTRO (U"During the learning phase the weights in the FFNet will be modified. "
	"All weights are modified in such a way that when a pattern is presented, "
	"the output unit with the correct category, hopefully, will have the largest output value.")
ENTRY (U"How does learning take place?")
NORMAL (U"The FFNet uses a %supervised learning algorithm: besides the input pattern, "
	"the neural net also needs to know to what category the pattern belongs. "
	"Learning proceeds as follows: a pattern is presented at the inputs. "
	"The pattern will be transformed in its passage through the layers of the network until it "
	"reaches the output layer. The units in the output layer all belong to a different category. "
	"The outputs of the network as they are now are compared with the outputs as they ideally would "
	"have been if this pattern were correctly classified: in the latter case "
	"the unit with the correct category would have had the largest output value and the "
	"output values of the other output units would have been very small. "
	"On the basis of this comparison all the connection weights are modified a little bit to guarantee that, the next time "
	"this same pattern is presented at the inputs, the value of the output unit that corresponds with the correct category "
	"is a little bit higher than it is now and that, at the same time, the output values of all the other incorrect outputs are a "
	"little bit lower than they are now. (The differences between the actual outputs and the idealized outputs "
	"are propagated back from the top layer to lower layers to be used at these layers to modify connection weights. "
	"This is why the term %%backpropagation network% is also often used to describe this type of neural network.)")
NORMAL (U"If you perform the procedure above once for every pattern and category pair in your data "
	"set you have performed one @epoch of learning.")
NORMAL (U"The hope is that eventually, probably after many epochs, "
	"the neural net will come to remember these pattern-category pairs. "
	"You even hope that the neural net, when the learning phase has terminated, will be able to %generalize "
	"and has learned to "
	"@@FFNet & PatternList: To Categories...|classify@ correctly any unknown pattern presented to it. ")
NORMAL (U"Because real-life data often contains noise as well as partly contradictory information, "
	"these hopes can be fulfilled only partly. ")
NORMAL (U"For @@FFNet & PatternList & Categories: Learn...|learning@ you "
	"need to select three different objects together: a FFNet (the %classifier), "
	"a PatternList (the %inputs) and a Categories (the %%correct outputs%).")
ENTRY (U"How long will the learning phase take?")
NORMAL (U"In general, this question is hard to answer. It depends on the size of the neural network, "
	"the number of patterns to be learned, the number of epochs, the tolerance of the minimizer "
	"and the speed of your computer, how much computing time the learning phase may take. ")
NORMAL (U"If computing time becomes excessive in your interactive environment then consider using the "
	"powerful @@Scripting|scripting@ facilities in Praat to process your learning job as a batch job. ")
MAN_END

MAN_BEGIN (U"Feedforward neural networks 1.2. The classification phase", U"djmw", 20040428)
INTRO (U"In the classification phase, the weights of the network are fixed. ")
NORMAL (U"A pattern, presented at the inputs, will be transformed from layer to layer until it reaches the output layer. "
	"Now classification can occur by selecting the category associated with the output unit that has "
	"the largest output value.  "
	"For classification we only need to select an FFNet and a PatternList together and "
	"choose @@FFNet & PatternList: To Categories...|To Categories...@. ")
NORMAL (U"In contrast to the @@Feedforward neural networks 1.1. The learning phase|learning phase@ classification is very fast.")
MAN_END

MAN_BEGIN (U"Feedforward neural networks 2. Quick start", U"djmw", 20040426)
INTRO (U"You may create the iris example set with the @@Create iris example...@ command "
	"that you will find under the ##Feedforward neural networks# option in the #New menu. "
	"Three new objects will appear in the @@List of Objects@: a @FFNet, a @Categories and "
	"a @PatternList.")
NORMAL (U"The #PatternList contains the @@iris data set@ in a matrix of 150 rows by 4 columns. "
	"To guarantee that every cell in the PatternList is in the [0,1] interval, all measurement "
	"values were divided by 10. In the #Categories the three iris species %setosa, "
	"%versicolor, and %virginica were categorized with the numbers #1, #2 and #3, respectively. "
	"Because there are 4 data columns in the PatternList and 3 different iris species in the Categories, "
	"the newly created #FFNet has 4 inputs and 3 outputs. "
	"If you enter a positive number in one of the fields in the form, the FFNet will have "
	"this number of units in a %%hidden layer%. The name of the newly created FFNet "
	"will reflect its topology. If you opt for the standard setting, which is 0 hidden units, the FFNet will be named 4-3.")
ENTRY (U"Learning the iris data")
NORMAL (U"The first thing you probably might want to do is to let the #FFNet learn the association in "
	"each pattern-category pair. To do this select all three objects together and choose "
	"@@FFNet & PatternList & Categories: Learn...|Learn...@. "
	"A form will appear, asking you to supply some settings for "
	"the learning algorithm. Learning starts after you have clicked the OK button. "
	"As the example network has only a small number of weights that need to be adjusted, "
	"and the learning data set is very small, this will only take a very short time.")
ENTRY (U"Classification")
NORMAL (U"Now, if you are curious how well the FFNet has learned the iris data, you may select the "
	"#FFNet and the #PatternList together and choose @@FFNet & PatternList: To Categories...|To Categories...@. "
	"A new #Categories appears in the ##List of Objects# with the name %%4-3_iris% (if %%4-3% was the name of the FFNet and %iris% the name of the PatternList). "
	"We have two different Categories in the list of objects, the topmost one has the original categories, the other "
	"the categories as were assigned by the FFNet classifier. The obvious thing to do now is to compare the "
	"original categories with the assigned categories by making a @@Confusion|confusion table@. "
	"Select the two #Categories and choose @@Categories: To Confusion|To Confusion@ and a newly "
	"created @Confusion appears. Pressing the @Info button will show you an info window with, "
	"among others, the fraction correct. ")
NORMAL (U"You may also want to "
	"@@Feedforward neural networks 3. FFNet versus discriminant classifier|compare the FFNet classifier with a discriminant classifier@.")
ENTRY (U"Create other neural net topologies")
NORMAL (U"With a #PatternList and a #Categories selected together, you can for example create a new #FFNet of a different topology.")
MAN_END

MAN_BEGIN (U"Feedforward neural networks 3. FFNet versus discriminant classifier", U"djmw", 20040426)
NORMAL (U"You may want to compare the FFNet classifier with a discriminant classifier. "
	"Unlike the FFNet, a @@Discriminant|discriminant@ classifier does not need any iterative procedure in the "
	"learning phase and can be used immediately after creation for classification. "
	"The following three simple steps will give you the confusion matrix based on discriminant analysis:")
LIST_ITEM (U"1. Select the PatternList and the Categories together and choose ##To Discriminant#.  "
	"A newly created Discriminant will appear.")
LIST_ITEM (U"2. Select the Discriminant and the PatternList together and choose ##To Categories...#. A newly created @Categories will appear.")
LIST_ITEM (U"3. Select the two appropriate Categories and choose @@categories: To Confusion|To Confusion@.  "
	"A newly created @Confusion will appear. After pushing the @Info button, the info window will "
	"show you the fraction correct.")
NORMAL (U"See also the @@Discriminant analysis@ tutorial for more information.")
MAN_END

MAN_BEGIN (U"Feedforward neural networks 4. Command overview", U"djmw", 20040426)
INTRO (U"FFNet commands")
ENTRY (U"Creation:")
LIST_ITEM (U"\\bu @@PatternList & Categories: To FFNet...@")
LIST_ITEM (U"\\bu @@Create FFNet...@")
ENTRY (U"Learning:")
LIST_ITEM (U"\\bu @@FFNet & PatternList & Categories: Learn...@")
LIST_ITEM (U"\\bu @@FFNet & PatternList & Categories: Learn slow...@")
ENTRY (U"Classification:")
LIST_ITEM (U"\\bu @@FFNet & PatternList: To Categories...@")
ENTRY (U"Drawing:")
LIST_ITEM (U"\\bu @@FFNet: Draw topology@")
LIST_ITEM (U"\\bu @@FFNet: Draw weights...@")
LIST_ITEM (U"\\bu @@FFNet: Draw cost history...@")
ENTRY (U"Queries")
LIST_ITEM (U"\\bu @@FFNet & PatternList & Categories: Get total costs...@")
LIST_ITEM (U"\\bu @@FFNet & PatternList & Categories: Get average costs...@")
LIST_ITEM (U"\\bu @@FFNet & PatternList & ActivationList: Get total costs...@")
LIST_ITEM (U"\\bu @@FFNet & PatternList & ActivationList: Get average costs...@")
ENTRY (U"Analysis:")
LIST_ITEM (U"\\bu ##FFNet & PatternList: To ActivationList...#")
LIST_ITEM (U"\\bu @@FFNet & PatternList & Categories: Get total costs...@")
LIST_ITEM (U"\\bu @@FFNet & PatternList & Categories: Get average costs...@")
LIST_ITEM (U"\\bu @@FFNet & PatternList & ActivationList: Get total costs...@")
LIST_ITEM (U"\\bu @@FFNet & PatternList & ActivationList: Get average costs...@")
ENTRY (U"Analysis:")
LIST_ITEM (U"\\bu ##FFNet & PatternList: To Activation...#")
ENTRY (U"Modification:")
LIST_ITEM (U"\\bu @@FFNet: Reset...@")
LIST_ITEM (U"\\bu ##FFNet: Select biases...#")
LIST_ITEM (U"\\bu ##FFNet: Select all weights#")
MAN_END

MAN_BEGIN (U"FFNet", U"djmw", 19961015)
INTRO (U"One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL (U"A #FFNet models a feedforward neural net. A feedforward "
	"neural net can %learn associations between its %input and its %output. "
	"The @@Feedforward neural networks@ tutorial gives you an introduction to feedforward neural nets.")
MAN_END

MAN_BEGIN (U"FFNet: Draw topology", U"djmw", 19970218)
INTRO (U"You can choose this command after selecting 1 or more @FFNet's.")
ENTRY (U"Behaviour")
NORMAL (U"Draws all units and all connections of a feedforward neural net.")
MAN_END

MAN_BEGIN (U"FFNet: Draw weights...", U"djmw", 20040422)
INTRO (U"Draws the weights in a layer of the selected @FFNet feedforward neural net.")
ENTRY (U"Settings")
TAG (U"##Layer number")
DEFINITION (U"determines the layer.")
TAG (U"##Garnish")
DEFINITION (U"determines whether additional information is drawn.")
ENTRY (U"Behaviour")
NORMAL (U"The weights are arranged in a matrix. The columns of this matrix are indexed by the units in the layer, "
	"while the rows are indexed by the units in the previous layer. There is one extra row for the biases."
	"The values of the weights are shown as rectangles. The area of a rectangle is proportional "
	"to the value.  Negative values are shown as filled black rectangles.")
MAN_END

MAN_BEGIN (U"FFNet: Draw cost history...", U"djmw", 19970218)
INTRO (U"You can choose this command after selecting 1 or more @FFNet's.")
ENTRY (U"Settings")
TAG (U"##Iteration range")
DEFINITION (U"determines the horizontal range of the plot.")
TAG (U"##Cost range")
DEFINITION (U"determines the vertical range of the plot.")
TAG (U"##Garnish")
DEFINITION (U"determines whether a box and axis labels are drawn.")
ENTRY (U"Behaviour")
NORMAL (U"Draws the history of the cost versus iteration number during previous learning.")
MAN_END

MAN_BEGIN (U"FFNet: Extract weights...", U"djmw", 20040422)
INTRO (U"Extract all the weights, from all the units in the specified layer of the selected "
	"@FFNet, to a @TableOfReal.")
ENTRY (U"Settings")
TAG (U"##Layer number")
DEFINITION (U"determines the layer.")
ENTRY (U"Behaviour")
NORMAL (U"The weights will be arranged in the TableOfReal as follows: ")
NORMAL (U"The table columns will be indexed by the unit numbers in the selected layer, "
	"while the rows will be indexed by the unit numbers from the previous layer. "
	"There will be one extra row to accommodate the bias weights. "
	"The rows and columns are labelled with layer number and unit number as \"L%i-%j\", where %i is the layer number and "
	"%j the unit number from that layer. The layer number for the rows is one less than the layer number in the columns. "
	"The last row is labelled as \"Bias\".")
MAN_END

MAN_BEGIN (U"FFNet: Get number of outputs", U"djmw", 20040420)
INTRO (U"Queries the selected @FFNet for the number of output units in the output layer. ")
MAN_END

MAN_BEGIN (U"FFNet: Get number of inputs", U"djmw", 20040420)
INTRO (U"Queries the selected @FFNet for the number of inputs. ")
NORMAL (U"For a network with only one layer, the inputs are connected directly to the output layer. "
	"In a two-layer network the inputs are connected to a hidden layer.")
MAN_END

MAN_BEGIN (U"FFNet: Get number of hidden units...", U"djmw", 20040420)
INTRO (U"Queries the selected @FFNet for the number of units in a hidden layer.")
ENTRY (U"Settings")
TAG (U"##Hidden layer number")
DEFINITION (U"determines the layer that is queried.")
ENTRY (U"Layer numbering")
NORMAL (U"The number of hidden layers is always one less than the total number of layers in a FFNet. "
	"A network with the output units connected to the inputs therefore has only 1 layer, the output layer and "
	"no hidden layers. ")
MAN_END

MAN_BEGIN (U"FFNet: Get number of hidden weights...", U"djmw", 20040420)
INTRO (U"Queries the selected @FFNet for the number of weights in a hidden layer.")
ENTRY (U"Settings")
TAG (U"##Hidden layer number")
DEFINITION (U"determines the layer that is queried.")
MAN_END

MAN_BEGIN (U"FFNet: Reset...", U"djmw", 20040420)
INTRO (U"You can choose this command after selecting 1 or more @FFNet's.")
ENTRY (U"WARNING")
NORMAL (U"This command destroys all previous learning.")
ENTRY (U"Settings")
TAG (U"##Range")
DEFINITION (U"determines the upper limit of  the [-%range, +%range] interval from "
	"which new weights will be randomly selected.")
ENTRY (U"Behaviour")
NORMAL (U"All (selected) weights are reset to random numbers uniformly drawn from the interval [-%range, +%range]. "
	"This command also clears the cost history.")
MAN_END

MAN_BEGIN (U"FFNet: Select biases...", U"djmw", 20040422)
INTRO (U"Selects only the biases in one particular layer as subject for modification during learning of the @FFNet.")
ENTRY (U"Settings")
TAG (U"##Layer number")
DEFINITION (U"determines the layer whose biases will be modified.")
ENTRY (U"Behaviour")
NORMAL (U"This command induces very specific behaviour during a following learning phase. "
	"Instead of all the weights, only the biases in the specified layer will be changed during learning and the "
	"rest of the weights stay fixed. ")
MAN_END

#define FFNet_Create_COMMON_HELP_INOUT \
ENTRY (U"Settings")\
TAG (U"##Number of inputs")\
DEFINITION (U"the dimension of the input of the neural net.")\
TAG (U"##Number of outputs (\\>_ 1)#")\
DEFINITION (U"the number of different categories that you want the net to learn.")

#define FFNet_Create_COMMON_HELP_HIDDEN \
TAG (U"##Number of units in hidden layer 1#, ##Number of units in hidden layer 2#") \
DEFINITION (U"determine the number of units in the hidden layers. " \
	"If you want a neural net with no hidden layers, both numbers have to be 0. "\
	"If you want a neural net with only 1 hidden layer then one of these numbers should differ from 0. ")

MAN_BEGIN (U"Create FFNet...", U"djmw", 20040420)
INTRO (U"Create a new feedforward neural net of type @FFNet.")
FFNet_Create_COMMON_HELP_INOUT
FFNet_Create_COMMON_HELP_HIDDEN
MAN_END

MAN_BEGIN (U"Create FFNet (linear outputs)...", U"djmw", 20040422)
INTRO (U"Create a @FFNet feedforward neural network whose output units are linear.")
FFNet_Create_COMMON_HELP_INOUT
FFNet_Create_COMMON_HELP_HIDDEN
MAN_END

MAN_BEGIN (U"Create iris example...", U"djmw", 20160524)
INTRO (U"A @FFNet feedforward neural net will be created together with two other objects: "
	"a @PatternList and a @Categories. The PatternList will contain the observations in the @@iris data set@, "
	"and the Categories will contain the 3 different iris species categorized by numbers.")
ENTRY (U"Settings")
FFNet_Create_COMMON_HELP_HIDDEN
NORMAL (U"For this simple data you can leave both hidden layers empty.")

MAN_END

MAN_BEGIN (U"iris data set", U"djmw", 19961015)
NORMAL (U"A data set with 150 random samples of flowers from the iris species %setosa, "
	"%versicolor, and %virginica collected by @@Anderson (1935)@. From each species there are 50 observations for "
	"sepal length, sepal width, petal length, and petal width in cm. This dataset was "
	"used by @@Fisher (1936)@ in his initiation of the linear-discriminant-function technique.")
MAN_END

MAN_BEGIN (U"FFNet: Categories", U"djmw", 19960918)
INTRO (U"The categories for training a neural net with a @PatternList. ")
ENTRY (U"Preconditions")
NORMAL (U"The number of categories in a @Categories must equal the number of rows in #PatternList.")
MAN_END

MAN_BEGIN (U"ActivationList", U"djmw", 20160524)
INTRO (U"A list of activations, organized as a @Matrix whose elements should be >= 0 and <= 1. "
"Classification: the response of a particular layer in a neural net to a @PatternList."
"Learning: the desired response of the output layer in a neural net to a @PatternList.")
MAN_END

MAN_BEGIN (U"FFNet: Principal components", U"djmw", 19960918)
INTRO (U"When you select @FFNet and @Eigen the decision planes of layer 1 are drawn in the PC-plane.\n")
MAN_END

MAN_BEGIN (U"FFNet & PatternList: To Categories...", U"djmw", 19960918)
INTRO (U"The @FFNet is used as a classifier. Each pattern from the @PatternList will be "
	"classified into one of the FFNet's categories.")
MAN_END

MAN_BEGIN (U"PatternList & Categories: To FFNet...", U"djmw", 20040422)
INTRO (U"Create a new @FFNet feedforward neural network. "
	"The number of inputs of the newly created FFNet will be equal to the number of "
	"columns in the @PatternList and the number of outputs "
	"will be equal to the number of unique categories in the @Categories.")
ENTRY (U"Settings")
FFNet_Create_COMMON_HELP_HIDDEN
MAN_END

MAN_BEGIN (U"FFNet & PatternList & Categories: Learn slow...", U"djmw", 19960918)
INTRO (U"To learn an association you have to select a @FFNet, a @PatternList and a @Categories object.")
ENTRY (U"Preconditions")
LIST_ITEM (U"The number of columns in a #PatternList must equal the number of input units of #FFNet.")
ENTRY (U" Algorithm")
NORMAL (U"Steepest descent")
ENTRY (U"Preconditions")
LIST_ITEM (U"The number of rows in a #PatternList must equal the number of categories in a #Categories.")
LIST_ITEM (U"The number of unique categories in a #Categories must equal the number of output units in #FFNet.")
MAN_END

MAN_BEGIN (U"FFNet & PatternList & Categories: Learn...", U"djmw", 20040511)
INTRO (U"You can choose this command after selecting one @PatternList, one @Categories and one @FFNet.")
ENTRY (U"Settings")
TAG (U"##Maximum number of epochs")
DEFINITION (U"the maximum number of times that the complete #PatternList dataset will be presented to the neural net.")
TAG (U"##Tolerance of minimizer")
DEFINITION (U"when the difference in costs between two successive learning cycles is "
"smaller than this value, the minimization process will be stopped.")
NORMAL (U"##Cost function")
LIST_ITEM (U"Minimum-squared-error:")
LIST_ITEM (U"  %costs = \\su__%allPatterns_ \\su__%allOutputs_ (%o__%k_ - d__%k_)^2, where")
LIST_ITEM (U"      %o__%k_ : actual output of unit %k")
LIST_ITEM (U"      %d__%k_ : desired output of unit %k")
LIST_ITEM (U"Minimum-cross-entropy:")
LIST_ITEM (U"  %costs = - \\su__%allPatterns_ \\su__%allOutputs_ (%d__%k_ \\.c ln %o__%k_ + (1-%d__%k_) \\.c ln (1-%o__%k_))")
ENTRY (U"Algorithm")
NORMAL (U"The minimization procedure is a variant of conjugate gradient minimization, "
	"see for example @@Press et al. (1992)@, chapter 10, or @@Nocedal & Wright (1999)@, chapter 5.")
MAN_END

MAN_BEGIN (U"FFNet & PatternList & Categories: Get total costs...", U"djmw", 20041118)
INTRO (U"Query the selected @FFNet, @PatternList and @Categories for the total costs.")
ENTRY (U"Algorithm")
NORMAL (U"All patterns are propagated and the total costs are calculated as is shown in @@FFNet & PatternList & Categories: Learn...@. ")
MAN_END

MAN_BEGIN (U"FFNet & PatternList & ActivationList: Get total costs...", U"djmw", 20160524)
INTRO (U"Query the selected @FFNet, @PatternList and @ActivationList for the total costs.")
ENTRY (U"Algorithm")
NORMAL (U"All patterns are propagated and the total costs are calculated as is shown in @@FFNet & PatternList & Categories: Learn...@. ")
MAN_END

MAN_BEGIN (U"FFNet & PatternList & Categories: Get average costs...", U"djmw", 20041118)
INTRO (U"Query the selected @FFNet, @PatternList and @Categories for the average costs.")
ENTRY (U"Algorithm")
NORMAL (U"All patterns are propagated and the total costs are calculated as is shown in @@FFNet & PatternList & Categories: Learn...@. "
	"These total costs are then divided by the number of patterns.")
MAN_END

MAN_BEGIN (U"FFNet & PatternList & ActivationList: Get average costs...", U"djmw", 20160526)
INTRO (U"Query the selected @FFNet, @PatternList and @ActivationList for the average costs.")
ENTRY (U"Algorithm")
NORMAL (U"All patterns are propagated and the total costs are calculated as is shown in @@FFNet & PatternList & Categories: Learn...@. "
	"These total costs are then divided by the number of patterns.")
MAN_END

MAN_BEGIN (U"Anderson (1935)", U"djmw", 20040423)
NORMAL (U"E. Anderson (1935): \"The irises of the Gasp\\e' peninsula.\" "
	"%%Bulletin of the American Iris Society% #59: 2\\--5.")
MAN_END

MAN_BEGIN (U"Fisher (1936)", U"djmw", 19980114)
NORMAL (U"R.A. Fisher (1936): \"The use of multiple measurements in taxonomic "
	"problems.\" %%Annals of Eugenics% #7: 179\\--188.")
MAN_END

MAN_BEGIN (U"Nocedal & Wright (1999)", U"djmw", 20040511)
NORMAL (U"J. Nocedal & S.J. Wright (1999): %%Numerical optimization.% Springer.")
MAN_END
}

/* End of file manual_FFNet.c */
