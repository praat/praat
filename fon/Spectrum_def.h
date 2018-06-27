/* Spectrum_def.h
 *
 * Copyright (C) 2002-2005,2011,2015-2018 Paul Boersma
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


#define ooSTRUCT Spectrum
oo_DEFINE_CLASS (Spectrum, Matrix)

	#if oo_READING
		oo_VERSION_UNTIL (1)
			for (integer i = 1; i <= our nx; i ++)
				our z [2] [i] = - our z [2] [i];
		oo_VERSION_END
	#endif

	#if oo_DECLARING
		void v_info ()
			override;
		int v_domainQuantity ()
			override { return MelderQuantity_FREQUENCY_HERTZ; }
		double v_getValueAtSample (integer isamp, integer which, int units)
			override;
	#endif

oo_END_CLASS (Spectrum)
#undef ooSTRUCT


/* End of file Spectrum_def.h */
