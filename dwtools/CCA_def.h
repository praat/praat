/* CCA_def.h
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

/*
 djmw 2001
 djmw 20020423 GPL header
 djmw 20031214 Added x/yLabels
 djmw 20060529 Added object version numbers.
 djmw 20080122 float -> double
 */

#define ooSTRUCT CCA
oo_DEFINE_CLASS (CCA, Data)

	oo_LONG (numberOfCoefficients)
	oo_LONG (numberOfObservations)
	oo_OBJECT (Strings, 0, yLabels)
	oo_OBJECT (Strings, 0, xLabels)
	oo_OBJECT(Eigen, 0, y)
	oo_OBJECT(Eigen, 0, x)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (CCA)
#undef ooSTRUCT

/* End of file CCA_def.h */
