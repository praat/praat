/* RBM.cpp
 *
 * Copyright (C) 2016 Paul Boersma
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

#include "RBM.h"

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

void structRBM :: v_info ()
{
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of input nodes: ", our numberOfInputNodes);
	MelderInfo_writeLine (U"Number of output nodes: ", our numberOfOutputNodes);
}

Thing_implement (RBM, Daata, 0);

void RBM_init (RBM me, long numberOfInputNodes, long numberOfOutputNodes, bool inputsAreBinary) {
	my numberOfInputNodes = numberOfInputNodes;
	my inputBiases = NUMvector <double> (1, numberOfInputNodes);
	my inputActivities = NUMvector<double> (1, numberOfInputNodes);
	my inputReconstruction = NUMvector<double> (1, numberOfInputNodes);
	my numberOfOutputNodes = numberOfOutputNodes;
	my outputBiases = NUMvector <double> (1, numberOfOutputNodes);
	my outputActivities = NUMvector<double> (1, numberOfOutputNodes);
	my outputReconstruction = NUMvector<double> (1, numberOfOutputNodes);
	my weights = NUMmatrix<double> (1, numberOfInputNodes, 1, numberOfOutputNodes);
	my inputsAreBinary = inputsAreBinary;
}

autoRBM RBM_create (long numberOfInputNodes, long numberOfOutputNodes, bool inputsAreBinary) {
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

inline static double sample (double probability) {
	return probability > NUMrandomUniform (0.0, 1.0) ? 1.0 : 0.0;
}

static void RBM_spreadUp (RBM me) {
	for (long jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		double excitation = my outputBiases [jnode];
		for (long inode = 1; inode <= my numberOfInputNodes; inode ++) {
			excitation += my inputActivities [inode] * my weights [inode] [jnode];
		}
		my outputActivities [jnode] = sample (logistic (excitation));
	}
}

static void RBM_spreadDown (RBM me) {
	for (long inode = 1; inode <= my numberOfInputNodes; inode ++) {
		double excitation = my inputBiases [inode];
		for (long jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
			excitation += my weights [inode] [jnode] * my outputActivities [jnode];
		}
		if (my inputsAreBinary) {
			my inputReconstruction [inode] = logistic (excitation);
		} else {
			my inputReconstruction [inode] = logistic (excitation) + NUMrandomGauss (0.0, 1.0);
		}
	}
}

static void RBM_spreadUpOnceAgain (RBM me) {
	for (long jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
		double excitation = my outputBiases [jnode];
		for (long inode = 1; inode <= my numberOfInputNodes; inode ++) {
			excitation += my inputReconstruction [inode] * my weights [inode] [jnode];
		}
		my outputReconstruction [jnode] = logistic (excitation);   // without sampling
	}
}

static void RBM_update (RBM me, double learningRate) {
	for (long inode = 1; inode <= my numberOfInputNodes; inode ++) {
		for (long jnode = 1; jnode <= my numberOfOutputNodes; jnode ++) {
			my weights [inode] [jnode] += learningRate *
				(my inputActivities [inode] * my outputActivities [jnode] -
				 my inputReconstruction [inode] * my outputReconstruction [jnode]);
		}
	}
}

void RBM_learn (RBM me, Pattern thee, double learningRate) {
	for (long ipattern = 1; ipattern <= thy ny; ipattern ++) {
		RBM_spreadUp (me);
		RBM_spreadDown (me);
		RBM_spreadUpOnceAgain (me);
		RBM_update (me, learningRate);
	}
}

/* End of file RBM.cpp */
