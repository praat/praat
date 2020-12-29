/* OTGrammar_ex_NPA.cpp
 *
 * Copyright (C) 1992-2005,2007,2009,2011,2012,2015-2020 Paul Boersma
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

/*
 * pb 2002/07/16 GPL
 * pb 2007/07/23 constraint plasticity
 * pb 2007/08/12 wchar
 * pb 2011/03/29 C++
 * pb 2011/07/12 C++
 * pb 2015/06/03 char32
 */

#include "OTGrammar.h"

autoOTGrammar OTGrammar_create_NPA_grammar () {
	try {
		OTGrammarCandidate candidate;
		OTGrammarTableau tableau;
		OTGrammarConstraint constraint;
		autoOTGrammar me = Thing_new (OTGrammar);
		my constraints = newvectorzero <structOTGrammarConstraint> (my numberOfConstraints = 3);
		constraint = & my constraints [1];
			constraint -> name = Melder_dup (U"*G\\s{ESTURE}");
			constraint -> ranking = 102.7;
			constraint -> plasticity = 1.0;
		constraint = & my constraints [2];
			constraint -> name = Melder_dup (U"*R\\s{EPLACE} (n, m)");
			constraint -> ranking = 100.0;
			constraint -> plasticity = 1.0;
		constraint = & my constraints [3];
			constraint -> name = Melder_dup (U"*R\\s{EPLACE} (t, p)");
			constraint -> ranking = 112.0;
			constraint -> plasticity = 1.0;
		my tableaus = newvectorzero <structOTGrammarTableau> (my numberOfTableaus = 2);
		tableau = & my tableaus [1];
			tableau -> input = Melder_dup (U"an+pa");
			tableau -> candidates = newvectorzero <structOTGrammarCandidate> (tableau -> numberOfCandidates = 2);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_dup (U"anpa");
				candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints = 3);
				candidate -> marks [1] = 1;
			candidate = & tableau -> candidates [2];
				candidate -> output = Melder_dup (U"ampa");
				candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints = 3);
				candidate -> marks [2] = 1;
		tableau = & my tableaus [2];
			tableau -> input = Melder_dup (U"at+ma");
			tableau -> candidates = newvectorzero <structOTGrammarCandidate> (tableau -> numberOfCandidates = 2);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_dup (U"atma");
				candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints = 3);
				candidate -> marks [1] = 1;
			candidate = & tableau -> candidates [2];
				candidate -> output = Melder_dup (U"apma");
				candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints = 3);
				candidate -> marks [3] = 1;
		OTGrammar_checkIndex (me.get());
		OTGrammar_newDisharmonies (me.get(), 0.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Nasal place assimilation grammar not created.");
	}
}

autoPairDistribution OTGrammar_create_NPA_distribution () {
	try {
		autoPairDistribution me = PairDistribution_create ();
		PairDistribution_add (me.get(), U"at+ma", U"atma", 100);
		PairDistribution_add (me.get(), U"at+ma", U"apma",   0);
		PairDistribution_add (me.get(), U"an+pa", U"anpa",  20);
		PairDistribution_add (me.get(), U"an+pa", U"ampa",  80);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Nasal place assimilation distribution not created.");
	}
}

/* End of file OTGrammar_ex_NPA.cpp */
