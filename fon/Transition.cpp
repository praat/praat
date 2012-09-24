/* Transition.cpp
 *
 * Copyright (C) 1997-2012 Paul Boersma
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

#include "Transition.h"
#include "NUM2.h"
#include "Eigen.h"

#include "oo_DESTROY.h"
#include "Transition_def.h"
#include "oo_COPY.h"
#include "Transition_def.h"
#include "oo_EQUAL.h"
#include "Transition_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Transition_def.h"
#include "oo_WRITE_BINARY.h"
#include "Transition_def.h"
#include "oo_READ_TEXT.h"
#include "Transition_def.h"
#include "oo_READ_BINARY.h"
#include "Transition_def.h"
#include "oo_DESCRIPTION.h"
#include "Transition_def.h"

Thing_implement (Transition, Data, 0);

void structTransition :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Number of states: ", Melder_integer (numberOfStates));
}

void structTransition :: v_writeText (MelderFile file) {
	texputi4 (file, numberOfStates, L"numberOfStates", 0,0,0,0,0);
	MelderFile_write (file, L"\nstateLabels []: ");
	if (numberOfStates < 1) MelderFile_write (file, L"(empty)");
	MelderFile_write (file, L"\n");
	for (long i = 1; i <= numberOfStates; i ++) {
		MelderFile_write (file, L"\"");
		if (stateLabels [i] != NULL) MelderFile_write (file, stateLabels [i]);
		MelderFile_write (file, L"\"\t");
	}
	for (long i = 1; i <= numberOfStates; i ++) {
		MelderFile_write (file, L"\nstate [", Melder_integer (i), L"]:");
		for (long j = 1; j <= numberOfStates; j ++) {
			MelderFile_write (file, L"\t", Melder_double (data [i] [j]));
		}
	}
}

void Transition_init (Transition me, long numberOfStates) {
	if (numberOfStates < 1)
		Melder_throw ("Cannot create empty matrix.");
	my numberOfStates = numberOfStates;
	my stateLabels = NUMvector <wchar_t *> (1, numberOfStates);
	my data = NUMmatrix <double> (1, my numberOfStates, 1, my numberOfStates);
}

Transition Transition_create (long numberOfStates) {
	try {
		autoTransition me = Thing_new (Transition);
		Transition_init (me.peek(), numberOfStates);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Transition not created.");
	}
}

static void NUMrationalize (double x, long *numerator, long *denominator) {
	double epsilon = 1e-6;
	*numerator = 1;
	for (*denominator = 1; *denominator <= 100000; (*denominator) ++) {
		double numerator_d = x * *denominator, rounded = floor (numerator_d + 0.5);
		if (fabs (rounded - numerator_d) < epsilon) {
			*numerator = rounded;
			return;
		}
	}
	*denominator = 0;   // failure
}

static void print4 (wchar_t *buffer, double value, int iformat, int width, int precision) {
	wchar_t formatString [40];
	if (iformat == 4) {
		long numerator, denominator;
		NUMrationalize (value, & numerator, & denominator);
		if (numerator == 0)
			swprintf (buffer, 40, L"0");
		else if (denominator > 1)
			swprintf (buffer, 40, L"%ld/%ld", numerator, denominator);
		else
			swprintf (buffer, 40, L"%.7g", value);
	} else {
		swprintf (formatString, 40, L"%%%d.%d%c", width, precision, iformat == 1 ? 'f' : iformat == 2 ? 'e' : 'g');
		swprintf (buffer, 40, formatString, value);
	}
}

void Transition_drawAsNumbers (Transition me, Graphics g, int iformat, int precision) {
	double maxTextWidth = 0, maxTextHeight = 0;
	Graphics_setInner (g);
	Graphics_setWindow (g, 0.5, my numberOfStates + 0.5, 0, 1);
	double leftMargin = Graphics_dxMMtoWC (g, 1);
	double lineSpacing = Graphics_dyMMtoWC (g, 1.5 * Graphics_inqFontSize (g) * 25.4 / 72);
	Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_BOTTOM);
	for (long col = 1; col <= my numberOfStates; col ++) {
		if (my stateLabels && my stateLabels [col] && my stateLabels [col] [0]) {
			Graphics_text (g, col, 1, my stateLabels [col]);
			if (! maxTextHeight) maxTextHeight = lineSpacing;
		}
	}
	for (long row = 1; row <= my numberOfStates; row ++) {
		double y = 1 - lineSpacing * (row - 1 + 0.7);
		Graphics_setTextAlignment (g, Graphics_RIGHT, Graphics_HALF);
		if (my stateLabels && my stateLabels [row]) {
			double textWidth = Graphics_textWidth (g, my stateLabels [row]);
			if (textWidth > maxTextWidth) maxTextWidth = textWidth;
			Graphics_text (g, 0.5 - leftMargin, y, my stateLabels [row]);
		}
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		for (long col = 1; col <= my numberOfStates; col ++) {
			wchar_t text [40];
			print4 (text, my data [row] [col], iformat, 0, precision);
			Graphics_text (g, col, y, text);
		}
	}
	if (maxTextWidth)
		Graphics_line (g, 0.5 - maxTextWidth - leftMargin, 1, my numberOfStates + 0.5, 1);
	if (maxTextHeight)
		Graphics_line (g, 0.5, 1 + maxTextHeight, 0.5, 1 - lineSpacing * (my numberOfStates + 0.2));
	Graphics_unsetInner (g);
}

static void Transition_transpose (Transition me) {
	for (long i = 1; i < my numberOfStates; i ++) {
		for (long j = i + 1; j <= my numberOfStates; j ++) {
			double temp = my data [i] [j];
			my data [i] [j] = my data [j] [i];
			my data [j] [i] = temp;
		}
	}
}

void Transition_eigen (Transition me, Matrix *out_eigenvectors, Matrix *out_eigenvalues) {
	*out_eigenvectors = NULL;
	*out_eigenvalues = NULL;
	bool transposed = false;
	try {
		autoEigen eigen = Thing_new (Eigen);
		Transition_transpose (me);
		Eigen_initFromSymmetricMatrix (eigen.peek(), my data, my numberOfStates);
		Transition_transpose (me);
		transposed = true;
		autoMatrix eigenvectors = Matrix_createSimple (my numberOfStates, my numberOfStates);
		autoMatrix eigenvalues = Matrix_createSimple (my numberOfStates, 1);
		for (long i = 1; i <= my numberOfStates; i ++) {
			eigenvalues -> z [i] [1] = eigen -> eigenvalues [i];
			for (long j = 1; j <= my numberOfStates; j ++)
				eigenvectors -> z [i] [j] = eigen -> eigenvectors [j] [i];
		}
		*out_eigenvectors = eigenvectors.transfer();
		*out_eigenvalues = eigenvalues.transfer();
	} catch (MelderError) {
		if (transposed)
			Transition_transpose (me);
		Melder_throw (me, ": eigenvectors not computed.");
	}
}

Transition Transition_power (Transition me, long power) {
	try {
		autoTransition thee = Data_copy (me);
		autoTransition him = Data_copy (me);
		for (long ipow = 2; ipow <= power; ipow ++) {
			double **tmp = his data; his data = thy data; thy data = tmp;   // OPTIMIZE
			for (long irow = 1; irow <= my numberOfStates; irow ++) {
				for (long icol = 1; icol <= my numberOfStates; icol ++) {
					thy data [irow] [icol] = 0.0;
					for (long i = 1; i <= my numberOfStates; i ++) {
						thy data [irow] [icol] += his data [irow] [i] * my data [i] [icol];
					}
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": power not computed.");
	}
}

Matrix Transition_to_Matrix (Transition me) {
	try {
		autoMatrix thee = Matrix_createSimple (my numberOfStates, my numberOfStates);
		for (long i = 1; i <= my numberOfStates; i ++)
			for (long j = 1; j <= my numberOfStates; j ++)
				thy z [i] [j] = my data [i] [j];
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

Transition Matrix_to_Transition (Matrix me) {
	try {
		if (my nx != my ny)
			Melder_throw ("Matrix should be square.");
		autoTransition thee = Transition_create (my nx);
		for (long i = 1; i <= my nx; i ++)
			for (long j = 1; j <= my nx; j ++)
				thy data [i] [j] = my z [i] [j];
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Transition.");
	}
}

/* End of file Transition.cpp */
