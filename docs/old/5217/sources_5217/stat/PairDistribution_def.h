/* PairDistribution_def.h
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
 * pb 2007/08/12
 */


#define ooSTRUCT PairProbability
oo_DEFINE_CLASS (PairProbability, Data)

	oo_STRINGW (string1)
	oo_STRINGW (string2)
	oo_DOUBLE (weight)

oo_END_CLASS (PairProbability)
#undef ooSTRUCT


#define ooSTRUCT PairDistribution
oo_DEFINE_CLASS (PairDistribution, Data)

	oo_COLLECTION (Ordered, pairs, PairProbability, 0)

oo_END_CLASS (PairDistribution)
#undef ooSTRUCT


/* End of file PairDistribution_def.h */
