/* manual_FFNet.c
 *
 * Copyright (C) 1994-2004 David Weenink
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
 djmw 20020408 GPL
 djmw 20041123 Latest modification
*/

#include "ManPagesM.h"
#include "FFNet.h"

static void drawFFNet_345 (Graphics g)
{
	FFNet me = FFNet_create (3, 4, 0, 5, 0);
	FFNet_drawTopology (me, g);
	forget (me);
}

void manual_FFNet_init (ManPages me);
void manual_FFNet_init (ManPages me)
{

MAN_BEGIN ("epoch", "djmw", 20040428)
INTRO ("A term that is often used in the context of machine learning. An epoch is one complete "
	"presentation of the %%data set to be learned% to a learning machine.")
NORMAL ("Learning machines like @@FFNet|feedforward neural nets@ that use iterative algorithms "
	"often need many epochs during their learning phase.")
NORMAL ("A @@Discriminant|discriminant classifier@ is also a learning machine. "
	"However, in contrast with neural nets a discriminant classifier only needs one epoch to learn.")
MAN_END

MAN_BEGIN ("Feedforward neural networks", "djmw", 20040511)
INTRO ("This tutorial describes the use of @FFNet feedforward neural networks in P\\s{RAAT}. ")
NORMAL ("@@Feedforward neural networks 1. What is a feedforward neural network?|1. What is a feedforward neural network?@")
LIST_ITEM ("  @@Feedforward neural networks 1.1. The learning phase|1.1 The learning phase")
LIST_ITEM ("  @@Feedforward neural networks 1.2. The classification phase|1.2 The classification phase")
NORMAL ("@@Feedforward neural networks 2. Quick start|2. Quick start@")
NORMAL ("@@Feedforward neural networks 3. FFNet versus discriminant classifier|3. FFNet versus discriminant classifier@")
NORMAL ("@@Feedforward neural networks 4. Command overview|4. Command overview@")
MAN_END

MAN_BEGIN ("Feedforward neural networks 1. What is a feedforward neural network?", "djmw", 20040426)
INTRO ("A feedforward neural network is a biologically inspired classification algorithm. "
	"It consist of a (possibly large) number of simple neuron-like processing %units, organized in %layers. "
	"Every unit in a layer is connected with all the units in the previous layer. "
	"These connections are not all equal, each connection may have a different strength or %weight. "
	"The weights on these connections encode the knowledge of a network. "
	"Often the units in a neural network are also called %nodes.")
NORMAL ("Data enters at the inputs and passes through the network, layer by layer, until it arrives at the outputs. "
	"During normal operation, that is when it acts as a classifier, there is no feedback between layers. "
	"This is why they are called %%feedforward% neural networks. ")
NORMAL ("In the following figure we see an example of a 2-layered network with, from top to bottom: "
	"an output layer with 5 units, a %hidden layer with 4 units, respectively. The network has 3 input units.")
PICTURE (5, 5, drawFFNet_345)
NORMAL ("The 3 inputs are shown as circles and these do not belong to any layer of the network (although the inputs "
	"sometimes are considered as a virtual layer with layer number 0). Any layer that is not an output layer is a "
	"%hidden layer. This network therefore has 1 hidden layer and 1 output layer. The figure also shows all the "
	"connections between the units in different layers. A layer only connects to the previous layer. ")
NORMAL ("The operation of this network can be divided into two phases:")
NORMAL ("@@Feedforward neural networks 1.1. The learning phase|1. The learning phase")
NORMAL ("@@Feedforward neural networks 1.2. The classification phase|2. The classification phase")
MAN_END

MAN_BEGIN ("Feedforward neural networks 1.1. The learning phase", "djmw", 20040428)
INTRO ("During the learning phase the weights in the FFNet will be modified. "
	"All weights are modified in such a way that when a pattern is presented, "
	"the output unit with the correct category, hopefully, will have the largest output value.")
ENTRY ("How does learning take place?")
NORMAL ("The FFNet uses a %supervised learning algorithm: besides the input pattern, "
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
	"This is why the term %%backpropagation network% is also often used to describe this type of neural network.")
NORMAL ("If you perform the procedure above once for every pattern and category pair in your data "
	"set you have have performed 1 @epoch of learning.")
NORMAL ("The hope is that eventually, probably after many epochs, "
	"the neural net will remember these pattern-category pairs. "
	"You even hope that the neural net when the learning phase has terminated, will be able to %generalize "
	"and has learned to "
	"@@FFNet & Pattern: To Categories...|classify@ correctly any unknown pattern presented to it. ")
NORMAL ("Because real-life data many times contains noise as well as partly contradictory information "
	"these hopes can only be partly fulfilled. ")
NORMAL ("For @@FFNet & Pattern & Categories: Learn...|learning@ you "
	"need to select 3 different objects together: a FFNet (the %classifier), "
	"a Pattern (the %inputs) and a Categories (the %%correct outputs%).")
ENTRY ("How long will the learning phase take?")
NORMAL ("In general this question is hard to answer. It depends on the size of the neural network, "
	"the number of patterns to be learned, the number of epochs, the tolerance of the minimizer "
	"and the speed of your computer, how much computing time the learning phase may take. ")
NORMAL ("If computing time becomes excessive in your interactive environment then consider using the "
	"powerful @@Scripting|scripting@ facilities in P\\s{RAAT} to process your learning job as a batch job. ")
MAN_END

MAN_BEGIN ("Feedforward neural networks 1.2. The classification phase", "djmw", 20040428)
INTRO ("In the classification phase the weights of the network are fixed. ")
NORMAL ("A pattern, presented at the inputs, will be transformed from layer to layer until it reaches the output layer. "
	"Now classification can occur by selecting the category associated with the output unit that has "
	"the largest output value.  "
	"For classification we only need to select an FFNet and a Pattern together and "
	"choose  @@FFNet & Pattern: To Categories...|To Categories...@. ")
NORMAL ("In contrast to the @@Feedforward neural networks 1.1. The learning phase|learning phase@ classification is very fast.")
MAN_END

MAN_BEGIN ("Feedforward neural networks 2. Quick start", "djmw", 20040426)
INTRO ("You may create the iris example set with the @@Create iris example...@ command "
	"that you will find under the ##Neural nets# option in the #New menu. "
	"Three new objects will appear in the @@List of Objects@: a @FFNet, a @Categories and "
	"a @Pattern.")
NORMAL ("The #Pattern contains the @@iris data set@ in a 150 rows by 4 columns matrix. "
	"To  guarantee that every cell in the Pattern is in the [0,1] interval, all measurement "
	"values were divided by 10. In the #Categories the three iris species %setosa, "
	"%versicolor, and %virginica were categorized with the numbers #1, #2 and #3, respectively. "
	"Because there are 4 data columns in the Pattern and 3 different iris species in the Categories, "
	"the newly created #FFNet has 4 inputs and 3 outputs. "
	"If you have entered a positive number in one of the fields in the form, the FFNet will have "
	"this number of units in a %%hidden layer%. The name of the newly created FFNet "
	"will reflect its topology. If you did opt for the default, 0 hidden units, the FFNet will be named 4-3.")
ENTRY ("Learning the iris data")
NORMAL ("The first thing you probably might want to do is to let the #FFNet learn the association in "
	"each pattern-category pair.  To do this select all three objects together and choose "
	"@@FFNet & Pattern & Categories: Learn...|Learn...@. "
	"A form will appear, asking you to supply some settings for "
	"the learning algorithm. Learning starts after you have clicked the OK-button. "
	"Since the example network does not have too many weights that need to be adjusted and "
	"the learning data set is very small and computers nowadays are very fast, this will only take a "
	"very short time.")
ENTRY ("Classify")
NORMAL ("Now, if you are curious how well the FFNet has learned the iris data, you may select the "
	"#FFNet and the #Pattern together and choose @@FFNet & Pattern: To Categories...|To Categories...@. "
	"A new #Categories appears in the ##List of Objects# with the name %4-3_iris (if %%4-3% was the name of the FFNet and %iris% the name of the Pattern). "
	"We have two different Categories in the list of objects, the topmost one has the original categories, the other "
	"the categories as were assigned by the FFNet classifier. The obvious thing to do now is to compare the "
	"original categories with the assigned categories by making a @@Confusion|confusion table@. "
	"Select the two #Categories and choose @@Categories: To Confusion|To Confusion@ and a newly "
	"created @Confusion appears. Pressing the @Info button will show you an info window with, "
	"among others, the fraction correct. ")
NORMAL ("You might also want to "
	"@@Feedforward neural networks 3. FFNet versus discriminant classifier|compare the FFNet classifier with a discriminant classifier@.")
ENTRY ("Create other neural net topologies")
NORMAL ("With a #Pattern and a #Categories selected together, you can for example create a new #FFNet of a different topology.")
MAN_END

MAN_BEGIN ("Feedforward neural networks 3. FFNet versus discriminant classifier", "djmw", 20040426)
NORMAL ("You might want to compare the FFNet classifier with a discriminant classifier. "
	"Unlike the FFNet, a @@Discriminant|discriminant@ classifier does not need any iterative procedure in the "
	"learning phase and can be used immediately after creation for classification. "
	"The following three simple steps will give you the confusion matrix based on discriminant analysis:")
LIST_ITEM ("1. Select the Pattern and the Categories together and choose ##To Discriminant#.  "
	"A newly created Discriminant will appear.")
LIST_ITEM ("2. Select the Discriminant and the Pattern together and choose ##To Categories...#. A newly created @Categories will appear.")
LIST_ITEM ("3. Select the two appropriate Categories and choose @@categories: To Confusion|To Confusion@.  "
	"A newly created @Confusion will appear. After pushing the @Info button, the info window will "
	"show you the fraction correct.")
NORMAL ("See also the @@Discriminant analysis@ tutorial for more information.")
MAN_END

MAN_BEGIN ("Feedforward neural networks 4. Command overview", "djmw", 20040426)
INTRO ("FFNet commands")
ENTRY ("Creation:")
LIST_ITEM ("\\bu @@Pattern & Categories: To FFNet...@")
LIST_ITEM ("\\bu @@Create FFNet...@")
ENTRY ("Learning:")
LIST_ITEM ("\\bu @@FFNet & Pattern & Categories: Learn...@")
LIST_ITEM ("\\bu @@FFNet & Pattern & Categories: Learn slow...@")
ENTRY ("Classification:")
LIST_ITEM ("\\bu @@FFNet & Pattern: To Categories...@")
ENTRY ("Drawing:")
LIST_ITEM ("\\bu @@FFNet: Draw topology@")
LIST_ITEM ("\\bu @@FFNet: Draw weights...@")
LIST_ITEM ("\\bu @@FFNet: Draw cost history...@")
ENTRY ("Queries")
LIST_ITEM ("\\bu @@FFNet & Pattern & Categories: Get total costs...@")
LIST_ITEM ("\\bu @@FFNet & Pattern & Categories: Get average costs...@")
LIST_ITEM ("\\bu @@FFNet & Pattern & Activation: Get total costs...@")
LIST_ITEM ("\\bu @@FFNet & Pattern & Activation: Get average costs...@")
ENTRY ("Analysis:")
LIST_ITEM ("\\bu ##FFNet & Pattern: To Activation...#")
ENTRY ("Modification:")
LIST_ITEM ("\\bu @@FFNet: Reset...@")
LIST_ITEM ("\\bu ##FFNet: Select biases...#")
LIST_ITEM ("\\bu ##FFNet: Select all weights#")
MAN_END

MAN_BEGIN ("FFNet", "djmw", 19961015)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}.")
NORMAL ("A #FFNet models a feedforward neural net. A feedforward "
	"neural net can %learn associations between its %input and its %output. "
	"The @@Feedforward neural networks@ tutorial gives you an introduction to feedforward neural nets.")
