/* ExperimentMFC.cpp
 *
 * Copyright (C) 2001-2009,2011-2013,2015-2020 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2002/10/31 NUMlog2
 * pb 2003/03/08 inter-stimulus interval; version 2
 * pb 2003/09/14 MelderDir_relativePathToFile
 * pb 2004/06/22 added response keys; version 3
 * pb 2004/08/12 removed a bug (something said carrierBefore instead of carrierAfter)
 *     that caused Praat to crash if the carrier before was longer than the carrier after
 * pb 2005/11/21 added replay button; version 4
 * pb 2005/12/02 response sounds are read
 * pb 2006/10/28 erased MacOS 9 stuff
 * pb 2006/12/20 stereo
 * pb 2007/08/12 wchar
 * pb 2007/09/26 added font size; version 5
 * pb 2007/10/01 can write as encoding
 * pb 2008/04/08 in ExtractResults, check that a resonse was given
 * pb 2008/10/20 except nonstandard sound files
 * pb 2009/03/18 removed a bug introduced by the previous change (an error check got hidden)
 * pb 2011/03/03 added reaction times; version 2 of ResultsMFC
 * pb 2011/03/15 allowed result extraction from incomplete experiments
 * pb 2011/03/23 C++
 * pb 2011/07/06 C++
 * pb 2013/01/01 added blank while playing; version 6
 * pb 2016/09/25 added font size and response key to goodness ratings; version 7
 */

#include "ExperimentMFC.h"

#include "oo_DESTROY.h"
#include "ExperimentMFC_def.h"
#include "oo_COPY.h"
#include "ExperimentMFC_def.h"
#include "oo_EQUAL.h"
#include "ExperimentMFC_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ExperimentMFC_def.h"
#include "oo_READ_TEXT.h"
#include "ExperimentMFC_def.h"
#include "oo_WRITE_TEXT.h"
#include "ExperimentMFC_def.h"
#include "oo_READ_BINARY.h"
#include "ExperimentMFC_def.h"
#include "oo_WRITE_BINARY.h"
#include "ExperimentMFC_def.h"
#include "oo_DESCRIPTION.h"
#include "ExperimentMFC_def.h"

#include "enums_getText.h"
#include "Experiment_enums.h"
#include "enums_getValue.h"
#include "Experiment_enums.h"


#pragma mark - class ExperimentMFC

Thing_implement (ExperimentMFC, Daata, 7);

static void readSound (ExperimentMFC me, conststring32 fileNameHead, conststring32 fileNameTail,
	double medialSilenceDuration, conststring32 name, autoSound *sound)
{
	char32 fileNameBuffer [256], *fileNames = & fileNameBuffer [0];
	Melder_sprint (fileNameBuffer,256, name);
	structMelderFile file { };
	/*
		The following conversion is needed when fileNameHead is an absolute path,
		and the stimulus names contain slashes for relative paths.
		An ugly case, but allowed.
	*/
	#if defined (_WIN32)
		for (;;) { char32 *slash = str32chr (fileNames, U'/'); if (! slash) break; *slash = U'\\'; }
	#endif
	sound->reset();
	char32 pathName [kMelder_MAXPATH+1];
	/*
		`fileNames` can contain commas, which separate partial file names.
		The separate files should be concatenated.
	*/
	for (;;) {
		/*
			Determine partial file name.
		*/
		char32 *comma = str32chr (fileNames, U',');
		if (comma)
			*comma = U'\0';
		/*
			Determine complete (relative) file name.
		*/
		Melder_sprint (pathName,kMelder_MAXPATH+1, fileNameHead, fileNames, fileNameTail);
		/*
			Make sure we are in the correct folder.
		*/
		if (MelderDir_isNull (& my rootDirectory)) {
			/*
				Absolute file name.
			*/
			Melder_pathToFile (pathName, & file);
		} else {
			/*
				Relative or absolute file name.
			*/
			MelderDir_relativePathToFile (& my rootDirectory, pathName, & file);
			if (Melder_debug == 32) {
				MelderInfo_open ();
				MelderInfo_writeLine (U"Path name <", pathName, U">");
				MelderInfo_writeLine (U"Root folder <", my rootDirectory.path, U">");
				MelderInfo_writeLine (U"Full path name <", file.path, U">");
				MelderInfo_close ();
			}
		}
		/*
			Read the substimulus.
		*/
		autoSound substimulus = Data_readFromFile (& file). static_cast_move<structSound>();
		Melder_require (substimulus -> classInfo == classSound,
			U"File ", & file, U" contains a ", Thing_className (substimulus.get()), U" instead of a sound.");
		/*
			Check whether all sounds have the same number of channels.
		*/
		if (my numberOfChannels == 0) {   // first time?
			my numberOfChannels = substimulus -> ny;
		} else if (substimulus -> ny != my numberOfChannels) {
			Melder_throw (U"The sound in file ", & file, U" has a different number of channels than some other sound.");
		}
		/*
			Check whether all sounds have the same sampling frequency.
		*/
		if (my samplePeriod == 0.0) {
			my samplePeriod = substimulus -> dx;   // this must be the first sound read
		} else if (substimulus -> dx != my samplePeriod) {
			Melder_throw (U"The sound in file ", & file, U" has a different sampling frequency than some other sound.");
		}
		/*
			Append the substimuli, perhaps with silent intervals.
		*/
		if (*sound) {
			*sound = Sounds_append (sound->get(), medialSilenceDuration, substimulus.get());
		} else {
			*sound = substimulus.move();
		}
		/*
			Cycle.
		*/
		if (! comma)
			break;
		fileNames = & comma [1];
	}
}

