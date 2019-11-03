/* ExperimentMFC_def.h
 *
 * Copyright (C) 2001-2007,2009,2011,2013,2015-2019 Paul Boersma
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
 * pb 2003/02/07 added rootDirectory
 * pb 2003/03/08 added stimulusMedialSilenceDuration and stimulusInitialSilenceDuration (version 2)
 * pb 2004/06/22 added response key (version 3)
 * pb 2005/11/21 added replayButton (version 4)
 * pb 2005/12/04 added okButton and oopsButton (version 4)
 * pb 2005/12/06 fix stimuliAreSounds: defaults to TRUE
 * pb 2007/08/12 wchar
 * pb 2007/09/26 added font size (version 5)
 * pb 2011/03/03 added reaction time (version 2 of ResultsMFC)
 * pb 2013/01/01 added finalSilenceDuration and blankWhilePlaying (version 6)
 * pb 2016/09/25 added font size and response key to goodness ratings (version 7)
 */

/********* class ExperimentMFC **********/

#define ooSTRUCT SoundMFC
oo_DEFINE_STRUCT (SoundMFC)

	oo_STRING (name)

	#if ! oo_READING && ! oo_WRITING
		oo_OBJECT (Sound, 0, sound)
	#endif

oo_END_STRUCT (SoundMFC)
#undef ooSTRUCT


#define ooSTRUCT StimulusMFC
oo_DEFINE_STRUCT (StimulusMFC)

	oo_STRING (name)
	oo_FROM (4)
		oo_STRING (visibleText)
	oo_ENDFROM

	#if ! oo_READING && ! oo_WRITING
		oo_OBJECT (Sound, 0, sound)
	#endif

oo_END_STRUCT (StimulusMFC)
#undef ooSTRUCT


#define ooSTRUCT ResponseMFC
oo_DEFINE_STRUCT (ResponseMFC)

	oo_FLOAT (left)
	oo_FLOAT (right)
	oo_FLOAT (bottom)
	oo_FLOAT (top)
	oo_STRING (label)
	oo_FROM (5)
		oo_INT16 (fontSize)
	oo_ENDFROM
	oo_FROM (3)
		oo_STRING (key)
	oo_ENDFROM
	oo_STRING (name)

	#if ! oo_READING && ! oo_WRITING
		oo_OBJECT (Sound, 0, sound)
	#endif

oo_END_STRUCT (ResponseMFC)
#undef ooSTRUCT


#define ooSTRUCT GoodnessMFC
oo_DEFINE_STRUCT (GoodnessMFC)

	oo_FLOAT (left)
	oo_FLOAT (right)
	oo_FLOAT (bottom)
	oo_FLOAT (top)
	oo_STRING (label)
	oo_FROM (7)
		oo_INT16 (fontSize)
		oo_STRING (key)
	oo_ENDFROM

oo_END_STRUCT (GoodnessMFC)
#undef ooSTRUCT