MAN_END

MAN_BEGIN ("FFNet: Draw topology", "djmw", 19970218)
INTRO ("You can choose this command after selecting 1 or more @FFNet's.")
ENTRY ("Behaviour")
NORMAL ("Draws all units and all connections of a feedforward neural net.")
MAN_END

MAN_BEGIN ("FFNet: Draw weights...", "djmw", 20040422)
INTRO ("Draws the weights in a layer of the selected @FFNet feedforward neural net.")
ENTRY ("Arguments")
TAG ("%%Layer number%")
DEFINITION ("determines the layer.")
TAG ("%Garnish")
DEFINITION ("determines whether additional information is drawn.")
ENTRY ("Behaviour")
NORMAL ("The weights are arranged in a matrix. The columns of this matrix are indexed by the units in the layer, "
	"while the rows are indexed by the units in the previous layer. There is one extra row for the biases."
	"The values of the weights are shown as rectangles. The area of a rectangle is proportional "
	"to the value.  Negative values are shown as filled black rectangles.")
MAN_END

MAN_BEGIN ("FFNet: Draw cost history...", "djmw", 19970218)
INTRO ("You can choose this command after selecting 1 or more @FFNet's.")
ENTRY ("Arguments")
TAG ("%%Iteration range%")
DEFINITION ("determine the horizontal range of the plot.")
TAG ("%%Cost range%")
DEFINITION ("determine the vertical range of the plot.")
TAG ("%Garnish")
DEFINITION ("determines whether a box and axis labels are drawn.")
ENTRY ("Behaviour")
NORMAL ("Draws the history of the cost versus iteration number during previous learning.")
MAN_END

