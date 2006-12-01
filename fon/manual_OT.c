/* manual_OT.c
 *
 * Copyright (C) 1997-2006 Paul Boersma
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

/*
 * pb 2006/01/05
 */

#include "ManPagesM.h"
#include "OTGrammar.h"

static void draw_NoCoda_pat (Graphics g) {
	OTGrammar ot = OTGrammar_create_NoCoda_grammar ();
	OTGrammar_drawTableau (ot, g, "pat");
	forget (ot);
}
static void draw_NoCoda_pa (Graphics g) {
	OTGrammar ot = OTGrammar_create_NoCoda_grammar ();
	OTGrammar_drawTableau (ot, g, "pa");
	forget (ot);
}
static void draw_NoCoda_reverse (Graphics g) {
	OTGrammar ot = OTGrammar_create_NoCoda_grammar ();
	ot -> index [1] = 2;
	ot -> index [2] = 1;
	OTGrammar_drawTableau (ot, g, "pat");
	forget (ot);
}
static void draw_NPA_assimilate_anpa (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 1;
	ot -> index [3] = 2;
	OTGrammar_drawTableau (ot, g, "an+pa");
	forget (ot);
}
static void draw_NPA_assimilate_atma (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 1;
	ot -> index [3] = 2;
	OTGrammar_drawTableau (ot, g, "at+ma");
	forget (ot);
}
static void draw_NPA_faithful_anpa (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 2;
	ot -> index [3] = 1;
	OTGrammar_drawTableau (ot, g, "an+pa");
	forget (ot);
}
static void draw_NPA_faithful_atma (Graphics g) {
	OTGrammar ot = OTGrammar_create_NPA_grammar ();
	ot -> index [1] = 3;
	ot -> index [2] = 2;
	ot -> index [3] = 1;
	OTGrammar_drawTableau (ot, g, "at+ma");
	forget (ot);
}
static void draw_Wolof_ItI (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, "\\ict\\ic");
	forget (ot);
}
static void draw_Wolof_itE (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, "it\\ep");
	forget (ot);
}
static void draw_Wolof_etE (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, "et\\ep");
	forget (ot);
}
static void draw_Wolof_schwatschwa (Graphics g) {
	OTGrammar ot = OTGrammar_create_tongueRoot_grammar (1, 4);
	OTGrammar_drawTableau (ot, g, "\\swt\\sw");
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
	OTGrammar_drawTableau (ot, g, "\\swt\\ic");
	forget (ot);
}

void manual_OT_init (ManPages me);
void manual_OT_init (ManPages me) {

MAN_BEGIN ("Constraints", "ppgb", 20021105)
INTRO ("In @@Optimality Theory@, the `rules' that an output form has to satisfy. Since there can be many constraints "
	"and these constraints can conflict with each other, the constraints are %violable and the highest-ranked constraints "
	"have the largest say in determining the optimal output.")
NORMAL ("See the @@OT learning@ tutorial for many examples.")
MAN_END

MAN_BEGIN ("Create tongue-root grammar...", "ppgb", 20021204)
INTRO ("A command in the @@New menu@ for creating an @OTGrammar object with a tongue-root-harmony grammar.")
NORMAL ("These OTGrammar grammars only accept inputs of the form V__1_tV__2_, where V__1_ and V__2_ are "
	"chosen from the six front vowels i, \\ic, e, \\ep, \\sw, and a. In a text field, these "
	"vowels should be typed as $$##i#$, $$##\\bsic#$, $$##e#$, $$##\\bsep#$, $$##\\bssw#$, "
	"and $$##a#$, respectively (see @@Special symbols@).")
NORMAL ("The following phonological features are relevant:")
LIST_ITEM ("\t\tATR\tRTR")
LIST_ITEM ("\thigh\ti\t\\ic")
LIST_ITEM ("\tmid\te\t\\ep")
LIST_ITEM ("\tlow\t\\sw\ta")
ENTRY ("Constraints")
NORMAL ("The resulting OTGrammar will usually contain at least the following five constraints:")
TAG ("*[rtr / hi]")
DEFINITION ("\"do not implement [retracted tongue root] if the vowel is high.\"")
TAG ("*[atr / lo]")
DEFINITION ("\"do not implement [advanced tongue root] if the vowel is low.\"")
TAG ("P\\s{ARSE} (rtr)")
DEFINITION ("\"make an underlying [retracted tongue root] specification surface.\"")
TAG ("P\\s{ARSE} (atr)")
DEFINITION ("\"make an underlying [advanced tongue root] specification surface.\"")
TAG ("*G\\s{ESTURE} (contour)")
DEFINITION ("\"do not go from advanced to retracted tongue root, nor the other way around, within a word.\"")
NORMAL ("This set of constraints thus comprises: ")
LIST_ITEM ("\\bu two %%##grounding conditions#% (@@Archangeli & Pulleyblank (1994)@), "
	"which we can see as gestural constraints;")
LIST_ITEM ("\\bu two %%##faithfulness constraints#%, which favour the similarity between input and output, "
	"and can be seen as implementing the principle of maximization of perceptual contrast;")
LIST_ITEM ("\\bu a %%##harmony constraint#%, which, if crucially ranked higher than at least one faithfulness constraint, "
	"forces %%##tongue-root harmony#%.")
NORMAL ("In addition, there may be the following four constraints:")
TAG ("*[rtr / mid]")
DEFINITION ("\"do not implement [retracted tongue root] if the vowel is mid; universally ranked lower "
	"than *[rtr / hi].\"")
TAG ("*[rtr / lo]")
DEFINITION ("\"do not implement [retracted tongue root] if the vowel is low; universally ranked lower "
	"than *[rtr / mid].\"")
TAG ("*[atr / mid]")
DEFINITION ("\"do not implement [advanced tongue root] if the vowel is mid; universally ranked lower "
	"than *[atr / lo].\"")
TAG ("*[atr / hi]")
DEFINITION ("\"do not implement [advanced tongue root] if the vowel is high; universally ranked lower "
	"than *[atr / mid].\"")
NORMAL ("The universal rankings referred to are due to the %%##local-ranking principle#% (@@Boersma (1998)@). "
	"A learning algorithm may enforce this principle, e.g., if *[rtr / hi] falls down the ranking scale, "
	"*[rtr / mid] may be pushed along.")
NORMAL ("For information on learning these tongue-root grammars, see @@OT learning@ "
	"and @@Boersma (2000)@.")
MAN_END

MAN_BEGIN ("Optimality Theory", "ppgb", 20021105)
INTRO ("A framework for transferring one linguistic representation into another, "
	"e.g. transferring an underlying form into a surface form. Before @@Prince & Smolensky (1993)@, "
	"phonologists tended to this with a sequentially ordered set of rules, each of which transferred a representation "
	"into another. With @OT (that's the abbreviation), there are no intermediate steps in the derivation, but a set of ranked "
	"@constraints chooses the optimal output form from a set of candidates.")
NORMAL ("In P\\s{RAAT}, you can draw Optimality-Theoretic tableaus and simulate Optimality-Theoretic learning. "
	"See the @@OT learning@ tutorial.")
MAN_END

MAN_BEGIN ("OT", "ppgb", 20021105)
INTRO ("An abbreviation for @@Optimality Theory@.")
MAN_END

MAN_BEGIN ("OT learning", "ppgb", 20030316)
INTRO ("This tutorial describes how you can draw Optimality-Theoretic tableaus and "
	"simulate Optimality-Theoretic learning with P\\s{RAAT}.")
NORMAL ("You can read this tutorial sequentially with the help of the \"< 1\" and \"1 >\" buttons.")
LIST_ITEM ("1. @@OT learning 1. Kinds of OT grammars|Kinds of OT grammars@ (ordinal and stochastic, @OTGrammar)")
LIST_ITEM ("2. @@OT learning 2. The grammar|The grammar@")
LIST_ITEM1 ("2.1. @@OT learning 2.1. Viewing a grammar|Viewing a grammar@ (N\\s{O}C\\s{ODA} example, @OTGrammarEditor)")
LIST_ITEM1 ("2.2. @@OT learning 2.2. Inside the grammar|Inside the grammar@ (saving, inspecting)")
LIST_ITEM1 ("2.3. @@OT learning 2.3. Defining your own grammar|Defining your own grammar@")
LIST_ITEM1 ("2.4. @@OT learning 2.4. Evaluation|Evaluation@ (noise)")
LIST_ITEM1 ("2.5. @@OT learning 2.5. Editing a grammar|Editing a grammar@")
LIST_ITEM1 ("2.6. @@OT learning 2.6. Variable output|Variable output@ (place assimilation example)")
LIST_ITEM1 ("2.7. @@OT learning 2.7. Tableau pictures|Tableau pictures@ (printing, EPS)")
LIST_ITEM1 ("2.8. @@OT learning 2.8. Asking for one output|Asking for one output@")
LIST_ITEM1 ("2.9. @@OT learning 2.9. Output distributions|Output distributions@")
LIST_ITEM ("3. @@OT learning 3. Generating language data|Generating language data@")
LIST_ITEM1 ("3.1. @@OT learning 3.1. Data from a pair distribution|Data from a pair distribution@")
LIST_ITEM1 ("3.2. @@OT learning 3.2. Data from another grammar|Data from another grammar@ (tongue-root-harmony example)")
LIST_ITEM ("4. @@OT learning 4. Learning an ordinal grammar|Learning an ordinal grammar@")
LIST_ITEM ("5. @@OT learning 5. Learning a stochastic grammar|Learning a stochastic grammar@")
LIST_ITEM ("6. @@OT learning 6. Shortcut to OT learning|Shortcut to OT learning@")
LIST_ITEM ("7. @@OT learning 7. Learning from overt forms|Learning from overt forms@")
MAN_END

MAN_BEGIN ("OT learning 1. Kinds of OT grammars", "ppgb", 20041110)
INTRO ("This is chapter 1 of the @@OT learning@ tutorial.")
NORMAL ("According to @@Prince & Smolensky (1993)@, an @@Optimality Theory|Optimality-Theoretic@ (@OT) grammar "
	"consists of a number of ranked @constraints. "
	"For every possible input (usually an underlying form), GEN (the generator) generates a (possibly very large) number of "
	"%%##output candidates%#, and the ranking order of the constraints determines the winning candidate, "
	"which becomes the single optimal output.")
NORMAL ("In OT, ranking is %#strict, i.e., if a constraint %A is ranked higher than the constraints %B, %C, and %D, "
	"a candidate that violates only constraint %A will always be beaten by any candidate that respects %A "
	"(and any higher constraints), even if it violates %B, %C, and %D.")
ENTRY ("Ordinal OT grammars")
NORMAL ("Because only the ranking order of the constraints plays a role in evaluating the output candidates, "
	"Prince & Smolensky took the grammar to contain no absolute ranking values, i.e., they accepted only an ordinal relation "
	"between the constraint rankings. For such a grammar, @@Tesar & Smolensky (1998)@ devised a learning algorithm "
	"(Error-Driven Constraint Demotion, EDCD) that changes the ranking order "
	"whenever the form produced by the learner is different from the adult form. Such a learning step "
	"can sometimes lead to a large change in the behaviour of the grammar.")
ENTRY ("Stochastic OT grammars")
NORMAL ("The EDCD algorithm is fast and convergent. As a model of language acquisition, however, its drawbacks are that it "
	"is extremely sensitive to errors in the learning data and that it does not show realistic gradual learning curves. "
	"For these reasons, @@Boersma (1997)@ / @@Boersma (1998)@ / @@Boersma (2000)@ "
	"proposed stochastic constraint grammars "
	"in which every constraint has a %%##ranking value%# along a continuous ranking scale, "
	"and a small amount of %#noise is added to this ranking value at evaluation time. "
	"The associated error-driven learning algorithm (Gradual Learning Algorithm, GLA) effects small changes in the "
	"ranking values of the constraints with every learning step. An added virtue of the GLA is that it can learn "
	"languages with optionality and variation, which was something that EDCD could not do.")
NORMAL ("Ordinal OT grammars can be seen as a special case of the more general stochastic OT grammars: "
	"they have integer ranking values (%#strata) and zero evaluation noise. "
	"In P\\s{RAAT}, therefore, every constraint is taken to have a ranking value, "
	"so that you can do stochastic as well as ordinal OT.")
NORMAL ("An OT grammar is implemented as an @OTGrammar object. "
	"In an OTGrammar object, you specify all the constraints, all the possible inputs and all their possible outputs.")
MAN_END

MAN_BEGIN ("OT learning 2. The grammar", "ppgb", 20000122)
INTRO ("This is chapter 2 of the @@OT learning@ tutorial.")
NORMAL ("We can ask the grammar to produce an output form for any input form that is in its list of tableaus.")
LIST_ITEM ("2.1. @@OT learning 2.1. Viewing a grammar|Viewing a grammar@ (N\\s{O}C\\s{ODA} example, @OTGrammarEditor)")
LIST_ITEM ("2.2. @@OT learning 2.2. Inside the grammar|Inside the grammar@ (saving, inspecting)")
LIST_ITEM ("2.3. @@OT learning 2.3. Defining your own grammar|Defining your own grammar@")
LIST_ITEM ("2.4. @@OT learning 2.4. Evaluation|Evaluation@ (noise)")
LIST_ITEM ("2.5. @@OT learning 2.5. Editing a grammar|Editing a grammar@")
LIST_ITEM ("2.6. @@OT learning 2.6. Variable output|Variable output@ (place assimilation example)")
LIST_ITEM ("2.7. @@OT learning 2.7. Tableau pictures|Tableau pictures@ (printing, EPS files)")
LIST_ITEM ("2.8. @@OT learning 2.8. Asking for one output|Asking for one output@")
LIST_ITEM ("2.9. @@OT learning 2.9. Output distributions|Output distributions@")
MAN_END

MAN_BEGIN ("OT learning 2.1. Viewing a grammar", "ppgb", 20021204)
NORMAL ("Consider a language where the underlying form /pat/ leads to the surface form [pa], "
	"presumably because the structural constraint N\\s{O}C\\s{ODA} outranks the faithfulness constraint P\\s{ARSE}.")
NORMAL ("To create such a grammar in P\\s{RAAT}, choose ##Create NoCoda grammar# from the Optimality Theory submenu of the @@New menu@. "
	"An @OTGrammar object will then appear in the list of objects. "
	"If you click Edit, an @OTGrammarEditor will show up, containing:")
LIST_ITEM ("1. the constraint list, sorted by %#disharmony (= ranking value + noise):")
LIST_ITEM1 (" ")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      100.000")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      90.000")
LIST_ITEM1 (" ")
LIST_ITEM ("2. the tableaus for the two possible inputs /pat/ and /pa/:")
PICTURE (3.0, 1.0, draw_NoCoda_pat)
PICTURE (3.0, 0.7, draw_NoCoda_pa)
NORMAL ("From the first tableau, we see that the underlying form /pat/ will surface as [pa], "
	"because the alternative [pat] violates a constraint (namely, N\\s{O}C\\s{ODA}) with a higher disharmony than does [pa], "
	"which only violates P\\s{ARSE}, which has a lower disharmony.")
