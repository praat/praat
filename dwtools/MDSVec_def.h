/* MDSVec_def.h
 *
 * Copyright (C) 2018 David Weenink
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

#define ooSTRUCT MDSVec
oo_DEFINE_CLASS (MDSVec, Daata)

	oo_INTEGER (numberOfPoints)
	oo_INTEGER (numberOfProximities)
	oo_VEC (proximity, numberOfProximities)
	oo_INTVEC (rowIndex, numberOfProximities)
	oo_INTVEC (columnIndex, numberOfProximities)
	
	#if oo_DECLARING
	void v_info () noexcept
		override;
	#endif
oo_END_CLASS (MDSVec)
#undef ooSTRUCT

/* End of file MDSVec_def.h */
