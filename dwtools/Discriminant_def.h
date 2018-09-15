/* Discriminant_def.h
 *
 * Copyright (C) 1993-2018 David Weenink
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


#define ooSTRUCT Discriminant
oo_DEFINE_CLASS (Discriminant, Daata)
	oo_FROM (1)
		oo_OBJECT (Eigen, 0, eigen)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (1)
			eigen = Thing_new (Eigen);
			oo_INTEGER (eigen -> numberOfEigenvalues)
			oo_INTEGER (eigen -> dimension)
			oo_VEC (eigen -> eigenvalues, eigen -> numberOfEigenvalues)
			oo_MAT (eigen -> eigenvectors, eigen -> numberOfEigenvalues, eigen -> dimension)
		oo_VERSION_END
	#endif

	oo_INTEGER (numberOfGroups)
	oo_OBJECT (SSCPList, 0, groups)
	oo_OBJECT (SSCP, 0, total)
	oo_VEC (aprioriProbabilities, numberOfGroups)
	oo_MAT (costs, numberOfGroups, numberOfGroups)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (Discriminant)	
#undef ooSTRUCT

/* End of file Discriminant_def.h */	