NORMAL ("Note the standard OT tableau layout: asterisks (*) showing violations, exclamation marks (!) showing crucial violations, "
	"greying of cells that do not contribute to determining the winning candidate, and a finger (\\pf) pointing to the winner "
	"(this may look like a plus sign (+) if you don't have the Zapf Dingbats font installed on your computer or printer).")
NORMAL ("The second tableau shows that /pa/ always surfaces as [pa], which is no wonder since this is "
	"the only candidate. All cells are grey because none of them contributes to the determination of the winner.")
MAN_END

MAN_BEGIN ("OT learning 2.2. Inside the grammar", "ppgb", 20060202)
NORMAL ("You can write an @OTGrammar grammar into a text file by choosing @@Write to text file...@ from the Write menu "
	"of the Objects window. For the N\\s{O}C\\s{ODA} example, the contents of the file will look like:")
CODE ("File type = \"ooTextFile\"")
CODE ("Object class = \"OTGrammar 1\"")
CODE ("decisionStrategy = <OptimalityTheory>")
CODE ("2 constraints")
CODE ("constraint [1]: \"N\\bss{O}C\\bss{ODA}\" 100 100 ! NOCODA")
CODE ("constraint [2]: \"P\\bss{ARSE}\" 90 90 ! PARSE")
CODE (" ")
CODE ("0 fixed rankings")
CODE (" ")
CODE ("2 tableaus")
CODE ("input [1]: \"pat\" 2")
CODE1 ("candidate [1]: \"pa\" 0 1")
CODE1 ("candidate [2]: \"pat\" 1 0")
CODE ("input [2]: \"pa\" 1")
CODE1 ("candidate [1]: \"pa\" 0 0")
NORMAL ("To understand more about this data structure, consult the @OTGrammar class description "
	"or click #Inspect after selecting the OTGrammar object. The $$\"\\bss{...}\"$ braces ensure that "
	"the constraint names show up with their traditional small capitals (see @@Text styles@).")
NORMAL ("You can read this text file into Praat again with @@Read from file...@ from the Read menu in the Objects window.")
MAN_END

MAN_BEGIN ("OT learning 2.3. Defining your own grammar", "ppgb", 20060105)
NORMAL ("By editing a text file created from an example in the @@New menu@, you can define your own OT grammars.")
NORMAL ("As explained at @@Write to text file...@, Praat is quite resilient about its text file formats. "
	"As long as the strings and numbers appear in the correct order, you can redistribute the data "
	"across the lines, add all kinds of comments, or leave the comments out. "
	"For the N\\s{O}C\\s{ODA} example, the text file could also have looked like:")
CODE ("\"ooTextFile\"")
CODE ("\"OTGrammar 1\"")
CODE ("<OptimalityTheory>")
CODE ("2")
CODE ("\"N\\bss{O}C\\bss{ODA}\" 100 100")
CODE ("\"P\\bss{ARSE}\"       90  90")
CODE ("0   ! number of fixed rankings")
CODE ("2   ! number of accepted inputs")
CODE ("\"pat\" 2      ! input form with number of output candidates")
CODE1 ("\"pa\"  0 1   ! first candidate with violations")
CODE1 ("\"pat\" 1 0   ! second candidate with violations")
CODE ("\"pa\" 1       ! input form with number of candidates")
CODE1 ("\"pa\"  0 0")
NORMAL ("To define your own grammar, you just provide a number of constraints and their rankings, "
	"and all the possible input forms with all their output candidates, and all the constraint violations "
	"for each candidate. The order in which you specify the constraints is free (you don't have to specify "
	"the highest-ranked first), as long as the violations are in the same order; you could also have reversed "
	"the order of the two input forms, as long as the corresponding candidates follow them; "
	"and, you could also have reversed the order of the candidates within the /pat/ tableau, "
	"as long as the violations follow the output forms. Thus, you could just as well have written:")
CODE ("\"ooTextFile\"")
CODE ("\"OTGrammar 1\"")
CODE ("<OptimalityTheory>")
CODE ("2")
CODE ("\"P\\bss{ARSE}\"       90  90")
CODE ("\"N\\bss{O}C\\bss{ODA}\" 100 100")
CODE ("0")
CODE ("2")
CODE ("\"pa\" 1")
CODE1 ("\"pa\"  0 0")
CODE ("\"pat\" 2")
CODE1 ("\"pat\" 0 1")
CODE1 ("\"pa\"  1 0")
NORMAL ("The $$<OptimalityTheory>$ thing in the above refers to the %%decision strategy%. "
	"In this tutorial I assume OT's strict ranking throughout, "
	"but you can experiment with Smolensky's $$<HarmonicGrammar>$ (where the constraint rankings represent addable, "
	"possibly negative weights), or with Frank Keller's $$<LinearOT>$ (like Harmonic Grammar, but with the restriction "
	"that there are no negative weights), or with $$<ExponentialHG>$ (where the weights are exp(disharmony), combining "
	"the continuity advantage of Harmonic Grammar with the positive-definiteness advantage of Linear OT).")