static void permuteRandomly (ExperimentMFC me, integer first, integer last) {
	for (integer itrial = first; itrial < last; itrial ++) {
		integer jtrial = NUMrandomInteger (itrial, last);
		integer dummy = my stimuli [jtrial];
		my stimuli [jtrial] = my stimuli [itrial];
		my stimuli [itrial] = dummy;
	}
}

void ExperimentMFC_start (ExperimentMFC me) {
	try {
		integer maximumStimulusPlaySamples, maximumResponsePlaySamples, maximumPlaySamples;
		integer stimulusCarrierBeforeSamples = 0, stimulusCarrierAfterSamples = 0, maximumStimulusSamples = 0;
		integer responseCarrierBeforeSamples = 0, responseCarrierAfterSamples = 0, maximumResponseSamples = 0;
		Melder_warningOff ();
		my trial = 0;
		my playBuffer.reset();   // is this needed?
		my pausing = false;
		my numberOfTrials = my numberOfDifferentStimuli * my numberOfReplicationsPerStimulus;
		my stimuli = zero_INTVEC (my numberOfTrials);
		my responses = zero_INTVEC (my numberOfTrials);
		my goodnesses = zero_VEC (my numberOfTrials);
		my reactionTimes = zero_VEC (my numberOfTrials);
		/*
			Read all the sounds. They must all have the same sampling frequency and number of channels.
		*/
		my samplePeriod = 0.0;
		my numberOfChannels = 0;
		if (my stimuliAreSounds) {
			if (my stimulusCarrierBefore. name && my stimulusCarrierBefore. name [0]) {
				readSound (me, my stimulusFileNameHead.get(), my stimulusFileNameTail.get(), my stimulusMedialSilenceDuration,
						my stimulusCarrierBefore. name.get(), & my stimulusCarrierBefore. sound);
				stimulusCarrierBeforeSamples = my stimulusCarrierBefore. sound -> nx;
			}
			if (my stimulusCarrierAfter. name && my stimulusCarrierAfter. name [0]) {
				readSound (me, my stimulusFileNameHead.get(), my stimulusFileNameTail.get(), my stimulusMedialSilenceDuration,
						my stimulusCarrierAfter. name.get(), & my stimulusCarrierAfter. sound);
				stimulusCarrierAfterSamples = my stimulusCarrierAfter. sound -> nx;
			}
			for (integer istim = 1; istim <= my numberOfDifferentStimuli; istim ++) {
				readSound (me, my stimulusFileNameHead.get(), my stimulusFileNameTail.get(), my stimulusMedialSilenceDuration,
						my stimulus [istim]. name.get(), & my stimulus [istim]. sound);
				if (my stimulus [istim]. sound -> nx > maximumStimulusSamples)
					maximumStimulusSamples = my stimulus [istim]. sound -> nx;
			}
		}
		if (my responsesAreSounds) {
			if (my responseCarrierBefore. name && my responseCarrierBefore. name [0]) {
				readSound (me, my responseFileNameHead.get(), my responseFileNameTail.get(), my responseMedialSilenceDuration,
						my responseCarrierBefore. name.get(), & my responseCarrierBefore. sound);
				responseCarrierBeforeSamples = my responseCarrierBefore. sound -> nx;
			}
			if (my responseCarrierAfter. name && my responseCarrierAfter. name [0]) {
				readSound (me, my responseFileNameHead.get(), my responseFileNameTail.get(), my responseMedialSilenceDuration,
						my responseCarrierAfter. name.get(), & my responseCarrierAfter. sound);
				responseCarrierAfterSamples = my responseCarrierAfter. sound -> nx;
			}
			for (integer iresp = 1; iresp <= my numberOfDifferentResponses; iresp ++) {
				readSound (me, my responseFileNameHead.get(), my responseFileNameTail.get(), my responseMedialSilenceDuration,
						my response [iresp]. name.get(), & my response [iresp]. sound);
				if (my response [iresp]. sound -> nx > maximumResponseSamples)
					maximumResponseSamples = my response [iresp]. sound -> nx;
			}
		}
		/*
			Create the play buffer.
		*/
		maximumStimulusPlaySamples =
			Melder_iround (my stimulusInitialSilenceDuration / my samplePeriod)
			+ Melder_iround (my stimulusFinalSilenceDuration / my samplePeriod)
			+ stimulusCarrierBeforeSamples + maximumStimulusSamples + stimulusCarrierAfterSamples + 2;
		maximumResponsePlaySamples =
			Melder_iround (my responseInitialSilenceDuration / my samplePeriod)
			+ Melder_iround (my responseFinalSilenceDuration / my samplePeriod)
			+ responseCarrierBeforeSamples + maximumResponseSamples + responseCarrierAfterSamples + 2;
		maximumPlaySamples = maximumStimulusPlaySamples > maximumResponsePlaySamples ? maximumStimulusPlaySamples : maximumResponsePlaySamples;
		my playBuffer = Sound_create (my numberOfChannels, 0.0, maximumPlaySamples * my samplePeriod,
			maximumPlaySamples, my samplePeriod, 0.5 * my samplePeriod);
		/*
			Determine the order in which the stimuli will be presented to the subject.
		*/
		if (my randomize == kExperiment_randomize::CYCLIC_NON_RANDOM) {
			for (integer itrial = 1; itrial <= my numberOfTrials; itrial ++)
				my stimuli [itrial] = (itrial - 1) % my numberOfDifferentStimuli + 1;
		} else if (my randomize == kExperiment_randomize::PERMUTE_ALL) {
			for (integer itrial = 1; itrial <= my numberOfTrials; itrial ++)
				my stimuli [itrial] = (itrial - 1) % my numberOfDifferentStimuli + 1;
			permuteRandomly (me, 1, my numberOfTrials);
		} else if (my randomize == kExperiment_randomize::PERMUTE_BALANCED) {
			for (integer ireplica = 1; ireplica <= my numberOfReplicationsPerStimulus; ireplica ++) {
				integer offset = (ireplica - 1) * my numberOfDifferentStimuli;
				for (integer istim = 1; istim <= my numberOfDifferentStimuli; istim ++)
					my stimuli [offset + istim] = istim;
				permuteRandomly (me, offset + 1, offset + my numberOfDifferentStimuli);
			}
		} else if (my randomize == kExperiment_randomize::PERMUTE_BALANCED_NO_DOUBLETS) {
			for (integer ireplica = 1; ireplica <= my numberOfReplicationsPerStimulus; ireplica ++) {
				integer offset = (ireplica - 1) * my numberOfDifferentStimuli;
				for (integer istim = 1; istim <= my numberOfDifferentStimuli; istim ++)
					my stimuli [offset + istim] = istim;
				do {
					permuteRandomly (me, offset + 1, offset + my numberOfDifferentStimuli);
				} while (ireplica != 1 && my stimuli [offset + 1] == my stimuli [offset] && my numberOfDifferentStimuli > 1);
			}
		} else if (my randomize == kExperiment_randomize::WITH_REPLACEMENT) {
			for (integer itrial = 1; itrial <= my numberOfTrials; itrial ++)
				my stimuli [itrial] = NUMrandomInteger (1, my numberOfDifferentStimuli);
		}
		Melder_warningOn ();
	} catch (MelderError) {
		Melder_warningOn ();
		my numberOfTrials = 0;
		my stimuli. reset();
		Melder_throw (me, U": not started.");
	}
}

