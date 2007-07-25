/* OTGrammar_ex_NoCoda.c
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
 * pb 2007/07/23 constraint plasticity
 */

#include "OTGrammar.h"

OTGrammar OTGrammar_create_NoCoda_grammar (void) {
	OTGrammarCandidate candidate;
	OTGrammarTableau tableau;
	OTGrammarConstraint constraint;
	OTGrammar me = new (OTGrammar); cherror
	my constraints = NUMstructvector (OTGrammarConstraint, 1, my numberOfConstraints = 2); cherror
	constraint = & my constraints [1];
		constraint -> name = Melder_strdup ("N\\s{O}C\\s{ODA}"); cherror
		constraint -> ranking = 100.0;
		constraint -> plasticity = 1.0;
	constraint = & my constraints [2];
		constraint -> name = Melder_strdup ("P\\s{ARSE}"); cherror
		constraint -> ranking = 90.0;
		constraint -> plasticity = 1.0;
	my tableaus = NUMstructvector (OTGrammarTableau, 1, my numberOfTableaus = 2); cherror
	tableau = & my tableaus [1];
		tableau -> input = Melder_strdup ("pat"); cherror
		tableau -> candidates = NUMstructvector (OTGrammarCandidate, 1, tableau -> numberOfCandidates =  2); cherror
		candidate = & tableau -> candidates [1];
			candidate -> output = Melder_strdup ("pa");
			candidate -> marks = NUMivector (1, candidate -> numberOfConstraints = 2); cherror
			candidate -> marks [2] = 1;
		candidate = & tableau -> candidates [2];
			candidate -> output = Melder_strdup ("pat");
			candidate -> marks = NUMivector (1, candidate -> numberOfConstraints = 2); cherror
			candidate -> marks [1] = 1;
	tableau = & my tableaus [2];
		tableau -> input = Melder_strdup ("pa"); cherror
		tableau -> candidates = NUMstructvector (OTGrammarCandidate, 1, tableau -> numberOfCandidates =  1); cherror
		candidate = & tableau -> candidates [1];
			candidate -> output = Melder_strdup ("pa");
			candidate -> marks = NUMivector (1, candidate -> numberOfConstraints = 2); cherror
	OTGrammar_checkIndex (me);
	OTGrammar_newDisharmonies (me, 0.0);
end:	iferror forget (me);
	return me;
}

/* End of file OTGrammar_ex_NoCoda.c */
