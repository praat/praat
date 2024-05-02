/* WarpedLPC.cpp
 *
 * Copyright (C) 2024 David Weenink
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

#include "WarpedLPC.h"
#include "melder.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "WarpedLPC_def.h"
#include "oo_COPY.h"
#include "WarpedLPC_def.h"
#include "oo_EQUAL.h"
#include "WarpedLPC_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "WarpedLPC_def.h"
#include "oo_WRITE_TEXT.h"
#include "WarpedLPC_def.h"
#include "oo_WRITE_BINARY.h"
#include "WarpedLPC_def.h"
#include "oo_READ_TEXT.h"
#include "WarpedLPC_def.h"
#include "oo_READ_BINARY.h"
#include "WarpedLPC_def.h"
#include "oo_DESCRIPTION.h"
#include "WarpedLPC_def.h"

#include "enums_getText.h"
#undef _WarpedLPC_enums_h_
#include "WarpedLPC_enums.h"
#include "enums_getValue.h"
#undef _WarpedLPC_enums_h_
#include "WarpedLPC_enums.h"

Thing_implement (WarpedLPC, LPC, 1);

autoWarpedLPC WarpedLPC_create (double tmin, double tmax, integer nt, double dt, double t1, integer predictionOrder, double samplingPeriod) {
	try {
		autoWarpedLPC me = Thing_new (WarpedLPC);
		LPC_init (me.get(), tmin, tmax, nt, dt, t1, predictionOrder, samplingPeriod);
		return me;
	} catch (MelderError) {
		Melder_throw (U"WarpedLPC not created.");
	}
}


