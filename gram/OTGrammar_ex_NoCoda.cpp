/* OTGrammar_ex_NoCoda.cpp
 *
 * Copyright (C) 1997-2005,2007,2009,2011,2012,2015-2020 Paul Boersma
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

#include "OTGrammar.h"

autoOTGrammar OTGrammar_create_NoCoda_grammar () {
	try {
		OTGrammarCandidate candidate;
		OTGrammarTableau tableau;
		OTGrammarConstraint constraint;
		autoOTGrammar me = Thing_new (OTGrammar);
		my constraints = newvectorzero <structOTGrammarConstraint> (my numberOfConstraints = 2);
		constraint = & my constraints [1];
			constraint -> name = Melder_dup (U"N\\s{O}C\\s{ODA}");
			constraint -> ranking = 100.0;
			constraint -> plasticity = 1.0;
		constraint = & my constraints [2];
			constraint -> name = Melder_dup (U"P\\s{ARSE}");
			constraint -> ranking = 90.0;
			constraint -> plasticity = 1.0;
		my tableaus = newvectorzero <structOTGrammarTableau> (my numberOfTableaus = 2);
		tableau = & my tableaus [1];
			tableau -> input = Melder_dup (U"pat");
			tableau -> candidates = newvectorzero <structOTGrammarCandidate> (tableau -> numberOfCandidates =  2);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_dup (U"pa");
				candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints = 2);
				candidate -> marks [2] = 1;
			candidate = & tableau -> candidates [2];
				candidate -> output = Melder_dup (U"pat");
				candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints = 2);
				candidate -> marks [1] = 1;
		tableau = & my tableaus [2];
			tableau -> input = Melder_dup (U"pa");
			tableau -> candidates = newvectorzero <structOTGrammarCandidate> (tableau -> numberOfCandidates =  1);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_dup (U"pa");
				candidate -> marks = zero_INTVEC (candidate -> numberOfConstraints = 2);
		OTGrammar_checkIndex (me.get());
		OTGrammar_newDisharmonies (me.get(), 0.0);
		return me;
	} catch (MelderError) {
		Melder_throw (U"NoCoda grammar not created.");
	}
}

/* End of file OTGrammar_ex_NoCoda.cpp */
