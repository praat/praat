/* PairDistribution.c
 *
 * Copyright (C) 1997-2004 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2004/09/03 PairDistribution_Distributions_getFractionCorrect
 */

#include "PairDistribution.h"

#include "oo_DESTROY.h"
#include "PairDistribution_def.h"
#include "oo_COPY.h"
#include "PairDistribution_def.h"
#include "oo_EQUAL.h"
#include "PairDistribution_def.h"
#include "oo_WRITE_ASCII.h"
#include "PairDistribution_def.h"
#include "oo_READ_ASCII.h"
#include "PairDistribution_def.h"
#include "oo_WRITE_BINARY.h"
#include "PairDistribution_def.h"
#include "oo_READ_BINARY.h"
#include "PairDistribution_def.h"
#include "oo_DESCRIPTION.h"
#include "PairDistribution_def.h"

class_methods (PairProbability, Data)
	class_method_local (PairProbability, destroy)
	class_method_local (PairProbability, copy)
	class_method_local (PairProbability, equal)
	class_method_local (PairProbability, writeAscii)
	class_method_local (PairProbability, readAscii)
	class_method_local (PairProbability, writeBinary)
	class_method_local (PairProbability, readBinary)
	class_method_local (PairProbability, description)
class_methods_end

class_methods (PairDistribution, Data)
	class_method_local (PairDistribution, destroy)
	class_method_local (PairDistribution, copy)
	class_method_local (PairDistribution, equal)
	class_method_local (PairDistribution, writeAscii)
	class_method_local (PairDistribution, readAscii)
	class_method_local (PairDistribution, writeBinary)
	class_method_local (PairDistribution, readBinary)
	class_method_local (PairDistribution, description)
class_methods_end

PairProbability PairProbability_create (const char *string1, const char *string2, double weight) {
	PairProbability me = new (PairProbability);
	if (! me) return NULL;
	my string1 = Melder_strdup (string1);
	my string2 = Melder_strdup (string2);
	my weight = weight;
	if (Melder_hasError ()) forget (me);
	return me;
}

PairDistribution PairDistribution_create (void) {
	PairDistribution me = new (PairDistribution);
	if (! me || ! (my pairs = Ordered_create ())) { forget (me); return NULL; }
	return me;
}

int PairDistribution_add (PairDistribution me, const char *string1, const char *string2, double weight) {
	PairProbability pair = PairProbability_create (string1, string2, weight);
	if (! pair || ! Collection_addItem (my pairs, pair)) return 0;
	return 1;
}

int PairDistribution_to_Stringses (PairDistribution me, long nout, Strings *strings1, Strings *strings2) {
	double total = 0.0;
	long nin = my pairs -> size, iin, iout;
	*strings1 = *strings2 = NULL;
	if (nin < 1) { Melder_error ("No candidates."); goto end; }
	if (nout < 1) { Melder_error ("Number of generated string pairs should be positive."); goto end; }
	for (iin = 1; iin <= nin; iin ++) {
		PairProbability prob = my pairs -> item [iin];
		total += prob -> weight;
	}
	if (total <= 0.0) { Melder_error ("Sum of weights not positive."); goto end; }
	*strings1 = new (Strings); cherror
	(*strings1) -> numberOfStrings = nout;
	(*strings1) -> strings = NUMpvector (1, nout); cherror
	*strings2 = new (Strings); cherror
	(*strings2) -> numberOfStrings = nout;
	(*strings2) -> strings = NUMpvector (1, nout); cherror
	for (iout = 1; iout <= nout; iout ++) {
		PairProbability prob;
		do {
			double rand = NUMrandomUniform (0, total), sum = 0.0;
			for (iin = 1; iin <= nin; iin ++) {
				PairProbability prob = my pairs -> item [iin];
				sum += prob -> weight;
				if (rand <= sum) break;
			}
		} while (iin > nin);   /* Guard against rounding errors. */
		prob = my pairs -> item [iin];
		if (! prob -> string1 || ! prob -> string2) { Melder_error ("No string in probability pair %ld.", iin); goto end; }
		(*strings1) -> strings [iout] = Melder_strdup (prob -> string1); cherror
		(*strings2) -> strings [iout] = Melder_strdup (prob -> string2); cherror
	}
end:
	iferror { forget (*strings1); forget (*strings2);
			return Melder_error ("(PairDistribution_to_Stringses:) Not performed."); }
	return 1;
}

int PairDistribution_peekPair (PairDistribution me, char **string1, char **string2) {
	double total = 0.0;
	long nin = my pairs -> size, iin;
	PairProbability prob;
	*string1 = *string2 = NULL;
	if (nin < 1) { Melder_error ("No candidates."); goto end; }
	for (iin = 1; iin <= nin; iin ++) {
		prob = my pairs -> item [iin];
		total += prob -> weight;
	}
	do {
		double rand = NUMrandomUniform (0, total), sum = 0.0;
		for (iin = 1; iin <= nin; iin ++) {
			prob = my pairs -> item [iin];
			sum += prob -> weight;
			if (rand <= sum) break;
		}
	} while (iin > nin);   /* Guard against rounding errors. */
	prob = my pairs -> item [iin];
	if (! prob -> string1 || ! prob -> string2) { Melder_error ("No string in probability pair %ld.", iin); goto end; }
	*string1 = prob -> string1;
	*string2 = prob -> string2;
end:
	iferror return Melder_error ("(PairDistribution_peekPair:) Not performed.");
	return 1;
}

