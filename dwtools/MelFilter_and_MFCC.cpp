/* MelFilter_and_MFCC.cpp
 *
 * Copyright (C) 1993-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 2001
 djmw 20020813 GPL header
 djmw 20110207 Latest modification
*/

#include "MelFilter_and_MFCC.h"
#include "NUM2.h"

#define MAX(m,n) ((m) > (n) ? (m) : (n))
#define MIN(m,n) ((m) < (n) ? (m) : (n))

MFCC MelFilter_to_MFCC (MelFilter me, long numberOfCoefficients)
{
	try {
		long nf = my ny;
		double fmax_mel = my y1 + (nf - 1) * my dy;

		Melder_assert (numberOfCoefficients > 0);
	
		autoNUMmatrix<double> dct (NUMcosinesTable (1, numberOfCoefficients, nf), 1, 1);
		autoMFCC thee = MFCC_create (my xmin, my xmax, my nx, my dx, my x1, numberOfCoefficients, my y1, fmax_mel);		
		for (long frame = 1; frame <= my nx; frame++)
		{
			CC_Frame cf = (CC_Frame) & thy frame[frame];
		
			CC_Frame_init (cf, numberOfCoefficients); 
			for (long i = 1; i <= numberOfCoefficients; i++)
			{
				double p = 0;
				for (long  j = 1; j <= nf; j++)
				{
					p += my z[j][frame] * dct[i][j];
				}
				cf -> c[i] = p;
			}
		
			// c0 equals the average of the filterbank outputs.
			double p = 0;
			for (long j = 1; j <= nf; j++)
			{
				p += my z[j][frame];
			}
			cf -> c0 = p / nf;
		}
		return thee.transfer();
	} catch (MelderError) { Melder_thrown (me, ": no MFCC created."); }
}

MelFilter MFCC_to_MelFilter (MFCC me, long first_cc, long last_cc, double f1_mel, double df_mel)
{
	try {
		int use_c0 = 0;
		long nf = ((my fmax - MAX(my fmin, f1_mel)) / df_mel + 0.5);
		double fmin = MAX (f1_mel - df_mel, 0), fmax = f1_mel + (nf + 1) * df_mel;

		if (nf < 1) Melder_throw ("MFCC_to_MelFilter: the position of the "
		"first filter, the distance between the filters, and, the maximum "
		"do not result in a positive number of filters.");

		// Default values

		if (first_cc == 0)
		{
			first_cc = 1;
			use_c0 = 1;
		}
		if (last_cc == 0)
		{
			last_cc = my maximumNumberOfCoefficients;
		}
	
		// Be strict
	
		if (last_cc < first_cc || first_cc < 1 || last_cc > my maximumNumberOfCoefficients)  Melder_throw
		("MFCC_to_MelFilter: coefficients must be in interval [1,", my maximumNumberOfCoefficients,"].");
		autoNUMmatrix<double> dct (NUMcosinesTable (first_cc, last_cc, nf), first_cc, 1); // TODO ??
		//if ((dct = NUMcosinesTable (first_cc, last_cc, nf)) == NULL) return NULL;

		autoMelFilter thee = MelFilter_create (my xmin, my xmax, my nx, my dx, my x1, fmin, fmax, nf, df_mel, f1_mel);

		for (long frame = 1; frame <= my nx; frame++)
		{
			CC_Frame cf = (CC_Frame) & my frame[frame];
			long ie = MIN (last_cc, cf -> numberOfCoefficients); 
			for (long j = 1; j <= nf; j++)
			{
				double t = 0;
				for (long i = first_cc; i <= ie; i++)
				{
					t += cf -> c[i] * dct[i][j];
				}

				// The inverse CT has a factor 1/N

				t /= nf;
				if (use_c0) t +=  cf -> c0;
				thy z[j][frame] = t;
			}
		}
		return thee.transfer();
	} catch (MelderError) {Melder_thrown (me, ": no MelFilter created."); }
}

#undef MAX
#undef MIN

/* End of file MelFilter_and_MFCC.cpp */
