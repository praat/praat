/* manual_Exp.c
 *
 * Copyright (C) 2001-2004 Paul Boersma
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

void manual_Exp_init (ManPages me);
void manual_Exp_init (ManPages me) {

MAN_BEGIN ("ExperimentMFC", "ppgb", 20040928)
INTRO ("One of the @@types of objects@ in P\\s{RAAT}, "
	"for running a Multiple Forced Choice listening experiment.")
NORMAL ("With Praat, you can do simple experiments on identification and discrimination. "
	"`Simple' means that for identification, the subject hears a sound and has to click on one of a set of "
	"labelled rectangles (optionally, you can have the subject give a goodness-of-fit judgment). "
	"For discrimination, you can have simple same-different choices, or more intricate things like AXB, 4I-oddity, and so on.")
NORMAL ("The advantage of using Praat for this is that it is free, it works on Windows, Unix, and Macintosh, "
	"and the whole experiment (experiment file plus sound files) is portable across computers "
	"(you can run it from a CD, for instance). Because of the limited possibilities, "
	"it is also quite easy to set up the experiment. Just read the description below.")
NORMAL ("If you require more from your experiment design, you use a dedicated program like Presentation or E-prime instead of Praat. "
	"With these programs, you can measure reaction times as well, or you can let the stimulus depend on the subject's previous responses. "
	"If you do not need those extra capabilities, you can use the simpler method of Praat. "
	"My estimation is that that is fine for 90 percent of the perception experiments that phoneticians perform.")
ENTRY ("Example")
NORMAL ("An experiment is defined in a simple text file, which we call an %%experiment file%. "
	"The following is an example of such an experiment file. The first two lines have to be typed "
	"exactly as in this example, the rest depends on your stimuli, on your response categories, "
	"and on the way the experiment is to be presented to the listener. "
	"The order of the elements in this file cannot be changed.")
CODE ("\"ooTextFile\"")
CODE ("\"ExperimentMFC 3\"")
CODE ("fileNameHead = \"Sounds/\"")
CODE ("fileNameTail = \".wav\"")
CODE ("carrierBefore = \"weSayTheWord\"")
CODE ("carrierAfter = \"again\"")
CODE ("initialSilenceDuration = 0.5 seconds")
CODE ("interStimulusInterval = 0")
CODE ("numberOfDifferentStimuli = 4")
CODE1 ("\"heed\"")
CODE1 ("\"hid\"")
CODE1 ("\"hood\"")
CODE1 ("\"hud\"")
CODE ("numberOfReplicationsPerStimulus = 3")
CODE ("breakAfterEvery = 0")
CODE ("randomize = <PermuteBalancedNoDoublets>")
CODE ("startText = \"Click to start.\"")
CODE ("runText = \"Choose the vowel that you heard.\"")
CODE ("pauseText = \"You can have a short break if you like. Click to proceed.\"")
CODE ("endText = \"The experiment has finished.\"")
CODE ("numberOfResponseCategories = 5")
CODE1 ("0.2 0.3 0.7 0.8 \"h I d\" \"\" \"i\"")
CODE1 ("0.3 0.4 0.5 0.6 \"h E d\" \"\" \"e\"")
CODE1 ("0.4 0.5 0.3 0.4 \"h A d\" \"\" \"a\"")
CODE1 ("0.5 0.6 0.5 0.6 \"h O d\" \"\" \"o\"")
CODE1 ("0.6 0.7 0.7 0.8 \"h U d\" \"\" \"u\"")
CODE ("numberOfGoodnessCategories = 5")
CODE1 ("0.25 0.35 0.10 0.20 \"1 (poor)\"")
CODE1 ("0.35 0.45 0.10 0.20 \"2\"")
CODE1 ("0.45 0.55 0.10 0.20 \"3\"")
CODE1 ("0.55 0.65 0.10 0.20 \"4\"")
CODE1 ("0.65 0.75 0.10 0.20 \"5 (good)\"")
NORMAL ("This experiment will play 4 different stimuli to the listener, each 3 times. "
	"Thus, the listener is confronted with 12 trials.")
ENTRY ("1. The stimuli")
NORMAL ("The names of the sound files containing the stimuli must be identical to the names "
	"of the stimuli, bracketed with %fileNamehead and %fileNameTail. "
	"You can see that the stimulus names are %heed, %hid, %hood, and %hud. "
	"Hence, the 4 stimuli are expected in the following 4 files:")
LIST_ITEM ("Sounds/heed.wav")
LIST_ITEM ("Sounds/hid.wav")
LIST_ITEM ("Sounds/hood.wav")
LIST_ITEM ("Sounds/hud.wav")
NORMAL ("You can also use AIFF files, in which case %fileNameTail would probably be \".aiff\", or any other "
	"type of sound file that Praat supports. They all have to have the same sampling frequency.")
NORMAL ("In this example, the experiment will look for the sound files in the directory #Sounds, "
	"which has to be in the same directory as your experiment file. "
	"In other words, \"Sounds/heed.wav\" is a %%relative file path%.")
NORMAL ("Instead of a relative path, you can also supply a %%full file path%. "
	"Such a path depends on your computer and on your operating system. "
	"For instance, if you have a Windows computer and the stimuli are in the directory ##D:\\bsCorpus\\bsAutumn\\bsSpeaker23#, "
	"you can write")
CODE ("fileNameHead = \"D:\\bsCorpus\\bsAutumn\\bsSpeaker23\\bs\"")
NORMAL ("If you have a Macintosh (OS X) or Unix computer and the stimuli are in ##/Users/mietta/Sounds/Dutch#, you write")
CODE ("fileNameHead = \"/Users/mietta/Sounds/Dutch/\"")
NORMAL ("If you have an older Macintosh and the stimuli are in ##My disk:My experiment:Stimuli#, you write")
CODE ("fileNameHead = \"My disk:My experiment:Stimuli:\"")
NORMAL ("But relative file paths will usually be preferred: they are more %portable. "
	"The advantage of using relative file paths is that you can move your whole experiment (experiment file plus sounds) "
	"from one computer to another without changing the experiment file, "
	"as long as you put the experiment file in the same directory as where you put the directory #Sounds. "
	"Or you can put the whole experiment on a CD and run the experiment directly from the CD. "
	"Since Praat supports the forward slash \"/\" as a directory separator on all computers, "
	"you can run the exact same experiment on Macintosh, Windows and Unix computers, "
	"independent of the type of computer where you have created your experiment.")
ENTRY ("2. The carrier phrase")
NORMAL ("The stimuli can be embedded in a carrier phrase. In the example, we see that the stimulus "
	"will be inserted between the sounds in the files ##weSayTheWord.wav# "
	"and ##again.wav#, both of which are expected to be in the directory #Sounds. "
	"If you do not want a carrier phrase, you do:")
CODE ("carrierBefore = \"\"")
CODE ("carrierAfter = \"\"")
NORMAL ("If you want only an introductory phrase before the stimulus, and nothing after the stimulus, "
	"you do something like")
CODE ("carrierBefore = \"listenTo\"")
CODE ("carrierAfter = \"\"")
NORMAL ("and of course you supply the file ##listenTo.wav# in the directory #Sounds.")
NORMAL ("If you want to have a short silence before every stimulus (and before the carrier phrase), you supply a non-zero %initialSilenceDuration, "
	"as in the example.")
NORMAL ("Since the carrier phrase is concatenated with the stimulus before it is played, it should have the same "
	"sampling frequency as the stimulus files.")
ENTRY ("3. Breaks")
NORMAL ("A new stimulus will arrive as soon as the listener makes her choice. To allow her some breathing "
	"time, you can insert a break after every so many trials. In the example, %breakAfterEvery is 0, "
	"because there are only 12 trials. A typical experiment has perhaps 180 trials, and you may want to "
	"insert a break after every 40 trials. In that case, set %breakAfterEvery to 40.")
ENTRY ("4. Randomization strategies")
NORMAL ("The 3 times 4 stimuli will have to be played in a certain order. For the least random order, you say")
CODE ("randomize = <CyclicNonRandom>")
NORMAL ("In this case, the stimuli will be played in the order in which they were specified in the file, 3 times:")
FORMULA ("heed hid hood hud heed hid hood hud heed hid hood hud")
NORMAL ("The most likely case in which you would want to use this randomization strategy, is if you have, say, 120 "
	"different stimuli and you want to play them only once (%numberOfReplicationsPerStimulus = 1) in a fixed order.")
NORMAL ("The other extreme, the most random order, is")
CODE ("randomize = <WithReplacement>")
NORMAL ("In this case, a stimulus will be chosen at random 12 times without memory, for instance")
FORMULA ("hid hood hood heed hid hood hud hud hid hood heed hid")
NORMAL ("The order will probably be different for each listener. "
	"In this example, %hood and %hid occur four times each, %heed and %hud only twice each. "
	"This strategy is too random for most experiments. Usually, you will want to have the same number "
	"of replications of each stimulus. The most random way to do this is")
CODE ("randomize = <PermuteAll>")
NORMAL ("In this case, all stimuli will be played exactly 3 times, for instance")
FORMULA ("heed hood hud hud hid heed heed hud hood hid hid hood")
NORMAL ("Quite often, you will want a less random order, namely one in which the 12 trials are divided into "
	"3 blocks of 4 stimuli. Within each block, all 4 different stimuli occur in a random order:")
CODE ("randomize = <PermuteBalanced>")
NORMAL ("In this case, each stimulus occurs exactly once within each block:")
FORMULA ("heed hood hud hid hood hud hid heed heed hud hood hid")
NORMAL ("This strategy ensures a certain spreading of the stimuli over the sequence of 12 trials. "
	"As we see here, it is still possible that the same stimulus (%heed) occurs twice in a row, "
	"namely as the last stimulus of the second block and the first stimulus of the third. "
	"If you want to prevent that situation, you use")
CODE ("randomize = <PermuteBalancedNoDoublets>")
NORMAL ("This will ensure that the same stimulus is never applied twice in a row:")
FORMULA ("heed hood hud hid hood hud hid heed hud heed hood hid")
NORMAL ("This randomization strategy is used in our example, and advised for most listening experiments "
	"in which you want to minimize effects of stimulus order.")
NORMAL ("The randomization procedure does not interfere in any way with the breaks. The order is determined "
	"before any breaks are inserted.")
ENTRY ("5. Instructions")
NORMAL ("Before the experiment begins, the listener will see the %startText in the centre of the screen. "
	"During each trial, she will see the %runText at the top of the screen. "
	"During breaks, she will see the %pauseText in the centre of the screen. "
	"After all the trials have been performed, she will see the %endText.")
ENTRY ("6. Response categories")
NORMAL ("Every trial comes with the same set of response categories. The example has five of them. "
	"For each response category, you supply the area of the screen where a rectangle will be drawn. "
	"The whole screen measures from 0.0 (left) to 1.0 (right) and from 0.0 (bottom) to 1.0 (top). "
	"Thus, \"0.2 0.3 0.7 0.8\" means that a rectangle will be drawn somewhere in the top left quadrant "
	"of the screen. You also supply the text that will be drawn in this rectangle, for instance the text \"h I d\".")
NORMAL ("The second text that you supply for every response is a response key on the keyboard. "
	"In the above example this is \"\", i.e. the subject cannot press a key as a response. "
	"If you want the user to be able to press the \"m\" key instead of clicking in the \"h I d\" rectangle, "
	"the line in the experiment file would be:")
CODE1 ("0.2 0.3 0.7 0.8 \"h I d\" \"m\" \"i\"")
NORMAL ("The third text that you supply for each rectangle is the response category as it will be reported by Praat to you when the user clicks it, "
	"e.g. the text \"i\". If you want Praat to ignore mouse clicks on this rectangle, specify an empty response "
	"category, i.e. \"\".")
NORMAL ("The border of the rectangles will be maroon, the background of the screen will be light grey. "
	"The colour of clickable rectangles will be yellow, that of non-clickable rectangles (those with "
	"empty category specifications) light grey.")
ENTRY ("7. Goodness judgments")
NORMAL ("If %numberOfGoodnessCategories is not 0, some more rectangles will be drawn, as in the example. "
	"You specify again the locations of these rectangles (in the example, they touch each other), "
	"and the texts on them. Praat will record the number of the button when the listener clicks on it. "
	"Thus, if she clicks on the button labelled \"1 (poor)\", Praat will record a goodness judgment of 1, "
	"because this is the first button in the list. If she clicks on \"5 (good)\", Praat will record a "
	"goodness judgment of 5.")
ENTRY ("8. How an experiment proceeds")
NORMAL ("A text file with an ExperimentMFC object can be read into Praat with @@Read from file...@ "
	"(it is not a script but a data file, so do not try to read it with ##Open Praat script...#). "
	"You can then choose #Run. After the experiment finishes, you can close the experiment window "
	"and choose ##Extract results#. The resulting ResultsMFC object contains for each trial the stimulus "
	"name (e.g. \"hood\"), the response category (e.g. \"u\"), and the goodness judgment (e.g. 4). "
	"You will want to save this ResultsMFC object to a text file with @@Write to text file...@. "
	"You may want to call these text files by the names of the subjects, e.g. ##ts.ResultsMFC# "
	"and ##mj.ResultsMFC#. Once you have collected the results of all your subjects, you can read "
	"all the results files into Praat with @@Read from file...@, then select all the resulting "
	"ResultsMFC objects (which will have automatically been named #ts, #mj, and so on), then choose "
	"##Collect to table#. This will result in a table whose first column contains the names of the subjects, "
	"while the second column contains the stimulus names and the third column the responses "
	"(if there are goodness judgments, these will go into a fourth column). The table can be saved "
	"as a table file (with ##Write to table file...#), which can be read by programs like Excel and SPSS.")
ENTRY ("9. A simple discrimination experiment")
NORMAL ("The example above was an %identification experiment: the subject had identify a single sound as one element "
	"of a set of categories. Phoneticians will often do %discrimination experiments, which are experiments in which "
	"a stimulus consists of multiple sub-stimuli played in sequence, and the subject has to judge the similarity "
	"between these sub-stimuli.")
NORMAL ("The simplest discrimination task has only two sub-stimuli, and the subject has to say whether these are "
	"the %same or %different. Suppose you have vowel-like sounds along an F1 continuum with seven steps, "
	"say 300, 320, 340, 360, 380, 400, and 420 Hertz, and you are interested in knowing how well the listeners "
	"can distinguish these. As your stimuli, you create pairs of these sounds, separated by 0.8 seconds of silence. "
	"It is important to include stimuli in which the sounds are identical, e.g. stimuli in which both sounds have an F1 "
	"of 340 Hz (see the literature on signal detection theory). Since sounds that are very different acoustically "
	"will always be heard as different, you do not include pairs in which the distance is larger than 60 Hz. "
	"The experiment file will look like this:")
CODE ("\"ooTextFile\"")
CODE ("\"ExperimentMFC 3\"")
CODE ("\"stimuli/\"  \".wav\"")
CODE ("carrier phrase \"\"  \"\"")
CODE ("initial silence 0.5 seconds")
CODE ("inter-stimulus interval 0.8 seconds")
CODE ("37 different stimuli")
CODE1 ("\"300,300\"  \"300,320\"  \"300,340\"  \"300,360\"")
CODE1 ("\"320,300\"  \"320,320\"  \"320,340\"  \"320,360\"  \"320,380\"")
CODE1 ("\"340,300\"  \"340,320\"  \"340,340\"  \"340,360\"  \"340,380\"  \"340,400\"")
CODE1 ("\"360,300\"  \"360,320\"  \"360,340\"  \"360,360\"  \"360,380\"  \"360,400\"  \"360,420\"")
CODE1 ("\"380,320\"  \"380,340\"  \"380,360\"  \"380,380\"  \"380,400\"  \"380,420\"")
CODE1 ("\"400,340\"  \"400,360\"  \"400,380\"  \"400,400\"  \"400,420\"")
CODE1 ("\"420,360\"  \"420,380\"  \"420,400\"  \"420,420\"")
CODE ("10 replications per stimulus")
CODE ("break after every 50 stimuli")
CODE ("<PermuteBalancedNoDoublets>")
CODE ("\"Click to start.\"")
CODE ("\"Say whether these sounds were the same or different.\"")
CODE ("\"You can have a short break if you like. Click to proceed.\"")
CODE ("\"The experiment has finished. Call the experimenter.\"")
CODE ("2 response categories")
CODE1 ("0.1 0.4 0.35 0.65 \"same\" \"\" \"same\"")
CODE1 ("0.6 0.9 0.35 0.65 \"different\" \"\" \"different\"")
CODE ("0 goodness categories")
NORMAL ("In this example, the subject will have to click 370 times. After every 50 times, she will have the "
	"opportunity to sip her tea. A 0.5-seconds silence is played before every stimulus, so that the listener "
	"will not hear the stimulus immediately after her mouse click.")
NORMAL ("The experimenter does not have to create the stimulus pairs as sound files. "
	"You can specify multiple sound files by separating them with commas. Thus, \"320,300\" means that "
	"Praat will play the files ##320.wav# and ##300.wav#. These two substimili will be separated here by a silence "
	"of 0.8 seconds, called the %%inter-stimulus interval%.")
NORMAL ("Note that the text in this file is rather different from the previous example. "
	"It does not matter whether you write \"numberOfDifferentStimuli\", or \"different stimuli\", or anything else; "
	"Praat ignores these texts as long as they do not contain numbers, quoted strings, or things between <>.")
ENTRY ("10. An AXB discrimination experiment")
NORMAL ("In the AXB task, the subject will hear three stimuli in sequence, and has to say "
	"whether the second (X) is more similar to the first (A) or to the second (B). "
	"An experiment file could look like follows:")
CODE ("\"ooTextFile\"")
CODE ("\"ExperimentMFC 3\"")
CODE ("\"stimuli/\"  \".wav\"")
CODE ("carrier \"\"  \"\"")
CODE ("initial silence 0.5")
CODE ("inter-stimulus interval 0.3")
CODE ("100 stimuli")
CODE1 ("\"300,300,320\"  \"300,320,340\"  \"300,340,340\"  \"300,340,360\"")
CODE1 ("...")
CODE1 ("(and 96 more triplets of substimuli)")
CODE1 ("...")
CODE ("4 replications")
CODE ("break every 50")
CODE ("<PermuteBalancedNoDoublets>")
CODE ("\"Click to start.\"")
CODE ("\"Say whether the second sound is more similar to the first or to the third.\"")
CODE ("\"You can have a short break if you like. Click to proceed.\"")
CODE ("\"The experiment has finished.\"")
CODE ("3 response categories")
CODE1 ("0.1 0.3 0.4 0.6 \"first\" \"\" \"A\"")
CODE1 ("0.4 0.6 0.4 0.6 \"second\" \"\" \"\"")
CODE1 ("0.7 0.9 0.4 0.6 \"third\" \"\" \"B\"")
CODE ("0 goodness categories")
NORMAL ("In this example, the subject has to click 400 times. She sees three buttons, "
	"labelled %first, %second, and %third, but the second button (the one with the empty response category) "
	"is not clickable: it has a light grey rather than a yellow interior and cannot be chosen by the subject. "
	"In your ResultsMFC object, you will only see %A and %B responses.")
ENTRY ("11. A 4I-oddity experiment")
NORMAL ("In the four-items-oddity task, the subject will hear four stimuli in sequence, and has to say "
	"whether the second or the third is the odd one out. The other three substimuli are identical. "
	"An experiment file could look as follows:")
CODE ("\"ooTextFile\"")
CODE ("\"ExperimentMFC 3\"")
CODE ("\"stimuli/\"  \".wav\"")
CODE ("carrierBefore = \"\"")
CODE ("carrierAfter = \"\"")
CODE ("initialSilenceDuration = 0.5 seconds")
CODE ("interStimulusInterval = 0.3 seconds")
CODE ("numberOfDifferentStimuli = 60")
CODE1 ("\"300,300,320,300\"  \"300,320,300,300\"")
CODE1 ("\"300,300,340,300\"  \"300,340,300,300\"")
CODE1 ("\"300,300,360,300\"  \"300,360,300,300\"")
CODE1 ("\"320,320,300,320\"  \"320,300,320,320\"")
CODE1 ("\"320,320,340,320\"  \"320,340,320,320\"")
CODE1 ("\"320,320,360,320\"  \"320,360,320,320\"")
CODE1 ("\"320,320,380,320\"  \"320,380,320,320\"")
CODE1 ("\"340,340,300,340\"  \"340,300,340,340\"")
CODE1 ("\"340,340,320,340\"  \"340,320,340,340\"")
CODE1 ("\"340,340,360,340\"  \"340,360,340,340\"")
CODE1 ("\"340,340,380,340\"  \"340,380,340,340\"")
CODE1 ("\"340,340,400,340\"  \"340,400,340,340\"")
CODE1 ("\"360,360,300,360\"  \"360,300,360,360\"")
CODE1 ("\"360,360,320,360\"  \"360,320,360,360\"")
CODE1 ("\"360,360,340,360\"  \"360,340,360,360\"")
CODE1 ("\"360,360,380,360\"  \"360,380,360,360\"")
CODE1 ("\"360,360,400,360\"  \"360,400,360,360\"")
CODE1 ("\"360,360,420,360\"  \"360,420,360,360\"")
CODE1 ("\"380,380,320,380\"  \"380,320,380,380\"")
CODE1 ("\"380,380,340,380\"  \"380,340,380,380\"")
CODE1 ("\"380,380,360,380\"  \"380,360,380,380\"")
CODE1 ("\"380,380,400,380\"  \"380,400,380,380\"")
CODE1 ("\"380,380,420,380\"  \"380,420,380,380\"")
CODE1 ("\"400,400,340,400\"  \"400,340,400,400\"")
CODE1 ("\"400,400,360,400\"  \"400,360,400,400\"")
CODE1 ("\"400,400,380,400\"  \"400,380,400,400\"")
CODE1 ("\"400,400,420,400\"  \"400,420,400,400\"")
CODE1 ("\"420,420,360,420\"  \"420,360,420,420\"")
CODE1 ("\"420,420,380,420\"  \"420,380,420,420\"")
CODE1 ("\"420,420,400,420\"  \"420,400,420,420\"")
CODE ("numberOfReplicationsPerStimulus = 5")
CODE ("breakAfterEvery = 40")
CODE ("randomize = <PermuteBalancedNoDoublets>")
CODE ("startText = \"Click to start.\"")
CODE ("runText = \"Say whether the second or the third sound is different from the rest.\"")
CODE ("pauseText = \"You can have a short break if you like. Click to proceed.\"")
CODE ("endText = \"The experiment has finished.\"")
CODE ("numberOfResponseCategories = 4")
CODE1 ("0.04 0.24 0.4 0.6 \"first\" \"\" \"\"")
CODE1 ("0.28 0.48 0.4 0.6 \"second\" \"\" \"2\"")
CODE1 ("0.52 0.72 0.4 0.6 \"third\" \"\" \"3\"")
CODE1 ("0.76 0.96 0.4 0.6 \"fourth\" \"\" \"\"")
CODE ("numberOfGoodnessCategories = 0")
NORMAL ("In this example, the subject has to click 300 times. She sees four buttons, "
	"but the first and fourth buttons cannot be chosen. "
	"In your ResultsMFC object, you will only see the responses %2 and %3.")
ENTRY ("12. Further tricks")
NORMAL ("Praat only supports a fixed inter-stimulus interval, but sometimes you may want to test "
	"discrimination as a function of the inter-stimulus interval itself. You can achieve this by "
	"supplying an %interStimulusInterval of 0 and using sound files with various silences:")
CODE1 ("\"300,silence0.5,320\"  \"300,silence1.5,320\"  \"300,silence4.5,320\"")
NORMAL ("In this example, you have to supply the sound files ##silence0.5.wav# and so on. You can "
	"create them with the help of @@Create Sound...@ (supply a %formula of 0).")
MAN_END

}

/* End of file manual_Exp.c */
