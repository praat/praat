/* manual_gram.cpp
 *
 * Copyright (C) 1997-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "ManPagesM.h"
#include "OTGrammar.h"

static void draw_NoCoda_pat (Graphics g) {
	OTGrammar ot = OTGrammar_create_NoCoda_grammar ();
	OTGrammar_drawTableau (ot, g, false, L"pat");
	forget (ot);
}
static void draw_NoCoda_pa (Graphics g) {
	OTGrammar ot = OTGrammar_create_NoCoda_grammar ();
	OTGrammar_drawTableau (ot, g, false, L"pa");
	forget (ot);
}
static void draw_NoCoda_reverse (Graphics g) {
	OTGrammar ot = OTGrammar_create_NoCoda_grammar ();
	ot -> index [1] = 2;
	ot -> index [2] = 1;
	OTGrammar_drawTableau (ot, g, false, L"pat");
	forget (ot);
}
static void draw_NPA_assimilate_anpa (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 1;
	ot -> index [3] = 2;
	OTGrammar_drawTableau (ot, g, false, L"an+pa");
	forget (ot);
}
static void draw_NPA_assimilate_atma (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 1;
	ot -> index [3] = 2;
	OTGrammar_drawTableau (ot, g, false, L"at+ma");
	forget (ot);
}
static void draw_NPA_faithful_anpa (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 2;
	ot -> index [3] = 1;
	OTGrammar_drawTableau (ot, g, false, L"an+pa");
	forget (ot);
}
static void draw_NPA_faithful_atma (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 2;
	ot -> index [3] = 1;
	OTGrammar_drawTableau (ot, g, false, L"at+ma");
	forget (ot);
}
static void draw_Wolof_ItI (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, false, L"\\ict\\ic");
	forget (ot);
}
static void draw_Wolof_itE (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, false, L"it\\ef");
	forget (ot);
}
static void draw_Wolof_etE (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, false, L"et\\ef");
	forget (ot);
}
static void draw_Wolof_schwatschwa (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, false, L"\\swt\\sw");
	forget (ot);
}
static void draw_Infant_swtI (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 3);
	ot -> constraints [1]. disharmony = 3;
	ot -> constraints [2]. disharmony = 4;
	ot -> constraints [3]. disharmony = 2;
	ot -> constraints [4]. disharmony = 1;
	ot -> constraints [5]. disharmony = 5;
	OTGrammar_sort (ot);
	OTGrammar_drawTableau (ot, g, false, L"\\swt\\ic");
	forget (ot);
}

void manual_gram_init (ManPages me);
void manual_gram_init (ManPages me) {

MAN_BEGIN (L"constraints", L"ppgb", 20021105)
INTRO (L"In @@Optimality Theory@, the `rules' that an output form has to satisfy. Since there can be many constraints "
	"and these constraints can conflict with each other, the constraints are %violable and the highest-ranked constraints "
	"have the largest say in determining the optimal output.")
NORMAL (L"See the @@OT learning@ tutorial for many examples.")
MAN_END

MAN_BEGIN (L"Create tongue-root grammar...", L"ppgb", 20021204)
INTRO (L"A command in the @@New menu@ for creating an @OTGrammar object with a tongue-root-harmony grammar.")
NORMAL (L"These OTGrammar grammars only accept inputs of the form V__1_tV__2_, where V__1_ and V__2_ are "
	"chosen from the six front vowels i, \\ic, e, \\ef, \\sw, and a. In a text field, these "
	"vowels should be typed as $$##i#$, $$##\\bsic#$, $$##e#$, $$##\\bsep#$, $$##\\bssw#$, "
	"and $$##a#$, respectively (see @@Special symbols@).")
NORMAL (L"The following phonological features are relevant:")
LIST_ITEM (L"\t\tATR\tRTR")
LIST_ITEM (L"\thigh\ti\t\\ic")
LIST_ITEM (L"\tmid\te\t\\ef")
LIST_ITEM (L"\tlow\t\\sw\ta")
ENTRY (L"Constraints")
NORMAL (L"The resulting OTGrammar will usually contain at least the following five constraints:")
TAG (L"*[rtr / hi]")
DEFINITION (L"\"do not implement [retracted tongue root] if the vowel is high.\"")
TAG (L"*[atr / lo]")
DEFINITION (L"\"do not implement [advanced tongue root] if the vowel is low.\"")
TAG (L"P\\s{ARSE} (rtr)")
DEFINITION (L"\"make an underlying [retracted tongue root] specification surface.\"")
TAG (L"P\\s{ARSE} (atr)")
DEFINITION (L"\"make an underlying [advanced tongue root] specification surface.\"")
TAG (L"*G\\s{ESTURE} (contour)")
DEFINITION (L"\"do not go from advanced to retracted tongue root, nor the other way around, within a word.\"")
NORMAL (L"This set of constraints thus comprises: ")
LIST_ITEM (L"\\bu two %%##grounding conditions#% (@@Archangeli & Pulleyblank (1994)@), "
	"which we can see as gestural constraints;")
LIST_ITEM (L"\\bu two %%##faithfulness constraints#%, which favour the similarity between input and output, "
	"and can be seen as implementing the principle of maximization of perceptual contrast;")
LIST_ITEM (L"\\bu a %%##harmony constraint#%, which, if crucially ranked higher than at least one faithfulness constraint, "
	"forces %%##tongue-root harmony#%.")
NORMAL (L"In addition, there may be the following four constraints:")
TAG (L"*[rtr / mid]")
DEFINITION (L"\"do not implement [retracted tongue root] if the vowel is mid; universally ranked lower "
	"than *[rtr / hi].\"")
TAG (L"*[rtr / lo]")
DEFINITION (L"\"do not implement [retracted tongue root] if the vowel is low; universally ranked lower "
	"than *[rtr / mid].\"")
TAG (L"*[atr / mid]")
DEFINITION (L"\"do not implement [advanced tongue root] if the vowel is mid; universally ranked lower "
	"than *[atr / lo].\"")
TAG (L"*[atr / hi]")
DEFINITION (L"\"do not implement [advanced tongue root] if the vowel is high; universally ranked lower "
	"than *[atr / mid].\"")
NORMAL (L"The universal rankings referred to are due to the %%##local-ranking principle#% (@@Boersma (1998)@). "
	"A learning algorithm may enforce this principle, e.g., if *[rtr / hi] falls down the ranking scale, "
	"*[rtr / mid] may be pushed along.")
NORMAL (L"For information on learning these tongue-root grammars, see @@OT learning@ "
	"and @@Boersma (2000)@.")
MAN_END

MAN_BEGIN (L"Optimality Theory", L"ppgb", 20021105)
INTRO (L"A framework for transferring one linguistic representation into another, "
	"e.g. transferring an underlying form into a surface form. Before @@Prince & Smolensky (1993)@, "
	"phonologists tended to this with a sequentially ordered set of rules, each of which transferred a representation "
	"into another. With @OT (that's the abbreviation), there are no intermediate steps in the derivation, but a set of ranked "
	"@constraints chooses the optimal output form from a set of candidates.")
NORMAL (L"In Praat, you can draw Optimality-Theoretic tableaus and simulate Optimality-Theoretic learning. "
	"See the @@OT learning@ tutorial.")
MAN_END

MAN_BEGIN (L"OT", L"ppgb", 20021105)
INTRO (L"An abbreviation for @@Optimality Theory@.")
MAN_END

MAN_BEGIN (L"OT learning", L"ppgb", 20070423)
INTRO (L"This tutorial describes how you can draw Optimality-Theoretic and Harmonic-Grammar tableaus and "
	"simulate Optimality-Theoretic and Harmonic-Grammar learning with Praat.")
NORMAL (L"You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM (L"1. @@OT learning 1. Kinds of grammars|Kinds of grammars@ (ordinal and stochastic, @OTGrammar)")
LIST_ITEM (L"2. @@OT learning 2. The grammar|The grammar@")
LIST_ITEM1 (L"2.1. @@OT learning 2.1. Viewing a grammar|Viewing a grammar@ (N\\s{O}C\\s{ODA} example, @OTGrammarEditor)")
LIST_ITEM1 (L"2.2. @@OT learning 2.2. Inside the grammar|Inside the grammar@ (saving, inspecting)")
LIST_ITEM1 (L"2.3. @@OT learning 2.3. Defining your own grammar|Defining your own grammar@")
LIST_ITEM1 (L"2.4. @@OT learning 2.4. Evaluation|Evaluation@ (noise)")
LIST_ITEM1 (L"2.5. @@OT learning 2.5. Editing a grammar|Editing a grammar@")
LIST_ITEM1 (L"2.6. @@OT learning 2.6. Variable output|Variable output@ (place assimilation example)")
LIST_ITEM1 (L"2.7. @@OT learning 2.7. Tableau pictures|Tableau pictures@ (printing, EPS)")
LIST_ITEM1 (L"2.8. @@OT learning 2.8. Asking for one output|Asking for one output@")
LIST_ITEM1 (L"2.9. @@OT learning 2.9. Output distributions|Output distributions@")
LIST_ITEM (L"3. @@OT learning 3. Generating language data|Generating language data@")
LIST_ITEM1 (L"3.1. @@OT learning 3.1. Data from a pair distribution|Data from a pair distribution@")
LIST_ITEM1 (L"3.2. @@OT learning 3.2. Data from another grammar|Data from another grammar@ (tongue-root-harmony example)")
LIST_ITEM (L"4. @@OT learning 4. Learning an ordinal grammar|Learning an ordinal grammar@")
LIST_ITEM (L"5. @@OT learning 5. Learning a stochastic grammar|Learning a stochastic grammar@")
LIST_ITEM (L"6. @@OT learning 6. Shortcut to grammar learning|Shortcut to grammar learning@")
LIST_ITEM (L"7. @@OT learning 7. Learning from overt forms|Learning from overt forms@")
MAN_END

MAN_BEGIN (L"OT learning 1. Kinds of grammars", L"ppgb", 20100330)
INTRO (L"This is chapter 1 of the @@OT learning@ tutorial.")
NORMAL (L"According to @@Prince & Smolensky (1993)@, an @@Optimality Theory|Optimality-Theoretic@ (@OT) grammar "
	"consists of a number of ranked @constraints. "
	"For every possible input (usually an underlying form), GEN (the generator) generates a (possibly very large) number of "
	"%%output candidates%, and the ranking order of the constraints determines the winning candidate, "
	"which becomes the single optimal output.")
NORMAL (L"According to @@Prince & Smolensky (1993)@ and @@Smolensky & Legendre (2006)@, a Harmonic Grammar (HG) "
	"consists of a number of weighted @constraints. "
	"The winning candidate, which becomes the single optimal output, is the one with the greatest %harmony, which "
	"is a measure of goodness determined by the weights of the constraints violated by each candidate.")
NORMAL (L"In OT, ranking is %strict, i.e., if a constraint %A is ranked higher than the constraints %B, %C, and %D, "
	"a candidate that violates only constraint %A will always be beaten by any candidate that respects %A "
	"(and any higher constraints), even if it violates %B, %C, and %D.")
NORMAL (L"In HG, weighting is %additive, i.e., a candidate that only violates a constraint %A with a weight of 100 "
	"has a harmony of -100 and will therefore beat a candidate that violates both a constraint %B with a weight of 70 "
	"and a constraint %C with a weight of 40 and therefore has a harmony of only -110. Also, two violations of constraint %B "
	"(harmony 2 * -70 = -140) are worse than one violation of constraint %A (harmony -100).")
ENTRY (L"1. Ordinal OT grammars")
NORMAL (L"Because only the ranking order of the constraints plays a role in evaluating the output candidates, "
	"Prince & Smolensky took an OT grammar to contain no absolute ranking values, i.e., they accepted only an ordinal relation "
	"between the constraint rankings. For such a grammar, @@Tesar & Smolensky (1998)@ devised an on-line learning algorithm "
	"(Error-Driven Constraint Demotion, EDCD) that changes the ranking order "
	"whenever the form produced by the learner is different from the adult form "
	"(a corrected version of the algorithm can be found in @@Boersma (2009b)@). Such a learning step "
	"can sometimes lead to a large change in the behaviour of the grammar.")
ENTRY (L"2. Stochastic OT grammars")
NORMAL (L"The EDCD algorithm is fast and convergent. As a model of language acquisition, however, its drawbacks are that it "
	"is extremely sensitive to errors in the learning data and that it does not show realistic gradual learning curves. "
	"For these reasons, @@Boersma (1997)@ "
	"proposed stochastic OT grammars in which every constraint has a %%ranking value% along a continuous ranking scale, "
	"and a small amount of %noise is added to this ranking value at evaluation time. "
	"The associated error-driven on-line learning algorithm (Gradual Learning Algorithm, GLA) effects small changes in the "
	"ranking values of the constraints with every learning step. An added virtue of the GLA is that it can learn "
	"languages with optionality and variation, which was something that EDCD could not do. "
	"For how this algorithm works on some traditional phonological problems, see @@Boersma & Hayes (2001)@.")
NORMAL (L"Ordinal OT grammars can be seen as a special case of the more general stochastic OT grammars: "
	"they have integer ranking values (%strata) and zero evaluation noise. "
	"In Praat, therefore, every constraint is taken to have a ranking value, "
	"so that you can do stochastic as well as ordinal OT.")
ENTRY (L"3. Categorical Harmonic Grammars")
NORMAL (L"@@J\\a\"ger (2003)@ and @@Soderstrom, Mathis & Smolensky (2006)@ devised an on-line learning algorithm "
	"for Harmonic Grammars (stochastic gradient ascent). As proven by @@Fischer (2005)@, "
	"this algorithm is guaranteed to converge upon a correct grammar, if there exists one that handles the data.")
ENTRY (L"4. Stochastic Harmonic Grammars")
NORMAL (L"There are two kinds of stochastic models of HG, namely MaxEnt (= Maximum Entropy) grammars "
	"(@@Smolensky (1986)@, @@J\\a\"ger (2003)@), in which the probablity of a candidate winning depends on its harmony, "
	"and Noisy HG (@@Boersma & Escudero (2008)@, @@Boersma & Pater (2008)@), in which noise is added to constraint weights "
	"at evaluation time, as in Stochastic OT.")
NORMAL (L"The algorithm by @@J\\a\"ger (2003)@ and @@Soderstrom, Mathis & Smolensky (2006)@ "
	"can learn languages with optionality and variation (@@Boersma & Pater (2008)@).")
ENTRY (L"The OTGrammar object")
NORMAL (L"An OT grammar is implemented as an @OTGrammar object. "
	"In an OTGrammar object, you specify all the constraints, all the possible inputs and all their possible outputs.")
MAN_END

MAN_BEGIN (L"OT learning 2. The grammar", L"ppgb", 20000122)
INTRO (L"This is chapter 2 of the @@OT learning@ tutorial.")
NORMAL (L"We can ask the grammar to produce an output form for any input form that is in its list of tableaus.")
LIST_ITEM (L"2.1. @@OT learning 2.1. Viewing a grammar|Viewing a grammar@ (N\\s{O}C\\s{ODA} example, @OTGrammarEditor)")
LIST_ITEM (L"2.2. @@OT learning 2.2. Inside the grammar|Inside the grammar@ (saving, inspecting)")
LIST_ITEM (L"2.3. @@OT learning 2.3. Defining your own grammar|Defining your own grammar@")
LIST_ITEM (L"2.4. @@OT learning 2.4. Evaluation|Evaluation@ (noise)")
LIST_ITEM (L"2.5. @@OT learning 2.5. Editing a grammar|Editing a grammar@")
LIST_ITEM (L"2.6. @@OT learning 2.6. Variable output|Variable output@ (place assimilation example)")
LIST_ITEM (L"2.7. @@OT learning 2.7. Tableau pictures|Tableau pictures@ (printing, EPS files)")
LIST_ITEM (L"2.8. @@OT learning 2.8. Asking for one output|Asking for one output@")
LIST_ITEM (L"2.9. @@OT learning 2.9. Output distributions|Output distributions@")
MAN_END

MAN_BEGIN (L"OT learning 2.1. Viewing a grammar", L"ppgb", 20070725)
NORMAL (L"Consider a language where the underlying form /pat/ leads to the surface form [pa], "
	"presumably because the structural constraint N\\s{O}C\\s{ODA} outranks the faithfulness constraint P\\s{ARSE}.")
NORMAL (L"To create such a grammar in Praat, choose ##Create NoCoda grammar# from the Optimality Theory submenu of the @@New menu@. "
	"An @OTGrammar object will then appear in the list of objects. "
	"If you click ##View & Edit#, an @OTGrammarEditor will show up, containing:")
LIST_ITEM (L"1. the constraint list, sorted by %#disharmony (= ranking value + noise):")
LIST_ITEM1 (L" ")
LIST_ITEM1 (L"\t\t      %%ranking value%\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      100.000\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      90.000\t       1.000")
LIST_ITEM1 (L" ")
LIST_ITEM (L"2. the tableaus for the two possible inputs /pat/ and /pa/:")
PICTURE (3.0, 1.0, draw_NoCoda_pat)
PICTURE (3.0, 0.7, draw_NoCoda_pa)
NORMAL (L"From the first tableau, we see that the underlying form /pat/ will surface as [pa], "
	"because the alternative [pat] violates a constraint (namely, N\\s{O}C\\s{ODA}) with a higher disharmony than does [pa], "
	"which only violates P\\s{ARSE}, which has a lower disharmony.")
NORMAL (L"Note the standard OT tableau layout: asterisks (*) showing violations, exclamation marks (!) showing crucial violations, "
	"greying of cells that do not contribute to determining the winning candidate, and a finger (\\pf) pointing to the winner "
	"(this may look like a plus sign (+) if you don't have the Zapf Dingbats font installed on your computer or printer). "
	"An HG tableau contains asterisks and a pointing finger, but no exclamation marks or grey cells.")
NORMAL (L"The second tableau shows that /pa/ always surfaces as [pa], which is no wonder since this is "
	"the only candidate. All cells are grey because none of them contributes to the determination of the winner.")
MAN_END

MAN_BEGIN (L"OT learning 2.2. Inside the grammar", L"ppgb", 20110129)
NORMAL (L"You can save an @OTGrammar grammar as a text file by choosing @@Save as text file...@ from the #Save menu "
	"of the Objects window. For the N\\s{O}C\\s{ODA} example, the contents of the file will look like:")
CODE (L"File type = \"ooTextFile\"")
CODE (L"Object class = \"OTGrammar 2\"")
CODE (L"decisionStrategy = <OptimalityTheory>")
CODE (L"leak = 0")
CODE (L"2 constraints")
CODE (L"constraint [1]: \"N\\bss{O}C\\bss{ODA}\" 100 100 1 ! NOCODA")
CODE (L"constraint [2]: \"P\\bss{ARSE}\" 90 90 1 ! PARSE")
CODE (L" ")
CODE (L"0 fixed rankings")
CODE (L" ")
CODE (L"2 tableaus")
CODE (L"input [1]: \"pat\" 2")
CODE1 (L"candidate [1]: \"pa\" 0 1")
CODE1 (L"candidate [2]: \"pat\" 1 0")
CODE (L"input [2]: \"pa\" 1")
CODE1 (L"candidate [1]: \"pa\" 0 0")
NORMAL (L"To understand more about this data structure, consult the @OTGrammar class description "
	"or click #Inspect after selecting the OTGrammar object. The $$\"\\bss{...}\"$ braces ensure that "
	"the constraint names show up with their traditional small capitals (see @@Text styles@).")
NORMAL (L"You can read this text file into Praat again with @@Read from file...@ from the #Open menu in the Objects window.")
MAN_END

MAN_BEGIN (L"OT learning 2.3. Defining your own grammar", L"ppgb", 20110129)
NORMAL (L"By editing a text file created from an example in the @@New menu@, you can define your own OT grammars.")
NORMAL (L"As explained at @@Save as text file...@, Praat is quite resilient about its text file formats. "
	"As long as the strings and numbers appear in the correct order, you can redistribute the data "
	"across the lines, add all kinds of comments, or leave the comments out. "
	"For the N\\s{O}C\\s{ODA} example, the text file could also have looked like:")
CODE (L"\"ooTextFile\"")
CODE (L"\"OTGrammar 2\"")
CODE (L"<OptimalityTheory>")
CODE (L"0.0   ! leak")
CODE (L"2   ! number of constraints")
CODE (L"\"N\\bss{O}C\\bss{ODA}\" 100 100  1")
CODE (L"\"P\\bss{ARSE}\"       90  90  1")
CODE (L"0   ! number of fixed rankings")
CODE (L"2   ! number of accepted inputs")
CODE (L"\"pat\" 2      ! input form with number of output candidates")
CODE1 (L"\"pa\"  0 1   ! first candidate with violations")
CODE1 (L"\"pat\" 1 0   ! second candidate with violations")
CODE (L"\"pa\" 1       ! input form with number of candidates")
CODE1 (L"\"pa\"  0 0")
NORMAL (L"To define your own grammar, you just provide a number of constraints and their rankings, "
	"and all the possible input forms with all their output candidates, and all the constraint violations "
	"for each candidate. The order in which you specify the constraints is free (you don't have to specify "
	"the highest-ranked first), as long as the violations are in the same order; you could also have reversed "
	"the order of the two input forms, as long as the corresponding candidates follow them; "
	"and, you could also have reversed the order of the candidates within the /pat/ tableau, "
	"as long as the violations follow the output forms. Thus, you could just as well have written:")
CODE (L"\"ooTextFile\"")
CODE (L"\"OTGrammar 2\"")
CODE (L"<OptimalityTheory> 0.0")
CODE (L"2")
CODE (L"\"P\\bss{ARSE}\"       90  90 1.0")
CODE (L"\"N\\bss{O}C\\bss{ODA}\" 100 100 1.0")
CODE (L"0")
CODE (L"2")
CODE (L"\"pa\" 1")
CODE1 (L"\"pa\"  0 0")
CODE (L"\"pat\" 2")
CODE1 (L"\"pat\" 0 1")
CODE1 (L"\"pa\"  1 0")
NORMAL (L"The $$<OptimalityTheory>$ thing in the above refers to the %%decision strategy%. "
	"In this tutorial I mostly assume OT's strict ranking, "
	"but you can experiment with Smolensky's $$<HarmonicGrammar>$ (where the constraint disharmonies represent addable, "
	"possibly negative weights), or with Frank Keller's $$<LinearOT>$ (like Harmonic Grammar, but with the restriction "
	"that negative disharmonies do not count), or with $$<PositiveHG>$ (like Harmonic Grammar, but with the restriction "
	"that disharmonies below 1.0 have weight 1.0), or with $$<ExponentialHG>$ (where the weights are exp(disharmony), somewhere "
	"between Harmonic Grammar and Linear OT), or with a $$<MaximumEntropy>$ grammar "
	"(where the probability that a candidate is chosen is proportional to exp(-disharmony)).")
NORMAL (L"The $$leak$ thing in the above refers to the amount to which constraint weights (especially in Harmonic Grammar) "
	"can leak while learning.")
MAN_END

MAN_BEGIN (L"OT learning 2.4. Evaluation", L"ppgb", 20070725)
NORMAL (L"In an Optimality-Theoretic model of grammar, %#evaluation refers to the determination "
	"of the winning candidate on the basis of the constraint ranking.")
NORMAL (L"In an ordinal OT model of grammar, repeated evaluations will yield the same winner again and again. "
	"We can simulate this behaviour with our N\\s{O}C\\s{ODA} example. "
	"In the editor, you can choose ##Evaluate (zero noise)# or use its keyboard shortcut Command-0 (= Command-zero). "
	"Repeated evaluations (keep Command-0 pressed) will always yield the following grammar:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      100.000\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      90.000\t       1.000")
NORMAL (L"In a stochastic OT model of grammar, repeated evaluations will yield different disharmonies each time. "
	"To see this, choose ##Evaluate (noise 2.0)# or use its keyboard shortcut Command-2. "
	"Repeated evaluations will yield grammars like the following:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      100.427\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      87.502\t       1.000")
NORMAL (L"and")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      101.041\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      90.930\t       1.000")
NORMAL (L"and")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      96.398\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      89.482\t       1.000")
NORMAL (L"The disharmonies vary around the ranking values, "
	"according to a Gaussian distribution with a standard deviation of 2.0. "
	"The winner will still be [pa] in almost all cases, because the probability of bridging "
	"the gap between the two ranking values is very low, namely 0.02 per cent according "
	"to @@Boersma (1998)@, page 332.")
NORMAL (L"With a noise much higher than 2.0, the chances of P\\s{ARSE} outranking N\\s{O}C\\s{ODA} will rise. "
	"To see this, choose ##Evaluate...# and supply 5.0 for the noise. Typical outcomes are:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      92.634\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      86.931\t       1.000")
NORMAL (L"and")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      101.162\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      85.311\t       1.000")
NORMAL (L"and")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      99.778\t       1.000")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      100.000\t      98.711\t       1.000")
NORMAL (L"In the last case, the order of the constraints has been reversed. "
	"You will see that [pat] has become the winning candidate:")
PICTURE (3.0, 1.0, draw_NoCoda_reverse)
NORMAL (L"However, in the remaining part of this tutorial, we will stick with a noise "
	"with a standard deviation of 2.0. This specific number ensures that we can "
	"model fairly rigid rankings by giving the constraints a ranking difference of 10, a nice round number. "
	"Also, the learning algorithm will separate many constraints in such a way that "
	"the differences between their ranking values are in the vicinity of 10.")
MAN_END

MAN_BEGIN (L"OT learning 2.5. Editing a grammar", L"ppgb", 20110808)
NORMAL (L"In the N\\s{O}C\\s{ODA} example, the winning candidate for the input /pat/ was always [pa].")
NORMAL (L"To make [pat] the winner instead, N\\s{O}C\\s{ODA} should be ranked lower than P\\s{ARSE}. "
	"To achieve this even with zero noise, go to the editor and select the N\\s{O}C\\s{ODA} constraint by clicking on it "
	"(a spade symbol \\sp will mark the selected constraint), "
	"and choose ##Edit ranking...# from the #Edit menu, or use the keyboard shortcut Command-E.")
NORMAL (L"In the resulting command window, we lower the ranking of the constraint from 100 to 80, and click OK. "
	"This is what you will see in the editor:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t\\sp ##N\\s{O}C\\s{ODA}#\t      80.000\t      103.429\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      88.083\t       1.000")
PICTURE (3.0, 1.0, draw_NoCoda_pat)
NORMAL (L"Nothing has happened to the tableau, because the disharmonies still have their old values. So choose "
	"##Evaluate (noise 2.0)# (Command-2) or ##Evaluate (zero noise)# (Command-0). The new disharmonies "
	"will centre around the new ranking values, and we see that [pat] becomes the new winner:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      90.000\t      90.743\t       1.000")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      80.000\t      81.581\t       1.000")
PICTURE (3.0, 1.0, draw_NoCoda_reverse)
MAN_END

MAN_BEGIN (L"OT learning 2.6. Variable output", L"ppgb", 20070725)
NORMAL (L"Each time you press Command-2, which invokes the command ##Evaluate (noise 2.0)# from the #Edit menu, "
	"you will see the disharmonies changing. If the distance between the constraint rankings is 10, however, "
	"the winning candidates will very probably stay the same.")
NORMAL (L"So starting from the N\\s{O}C\\s{ODA} example, we edit the rankings of the constraints again, "
	"setting the ranking value of P\\s{ARSE} to 88 and that of N\\s{O}C\\s{ODA} to 85. If we now press Command-2 "
	"repeatedly, we will get [pat] in most of the cases, "
	"but we will see the finger pointing at [pa] in 14 percent of the cases:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      88.000\t      87.421\t       1.000")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      85.000\t      85.585\t       1.000")
PICTURE (3.0, 1.0, draw_NoCoda_reverse)
NORMAL (L"but")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##N\\s{O}C\\s{ODA}#\t      85.000\t      87.128\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE}#\t      88.000\t      85.076\t       1.000")
PICTURE (3.0, 1.0, draw_NoCoda_pat)
NORMAL (L"As a more functionally oriented example, we consider nasal place assimilation. "
	"Suppose that the underlying sequence /an+pa/ surfaces as the assimilated [ampa] "
	"in 80 percent of the cases, and as the faithful [anpa] in the remaining 20 percent, "
	"while the non-nasal stop /t/ never assimilates. "
	"This can be achieved by having the articulatory constraint *G\\s{ESTURE} "
	"ranked at a short distance above *R\\s{EPLACE} (n, m):")
CODE (L"\"ooTextFile\"")
CODE (L"\"OTGrammar 2\"")
CODE (L"decisionStrategy = <OptimalityTheory>")
CODE (L"leak = 0.0")
CODE (L"3 constraints")
CODE (L"\"*G\\bss{ESTURE}\"          102.7 0 1")
CODE (L"\"*R\\bss{EPLACE} (n, m)\"   100.0 0 1")
CODE (L"\"*R\\bss{EPLACE} (t, p)\"   112.0 0 1")
CODE (L"0 fixed rankings")
CODE (L"2 tableaus")
CODE (L"\"an+pa\" 2")
CODE1 (L"\"anpa\"  1 0 0")
CODE1 (L"\"ampa\"  0 1 0")
CODE (L"\"at+ma\" 2")
CODE1 (L"\"atma\"  1 0 0")
CODE1 (L"\"apma\"  0 0 1")
NORMAL (L"You can create this grammar with ##Create place assimilation grammar# from the @@New menu@. "
	"In the editor, it will often look like follows:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (t, p)#\t      112.000\t      109.806\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE}#\t      102.700\t      102.742\t       1.000")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (n, m)#\t      100.000\t      101.044\t       1.000")
PICTURE (4.0, 1.0, draw_NPA_assimilate_anpa)
PICTURE (4.0, 1.0, draw_NPA_assimilate_atma)
NORMAL (L"If you keep the Command-2 keys pressed, however, you will see that the tableaus change "
	"into something like the following in approximately 20 percent of the cases:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (t, p)#\t      112.000\t      113.395\t       1.000")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (n, m)#\t      100.000\t      103.324\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE}#\t      102.700\t      101.722\t       1.000")
PICTURE (4.0, 1.0, draw_NPA_faithful_anpa)
PICTURE (4.0, 1.0, draw_NPA_faithful_atma)
NORMAL (L"We see that /at+ma/ always surfaces at [atma], because *R\\s{EPLACE} (t, p) is ranked much higher "
	"than the other two, and that the output of /an+pa/ is variable because of the close rankings "
	"of *G\\s{ESTURE} and *R\\s{EPLACE} (n, m).")
NORMAL (L"If you try this with a Harmonic Grammar or in Linear OT, you will see the same kind of variation. "
	"Although in HG, e.g. in @@Smolensky & Legendre (2006)@, the variation is usually obtained at the candidate level, "
	"namely by giving each candidate a probability proportional to exp(%harmony/%temperature), "
	"in our version of HG the variation comes about at the constraint level, "
	"namely by the noise that is temporarily added to the ranking of each constraint at evaluation time.")
MAN_END

MAN_BEGIN (L"OT learning 2.7. Tableau pictures", L"ppgb", 20110129)
NORMAL (L"To show a tableau in the @@Picture window@ instead of in the editor, "
	"you select an @OTGrammar object and click ##Draw tableau...#. "
	"After you specify the input form, a tableau is drawn with the current font and size "
	"at the location of the current selection (%viewport) in the Picture window. The top left corner of the tableau "
	"is aligned with the top left corner of the selection. You can draw more than one object into the Picture "
	"window, whose menus also allow you to add a lot of graphics of your own design.")
NORMAL (L"Besides printing the entire picture (with @@Print...@), you can save a part of it to an EPS file "
	"for inclusion into your favourite word processor (with @@Save as EPS file...@). "
	"For the latter to succeed, make sure that the selection includes at least your tableau; "
	"otherwise, some part of your tableau may end up truncated.")
MAN_END

MAN_BEGIN (L"OT learning 2.8. Asking for one output", L"ppgb", 20110808)
NORMAL (L"To ask the grammar to produce a single output for a specified input form, "
	"you can choose @@OTGrammar: Input to output...@. The command window will ask you to provide "
	"an input form and the strength of the noise (the standard value is 2.0 again). "
	"This will perform an evaluation and write the result into the Info window.")
NORMAL (L"If you are viewing the grammar in the @OTGrammarEditor, you will see the disharmonies change, "
	"and if the grammar allows variation, you will see that the winner in the tableau in the editor "
	"varies with the winner shown in the Info window.")
NORMAL (L"Since the editor shows more information than the Info window, "
	"this command is not very useful except for purposes of scripting. "
	"See the following page for some related but more useful commands.")
MAN_END

MAN_BEGIN (L"OT learning 2.9. Output distributions", L"ppgb", 20110808)
NORMAL (L"To ask the grammar to produce %many outputs for a specified input form, "
	"and collect them in a @Strings object, "
	"you select an @OTGrammar and choose @@OTGrammar: Input to outputs...|Input to outputs...@.")
NORMAL (L"For example, select the object \"OTGrammar assimilation\" from our place assimilation example "
	"(@@OT learning 2.6. Variable output|\\SS2.6@), and click ##Input to outputs...#. "
	"In the resulting command window, you specify 1000 trials, a noise strength of 2.0, and \"an+pa\" for the input form.")
NORMAL (L"After you click OK, a @Strings object will appear in the list. "
	"If you click Info, you will see that it contains 1000 strings. "
	"If you click Inspect, you will see that most of the strings are \"ampa\", "
	"but some of them are \"anpa\". These are the output forms computed from 1000 evaluations "
	"for the input /an+pa/.")
NORMAL (L"To count how many instances of [ampa] and [anpa] were generated, you select the @Strings object "
	"and click @@Strings: To Distributions|To Distributions@. You will see a new @Distributions object appear in the list. "
	"If you draw this to the Picture window (with ##Draw as numbers...#), you will see something like:")
LIST_ITEM (L"\tampa\t815")
LIST_ITEM (L"\tanpa\t185")
NORMAL (L"which means that our grammar, when fed with 1000 /an+pa/ inputs, produced [ampa] 815 times, "
	"and [anpa] 185 times, which is consistent with our initial guess that a ranking difference of 2.7 "
	"would cause approximately an 80\\%  - 20\\%  distribution of [ampa] and [anpa].")
ENTRY (L"Checking the distribution hypothesis")
NORMAL (L"To see whether the guess of a 2.7 ranking difference is correct, we perform 1,000,000 trials instead of 1000. "
	"The output distribution (if you have enough memory in your computer) becomes something like "
	"(set the %Precision to 7 in the #Draw command window):")
LIST_ITEM (L"\tampa\t830080")
LIST_ITEM (L"\tanpa\t169920")
NORMAL (L"The expected values under the 80\\%  - 20\\%  distribution hypothesis are:")
LIST_ITEM (L"\tampa\t800000")
LIST_ITEM (L"\tanpa\t200000")
NORMAL (L"We compute (e.g. with @@Calculator...@) a %\\ci^2 of 30080^2/800000 + 30080^2/200000 = 5655.04, "
	"which, of course, is much too high for a distribution with a single degree of freedom. "
	"So the ranking difference must be smaller. If it is 2.4 (change the ranking of *G\\s{ESTURE} to 102.4), "
	"the numbers become something like")
LIST_ITEM (L"\tampa\t801974")
LIST_ITEM (L"\tanpa\t198026")
NORMAL (L"which gives a %\\ci^2 of 24.35. By using the Calculator with the formula $$chiSquareQ (24.35, 1)$, "
	"we find that values larger than this have a probability of 8\\.c10^^-7^ "
	"under the 80\\%  - 20\\%  distribution hypothesis, which must therefore be rejected again.")
NORMAL (L"Rather than continuing this iterative procedure to find the correct ranking values for an "
	"80\\%  - 20\\%  grammar, we will use the Gradual Learning Algorithm "
	"(@@OT learning 5. Learning a stochastic grammar|\\SS5@) to determine the rankings automatically, "
	"without any memory of past events other than the memory associated with maintaining the ranking values.")
ENTRY (L"Measuring all inputs")
NORMAL (L"To measure the outcomes of all the possible inputs at once, you select an @OTGrammar "
	"and choose @@OTGrammar: To output Distributions...|To output Distributions...@. "
	"As an example, try this on our place assimilation grammar. You can supply 1000000 for the number of trials, "
	"and the usual 2.0 for the standard deviation of the noise. "
	"After you click OK, a @Distributions object will appear in the list. "
	"If you draw this to the Picture window, the result will look like:")
LIST_ITEM (L"\t/an+pa/ \\-> anpa\t169855")
LIST_ITEM (L"\t/an+pa/ \\-> ampa\t830145")
LIST_ITEM (L"\t/at+ma/ \\-> atma\t999492")
LIST_ITEM (L"\t/at+ma/ \\-> apma\t508")
NORMAL (L"We see that the number of [apma] outputs is not zero. This is due to the difference of 9.3 "
	"between the rankings of *R\\s{EPLACE} (t, p) and *G\\s{ESTURE}. If you rank "
	"*R\\s{EPLACE} (t, p) at 116.0, the number of produced [apma] reduces to about one in a million, "
	"as you can easily check with some patience.")
MAN_END

MAN_BEGIN (L"OT learning 3. Generating language data", L"ppgb", 20021204)
NORMAL (L"A learner needs two things: a grammar that she can adjust (@@OT learning 2. The grammar|\\SS2@), and language data.")
LIST_ITEM (L"3.1. @@OT learning 3.1. Data from a pair distribution|Data from a pair distribution@")
LIST_ITEM (L"3.2. @@OT learning 3.2. Data from another grammar|Data from another grammar@ (tongue-root-harmony example)")
MAN_END

MAN_BEGIN (L"OT learning 3.1. Data from a pair distribution", L"ppgb", 20110131)
NORMAL (L"If the grammar contains faithfulness constraints, the learner needs pairs of "
	"underlying and adult surface forms. For our place assimilation example, she needs a lot of "
	"/at+ma/ - [atma] pairs, and four times as many /an+pa/ - [ampa] pairs as /an+pa/ - [anpa] pairs. "
	"We can specify this language-data distribution in a @PairDistribution object, "
	"which we could simply save as a text file:")
CODE (L"\"ooTextFile\"")
CODE (L"\"PairDistribution\"")
CODE (L"4 pairs")
CODE (L"\"at+ma\"  \"atma\"  100")
CODE (L"\"at+ma\"  \"apma\"    0")
CODE (L"\"an+pa\"  \"anpa\"   20")
CODE (L"\"an+pa\"  \"ampa\"   80")
NORMAL (L"The values appear to represent percentages, but they could also have been 1.0, 0.0, 0.2, and 0.8, "
	"or any other values with the same proportions. We could also have left out the second pair "
	"and specified \"3 pairs\" instead of \"4 pairs\" in the third line.")
NORMAL (L"We can create this pair distribution with ##Create place assimilation distribution# from the "
	"Optimality Theory submenu of the @@New menu@ in the Objects window. To see that it really contains "
	"the above data, you can draw it to the Picture window. To change the values, use Inspect "
	"(in which case you should remember to click Change after any change).")
NORMAL (L"To generate input-output pairs from the above distribution, select the @PairDistribution and click "
	"@@PairDistribution: To Stringses...|To Stringses...@. "
	"If you then just click OK, there will appear two @Strings objects in the list, called \"input\" "
	"(underlying forms) and \"output\" (surface forms). Both contain 1000 strings. If you Inspect them both, "
	"you can see that e.g. the 377th string in \"input\" corresponds to the 377th string in \"output\", "
	"i.e., the two series of strings are aligned. See also the example at @@PairDistribution: To Stringses...@.")
NORMAL (L"These two Strings objects are sufficient to help an @OTGrammar grammar to change its constraint rankings "
	"in such a way that the output distributions generated by the grammar match the output distributions "
	"in the language data. See @@OT learning 5. Learning a stochastic grammar|\\SS5@.")
MAN_END

MAN_BEGIN (L"OT learning 3.2. Data from another grammar", L"ppgb", 20110128)
NORMAL (L"Instead of generating input-output pairs directly from a @PairDistribution object, "
	"you can also generate input forms and their winning outputs from an @OTGrammar grammar. Of course, "
	"that's what the language data presented to real children comes from. Our example will be "
	"a tongue-root harmony grammar.")
NORMAL (L"Choose @@Create tongue-root grammar...@ from the Optimality Theory submenu of the @@New menu@. "
	"Set %%Constraint set% to \"Five\", and %Ranking to \"Wolof\". Click OK. An object called "
	"\"OTGrammar Wolof\" will appear in the list. Click ##View & Edit#. You will see the following grammar "
	"appear in the @OTGrammarEditor:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      100.000\t      100.000\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      50.000\t      50.000\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      30.000\t      30.000\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      20.000\t      20.000\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      10.000\t      10.000\t       1.000")
NORMAL (L"This simplified Wolof grammar, with five constraints with clearly different rankings, is equivalent "
	"to the traditional OT ranking")
FORMULA (L"*[rtr / hi] >> P\\s{ARSE} (rtr) >> *G\\s{ESTURE} (contour) >> P\\s{ARSE} (atr) >> *[atr / lo]")
NORMAL (L"These constraints are based on a description of Wolof by "
	"@@Archangeli & Pulleyblank (1994)|Archangeli & Pulleyblank (1994: 225\\--239)@. "
	"For the meaning of these constraints, see @@Boersma (1998)|Boersma (1998: 295)@, "
	"or the @@Create tongue-root grammar...@ manual page.")
NORMAL (L"For each input, there are four output candidates: "
	"the vowel heights will be the same as those in the input, but the tongue-root values of V__1_ and V__2_ are varied. "
	"For example, for the input [ita] we will have the four candidates "
	"[ita], [it\\sw], [\\icta], and [\\ict\\sw].")
NORMAL (L"With this way of generating candidates, we see that the five constraints are completely ranked. "
	"First, the absolute prohibition on surface [\\ic] shows that *[rtr / hi] outranks RTR faithfulness "
	"(otherwise, [\\ict\\ic] would have been the winner):")
PICTURE (4.0, 1.5, draw_Wolof_ItI)
NORMAL (L"Second, the faithful surfacing of the disharmonic /it\\ef/ shows that RTR faithfulness must outrank "
	"the harmony (anti-contour) constraint (otherwise, [ite] would have been the winner):")
PICTURE (4.0, 1.5, draw_Wolof_itE)
NORMAL (L"Third, the RTR-dominant harmonicization of underlying disharmonic /et\\ef/ shows that harmony must outrank ATR faithfulness "
	"(otherwise, [et\\ef] would have won):")
PICTURE (4.0, 1.5, draw_Wolof_etE)
NORMAL (L"Finally, the faithful surfacing of the low ATR vowel /\\sw/ even if not forced by harmony, shows that "
	"ATR faithfulness outranks *[atr / lo] (otherwise, [ata] would have been the winning candidate):")
PICTURE (4.0, 1.5, draw_Wolof_schwatschwa)
NORMAL (L"These four ranking arguments clearly establish the crucial rankings of all five constraints.")
ENTRY (L"Generating inputs from the grammar")
NORMAL (L"According to @@Prince & Smolensky (1993)@, the input to an OT grammar can be %anything. "
	"This is the idea of %%##richness of the base%#. "
	"When doing a practical investigation, however, we are only interested in the inputs "
	"that will illustrate the properties of our partial grammars. "
	"In the case of simplified Wolof, this means the 36 possible V__1_tV__2_ sequences "
	"where V__1_ and V__2_ are any of the six front vowels i, \\ic, e, \\ef, \\sw, and a "
	"(see @@Create tongue-root grammar...@).")
NORMAL (L"A set of inputs can be generated from an @OTGrammar object by inspecting the list of tableaus. "
	"So select the Wolof tongue-root grammar and choose @@OTGrammar: Generate inputs...|Generate inputs...@. "
	"Set %%Number of trials% to 100, and click OK. A @Strings object named \"Wolof_in\" "
	"will appear in the list. Click Inspect and examine the 100 input strings. "
	"You will see that they have been randomly chosen from the 36 possible V__1_tV__2_ sequences "
	"as described at @@Create tongue-root grammar...@:")
FORMULA (L"\\epta, et\\ef, \\epti, it\\ef, \\ept\\ef, iti, \\eft\\ic, it\\ic, \\icti, et\\ef, ...")
NORMAL (L"Thus, when asked to generate a random input, these grammars produce any of the 36 possible V__1_tV__2_ "
	"sequences, all with equal probability.")
ENTRY (L"Generating outputs from the grammar")
NORMAL (L"To compute the outputs for the above set of input forms, select %both the @OTGrammar object "
	"%and the input @Strings object, and choose @@OTGrammar & Strings: Inputs to outputs...|Inputs to outputs...@, "
	"perhaps specifying zero evaluation noise. "
	"A new Strings objects called \"Wolof_out\" will appear in the list. "
	"If you Inspect it, you will see that it contains a string sequence aligned with the original input strings:")
FORMULA (L"\\epta, \\ept\\ef, \\efti, it\\ef, \\eft\\ef, iti, \\efti, iti, iti, \\eft\\ef, ...")
NORMAL (L"In this way, we have created two Strings objects, which together form a series of input-output pairs "
	"needed for learning a grammar that contains faithfulness constraints.")
MAN_END

MAN_BEGIN (L"OT learning 4. Learning an ordinal grammar", L"ppgb", 20100331)
NORMAL (L"With the data from a tongue-root-harmony language with five completely ranked constraints, "
	"we can have a throw at learning this language, starting with a grammar in which all the constraints "
	"are ranked at the same height, or randomly ranked, or with articulatory constraints outranking "
	"faithfulness constraints.")
NORMAL (L"Let's try the third of these. Create an infant tongue-root grammar by choosing "
	"@@Create tongue-root grammar...@ and specifying \"Five\" for the constraint set "
	"and \"Infant\" for the ranking. The result after a single evaluation will be like:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      100.000\t      100.631\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      100.000\t      100.244\t       1.000")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      100.000\t      97.086\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      50.000\t      51.736\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      50.000\t      46.959\t       1.000")
NORMAL (L"Such a grammar produces all kinds of non-adult results. For instance, the input /\\swt\\ic/ "
	"will surface as [at\\ic]:")
PICTURE (4.0, 1.5, draw_Infant_swtI)
NORMAL (L"The adult form is very different: [\\swti]. The cause of the discrepancy is in the order of "
	"the constraints *[atr / lo] and *[rtr / hi], which militate against [\\sw] and [\\ic], respectively. "
	"Simply reversing the rankings of these two constraints would solve the problem in this case. "
	"More generally, @@Tesar & Smolensky (1998)@ claim that demoting all the constraints that cause the "
	"adult form to lose into the stratum just below the highest-ranked constraint "
	"violated in the learner's form (here, moving *[atr / lo] just below *[rtr / hi] "
	"into the same stratum as P\\s{ARSE} (rtr)), "
	"will guarantee convergence to the target grammar, "
	"%%if there is no variation in the data% (Tesar & Smolensky's algorithm is actually incorrect, "
	"but can be repaired easily, as shown by @@Boersma (2009b)@).")
NORMAL (L"But Tesar & Smolensky's algorithm cannot be used for variable data, since all constraints would be "
	"tumbling down, exchanging places and producing wildly different grammars at each learning step. "
	"Since language data do tend to be variable, we need a gradual and balanced learning algorithm, "
	"and the following algorithm is guaranteed to converge "
	"to the target language, if that language can be described by a stochastic OT grammar.")
NORMAL (L"The reaction of the learner to hearing the mismatch between the adult [\\swti] and her own [at\\ic], "
	"is simply:")
LIST_ITEM (L"1. to move the constraints violated in her own form, i.e. *[rtr / hi] and P\\s{ARSE} (atr), "
	"up by a small step along the ranking scale, thus decreasing the probability that her form will be the winner "
	"at the next evaluation of the same input;")
LIST_ITEM (L"2. and to move the constraints violated in the adult form, namely *[atr / lo] and P\\s{ARSE} (rtr), "
	"down along the ranking scale, thus increasing the probability that the adult form will be the learner's "
	"winner the next time.")
NORMAL (L"If the small reranking step (the %#plasticity) is 0.1, the grammar will become:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      100.000\t      100.631\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      99.900\t      100.244\t       1.000")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      100.100\t      97.086\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      49.900\t      51.736\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      50.100\t      46.959\t       1.000")
NORMAL (L"The disharmonies, of course, will be different at the next evaluation, with a probability slightly higher "
	"than 50\\%  that *[rtr / hi] will outrank *[atr / lo]. Thus the relative rankings of these two grounding "
	"constraints have moved into the direction of the adult grammar, in which they are ranked at opposite "
	"ends of the grammar.")
NORMAL (L"Note that the relative rankings of P\\s{ARSE} (atr) and P\\s{ARSE} (rtr) are "
	"now moving in a direction opposite to where they will have to end up in this RTR-dominant language. "
	"This does not matter: the procedure will converge nevertheless.")
NORMAL (L"We are now going to simulate the infant who learns simplified Wolof. Take an adult Wolof grammar "
	"and generate 1000 input strings and the corresponding 1000 output strings "
	"following the procedure described in @@OT learning 3.2. Data from another grammar|\\SS3.2@. "
	"Now select the infant @OTGrammar and both @Strings objects, and choose @@OTGrammar & 2 Strings: Learn...|Learn...@. "
	"After you click OK, the learner processes each of the 1000 input-output pairs in succession, "
	"gradually changing the constraint ranking in case of a mismatch. The resulting grammar may look like:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      100.800\t      98.644\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      89.728\t      94.774\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      89.544\t      86.442\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      66.123\t      65.010\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      63.553\t      64.622\t       1.000")
NORMAL (L"We already see some features of the target grammar, namely the top ranking of *[rtr / hi] "
	"and RTR dominance (the mutual ranking of the P\\s{ARSE} constraints). The steps have not been exactly 0.1, "
	"because we also specified a relative plasticity spreading of 0.1, thus giving steps typically in the range of 0.7 to 1.3. "
	"The step is also multiplied by the %%constraint plasticity%, which is simply 1.000 in all examples in this tutorial; "
	"you could set it to 0.0 to prevent a constraint from moving up or down at all. "
	"The %leak is the part of the constraint weight (especially in Harmonic Grammar) that is thrown away whenever a constraint is reranked; "
	"e.g if the leak is 0.01 and the step is 0.11, the constraint weight is multiplied by (1 \\-- 0.01\\.c0.11) = 0.9989 before "
	"the learning step is taken; in this way you could implement forgetful learning of correlations.")
NORMAL (L"After learning once more with the same data, the result is:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      100.800\t      104.320\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      81.429\t      82.684\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      79.966\t      78.764\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      81.316\t      78.166\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      77.991\t      77.875\t       1.000")
NORMAL (L"This grammar now sometimes produces faithful disharmonic utterances, because the P\\s{ARSE} now often "
	"outrank the gestural constraints at evaluation time. But there is still a lot of variation produced. "
	"Learning once more with the same data gives:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      100.800\t      100.835\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      86.392\t      82.937\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      81.855\t      81.018\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      78.447\t      78.457\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      79.409\t      76.853\t       1.000")
NORMAL (L"By inspecting the first column, you can see that the ranking values are already in the same order as in the target grammar, "
	"so that the learner will produce 100 percent correct adult utterances if her evaluation noise is zero. However, "
	"with a noise of 2.0, there will still be variation. For instance, the disharmonies above will "
	"produce [ata] instead of [\\swt\\sw] for underlying /\\swt\\sw/. Learning seven times more "
	"with the same data gives a reasonable proficiency:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      100.800\t      99.167\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      91.580\t      93.388\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      85.487\t      86.925\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      80.369\t      78.290\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      75.407\t      74.594\t       1.000")
NORMAL (L"No input forms have error rates above 4 percent now, so the child has learned a lot with only 10,000 data, "
	"which may be on the order of the number of input data she receives every day.")
NORMAL (L"We could have sped up the learning process appreciably by using a plasticity of 1.0 instead of 0.1. "
	"This would have given a comparable grammar after only 1000 data. After 10,000 data, we would have")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*[rtr / hi]#\t      107.013\t      104.362\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (rtr)#\t      97.924\t      99.984\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE} (contour)#\t      89.679\t      89.473\t       1.000")
LIST_ITEM1 (L"\t##P\\s{ARSE} (atr)#\t      81.479\t      83.510\t       1.000")
LIST_ITEM1 (L"\t##*[atr / lo]#\t      73.067\t      72.633\t       1.000")
NORMAL (L"With this grammar, all the error rates are below 0.2 percent. We see that crucially ranked constraints "
	"will become separated after a while by a gap of about 10 along the ranking scale.")
NORMAL (L"If we have three constraints obligatorily ranked as A >> B >> C in the adult grammar, with ranking differences of 8 between "
	"A and B and between B and C in the learner's grammar (giving an error rate of 0.2\\% ), the ranking A >> C has a chance of less than 1 in 100 million "
	"to be reversed at evaluation time. This relativity of error rates is an empirical prediction of our stochastic OT grammar model.")
NORMAL (L"Our Harmonic Grammars with constraint noise (Noisy HG) are slightly different in that respect, "
	"but are capable of learning a constraint ranking for any language that can be generated from an ordinal ranking. "
	"As proved by @@Boersma & Pater (2008)@, the same learning rule as was devised for MaxEnt grammars by @@J\\a\"ger (2003)@ "
	"is able to learn all languages generated by %nonnoisy HG grammars as well; "
	"the GLA, by contrast, failed to converge on 0.4 percent of randomly generated OT languages "
	"(failures of the GLA on ordinal grammars were discovered first by @@Pater (2008)@). "
	"This learning rule for HG and MaxEnt is the same as the GLA described above, "
	"except that the learning step of each constraint is multiplied by "
	"the difference of the number of violations of this constraint between the correct form and the incorrect winner; "
	"this multiplication is crucial (without it, stochastic gradient ascent is not guaranteed to converge), "
	"as was noted by J\\a\"ger for MaxEnt. The same procedure for updating weights occurs "
	"in @@Soderstrom, Mathis & Smolensky (2006)@, who propose "
	"an incremental version (formulas 21 and 35d) of the harmony version (formulas 14 and 18) "
	"of the learning equation for Boltzmann machines (formula 13). "
	"The differences between the three implementations is that in Stochastic OT and Noisy HG the evaluation noise (or %temperature) is in the constraint rankings, "
	"in MaxEnt it is in the candidate probabilities, and in Boltzmann machines it is in the activities (i.e. the constraint violations). "
	"The upate procedure is also similar to that of the %perceptron, a neural network invented by @@Rosenblatt (1962)@ "
	"for classifying continuous inputs.")
MAN_END

MAN_BEGIN (L"OT learning 5. Learning a stochastic grammar", L"ppgb", 20070725)
NORMAL (L"Having shown that the algorithm can learn deep obligatory rankings, we will now see "
	"that it also performs well in replicating the variation in the language environment.")
NORMAL (L"Create a place assimilation grammar as described in @@OT learning 2.6. Variable output|\\SS2.6@, "
	"and set all its rankings to 100.000:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*G\\s{ESTURE}#\t      100.000\t      100.000\t       1.000")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (t, p)#\t      100.000\t      100.000\t       1.000")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (n, m)#\t      100.000\t      100.000\t       1.000")
NORMAL (L"Create a place assimilation distribution and generate 1000 string pairs (@@OT learning 3.1. Data from a pair distribution|\\SS3.1@). "
	"Select the grammar and the two @Strings objects, and learn with a plasticity of 0.1:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (t, p)#\t      104.540\t      103.140\t       1.000")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (n, m)#\t      96.214\t      99.321\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE}#\t      99.246\t      97.861")
NORMAL (L"The output distributions are now (using @@OTGrammar: To output Distributions...@, see @@OT learning 2.9. Output distributions|\\SS2.9@):")
LIST_ITEM1 (L"\t/an+pa/ \\-> anpa\t14.3\\% ")
LIST_ITEM1 (L"\t/an+pa/ \\-> ampa\t85.7\\% ")
LIST_ITEM1 (L"\t/at+ma/ \\-> atma\t96.9\\% ")
LIST_ITEM1 (L"\t/at+ma/ \\-> apma\t3.1\\% ")
NORMAL (L"After another 10,000 new string pairs, we have:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (t, p)#\t      106.764\t      107.154\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE}#\t      97.899\t      97.161\t       1.000")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (n, m)#\t      95.337\t      96.848\t       1.000")
NORMAL (L"With the following output distributions (measured with a million draws):")
LIST_ITEM1 (L"\t/an+pa/ \\-> anpa\t18.31\\% ")
LIST_ITEM1 (L"\t/an+pa/ \\-> ampa\t81.69\\% ")
LIST_ITEM1 (L"\t/at+ma/ \\-> atma\t99.91\\% ")
LIST_ITEM1 (L"\t/at+ma/ \\-> apma\t0.09\\% ")
NORMAL (L"The error rate is acceptably low, but the accuracy in reproducing the 80\\%  - 20\\%  "
	"distribution could be better. This is because the relatively high plasticity of 0.1 "
	"can only give a coarse approximation. So we lower the plasticity to 0.001, "
	"and supply 100,000 new data:")
LIST_ITEM1 (L"\t\t      %%ranking value\t      %disharmony\t      %plasticity")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (t, p)#\t      106.810\t      107.184\t       1.000")
LIST_ITEM1 (L"\t##*G\\s{ESTURE}#\t      97.782\t      99.682\t       1.000")
LIST_ITEM1 (L"\t##*R\\s{EPLACE} (n, m)#\t      95.407\t      98.760\t       1.000")
NORMAL (L"With the following output distributions:")
LIST_ITEM1 (L"\t/an+pa/ \\-> anpa\t20.08\\% ")
LIST_ITEM1 (L"\t/an+pa/ \\-> ampa\t79.92\\% ")
LIST_ITEM1 (L"\t/at+ma/ \\-> atma\t99.94\\% ")
LIST_ITEM1 (L"\t/at+ma/ \\-> apma\t0.06\\% ")
NORMAL (L"So besides learning obligatory rankings like a child does, "
	"the algorithm can also replicate very well the probabilities of the environment. "
	"This means that a GLA learner can learn stochastic grammars.")
MAN_END

MAN_BEGIN (L"OT learning 6. Shortcut to grammar learning", L"ppgb", 20070523)
INTRO (L"Once you have mastered the tedious procedures of making Praat learn stochastic grammars, "
	"as described in the previous chapters of this tutorial, you can try a faster procedure, "
	"which simply involves selecting an @OTGrammar object together with a @PairDistribution object, "
	"and clicking ##Learn...#. Once you click OK, Praat will feed the selected grammar with input/output "
	"pairs drawn from the selected distribution, and the grammar will be modified every time its output "
	"is different from the given output. Here is the meaning of the arguments:")
TAG (L"%%Evaluation noise% (standard value: 2.0)")
DEFINITION (L"the standard deviation of the noise added to the ranking of each constraint at evaluation time.")
TAG (L"%%Strategy% (standard value: Symmetric all)")
DEFINITION (L"what to do when the learner's output is different from the given output. Possibilities:")
LIST_ITEM1 (L"Demotion only: lower the ranking of every constraint that is violated more in the correct output "
	"than in the learner's output. This algorithm crashes if there is variation in the data, i.e. if some inputs "
	"can have more than one possible adult outputs.")
LIST_ITEM1 (L"Symmetric one: lower the ranking of the highest-ranked constraint that is violated more in the adult output "
	"than in the learner's output, and raise the ranking of the highest-ranked constraint that is violated more "
	"in the learner's output than in the adult output. This is the \"minimal\" algorithm described and refuted in "
	"@@Boersma (1998)@, chapters 14-15.")
LIST_ITEM1 (L"Symmetric all: lower the ranking of all constraints that are violated more in the adult output "
	"than in the learner's output, and raise the ranking of all constraints that are violated more "
	"in the learner's output than in the adult output. This is the algorithm described in @@Boersma & Hayes (2001)@.")
LIST_ITEM1 (L"Weighted uncancelled: the same as \"Symmetric all\", but the size of the learning step "
	"is divided by the number of moving constraints. This makes sure that the average ranking of all the constraints "
	"is constant.")
LIST_ITEM1 (L"Weighted all: the \"Symmetric all\" strategy can reworded as follows: \"lower the ranking of all constraints "
	"that are violated in the adult output, and raise the ranking of all constraints that are violated in the learner's output\". "
	"Do that, but divide the size of the learning step by the number of moving constraints.")
LIST_ITEM1 (L"EDCD: Error-Driven Constraint Demotion, the algorithm described by @@Tesar & Smolensky (1998)@. "
	"All constraints that prefer the adult form and are ranked above the highest-ranked constraint that prefers the learner's form, "
	"are demoted to the ranking of that last constraint minus 1.0.")
TAG (L"%%Initial plasticity% (standard value: 1.0)")
TAG (L"%%Replications per plasticity% (standard value: 100000)")
TAG (L"%%Plasticity decrement% (standard value: 0.1)")
TAG (L"%%Number of plasticities% (standard value: 4)")
DEFINITION (L"these four arguments determine the %%learning scheme%, i.e. the number of times the grammar will "
	"receive data at a certain plasticity. With the standard values, there will be 100000 data while the plasticity is 1.0 "
	"(the initial plasticity), 100000 data while the plasticity is 0.1, 100000 data while the plasticity is 0.01, "
	"and 100000 data while the plasticity is 0.001. If you want learning at a constant plasticity, set the "
	"%%number of plasticities% to 1. Note that for the decision strategies of HarmonicGrammar, LinearOT, PositiveHG or MaximumEntropy "
	"the learning step for a constraint equals the plasticity multiplied by the difference between the "
	"numbers of violations of this constraint in the adult output and in the learner's output.")
TAG (L"%%Rel. plasticity spreading% (standard value: 0.1)")
DEFINITION (L"if this is not 0, the size of the learning step will vary randomly. For instance, if the plasticity is set to 0.01, "
	"and the relative plasticity spreading is 0.1, you will get actual learning steps that could be anywhere between 0.007 "
	"and 0.013, according to a Gaussian distribution with mean 0.01 and standard deviation 0.001.")
TAG (L"%%Honour local rankings% (standard value: on)")
DEFINITION (L"if this is on, the fixed rankings that you supplied in the grammar will be maintained during learning: "
	"if a constraint falls below a constraint that is supposed to be universally lower-ranked, this second constraint "
	"will be demoted as well.")
TAG (L"%%Number of chews% (standard value: 1)")
DEFINITION (L"the number of times that each input-output pair is fed to the grammar. Setting this number to 20 "
	"will give a slightly different (perhaps more accurate) result than simply raising the plasticity by a factor of 20.")
MAN_END

MAN_BEGIN (L"OT learning 7. Learning from overt forms", L"ppgb", 20031220)
INTRO (L"In order to be able to learn phonological production, both EDCD and GLA require pairs of underlying form "
	"and surface form. However, the language-learning child hears neither of these forms: she only hears ##%%overt forms%#, "
	"with less structural information than the underlying and surface forms contain.")
ENTRY (L"Interpretive parsing")
NORMAL (L"The language-learning child has to construct both the surface form and the underlying form from the overt form "
	"that she hears. @@Tesar & Smolensky (1998)@ proposed that the child computes a surface form from the "
	"overt form by using the same constraint ranking as in production. For instance, the overt form [\\si \\si\\'^ \\si], which "
	"is a sequence of three syllables with stress on the second syllable, will be interpreted as the surface form "
	"/(\\si \\si\\'^) \\si/ in iambic left-aligning languages (I\\s{AMBIC} >> T\\s{ROCHAIC}, and A\\s{LL}F\\s{EET}L\\s{EFT} "
	">> A\\s{LL}F\\s{EET}R\\s{IGHT}), but as the surface form /\\si (\\si\\'^ \\si)/ in trochaic right-aligning languages. "
	"Tesar & Smolensky call this procedure @@Robust Interpretive Parsing@, because it works even if the listener's grammar "
	"would never produce such a form. For instance, if I\\s{AMBIC} >> A\\s{LL}F\\s{EET}R\\s{IGHT} >> T\\s{ROCHAIC} >> "
	"A\\s{LL}F\\s{EET}L\\s{EFT}, the listener herself would produce the iambic right-aligned /\\si (\\si \\si\\'^)/ "
	"for any trisyllabic underlying form, though she will still interpret [\\si \\si\\'^ \\si] as /(\\si \\si\\'^) \\si/, "
	"which is illegal in her own grammar. Hearing forms that are illegal in one's own grammar is of course a common "
	"situation for language-learning children.")
NORMAL (L"In Tesar & Smolensky's view, the underlying form can be trivially computed from the surface form, "
	"since the surface form %contains enough information. For instance, the surface form /(\\si \\si\\'^) \\si/ must "
	"lead to the underlying form \\|f\\si \\si \\si\\|f if all parentheses and stress marks are removed. Since "
	"@@McCarthy & Prince (1995)@, this %containment view of surface representations has been abandoned. "
	"In Praat, therefore, the underlying form is not trivially computed from the surface form, "
	"but all the tableaus are scanned for the surface form that violates the least high-ranked constraints (in the usual "
	"OT sense), as long as it contains the given overt form. For instance, if I\\s{AMBIC} >> A\\s{LL}F\\s{EET}R\\s{IGHT} "
	">> T\\s{ROCHAIC} >> A\\s{LL}F\\s{EET}L\\s{EFT}, the overt form [\\si \\si\\'^ \\si] occurs in two candidates: "
	"the surface form /(\\si \\si\\'^) \\si/ in the tableau for the underlying form \\|f\\si \\si \\si\\|f, and "
	"the surface form /\\si (\\si\\'^ \\si)/ in the tableau for the underlying form \\|f\\si \\si \\si\\|f. The best candidate "
	"is the surface form /(\\si \\si\\'^) \\si/ in the tableau for the underlying form \\|f\\si \\si \\si\\|f. Hence, "
	"Praat's version of Robust Interpretive Parsing will map the overt form [\\si \\si\\'^ \\si] to the underlying form "
	"\\|f\\si \\si \\si\\|f (the 'winning tableau') and to the surface form /(\\si \\si\\'^) \\si/ (to be sure, "
	"this is the same result as in Tesar & Smolensky's "
	"version, but crucial differences between the two versions will appear when faithfulness constraints are involved).")
NORMAL (L"In Praat, you can do interpretive parsing. For example, create a grammar with ##Create metrics grammar...# "
	"from the @@New menu@. Then choose ##Get interpretive parse...# from the #Query submenu and supply \"[L1 L L]\" for the "
	"overt form, which means a sequence of three light syllables with a main stress on the first. The Info window will show you "
	"the optimal underlying and surface forms, given the current constraint ranking.")
ENTRY (L"Learning from partial forms")
NORMAL (L"Now that the learning child can convert an overt form to an underlying-surface pair, she can compare this "
	"surface form to the surface form that she herself would have derived from this underlying form. For instance, "
	"If I\\s{AMBIC} >> A\\s{LL}F\\s{EET}R\\s{IGHT} >> T\\s{ROCHAIC} >> A\\s{LL}F\\s{EET}L\\s{EFT}, the winning "
	"tableau is \\|f\\si \\si \\si\\|f, and the perceived adult surface form is /(\\si \\si\\'^) \\si/. "
	"But from the underlying form \\|f\\si \\si \\si\\|f, the learner will derive /\\si (\\si \\si\\'^)/ as her own surface form. "
	"The two surface forms are different, so that the learner can take action by reranking one or more constraints, "
	"perhaps with EDCD or GLA.")
NORMAL (L"In Praat, you can learn from partial forms. Select the metrics grammar and choose ##Learn from one partial output...#, "
	"and supply \"[L1 L L]\". If you do this several times, you will see that the winner for the tableau \"|L L L|\" will become one of "
	"the two forms with overt part \"[L1 L L]\".")
NORMAL (L"To run a whole simulation, you supply a @Distributions object with one column, perhaps from a text file. The following "
	"text file shows the overt forms for Latin, with the bisyllabic forms occurring more often than the trisyllabic forms:")
CODE (L"\"ooTextFile\"")
CODE (L"\"Distributions\"")
CODE (L"1 column with numeric data")
CODE (L"   \"Latin\"")
CODE (L"28 rows")
CODE (L"\"[L1 L]\" 25")
CODE (L"\"[L1 H]\" 25")
CODE (L"\"[H1 L]\" 25")
CODE (L"\"[H1 H]\" 25")
CODE (L"\"[L1 L L]\" 5")
CODE (L"\"[H1 L L]\" 5")
CODE (L"\"[L H1 L]\" 5")
CODE (L"\"[H H1 L]\" 5")
CODE (L"\"[L1 L H]\" 5")
CODE (L"\"[H1 L H]\" 5")
CODE (L"\"[L H1 H]\" 5")
CODE (L"\"[H H1 H]\" 5")
CODE (L"\"[L L1 L L]\" 1")
CODE (L"\"[L H1 L L]\" 1")
CODE (L"\"[L L H1 L]\" 1")
CODE (L"\"[L H H1 L]\" 1")
CODE (L"\"[L L1 L H]\" 1")
CODE (L"\"[L H1 L H]\" 1")
CODE (L"\"[L L H1 H]\" 1")
CODE (L"\"[L H H1 H]\" 1")
CODE (L"\"[H L1 L L]\" 1")
CODE (L"\"[H H1 L L]\" 1")
CODE (L"\"[H L H1 L]\" 1")
CODE (L"\"[H H H1 L]\" 1")
CODE (L"\"[H L1 L H]\" 1")
CODE (L"\"[H H1 L H]\" 1")
CODE (L"\"[H L H1 H]\" 1")
CODE (L"\"[H H H1 H]\" 1")
NORMAL (L"Read this file into Praat with @@Read from file...@. A @Distributions object then appears in the object list. "
	"Click @@Distributions: To Strings...|To Strings...@, then OK. A @Strings object containing 1000 strings, drawn randomly "
	"from the distribution, with relative frequencies as in the text file, will appear in the list. Click @Inspect to check the contents.")
NORMAL (L"You can now select the @OTGrammar together with the @Strings and choose ##Learn from partial outputs...#. "
	"A thousand times, Praat will construct a surface form from the overt form by interpretive parsing, "
	"and also construct the underlying form in the same way, from which it will construct another surface form by evaluating the "
	"tableau. Whenever the two surface forms are not identical, some constraints will be reranked. In the current implementation, "
	"the disharmonies for interpretive parsing and for production are the same, i.e., "
	"if the evaluation noise is not zero, the disharmonies are randomly renewed before each interpretive parsing "
	"but stay the same for the subsequent virtual production.")
MAN_END

/*
MAN_BEGIN (L"OTGrammar examples", L"ppgb", 20001027)
ENTRY (L"Safety margin and stochastic evaluation")
NORMAL (L"What is a %small demotion step? This must be taken relative to another quantity. "
	"This quantity is the evaluation noise.")
NORMAL (L"If the %%ranking spreading% were zero, the demotions in the GLA would immediately stop once that "
	"a constraint has fallen below its competitors. If the data contain an error, the grammar "
	"will change to an incorrect state, and the learner has to make an error to correct it. "
	"Though the error ratio is now one (because of the constant demotion step), "
	"the learner's behaviour can still be described as a `tit-for-tat' strategy, "
	"which is an unknown phenomenon in human speech variation.")
NORMAL (L"The solution is to have a finite ranking spreading: in this way, the learner will continue making "
	"errors (though less than 50\\% ) after the constraint has fallen below its competitors. "
	"For instance, with a ranking spreading of 2.0, the distance between the constraints will be about "
	"10 after a few thousand relevant data: the %%safety margin%. "
	"If the mean demotion step is 0.1, there is no chance "
	"that a modest amount of erroneous data will reverse the ranking. It is true, however, that "
	"one erroneous datum will decrease the constraint distance by 0.1, so that the learner will have to "
	"make one mistake herself to restore the original distance. But this is only natural, "
	"as she may well wait a long time before doing this: "
	"on the average, the same number of thousands of data. This is the %%patient error-matching learner%.")
ENTRY (L"Learning from surface data alone")
NORMAL (L"Many a young learner will take the adult surface forms, as perceived by herself, "
	"as her underlying forms. In other words, the input to her grammar will equal "
	"the output of the adult grammar.")
NORMAL (L"We can simulate what happens here by taking the adult output as the input to the learning algorithms:")
LIST_ITEM (L"1. Create a five-constraint Wolof grammar.")
LIST_ITEM (L"2. Generate 1000 input strings.")
LIST_ITEM (L"3. Compute the 1000 output strings.")
LIST_ITEM (L"4. Create a five-constraint \"equal\" grammar.")
LIST_ITEM (L"5. Select this monostratal grammar and the output strings only, "
	"and click ##Learn output (T&S)#. See @@OTAnyGrammar & Strings: Learn output (T&S)@.")
NORMAL (L"The result will be a grammar where the faithfulness constraints outrank all the "
	"gestural constraints that can be violated in any output string:")
LIST_ITEM (L"##*[rtr / hi]#  100.000  100.000")
LIST_ITEM (L"##P\\s{ARSE} (atr)#  100.000  100.000")
LIST_ITEM (L"##P\\s{ARSE} (rtr)#  100.000  100.000")
LIST_ITEM (L"##*G\\s{ESTURE} (contour)#  99.000  99.000")
LIST_ITEM (L"##*[atr / lo]#  99.000  99.000")
NORMAL (L"You will get a comparable result with @@OTAnyGrammar & Strings: Learn output (GLA)...@.")
NORMAL (L"The resulting grammar represents the learner's state after the acquisition of "
	"all the relevant gestures. The learner will now faithfully reproduce /et\\ef/ "
	"if that were in her lexicon. Before being able to render such an underlying form as [\\eft\\ef], "
	"she must learn that faithfulness can be violated.")
ENTRY (L"Example 2: When underlying forms are irrelevant")
NORMAL (L"Underlying forms are relevant only if faithfulness constraints are involved. "
	"If a grammar only contains constraints that evaluate the output, "
	"we need no input strings for our simulations. However, "
	"if the relevant constraint had fixed rankings, there would only be a single possible output, "
	"which seems uninteresting. An interesting output-only grammar, therefore, necessarily "
	"features stochastic evaluation, and at least some of the constraints wil have rankings that "
	"are close to each other.")
NORMAL (L"#%Example. @@Hayes & MacEachern (1998)@ identify 11 output-oriented constraints "
	"for the forms of quatrains in English folk verse.")
LIST_ITEM (L"1. Create a folk-verse grammar with equal constraint rankings (all 100). "
	"You may find it in the file ##folkVerse.OTGrammar# in the #demo directory of your #Praat distribution, "
	"or get it from ##http://www.fon.hum.uva.nl/praat/folkVerse.OTGrammar#.")
LIST_ITEM (L"2. Generate 1000 input strings. They will all be empty strings.")
LIST_ITEM (L"3. Read the file that contains the surface distribution of the possible outputs. "
	"It is in the #demo folder or at ##http://www.fon.hum.uva.nl/praat/folkVerse.Distributions#. "
	"A @Distributions object will appear in the list. "
	"Column \"Actual\" is the last column of table (10) in Hayes & MacEachern (1998).")
LIST_ITEM (L"4. From this surface distribution, create a list of 1000 output strings, "
	"using @@Distributions: To Strings...@ (set %column to 1).")
LIST_ITEM (L"5. Select the grammar, the \"input strings\", and the output strings, "
	"and learn in the usual way. After learning, you can see that some constraints have risen above 100, "
	"and some have fallen below 100.")
NORMAL (L"With each of the 1000 outputs, the learner can be regarded as having generated a quatrain herself "
	"and compared it to a quatrain in her folk-verse environment. If these quatrains are equal "
	"(a 10\\%  chance or so), nothing happens. Otherwise, the learner will demote the highest "
	"violated constraint (i.e., the one that is most disharmonic during her stochastic evaluation) "
	"in the heard quatrain that is not (or less) violated in the winner (the quatrain that she generated herself). "
	"She will also promote the highest violated constraint in the winner "
	"that is not (or less) violated in the heard quatrain.")
NORMAL (L"We are next going to generate a set of 589 quatrains, in order to be able to compare "
	"the behaviours of our folk-verse grammar and the English folk-verse writers:")
LIST_ITEM (L"1. Select the learned grammar and generate 589 (empty) input strings.")
LIST_ITEM (L"2. Select the learned grammar and the so-called input strings, "
	"and generate the output strings.")
LIST_ITEM (L"3. To see the distribution of the output strings, choose @@Strings: To Distributions@, "
	"and draw the resulting @Distributions object to your Picture window.")
LIST_ITEM (L"4. You can now compare the two distributions.")
NORMAL (L"Instead of generating the data from a @Distributions, you could have generated them from "
	"the target grammar in table (9) of Hayes & MacEachern (1998). "
	"Such a grammar is in your #demo folder (##folkVerse59.OTGrammar#) "
	"or at ##http://www.fon.hum.uva.nl/praat/folkVerse59.OTGrammar#. "
	"Because of the loosening of the tie between two of the constraints (see H & McE, fn. 43), "
	"this grammar will give different distributions from the \"actual\" values, "
	"but our algorithm will learn them correctly, provided you choose ##Symmetric all# "
	"or ##weighted uncancelled# for the learning strategy.")
MAN_END
*/

