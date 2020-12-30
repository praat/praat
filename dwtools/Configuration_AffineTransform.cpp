/* Configuration_AffineTransform.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20020315 GPL header
 */

#include "Configuration_AffineTransform.h"
#include "Configuration_and_Procrustes.h"
#include "Procrustes.h"
#include "SVD.h"

static void do_steps45 (constMATVU const& w, MATVU const& t, constMATVU const& c, double *out_f) {
	// Step 4 || 10: If W'T has negative diagonal elements, multiply corresponding columns in T by -1.
	for (integer i = 1; i <= w.ncol; i ++) {
		const double d = NUMinner (w.column (i), t.column (i));
		if (d < 0.0)
			t.column (i)  *=  -1.0;
	}

	// Step 5 & 11: f = tr W'T (Diag (T'CT))^-1/2

	*out_f = 0.0;
	for (integer i = 1; i <= w.ncol; i ++) {
		const longdouble d = NUMinner (w.column (i), t.column (i));
		double tct = 0.0;
		for (integer k = 1; k <= w.ncol; k ++)
			tct += t [k] [i] * NUMinner (c.row (k), t.column (i));
		if (tct > 0.0)
			*out_f += d / sqrt (tct);
	}
}

/*
	Using: Kiers & Groenen (1996), A monotonically convergent congruence algorithm for orthogonal congruence rotation,
	Psychometrika (61), 375-389.
*/
static void NUMmaximizeCongruence_inplace (MATVU const& t, constMATVU const& b, constMATVU const& a, integer maximumNumberOfIterations, double tolerance) {
	Melder_assert (t.nrow == t.ncol);
	Melder_assert (t.ncol == b.ncol && b.nrow == a.nrow && b.ncol == a.ncol);
	integer numberOfIterations = 0;

	if (b.ncol == 1) {
		t [1] [1] = 1.0;
		return;
	}
	const integer nc = b.ncol;
	autoMAT u = zero_MAT (nc, nc);
	autoVEC evec = zero_VEC (nc);
	autoSVD svd = SVD_create (nc, nc);

	// Steps 1 & 2: C = A'A and W = A'B

	autoMAT c = mtm_MAT (a);
	autoMAT w = mul_MAT (a.transpose(), b);
	const double checkc = NUMsum (c.all());
	const double checkw = NUMsum (w.all());
	
	Melder_require (checkc != 0.0 && checkw != 0.0,
		U"NUMmaximizeCongruence: the matrix should not be zero.");

	// Scale W by (diag(B'B))^-1/2

	for (integer j = 1; j <= nc; j ++) {
		double scale = NUMinner (b.column (j), b.row (j));
		if (scale > 0.0)
			scale = 1.0 / sqrt (scale);
		w.column (j)  *=  scale;
	}

	// Step 3: largest eigenvalue of C

	evec [1] = 1.0;
	const double rho = VECdominantEigenvector_inplace (evec.get(), c.get(), 1.0e-6);
	double f, f_old;

	do_steps45 (w.get(), t, c.get(), & f);
	do {
		for (integer j = 1; j <= nc; j ++) {
			// Step 7.a

			longdouble p = 0.0; 
			for (integer k = 1; k <= nc; k ++)
				for (integer i = 1; i <= nc; i ++)
					p += t [k] [j] * c [k] [i] * t [i] [j];

			// Step 7.b

			const double q = NUMinner (w.column (j), t.column (j));

			// Step 7.c

			if (q == 0.0) {
				u.column (j) <<= 0.0;
			} else {
				const double ww = NUMsum2 (w.column (j));
				for (integer i = 1; i <= nc; i ++) {
					const double ct = NUMinner (c.row (i), t.column (j));
					u [i] [j] = (q * (ct - rho * t [i] [j]) / double (p) - 2.0 * ww * t [i] [j] / q - w [i] [j]) / sqrt (double (p));
				}
			}
		}

		// Step 8

		SVD_update (svd.get(), u.all());

		// Step 9
		mul_MAT_out (t, svd -> u.all(), svd -> v.transpose());
		t  *=  -1.0;

		numberOfIterations++;
		f_old = f;

		// Steps 10 & 11 equal steps 4 & 5

		do_steps45 (w.all(), t, c.all(), & f);

	} while (fabs (f_old - f) > std::max (tolerance * fabs (f_old), NUMeps) && numberOfIterations < maximumNumberOfIterations);
}

autoAffineTransform Configurations_to_AffineTransform_congruence (Configuration me, Configuration thee, integer maximumNumberOfIterations, double tolerance) {
	try {
		// Use Procrustes transform to obtain starting configuration.
		// (We only need the transformation matrix T.)
		autoProcrustes p = Configurations_to_Procrustes (me, thee, false);
		Melder_assert (p -> dimension == my data.ncol);
		Melder_assert (p -> dimension == thy data.ncol);
		NUMmaximizeCongruence_inplace (p -> r.get (), my data.get (), thy data.get (), maximumNumberOfIterations, tolerance);
		autoAffineTransform at = AffineTransform_create (p -> dimension);
		at -> r.get () <<= p -> r.get ();
		return at;
	} catch (MelderError) {
		Melder_throw (me, U": no congruence transformation created.");
	}
}

autoConfiguration Configuration_AffineTransform_to_Configuration (Configuration me, AffineTransform thee) {
	try {
		Melder_require (my numberOfColumns == thy dimension,
			U"The number of columns in the Configuration should equal the dimension of the transform.");
		
		autoConfiguration him = Data_copy (me);

		// Apply transformation YT

		thy v_transform (his data.get(), my data.get());
		return him;
	} catch (MelderError) {
		Melder_throw (U"Configuration not created.");
	}
}

/* End of file Configuration_AffineTransform.cpp */
