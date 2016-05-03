/* manual_statistics.cpp
 *
 * Copyright (C) 1992-2011,2014 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ManPagesM.h"

void manual_statistics_init (ManPages me);
void manual_statistics_init (ManPages me) {

MAN_BEGIN (U"Statistics", U"ppgb", 20060506)
INTRO (U"This is the tutorial about basic statistical techniques in Praat, which work "
	"with the @Table object or even directly from the @Goodies menu. It assumes that you are familiar with the @Intro.")
NORMAL (U"(Under construction..................)")
NORMAL (U"Goodies menu:")
LIST_ITEM (U"• @@Difference of two proportions@")
NORMAL (U"For a selected Table:")
LIST_ITEM (U"• @@Logistic regression@")
NORMAL (U"For more sophisticated techniques, see:")
LIST_ITEM (U"• @@Principal component analysis@")
LIST_ITEM (U"• @@Multidimensional scaling@")
LIST_ITEM (U"• @@Discriminant analysis@")
MAN_END

MAN_BEGIN (U"Difference of two proportions", U"ppgb", 20090717)
INTRO (U"This page explains how you compute the significance of a difference between two proportions "
	"with a %\\ci^2 (chi-square) test.")
ENTRY (U"1. Example of normal use")
NORMAL (U"Suppose that you are interested in proving that for a certain experimental participant Task B is easier than Task A."
	"You let the participant perform Task A 110 times, and she turns out to perform this task correctly 71 times. "
	"You also let her perform Task B 120 times, and she performs this task correctly 93 times. "
	"The following table summarizes the results of your experiment:")
CODE1 (U"\t\tCorrect\tIncorrect")
CODE1 (U"\tTask A\t71\t39")
CODE1 (U"\tTask B\t93\t27")
NORMAL (U"The null hypothesis is that both tasks are equally difficult for the participant "
	"and that the probability that she performs Task A correctly is equal to "
	"the probability that she performs Task B correctly.")
NORMAL (U"To compute the probability that the observed proportions are at least as different as 93/120 and 71/110 "
	"if the null hypothesis is true, go to ##Report difference of two proportions# in the @Goodies menu "
	"and fill in the four values 71, 39, 93, and 27. The resulting two-tailed %p is 0.04300, suggesting "
	"that the null hypothesis can be rejected and the two tasks are not equally difficult for the participant "
	"(if the possibility that Task A is easier for her than Task B can be ruled out a priori, "
	"then the resulting one-tailed %p is 0.02150).")
ENTRY (U"2. Example of incorrect use: areal features")
NORMAL (U"An anonymous linguist once proposed that there was a causal relation between blood groups and the incidence "
	"of dental fricatives. He noticed that dental fricatives occurred mainly in languages whose speakers "
	"predominantly had blood group O. To prove his point, he tabulated 100 languages:")
CODE1 (U"\t\t\tHas /θ/ or /ð/\t\tNo dental fricatives")
CODE1 (U"\tGroup O\t\t24\t\t11")
CODE1 (U"\tGroup A or B\t\t29\t\t36")
NORMAL (U"Since %p < 0.05, the linguist regarded his hypothesis as being supported by the facts. "
	"However, this %χ^2 test assumes that the 100 languages are independent, but they are not. "
	"Two adjacent languages tend to correlate in their probability of having dental fricatives, "
	"and their speakers tend to correlate in their blood groups. Both are %%areal features%, "
	"which undermine the independence assumed by the %χ^2 test. The actual null hypothesis "
	"that the test rejected was the combined hypothesis that dental fricatives correlate with blood group "
	"%and that the 100 languages are independent.")
NORMAL (U"Another anonymous linguist proposed that those Limburgian dialects that had lost their tone contrast "
	"compensated this by having larger vowel inventories. He drew up a table of the dialects of 100 villages:")
CODE1 (U"\t\t\tHas tone\t\tHas no tone")
CODE1 (U"\tHas over 25 vowels\t\t10\t\t7")
CODE1 (U"\tHas under 25 vowels\t\t80\t\t3")
NORMAL (U"This result is very significant (%p < 10^^-4^), but only shows that %either there is a relation between "
	"tone and the number of vowels %or that the dialects are not independent. And since adjacent dialects are "
	"arguably dependent both with respect to tone and the number of vowels, the statistical significance does "
	"not allow us to draw any conclusion about the relationship between tone and the number of vowels.")
ENTRY (U"3. Example of problematic use: pooling participants")
NORMAL (U"An anonymous student decided to do the Task A versus Task B experiment described above, "
	"but did not let one participant perform all the 230 tasks. Instead, she let 5 participants perform 46 tasks each "
	"(22 times task A, 24 times Task B). The pooled data were:")
CODE1 (U"\t\tCorrect\tIncorrect")
CODE1 (U"\tTask A\t71\t39")
CODE1 (U"\tTask B\t104\t16")
NORMAL (U"The resulting %p is 0.00016. So what is the conclusion, if the measurements can clearly be dependent? "
	"Well, if the null hypothesis is that all five participants are equally good at Task A as at Task B, "
	"then this hypothesis can be rejected. The conclusion must be that %%these five participants% have on average "
	"more trouble with Task A than with Task B. The student incorrectly concluded, however, that Task A was "
	"more difficult for the average population than Task B. In order to be able to draw such a conclusion, "
	"however, a different test would be required, namely one that takes into account that the five participants "
	"form a random sample from the total population. The simplest such test would be a sign test over "
	"the participants: count those participants who score better on Task A than on Task B and see whether this number "
	"is reliably less than 50 percent of all participants. For five participants, such a sign test "
	"would %never reach significance at a two-tailed 5 percent level (2·0.5^5 = 0.0625).")
ENTRY (U"4. Example of problematic use: pooling participants")
NORMAL (U"Our purpose was to disprove the null hypothesis that listeners' perception does not depend on the language "
	"they think they hear. However, certain vowel tokens acoustically in between the Dutch /ɑ/ and the Dutch /ɔ/ "
	"were perceived 50 percent of the time as /ɑ/ and 50 percent of the time as /ɔ/ when Dutch learners of Spanish thought "
	"they were hearing Dutch, but 60 percent of the time as /ɔ/ when they thought they were hearing Spanish. "
	"The responses of 40 listeners, all of whom underwent both language modes, is combined in the following table:")
CODE1 (U"\t\t/ɑ/\t/ɔ/")
CODE1 (U"\tDutch mode\t200\t200")
CODE1 (U"\tSpanish mode\t160\t240")
NORMAL (U"The result was %p = 0.0056, which reliably showed that these 40 listeners on average shifted their category "
	"boundary toward /ɑ/ when they thought that the language they were listening to was Spanish. "
	"The conclusion is that not all listeners were indifferent to the language mode, "
	"so that mode-dependent perception must exist. The explanation in this case was that the Spanish /a/ "
	"(which Dutch learners of Spanish identify with their /ɑ/) is more auditorily front than Dutch /ɑ/; "
	"in order to reject the null hypothesis that language modes exist but that their direction is random for each "
	"learner, i.e. the population average of the shift is zero, a separate test was required to show that the observed "
	"shift is representative of the population of Dutch learners of Spanish "
	"(this is easier to accomplish for 40 participants than for 5).")
MAN_END

MAN_BEGIN (U"Logistic regression", U"ppgb", 20141001)
INTRO (U"This page explains how you do logistic regression with Praat. "
	"You start by saving a table in a text file (if it contains non-ASCII symbols such as æ or ɛ, "
	"use the UTF-8 or UTF-16 format). "
	"The following example contains natural stimuli (female speaker) with measured F1 and duration values, "
	"and the responses of a certain listener who is presented each stimulus 10 times.")
CODE1 (U" F1    Dur   /æ/   /ɛ/")
CODE1 (U" 764    87    2     8")
CODE1 (U" 674   104    3     7")
CODE1 (U" 574   126    0    10")
CODE1 (U" 566    93    1     9")
CODE1 (U" 618   118    1     9")
CODE1 (U"1025   147   10     0")
CODE1 (U" 722   117    7     3")
CODE1 (U" 696   169    9     1")
CODE1 (U"1024   124   10     0")
CODE1 (U" 752    92    6     4")
NORMAL (U"In this table we see 10 different stimuli, each characterized by a certain combination "
	"of the factors (independent variables) %F1 (first formant in Hertz) and %Dur (duration in milliseconds). "
	"The first row of the table means that there was a stimulus with an F1 of 764 Hz and a duration of 87 ms, "
	"and that the listener responded to this stimulus 2 times with the response category /æ/, "
	"and the remaining 8 times with the category /ɛ/.")
NORMAL (U"A table as above can be typed into a text file. The columns can be separated with spaces and/or tab stops. "
	"The file can be read into Praat with ##Read Table from table file...#. "
	"The command ##To logistic regression...# will become available in the #Statistics menu.")
ENTRY (U"What does it do?")
/*SCRIPT (4.5, 4,
	"Axes... 60 180 900 500\n"
	"Marks bottom every... 1 30 yes yes no\n"
	"Marks left every... 1 50 yes yes no\n"
	"Text bottom... yes Duration (ms)\n"
	"Text left... yes F1 (Hz)\n"
	"mdur_ae = 135\n"
	"mdur_ep = 95\n"
	"sdur = 25\n"
	"mf1_ae = 780\n"
	"mf1_ep = 620\n"
	"sf1 = 60\n"
	"Draw ellipse... mdur_ae-sdur mdur_ae+sdur mf1_ae-sf1 mf1_ae+sf1\n"
	"Text... mdur_ae Centre mf1_ae Half /æ/\n"
	"Draw ellipse... mdur_ep-sdur mdur_ep+sdur mf1_ep-sf1 mf1_ep+sf1\n"
	"Text... mdur_ep Centre mf1_ep Half /ɛ/\n"
	"Draw inner box\n"
)*/
NORMAL (U"The logistic regression method will find values %α, %%β__F1_% and %%β__dur_% "
	"that optimize")
FORMULA (U"%α + %%β__F1_% %F1__%k_ + %%β__dur_% %Dur__%k_ = ln (%p__%k_(/ɛ/)/%p__%k_(/æ/))")
NORMAL (U"where %k runs from 1 to 10, and %p__%k_(/æ/) + %p__%k_(/ɛ/) = 1.")
NORMAL (U"The optimization criterion is %%maximum likelihood%, i.e. those %α, %%β__F1_% and %%β__dur_% "
	"will be chosen that lead to values for %p__%k_(/æ/) and %p__%k_(/ɛ/) that make the observations in the table "
	"most likely.")
NORMAL (U"Praat will create an object of type #LogisticRegression in the list. "
	"When you then click the #Info button, Praat will write the values of %α (the %intercept), "
	"%%β__F1_% and %%β__dur_% into the Info window (as well as much other information).")
NORMAL (U"The number of factors does not have to be 2; it can be 1 or more. "
	"The number of dependent categories is always 2.")
MAN_END

}

/* End of file manual_statistics.cpp */
