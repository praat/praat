#ifndef _EEG_h_
#define _EEG_h_
/* EEG.h
 *
 * Copyright (C) 2011-2012 Paul Boersma
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

#include "Sound.h"
#include "TextGrid.h"

#include "EEG_def.h"
oo_CLASS_CREATE (EEG, Function);

EEG EEG_create (double tmin, double tmax);

EEG EEG_readFromBdfFile (MelderFile file);

EEG EEGs_concatenate (Collection me);

/* End of file EEG.h */
#endif
