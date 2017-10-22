/* OTMulti_def.h
 *
 * Copyright (C) 2005,2007,2009-2011,2015-2017 Paul Boersma
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


#define ooSTRUCT OTConstraint
oo_DEFINE_STRUCT (OTConstraint)

	oo_STRING (name)
	oo_DOUBLE (ranking)
	oo_DOUBLE (disharmony)
	oo_FROM (2)
		oo_DOUBLE (plasticity)
	oo_ENDFROM

	#if oo_READING
		if (formatVersion < 2) {
			plasticity = 1.0;
		}
	#endif

	#if !oo_READING && !oo_WRITING
		oo_INT16 (tiedToTheLeft)
		oo_INT16 (tiedToTheRight)
	#endif

oo_END_STRUCT (OTConstraint)
#undef ooSTRUCT


#define ooSTRUCT OTCandidate
oo_DEFINE_STRUCT (OTCandidate)

	oo_STRING (string)
	oo_INTEGER (numberOfConstraints)
	oo_INT_VECTOR (marks, numberOfConstraints)

	#if !oo_READING && !oo_WRITING
		oo_DOUBLE (harmony)
		oo_DOUBLE (probability)
	#endif

oo_END_STRUCT (OTCandidate)
#undef ooSTRUCT


#define ooSTRUCT OTMulti
oo_DEFINE_CLASS (OTMulti, Daata)

	oo_FROM (1)
		oo_ENUM (kOTGrammar_decisionStrategy, decisionStrategy)
	oo_ENDFROM
	oo_FROM (2)
		oo_DOUBLE (leak)
	oo_ENDFROM
	oo_INTEGER (numberOfConstraints)
	oo_STRUCT_VECTOR (OTConstraint, constraints, numberOfConstraints)
	oo_INTEGER_VECTOR (index, numberOfConstraints)
	oo_INTEGER (numberOfCandidates)
	oo_STRUCT_VECTOR (OTCandidate, candidates, numberOfCandidates)

	#if oo_READING
		OTMulti_sort (this);
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (OTMulti)
#undef ooSTRUCT


/* End of file OTMulti_def.h */