MAN_BEGIN ("FFNet: Extract weights...", "djmw", 20040422)
INTRO ("Extract all the weights, from all the units in the specified layer of the selected "
	"@FFNet, to a @TableOfReal.")
ENTRY ("Arguments")
TAG ("%%Layer number%")
DEFINITION ("determines the layer.")
ENTRY ("Behaviour")
NORMAL ("The weights will be arranged in the TableOfReal as follows: ")
NORMAL ("The table columns will be indexed by the unit numbers in the selected layer, "
	"while the rows will be indexed by the unit numbers from the previous layer. "
	"There will be one extra row to accommodate the bias weights. "
	"The rows and columns are labelled with layer number and unit number as \"L%i-%j\", where %i is the layer number and "
	"%j the unit number from that layer. The layer number for the rows is one less than the layer number in the columns. "
	"The last row is labelled as \"Bias\".")
MAN_END

MAN_BEGIN ("FFNet: Get number of outputs", "djmw", 20040420)
INTRO ("Queries the selected @FFNet for the number of output units in the output layer. ")
MAN_END

MAN_BEGIN ("FFNet: Get number of inputs", "djmw", 20040420)
INTRO ("Queries the selected @FFNet for the number of inputs. ")
NORMAL ("For a network with only one layer, the inputs are connected directly to the output layer. "
	"In a two-layer network the inputs are connected to a hidden layer.")
