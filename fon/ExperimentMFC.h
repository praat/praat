#ifndef _ExperimentMFC_h_
#define _ExperimentMFC_h_
/* ExperimentMFC.h
 *
 * Copyright (C) 2001-2011,2015,2017 Paul Boersma
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

#include "Sound.h"
#include "Categories.h"
#include "Table.h"

#include "Experiment_enums.h"

#include "ExperimentMFC_def.h"

void ExperimentMFC_start (ExperimentMFC me);
void ExperimentMFC_playStimulus (ExperimentMFC me, integer istim);
void ExperimentMFC_playResponse (ExperimentMFC me, integer iresp);

autoResultsMFC ResultsMFC_create (integer numberOfResults);
autoResultsMFC ExperimentMFC_extractResults (ExperimentMFC me);
autoResultsMFC ResultsMFC_removeUnsharedStimuli (ResultsMFC me, ResultsMFC thee);

autoTable ResultsMFCs_to_Table (OrderedOf<structResultsMFC>* me);

autoCategories ResultsMFC_to_Categories_stimuli (ResultsMFC me);
autoCategories ResultsMFC_to_Categories_responses (ResultsMFC me);
void Categories_sort (Categories me);
double Categories_getEntropy (Categories me);

Collection_define (ExperimentMFCList, OrderedOf, ExperimentMFC) {
	DaataList asDaataList () {
		return reinterpret_cast<DaataList> (this);
	}
};

/* End of file ExperimentMFC.h */
#endif
