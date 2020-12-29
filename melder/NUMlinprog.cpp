/* NUMlinprog.cpp
 *
 * Copyright (C) 2008,2011,2012,2015-2020 Paul Boersma
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

#include "melder.h"
#include "../external/glpk/glpk.h"

struct structNUMlinprog {
	glp_prob *linearProgram;
	integer numberOfConstraints, ivar, numberOfVariables;
	autovector<int> ind;
	autoVEC val;
	int status;
};

void NUMlinprog_delete (NUMlinprog me) {
	if (! me)
		return;
	if (my linearProgram)
		glp_delete_prob (my linearProgram);
	my ind.reset();
	my val.reset();
	Melder_free (me);
}

NUMlinprog NUMlinprog_new (bool maximize) {
	NUMlinprog me = nullptr;
	try {
		me = Melder_calloc (structNUMlinprog, 1);
		my linearProgram = glp_create_prob ();   // TODO: check
		glp_set_obj_dir (my linearProgram, maximize ? GLP_MAX : GLP_MIN);
	} catch (MelderError) {
		if (me) NUMlinprog_delete (me);
		return NULL;
	}
	return me;
}

void NUMlinprog_addVariable (NUMlinprog me, double lowerBound, double upperBound, double coeff) {
	glp_add_cols (my linearProgram, 1);
	glp_set_col_bnds (my linearProgram, (int) ++ my numberOfVariables,
		isundef (lowerBound) ? ( isundef (upperBound) ? GLP_FR : GLP_UP ) :
		isundef (upperBound) ? GLP_LO :
		lowerBound == upperBound ? GLP_FX : GLP_DB,
		lowerBound, upperBound);
	glp_set_obj_coef (my linearProgram, (int) my ivar, coeff);
}

void NUMlinprog_addConstraint (NUMlinprog me, double lowerBound, double upperBound) {
	try {
		if (NUMisEmpty (my ind)) {
			my ind = newvectorzero<int> (my numberOfVariables);
			my val = zero_VEC (my numberOfVariables);
		}
		glp_add_rows (my linearProgram, 1);   // TODO: check
		glp_set_row_bnds (my linearProgram, (int) ++ my numberOfConstraints,
			isundef (lowerBound) ? ( isundef (upperBound) ? GLP_FR : GLP_UP ) :
			isundef (upperBound) ? GLP_LO :
			lowerBound == upperBound ? GLP_FX : GLP_DB, lowerBound, upperBound);
		my ivar = 0;
	} catch (MelderError) {
		Melder_throw (U"Linear programming: constraint not added.");
	}
}

void NUMlinprog_addConstraintCoefficient (NUMlinprog me, double coefficient) {
	++ my ivar;
	my ind [my ivar] = (int) my ivar;
	my val [my ivar] = coefficient;
	if (my ivar == my numberOfVariables)
		glp_set_mat_row (my linearProgram, (int) my numberOfConstraints, (int) my numberOfVariables,
				my ind.asArgumentToFunctionThatExpectsOneBasedArray(),
				my val.asArgumentToFunctionThatExpectsOneBasedArray());
}

void NUMlinprog_run (NUMlinprog me) {
	try {
		glp_smcp parm;
		glp_init_smcp (& parm);
		parm. msg_lev = GLP_MSG_OFF;
		my status = glp_simplex (my linearProgram, & parm);
		switch (my status) {
			case GLP_EBADB: Melder_throw (U"Unable to start the search, because the initial basis is invalid.");
			case GLP_ESING: Melder_throw (U"Unable to start the search, because the basis matrix is singular.");
			case GLP_ECOND: Melder_throw (U"Unable to start the search, because the basis matrix is ill-conditioned.");
			case GLP_EBOUND: Melder_throw (U"Unable to start the search, because some variables have incorrect bounds.");
			case GLP_EFAIL: Melder_throw (U"Search prematurely terminated due to solver failure.");
			case GLP_EOBJLL: Melder_throw (U"Search prematurely terminated: lower limit reached.");
			case GLP_EOBJUL: Melder_throw (U"Search prematurely terminated: upper limit reached.");
			case GLP_EITLIM: Melder_throw (U"Search prematurely terminated: iteration limit exceeded.");
			case GLP_ETMLIM: Melder_throw (U"Search prematurely terminated: time limit exceeded.");
			case GLP_ENOPFS: Melder_throw (U"The problem has no primal feasible solution.");
			case GLP_ENODFS: Melder_throw (U"The problem has no dual feasible solution.");
			default: break;
		}
		my status = glp_get_status (my linearProgram);
		switch (my status) {
			case GLP_INFEAS: Melder_throw (U"Solution is infeasible.");
			case GLP_NOFEAS: Melder_throw (U"Problem has no feasible solution.");
			case GLP_UNBND: Melder_throw (U"Problem has unbounded solution.");
			case GLP_UNDEF: Melder_throw (U"Solution is undefined.");
			default: break;
		}
		if (my status == GLP_FEAS)
			Melder_warning (U"Linear programming solution is feasible but not optimal.");
	} catch (MelderError) {
		Melder_throw (U"Linear programming: not run.");
	}
}

double NUMlinprog_getPrimalValue (NUMlinprog me, integer ivar) {
	return glp_get_col_prim (my linearProgram, (int) ivar);
}

/* End of file NUMlinprog.cpp */