static void playSound (ExperimentMFC me, Sound sound, Sound carrierBefore, Sound carrierAfter,
	double initialSilenceDuration, double finalSilenceDuration)
{
	integer numberOfSamplesWritten = 0;

	const integer initialSilenceSamples = Melder_iround (initialSilenceDuration / my samplePeriod);
	my playBuffer -> z.verticalBand (1, initialSilenceSamples) <<= 0.0;
	numberOfSamplesWritten += initialSilenceSamples;

	if (carrierBefore) {
		my playBuffer -> z.verticalBand (numberOfSamplesWritten + 1, numberOfSamplesWritten + carrierBefore -> nx)
				<<= carrierBefore -> z.all();
		numberOfSamplesWritten += carrierBefore -> nx;
	}

	if (sound) {
		my playBuffer -> z.verticalBand (numberOfSamplesWritten + 1, numberOfSamplesWritten + sound -> nx)
				<<= sound -> z.all();
		numberOfSamplesWritten += sound -> nx;
	}

	if (carrierAfter) {
		my playBuffer -> z.verticalBand (numberOfSamplesWritten + 1, numberOfSamplesWritten + carrierAfter -> nx)
				<<= carrierAfter -> z.all();
		numberOfSamplesWritten += carrierAfter -> nx;
	}

	const integer finalSilenceSamples = Melder_iround (finalSilenceDuration / my samplePeriod);
	my playBuffer -> z.verticalBand (numberOfSamplesWritten + 1, numberOfSamplesWritten + finalSilenceSamples) <<= 0.0;
	numberOfSamplesWritten += finalSilenceSamples;

	if (! my blankWhilePlaying)
		my startingTime = Melder_clock ();
	Sound_playPart (my playBuffer.get(), 0.0, numberOfSamplesWritten * my samplePeriod, 0, nullptr);
	if (my blankWhilePlaying)
		my startingTime = Melder_clock ();
}