MAN_BEGIN (L"OTGrammar", L"ppgb", 20041110)
INTRO (L"One of the @@types of objects@ in Praat. See the @@OT learning@ tutorial.")
ENTRY (L"Inside an OTGrammar")
NORMAL (L"With @Inspect, you will see the following attributes:")
TAG (L"%constraints")
DEFINITION (L"a list of constraints. Each constraint contains the following attributes:")
TAG1 (L"%name")
DEFINITION1 (L"the fixed name of the constraint, for instance \"P\\s{ARSE}\".")
TAG1 (L"%ranking")
DEFINITION1 (L"the continuous ranking value; will change during learning.")
TAG1 (L"%disharmony")
DEFINITION1 (L"the effective ranking value during stochastic evaluation; with a non-zero evaluation noise, "
	"this will be different from %ranking.")
TAG (L"%fixedRankings")
DEFINITION (L"an often empty list of locally ranked pairs of constraints. "
	"Each local-ranking pair contains the following attributes:")
TAG1 (L"%higher")
DEFINITION1 (L"the index of the universally higher-ranked of the two constraints, "
	"a number between 1 and the number of constraints.")
TAG1 (L"%lower")
DEFINITION1 (L"the index of the universally lower-ranked of the two constraints.")
TAG (L"%tableaus")
DEFINITION (L"a list of tableaus. Each tableau contains the following attributes:")
TAG1 (L"%input")
DEFINITION1 (L"the input string of the tableau. For production grammaras, the underlying form of the utterance, for example "
	"\\|fan+pa\\|f or \\|fb\\rt\\ic\\ng + \\s{PAST}\\|f.")
