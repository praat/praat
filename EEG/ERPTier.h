#ifndef _ERPTier_h_
#define _ERPTier_h_
/* ERPTier.h
 *
 * Copyright (C) 2011 Paul Boersma
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

#include "EEG.h"
#include "ERP.h"

#include "ERPTier_def.h"
oo_CLASS_CREATE (ERPPoint, AnyPoint);
oo_CLASS_CREATE (ERPTier, Function);

ERPTier EEG_to_ERPTier (EEG me, double fromTime, double toTime, int markerBit);

/* End of file ERPTier.h */
#endif
