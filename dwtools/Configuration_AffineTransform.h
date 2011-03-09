#ifndef _Configuration_AffineTransform_h_
#define _Configuration_AffineTransform_h_
/* Configuration_AffineTransform.c
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
 djmw 20020315 GPL header
 djmw 20110307 Latest modification
 */

#ifndef _Configuration_h_
	#include "Configuration.h"
#endif
#ifndef _AffineTransform_h_
	#include "AffineTransform.h"
#endif


#ifdef __cplusplus
	extern "C" {
#endif

AffineTransform Configurations_to_AffineTransform_congruence (Configuration me,
	Configuration thee, long maximumNumberOfIterations, double tolerance);

Configuration Configuration_and_AffineTransform_to_Configuration 
	(Configuration me, thou);

#ifdef __cplusplus
	}
#endif

#endif /* _Configuration_AffineTransform_h_ */
