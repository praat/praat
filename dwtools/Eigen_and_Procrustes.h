#ifndef _Eigen_and_Procrustes_h_
#define _Eigen_and_Procrustes_h_

/* Eigen_and_Procrustes.h
 * Copyright (C) 2004-2011 David Weenink
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
 * djmw 20041025 Initial version
 * djmw 20110307 Latest modification 
 */
 
#include "Eigen.h"
#include "Procrustes.h"

#ifdef __cplusplus
	extern "C" {
#endif

Procrustes Eigens_to_Procrustes (I, thou, long evec_from, long evec_to);

#ifdef __cplusplus
	}
#endif

#endif /* _Eigen_and_Procrustes_h_ */