#define ooSTRUCT ExperimentMFC
oo_DEFINE_CLASS (ExperimentMFC, Daata)

	oo_FROM (6)
		oo_QUESTION (blankWhilePlaying)
	oo_ENDFROM
	oo_FROM (4)
		oo_QUESTION (stimuliAreSounds)
	oo_ENDFROM
	oo_STRING (stimulusFileNameHead)
	oo_STRING (stimulusFileNameTail)
	oo_STRUCT (SoundMFC, stimulusCarrierBefore)
	oo_STRUCT (SoundMFC, stimulusCarrierAfter)
	oo_FROM (2)
		oo_DOUBLE (stimulusInitialSilenceDuration)
		oo_DOUBLE (stimulusMedialSilenceDuration)
	oo_ENDFROM
	oo_FROM (6)
		oo_DOUBLE (stimulusFinalSilenceDuration)
	oo_ENDFROM
	oo_INTEGER (numberOfDifferentStimuli)
	oo_STRUCTVEC (StimulusMFC, stimulus, numberOfDifferentStimuli)
	oo_INTEGER (numberOfReplicationsPerStimulus)
	oo_INTEGER (breakAfterEvery)
	oo_ENUM (kExperiment_randomize, randomize)
	oo_STRING (startText)
	oo_STRING (runText)
	oo_STRING (pauseText)
	oo_STRING (endText)
	oo_FROM (4)
		oo_INTEGER (maximumNumberOfReplays)
		oo_FLOAT (replay_left)
		oo_FLOAT (replay_right)
		oo_FLOAT (replay_bottom)
		oo_FLOAT (replay_top)
		oo_STRING (replay_label)
		oo_STRING (replay_key)
		oo_FLOAT (ok_left)
		oo_FLOAT (ok_right)
		oo_FLOAT (ok_bottom)
		oo_FLOAT (ok_top)
		oo_STRING (ok_label)
		oo_STRING (ok_key)
		oo_FLOAT (oops_left)
		oo_FLOAT (oops_right)
		oo_FLOAT (oops_bottom)
		oo_FLOAT (oops_top)
		oo_STRING (oops_label)
		oo_STRING (oops_key)
	oo_ENDFROM
	oo_FROM (4)
		oo_QUESTION (responsesAreSounds)
		oo_STRING (responseFileNameHead)
		oo_STRING (responseFileNameTail)
		oo_STRUCT (SoundMFC, responseCarrierBefore)
		oo_STRUCT (SoundMFC, responseCarrierAfter)
		oo_DOUBLE (responseInitialSilenceDuration)
		oo_DOUBLE (responseMedialSilenceDuration)
	oo_ENDFROM
	oo_FROM (6)
		oo_DOUBLE (responseFinalSilenceDuration)
	oo_ENDFROM
	oo_INTEGER (numberOfDifferentResponses)
	oo_STRUCTVEC (ResponseMFC, response, numberOfDifferentResponses)
	oo_FROM (1)
		oo_INTEGER (numberOfGoodnessCategories)
		oo_STRUCTVEC (GoodnessMFC, goodness, numberOfGoodnessCategories)
	oo_ENDFROM
	#if ! oo_READING && ! oo_WRITING
		oo_DOUBLE (samplePeriod)
		oo_INTEGER (numberOfChannels)
		oo_BOOLEAN (pausing)
		oo_INTEGER (trial)
		oo_INTEGER (numberOfTrials)
		oo_INTVEC (stimuli, numberOfTrials)
		oo_INTVEC (responses, numberOfTrials)
		oo_VEC (goodnesses, numberOfTrials)
		oo_DOUBLE (startingTime)
		oo_VEC (reactionTimes, numberOfTrials)
		oo_OBJECT (Sound, 0, playBuffer)
	#endif
	oo_DIR (rootDirectory)
	#if oo_READING
		MelderDir_copy (& Data_directoryBeingRead, & rootDirectory);
		oo_VERSION_UNTIL (4)
			stimuliAreSounds = true;
		oo_VERSION_END
	#endif

oo_END_CLASS (ExperimentMFC)
#undef ooSTRUCT


/********** class ResultsMFC **********/

#define ooSTRUCT TrialMFC
oo_DEFINE_STRUCT (TrialMFC)

	oo_STRING (stimulus)
	oo_STRING (response)
	oo_FROM (1)
		oo_DOUBLE (goodness)
	oo_ENDFROM
	oo_FROM (2)
		oo_DOUBLE (reactionTime)
	oo_ENDFROM

oo_END_STRUCT (TrialMFC)
#undef ooSTRUCT


#define ooSTRUCT ResultsMFC
oo_DEFINE_CLASS (ResultsMFC, Daata)

	oo_INTEGER (numberOfTrials)
	oo_STRUCTVEC (TrialMFC, result, numberOfTrials)

oo_END_CLASS (ResultsMFC)
#undef ooSTRUCT


/* End of file ExperimentMFC_def.h */