static int compare (PairProbability me, PairProbability thee) {
	return strcmp (my string1, thy string1);
}

static double PairDistribution_getFractionCorrect (PairDistribution me, int which) {
	double total = 0.0, correct = 0.0;
	long pairmin = 1, ipair;
	PairDistribution thee = Data_copy (me); cherror
	NUMsort_p (thy pairs -> size, thy pairs -> item, (int (*) (const void *, const void *)) compare);
	for (ipair = 1; ipair <= thy pairs -> size; ipair ++) {
		PairProbability prob = thy pairs -> item [ipair];
		total += prob -> weight;
	}
	if (total == 0.0) { Melder_error ("Total is zero."); goto end; }
	do {
		long pairmax = pairmin;
		char *firstInput = ((PairProbability) thy pairs -> item [pairmin]) -> string1;
		for (ipair = pairmin + 1; ipair <= thy pairs -> size; ipair ++) {
			PairProbability prob = thy pairs -> item [ipair];
			if (! strequ (prob -> string1, firstInput)) {
				pairmax = ipair - 1;
				break;
			}
		}
		if (ipair > thy pairs -> size) pairmax = thy pairs -> size;
		if (which == 0) {
			double pmax = 0.0;
			for (ipair = pairmin; ipair <= pairmax; ipair ++) {
				PairProbability prob = thy pairs -> item [ipair];
				double p = prob -> weight / total;
				if (p > pmax) pmax = p;
			}
			correct += pmax;
		} else {
			double sum = 0.0, p2 = 0.0;
			for (ipair = pairmin; ipair <= pairmax; ipair ++) {
				PairProbability prob = thy pairs -> item [ipair];
				double p = prob -> weight / total;
				sum += p;
				p2 += p * p;
			}
			correct += p2 / sum;
		}
		pairmin = pairmax + 1;
	} while (pairmin <= thy pairs -> size);
end:
	forget (thee);
	iferror { Melder_flushError ("(PairDistribution_getFractionCorrect:) Not computed."); return NUMundefined; }
	return correct;
}

double PairDistribution_getFractionCorrect_maximumLikelihood (PairDistribution me) {
	return PairDistribution_getFractionCorrect (me, 0);
}

double PairDistribution_getFractionCorrect_probabilityMatching (PairDistribution me) {
	return PairDistribution_getFractionCorrect (me, 1);
}

double PairDistribution_Distributions_getFractionCorrect (PairDistribution me, Distributions dist, long column) {
	double total = 0.0, correct = 0.0;
	long pairmin = 1, ipair;
	char string [1000];
	PairDistribution thee;
	if (column < 1 || column > dist -> numberOfColumns) return NUMundefined;
	thee = Data_copy (me); cherror
	NUMsort_p (thy pairs -> size, thy pairs -> item, (int (*) (const void *, const void *)) compare);
	for (ipair = 1; ipair <= thy pairs -> size; ipair ++) {
		PairProbability prob = thy pairs -> item [ipair];
		total += prob -> weight;
	}
	if (total == 0.0) { Melder_error ("Total is zero."); goto end; }
	do {
		long pairmax = pairmin, length, idist;
		double sum = 0.0, sumDist = 0.0;
		char *firstInput = ((PairProbability) thy pairs -> item [pairmin]) -> string1;
		for (ipair = pairmin + 1; ipair <= thy pairs -> size; ipair ++) {
			PairProbability prob = thy pairs -> item [ipair];
			if (! strequ (prob -> string1, firstInput)) {
				pairmax = ipair - 1;
				break;
			}
		}
		if (ipair > thy pairs -> size) pairmax = thy pairs -> size;
		for (ipair = pairmin; ipair <= pairmax; ipair ++) {
			PairProbability prob = thy pairs -> item [ipair];
			double p = prob -> weight / total, pout = 0.0;
			sprintf (string, "%s \\-> %s", prob -> string1, prob -> string2);
			for (idist = 1; idist <= dist -> numberOfRows; idist ++) {
				if (strequ (string, dist -> rowLabels [idist])) {
					pout = dist -> data [idist] [column];
					break;
				}
			}
			sum += p * pout;
		}
		sprintf (string, "%s \\-> ", firstInput);
		length = strlen (string);
		for (idist = 1; idist <= dist -> numberOfRows; idist ++) {
			if (strnequ (string, dist -> rowLabels [idist], length)) {
				sumDist += dist -> data [idist] [column];
			}
		}
		if (sumDist != 0.0) correct += sum / sumDist;
		pairmin = pairmax + 1;
	} while (pairmin <= thy pairs -> size);
end:
	forget (thee);
	iferror { Melder_flushError ("(PairDistribution_Distributions_getFractionCorrect:) Not computed."); return NUMundefined; }
	return correct;
}

/* End of file PairDistribution.c */
