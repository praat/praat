/* OTGrammar_def.h
 *
 * Copyright (C) 1997-2011,2015-2020 Paul Boersma
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


#define ooSTRUCT OTGrammarConstraint
oo_DEFINE_STRUCT (OTGrammarConstraint)

	oo_STRING (name)
	oo_DOUBLE (ranking)
	oo_DOUBLE (disharmony)

	oo_FROM (2)
		oo_DOUBLE (plasticity)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (2)
			our plasticity = 1.0;
		oo_VERSION_END
	#endif

	#if ! oo_READING && ! oo_WRITING
		oo_BOOLEAN (tiedToTheLeft)
		oo_BOOLEAN (tiedToTheRight)
	#endif

oo_END_STRUCT (OTGrammarConstraint)
#undef ooSTRUCT


#define ooSTRUCT OTGrammarFixedRanking
oo_DEFINE_STRUCT (OTGrammarFixedRanking)

	oo_INTEGER (higher)
	oo_INTEGER (lower)

oo_END_STRUCT (OTGrammarFixedRanking)
#undef ooSTRUCT


#define ooSTRUCT OTGrammarCandidate
oo_DEFINE_STRUCT (OTGrammarCandidate)

	oo_STRING (output)
	oo_INTEGER (numberOfConstraints)
	oo_INTVEC16 (marks, numberOfConstraints)

	#if ! oo_READING && ! oo_WRITING
		oo_DOUBLE (harmony)
		oo_DOUBLE (probability)
		oo_INTEGER (numberOfPotentialPartialOutputsMatching)
		oo_BOOLVEC (partialOutputMatches, numberOfPotentialPartialOutputsMatching)
	#endif

oo_END_STRUCT (OTGrammarCandidate)
#undef ooSTRUCT


#define ooSTRUCT OTGrammarTableau
oo_DEFINE_STRUCT (OTGrammarTableau)

	oo_STRING (input)
	oo_INTEGER (numberOfCandidates)
	oo_STRUCTVEC (OTGrammarCandidate, candidates, numberOfCandidates)

oo_END_STRUCT (OTGrammarTableau)
#undef ooSTRUCT


#define ooSTRUCT OTGrammar
oo_DEFINE_CLASS (OTGrammar, Daata)

	oo_FROM (1)
		oo_ENUM (kOTGrammar_decisionStrategy, decisionStrategy)
	oo_ENDFROM
	oo_FROM (2)
		oo_DOUBLE (leak)
	oo_ENDFROM
	oo_INTEGER (numberOfConstraints)
	oo_STRUCTVEC (OTGrammarConstraint, constraints, numberOfConstraints)
	oo_INTVEC (index, numberOfConstraints)   // not read or written in text files
	oo_INTEGER (numberOfFixedRankings)
	oo_STRUCTVEC (OTGrammarFixedRanking, fixedRankings, numberOfFixedRankings)
	oo_INTEGER (numberOfTableaus)
	oo_STRUCTVEC (OTGrammarTableau, tableaus, numberOfTableaus)

	#if oo_READING
		OTGrammar_sort (this);
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
		void checkConstraintNumber (integer constraintNumber) {
			Melder_require (constraintNumber >= 1,
				U"The specified constraint number (", constraintNumber, U") should not be less than 1.");
			Melder_require (constraintNumber <= our numberOfConstraints,
				U"The specified constraint number (", constraintNumber,
				U") should not exceed the number of constraints (", our numberOfConstraints, U")."
			);
		}
		void checkTableauNumber (integer tableauNumber) {
			Melder_require (tableauNumber >= 1,
				U"The specified tableau number (", tableauNumber, U") should not be less than 1.");
			Melder_require (tableauNumber <= our numberOfTableaus,
				U"The specified tableau number (", tableauNumber,
				U") should not exceed the number of tableaus (", our numberOfTableaus, U")."
			);
		}
		void checkTableauAndCandidateNumber (integer tableauNumber, integer candidateNumber) {
			our checkTableauNumber (tableauNumber);
			Melder_require (candidateNumber >= 1,
				U"The specified candidate number (", candidateNumber, U") should not be less than 1.");
			Melder_require (candidateNumber <= our tableaus [tableauNumber]. numberOfCandidates,
				U"The specified candidate number (", candidateNumber,
				U") should not exceed the number of candidates in tableau ", tableauNumber,
				U" (", our tableaus [tableauNumber]. numberOfCandidates, U")."
			);
		}
	#endif

oo_END_CLASS (OTGrammar)
#undef ooSTRUCT


/* End of file OTGrammar_def.h */
