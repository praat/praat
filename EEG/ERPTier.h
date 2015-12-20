#ifndef _ERPTier_h_
#define _ERPTier_h_
/* ERPTier.h
 *
 * Copyright (C) 2011,2014,2015 Paul Boersma
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
oo_CLASS_CREATE (ERPTier, AnyTier);


long ERPTier_getChannelNumber (ERPTier me, const char32 *channelName);
static inline void ERPTier_checkEventNumber (ERPTier me, long eventNumber) {
	if (eventNumber < 1)
		Melder_throw (U"The specified event number is ", eventNumber, U" but should have been positive.");
	if (eventNumber > my points.size())
		Melder_throw (U"The specified event number (", eventNumber, U") exceeds the number of events (", my points.size(), U").");
}
double ERPTier_getMean (ERPTier me, long pointNumber, long channelNumber, double tmin, double tmax);
double ERPTier_getMean (ERPTier me, long pointNumber, const char32 *channelName, double tmin, double tmax);
void ERPTier_subtractBaseline (ERPTier me, double tmin, double tmax);
void ERPTier_rejectArtefacts (ERPTier me, double threshold);
autoERP ERPTier_extractERP (ERPTier me, long pointNumber);
autoERP ERPTier_to_ERP_mean (ERPTier me);
autoERPTier ERPTier_extractEventsWhereColumn_number (ERPTier me, Table table, long columnNumber, int which_Melder_NUMBER, double criterion);
autoERPTier ERPTier_extractEventsWhereColumn_string (ERPTier me, Table table, long columnNumber, int which_Melder_STRING, const char32 *criterion);

autoERPTier EEG_to_ERPTier_bit (EEG me, double fromTime, double toTime, int markerBit);
autoERPTier EEG_to_ERPTier_marker (EEG me, double fromTime, double toTime, uint16 marker);
autoERPTier EEG_to_ERPTier_triggers (EEG me, double fromTime, double toTime,
	int which_Melder_STRING, const char32 *criterion);
autoERPTier EEG_to_ERPTier_triggers_preceded (EEG me, double fromTime, double toTime,
	int which_Melder_STRING, const char32 *criterion,
	int which_Melder_STRING_precededBy, const char32 *criterion_precededBy);

/* End of file ERPTier.h */
#endif
