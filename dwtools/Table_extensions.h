#ifndef _Table_extensions_h_
#define _Table_extensions_h_
/* Table_extensions.h
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
 djmw 20020411 initial GPL
 djmw 20110105 Latest modification.
*/

#include "TableOfReal.h"
#include "Collection.h"
#include "Pattern.h"
#include "Categories.h"
#include "Strings.h"
#include "SSCP.h"
#include "Table.h"

#ifdef __cplusplus
	extern "C" {
#endif

Table Table_createFromPetersonBarneyData ();
Table Table_createFromPolsVanNieropData ();
Table Table_createFromWeeninkData ();

void Table_drawScatterPlotWithConfidenceIntervals (Table me, Graphics g, long xcolumn, long ycolumn,
	double xmin, double xmax, double ymin, double ymax, long xci_min, long xci_max,
	long yci_min, long yci_max, double bar_mm, int garnish);

#ifdef __cplusplus
	}
#endif

#endif // _Table_extensions_h_
