/* ERP.cpp
 *
 * Copyright (C) 2011-2012,2013,2014,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
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

integer ERP_getChannelNumber (ERP me, conststring32 channelName) {
	for (integer ichan = 1; ichan <= my ny; ichan ++) {
		if (Melder_equ (my channelNames [ichan].get(), channelName))
			return ichan;
	}
	return 0;
}

void ERP_drawChannel_number (ERP me, Graphics graphics, integer channelNumber, double tmin, double tmax, double vmin, double vmax, bool garnish) {
	if (channelNumber < 1 || channelNumber > my ny) return;
	/*
	 * Automatic domain.
	 */
	if (tmin == tmax) {
		tmin = my xmin;
		tmax = my xmax;
	}
	/*
	 * Domain expressed in sample numbers.
	 */
	integer ixmin, ixmax;
	Matrix_getWindowSamplesX (me, tmin, tmax, & ixmin, & ixmax);
	/*
	 * Automatic vertical range.
	 */
	if (vmin == vmax) {
		Matrix_getWindowExtrema (me, ixmin, ixmax, channelNumber, channelNumber, & vmin, & vmax);
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
	Graphics_function (graphics, & my z [channelNumber] [0], ixmin, ixmax, Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
	Graphics_unsetInner (graphics);
	if (garnish) {
		Graphics_drawInnerBox (graphics);
		Graphics_textTop (graphics, true, Melder_cat (U"Channel ", my channelNames [channelNumber].get()));
		Graphics_textBottom (graphics, true, U"Time (s)");
		Graphics_marksBottom (graphics, 2, true, true, false);
		if (0.0 > tmin && 0.0 < tmax)
			Graphics_markBottom (graphics, 0.0, true, true, true, nullptr);
		Graphics_markLeft (graphics, vmin, true, true, false, nullptr);
		Graphics_markLeft (graphics, vmax, true, true, false, nullptr);
			Graphics_markBottom (graphics, 0.0, true, true, true, nullptr);
		if (vmin != 0.0 && vmax != 0.0 && (vmin > 0.0) != (vmax > 0.0)) {
			Graphics_markLeft (graphics, 0.0, true, true, true, nullptr);
		}
	}

}

void ERP_drawChannel_name (ERP me, Graphics graphics, conststring32 channelName, double tmin, double tmax, double vmin, double vmax, bool garnish) {
	ERP_drawChannel_number (me, graphics, ERP_getChannelNumber (me, channelName), tmin, tmax, vmin, vmax, garnish);
}

autoTable ERP_tabulate (ERP me, bool includeSampleNumbers, bool includeTime, int timeDecimals, int voltageDecimals, int units) {
	double voltageScaling = 1.0;
	conststring32 unitText = U"(V)";
	if (units == 2) {
		voltageDecimals -= 6;
		voltageScaling = 1000000.0;
		unitText = U"(uV)";
	}
	try {
		autoTable thee = Table_createWithoutColumnNames (my nx, includeSampleNumbers + includeTime + my ny);
		integer icol = 0;
		if (includeSampleNumbers) Table_setColumnLabel (thee.get(), ++ icol, U"sample");
		if (includeTime) Table_setColumnLabel (thee.get(), ++ icol, U"time(s)");
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			Table_setColumnLabel (thee.get(), ++ icol, Melder_cat (my channelNames [ichan].get(), unitText));
		}
		for (integer isamp = 1; isamp <= my nx; isamp ++) {
			icol = 0;
			if (includeSampleNumbers) Table_setNumericValue (thee.get(), isamp, ++ icol, isamp);
			if (includeTime) Table_setStringValue (thee.get(), isamp, ++ icol, Melder_fixed (my x1 + (isamp - 1) * my dx, timeDecimals));
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				Table_setStringValue (thee.get(), isamp, ++ icol, Melder_fixed (voltageScaling * my z [ichan] [isamp], voltageDecimals));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Table.");
	}
}

autoSound ERP_downto_Sound (ERP me) {
	try {
		autoSound thee = Thing_new (Sound);
		my structSound :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

/* End of file ERP.cpp */
