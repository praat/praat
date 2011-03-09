#ifndef _CCA_and_Correlation_h_
#define _CCA_and_Correlation_h_
/* CCA_and_Correlation.h
 * 
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20110307 Latest modification.
 */

#ifndef _CCA_h_
	#include "CCA.h"
#endif
#ifndef _SSCP_h_
	#include "SSCP.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif


TableOfReal CCA_and_Correlation_factorLoadings (CCA me, Correlation thee);
/*
	Get the canonical factor loadings (also structure correlation coefficients),
	the correlation of a canonical variable with an original variable.
*/

double CCA_and_Correlation_getVarianceFraction (CCA me, Correlation thee, int x_or_y, int canonicalVariate_from, int canonicalVariate_to);
/*
	Get the fraction of variance extracted from the dependent/independent set by the canonical variate range.
*/

double CCA_and_Correlation_getRedundancy_sl (CCA me, Correlation thee, int x_or_y, int canonicalVariate_from, int canonicalVariate_to);
/*
	The Stewart-Love redundancy is the fraction of variance extracted by the canonical variate times
	the fraction of shared variance between the canonical variate and the corresponding canonical variate
	of the other set.
	redundancy = VarianceFraction * (canonical correlation)^2
*/

#ifdef __cplusplus
	}
#endif

#endif /* _CCA_and_Correlation_h_ */