TAG1 (L"%candidates")
DEFINITION1 (L"a list of output candidates. Each output candidate consists of:")
TAG2 (L"%output")
DEFINITION2 (L"the output string of the tableau. In generative phonology: the surface form of the utterance, for example "
	"[anpa] or [ampa] or [b\\rt\\ct:t] or [b\\rt\\ae\\ng]. In functional phonology: the combination of the articulatory "
	"and the perceptual results, for example [anpa]-/anpa/ or [ampa]-/ampa/ or [b\\rt\\ct:t]-/b\\rt\\ct:t/ or "
	"[b\\rt\\ae\\ng]-/b\\rt\\ae\\ng/.")
TAG2 (L"%marks")
DEFINITION2 (L"a list of the number of violations of each constraint for this output form. If there are 13 constraints, "
	"this list will contain 13 integer numbers for each candidate.")
ENTRY (L"OTGrammar creation")
NORMAL (L"You can easily create some #OTGrammar examples from the @@New menu@, "
	"or type your own grammars into a text file and read the file into Praat. See the @@OT learning@ tutorial.")
ENTRY (L"OTGrammar actions")
NORMAL (L"You can perform the following actions on selected #OTGrammar objects:")
LIST_ITEM (L"@@OTGrammar: Generate inputs...@")
/*LIST_ITEM (L"@@OTGrammar: Sort...@")*/
LIST_ITEM (L"@@OTGrammar: Input to output...@")
LIST_ITEM (L"@@OTGrammar: Input to outputs...@ (compute the output distribution for a given input)")
LIST_ITEM (L"@@OTGrammar: To output Distributions...@")
LIST_ITEM (L"@@OTGrammar & Strings: Inputs to outputs...@")
LIST_ITEM (L"@@OTGrammar: Learn one...@")
LIST_ITEM (L"@@OTGrammar & 2 Strings: Learn...@")
NORMAL (L"You can view an #OTGrammar in an @OTGrammarEditor.")
MAN_END

