/* VocalTractTier_def.h
 *
 * Copyright (C) 2012 David Weenink
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

#define ooSTRUCT VocalTractPoint
oo_DEFINE_CLASS (VocalTractPoint, AnyPoint)

	oo_OBJECT (VocalTract, 0, d_vocalTract)

oo_END_CLASS (VocalTractPoint)
#undef ooSTRUCT


#define ooSTRUCT VocalTractTier
oo_DEFINE_CLASS (VocalTractTier, Function)

	oo_COLLECTION (SortedSetOfDouble, d_vocalTracts, VocalTractPoint, 0)

oo_END_CLASS (VocalTractTier)
#undef ooSTRUCT


/* End of file VocalTract_def.h */
