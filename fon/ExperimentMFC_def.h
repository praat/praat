/* ExperimentMFC_def.h
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

/*
 * pb 2002/07/16 GPL
 * pb 2003/02/07 added rootDirectory
 * pb 2003/03/08 added interStimulusInterval
 * pb 2003/03/08 added initialSilenceDuration
 * pb 2004/06/22 added response key
 */


#define ooSTRUCT StimulusMFC
oo_DEFINE_STRUCT (StimulusMFC)

	oo_STRING (name)
	#if !oo_READING && !oo_WRITING
		oo_OBJECT (Sound, sound)
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
	oo_FROM (3)
		oo_STRING (key)
	oo_ENDFROM
	oo_STRING (category)

oo_END_STRUCT (ResponseMFC)
#undef ooSTRUCT


#define ooSTRUCT GoodnessMFC
oo_DEFINE_STRUCT (GoodnessMFC)

	oo_FLOAT (left)
	oo_FLOAT (right)
	oo_FLOAT (bottom)
	oo_FLOAT (top)
	oo_STRING (label)

oo_END_STRUCT (GoodnessMFC)
#undef ooSTRUCT


#define ooSTRUCT TrialMFC
oo_DEFINE_STRUCT (TrialMFC)

	oo_STRING (stimulus)
	oo_STRING (response)
	oo_FROM (1)
		oo_DOUBLE (goodness)
	oo_ENDFROM

oo_END_STRUCT (TrialMFC)
#undef ooSTRUCT


#define ooSTRUCT ResultsMFC
oo_DEFINE_CLASS (ResultsMFC, Data)

	oo_LONG (numberOfTrials)
	oo_STRUCT_VECTOR (TrialMFC, result, my numberOfTrials)

oo_END_CLASS (ResultsMFC)
#undef ooSTRUCT


#define ooSTRUCT ExperimentMFC
oo_DEFINE_CLASS (ExperimentMFC, Data)

	oo_STRING (fileNameHead)
	oo_STRING (fileNameTail)
	oo_STRUCT (StimulusMFC, carrierBefore)
	oo_STRUCT (StimulusMFC, carrierAfter)
	oo_FROM (2)
		oo_DOUBLE (initialSilenceDuration)
		oo_DOUBLE (interStimulusInterval)
	oo_ENDFROM
	oo_LONG (numberOfDifferentStimuli)
	oo_STRUCT_VECTOR (StimulusMFC, stimulus, my numberOfDifferentStimuli)
	oo_LONG (numberOfReplicationsPerStimulus)
	oo_LONG (breakAfterEvery)
	oo_ENUM (Experiment_RANDOMIZE, randomize)
	oo_STRING (startText)
	oo_STRING (runText)
	oo_STRING (pauseText)
	oo_STRING (endText)
	oo_LONG (numberOfResponseCategories)
	oo_STRUCT_VECTOR (ResponseMFC, response, my numberOfResponseCategories)
	oo_FROM (1)
		oo_LONG (numberOfGoodnessCategories)
		oo_STRUCT_VECTOR (GoodnessMFC, goodness, my numberOfGoodnessCategories)
	oo_ENDFROM
	#if !oo_READING && !oo_WRITING
		oo_DOUBLE (samplePeriod)
		oo_BOOLEAN (pausing)
		oo_LONG (trial)
		oo_LONG (numberOfTrials)
		oo_LONG_VECTOR (stimuli, my numberOfTrials)
		oo_LONG_VECTOR (responses, my numberOfTrials)
		oo_DOUBLE_VECTOR (goodnesses, my numberOfTrials)
		oo_OBJECT (Sound, playBuffer)
	#endif
	oo_DIR (rootDirectory)
	#if oo_READING
		MelderFile_getParentDir (& Data_fileBeingRead, & my rootDirectory);
	#endif

oo_END_CLASS (ExperimentMFC)
#undef ooSTRUCT


/* End of file ExperimentMFC_def.h */