MAN_BEGIN (L"OTGrammar: Generate inputs...", L"ppgb", 19981230)
INTRO (L"A command to create a @Strings object from a selected @OTGrammar.")
NORMAL (L"A practical grammar-specific implementation of the %%richness of the base%: "
	"the inputs are drawn at random with equal probabilities from the inputs associated with the tableaus. "
	"For an example, see @@OT learning 3.2. Data from another grammar@.")
ENTRY (L"Setting")
TAG (L"##Number of trials")
DEFINITION (L"the number of times a string will be drawn from the possible inputs to the grammar.")
MAN_END

MAN_BEGIN (L"OTGrammar: Input to output...", L"ppgb", 20030916)
INTRO (L"A command to ask the selected @OTGrammar object to evaluate the candidates associated with a specified input form.")
NORMAL (L"See @@OT learning 2.8. Asking for one output@ for tutorial information.")
ENTRY (L"Settings")
TAG (L"##Input form")
DEFINITION (L"the input form whose surface form you want to know. If this string is not in the list "
	"of the possible inputs of the selected OTGrammar, you will get an error message.")
TAG (L"##Noise# (standard value: 2.0)")
DEFINITION (L"the standard deviation of the noise added to the ranking value of every constraint during evaluation. "
	"See @@OT learning 2.4. Evaluation@.")
