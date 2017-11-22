#ifndef _Configuration_and_Procrustes_h_
#define _Configuration_and_Procrustes_h_
/* Configuration_and_Procrustes.h
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20020813 GPL header
 djmw 20110307 Latest modification.
*/

#ifndef _Configuration_h_
	#include "Configuration.h"
#endif
#ifndef _Procrustes_h_
	#include "Procrustes.h"
#endif

autoProcrustes Configurations_to_Procrustes (Configuration me, Configuration thee, bool orthogonal);

#endif /* _Configuration_and_Procrustes_h_ */
