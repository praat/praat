#ifndef _CCA_and_Correlation_h_
#define _CCA_and_Correlation_h_
/* CCA_and_Correlation.h
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
 djmw 20020516 GPL header
 */

#ifndef _CCA_h_
	#include "CCA.h"
#endif
#ifndef _SSCP_h_
	#include "SSCP.h"
#endif

TableOfReal CCA_and_Correlation_factorLoadings (CCA me, Correlation thee);
/*
	Get the canonical factor loadings (also structure correlation coefficients),
	the correlation of a canonical variable with an original variable.
*/

#endif /* _CCA_and_Correlation_h_ */
