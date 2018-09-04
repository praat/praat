/* Speaker_def.h
 *
 * Copyright (C) 1992-2005,2011,2015-2018 Paul Boersma
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


#define ooSTRUCT Speaker_CordDimensions
oo_DEFINE_STRUCT (Speaker_CordDimensions)

	oo_INT16 (numberOfMasses)
	oo_DOUBLE (length)

oo_END_STRUCT (Speaker_CordDimensions)
#undef ooSTRUCT


#define ooSTRUCT Speaker_CordSpring
oo_DEFINE_STRUCT (Speaker_CordSpring)

	oo_DOUBLE (thickness)
	oo_DOUBLE (mass)
	oo_DOUBLE (k1)

oo_END_STRUCT (Speaker_CordSpring)
#undef ooSTRUCT


#define ooSTRUCT Speaker_GlottalShunt
oo_DEFINE_STRUCT (Speaker_GlottalShunt)

	oo_DOUBLE (Dx)
	oo_DOUBLE (Dy)
	oo_DOUBLE (Dz)

oo_END_STRUCT (Speaker_GlottalShunt)
#undef ooSTRUCT


#define ooSTRUCT Speaker_Velum
oo_DEFINE_STRUCT (Speaker_Velum)   // V

	oo_DOUBLE (x)
	oo_DOUBLE (y)
	oo_DOUBLE (a)

oo_END_STRUCT (Speaker_Velum)
#undef ooSTRUCT


#define ooSTRUCT Speaker_Palate
oo_DEFINE_STRUCT (Speaker_Palate)   // OM

	oo_DOUBLE (radius)

oo_END_STRUCT (Speaker_Palate)
#undef ooSTRUCT


#define ooSTRUCT Speaker_Tip
oo_DEFINE_STRUCT (Speaker_Tip)

	oo_DOUBLE (length)

oo_END_STRUCT (Speaker_Tip)
#undef ooSTRUCT


#define ooSTRUCT Speaker_Alveoli
oo_DEFINE_STRUCT (Speaker_Alveoli)

	oo_DOUBLE (x)
	oo_DOUBLE (y)
	oo_DOUBLE (a)

oo_END_STRUCT (Speaker_Alveoli)
#undef ooSTRUCT


#define ooSTRUCT Speaker_TeethCavity
oo_DEFINE_STRUCT (Speaker_TeethCavity)

	oo_DOUBLE (dx1)
	oo_DOUBLE (dx2)
	oo_DOUBLE (dy)

oo_END_STRUCT (Speaker_TeethCavity)
#undef ooSTRUCT


#define ooSTRUCT Speaker_LowerTeeth
oo_DEFINE_STRUCT (Speaker_LowerTeeth)   // rest position of J

	oo_DOUBLE (r)
	oo_DOUBLE (a)

oo_END_STRUCT (Speaker_LowerTeeth)
#undef ooSTRUCT


#define ooSTRUCT Speaker_UpperTeeth
oo_DEFINE_STRUCT (Speaker_UpperTeeth)   // U

	oo_DOUBLE (x)
	oo_DOUBLE (y)

oo_END_STRUCT (Speaker_UpperTeeth)
#undef ooSTRUCT


#define ooSTRUCT Speaker_Lip
oo_DEFINE_STRUCT (Speaker_Lip)

	oo_DOUBLE (dx)
	oo_DOUBLE (dy)

oo_END_STRUCT (Speaker_Lip)
#undef ooSTRUCT


#define ooSTRUCT Speaker_Nose
oo_DEFINE_STRUCT (Speaker_Nose)

	oo_DOUBLE (Dx)
	oo_DOUBLE (Dz)
	oo_VEC (weq, 14)

oo_END_STRUCT (Speaker_Nose)
#undef ooSTRUCT


#define ooSTRUCT Speaker
oo_DEFINE_CLASS (Speaker, Daata)

	oo_DOUBLE (relativeSize)   // different for female, male, child

	/* In the larynx. */

	oo_STRUCT (Speaker_CordDimensions, cord)
	oo_STRUCT (Speaker_CordSpring, lowerCord)
	oo_STRUCT (Speaker_CordSpring, upperCord)
	oo_STRUCT (Speaker_GlottalShunt, shunt)

	/* Above the larynx. */

	oo_STRUCT (Speaker_Velum, velum)
	oo_STRUCT (Speaker_Palate, palate)
	oo_STRUCT (Speaker_Tip, tip)
	oo_DOUBLE (neutralBodyDistance)
	oo_STRUCT (Speaker_Alveoli, alveoli)
	oo_STRUCT (Speaker_TeethCavity, teethCavity)
	oo_STRUCT (Speaker_LowerTeeth, lowerTeeth)
	oo_STRUCT (Speaker_UpperTeeth, upperTeeth)
	oo_STRUCT (Speaker_Lip, lowerLip)
	oo_STRUCT (Speaker_Lip, upperLip)

	/* In the nasal cavity. */

	oo_STRUCT (Speaker_Nose, nose)

oo_END_CLASS (Speaker)
#undef ooSTRUCT

/* End of file Speaker_def.h */
