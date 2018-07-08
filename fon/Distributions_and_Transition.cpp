/* Distributions_and_Transition.cpp
 *
 * Copyright (C) 1997-2011,2015,2016,2017 Paul Boersma
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

#include "Distributions_and_Transition.h"

autoTransition Distributions_to_Transition (Distributions underlying, Distributions surface, integer environment,
	Transition adjacency, bool greedy)
{
	try {
		if (! underlying) return autoTransition();

		/*
		 * Preconditions: range check and matrix matching.
		 */
		if (environment < 1 || environment > underlying -> numberOfColumns)
			Melder_throw (U"Environment (", environment, U") out of range (1-", underlying -> numberOfColumns, U").");
		if (surface && (underlying -> numberOfColumns != surface -> numberOfColumns || underlying -> numberOfRows != surface -> numberOfRows))
			Melder_throw (U"Sizes of underlying and surface distributions do not match.");
		if (adjacency && adjacency -> numberOfStates != underlying -> numberOfColumns)
			Melder_throw (U"Number of states (", adjacency -> numberOfStates, U") in adjacency matrix "
				U"does not match number of distributions (", underlying -> numberOfColumns, U")");

		/*
		 * Defaults.
		 */
		if (! surface) surface = underlying;

		/*
		 * Create the output object.
		 */
		autoTransition thee = Transition_create (underlying -> numberOfColumns);

		/*
		 * Copy labels and set name.
		 */
		for (integer i = 1; i <= thy numberOfStates; i ++) {
			thy stateLabels [i] = Melder_dup (underlying -> columnLabels [i].get());
		}
		Thing_setName (thee.get(), underlying -> columnLabels [environment].get());

		/*
		 * Compute the off-diagonal elements of the transition matrix in environment 'environment'.
		 */
		for (integer i = 1; i <= thy numberOfStates; i ++) {

			/*
			 * How many states are available for the learner to step to (excluding current state)?
			 */
			integer numberOfAdjacentStates;
			if (adjacency) {
				numberOfAdjacentStates = 0;
				for (integer j = 1; j <= thy numberOfStates; j ++)
					if (i != j && adjacency -> data [i] [j] != 0.0)
						numberOfAdjacentStates ++;
			} else {
				numberOfAdjacentStates = thy numberOfStates - 1;
			}

			/*
			 * Try all possible steps to adjacent states.
			 */
			for (integer j = 1; j <= thy numberOfStates; j ++) if (i != j) {

				/*
				 * Local: grammar step only possible to adjacent grammar.
				 */
				if (adjacency && adjacency -> data [i] [j] == 0) continue;

				/*
				 * Compute element (i, j): sum over all possible data.
				 */
				for (integer m = 1; m <= underlying -> numberOfRows; m ++) {

					/*
					 * Error-driven: grammar step only triggered by positive evidence.
					 * If the datum does not conflict with the current hypothesis (i), ignore it.
					 */
					if (underlying -> data [m] [i] != 0.0) continue;

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
		for (integer i = 1; i <= thy numberOfStates; i ++) {
			longdouble sum = 0.0;
			for (integer j = 1; j <= thy numberOfStates; j ++) if (j != i)
				sum += thy data [i] [j];
			thy data [i] [i] = sum > 1.0 ? 0.0 : 1.0 - (double) sum;   // guard against rounding errors
		}

		return thee;
	} catch (MelderError) {
		Melder_throw (underlying, U": Transition not computed.");
	}
}

autoDistributions Distributions_Transition_map (Distributions me, Transition map) {
	try {
		/*
		 * Preconditions: matrix matching.
		 */
		if (map -> numberOfStates != my numberOfRows)
			Melder_throw (U"Number of data (", map -> numberOfStates, U") in mapping matrix "
				U"does not match number of data (", my numberOfRows, U") in distribution.");

		/*
		 * Create the output object.
		 */
		autoDistributions thee = Data_copy (me);

		/*
		 * Compute the elements of the surface distributions.
		 */
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			for (integer icol = 1; icol <= my numberOfColumns; icol ++) {
				thy data [irow] [icol] = 0.0;
				for (integer istate = 1; istate <= map -> numberOfStates; istate ++) {
					thy data [irow] [icol] += my data [istate] [icol] * map -> data [istate] [irow];
				}
			}
		}

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not mapped to Transition.");
	}
}

autoDistributions Transition_to_Distributions_conflate (Transition me) {
	try {
		autoDistributions thee = Distributions_create (my numberOfStates, 1);

		/*
		 * Copy labels.
		 */
		for (integer i = 1; i <= my numberOfStates; i ++) {
			thy rowLabels [i] = Melder_dup (my stateLabels [i].get());
		}

		/*
		 * Average rows.
		 */
		for (integer i = 1; i <= my numberOfStates; i ++) {
			for (integer j = 1; j <= my numberOfStates; j ++)
				thy data [i] [1] += my data [j] [i];
			thy data [i] [1] /= my numberOfStates;
		}

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not conflated to Distributions.");
	}
}

/* End of file Distributions_and_Transition.cpp */
