#ifndef _FormantGrid_extensions_h_
#define _FormantGrid_extensions_h_
/* FormantGrid_extensions.h
 *
 * Copyright (C) 2009-2011 David Weenink
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
 djmw 20090310
 djmw 20110307 Latest modification
*/

#include "FormantGrid.h"
#include "Graphics.h"

#ifdef __cplusplus
	extern "C" {
#endif

void FormantGrid_draw (FormantGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool bandwidths, bool garnish, const wchar_t *method);

void FormantGrid_removeFormantAndBandwidthTiers (FormantGrid me, int position);
void FormantGrid_addFormantAndBandwidthTiers (FormantGrid me, int position);

#ifdef __cplusplus
	}
#endif

#endif /* _FormantGrid_extensions_h_ */