MAN_END

MAN_BEGIN (L"OTGrammar: Input to outputs...", L"ppgb", 20030916)
INTRO (L"A command to ask the selected @OTGrammar object to evaluate a number of times the candidates associated "
	"with a specified input form. The result is a @Distributions object.")
NORMAL (L"See @@OT learning 2.9. Output distributions@ for tutorial information and examples.")
ENTRY (L"Settings")
TAG (L"##Trials# (standard value: 1000)")
DEFINITION (L"the number of evaluations that you want to perform.")
TAG (L"##Noise# (standard value: 2.0)")
DEFINITION (L"the standard deviation of the noise added to the ranking value of every constraint during the evaluations. "
	"See @@OT learning 2.4. Evaluation@.")
TAG (L"##Input form")
DEFINITION (L"the input form whose surface forms you want to measure. If this string is not in the list "
	"of the possible inputs of the selected OTGrammar, you will get an error message.")
MAN_END

MAN_BEGIN (L"OTGrammar: Learn one...", L"ppgb", 20011120)
INTRO (L"Causes every selected @OTGrammar object to process one input/output pair "
	"according to the Gradual Learning Algorithm "
	"by @@Boersma (1998)@ and @@Boersma (2000)@. See @@OT learning 4. Learning an ordinal grammar@ "
	"and @@OT learning 5. Learning a stochastic grammar@.")
