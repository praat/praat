#ifndef _Configuration_and_Procrustes_h_
#define _Configuration_and_Procrustes_h_
/* Configuration_and_Procrustes.h
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
 djmw 20020813 GPL header
 djmw 20110307 Latest modification.
*/

#ifndef _Configuration_h_
	#include "Configuration.h"
#endif
#ifndef _Procrustes_h_
	#include "Procrustes.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

Procrustes Configurations_to_Procrustes (Configuration me, Configuration thee, int orthogonal);

#ifdef __cplusplus
	}
#endif

#endif /* _Configuration_and_Procrustes_h_ */