MAN_END

MAN_BEGIN ("OT learning 2.4. Evaluation", "ppgb", 20021105)
NORMAL ("In an Optimality-Theoretic model of grammar, %#evaluation refers to the determination "
	"of the winning candidate on the basis of the constraint ranking.")
NORMAL ("In an ordinal OT model of grammar, repeated evaluations will yield the same winner again and again. "
	"We can simulate this behaviour with our N\\s{O}C\\s{ODA} example. "
	"In the editor, you can choose ##Evaluate (zero noise)# or use its keyboard shortcut Command-0 (= Command-zero). "
	"Repeated evaluations (keep Command-0 pressed) will always yield the following grammar:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      100.000")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      90.000")
NORMAL ("In a stochastic OT model of grammar, repeated evaluations will yield different disharmonies each time. "
	"To see this, choose ##Evaluate (noise 2.0)# or use its keyboard shortcut Command-2. "
	"Repeated evaluations will yield grammars like the following:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      100.427")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      87.502")
NORMAL ("and")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      101.041")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      90.930")
NORMAL ("and")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      96.398")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      89.482")
NORMAL ("The disharmonies vary around the ranking values, "
	"according to a Gaussian distribution with a standard deviation of 2.0. "
	"The winner will still be [pa] in almost all cases, because the probability of bridging "
	"the gap between the two ranking values is very low, namely 0.02 per cent according "
	"to @@Boersma (1998)@, page 332.")
NORMAL ("With a noise much higher than 2.0, the chances of P\\s{ARSE} outranking N\\s{O}C\\s{ODA} will rise. "
	"To see this, choose ##Evaluate...# and supply 5.0 for the noise. Typical outcomes are:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      92.634")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      86.931")
NORMAL ("and")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      101.162")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      85.311")
NORMAL ("and")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      99.778")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      100.000\t      98.711")
NORMAL ("In the last case, the order of the constraints has been reversed. "
	"You will see that [pat] has become the winning candidate:")
PICTURE (3.0, 1.0, draw_NoCoda_reverse)
NORMAL ("However, in the remaining part of this tutorial, we wil stick with a noise "
	"with a standard deviation of 2.0. This specific number ensures that we can "
	"model fairly rigid rankings by giving the constraints a ranking difference of 10, a nice round number. "
	"Also, the learning algorithm will separate many constraints in such a way that "
	"the differences between their ranking values are in the vicinity of 10.")
MAN_END

MAN_BEGIN ("OT learning 2.5. Editing a grammar", "ppgb", 20000202)
NORMAL ("In the N\\s{O}C\\s{ODA} example, the winning candidate for the input /pat/ was always [pa].")
NORMAL ("To make [pat] the winner instead, N\\s{O}C\\s{ODA} should be ranked lower than P\\s{ARSE}. "
	"To achieve this even with zero noise, go to the editor and select the N\\s{O}C\\s{ODA} constraint by clicking on it "
	"(a spade symbol \\sp will mark the selected constraint), "
	"and choose ##Edit ranking...# from the Edit menu, or use the keyboard shortcut Command-E.")
NORMAL ("In the resulting dialog, we lower the ranking of the constraint from 100 to 80, and click OK. "
	"This is what you will see in the editor:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t\\sp ##N\\s{O}C\\s{ODA}#\t      80.000\t      103.429")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      88.083")
PICTURE (3.0, 1.0, draw_NoCoda_pat)
NORMAL ("Nothing has happened to the tableau, because the disharmonies still have their old values. So choose "
	"##Evaluate (noise 2.0)# (Command-2) or ##Evaluate (zero noise)# (Command-0). The new disharmonies "
	"will centre around the new ranking values, and we see that [pat] becomes the new winner:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      90.000\t      90.743")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      80.000\t      81.581")
PICTURE (3.0, 1.0, draw_NoCoda_reverse)
MAN_END

MAN_BEGIN ("OT learning 2.6. Variable output", "ppgb", 20060105)
NORMAL ("Each time you press Command-2, which invokes the command ##Evaluate (noise 2.0)# from the Edit menu, "
	"you will see the disharmonies changing. If the distance between the constraint rankings is 10, however, "
	"the winning candidates will very probably stay the same.")
NORMAL ("So starting from the N\\s{O}C\\s{ODA} example, we edit the rankings of the constraints again, "
	"setting the ranking value of P\\s{ARSE} to 88 and that of N\\s{O}C\\s{ODA} to 85. If we now press Command-2 "
	"repeatedly, we will get [pat] in most of the cases, "
	"but we will see the finger pointing at [pa] in 14 percent of the cases:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      88.000\t      87.421")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      85.000\t      85.585")
PICTURE (3.0, 1.0, draw_NoCoda_reverse)
NORMAL ("but")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##N\\s{O}C\\s{ODA}#\t      85.000\t      87.128")
LIST_ITEM1 ("\t##P\\s{ARSE}#\t      88.000\t      85.076")
PICTURE (3.0, 1.0, draw_NoCoda_pat)
NORMAL ("As a more functionally oriented example, we consider nasal place assimilation. "
	"Suppose that the underlying sequence /an+pa/ surfaces as the assimilated [ampa] "
	"in 80 percent of the cases, and as the faithful [anpa] in the remaining 20 percent, "
	"while the non-nasal stop /t/ never assimilates. "
	"This can be achieved by having the articulatory constraint *G\\s{ESTURE} "
	"ranked at a short distance above *R\\s{EPLACE} (n, m):")
CODE ("\"ooTextFile\"")
CODE ("\"OTGrammar 1\"")
CODE ("decisionStrategy = <OptimalityTheory>")
CODE ("3 constraints")
CODE ("\"*G\\bss{ESTURE}\"          102.7 0")
CODE ("\"*R\\bss{EPLACE} (n, m)\"   100.0 0")
CODE ("\"*R\\bss{EPLACE} (t, p)\"   112.0 0")
CODE ("0 fixed rankings")
CODE ("2 tableaus")
CODE ("\"an+pa\" 2")
CODE1 ("\"anpa\"  1 0 0")
CODE1 ("\"ampa\"  0 1 0")
CODE ("\"at+ma\" 2")
CODE1 ("\"atma\"  1 0 0")
CODE1 ("\"apma\"  0 0 1")
NORMAL ("You can create this grammar with ##Create place assimilation grammar# from the @@New menu@. "
	"In the editor, it will often look like follows:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (t, p)#\t      112.000\t      109.806")
LIST_ITEM1 ("\t##*G\\s{ESTURE}#\t      102.700\t      102.742")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (n, m)#\t      100.000\t      101.044")
PICTURE (4.0, 1.0, draw_NPA_assimilate_anpa)
PICTURE (4.0, 1.0, draw_NPA_assimilate_atma)
NORMAL ("If you keep the Command-2 keys pressed, however, you will see that the tableaus change "
	"into something like the following in approximately 20 percent of the cases:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (t, p)#\t      112.000\t      113.395")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (n, m)#\t      100.000\t      103.324")
LIST_ITEM1 ("\t##*G\\s{ESTURE}#\t      102.700\t      101.722")
PICTURE (4.0, 1.0, draw_NPA_faithful_anpa)
PICTURE (4.0, 1.0, draw_NPA_faithful_atma)
NORMAL ("We see that /at+ma/ always surfaces at [atma], because *R\\s{EPLACE} (t, p) is ranked much higher "
	"than the other two, and that the output of /an+pa/ is variable because of the close rankings "
	"of *G\\s{ESTURE} and *R\\s{EPLACE} (n, m).")
MAN_END

MAN_BEGIN ("OT learning 2.7. Tableau pictures", "ppgb", 20001010)
NORMAL ("To show a tableau in the @@Picture window@ instead of in the editor, "
	"you select an @OTGrammar object and click ##Draw tableau...#. "
	"After you specify the input form, a tableau is drawn with the current font and size "
	"at the location of the current selection (%viewport) in the Picture window. The top left corner of the tableau "
	"is aligned with the top left corner of the selection. You can draw more than one object into the Picture "
	"window, whose menus also allow you to add a lot of graphics of your own design.")
NORMAL ("Besides printing the entire picture (with @@Print...@), you can save a part of it to an EPS file "
	"for inclusion into your favourite word processor (with @@Write to EPS file...@). "
	"For the latter to succeed, make sure that the selection includes at least your tableau; "
	"otherwise, some part of your tableau may end up truncated.")
MAN_END

MAN_BEGIN ("OT learning 2.8. Asking for one output", "ppgb", 20030916)
NORMAL ("To ask the grammar to produce a single output for a specified input form, "
	"you can choose @@OTGrammar: Input to output...@. The dialog will ask you to provide "
	"an input form and the strength of the noise (the standard value is 2.0 again). "
	"This will perform an evaluation and write the result into the Info window.")
NORMAL ("If you are viewing the grammar in the @OTGrammarEditor, you will see the disharmonies change, "
	"and if the grammar allows variation, you will see that the winner in the tableau in the editor "
	"varies with the winner shown in the Info window.")
NORMAL ("Since the editor shows more information than the Info window, "
	"this command is not very useful except for purposes of scripting. "
	"See the following page for some related but more useful commands.")
MAN_END

MAN_BEGIN ("OT learning 2.9. Output distributions", "ppgb", 20000202)
NORMAL ("To ask the grammar to produce %many outputs for a specified input form, "
	"and collect them in a @Strings object, "
	"you select an @OTGrammar and choose @@OTGrammar: Input to outputs...|Input to outputs...@.")