void ExperimentMFC_playStimulus (ExperimentMFC me, integer istim) {
	playSound (me, my stimulus [istim]. sound.get(),
			my stimulusCarrierBefore. sound.get(), my stimulusCarrierAfter. sound.get(),
			my stimulusInitialSilenceDuration, my stimulusFinalSilenceDuration);
}

void ExperimentMFC_playResponse (ExperimentMFC me, integer iresp) {
	playSound (me, my response [iresp]. sound.get(),
			my responseCarrierBefore. sound.get(), my responseCarrierAfter. sound.get(),
			my responseInitialSilenceDuration, my responseFinalSilenceDuration);
}


#pragma mark - class ExperimentMFCList

Thing_implement (ExperimentMFCList, Ordered, 0);


#pragma mark - class ResultsMFC

Thing_implement (ResultsMFC, Daata, 2);

autoResultsMFC ResultsMFC_create (integer numberOfTrials) {
	try {
		autoResultsMFC me = Thing_new (ResultsMFC);
		my numberOfTrials = numberOfTrials;
		my result = newvectorzero <structTrialMFC> (my numberOfTrials);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ResultsMFC not created.");
	}
}

autoResultsMFC ExperimentMFC_extractResults (ExperimentMFC me) {
	try {
		if (my trial == 0 || my trial <= my numberOfTrials)
			Melder_warning (U"The experiment was not finished. Only the first ", my trial - 1 + my pausing, U" responses are valid.");
		autoResultsMFC thee = ResultsMFC_create (my numberOfTrials);
		for (integer trial = 1; trial <= my numberOfTrials; trial ++) {
			const char32 *pipe = ( my stimulus [my stimuli [trial]]. visibleText ?
					str32chr (my stimulus [my stimuli [trial]]. visibleText.get(), U'|') : nullptr );
			thy result [trial]. stimulus = Melder_dup (Melder_cat (my stimulus [my stimuli [trial]]. name.get(), pipe));
			//if (my responses [trial] < 1) Melder_throw (U"No response for trial ", trial, U".")
			thy result [trial]. response = Melder_dup (my responses [trial] ? my response [my responses [trial]]. name.get() : U"");
			thy result [trial]. goodness = my goodnesses [trial];
			thy result [trial]. reactionTime = my reactionTimes [trial];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": results not extracted.");
	}
}

