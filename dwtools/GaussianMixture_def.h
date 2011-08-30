/* GaussianMixture_def.h
 *
 * Copyright (C) 2010 David Weenink
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

#define ooSTRUCT GaussianMixture
oo_DEFINE_CLASS (GaussianMixture, Data)

	oo_LONG (numberOfComponents)
	oo_LONG (dimension)
	oo_DOUBLE_VECTOR (mixingProbabilities, numberOfComponents)
	oo_COLLECTION (Ordered, covariances, Covariance, 0)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS(GaussianMixture)
#undef ooSTRUCT

/* End of file GaussianMixture_def.h */
