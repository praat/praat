#ifndef _ExperimentMFC_h_
#define _ExperimentMFC_h_
/* ExperimentMFC.h
 *
 * Copyright (C) 2001-2011,2015 Paul Boersma
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

#include "Sound.h"
#include "Categories.h"
#include "Table.h"

#include "Experiment_enums.h"

#include "ExperimentMFC_def.h"

void ExperimentMFC_start (ExperimentMFC me);
void ExperimentMFC_playStimulus (ExperimentMFC me, long istim);
void ExperimentMFC_playResponse (ExperimentMFC me, long iresp);

autoResultsMFC ResultsMFC_create (long numberOfResults);
autoResultsMFC ExperimentMFC_extractResults (ExperimentMFC me);
autoResultsMFC ResultsMFC_removeUnsharedStimuli (ResultsMFC me, ResultsMFC thee);

autoTable ResultsMFCs_to_Table (Collection me);

autoCategories ResultsMFC_to_Categories_stimuli (ResultsMFC me);
autoCategories ResultsMFC_to_Categories_responses (ResultsMFC me);
void Categories_sort (Categories me);
double Categories_getEntropy (Categories me);

/* End of file ExperimentMFC.h */
#endif
