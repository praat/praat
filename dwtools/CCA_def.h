/* CCA_def.h
 * 
 * Copyright (C) 1993-2003 David Weenink
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
 */

#define ooSTRUCT CCA
oo_DEFINE_CLASS (CCA, Data)

	oo_LONG (numberOfCoefficients)
	oo_LONG (numberOfObservations)
	oo_OBJECT (Strings, yLabels)
	oo_OBJECT (Strings, xLabels)
	oo_OBJECT(Eigen, y)
	oo_OBJECT(Eigen, x)

oo_END_CLASS (CCA)
#undef ooSTRUCT

/* End of file CCA_def.h */
