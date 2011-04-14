/* NUMlinprog.cpp
 *
 * Copyright (C) 2008-2011 Paul Boersma
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
#include "glpk/glpk.h"

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
//start:
	me = Melder_calloc_e (struct structNUMlinprog, 1); cherror
	my linearProgram = glp_create_prob ();
	glp_set_obj_dir (my linearProgram, maximize ? GLP_MAX : GLP_MIN);
end:
	iferror {
		NUMlinprog_delete (me);
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

int NUMlinprog_addConstraint (NUMlinprog me, double lowerBound, double upperBound) {
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
		glp_add_rows (my linearProgram, 1);
		glp_set_row_bnds (my linearProgram, ++ my numberOfConstraints,
			lowerBound == NUMundefined ? ( upperBound == NUMundefined ? GLP_FR : GLP_UP ) :
			upperBound == NUMundefined ? GLP_LO :
			lowerBound == upperBound ? GLP_FX : GLP_DB, lowerBound, upperBound);
		my ivar = 0;
		return 1;
	} catch (MelderError) {
		rethrowmzero ("Linear programming: constraint not added.");
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

int NUMlinprog_run (NUMlinprog me) {
	glp_smcp parm;
	glp_init_smcp (& parm);
	parm. msg_lev = GLP_MSG_OFF;
	my status = glp_simplex (my linearProgram, & parm);
	switch (my status) {
		case GLP_EBADB: error1 (L"Unable to start the search, because the initial basis is invalid.");
		case GLP_ESING: error1 (L"Unable to start the search, because the basis matrix is singular.");
		case GLP_ECOND: error1 (L"Unable to start the search, because the basis matrix is ill-conditioned.");
		case GLP_EBOUND: error1 (L"Unable to start the search, because some variables have incorrect bounds.");
		case GLP_EFAIL: error1 (L"Search prematurely terminated due to solver failure.");
		case GLP_EOBJLL: error1 (L"Search prematurely terminated: lower limit reached.");
		case GLP_EOBJUL: error1 (L"Search prematurely terminated: upper limit reached.");
		case GLP_EITLIM: error1 (L"Search prematurely terminated: iteration limit exceeded.");
		case GLP_ETMLIM: error1 (L"Search prematurely terminated: time limit exceeded.");
		case GLP_ENOPFS: error1 (L"The problem has no primal feasible solution.");
		case GLP_ENODFS: error1 (L"The problem has no dual feasible solution.");
		default: break;
	}
	my status = glp_get_status (my linearProgram);
	switch (my status) {
		case GLP_INFEAS: error1 (L"Solution is infeasible.");
		case GLP_NOFEAS: error1 (L"Problem has no feasible solution.");
		case GLP_UNBND: error1 (L"Problem has unbounded solution.");
		case GLP_UNDEF: error1 (L"Solution is undefined.");
		default: break;
	}
	if (my status == GLP_FEAS) {
		Melder_warning1 (L"Linear programming solution is feasible but not optimal.");
	}
end:
	iferror return 0;
	return 1;
}

double NUMlinprog_getPrimalValue (NUMlinprog me, long ivar) {
	return glp_get_col_prim (my linearProgram, ivar);
}

/* End of file NUMlinprog.cpp */
