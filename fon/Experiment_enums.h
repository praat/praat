/* Experiment_enums.h
 *
 * Copyright (C) 2001-2009,2015,2017 Paul Boersma
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

enums_begin (kExperiment_randomize, 0)
	enums_add (kExperiment_randomize, 0, CYCLIC_NON_RANDOM, U"CyclicNonRandom")
	enums_add (kExperiment_randomize, 1, PERMUTE_ALL, U"PermuteAll")
	enums_add (kExperiment_randomize, 2, PERMUTE_BALANCED, U"PermuteBalanced")
	enums_add (kExperiment_randomize, 3, PERMUTE_BALANCED_NO_DOUBLETS, U"PermuteBalancedNoDoublets")
	enums_add (kExperiment_randomize, 4, WITH_REPLACEMENT, U"WithReplacement")
	/*
		As this enumerated type occurs in data, you should add new randomization methods only at the end.
	*/
enums_end (kExperiment_randomize, 4, PERMUTE_BALANCED_NO_DOUBLETS)

/* End of file Experiment_enums.h */
