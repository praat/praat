/* ExperimentMFC_def.h
 *
 * Copyright (C) 2001-2011 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2003/02/07 added rootDirectory
 * pb 2003/03/08 added interStimulusInterval and initialSilenceDuration (version 2)
 * pb 2004/06/22 added response key (version 3)
 * pb 2005/11/21 added replayButton (version 4)
 * pb 2005/12/04 added okButton and oopsButton (version 4)
 * pb 2005/12/06 fix stimuliAreSounds: defaults to TRUE
 * pb 2007/08/12 wchar_t
 * pb 2007/09/26 added font size (version 5)
 * pb 2011/03/03 added reaction time (version 2 of ResultsMFC)
 */

/********* class ExperimentMFC **********/

#define ooSTRUCT SoundMFC
oo_DEFINE_STRUCT (SoundMFC)

	oo_STRINGW (name)
	#if !oo_READING && !oo_WRITING
		oo_OBJECT (Sound, 0, sound)
	#endif
		
oo_END_STRUCT (SoundMFC)
#undef ooSTRUCT


#define ooSTRUCT StimulusMFC
oo_DEFINE_STRUCT (StimulusMFC)

	oo_STRINGW (name)
	oo_FROM (4)
		oo_STRINGW (visibleText)
	oo_ENDFROM
	#if !oo_READING && !oo_WRITING
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
	oo_STRINGW (label)
	oo_FROM (5)
		oo_INT (fontSize)
	oo_ENDFROM
	oo_FROM (3)
		oo_STRINGW (key)
	oo_ENDFROM
	oo_STRINGW (name)
	#if !oo_READING && !oo_WRITING
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
	oo_STRINGW (label)

oo_END_STRUCT (GoodnessMFC)
#undef ooSTRUCT


#define ooSTRUCT ExperimentMFC
oo_DEFINE_CLASS (ExperimentMFC, Data)

	oo_FROM (4)
		oo_QUESTION (stimuliAreSounds)
	oo_ENDFROM
	oo_STRINGW (stimulusFileNameHead)
	oo_STRINGW (stimulusFileNameTail)
	oo_STRUCT (SoundMFC, stimulusCarrierBefore)
	oo_STRUCT (SoundMFC, stimulusCarrierAfter)
	oo_FROM (2)
		oo_DOUBLE (stimulusInitialSilenceDuration)
		oo_DOUBLE (stimulusMedialSilenceDuration)
	oo_ENDFROM
	oo_LONG (numberOfDifferentStimuli)
	oo_STRUCT_VECTOR (StimulusMFC, stimulus, my numberOfDifferentStimuli)
	oo_LONG (numberOfReplicationsPerStimulus)
	oo_LONG (breakAfterEvery)
	oo_ENUM (kExperiment_randomize, randomize)
	oo_STRINGW (startText)
	oo_STRINGW (runText)
	oo_STRINGW (pauseText)
	oo_STRINGW (endText)
	oo_FROM (4)
		oo_LONG (maximumNumberOfReplays)
		oo_FLOAT (replay_left)
		oo_FLOAT (replay_right)
		oo_FLOAT (replay_bottom)
		oo_FLOAT (replay_top)
		oo_STRINGW (replay_label)
		oo_STRINGW (replay_key)
		oo_FLOAT (ok_left)
		oo_FLOAT (ok_right)
		oo_FLOAT (ok_bottom)
		oo_FLOAT (ok_top)
		oo_STRINGW (ok_label)
		oo_STRINGW (ok_key)
		oo_FLOAT (oops_left)
		oo_FLOAT (oops_right)
		oo_FLOAT (oops_bottom)
		oo_FLOAT (oops_top)
		oo_STRINGW (oops_label)
		oo_STRINGW (oops_key)
	oo_ENDFROM
	oo_FROM (4)
		oo_QUESTION (responsesAreSounds)
		oo_STRINGW (responseFileNameHead)
		oo_STRINGW (responseFileNameTail)
		oo_STRUCT (SoundMFC, responseCarrierBefore)
		oo_STRUCT (SoundMFC, responseCarrierAfter)
		oo_DOUBLE (responseInitialSilenceDuration)
		oo_DOUBLE (responseMedialSilenceDuration)
	oo_ENDFROM
	oo_LONG (numberOfDifferentResponses)
	oo_STRUCT_VECTOR (ResponseMFC, response, my numberOfDifferentResponses)
	oo_FROM (1)
		oo_LONG (numberOfGoodnessCategories)
		oo_STRUCT_VECTOR (GoodnessMFC, goodness, my numberOfGoodnessCategories)
	oo_ENDFROM
	#if !oo_READING && !oo_WRITING
		oo_DOUBLE (samplePeriod)
		oo_INT (numberOfChannels)
		oo_BOOLEAN (pausing)
		oo_LONG (trial)
		oo_LONG (numberOfTrials)
		oo_LONG_VECTOR (stimuli, my numberOfTrials)
		oo_LONG_VECTOR (responses, my numberOfTrials)
		oo_DOUBLE_VECTOR (goodnesses, my numberOfTrials)
		oo_DOUBLE (startingTime)
		oo_DOUBLE_VECTOR (reactionTimes, my numberOfTrials)
		oo_OBJECT (Sound, 0, playBuffer)
	#endif
	oo_DIR (rootDirectory)
	#if oo_READING
		MelderDir_copy (& Data_directoryBeingRead, & my rootDirectory);
		if (localVersion < 4) my stimuliAreSounds = TRUE;
	#endif

oo_END_CLASS (ExperimentMFC)
#undef ooSTRUCT


/********** class ResultsMFC **********/

#define ooSTRUCT TrialMFC
oo_DEFINE_STRUCT (TrialMFC)

	oo_STRINGW (stimulus)
	oo_STRINGW (response)
	oo_FROM (1)
		oo_DOUBLE (goodness)
	oo_ENDFROM
	oo_FROM (2)
		oo_DOUBLE (reactionTime)
	oo_ENDFROM

oo_END_STRUCT (TrialMFC)
#undef ooSTRUCT


#define ooSTRUCT ResultsMFC
oo_DEFINE_CLASS (ResultsMFC, Data)

	oo_LONG (numberOfTrials)
	oo_STRUCT_VECTOR (TrialMFC, result, my numberOfTrials)

oo_END_CLASS (ResultsMFC)
#undef ooSTRUCT


/* End of file ExperimentMFC_def.h */