MAN_END


MAN_BEGIN ("FFNet: Get number of hidden units...", "djmw", 20040420)
INTRO ("Queries the selected @FFNet for the number of units in a hidden layer.")
ENTRY ("Argument")
TAG ("%%Hidden layer number%")
DEFINITION ("determines the layer that is queried.")
ENTRY ("Layer numbering")
NORMAL (	"The number of hidden layers is always one less than the total number of layers in a FFNet. "
	"A network with the output units connected to the inputs therefore has only 1 layer, the output layer and "
	"no hidden layers. ")
MAN_END

MAN_BEGIN ("FFNet: Get number of hidden weights...", "djmw", 20040420)
INTRO ("Queries the selected @FFNet for the number of weights in a hidden layer.")
ENTRY ("Argument")
TAG ("%%Hidden layer number%")
DEFINITION ("determines the layer that is queried.")
MAN_END

MAN_BEGIN ("FFNet: Reset...", "djmw", 20040420)
INTRO ("You can choose this command after selecting 1 or more @FFNet's.")
ENTRY ("WARNING")
NORMAL ("This command destroys all previous learning.")
ENTRY ("Arguments")
TAG ("%%Range")
DEFINITION ("determines the upper limit of  the [-%range, +%range] interval from "
	"which new weights will be randomly selected.")
ENTRY ("Behaviour")
NORMAL ("All (selected) weights are reset to random numbers uniformly drawn from the interval [-%range, +%range]. "
	"This command also clears the cost history.")
MAN_END

MAN_BEGIN ("FFNet: Select biases...", "djmw", 20040422)
INTRO ("Selects only the biases in one particular layer as subject for modification during learning of the @FFNet.")
ENTRY ("Argument")
TAG ("%%Layer number%")
DEFINITION ("determines the layer whose biases will be modified.")
ENTRY ("Behaviour")
NORMAL ("This command induces very specific behaviour during a following learning phase. "
	"Instead of all the weights, only the biases in the specified layer will be changed during learning and the "
	"rest of the weights stay fixed. ")
MAN_END

