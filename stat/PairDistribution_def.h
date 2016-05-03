/* PairDistribution_def.h
 *
 * Copyright (C) 1997-2011,2013,2015 Paul Boersma
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


#define ooSTRUCT PairProbability
oo_DEFINE_CLASS (PairProbability, Daata)

	oo_STRING (string1)
	oo_STRING (string2)
	oo_DOUBLE (weight)

oo_END_CLASS (PairProbability)
#undef ooSTRUCT


#define ooSTRUCT PairDistribution
oo_DEFINE_CLASS (PairDistribution, Daata)

	oo_COLLECTION_OF (OrderedOf, pairs, PairProbability, 0)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (PairDistribution)
#undef ooSTRUCT


/* End of file PairDistribution_def.h */
