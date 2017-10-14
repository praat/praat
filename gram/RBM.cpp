/* RBM.cpp
 *
 * Copyright (C) 2016,2017 Paul Boersma
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

//#include <OpenCL/OpenCL.h>
#include "RBM.h"
#include "PAIRWISE_SUM.h"

#include "oo_DESTROY.h"
#include "RBM_def.h"
#include "oo_COPY.h"
#include "RBM_def.h"
#include "oo_EQUAL.h"
#include "RBM_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "RBM_def.h"
#include "oo_WRITE_TEXT.h"
#include "RBM_def.h"
#include "oo_READ_TEXT.h"
#include "RBM_def.h"
#include "oo_WRITE_BINARY.h"
#include "RBM_def.h"
#include "oo_READ_BINARY.h"
#include "RBM_def.h"
#include "oo_DESCRIPTION.h"
#include "RBM_def.h"

void structRBM :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of input nodes: ", our numberOfInputNodes);
	MelderInfo_writeLine (U"Number of output nodes: ", our numberOfOutputNodes);
}

Thing_implement (RBM, Daata, 0);

void RBM_init (RBM me, integer numberOfInputNodes, integer numberOfOutputNodes, bool inputsAreBinary) {
	my numberOfInputNodes = numberOfInputNodes;
	my inputBiases = NUMvector <double> (1, numberOfInputNodes);
	my inputActivities = NUMvector <double> (1, numberOfInputNodes);
	my inputReconstruction = NUMvector <double> (1, numberOfInputNodes);
	my numberOfOutputNodes = numberOfOutputNodes;
	my outputBiases = NUMvector <double> (1, numberOfOutputNodes);
	my outputActivities = NUMvector <double> (1, numberOfOutputNodes);
	my outputReconstruction = NUMvector <double> (1, numberOfOutputNodes);
	my weights = NUMmatrix <double> (1, numberOfInputNodes, 1, numberOfOutputNodes);
	my inputsAreBinary = inputsAreBinary;
}

autoRBM RBM_create (integer numberOfInputNodes, integer numberOfOutputNodes, bool inputsAreBinary) {
	try {
		autoRBM me = Thing_new (RBM);
		RBM_init (me.get(), numberOfInputNodes, numberOfOutputNodes, inputsAreBinary);
		return me;
	} catch (MelderError) {
		Melder_throw (U"RBM not created.");
	}
}

inline static double logistic (double excitation) {
	return 1.0 / (1.0 + exp (- excitation));
}

void RBM_spreadUp (RBM me) {
	integer numberOfOutputNodes = my numberOfOutputNodes;
	for (integer jnode = 1; jnode <= numberOfOutputNodes; jnode ++) {
		PAIRWISE_SUM (real80, excitation, integer, my numberOfInputNodes,
 			double *p_inputActivity = & my inputActivities [0];
 			double *p_weight = & my weights [1] [jnode] - numberOfOutputNodes,
 			( p_inputActivity += 1, p_weight += numberOfOutputNodes ),
 			(real80) *p_inputActivity * (real80) *p_weight
		);
		excitation += my outputBiases [jnode];
		my outputActivities [jnode] = logistic ((real) excitation);
	}
}

void RBM_sampleInput (RBM me) {
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		if (my inputsAreBinary) {
			double probability = my inputActivities [inode];
			my inputActivities [inode] = (double) NUMrandomBernoulli (probability);
		} else {   // Gaussian
			double excitation = my inputActivities [inode];
			my inputActivities [inode] = NUMrandomGauss (excitation, 1.0);
		}
	}
}

void RBM_sampleOutput (RBM me) {
	for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		double probability = my outputActivities [jnode];
		my outputActivities [jnode] = (double) NUMrandomBernoulli (probability);
	}
}

void RBM_spreadDown (RBM me) {
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		real80 excitation = my inputBiases [inode];
		for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
			excitation += my weights [inode] [jnode] * my outputActivities [jnode];
		}
		if (my inputsAreBinary) {
			my inputActivities [inode] = logistic ((real) excitation);
		} else {   // linear
			my inputActivities [inode] = (real) excitation;
		}
	}
}

void RBM_spreadDown_reconstruction (RBM me) {
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		real80 excitation = my inputBiases [inode];
		for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
			excitation += my weights [inode] [jnode] * my outputActivities [jnode];
		}
		if (my inputsAreBinary) {
			my inputReconstruction [inode] = logistic ((real) excitation);
		} else {   // linear
			my inputReconstruction [inode] = (real) excitation;
		}
	}
}

void RBM_spreadUp_reconstruction (RBM me) {
	for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		real80 excitation = my outputBiases [jnode];
		for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
			excitation += my inputReconstruction [inode] * my weights [inode] [jnode];
		}
		my outputReconstruction [jnode] = logistic ((real) excitation);
	}
}

void RBM_update (RBM me, double learningRate) {
	for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		my outputBiases [jnode] += learningRate * (my outputActivities [jnode] - my outputReconstruction [jnode]);
	}
	for (integer inode = 1; inode <= my numberOfInputNodes; inode ++) {
		my inputBiases [inode] += learningRate * (my inputActivities [inode] - my inputReconstruction [inode]);
		for (integer jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
			my weights [inode] [jnode] += learningRate *
				(my inputActivities [inode] * my outputActivities [jnode] -
				 my inputReconstruction [inode] * my outputReconstruction [jnode]);
		}
	}
}

void RBM_PatternList_applyToInput (RBM me, PatternList thee, integer rowNumber) {
	Melder_assert (my numberOfInputNodes == thy nx);
	for (integer ifeature = 1; ifeature <= my numberOfInputNodes; ifeature ++) {
		my inputActivities [ifeature] = thy z [rowNumber] [ifeature];
	}
}

void RBM_PatternList_applyToOutput (RBM me, PatternList thee, integer rowNumber) {
	Melder_assert (my numberOfOutputNodes == thy nx);
	for (integer icat = 1; icat <= my numberOfOutputNodes; icat ++) {
		my outputActivities [icat] = thy z [rowNumber] [icat];
	}
}

void RBM_PatternList_learn (RBM me, PatternList thee, double learningRate) {
	for (integer ipattern = 1; ipattern <= thy ny; ipattern ++) {
		RBM_PatternList_applyToInput (me, thee, ipattern);
		RBM_spreadUp (me);
		RBM_sampleOutput (me);
		RBM_spreadDown_reconstruction (me);
		RBM_spreadUp_reconstruction (me);
		RBM_update (me, learningRate);
	}
}

autoMatrix RBM_extractInputActivities (RBM me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfInputNodes);
		NUMvector_copyElements <double> (my inputActivities, thy z [1], 1, my numberOfInputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input activities not extracted.");
	}
}

autoMatrix RBM_extractOutputActivities (RBM me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfOutputNodes);
		NUMvector_copyElements <double> (my outputActivities, thy z [1], 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output activities not extracted.");
	}
}

autoMatrix RBM_extractInputReconstruction (RBM me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfInputNodes);
		NUMvector_copyElements <double> (my inputReconstruction, thy z [1], 1, my numberOfInputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input reconstruction not extracted.");
	}
}

autoMatrix RBM_extractOutputReconstruction (RBM me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfOutputNodes);
		NUMvector_copyElements <double> (my outputReconstruction, thy z [1], 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": output reconstruction not extracted.");
	}
}

autoMatrix RBM_extractInputBiases (RBM me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfInputNodes);
		NUMvector_copyElements <double> (my inputBiases, thy z [1], 1, my numberOfInputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input biases not extracted.");
	}
}

autoMatrix RBM_extractOutputBiases (RBM me) {
	try {
		autoMatrix thee = Matrix_createSimple (1, my numberOfOutputNodes);
		NUMvector_copyElements <double> (my outputBiases, thy z [1], 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": input biases not extracted.");
	}
}

autoMatrix RBM_extractWeights (RBM me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfInputNodes, my numberOfOutputNodes);
		NUMmatrix_copyElements <double> (my weights, thy z, 1, my numberOfInputNodes, 1, my numberOfOutputNodes);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": weights not extracted.");
	}
}

/* End of file RBM.cpp */