#define FFNet_Create_COMMON_HELP_INOUT \
ENTRY ("Arguments")\
TAG ("%%Number of inputs%")\
DEFINITION ("the dimension of the input of the neural net.")\
TAG ("%%Number of outputs (\\>_ 1)%")\
DEFINITION ("the number of different categories that you want the net to learn.")

#define FFNet_Create_COMMON_HELP_HIDDEN \
TAG ("%%Number of units in hidden layer 1, Number of units in hidden layer 2%") \
DEFINITION ("determine the number of units in the hidden layers. " \
	"If you want a neural net with no hidden layers, both numbers have to be 0. "\
	"If you want a neural net with only 1 hidden layer then one of these numbers has to differ from 0. ")

MAN_BEGIN ("Create FFNet...", "djmw", 20040420)
INTRO ("Create a new feedforward neural net of type @FFNet.")
FFNet_Create_COMMON_HELP_INOUT
FFNet_Create_COMMON_HELP_HIDDEN
MAN_END

MAN_BEGIN ("Create FFNet (linear outputs)...", "djmw", 20040422)
INTRO ("Create a @FFNet feedforward neural network whose output units are linear.")
FFNet_Create_COMMON_HELP_INOUT
FFNet_Create_COMMON_HELP_HIDDEN
MAN_END

MAN_BEGIN ("Create iris example...", "djmw", 20040423)
INTRO ("A @FFNet feedforward neural net will be created together with two other objects: "
	"a @Pattern and a @Categories. The Pattern will contain the observations in the @@iris data set@, "
	"and the Categories will contain the 3 different iris species categorized by numbers.")
ENTRY ("Arguments")
FFNet_Create_COMMON_HELP_HIDDEN
NORMAL ("For this simple data you can leave both hidden layers empty.")

MAN_END

MAN_BEGIN ("iris data set", "djmw", 19961015)
NORMAL ("A data set with 150 random samples of flowers from the iris species %setosa, "
	"%versicolor, and %virginica collected by @@Anderson (1935)@. From each species there are 50 observations for "
	"sepal length, sepal width, petal length, and petal width in cm. This dataset was "
	"used by @@Fisher (1936)@ in his initiation of the linear-discriminant-function technique.")
MAN_END

MAN_BEGIN ("FFNet: Pattern", "djmw", 19960918)
INTRO ("A @Pattern is a @Matrix in which each row forms one input pattern (vector) for the neural net.")
NORMAL ("The number of columns is the dimensionality of the input. "
"The number of rows is the number of patterns.")
MAN_END

MAN_BEGIN ("FFNet: Categories", "djmw", 19960918)
INTRO ("The categories for training a neural net with a @Pattern. ")
ENTRY ("Preconditions")
NORMAL ("The number of categories in a @Categories must equal the number of rows in #Pattern.")
MAN_END

MAN_BEGIN ("Activation", "djmw", 20041118)
INTRO ("A @Matrix whose elements must be >= 0 and <= 1. "
"Classification: the response of a particular layer in a neural net to a @Pattern."
"Learning: the desired response of the output layer in a neural net to a @Pattern.")
MAN_END

MAN_BEGIN ("FFNet: Principal components", "djmw", 19960918)
INTRO ("When you select @FFNet and @Eigen the decision planes of layer 1 are drawn in the PC-plane.\n")
MAN_END

MAN_BEGIN ("FFNet & Pattern: To Categories...", "djmw", 19960918)
INTRO ("The @FFNet is used as a classifier. Each pattern from the @Pattern will be "
	"classified into one of the FFNet's categories.")
MAN_END

MAN_BEGIN ("Pattern & Categories: To FFNet...", "djmw", 20040422)
INTRO ("Create a new @FFNet feedforward neural network. "
	"The number of inputs of the newly created FFNet will be equal to the number of "
	"columns in the @Pattern and the number of outputs "
	"will be equal to the number of unique categories in the @Categories.")
ENTRY ("Arguments")
FFNet_Create_COMMON_HELP_HIDDEN
MAN_END