autoResultsMFC ResultsMFC_removeUnsharedStimuli (ResultsMFC me, ResultsMFC thee) {
	try {
		autoResultsMFC him = ResultsMFC_create (thy numberOfTrials);
		his numberOfTrials = 0;
		for (integer i = 1; i <= thy numberOfTrials; i ++) {
			bool present = false;
			for (integer j = 1; j <= my numberOfTrials; j ++) {
				if (str32equ (thy result [i]. stimulus.get(), my result [j]. stimulus.get())) {
					present = true;
					break;
				}
			}
			if (present) {
				his numberOfTrials ++;
				his result [his numberOfTrials]. stimulus = Melder_dup (thy result [i]. stimulus.get());
				his result [his numberOfTrials]. response = Melder_dup (thy result [i]. response.get());
			}
		}
		if (his numberOfTrials == 0)
			Melder_throw (U"No shared stimuli.");
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": unshared stimuli not removed.");
	}
}

autoTable ResultsMFCs_to_Table (OrderedOf<structResultsMFC>* me) {
	try {
		integer irow = 0;
		bool hasGoodnesses = false, hasReactionTimes = false;
		for (integer iresults = 1; iresults <= my size; iresults ++) {
			ResultsMFC results = my at [iresults];
			for (integer itrial = 1; itrial <= results -> numberOfTrials; itrial ++) {
				irow ++;
				if (results -> result [itrial]. goodness != 0)
					hasGoodnesses = true;
				if (results -> result [itrial]. reactionTime != 0.0)
					hasReactionTimes = true;
			}
		}
		autoTable thee = Table_create (irow, 3 + hasGoodnesses + hasReactionTimes);
		Table_setColumnLabel (thee.get(), 1, U"subject");
		Table_setColumnLabel (thee.get(), 2, U"stimulus");
		Table_setColumnLabel (thee.get(), 3, U"response");
		if (hasGoodnesses)
			Table_setColumnLabel (thee.get(), 4, U"goodness");
		if (hasReactionTimes)
			Table_setColumnLabel (thee.get(), 4 + hasGoodnesses, U"reactionTime");
		irow = 0;
		for (integer iresults = 1; iresults <= my size; iresults ++) {
			ResultsMFC results = my at [iresults];
			for (integer itrial = 1; itrial <= results -> numberOfTrials; itrial ++) {
				irow ++;
				Table_setStringValue (thee.get(), irow, 1, results -> name.get());
				Table_setStringValue (thee.get(), irow, 2, results -> result [itrial]. stimulus.get());
				Table_setStringValue (thee.get(), irow, 3, results -> result [itrial]. response.get());
				if (hasGoodnesses)
					Table_setNumericValue (thee.get(), irow, 4, results -> result [itrial]. goodness);
				if (hasReactionTimes)
					Table_setNumericValue (thee.get(), irow, 4 + hasGoodnesses, results -> result [itrial]. reactionTime);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"ResultsMFC objects not collected to Table.");
	}
}

autoCategories ResultsMFC_to_Categories_stimuli (ResultsMFC me) {
	try {
		autoCategories thee = Categories_create ();
		for (integer trial = 1; trial <= my numberOfTrials; trial ++) {
			autoSimpleString category = SimpleString_create (my result [trial]. stimulus.get());
			thy addItem_move (category.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": stimuli not converted to Categories.");
	}
}

autoCategories ResultsMFC_to_Categories_responses (ResultsMFC me) {
	try {
		autoCategories thee = Categories_create ();
		for (integer trial = 1; trial <= my numberOfTrials; trial ++) {
			autoSimpleString category = SimpleString_create (my result [trial]. response.get());
			thy addItem_move (category.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": responses not converted to Categories.");
	}
}


#pragma mark - class Categories extensions

void Categories_sort (Categories me) {
	my sort (SimpleString_compare);
}

double Categories_getEntropy (Categories me) {
	integer numberOfTokens = 0;
	conststring32 previousString = nullptr;
	double entropy = 0.0;
	autoCategories thee = Data_copy (me);
	Categories_sort (thee.get());
	for (integer i = 1; i <= thy size; i ++) {
		SimpleString s = thy at [i];
		conststring32 string = s -> string.get();
		if (previousString && ! str32equ (string, previousString)) {
			double p = (double) numberOfTokens / thy size;
			entropy -= p * NUMlog2 (p);
			numberOfTokens = 1;
		} else {
			numberOfTokens ++;
		}
		previousString = string;
	}
	if (numberOfTokens) {
		double p = (double) numberOfTokens / thy size;
		entropy -= p * NUMlog2 (p);
	}
	return entropy;
}

/* End of file ExperimentMFC.cpp */
