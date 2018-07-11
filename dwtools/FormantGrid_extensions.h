#ifndef _FormantGrid_extensions_h_
#define _FormantGrid_extensions_h_
/* FormantGrid_extensions.h
 *
 * Copyright (C) 2009-2017 David Weenink
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
 djmw 20090310
 djmw 20110307 Latest modification
*/

#include "FormantGrid.h"
#include "Graphics.h"

void FormantGrid_draw (FormantGrid me, Graphics g, double xmin, double xmax, double ymin, double ymax, bool bandwidths, bool garnish, conststring32 method);

void FormantGrid_removeFormantAndBandwidthTiers (FormantGrid me, integer position);
void FormantGrid_addFormantAndBandwidthTiers (FormantGrid me, integer position);

#endif /* _FormantGrid_extensions_h_ */