MAN_END

MAN_BEGIN (L"OTGrammar: To output Distributions...", L"ppgb", 19981230)
INTRO (L"A command to ask the selected @OTGrammar object to evaluate a number of times the candidates associated "
	"with every input form. The result is a @Distributions object. See @@OT learning 2.9. Output distributions@.")
MAN_END

MAN_BEGIN (L"OTGrammar & PairDistribution: Find positive weights...", L"ppgb", 20080331)
INTRO (L"A command to change the weights of the selected @OTGrammar on the basis of the language data "
	"in the selected @PairDistribution.")
NORMAL (L"This command works only if the decision strategy of the selected OTGrammar is "
	"HarmonicGrammar, LinearOT, PositiveHG, or ExponentialHG. Also, "
	"the selected PairDistribution must contain exactly one non-zero-probability output for every possible input of the grammar.")
NORMAL (L"The procedure follows the linear programming method by @@Pater, Potts & Bhatt (2007)@. "
	"This method tries to find a special correct weighting of the constraints, "
	"namely one that minimizes the sum of the constraint weights.")
ENTRY (L"Settings")
TAG (L"##Weight floor# (standard value: 1.0)")
DEFINITION (L"After the command finishes, every weight will have at least this value.")
TAG (L"##Margin of separation# (standard value: 1.0)")
DEFINITION (L"After the command finishes, the harmony of every optimal (and correct) output candidate "
	"will be at least this much greater than the harmony of any competitor in the same tableau.")
