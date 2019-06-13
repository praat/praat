/* NMF_def.h
 *
 * Copyright (C) 2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#define ooSTRUCT NMF
oo_DEFINE_CLASS (NMF, Daata)

	oo_INTEGER (numberOfRows)
	oo_INTEGER (numberOfColumns)
	oo_INTEGER (numberOfFeatures)
	
	oo_MAT (features, numberOfRows, numberOfFeatures)
	oo_MAT (weights, numberOfFeatures, numberOfColumns)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (NMF)
#undef ooSTRUCT


/* End of file NMF_def.h */
