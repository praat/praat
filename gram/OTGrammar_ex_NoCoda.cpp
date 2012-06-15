/* OTGrammar_ex_NoCoda.cpp
 *
 * Copyright (C) 1997-2011 Paul Boersma
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

#include "OTGrammar.h"

OTGrammar OTGrammar_create_NoCoda_grammar (void) {
	try {
		OTGrammarCandidate candidate;
		OTGrammarTableau tableau;
		OTGrammarConstraint constraint;
		autoOTGrammar me = Thing_new (OTGrammar);
		my constraints = NUMvector <structOTGrammarConstraint> (1, my numberOfConstraints = 2);
		constraint = & my constraints [1];
			constraint -> name = Melder_wcsdup (L"N\\s{O}C\\s{ODA}");
			constraint -> ranking = 100.0;
			constraint -> plasticity = 1.0;
		constraint = & my constraints [2];
			constraint -> name = Melder_wcsdup (L"P\\s{ARSE}");
			constraint -> ranking = 90.0;
			constraint -> plasticity = 1.0;
		my tableaus = NUMvector <structOTGrammarTableau> (1, my numberOfTableaus = 2);
		tableau = & my tableaus [1];
			tableau -> input = Melder_wcsdup (L"pat");
			tableau -> candidates = NUMvector <structOTGrammarCandidate> (1, tableau -> numberOfCandidates =  2);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_wcsdup (L"pa");
				candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints = 2);
				candidate -> marks [2] = 1;
			candidate = & tableau -> candidates [2];
				candidate -> output = Melder_wcsdup (L"pat");
				candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints = 2);
				candidate -> marks [1] = 1;
		tableau = & my tableaus [2];
			tableau -> input = Melder_wcsdup (L"pa");
			tableau -> candidates = NUMvector <structOTGrammarCandidate> (1, tableau -> numberOfCandidates =  1);
			candidate = & tableau -> candidates [1];
				candidate -> output = Melder_wcsdup (L"pa");
				candidate -> marks = NUMvector <int> (1, candidate -> numberOfConstraints = 2);
		OTGrammar_checkIndex (me.peek());
		OTGrammar_newDisharmonies (me.peek(), 0.0);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("NoCoda grammar not created.");
	}
}

/* End of file OTGrammar_ex_NoCoda.cpp */