NORMAL ("For example, select the object \"OTGrammar assimilation\" from our place assimilation example "
	"(@@OT learning 2.6. Variable output|\\SS2.6@), and click ##Input to outputs...#. "
	"In the resulting dialog, you specify 1000 trials, a noise strength of 2.0, and \"an+pa\" for the input form.")
NORMAL ("After you click OK, a @Strings object will appear in the list. "
	"If you click Info, you will see that it contains 1000 strings. "
	"If you click Inspect, you will see that most of the strings are \"ampa\", "
	"but some of them are \"anpa\". These are the output forms computed from 1000 evaluations "
	"for the input /an+pa/.")
NORMAL ("To count how many instances of [ampa] and [anpa] were generated, you select the @Strings object "
	"and click @@Strings: To Distributions|To Distributions@. You will see a new @Distributions object appear in the list. "
	"If you draw this to the Picture window (with ##Draw as numbers...#), you will see something like:")
LIST_ITEM ("\tampa\t815")
LIST_ITEM ("\tanpa\t185")
NORMAL ("which means that our grammar, when fed with 1000 /an+pa/ inputs, produced [ampa] 815 times, "
	"and [anpa] 185 times, which is consistent with our initial guess that a ranking difference of 2.7 "
	"would cause approximately an 80\\%  - 20\\%  distribution of [ampa] and [anpa].")
ENTRY ("Checking the distribution hypothesis")
NORMAL ("To see whether the guess of a 2.7 ranking difference is correct, we perform 1,000,000 trials instead of 1000. "
	"The output distribution (if you have enough memory in your computer) becomes something like "
	"(set the %Precision to 7 in the drawing dialog):")
LIST_ITEM ("\tampa\t830080")
LIST_ITEM ("\tanpa\t169920")
NORMAL ("The expected values under the 80\\%  - 20\\%  distribution hypothesis are:")
LIST_ITEM ("\tampa\t800000")
LIST_ITEM ("\tanpa\t200000")
NORMAL ("We compute (e.g. with @@Calculator...@) a %\\ci^2 of 30080^2/800000 + 30080^2/200000 = 5655.04, "
	"which, of course, is much too high for a distribution with a single degree of freedom. "
	"So the ranking difference must be smaller. If it is 2.4 (change the ranking of *G\\s{ESTURE} to 102.4), "
	"the numbers become something like")
LIST_ITEM ("\tampa\t801974")
LIST_ITEM ("\tanpa\t198026")
NORMAL ("which gives a %\\ci^2 of 24.35. By using the Calculator with the formula $$chiSquareQ (24.35, 1)$, "
	"we find that values larger than this have a probability of 8\\.c10^^-7^ "
	"under the 80\\%  - 20\\%  distribution hypothesis, which must therefore be rejected again.")
NORMAL ("Rather than continuing this iterative procedure to find the correct ranking values for an "
	"80\\%  - 20\\%  grammar, we will use the Gradual Learning Algorithm "
	"(@@OT learning 5. Learning a stochastic grammar|\\SS5@) to determine the rankings automatically, "
	"without any memory of past events other than the memory associated with maintaining the ranking values.")
ENTRY ("Measuring all inputs")
NORMAL ("To measure the outcomes of all the possible inputs at once, you select an @OTGrammar "
	"and choose @@OTGrammar: To output Distributions...|To output Distributions...@. "
	"As an example, try this on our place assimilation grammar. You can supply 1000000 for the number of trials, "
	"and the usual 2.0 for the standard deviation of the noise. "
	"After you click OK, a @Distributions object will appear in the list. "
	"If you draw this to the Picture window, the result will look like:")
LIST_ITEM ("\t/an+pa/ \\-> anpa\t169855")
LIST_ITEM ("\t/an+pa/ \\-> ampa\t830145")
LIST_ITEM ("\t/at+ma/ \\-> atma\t999492")
LIST_ITEM ("\t/at+ma/ \\-> apma\t508")
NORMAL ("We see that the number of [apma] outputs is not zero. This is due to the difference of 9.3 "
	"between the rankings of *R\\s{EPLACE} (t, p) and *G\\s{ESTURE}. If you rank "
	"*R\\s{EPLACE} (t, p) at 116.0, the number of produced [apma] reduces to about one in a million, "
	"as you can easily check with some patience.")
MAN_END

MAN_BEGIN ("OT learning 3. Generating language data", "ppgb", 20021204)
NORMAL ("A learner needs two things: a grammar that she can adjust (@@OT learning 2. The grammar|\\SS2@), and language data.")
LIST_ITEM ("3.1. @@OT learning 3.1. Data from a pair distribution|Data from a pair distribution@")
LIST_ITEM ("3.2. @@OT learning 3.2. Data from another grammar|Data from another grammar@ (tongue-root-harmony example)")
MAN_END

MAN_BEGIN ("OT learning 3.1. Data from a pair distribution", "ppgb", 20021105)
NORMAL ("If the grammar contains faithfulness constraints, the learner needs pairs of "
	"underlying and adult surface forms. For our place assimilation example, she needs a lot of "
	"/at+ma/ - [atma] pairs, and four times as many /an+pa/ - [ampa] pairs as /an+pa/ - [anpa] pairs. "
	"We can specify this language-data distribution in a @PairDistribution object, "
	"which we could simply write into a text file:")
CODE ("\"ooTextFile\"")
CODE ("\"PairDistribution\"")
CODE ("4 pairs")
CODE ("\"at+ma\"  \"atma\"  100")
CODE ("\"at+ma\"  \"apma\"    0")
CODE ("\"an+pa\"  \"anpa\"   20")
CODE ("\"an+pa\"  \"ampa\"   80")
NORMAL ("The values appear to represent percentages, but they could also have been 1.0, 0.0, 0.2, and 0.8, "
	"or any other values with the same proportions. We could also have left out the second pair "
	"and specified \"3 pairs\" instead of \"4 pairs\" in the third line.")
NORMAL ("We can create this pair distribution with ##Create place assimilation distribution# from the "
	"Optimality Theory submenu of the @@New menu@ in the Objects window. To see that it really contains "
	"the above data, you can draw it to the Picture window. To change the values, use Inspect "
	"(in which case you should remember to click Change after any change).")
NORMAL ("To generate input-output pairs from the above distribution, select the @PairDistribution and click "
	"@@PairDistribution: To Stringses...|To Stringses...@. "
	"If you then just click OK, there will appear two @Strings objects in the list, called \"input\" "
	"(underlying forms) and \"output\" (surface forms). Both contain 1000 strings. If you Inspect them both, "
	"you can see that e.g. the 377th string in \"input\" corresponds to the 377th string in \"output\", "
	"i.e., the two series of strings are aligned. See also the example at @@PairDistribution: To Stringses...@.")
NORMAL ("These two Strings objects are sufficient to help an @OTGrammar grammar to change its constraint rankings "
	"in such a way that the output distributions generated by the grammar match the output distributions "
	"in the language data. See @@OT learning 5. Learning a stochastic grammar|\\SS5@.")
MAN_END

MAN_BEGIN ("OT learning 3.2. Data from another grammar", "ppgb", 20021204)
NORMAL ("Instead of generating input-output pairs directly from a @PairDistribution object, "
	"you can also generate input forms and their winning outputs from an @OTGrammar grammar. Of course, "
	"that's what the language data presented to real children comes from. Our example will be "
	"a tongue-root harmony grammar.")
NORMAL ("Choose @@Create tongue-root grammar...@ from the Optimality Theory submenu of the @@New menu@. "
	"Set %%Constraint set% to \"Five\", and %Ranking to \"Wolof\". Click OK. An object called "
	"\"OTGrammar Wolof\" will appear in the list. Click #Edit. You will see the following grammar "
	"appear in the @OTGrammarEditor:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      100.000\t      100.000")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      50.000\t      50.000")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      30.000\t      30.000")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      20.000\t      20.000")
LIST_ITEM1 ("\t##*[atr / lo]#\t      10.000\t      10.000")
NORMAL ("This simplified Wolof grammar, with five constraints with clearly different rankings, is equivalent "
	"to the traditional OT ranking")
FORMULA ("*[rtr / hi] >> P\\s{ARSE} (rtr) >> *G\\s{ESTURE} (contour) >> P\\s{ARSE} (atr) >> *[atr / lo]")
NORMAL ("These constraints are based on a description of Wolof by "
	"@@Archangeli & Pulleyblank (1994)|Archangeli & Pulleyblank (1994: 225\\--239)@. "
	"For the meaning of these constraints, see @@Boersma (1998)|Boersma (1998: 295)@, "
	"or the @@Create tongue-root grammar...@ manual page.")
NORMAL ("For each input, there are four output candidates: "
	"the vowel heights will be the same as those in the input, but the tongue-root values of V__1_ and V__2_ are varied. "
	"For example, for the input [ita] we will have the four candidates "
	"[ita], [it\\sw], [\\icta], and [\\ict\\sw].")
NORMAL ("With this way of generating candidates, we see that the five constraints are completely ranked. "
	"First, the absolute prohibition on surface [\\ic] shows that *[rtr / hi] outranks RTR faithfulness "
	"(otherwise, [\\ict\\ic] would have been the winner):")
PICTURE (4.0, 1.5, draw_Wolof_ItI)
NORMAL ("Second, the faithful surfacing of the disharmonic /it\\ep/ shows that RTR faithfulness must outrank "
	"the harmony (anti-contour) constraint (otherwise, [ite] would have been the winner):")
PICTURE (4.0, 1.5, draw_Wolof_itE)
NORMAL ("Third, the RTR-dominant harmonicization of underlying disharmonic /et\\ep/ shows that harmony must outrank ATR faithfulness "
	"(otherwise, [et\\ep] would have won):")
PICTURE (4.0, 1.5, draw_Wolof_etE)
NORMAL ("Finally, the faithful surfacing of the low ATR vowel /\\sw/ even if not forced by harmony, shows that "
	"ATR faithfulness outranks *[atr / lo] (otherwise, [ata] would have been the winning candidate):")
