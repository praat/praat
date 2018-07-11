#ifndef _ERPTier_h_
#define _ERPTier_h_
/* ERPTier.h
 *
 * Copyright (C) 2011,2012,2014-2018 Paul Boersma
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

#include "EEG.h"
#include "ERP.h"

#include "ERPTier_def.h"

integer ERPTier_getChannelNumber (ERPTier me, conststring32 channelName);
static inline void ERPTier_checkEventNumber (ERPTier me, integer eventNumber) {
	if (eventNumber < 1)
		Melder_throw (U"The specified event number is ", eventNumber, U" but should have been positive.");
	if (eventNumber > my points.size)
		Melder_throw (U"The specified event number (", eventNumber, U") exceeds the number of events (", my points.size, U").");
}
double ERPTier_getMean (ERPTier me, integer pointNumber, integer channelNumber, double tmin, double tmax);
double ERPTier_getMean (ERPTier me, integer pointNumber, conststring32 channelName, double tmin, double tmax);
void ERPTier_subtractBaseline (ERPTier me, double tmin, double tmax);
void ERPTier_rejectArtefacts (ERPTier me, double threshold);
autoERP ERPTier_extractERP (ERPTier me, integer pointNumber);
autoERP ERPTier_to_ERP_mean (ERPTier me);
autoERPTier ERPTier_extractEventsWhereColumn_number (ERPTier me, Table table, integer columnNumber, kMelder_number which, double criterion);
autoERPTier ERPTier_extractEventsWhereColumn_string (ERPTier me, Table table, integer columnNumber, kMelder_string which, conststring32 criterion);

autoERPTier EEG_to_ERPTier_bit (EEG me, double fromTime, double toTime, int markerBit);
autoERPTier EEG_to_ERPTier_marker (EEG me, double fromTime, double toTime, uint16 marker);
autoERPTier EEG_to_ERPTier_triggers (EEG me, double fromTime, double toTime,
	kMelder_string which, conststring32 criterion);
autoERPTier EEG_to_ERPTier_triggers_preceded (EEG me, double fromTime, double toTime,
	kMelder_string which, conststring32 criterion,
	kMelder_string precededBy, conststring32 criterion_precededBy);

/* End of file ERPTier.h */
#endif
