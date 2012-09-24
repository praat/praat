/* OTGrammar_ex_NPA.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2011/03/29 C++
 * pb 2011/07/12 C++
 */

#include "OTGrammar.h"

OTGrammar OTGrammar_create_NPA_grammar (void) {
	try {
		OTGrammarCandidate candidate;
		OTGrammarTableau tableau;
		OTGrammarConstraint constraint;
		autoOTGrammar me = Thing_new (OTGrammar);
		my constraints = NUMvector <structOTGrammarConstraint> (1, my numberOfConstraints = 3);
		constraint = & my constraints [1];
			constraint -> name = Melder_wcsdup (L"*G\\s{ESTURE}");
			constraint -> ranking = 102.7;
			constraint -> plasticity = 1.0;
		constraint = & my constraints [2];
			constraint -> name = Melder_wcsdup (L"*R\\s{EPLACE} (n, m)");
			constraint -> ranking = 100.0;
			constraint -> plasticity = 1.0;
		constraint = & my constraints [3];
			constraint -> name = Melder_wcsdup (L"*R\\s{EPLACE} (t, p)");
			constraint -> ranking = 112.0;
			constraint -> plasticity = 1.0;
		my tableaus = NUMvector <structOTGrammarTableau> (1, my numberOfTableaus = 2);
		tableau = & my tableaus [1];
			tableau -> input = Melder_wcsdup (L"an+pa");
			tableau -> candidates = NUMvector <structOTGrammarCandidate> (1, tableau -> numberOfCandidates = 2);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_wcsdup (L"anpa");
				candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints = 3);
				candidate -> marks [1] = 1;
			candidate = & tableau -> candidates [2];
				candidate -> output = Melder_wcsdup (L"ampa");
				candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints = 3);
				candidate -> marks [2] = 1;
		tableau = & my tableaus [2];
			tableau -> input = Melder_wcsdup (L"at+ma");
			tableau -> candidates = NUMvector <structOTGrammarCandidate> (1, tableau -> numberOfCandidates = 2);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_wcsdup (L"atma");
				candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints = 3);
				candidate -> marks [1] = 1;
			candidate = & tableau -> candidates [2];
				candidate -> output = Melder_wcsdup (L"apma");
				candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints = 3);
				candidate -> marks [3] = 1;
		OTGrammar_checkIndex (me.peek());
		OTGrammar_newDisharmonies (me.peek(), 0.0);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Nasal place assimilation grammar not created.");
	}
}

PairDistribution OTGrammar_create_NPA_distribution (void) {
	try {
		autoPairDistribution me = PairDistribution_create ();
		PairDistribution_add (me.peek(), L"at+ma", L"atma", 100);
		PairDistribution_add (me.peek(), L"at+ma", L"apma",   0);
		PairDistribution_add (me.peek(), L"an+pa", L"anpa",  20);
		PairDistribution_add (me.peek(), L"an+pa", L"ampa",  80);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Nasal place assimilation distribution not created.");
	}
}

/* End of file OTGrammar_ex_NPA.cpp */
