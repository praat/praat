/* Discriminant_def.h
 *
 * Copyright (C) 1993-2008 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#define ooSTRUCT Discriminant
oo_DEFINE_CLASS (Discriminant, Daata)
	oo_FROM (1)
		oo_AUTO_OBJECT (Eigen, 0, eigen)
	oo_ENDFROM

	#if oo_READING
		if (formatVersion < 1) {
			eigen = Thing_new (Eigen);
			oo_LONG (eigen -> numberOfEigenvalues)
			oo_LONG (eigen -> dimension)
			oo_DOUBLE_VECTOR (eigen -> eigenvalues, eigen -> numberOfEigenvalues)
			oo_DOUBLE_MATRIX (eigen -> eigenvectors, eigen -> numberOfEigenvalues, eigen -> dimension)
		}
	#endif
	
	oo_LONG (numberOfGroups)
	oo_AUTO_OBJECT (SSCPList, 0, groups)
	oo_AUTO_OBJECT (SSCP, 0, total)
	oo_DOUBLE_VECTOR (aprioriProbabilities, numberOfGroups)
	oo_DOUBLE_MATRIX (costs, numberOfGroups, numberOfGroups)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (Discriminant)	
#undef ooSTRUCT

/* End of file Discriminant_def.h */	
