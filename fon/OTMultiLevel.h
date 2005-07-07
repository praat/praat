#ifndef _OTMultiLevel_h_
#define _OTMultiLevel_h_
/* OTMultiLevel.h
 *
 * Copyright (C) 2005 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2005/06/11
 */

#ifndef _Data_h_
	#include "Data.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif
#ifndef _PairDistribution_h_
	#include "PairDistribution.h"
#endif
#ifndef _Distributions_h_
	#include "Distributions.h"
#endif

#include "OTMultiLevel_def.h"

#define OTMultiLevel_methods Data_methods
oo_CLASS_CREATE (OTMultiLevel, Data)

void OTMultiLevel_checkIndex (OTMultiLevel me);

void OTMultiLevel_sort (OTMultiLevel me);
/* Low level: meant to maintain the invariant
 *      my constraints [my index [i]]. disharmony >= my constraints [my index [i+1]]. disharmony
 * Therefore, call after every direct assignment to the 'disharmony' attribute.
 * Tied constraints should not exist.
 */

void OTMultiLevel_newDisharmonies (OTMultiLevel me, double evaluationNoise);

/* End of file OTMultiLevel.h */
#endif
