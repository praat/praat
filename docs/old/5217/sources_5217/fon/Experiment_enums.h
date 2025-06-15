/* Experiment_enums.h
 *
 * Copyright (C) 2001-2009 Paul Boersma
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
 * pb 2009/03/18
 */

enums_begin (kExperiment_randomize, 0)
	enums_add (kExperiment_randomize, 0, CYCLIC_NON_RANDOM, L"CyclicNonRandom")
	enums_add (kExperiment_randomize, 1, PERMUTE_ALL, L"PermuteAll")
	enums_add (kExperiment_randomize, 2, PERMUTE_BALANCED, L"PermuteBalanced")
	enums_add (kExperiment_randomize, 3, PERMUTE_BALANCED_NO_DOUBLETS, L"PermuteBalancedNoDoublets")
	enums_add (kExperiment_randomize, 4, WITH_REPLACEMENT, L"WithReplacement")
enums_end (kExperiment_randomize, 4, PERMUTE_BALANCED_NO_DOUBLETS)

/* End of file Experiment_enums.h */
