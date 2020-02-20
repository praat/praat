#ifndef _RealTier_and_FunctionTerms_h_
#define _RealTier_and_FunctionTerms_h_
/* RealTier_and_FunctionTerms.h
 *
 * Copyright (C) 2020 David Weenink
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

#include "ChebyshevTerms.h"
#include "LegendreTerms.h"
#include "RealTier.h"

void FunctionTerms_RealTier_fit (FunctionTerms me, RealTier thee, INTVEC freezeCoefficients, double tol, int ic, autoCovariance *c);

autoPolynomial RealTier_to_Polynomial (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm);

autoLegendreTerms RealTier_to_LegendreTerms (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm);

autoChebyshevSeries RealTier_to_ChebyshevSeries (RealTier me, integer degree, double tol, int ic, autoCovariance *cvm);

#endif /* _RealTier_and_FunctionTerms_h_ */