PICTURE (4.0, 1.5, draw_Wolof_schwatschwa)
NORMAL ("These four ranking arguments clearly establish the crucial rankings of all five constraints.")
ENTRY ("Generating inputs from the grammar")
NORMAL ("According to @@Prince & Smolensky (1993)@, the input to an OT grammar can be %anything. "
	"This is the idea of %%##richness of the base%#. "
	"When doing a practical investigation, however, we are only interested in the inputs "
	"that will illustrate the properties of our partial grammars. "
	"In the case of simplified Wolof, this means the 36 possible V__1_tV__2_ sequences "
	"where V__1_ and V__2_ are any of the six front vowels i, \\ic, e, \\ep, \\sw, and a "
	"(see @@Create tongue-root grammar...@).")
NORMAL ("A set of inputs can be generated from an @OTGrammar object by inspecting the list of tableaus. "
	"So select the Wolof tongue-root grammar and choose @@OTGrammar: Generate inputs...|Generate inputs...@. "
	"Set %%Number of trials% to 100, and click OK. A @Strings object named \"Wolof_in\" "
	"will appear in the list. Click Inspect and examine the 100 input strings. "
	"You will see that they have been randomly chosen from the 36 possible V__1_tV__2_ sequences "
	"as described at @@Create tongue-root grammar...@:")
FORMULA ("\\epta, et\\ep, \\epti, it\\ep, \\ept\\ep, iti, \\ept\\ic, it\\ic, \\icti, et\\ep, ...")
NORMAL ("Thus, when asked to generate a random input, these grammars produce any of the 36 possible V__1_tV__2_ "
	"sequences, all with equal probability.")
ENTRY ("Generating outputs from the grammar")
NORMAL ("To compute the outputs for the above set of input forms, select %both the @OTGrammar object "
	"%and the input @Strings object, and choose @@OTGrammar & Strings: Inputs to outputs...|Inputs to outputs...@, "
	"perhaps specifying zero evaluation noise. "
	"A new Strings objects called \"Wolof_out\" will appear in the list. "
	"If you Inspect it, you will see that it contains a string sequence aligned with the original input strings:")
FORMULA ("\\epta, \\ept\\ep, \\epti, it\\ep, \\ept\\ep, iti, \\epti, iti, iti, \\ept\\ep, ...")
NORMAL ("In this way, we have created two Strings objects, which together form a series of input-output pairs "
	"needed for learning a grammar that contains faithfulness constraints.")
MAN_END

MAN_BEGIN ("OT learning 4. Learning an ordinal grammar", "ppgb", 20011120)
NORMAL ("With the data from a tongue-root-harmony language with five completely ranked constraints, "
	"we can have a throw at learning this language, starting with a grammar in which all the constraints "
	"are ranked at the same height, or randomly ranked, or with articulatory constraints outranking "
	"faithfulness constraints.")
NORMAL ("Let's try the third of these. Create an infant tongue-root grammar by choosing "
	"@@Create tongue-root grammar...@ and specifying \"Five\" for the constraint set "
	"and \"Infant\" for the ranking. The result after a single evaluation will be like:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      100.000\t      100.631")
LIST_ITEM1 ("\t##*[atr / lo]#\t      100.000\t      100.244")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      100.000\t      97.086")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      50.000\t      51.736")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      50.000\t      46.959")
NORMAL ("Such a grammar produces all kinds of non-adult results. For instance, the input /\\swt\\ic/ "
	"will surface as [at\\ic]:")
PICTURE (4.0, 1.5, draw_Infant_swtI)
NORMAL ("The adult form is very different: [\\swti]. The cause of the discrepancy is in the order of "
	"the constraints *[atr / lo] and *[rtr / hi], which militate against [\\sw] and [\\ic], respectively. "
	"Simply reversing the rankings of these two constraints would solve the problem in this case. "
	"More generally, @@Tesar & Smolensky (1998)@ prove that demoting all the constraints that cause the "
	"adult form to lose into the stratum just below the highest-ranked constraint "
	"violated in the learner's form (here, moving *[atr / lo] just below *[rtr / hi] "
	"into the same stratum as P\\s{ARSE} (rtr)), "
	"will guarantee convergence to the target grammar, "
	"%%if there is no variation in the data%.")
NORMAL ("But Tesar & Smolensky's algorithm cannot be used for variable data, since all constraints would be "
	"tumbling down, exchanging places and producing wildly different grammars at each learning step. "
	"Since language data do tend to be variable, we need a gradual and balanced learning algorithm, "
	"and the following algorithm is guaranteed to converge "
	"to the target language, if that language can be described by a stochastic OT grammar.")
NORMAL ("The reaction of the learner to hearing the mismatch between the adult [\\swti] and her own [at\\ic], "
	"is simply:")
LIST_ITEM ("1. to move the constraints violated in her own form, i.e. *[rtr / hi] and P\\s{ARSE} (atr), "
	"up by a small step along the ranking scale, thus decreasing the probability that her form will be the winner "
	"at the next evaluation of the same input;")
LIST_ITEM ("2. and to move the constraints violated in the adult form, namely *[atr / lo] and P\\s{ARSE} (rtr), "
	"down along the ranking scale, thus increasing the probability that the adult form will be the learner's "
	"winner the next time.")
NORMAL ("If the small reranking step (the %#plasticity) is 0.1, the grammar will become:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      100.000\t      100.631")
LIST_ITEM1 ("\t##*[atr / lo]#\t      99.900\t      100.244")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      100.100\t      97.086")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      49.900\t      51.736")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      50.100\t      46.959")
NORMAL ("The disharmonies, of course, will be different at the next evaluation, with a probability slightly higher "
	"than 50\\%  that *[rtr / hi] will outrank *[atr / lo]. Thus the relative rankings of these two grounding "
	"constraints have moved into the direction of the adult grammar, in which they are ranked at opposite "
	"ends of the grammar.")
NORMAL ("Note that the relative rankings of P\\s{ARSE} (atr) and P\\s{ARSE} (rtr) are "
	"now moving in a direction opposite to where they will have to end up in this RTR-dominant language. "
	"This does not matter: the procedure will converge nevertheless.")
NORMAL ("We are now going to simulate the infant who learns simplified Wolof. Take an adult Wolof grammar "
	"and generate 1000 input strings and the corresponding 1000 output strings "
	"following the procedure described in @@OT learning 3.2. Data from another grammar|\\SS3.2@. "
	"Now select the infant @OTGrammar and both @Strings objects, and choose @@OTGrammar & 2 Strings: Learn...|Learn...@. "
	"After you click OK, the learner processes each of the 1000 input-output pairs in succession, "
	"gradually changing the constraint ranking in case of a mismatch. The resulting grammar may look like:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      100.800\t      98.644")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      89.728\t      94.774")
LIST_ITEM1 ("\t##*[atr / lo]#\t      89.544\t      86.442")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      66.123\t      65.010")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      63.553\t      64.622")
NORMAL ("We already see some features of the target grammar, namely the top ranking of *[rtr / hi] "
	"and RTR dominance (the mutual ranking of the P\\s{ARSE} constraints). The steps have not been exactly 0.1, "
	"because we also specified a relative plasticity spreading of 0.1, thus giving steps typically in the range of 0.7 to 1.3.")
NORMAL ("After learning once more with the same data, the result is:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      100.800\t      104.320")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      81.429\t      82.684")
LIST_ITEM1 ("\t##*[atr / lo]#\t      79.966\t      78.764")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      81.316\t      78.166")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      77.991\t      77.875")
NORMAL ("This grammar now sometimes produces faithful disharmonic utterances, because the P\\s{ARSE} now often "
	"outrank the gestural constraints at evaluation time. But there is still a lot of variation produced. "
	"Learning once more with the same data gives:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      100.800\t      100.835")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      86.392\t      82.937")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      81.855\t      81.018")
LIST_ITEM1 ("\t##*[atr / lo]#\t      78.447\t      78.457")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      79.409\t      76.853")
NORMAL ("By inspecting the first column, you can see that the ranking values are already in the same order as in the target grammar, "
	"so that the learner will produce 100 percent correct adult utterances if her evaluation noise is zero. However, "
	"with a noise of 2.0, there will still be variation. For instance, the disharmonies above will "
	"produce [ata] instead of [\\swt\\sw] for underlying /\\swt\\sw/. Learning seven times more "
	"with the same data gives a reasonable proficiency:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      100.800\t      99.167")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      91.580\t      93.388")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      85.487\t      86.925")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      80.369\t      78.290")
LIST_ITEM1 ("\t##*[atr / lo]#\t      75.407\t      74.594")
NORMAL ("No input forms have error rates above 4 percent now, so the child has learned a lot with only 10,000 data, "
	"which may be on the order of the number of input data she receives every day.")
NORMAL ("We could have sped up the learning process appreciably by using a plasticity of 1.0 instead of 0.1. "
	"This would have given a comparable grammar after only 1000 data. After 10,000 data, we would have")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*[rtr / hi]#\t      107.013\t      104.362")
LIST_ITEM1 ("\t##P\\s{ARSE} (rtr)#\t      97.924\t      99.984")
LIST_ITEM1 ("\t##*G\\s{ESTURE} (contour)#\t      89.679\t      89.473")
LIST_ITEM1 ("\t##P\\s{ARSE} (atr)#\t      81.479\t      83.510")
LIST_ITEM1 ("\t##*[atr / lo]#\t      73.067\t      72.633")
NORMAL ("With this grammar, all the error rates are below 0.2 percent. We see that crucially ranked constraints "
	"will become separated after a while by a gap of about 10 along the ranking scale.")
NORMAL ("If we have three constraints obligatorily ranked as A >> B >> C in the adult grammar, with ranking differences of 8 between "
	"A and B and between B and C in the learner's grammar (giving an error rate of 0.2\\% ), the ranking A >> C has a chance of less than 1 in 100 million "
	"to be reversed at evaluation time. This relativity of error rates is an empirical prediction of our stochastic grammar model.")
