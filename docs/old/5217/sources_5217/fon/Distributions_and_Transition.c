/* Distributions_and_Transition.c
 *
 * Copyright (C) 1997-2007 Paul Boersma
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
 * pb 1997/03/02 created
 * pb 2002/07/16 GPL
 * pb 2007/08/12 wchar_t
 */

#include "Distributions_and_Transition.h"

Transition Distributions_to_Transition (Distributions underlying, Distributions surface, long environment,
	Transition adjacency, int greedy)
{
	if (underlying == NULL) return NULL;
	Transition thee = NULL;

	/*
	 * Preconditions: range check and matrix matching.
	 */
	if (environment < 1 || environment > underlying -> numberOfColumns)
		error5 (L"Environment (", Melder_integer (environment), L") out of range (1-", Melder_integer (underlying -> numberOfColumns), L").")
	if (surface && (underlying -> numberOfColumns != surface -> numberOfColumns || underlying -> numberOfRows != surface -> numberOfRows))
		error1 (L"Sizes of underlying and surface distributions do not match.")
	if (adjacency && adjacency -> numberOfStates != underlying -> numberOfColumns)
		error5 (L"Number of states (", Melder_integer (adjacency -> numberOfStates), L") in adjacency matrix "
			"does not match number of distributions (", Melder_integer (underlying -> numberOfColumns), L")")

	/*
	 * Defaults.
	 */
	if (surface == NULL) surface = underlying;

	/*
	 * Create the output object.
	 */
	thee = Transition_create (underlying -> numberOfColumns); cherror

	/*
	 * Copy labels and set name.
	 */
	for (long i = 1; i <= thy numberOfStates; i ++) {
		thy stateLabels [i] = Melder_wcsdup_e (underlying -> columnLabels [i]); cherror
	}
	Thing_setName (thee, underlying -> columnLabels [environment]); cherror

	/*
	 * Compute the off-diagonal elements of the transition matrix in environment 'environment'.
	 */
	for (long i = 1; i <= thy numberOfStates; i ++) {

		/*
		 * How many states are available for the learner to step to (excluding current state)?
		 */
		long numberOfAdjacentStates;
		if (adjacency) {
			numberOfAdjacentStates = 0;
			for (long j = 1; j <= thy numberOfStates; j ++)
				if (i != j && adjacency -> data [i] [j])
					numberOfAdjacentStates ++;
		} else {
			numberOfAdjacentStates = thy numberOfStates - 1;
		}

		/*
		 * Try all possible steps to adjacent states.
		 */
		for (long j = 1; j <= thy numberOfStates; j ++) if (i != j) {

			/*
			 * Local: grammar step only possible to adjacent grammar.
			 */
			if (adjacency && adjacency -> data [i] [j] == 0) continue;

			/*
			 * Compute element (i, j): sum over all possible data.
			 */
			for (long m = 1; m <= underlying -> numberOfRows; m ++) {

				/*
				 * Error-driven: grammar step only triggered by positive evidence.
				 * If the datum does not conflict with the current hypothesis (i), ignore it.
				 */
				if (underlying -> data [m] [i]) continue;

				/*
				 * Greedy: grammar step only taken if new grammar accepts datum.
				 */
				if (greedy && underlying -> data [m] [j] == 0) continue;

				/*
				 * The step is taken if this datum occurs and this grammar (j) is chosen.
				 */
				thy data [i] [j] += surface -> data [m] [environment] / numberOfAdjacentStates;
			}
		}
	}

	/*
	 * Compute the elements on the diagonal, so that the sum of each row is unity.
	 */
	for (long i = 1; i <= thy numberOfStates; i ++) {
		double sum = 0.0;
		for (long j = 1; j <= thy numberOfStates; j ++) if (j != i)
			sum += thy data [i] [j];
		thy data [i] [i] = sum > 1.0 ? 0.0 : 1.0 - sum;   /* Guard against rounding errors. */
	}

end:
	iferror {
		forget (thee);
		return Melder_errorp1 (L"Distributions to Transition: not performed.");
	}
	return thee;
}

Distributions Distributions_Transition_map (Distributions me, Transition map) {
	Distributions thee = NULL;

	/*
	 * Preconditions: matrix matching.
	 */
	if (map -> numberOfStates != my numberOfRows)
		error5 (L"Number of data (", Melder_integer (map -> numberOfStates), L") in mapping matrix "
			"does not match number of data (", Melder_integer (my numberOfRows), L") in distribution.")

	/*
	 * Create the output object.
	 */
	thee = Data_copy (me); cherror

	/*
	 * Compute the elements of the surface distributions.
	 */
	for (long row = 1; row <= my numberOfRows; row ++) for (long col = 1; col <= my numberOfColumns; col ++) {
		thy data [row] [col] = 0.0;
		for (long m = 1; m <= map -> numberOfStates; m ++)
			thy data [row] [col] += my data [m] [col] * map -> data [m] [row];
	}

end:
	iferror {
		forget (thee);
		return Melder_errorp1 (L"Distributions & Transition: Mapping not performed.");
	}
	return thee;
}

Distributions Transition_to_Distributions_conflate (Transition me) {
	Distributions thee = NULL;
//start:
	thee = Distributions_create (my numberOfStates, 1); cherror

	/*
	 * Copy labels.
	 */
	for (long i = 1; i <= my numberOfStates; i ++) {
		thy rowLabels [i] = Melder_wcsdup_e (my stateLabels [i]); cherror
	}

	/*
	 * Average rows.
	 */
	for (long i = 1; i <= my numberOfStates; i ++) {
		for (long j = 1; j <= my numberOfStates; j ++)
			thy data [i] [1] += my data [j] [i];
		thy data [i] [1] /= my numberOfStates;
	}

end:
	iferror {
		forget (thee);
		return Melder_errorp ("(Transition_to_Distributions_conflate:) Not performed.");
	}
	return thee;
}

/* End of file Distributions_and_Transition.c */
