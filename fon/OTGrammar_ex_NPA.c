/* OTGrammar_ex_NPA.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2007/07/23 constraint plasticity
 * pb 2007/08/12 wchar_t
 */

#include "OTGrammar.h"

OTGrammar OTGrammar_create_NPA_grammar (void) {
	OTGrammarCandidate candidate;
	OTGrammarTableau tableau;
	OTGrammarConstraint constraint;
	OTGrammar me = new (OTGrammar); cherror
	my constraints = NUMstructvector (OTGrammarConstraint, 1, my numberOfConstraints = 3); cherror
	constraint = & my constraints [1];
		constraint -> name = Melder_wcsdup (L"*G\\s{ESTURE}"); cherror
		constraint -> ranking = 102.7;
		constraint -> plasticity = 1.0;
	constraint = & my constraints [2];
		constraint -> name = Melder_wcsdup (L"*R\\s{EPLACE} (n, m)"); cherror
		constraint -> ranking = 100.0;
		constraint -> plasticity = 1.0;
	constraint = & my constraints [3];
		constraint -> name = Melder_wcsdup (L"*R\\s{EPLACE} (t, p)"); cherror
		constraint -> ranking = 112.0;
		constraint -> plasticity = 1.0;
	my tableaus = NUMstructvector (OTGrammarTableau, 1, my numberOfTableaus = 2); cherror
	tableau = & my tableaus [1];
		tableau -> input = Melder_wcsdup (L"an+pa"); cherror
		tableau -> candidates = NUMstructvector (OTGrammarCandidate, 1, tableau -> numberOfCandidates = 2); cherror
		candidate = & tableau -> candidates [1];
			candidate -> output = Melder_wcsdup (L"anpa");
			candidate -> marks = NUMivector (1, candidate -> numberOfConstraints = 3); cherror
			candidate -> marks [1] = 1;
		candidate = & tableau -> candidates [2];
			candidate -> output = Melder_wcsdup (L"ampa");
			candidate -> marks = NUMivector (1, candidate -> numberOfConstraints = 3); cherror
			candidate -> marks [2] = 1;
	tableau = & my tableaus [2];
		tableau -> input = Melder_wcsdup (L"at+ma"); cherror
		tableau -> candidates = NUMstructvector (OTGrammarCandidate, 1, tableau -> numberOfCandidates = 2); cherror
		candidate = & tableau -> candidates [1];
			candidate -> output = Melder_wcsdup (L"atma");
			candidate -> marks = NUMivector (1, candidate -> numberOfConstraints = 3); cherror
			candidate -> marks [1] = 1;
		candidate = & tableau -> candidates [2];
			candidate -> output = Melder_wcsdup (L"apma");
			candidate -> marks = NUMivector (1, candidate -> numberOfConstraints = 3); cherror
			candidate -> marks [3] = 1;
	OTGrammar_checkIndex (me);
	OTGrammar_newDisharmonies (me, 0.0);
end:	iferror forget (me);
	return me;
}

PairDistribution OTGrammar_create_NPA_distribution (void) {
	PairDistribution me = PairDistribution_create ();
	PairDistribution_add (me, L"at+ma", L"atma", 100);
	PairDistribution_add (me, L"at+ma", L"apma",   0);
	PairDistribution_add (me, L"an+pa", L"anpa",  20);
	PairDistribution_add (me, L"an+pa", L"ampa",  80);
	return me;
}

/* End of file OTGrammar_ex_NPA.c */
