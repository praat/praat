#ifndef _CCA_and_Correlation_h_
#define _CCA_and_Correlation_h_
/* CCA_and_Correlation.h
 * 
 * Copyright (C) 1993-2020 David Weenink
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

/*
 djmw 2001
 djmw 20020516 GPL header
 djmw 20110307 Latest modification.
 */

#include "CCA.h"
#include "Correlation.h"

autoTableOfReal CCA_Correlation_factorLoadings (CCA me, Correlation thee);
/*
	Get the canonical factor loadings (also structure correlation coefficients),
	the correlation of a canonical variable with an original variable.
*/

double CCA_Correlation_getVarianceFraction (CCA me, Correlation thee, int x_or_y, integer canonicalVariate_from, integer canonicalVariate_to);
/*
	Get the fraction of variance extracted from the dependent/independent set by the canonical variate range.
*/

double CCA_Correlation_getRedundancy_sl (CCA me, Correlation thee, int x_or_y, integer canonicalVariate_from, integer canonicalVariate_to);
/*
	The Stewart-Love redundancy is the fraction of variance extracted by the canonical variate times
	the fraction of shared variance between the canonical variate and the corresponding canonical variate
	of the other set.
	redundancy = VarianceFraction * (canonical correlation)^2
*/

#endif /* _CCA_and_Correlation_h_ */
