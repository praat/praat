/* NUMlinprog.cpp
 *
 * Copyright (C) 2008-2011,2012 Paul Boersma
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
 * pb 2008/04/12 created
 * pb 2011/03/29 C++
 */

#include "NUM.h"
#include "../external/glpk/glpk.h"

#define my  me ->

struct structNUMlinprog {
	glp_prob *linearProgram;
	long numberOfConstraints, ivar, numberOfVariables;
	int *ind;
	double *val;
	int status;
};

void NUMlinprog_delete (NUMlinprog me) {
	if (me == NULL) return;
	if (my linearProgram != NULL) glp_delete_prob (my linearProgram);
	NUMvector_free <int> (my ind, 1);
	NUMvector_free <double> (my val, 1);
	Melder_free (me);
}

NUMlinprog NUMlinprog_new (bool maximize) {
	NUMlinprog me = NULL;
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
	glp_set_col_bnds (my linearProgram, ++ my numberOfVariables,
		lowerBound == NUMundefined ? ( upperBound == NUMundefined ? GLP_FR : GLP_UP ) :
		upperBound == NUMundefined ? GLP_LO :
		lowerBound == upperBound ? GLP_FX : GLP_DB, lowerBound, upperBound);
	glp_set_obj_coef (my linearProgram, my ivar, coeff);
}

void NUMlinprog_addConstraint (NUMlinprog me, double lowerBound, double upperBound) {
	try {
		if (my ind == NULL) {
			/*
			 * Check without change.
			 */
			autoNUMvector <int> ind (1, my numberOfVariables);
			autoNUMvector <double> val (1, my numberOfVariables);
			/*
			 * Change without error.
			 */
			my ind = ind.transfer();
			my val = val.transfer();
		}
		glp_add_rows (my linearProgram, 1);   // TODO: check
		glp_set_row_bnds (my linearProgram, ++ my numberOfConstraints,
			lowerBound == NUMundefined ? ( upperBound == NUMundefined ? GLP_FR : GLP_UP ) :
			upperBound == NUMundefined ? GLP_LO :
			lowerBound == upperBound ? GLP_FX : GLP_DB, lowerBound, upperBound);
		my ivar = 0;
	} catch (MelderError) {
		Melder_throw ("Linear programming: constraint not added.");
	}
}

void NUMlinprog_addConstraintCoefficient (NUMlinprog me, double coefficient) {
	++ my ivar;
	my ind [my ivar] = my ivar;
	my val [my ivar] = coefficient;
	if (my ivar == my numberOfVariables) {
		glp_set_mat_row (my linearProgram, my numberOfConstraints, my numberOfVariables, my ind, my val);
	}
}

void NUMlinprog_run (NUMlinprog me) {
	try {
		glp_smcp parm;
		glp_init_smcp (& parm);
		parm. msg_lev = GLP_MSG_OFF;
		my status = glp_simplex (my linearProgram, & parm);
		switch (my status) {
			case GLP_EBADB: Melder_throw ("Unable to start the search, because the initial basis is invalid.");
			case GLP_ESING: Melder_throw (L"Unable to start the search, because the basis matrix is singular.");
			case GLP_ECOND: Melder_throw (L"Unable to start the search, because the basis matrix is ill-conditioned.");
			case GLP_EBOUND: Melder_throw (L"Unable to start the search, because some variables have incorrect bounds.");
			case GLP_EFAIL: Melder_throw (L"Search prematurely terminated due to solver failure.");
			case GLP_EOBJLL: Melder_throw (L"Search prematurely terminated: lower limit reached.");
			case GLP_EOBJUL: Melder_throw (L"Search prematurely terminated: upper limit reached.");
			case GLP_EITLIM: Melder_throw (L"Search prematurely terminated: iteration limit exceeded.");
			case GLP_ETMLIM: Melder_throw (L"Search prematurely terminated: time limit exceeded.");
			case GLP_ENOPFS: Melder_throw (L"The problem has no primal feasible solution.");
			case GLP_ENODFS: Melder_throw (L"The problem has no dual feasible solution.");
			default: break;
		}
		my status = glp_get_status (my linearProgram);
		switch (my status) {
			case GLP_INFEAS: Melder_throw (L"Solution is infeasible.");
			case GLP_NOFEAS: Melder_throw (L"Problem has no feasible solution.");
			case GLP_UNBND: Melder_throw (L"Problem has unbounded solution.");
			case GLP_UNDEF: Melder_throw (L"Solution is undefined.");
			default: break;
		}
		if (my status == GLP_FEAS) {
			Melder_warning (L"Linear programming solution is feasible but not optimal.");
		}
	} catch (MelderError) {
		Melder_throw ("Linear programming: not run.");
	}
}

double NUMlinprog_getPrimalValue (NUMlinprog me, long ivar) {
	return glp_get_col_prim (my linearProgram, ivar);
}

/* End of file NUMlinprog.cpp */
