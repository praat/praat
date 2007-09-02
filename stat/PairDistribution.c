/* PairDistribution.c
 *
 * Copyright (C) 1997-2007 Paul Boersma
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
 * pb 2007/03/29 PairDistribution_to_Table
 */

#include "PairDistribution.h"

#include "oo_DESTROY.h"
#include "PairDistribution_def.h"
#include "oo_COPY.h"
#include "PairDistribution_def.h"
#include "oo_EQUAL.h"
#include "PairDistribution_def.h"
#include "oo_WRITE_TEXT.h"
#include "PairDistribution_def.h"
#include "oo_READ_TEXT.h"
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
	class_method_local (PairProbability, writeText)
	class_method_local (PairProbability, readText)
	class_method_local (PairProbability, writeBinary)
	class_method_local (PairProbability, readBinary)
	class_method_local (PairProbability, description)
class_methods_end

static void info (I) {
	iam (PairDistribution);
	inherited (PairDistribution) info (me);
	MelderInfo_writeLine2 (L"Number of pairs: ", Melder_integer (my pairs -> size));
}

class_methods (PairDistribution, Data)
	class_method_local (PairDistribution, destroy)
	class_method_local (PairDistribution, copy)
	class_method_local (PairDistribution, equal)
	class_method_local (PairDistribution, writeText)
	class_method_local (PairDistribution, readText)
	class_method_local (PairDistribution, writeBinary)
	class_method_local (PairDistribution, readBinary)
	class_method_local (PairDistribution, description)
	class_method (info)
class_methods_end

PairProbability PairProbability_create (const wchar_t *string1, const wchar_t *string2, double weight) {
	PairProbability me = new (PairProbability);
	if (! me) return NULL;
	my string1 = Melder_wcsdup (string1);
	my string2 = Melder_wcsdup (string2);
	my weight = weight;
	if (Melder_hasError ()) forget (me);
	return me;
}

PairDistribution PairDistribution_create (void) {
	PairDistribution me = new (PairDistribution);
	if (! me || ! (my pairs = Ordered_create ())) { forget (me); return NULL; }
	return me;
}

int PairDistribution_add (PairDistribution me, const wchar_t *string1, const wchar_t *string2, double weight) {
	PairProbability pair = PairProbability_create (string1, string2, weight);
	if (! pair || ! Collection_addItem (my pairs, pair)) return 0;
	return 1;
}

void PairDistribution_removeZeroWeights (PairDistribution me) {
	long ipair;
	for (ipair = my pairs -> size; ipair > 0; ipair --) {
		PairProbability prob = my pairs -> item [ipair];
		if (prob -> weight <= 0.0) {
			Collection_removeItem (my pairs, ipair);
		}
	}
}

int PairDistribution_to_Stringses (PairDistribution me, long nout, Strings *strings1, Strings *strings2) {
	double total = 0.0;
	long nin = my pairs -> size, iin, iout;
	*strings1 = *strings2 = NULL;
	if (nin < 1)
		error1 (L"No candidates.")
	if (nout < 1)
		error1 (L"Number of generated string pairs should be positive.")
	for (iin = 1; iin <= nin; iin ++) {
		PairProbability prob = my pairs -> item [iin];
		total += prob -> weight;
	}
	if (total <= 0.0)
		error1 (L"Sum of weights not positive.")
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
		if (! prob -> string1 || ! prob -> string2)
			error3 (L"No string in probability pair ", Melder_integer (iin), L".")
		(*strings1) -> strings [iout] = Melder_wcsdup (prob -> string1); cherror
		(*strings2) -> strings [iout] = Melder_wcsdup (prob -> string2); cherror
	}
end:
	iferror { forget (*strings1); forget (*strings2);
			return Melder_error1 (L"(PairDistribution_to_Stringses:) Not performed."); }
	return 1;
}

int PairDistribution_peekPair (PairDistribution me, wchar_t **string1, wchar_t **string2) {
	double total = 0.0;
	long nin = my pairs -> size, iin;
	PairProbability prob;
	*string1 = *string2 = NULL;
	if (nin < 1) error1 (L"No candidates.")
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
	if (! prob -> string1 || ! prob -> string2) error3 (L"No string in probability pair ", Melder_integer (iin), L".")
	*string1 = prob -> string1;
	*string2 = prob -> string2;
end:
	iferror return Melder_error1 (L"(PairDistribution_peekPair:) Not performed.");
	return 1;
}

static int compare (PairProbability me, PairProbability thee) {
	return wcscmp (my string1, thy string1);
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
	if (total == 0.0)
		error1 (L"Total is zero.")
	do {
		long pairmax = pairmin;
		wchar_t *firstInput = ((PairProbability) thy pairs -> item [pairmin]) -> string1;
		for (ipair = pairmin + 1; ipair <= thy pairs -> size; ipair ++) {
			PairProbability prob = thy pairs -> item [ipair];
			if (! wcsequ (prob -> string1, firstInput)) {
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
	wchar_t string [1000];
	PairDistribution thee;
	if (column < 1 || column > dist -> numberOfColumns) return NUMundefined;
	thee = Data_copy (me); cherror
	NUMsort_p (thy pairs -> size, thy pairs -> item, (int (*) (const void *, const void *)) compare);
	for (ipair = 1; ipair <= thy pairs -> size; ipair ++) {
		PairProbability prob = thy pairs -> item [ipair];
		total += prob -> weight;
	}
	if (total == 0.0) error1 (L"Total is zero.")
	do {
		long pairmax = pairmin, length, idist;
		double sum = 0.0, sumDist = 0.0;
		wchar_t *firstInput = ((PairProbability) thy pairs -> item [pairmin]) -> string1;
		for (ipair = pairmin + 1; ipair <= thy pairs -> size; ipair ++) {
			PairProbability prob = thy pairs -> item [ipair];
			if (! wcsequ (prob -> string1, firstInput)) {
				pairmax = ipair - 1;
				break;
			}
		}
		if (ipair > thy pairs -> size) pairmax = thy pairs -> size;
		for (ipair = pairmin; ipair <= pairmax; ipair ++) {
			PairProbability prob = thy pairs -> item [ipair];
			double p = prob -> weight / total, pout = 0.0;
			swprintf (string, 1000, L"%ls \\-> %ls", prob -> string1, prob -> string2);
			for (idist = 1; idist <= dist -> numberOfRows; idist ++) {
				if (wcsequ (string, dist -> rowLabels [idist])) {
					pout = dist -> data [idist] [column];
					break;
				}
			}
			sum += p * pout;
		}
		swprintf (string, 1000, L"%ls \\-> ", firstInput);
		length = wcslen (string);
		for (idist = 1; idist <= dist -> numberOfRows; idist ++) {
			if (wcsnequ (string, dist -> rowLabels [idist], length)) {
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

Table PairDistribution_to_Table (PairDistribution me) {
	Table thee = Table_createWithColumnNames (my pairs -> size, L"string1 string2 weight"); cherror
	for (long ipair = 1; ipair <= my pairs -> size; ipair ++) {
		PairProbability prob = my pairs -> item [ipair];
		Table_setStringValue (thee, ipair, 1, prob -> string1); cherror
		Table_setStringValue (thee, ipair, 2, prob -> string2); cherror
		Table_setNumericValue (thee, ipair, 3, prob -> weight); cherror
	}
end:
	iferror forget (thee);
	return thee;
}

/* End of file PairDistribution.c */
