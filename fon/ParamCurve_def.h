/* ParamCurve_def.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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


#define ooSTRUCT ParamCurve
oo_DEFINE_CLASS (ParamCurve, Function)

	oo_OBJECT (Sound, 2, x)
	oo_OBJECT (Sound, 2, y)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (ParamCurve)
#undef ooSTRUCT


/* End of file ParamCurve_def.h */
