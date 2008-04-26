/* FormantGrid_def.h
 *
 * Copyright (C) 2008 Paul Boersma & David Weenink
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
 * pb 2008/04/24 created
 */

#define ooSTRUCT FormantGrid
oo_DEFINE_CLASS (FormantGrid, Function)

	oo_COLLECTION (Ordered, formants, RealTier, 0)
	oo_COLLECTION (Ordered, bandwidths, RealTier, 0)

oo_END_CLASS (FormantGrid)
#undef ooSTRUCT

/* End of file FormantGrid_def.h */
