#ifndef _ExperimentMFC_h_
#define _ExperimentMFC_h_
/* ExperimentMFC.h
 *
 * Copyright (C) 2001-2003 Paul Boersma
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
 * pb 2001/06/07
 * pb 2002/07/16 GPL
 * pb 2002/08/28 to Table
 * pb 2003/03/09 playStimulus
 */

#ifndef _Sound_h_
	#include "Sound.h"
#endif
#ifndef _Categories_h_
	#include "Categories.h"
#endif
#ifndef _Table_h_
	#include "Table.h"
#endif
#include "Experiment_enums.h"

#include "ExperimentMFC_def.h"
#define ExperimentMFC_methods Data_methods
oo_CLASS_CREATE (ExperimentMFC, Data)

#define ResultsMFC_methods Data_methods
oo_CLASS_CREATE (ResultsMFC, Data)

int ExperimentMFC_start (ExperimentMFC me);
void ExperimentMFC_playStimulus (ExperimentMFC me, long istim);

ResultsMFC ResultsMFC_create (long numberOfResults);
ResultsMFC ExperimentMFC_extractResults (ExperimentMFC me);
ResultsMFC ResultsMFC_removeUnsharedStimuli (ResultsMFC me, ResultsMFC thee);

Table ResultsMFCs_to_Table (Collection me);

Categories ResultsMFC_to_Categories_stimuli (ResultsMFC me);
Categories ResultsMFC_to_Categories_responses (ResultsMFC me);
void Categories_sort (Categories me);
double Categories_getEnthropy (Categories me);

/* End of file ExperimentMFC.h */
#endif
