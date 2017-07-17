/* FeatureWeights_def.h
 *
 * Copyright (C) 2007-2009 Ola Söder, 2011 Paul Boersma
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


#define ooSTRUCT FeatureWeights
oo_DEFINE_CLASS (FeatureWeights, Daata)

	oo_AUTO_OBJECT (TableOfReal, 0, fweights)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (FeatureWeights)
#undef ooSTRUCT


/* End of file FeatureWeights_def.h */