MAN_END

MAN_BEGIN ("OT learning 5. Learning a stochastic grammar", "ppgb", 20011120)
NORMAL ("Having shown that the algorithm can learn deep obligatory rankings, we will now see "
	"that it also performs well in replicating the variation in the language environment.")
NORMAL ("Create a place assimilation grammar as described in @@OT learning 2.6. Variable output|\\SS2.6@, "
	"and set all its rankings to 100.000:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*G\\s{ESTURE}#\t      100.000\t      100.000")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (t, p)#\t      100.000\t      100.000")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (n, m)#\t      100.000\t      100.000")
NORMAL ("Create a place assimilation distribution and generate 1000 string pairs (@@OT learning 3.1. Data from a pair distribution|\\SS3.1@). "
	"Select the grammar and the two @Strings objects, and learn with a plasticity of 0.1:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (t, p)#\t      104.540\t      103.140")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (n, m)#\t      96.214\t      99.321")
LIST_ITEM1 ("\t##*G\\s{ESTURE}#\t      99.246\t      97.861")
NORMAL ("The output distributions are now (using @@OTGrammar: To output Distributions...@, see @@OT learning 2.9. Output distributions|\\SS2.9@):")
LIST_ITEM1 ("\t/an+pa/ \\-> anpa\t14.3\\% ")
LIST_ITEM1 ("\t/an+pa/ \\-> ampa\t85.7\\% ")
LIST_ITEM1 ("\t/at+ma/ \\-> atma\t96.9\\% ")
LIST_ITEM1 ("\t/at+ma/ \\-> apma\t3.1\\% ")
NORMAL ("After another 10,000 new string pairs, we have:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (t, p)#\t      106.764\t      107.154")
LIST_ITEM1 ("\t##*G\\s{ESTURE}#\t      97.899\t      97.161")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (n, m)#\t      95.337\t      96.848")
NORMAL ("With the following output distributions (measured with a million draws):")
LIST_ITEM1 ("\t/an+pa/ \\-> anpa\t18.31\\% ")
LIST_ITEM1 ("\t/an+pa/ \\-> ampa\t81.69\\% ")
LIST_ITEM1 ("\t/at+ma/ \\-> atma\t99.91\\% ")
LIST_ITEM1 ("\t/at+ma/ \\-> apma\t0.09\\% ")
NORMAL ("The error rate is acceptably low, but the accuracy in reproducing the 80\\%  - 20\\%  "
	"distribution could be better. This is because the relatively high plasticity of 0.1 "
	"can only give a coarse approximation. So we lower the plasticity to 0.001, "
	"and supply 100,000 new data:")
LIST_ITEM1 ("\t\t      %%ranking value\t      %disharmony")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (t, p)#\t      106.810\t      107.184")
LIST_ITEM1 ("\t##*G\\s{ESTURE}#\t      97.782\t      99.682")
LIST_ITEM1 ("\t##*R\\s{EPLACE} (n, m)#\t      95.407\t      98.760")
NORMAL ("With the following output distributions:")
LIST_ITEM1 ("\t/an+pa/ \\-> anpa\t20.08\\% ")
LIST_ITEM1 ("\t/an+pa/ \\-> ampa\t79.92\\% ")
LIST_ITEM1 ("\t/at+ma/ \\-> atma\t99.94\\% ")
LIST_ITEM1 ("\t/at+ma/ \\-> apma\t0.06\\% ")
NORMAL ("So besides learning obligatory rankings like a child does, "
	"the algorithm can also replicate very well the probabilities of the environment. "
	"This means that a GLA learner can learn stochastic grammars.")
MAN_END

MAN_BEGIN ("OT learning 6. Shortcut to OT learning", "ppgb", 20030916)
INTRO ("Once you have mastered the tedious procedures of making Praat learn stochastic grammars, "
	"as described in the previous chapters of this tutorial, you can try a faster procedure, "
	"which simply involves selecting an @OTGrammar object together with a @PairDistribution object, "
	"and clicking ##Learn...#. Once you click OK, Praat will feed the selected grammar with input/output "
	"pairs drawn from the selected distribution, and the grammar will be modified every time its output "
	"is different from the given output. Here is the meaning of the arguments:")
TAG ("%%Evaluation noise% (standard value: 2.0)")
DEFINITION ("the standard deviation of the noise added to the ranking of each constraint at evaluation time.")
TAG ("%%Strategy% (standard value: Symmetric all)")
DEFINITION ("what to do when the learner's output is different from the given output. Possibilities:")
LIST_ITEM1 ("Demotion only: lower the ranking of every constraint that is violated more in the correct output "
	"than in the learner's output. This algorithm crashes if there is variation in the data, i.e. if some inputs "
	"can have more than one possible adult outputs.")
LIST_ITEM1 ("Symmetric one: lower the ranking of the highest-ranked constraint that is violated more in the adult output "
	"than in the learner's output, and raise the ranking of the highest-ranked constraint that is violated more "
	"in the learner's output than in the adult output. This is the \"minimal\" algorithm described and refuted in "
	"@@Boersma (1998)@, chapters 14-15.")
LIST_ITEM1 ("Symmetric all: lower the ranking of all constraints that are violated more in the adult output "
	"than in the learner's output, and raise the ranking of all constraints that are violated more "
	"in the learner's output than in the adult output. This is the algorithm described in @@Boersma & Hayes (2001)@.")
LIST_ITEM1 ("Weighted uncancelled: the same as \"Symmetric all\", but the size of the learning step "
	"is divided by the number of moving constraints. This makes sure that the average ranking of all the constraints "
	"is constant.")
LIST_ITEM1 ("Weighted all: the \"Symmetric all\" strategy can reworded as follows: \"lower the ranking of all constraints "
	"that are violated in the adult output, and raise the ranking of all constraints that are violated in the learner's output\". "
	"Do that, but divide the size of the learning step by the number of moving constraints.")
LIST_ITEM1 ("EDCD: Error-Driven Constraint Demotion, the algorithm described by @@Tesar & Smolensky (1998)@. "
	"All constraints that prefer the adult form and are ranked above the highest-ranked constraint that prefers the learner's form, "
	"are demoted to the ranking of that last constraint minus 1.0.")
TAG ("%%Initial plasticity% (standard value: 1.0)")
TAG ("%%Replications per plasticity% (standard value: 100000)")
TAG ("%%Plasticity decrement% (standard value: 0.1)")
TAG ("%%Number of plasticities% (standard value: 4)")
DEFINITION ("these four arguments determine the %%learning scheme%, i.e. the number of times the grammar will "
	"receive data at a certain plasticity. With the standard values, there will be 100000 data while the plasticity is 1.0 "
	"(the initial plasticity), 100000 data while the plasticity is 0.1, 100000 data while the plasticity is 0.01, "
	"and 100000 data while the plasticity is 0.001. If you want learning at a constant plasticity, set the "
	"%%number of plasticities% to 1.")
TAG ("%%Rel. plasticity spreading% (standard value: 0.1)")
DEFINITION ("if this is not 0, the size of the learning step will vary randomly. For instance, if the plasticity is set to 0.01, "
	"and the relative plasticity spreading is 0.1, you will get actual learning steps that could be anywhere between 0.007 "
	"and 0.013, according to a Gaussian distribution with mean 0.01 and standard deviation 0.001.")
TAG ("%%Honour local rankings% (standard value: on)")
DEFINITION ("if this is on, the fixed rankings that you supplied in the grammar will be maintained during learning: "
	"if a constraint falls below a constraint that is supposed to be universally lower-ranked, this second constraint "
	"will be demoted as well.")
TAG ("%%Number of chews% (standard value: 1)")
DEFINITION ("the number of times that each input-output pair is fed to the grammar. Setting this number to 20 "
	"will give a slightly different (perhaps more accurate) result than simply raising the plasticity by a factor of 20.")
MAN_END

MAN_BEGIN ("OT learning 7. Learning from overt forms", "ppgb", 20031220)
INTRO ("In order to be able to learn phonological production, both EDCD and GLA require pairs of underlying form "
	"and surface form. However, the language-learning child hears neither of these forms: she only hears ##%%overt forms%#, "
	"with less structural information than the underlying and surface forms contain.")
