#ifndef _IntensityTierEditor_h_
#define _IntensityTierEditor_h_
/* IntensityTierEditor.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/10/16
 */

#ifndef _RealTierEditor_h_
	#include "RealTierEditor.h"
#endif
#ifndef _IntensityTier_h_
	#include "IntensityTier.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

typedef struct structIntensityTierEditor *IntensityTierEditor;

IntensityTierEditor IntensityTierEditor_create (Widget parent, const char *title,
	IntensityTier intensity, Sound sound, int ownSound);
/*
	'sound' may be NULL.
*/

/* End of file IntensityTierEditor.h */
#endif