MAN_END

MAN_BEGIN (L"OTGrammar & Strings: Inputs to outputs...", L"ppgb", 19981230)
INTRO (L"An action that creates a @Strings object from a selected @OTGrammar and a selected @Strings.")
NORMAL (L"The selected Strings object is considered as a list of inputs to the OTGrammar grammar.")
ENTRY (L"Settings")
TAG (L"##Noise")
DEFINITION (L"the standard deviation of the noise that will be temporarily added to the ranking value at each evaluation.")
NORMAL (L"The resulting Strings object will contain the output string of the grammar for each of the input strings.")
NORMAL (L"See @@OT learning 3.2. Data from another grammar@.")
MAN_END

MAN_BEGIN (L"OTGrammar & 2 Strings: Learn...", L"ppgb", 20100331)
INTRO (L"Causes the selected @OTGrammar object to process a number of input/output pairs "
	"according to the Gradual Learning Algorithm by @@Boersma (1997)@ and @@Boersma & Hayes (2001)@ "
	"or with any other reranking strategies. See @@OT learning 4. Learning an ordinal grammar@ "
	"and @@OT learning 5. Learning a stochastic grammar@.")
MAN_END

MAN_BEGIN (L"OTGrammarEditor", L"ppgb", 20030316)
INTRO (L"One of the @editors in Praat, for viewing and editing the grammar in an @OTGrammar object.")
NORMAL (L"See the @@OT learning@ tutorial for examples.")
ENTRY (L"Usage")
NORMAL (L"The menu command that you will probably use most often if you investigate variation, "
	"is the ##Evaluate (noise 2.0)# command, which you can invoke from the #Edit menu or by pressing Command-2.")
NORMAL (L"This command performs a new evaluation with the current ranking values. Some noise is added to the "
	"ranking values, so that the %#disharmonies of the constraint will change. This may cause a change in the "
	"ranking order of the constraints, which in its turn may cause a different candidate to win in some tableaus.")
MAN_END

MAN_BEGIN (L"Robust Interpretive Parsing", L"ppgb", 20021105)
INTRO (L"The mapping from overt forms to surface forms in the acquisition model by @@Tesar & Smolensky (1998)@.")
NORMAL (L"In Praat, you can do robust interpretive parsing on any @OTGrammar object. "
	"See @@OT learning 7. Learning from overt forms@.")
MAN_END

}

/* End of file manual_gram.cpp */
