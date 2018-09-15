/* GaussianMixture_def.h
 *
 * Copyright (C) 2010-2018 David Weenink, 2015 Paul Boersma
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

#define ooSTRUCT GaussianMixture
oo_DEFINE_CLASS (GaussianMixture, Daata)

	oo_INTEGER (numberOfComponents)
	oo_INTEGER (dimension)
	oo_VEC (mixingProbabilities, numberOfComponents)
	oo_OBJECT (CovarianceList, 0, covariances)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS(GaussianMixture)
#undef ooSTRUCT

/* End of file GaussianMixture_def.h */