ENTRY ("Interpretive parsing")
NORMAL ("The language-learning child has to construct both the surface form and the underlying form from the overt form "
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
NORMAL ("In Tesar & Smolensky's view, the underlying form can be trivially computed from the surface form, "
	"since the surface form %contains enough information. For instance, the surface form /(\\si \\si\\'^) \\si/ must "
	"lead to the underlying form \\|f\\si \\si \\si\\|f if all parentheses and stress marks are removed. Since "
	"@@McCarthy & Prince (1995)@, this %containment view of surface representations has been abandoned. "
	"In P\\s{RAAT}, therefore, the underlying form is not trivially computed from the surface form, "
	"but all the tableaus are scanned for the surface form that violates the least high-ranked constraints (in the usual "
	"OT sense), as long as it contains the given overt form. For instance, if I\\s{AMBIC} >> A\\s{LL}F\\s{EET}R\\s{IGHT} "
	">> T\\s{ROCHAIC} >> A\\s{LL}F\\s{EET}L\\s{EFT}, the overt form [\\si \\si\\'^ \\si] occurs in two candidates: "
	"the surface form /(\\si \\si\\'^) \\si/ in the tableau for the underlying form \\|f\\si \\si \\si\\|f, and "
	"the surface form /\\si (\\si\\'^ \\si)/ in the tableau for the underlying form \\|f\\si \\si \\si\\|f. The best candidate "
	"is the surface form /(\\si \\si\\'^) \\si/ in the tableau for the underlying form \\|f\\si \\si \\si\\|f. Hence, "
	"P\\s{RAAT}'s version of Robust Interpretive Parsing will map the overt form [\\si \\si\\'^ \\si] to the underlying form "
	"\\|f\\si \\si \\si\\|f (the 'winning tableau') and to the surface form /(\\si \\si\\'^) \\si/ (to be sure, "
	"this is the same result as in Tesar & Smolensky's "
	"version, but crucial differences between the two versions will appear when faithfulness constraints are involved).")
NORMAL ("In P\\s{RAAT}, you can do interpretive parsing. For example, create a grammar with ##Create metrics grammar...# "
	"from the @@New menu@. Then choose ##Get interpretive parse...# from the #Query submenu and supply \"[L1 L L]\" for the "
	"overt form, which means a sequence of three light syllables with a main stress on the first. The Info window will show you "
	"the optimal underlying and surface forms, given the current constraint ranking.")
ENTRY ("Learning from partial forms")
NORMAL ("Now that the learning child can convert an overt form to an underlying-surface pair, she can compare this "
	"surface form to the surface form that she herself would have derived from this underlying form. For instance, "
	"If I\\s{AMBIC} >> A\\s{LL}F\\s{EET}R\\s{IGHT} >> T\\s{ROCHAIC} >> A\\s{LL}F\\s{EET}L\\s{EFT}, the winning "
	"tableau is \\|f\\si \\si \\si\\|f, and the perceived adult surface form is /(\\si \\si\\'^) \\si/. "
	"But from the underlying form \\|f\\si \\si \\si\\|f, the learner will derive /\\si (\\si \\si\\'^)/ as her own surface form. "
	"The two surface forms are different, so that the learner can take action by reranking one or more constraints, "
	"perhaps with EDCD or GLA.")
NORMAL ("In P\\s{RAAT}, you can learn from partial forms. Select the metrics grammar and choose ##Learn from one partial output...#, "
	"and supply \"[L1 L L]\". If you do this several times, you will see that the winner for the tableau \"|L L L|\" will become one of "
	"the two forms with overt part \"[L1 L L]\".")
NORMAL ("To run a whole simulation, you supply a @Distributions object with one column, perhaps from a text file. The following "
	"text file shows the overt forms for Latin, with the bisyllabic forms occurring more often than the trisyllabic forms:")
CODE ("\"ooTextFile\"")
CODE ("\"Distributions\"")
CODE ("1 column with numeric data")
CODE ("   \"Latin\"")
CODE ("28 rows")
CODE ("\"[L1 L]\" 25")
CODE ("\"[L1 H]\" 25")
CODE ("\"[H1 L]\" 25")
CODE ("\"[H1 H]\" 25")
CODE ("\"[L1 L L]\" 5")
CODE ("\"[H1 L L]\" 5")
CODE ("\"[L H1 L]\" 5")
CODE ("\"[H H1 L]\" 5")
CODE ("\"[L1 L H]\" 5")
CODE ("\"[H1 L H]\" 5")
CODE ("\"[L H1 H]\" 5")
CODE ("\"[H H1 H]\" 5")
CODE ("\"[L L1 L L]\" 1")
CODE ("\"[L H1 L L]\" 1")
CODE ("\"[L L H1 L]\" 1")
CODE ("\"[L H H1 L]\" 1")
CODE ("\"[L L1 L H]\" 1")
CODE ("\"[L H1 L H]\" 1")
CODE ("\"[L L H1 H]\" 1")
CODE ("\"[L H H1 H]\" 1")
CODE ("\"[H L1 L L]\" 1")
CODE ("\"[H H1 L L]\" 1")
CODE ("\"[H L H1 L]\" 1")
CODE ("\"[H H H1 L]\" 1")
CODE ("\"[H L1 L H]\" 1")
CODE ("\"[H H1 L H]\" 1")
CODE ("\"[H L H1 H]\" 1")
CODE ("\"[H H H1 H]\" 1")
NORMAL ("Read this file into P\\s{RAAT} with @@Read from file...@. A @Distributions object then appears in the object list. "
	"Click @@Distributions: To Strings...|To Strings...@, then OK. A @Strings object containing 1000 strings, drawn randomly "
	"from the distribution, with relative frequencies as in the text file, will appear in the list. Click @Inspect to check the contents.")
NORMAL ("You can now select the @OTGrammar together with the @Strings and choose ##Learn from partial outputs...#. "
	"A thousand times, P\\s{RAAT} will construct a surface form from the overt form by interpretive parsing, "
	"and also construct the underlying form in the same way, from which it will construct another surface form by evaluating the "
	"tableau. Whenever the two surface forms are not identical, some constraints will be reranked. In the current implementation, "
	"the disharmonies for interpretive parsing and for production are the same, i.e., "
	"if the evaluation noise is not zero, the disharmonies are randomly renewed before each interpretive parsing "
	"but stay the same for the subsequent virtual production.")
MAN_END

/*
MAN_BEGIN ("OTGrammar examples", "ppgb", 20001027)
ENTRY ("Safety margin and stochastic evaluation")
NORMAL ("What is a %small demotion step? This must be taken relative to another quantity. "
	"This quantity is the evaluation noise.")
NORMAL ("If the %%ranking spreading% were zero, the demotions in the GLA would immediately stop once that "
	"a constraint has fallen below its competitors. If the data contain an error, the grammar "
	"will change to an incorrect state, and the learner has to make an error to correct it. "
	"Though the error ratio is now one (because of the constant demotion step), "
	"the learner's behaviour can still be described as a `tit-for-tat' strategy, "
	"which is an unknown phenomenon in human speech variation.")
NORMAL ("The solution is to have a finite ranking spreading: in this way, the learner will continue making "
	"errors (though less than 50\\% ) after the constraint has fallen below its competitors. "
	"For instance, with a ranking spreading of 2.0, the distance between the constraints will be about "
	"10 after a few thousand relevant data: the %%safety margin%. "
	"If the mean demotion step is 0.1, there is no chance "
	"that a modest amount of erroneous data will reverse the ranking. It is true, however, that "
	"one erroneous datum will decrease the constraint distance by 0.1, so that the learner will have to "
	"make one mistake herself to restore the original distance. But this is only natural, "
	"as she may well wait a long time before doing this: "
	"on the average, the same number of thousands of data. This is the %%patient error-matching learner%.")
ENTRY ("Learning from surface data alone")
NORMAL ("Many a young learner will take the adult surface forms, as perceived by herself, "
	"as her underlying forms. In other words, the input to her grammar will equal "
	"the output of the adult grammar.")
NORMAL ("We can simulate what happens here by taking the adult output as the input to the learning algorithms:")
LIST_ITEM ("1. Create a five-constraint Wolof grammar.")
LIST_ITEM ("2. Generate 1000 input strings.")
LIST_ITEM ("3. Compute the 1000 output strings.")
LIST_ITEM ("4. Create a five-constraint \"equal\" grammar.")
LIST_ITEM ("5. Select this monostratal grammar and the output strings only, "
	"and click ##Learn output (T&S)#. See @@OTAnyGrammar & Strings: Learn output (T&S)@.")
NORMAL ("The result will be a grammar where the faithfulness constraints outrank all the "
	"gestural constraints that can be violated in any output string:")
LIST_ITEM ("##*[rtr / hi]#  100.000  100.000")
LIST_ITEM ("##P\\s{ARSE} (atr)#  100.000  100.000")
LIST_ITEM ("##P\\s{ARSE} (rtr)#  100.000  100.000")
LIST_ITEM ("##*G\\s{ESTURE} (contour)#  99.000  99.000")
LIST_ITEM ("##*[atr / lo]#  99.000  99.000")
NORMAL ("You will get a comparable result with @@OTAnyGrammar & Strings: Learn output (GLA)...@.")
NORMAL ("The resulting grammar represents the learner's state after the acquisition of "
	"all the relevant gestures. The learner will now faithfully reproduce /et\\ep/ "
	"if that were in her lexicon. Before being able to render such an underlying form as [\\ept\\ep], "
	"she must learn that faithfulness can be violated.")
ENTRY ("Example 2: When underlying forms are irrelevant")
NORMAL ("Underlying forms are relevant only if faithfulness constraints are involved. "
	"If a grammar only contains constraints that evaluate the output, "
	"we need no input strings for our simulations. However, "
	"if the relevant constraint had fixed rankings, there would only be a single possible output, "
	"which seems uninteresting. An interesting output-only grammar, therefore, necessarily "
	"features stochastic evaluation, and at least some of the constraints wil have rankings that "
	"are close to each other.")
NORMAL ("#%Example. @@Hayes & MacEachern (1998)@ identify 11 output-oriented constraints "
	"for the forms of quatrains in English folk verse.")
LIST_ITEM ("1. Create a folk-verse grammar with equal constraint rankings (all 100). "
	"You may find it in the file ##folkVerse.OTGrammar# in the #demo directory of your #Praat distribution, "
	"or get it from ##http://www.fon.hum.uva.nl/praat/folkVerse.OTGrammar#.")
LIST_ITEM ("2. Generate 1000 input strings. They will all be empty strings.")
LIST_ITEM ("3. Read the file that contains the surface distribution of the possible outputs. "
	"It is in the #demo folder or at ##http://www.fon.hum.uva.nl/praat/folkVerse.Distributions#. "
	"A @Distributions object will appear in the list. "
	"Column \"Actual\" is the last column of table (10) in Hayes & MacEachern (1998).")
LIST_ITEM ("4. From this surface distribution, create a list of 1000 output strings, "
	"using @@Distributions: To Strings...@ (set %column to 1).")
LIST_ITEM ("5. Select the grammar, the \"input strings\", and the output strings, "
	"and learn in the usual way. After learning, you can see that some constraints have risen above 100, "
	"and some have fallen below 100.")
NORMAL ("With each of the 1000 outputs, the learner can be regarded as having generated a quatrain herself "
	"and compared it to a quatrain in her folk-verse environment. If these quatrains are equal "
	"(a 10\\%  chance or so), nothing happens. Otherwise, the learner will demote the highest "
	"violated constraint (i.e., the one that is most disharmonic during her stochastic evaluation) "
	"in the heard quatrain that is not (or less) violated in the winner (the quatrain that she generated herself). "
	"She will also promote the highest violated constraint in the winner "
	"that is not (or less) violated in the heard quatrain.")
NORMAL ("We are next going to generate a set of 589 quatrains, in order to be able to compare "
	"the behaviours of our folk-verse grammar and the English folk-verse writers:")
LIST_ITEM ("1. Select the learned grammar and generate 589 (empty) input strings.")
LIST_ITEM ("2. Select the learned grammar and the so-called input strings, "
	"and generate the output strings.")
LIST_ITEM ("3. To see the distribution of the output strings, choose @@Strings: To Distributions@, "
	"and draw the resulting @Distributions object to your Picture window.")
LIST_ITEM ("4. You can now compare the two distributions.")
NORMAL ("Instead of generating the data from a @Distributions, you could have generated them from "
	"the target grammar in table (9) of Hayes & MacEachern (1998). "
	"Such a grammar is in your #demo folder (##folkVerse59.OTGrammar#) "
	"or at ##http://www.fon.hum.uva.nl/praat/folkVerse59.OTGrammar#. "
	"Because of the loosening of the tie between two of the constraints (see H & McE, fn. 43), "
	"this grammar will give different distributions from the \"actual\" values, "
	"but our algorithm will learn them correctly, provided you choose ##Symmetric all# "
	"or ##weighted uncancelled# for the learning strategy.")
MAN_END
*/

MAN_BEGIN ("OTGrammar", "ppgb", 20041110)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}. See the @@OT learning@ tutorial.")
ENTRY ("Inside an OTGrammar")
NORMAL ("With @Inspect, you will see the following attributes:")
TAG ("%constraints")
DEFINITION ("a list of constraints. Each constraint contains the following attributes:")
TAG1 ("%name")
DEFINITION1 ("the fixed name of the constraint, for instance \"P\\s{ARSE}\".")
TAG1 ("%ranking")
DEFINITION1 ("the continuous ranking value; will change during learning.")
TAG1 ("%disharmony")
DEFINITION1 ("the effective ranking value during stochastic evaluation; with a non-zero evaluation noise, "
	"this will be different from %ranking.")