MAN_BEGIN ("FFNet & Pattern & Categories: Learn slow...", "djmw", 19960918)
INTRO ("To learn an association you have to select a @FFNet, a @Pattern and a @Categories object.")
ENTRY ("Preconditions")
LIST_ITEM ("The number of columns in a #Pattern must equal the number of input units of #FFNet.")
ENTRY (" Algorithm")
NORMAL ("Steepest descent")
ENTRY ("Preconditions")
LIST_ITEM ("The number of rows in a #Pattern must equal the number of categories in a #Categories.")
LIST_ITEM ("The number of unique categories in a #Categories must equal the number of output units in #FFNet.")
MAN_END

MAN_BEGIN ("FFNet & Pattern & Categories: Learn...", "djmw", 20040511)
INTRO ("You can choose this command after selecting one @Pattern, one @Categories and one @FFNet.")
ENTRY ("Arguments")
TAG ("%%Maximum number of epochs%")
DEFINITION ("the maximum number of times that the complete #Pattern dataset will be presented to the neural net.")
TAG ("%%Tolerance of minimizer%")
DEFINITION ("when the difference in costs between two successive learning cycles is "
"smaller than this value, the minimization process will be stopped.")
NORMAL ("%%Cost function%")
LIST_ITEM ("Minimum-squared-error:")
LIST_ITEM ("  %costs = \\su__%allPatterns_ \\su__%allOutputs_ (%o__%k_ - d__%k_)^2, where")
LIST_ITEM ("      %o__%k_ : actual output of unit %k")
LIST_ITEM ("      %d__%k_ : desired output of unit %k")
LIST_ITEM ("Minimum-cross-entropy:")
LIST_ITEM ("  %costs = - \\su__%allPatterns_ \\su__%allOutputs_ (%d__%k_ \\.c ln %o__%k_ + (1-%d__%k_) \\.c ln (1-%o__%k_))")
ENTRY ("Algorithm")
NORMAL ("The minimization procedure is a variant of conjugate gradient minimization, "
	"see for example @@Press et al. (1992)@, chapter 10, or @@Nocedal & Wright (1999)@, chapter 5.")
MAN_END

MAN_BEGIN ("FFNet & Pattern & Categories: Get total costs...", "djmw", 20041118)
INTRO ("Query the selected @FFNet, @Pattern and @Categories for the total costs.")
ENTRY ("Algorithm")
NORMAL ("All patterns are propagated and the total costs are calculated as is shown in @@FFNet & Pattern & Categories: Learn...@. ")
MAN_END

MAN_BEGIN ("FFNet & Pattern & Activation: Get total costs...", "djmw", 20041118)
INTRO ("Query the selected @FFNet, @Pattern and @Activation for the total costs.")
ENTRY ("Algorithm")
NORMAL ("All patterns are propagated and the total costs are calculated as is shown in @@FFNet & Pattern & Categories: Learn...@. ")
MAN_END

MAN_BEGIN ("FFNet & Pattern & Categories: Get average costs...", "djmw", 20041118)
INTRO ("Query the selected @FFNet, @Pattern and @Categories for the average costs.")
ENTRY ("Algorithm")
NORMAL ("All patterns are propagated and the total costs are calculated as is shown in @@FFNet & Pattern & Categories: Learn...@. "
	"These total costs are then divided by the number of patterns.")
MAN_END

MAN_BEGIN ("FFNet & Pattern & Activation: Get average costs...", "djmw", 20041118)
INTRO ("Query the selected @FFNet, @Pattern and @Activation for the average costs.")
ENTRY ("Algorithm")
NORMAL ("All patterns are propagated and the total costs are calculated as is shown in @@FFNet & Pattern & Categories: Learn...@. "
	"These total costs are then divided by the number of patterns.")
MAN_END

MAN_BEGIN ("Anderson (1935)", "djmw", 20040423)
NORMAL ("E. Anderson (1935), \"The irises of the Gasp\\e' peninsula\", "
	"%%Bulletin of the American Iris Society% #59, 2-5.")
MAN_END

MAN_BEGIN ("Fisher (1936)", "djmw", 19980114)
NORMAL ("R.A. Fisher (1936), \"The use of multiple measurements in taxonomic "
	"problems\", %%Annals of Eugenics% #7, 179-188.")
MAN_END

MAN_BEGIN ("Nocedal & Wright (1999)", "djmw", 20040511)
NORMAL ("J. Nocedal & S.J. Wright, %%Numerical optimization%, Springer, 1999.")
MAN_END
}

/* End of file manual_FFNet.c */
