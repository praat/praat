/* ERP.cpp
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

#include "ERP.h"

#include "oo_DESTROY.h"
#include "ERP_def.h"
#include "oo_COPY.h"
#include "ERP_def.h"
#include "oo_EQUAL.h"
#include "ERP_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ERP_def.h"
#include "oo_WRITE_TEXT.h"
#include "ERP_def.h"
#include "oo_READ_TEXT.h"
#include "ERP_def.h"
#include "oo_WRITE_BINARY.h"
#include "ERP_def.h"
#include "oo_READ_BINARY.h"
#include "ERP_def.h"
#include "oo_DESCRIPTION.h"
#include "ERP_def.h"

/********** class ERPTier **********/

Thing_implement (ERP, Sound, 2);

long structERP :: f_getChannelNumber (const wchar_t *channelName) {
	for (long ichan = 1; ichan <= ny; ichan ++) {
		if (Melder_wcsequ (d_channelNames [ichan], channelName)) {
			return ichan;
		}
	}
	return 0;
}

void structERP :: f_draw (Graphics graphics, long channelNumber, double tmin, double tmax, double vmin, double vmax, bool garnish) {
	if (channelNumber < 1 || channelNumber > this -> ny) return;
	/*
	 * Automatic domain.
	 */
	if (tmin == tmax) {
		tmin = this -> xmin;
		tmax = this -> xmax;
	}
	/*
	 * Domain expressed in sample numbers.
	 */
	long ixmin, ixmax;
	Matrix_getWindowSamplesX (this, tmin, tmax, & ixmin, & ixmax);
	/*
	 * Automatic vertical range.
	 */
	if (vmin == vmax) {
		Matrix_getWindowExtrema (this, ixmin, ixmax, channelNumber, channelNumber, & vmin, & vmax);
		if (vmin == vmax) {
			vmin -= 1.0;
			vmax += 1.0;
		}
	}
	/*
	 * Set coordinates for drawing.
	 */
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, tmin, tmax, vmin, vmax);
	Graphics_function (graphics, this -> z [channelNumber], ixmin, ixmax, Matrix_columnToX (this, ixmin), Matrix_columnToX (this, ixmax));
	Graphics_unsetInner (graphics);
	if (garnish) {
		Graphics_drawInnerBox (graphics);
		Graphics_textTop (graphics, true, Melder_wcscat (L"Channel ", d_channelNames [channelNumber]));
		Graphics_textBottom (graphics, true, L"Time (s)");
		Graphics_marksBottom (graphics, 2, true, true, false);
		if (0.0 > tmin && 0.0 < tmax)
			Graphics_markBottom (graphics, 0.0, true, true, true, NULL);
		Graphics_markLeft (graphics, vmin, true, true, false, NULL);
		Graphics_markLeft (graphics, vmax, true, true, false, NULL);
			Graphics_markBottom (graphics, 0.0, true, true, true, NULL);
		if (vmin != 0.0 && vmax != 0.0 && (vmin > 0.0) != (vmax > 0.0)) {
			Graphics_markLeft (graphics, 0.0, true, true, true, NULL);
		}
	}

}

void structERP :: f_draw (Graphics graphics, const wchar_t *channelName, double tmin, double tmax, double vmin, double vmax, bool garnish) {
	f_draw (graphics, f_getChannelNumber (channelName), tmin, tmax, vmin, vmax, garnish);
}

Table structERP :: f_tabulate (bool includeSampleNumbers, bool includeTime, int timeDecimals, int voltageDecimals, int units)
{
	double voltageScaling = 1.0;
	const wchar_t *unitText = L"(V)";
	if (units == 2) {
		voltageDecimals -= 6;
		voltageScaling = 1000000.0;
		unitText = L"(uV)";
	}
	try {
		autoTable thee = Table_createWithoutColumnNames (nx, includeSampleNumbers + includeTime + ny);
		long icol = 0;
		if (includeSampleNumbers) Table_setColumnLabel (thee.peek(), ++ icol, L"sample");
		if (includeTime) Table_setColumnLabel (thee.peek(), ++ icol, L"time(s)");
		for (long ichan = 1; ichan <= ny; ichan ++) {
			Table_setColumnLabel (thee.peek(), ++ icol, Melder_wcscat (d_channelNames [ichan], unitText));
		}
		for (long isamp = 1; isamp <= nx; isamp ++) {
			icol = 0;
			if (includeSampleNumbers) Table_setNumericValue (thee.peek(), isamp, ++ icol, isamp);
			if (includeTime) Table_setStringValue (thee.peek(), isamp, ++ icol, Melder_fixed (x1 + (isamp - 1) * dx, timeDecimals));
			for (long ichan = 1; ichan <= ny; ichan ++) {
				Table_setStringValue (thee.peek(), isamp, ++ icol, Melder_fixed (voltageScaling * z [ichan] [isamp], voltageDecimals));
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": not converted to Table.");
	}
}

Sound structERP :: f_downToSound () {
	try {
		autoSound thee = Thing_new (Sound);
		structSound :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": not converted to Sound.");
	}
}

/* End of file ERP.cpp */