TAG ("%fixedRankings")
DEFINITION ("an often empty list of locally ranked pairs of constraints. "
	"Each local-ranking pair contains the following attributes:")
TAG1 ("%higher")
DEFINITION1 ("the index of the universally higher-ranked of the two constraints, "
	"a number between 1 and the number of constraints.")
TAG1 ("%lower")
DEFINITION1 ("the index of the universally lower-ranked of the two constraints.")
TAG ("%tableaus")
DEFINITION ("a list of tableaus. Each tableau contains the following attributes:")
TAG1 ("%input")
DEFINITION1 ("the input string of the tableau. For production grammaras, the underlying form of the utterance, for example "
	"\\|fan+pa\\|f or \\|fb\\rt\\ic\\ng + \\s{PAST}\\|f.")
TAG1 ("%candidates")
DEFINITION1 ("a list of output candidates. Each output candidate consists of:")
TAG2 ("%output")
DEFINITION2 ("the output string of the tableau. In generative phonology: the surface form of the utterance, for example "
	"[anpa] or [ampa] or [b\\rt\\ct:t] or [b\\rt\\ae\\ng]. In functional phonology: the combination of the articulatory "
	"and the perceptual results, for example [anpa]-/anpa/ or [ampa]-/ampa/ or [b\\rt\\ct:t]-/b\\rt\\ct:t/ or "
	"[b\\rt\\ae\\ng]-/b\\rt\\ae\\ng/.")
TAG2 ("%marks")
DEFINITION2 ("a list of the number of violations of each constraint for this output form. If there are 13 constraints, "
	"this list will contain 13 integer numbers for each candidate.")
ENTRY ("OTGrammar creation")
NORMAL ("You can easily create some #OTGrammar examples from the @@New menu@, "
	"or type your own grammars into a text file and read the file into Praat. See the @@OT learning@ tutorial.")
ENTRY ("OTGrammar actions")
NORMAL ("You can perform the following actions on selected #OTGrammar objects:")
LIST_ITEM ("@@OTGrammar: Generate inputs...@")
/*LIST_ITEM ("@@OTGrammar: Sort...@")*/
LIST_ITEM ("@@OTGrammar: Input to output...@")
LIST_ITEM ("@@OTGrammar: Input to outputs...@ (compute the output distribution for a given input)")
LIST_ITEM ("@@OTGrammar: To output Distributions...@")
LIST_ITEM ("@@OTGrammar & Strings: Inputs to outputs...@")
LIST_ITEM ("@@OTGrammar: Learn one...@")
LIST_ITEM ("@@OTGrammar & 2 Strings: Learn...@")
NORMAL ("You can view an #OTGrammar in an @OTGrammarEditor.")
MAN_END

MAN_BEGIN ("OTGrammar: Generate inputs...", "ppgb", 19981230)
INTRO ("A command to create a @Strings object from a selected @OTGrammar.")
NORMAL ("A practical grammar-specific implementation of the %%richness of the base%: "
	"the inputs are drawn at random with equal probabilities from the inputs associated with the tableaus. "
	"For an example, see @@OT learning 3.2. Data from another grammar@.")
ENTRY ("Argument")
TAG ("%%Number of trials")
DEFINITION ("the number of times a string will be drawn from the possible inputs to the grammar.")
MAN_END

MAN_BEGIN ("OTGrammar: Input to output...", "ppgb", 20030916)
INTRO ("A command to ask the selected @OTGrammar object to evaluate the candidates associated with a specified input form.")
NORMAL ("See @@OT learning 2.8. Asking for one output@ for tutorial information.")
ENTRY ("Arguments")
TAG ("%%Input form")
DEFINITION ("the input form whose surface form you want to know. If this string is not in the list "
	"of the possible inputs of the selected OTGrammar, you will get an error message.")
TAG ("%Noise (standard value: 2.0)")
DEFINITION ("the standard deviation of the noise added to the ranking value of every constraint during evaluation. "
	"See @@OT learning 2.4. Evaluation@.")
MAN_END

MAN_BEGIN ("OTGrammar: Input to outputs...", "ppgb", 20030916)
INTRO ("A command to ask the selected @OTGrammar object to evaluate a number of times the candidates associated "
	"with a specified input form. The result is a @Distributions object.")
NORMAL ("See @@OT learning 2.9. Output distributions@ for tutorial information and examples.")
ENTRY ("Arguments")
TAG ("%Trials (standard value: 1000)")
DEFINITION ("the number of evaluations that you want to perform.")
TAG ("%Noise (standard value: 2.0)")
DEFINITION ("the standard deviation of the noise added to the ranking value of every constraint during the evaluations. "
	"See @@OT learning 2.4. Evaluation@.")
TAG ("%%Input form")
DEFINITION ("the input form whose surface forms you want to measure. If this string is not in the list "
	"of the possible inputs of the selected OTGrammar, you will get an error message.")
MAN_END

MAN_BEGIN ("OTGrammar: Learn one...", "ppgb", 20011120)
INTRO ("Causes every selected @OTGrammar object to process one input/output pair "
	"according to the Gradual Learning Algorithm "
	"by @@Boersma (1998)@ and @@Boersma (2000)@. See @@OT learning 4. Learning an ordinal grammar@ "
	"and @@OT learning 5. Learning a stochastic grammar@.")
MAN_END

MAN_BEGIN ("OTGrammar: To output Distributions...", "ppgb", 19981230)
INTRO ("A command to ask the selected @OTGrammar object to evaluate a number of times the candidates associated "
	"with every input form. The result is a @Distributions object. See @@OT learning 2.9. Output distributions@.")
MAN_END

MAN_BEGIN ("OTGrammar & Strings: Inputs to outputs...", "ppgb", 19981230)
INTRO ("An action that creates a @Strings object from a selected @OTGrammar and a selected @Strings.")
NORMAL ("The selected Strings object is considered as a list of inputs to the OTGrammar grammar.")
ENTRY ("Arguments")
TAG ("%%Noise")
DEFINITION ("the standard deviation of the noise that will be temporarily added to the ranking value at each evaluation.")
NORMAL ("The resulting Strings object will contain the output string of the grammar for each of the input strings.")
NORMAL ("See @@OT learning 3.2. Data from another grammar@.")
MAN_END

MAN_BEGIN ("OTGrammar & 2 Strings: Learn...", "ppgb", 20011120)
INTRO ("Causes the selected @OTGrammar object to process a number of input/output pairs "
	"according to the Gradual Learning Algorithm "
	"by @@Boersma (1998)@ and @@Boersma (2000)@. See @@OT learning 4. Learning an ordinal grammar@ "
	"and @@OT learning 5. Learning a stochastic grammar@.")
MAN_END

MAN_BEGIN ("OTGrammarEditor", "ppgb", 20030316)
INTRO ("One of the @editors in P\\s{RAAT}, for viewing and editing the grammar in an @OTGrammar object.")
NORMAL ("See the @@OT learning@ tutorial for examples.")
ENTRY ("Usage")
NORMAL ("The menu command that you will probably use most often if you investigate variation, "
	"is the ##Evaluate (noise 2.0)# command, which you can invoke from the Edit menu or by pressing Command-2.")
NORMAL ("This command performs a new evaluation with the current ranking values. Some noise is added to the "
	"ranking values, so that the %#disharmonies of the constraint will change. This may cause a change in the "
	"ranking order of the constraints, which in its turn may cause a different candidate to win in some tableaus.")
MAN_END

MAN_BEGIN ("Robust Interpretive Parsing", "ppgb", 20021105)
INTRO ("The mapping from overt forms to surface forms in the acquisition model by @@Tesar & Smolensky (1998)@.")
NORMAL ("In P\\s{RAAT}, you can do robust interpretive parsing on any @OTGrammar object. "
	"See @@OT learning 7. Learning from overt forms@.")
MAN_END

}
