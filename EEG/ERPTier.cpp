/* ERPTier.cpp
 *
 * Copyright (C) 2011-2012,2014,2015,2016,2017 Paul Boersma
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

#include "ERPTier.h"

#include "oo_DESTROY.h"
#include "ERPTier_def.h"
#include "oo_COPY.h"
#include "ERPTier_def.h"
#include "oo_EQUAL.h"
#include "ERPTier_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ERPTier_def.h"
#include "oo_WRITE_TEXT.h"
#include "ERPTier_def.h"
#include "oo_READ_TEXT.h"
#include "ERPTier_def.h"
#include "oo_WRITE_BINARY.h"
#include "ERPTier_def.h"
#include "oo_READ_BINARY.h"
#include "ERPTier_def.h"
#include "oo_DESCRIPTION.h"
#include "ERPTier_def.h"

/***** ERPPoint *****/

Thing_implement (ERPPoint, AnyPoint, 0);

/***** ERPTier *****/

Thing_implement (ERPTier, AnyTier, 0);

integer ERPTier_getChannelNumber (ERPTier me, const char32 *channelName) {
	for (integer ichan = 1; ichan <= my numberOfChannels; ichan ++) {
		if (Melder_equ (my channelNames [ichan], channelName)) {
			return ichan;
		}
	}
	return 0;
}

double ERPTier_getMean (ERPTier me, integer pointNumber, integer channelNumber, double tmin, double tmax) {
	if (pointNumber < 1 || pointNumber > my points.size) return undefined;
	if (channelNumber < 1 || channelNumber > my numberOfChannels) return undefined;
	ERPPoint point = my points.at [pointNumber];
	return Vector_getMean (point -> erp.get(), tmin, tmax, channelNumber);
}

double ERPTier_getMean (ERPTier me, integer pointNumber, const char32 *channelName, double tmin, double tmax) {
	return ERPTier_getMean (me, pointNumber, ERPTier_getChannelNumber (me, channelName), tmin, tmax);
}

static autoERPTier EEG_PointProcess_to_ERPTier (EEG me, PointProcess events, double fromTime, double toTime) {
	try {
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.get(), fromTime, toTime);
		thy numberOfChannels = my numberOfChannels - EEG_getNumberOfExtraSensors (me);
		Melder_assert (thy numberOfChannels > 0);
		thy channelNames = NUMvector <char32 *> (1, thy numberOfChannels);
		for (integer ichan = 1; ichan <= thy numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_dup (my channelNames [ichan]);
		}
		integer numberOfEvents = events -> nt;
		double soundDuration = toTime - fromTime;
		double samplingPeriod = my sound -> dx;
		integer numberOfSamples = Melder_ifloor (soundDuration / samplingPeriod) + 1;
		if (numberOfSamples < 1)
			Melder_throw (U"Time window too short.");
		double midTime = 0.5 * (fromTime + toTime);
		double soundPhysicalDuration = numberOfSamples * samplingPeriod;
		double firstTime = midTime - 0.5 * soundPhysicalDuration + 0.5 * samplingPeriod;   // distribute the samples evenly over the time domain
		for (integer ievent = 1; ievent <= numberOfEvents; ievent ++) {
			double eegEventTime = events -> t [ievent];
			autoERPPoint event = Thing_new (ERPPoint);
			event -> number = eegEventTime;
			event -> erp = Sound_create (thy numberOfChannels, fromTime, toTime, numberOfSamples, samplingPeriod, firstTime);
			double erpEventTime = 0.0;
			double eegSample = 1 + (eegEventTime - my sound -> x1) / samplingPeriod;
			double erpSample = 1 + (erpEventTime - firstTime) / samplingPeriod;
			integer sampleDifference = Melder_iround (eegSample - erpSample);
			for (integer ichannel = 1; ichannel <= thy numberOfChannels; ichannel ++) {
				for (integer isample = 1; isample <= numberOfSamples; isample ++) {
					integer jsample = isample + sampleDifference;
					event -> erp -> z [ichannel] [isample] = jsample < 1 || jsample > my sound -> nx ? 0.0 : my sound -> z [ichannel] [jsample];
				}
			}
			thy points. addItem_move (event.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": ERP analysis not performed.");
	}
}

autoERPTier EEG_to_ERPTier_bit (EEG me, double fromTime, double toTime, int markerBit) {
	try {
		autoPointProcess events = TextGrid_getStartingPoints (my textgrid.get(), markerBit, kMelder_string::EQUAL_TO, U"1");
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.get(), fromTime, toTime);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": ERPTier not created.");
	}
}

static autoPointProcess TextGrid_getStartingPoints_multiNumeric (TextGrid me, uint16_t number) {
	try {
		autoPointProcess thee;
		int numberOfBits = my tiers->size;
		for (int ibit = 0; ibit < numberOfBits; ibit ++) {
			(void) TextGrid_checkSpecifiedTierIsIntervalTier (me, ibit + 1);
			if (number & (1 << ibit)) {
				autoPointProcess bitEvents = TextGrid_getStartingPoints (me, ibit + 1, kMelder_string::EQUAL_TO, U"1");
				if (thee) {
					thee = PointProcesses_intersection (thee.get(), bitEvents.get());
				} else {
					thee = bitEvents.move();
				}
			}
		}
		for (int ibit = 0; ibit < numberOfBits; ibit ++) {
			autoPointProcess bitEvents = TextGrid_getStartingPoints (me, ibit + 1, kMelder_string::EQUAL_TO, U"1");
			if (! (number & (1 << ibit))) {
				if (thee) {
					thee = PointProcesses_difference (thee.get(), bitEvents.get());
				} else {
					thee = PointProcess_create (my xmin, my xmax, 10);
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": starting points not converted to PointProcess.");
	}
}

autoERPTier EEG_to_ERPTier_marker (EEG me, double fromTime, double toTime, uint16_t marker) {
	try {
		autoPointProcess events = TextGrid_getStartingPoints_multiNumeric (my textgrid.get(), marker);
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.get(), fromTime, toTime);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": ERPTier not created.");
	}
}

autoERPTier EEG_to_ERPTier_triggers (EEG me, double fromTime, double toTime,
	kMelder_string which, const char32 *criterion)
{
	try {
		autoPointProcess events = TextGrid_getPoints (my textgrid.get(), 2, which, criterion);
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.get(), fromTime, toTime);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": ERPTier not created.");
	}
}

autoERPTier EEG_to_ERPTier_triggers_preceded (EEG me, double fromTime, double toTime,
	kMelder_string which, const char32 *criterion,
	kMelder_string precededBy, const char32 *criterion_precededBy)
{
	try {
		autoPointProcess events = TextGrid_getPoints_preceded (my textgrid.get(), 2,
			which, criterion, precededBy, criterion_precededBy);
		autoERPTier thee = EEG_PointProcess_to_ERPTier (me, events.get(), fromTime, toTime);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": ERPTier not created.");
	}
}

void ERPTier_subtractBaseline (ERPTier me, double tmin, double tmax) {
	integer numberOfEvents = my points.size;
	if (numberOfEvents < 1)
		return;   // nothing to do
	ERPPoint firstEvent = my points.at [1];
	integer numberOfChannels = firstEvent -> erp -> ny;
	integer numberOfSamples = firstEvent -> erp -> nx;
	for (integer ievent = 1; ievent <= numberOfEvents; ievent ++) {
		ERPPoint event = my points.at [ievent];
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double mean = Vector_getMean (event -> erp.get(), tmin, tmax, ichannel);
			double *channel = event -> erp -> z [ichannel];
			for (integer isample = 1; isample <= numberOfSamples; isample ++) {
				channel [isample] -= mean;
			}
		}
	}
}

void ERPTier_rejectArtefacts (ERPTier me, double threshold) {
	integer numberOfEvents = my points.size;
	if (numberOfEvents < 1)
		return;   // nothing to do
	ERPPoint firstEvent = my points.at [1];
	integer numberOfChannels = firstEvent -> erp -> ny;
	integer numberOfSamples = firstEvent -> erp -> nx;
	if (numberOfSamples < 1)
		return;   // nothing to do
	for (integer ievent = numberOfEvents; ievent >= 1; ievent --) {   // cycle down because of removal
		ERPPoint event = my points.at [ievent];
		double minimum = event -> erp -> z [1] [1];
		double maximum = minimum;
		for (integer ichannel = 1; ichannel <= (numberOfChannels & ~ 15); ichannel ++) {
			double *channel = event -> erp -> z [ichannel];
			for (integer isample = 1; isample <= numberOfSamples; isample ++) {
				double value = channel [isample];
				if (value < minimum) minimum = value;
				if (value > maximum) maximum = value;
			}
		}
		if (minimum < - threshold || maximum > threshold) {
			my points. removeItem (ievent);
		}
	}
}

autoERP ERPTier_extractERP (ERPTier me, integer eventNumber) {
	try {
		integer numberOfEvents = my points.size;
		if (numberOfEvents < 1)
			Melder_throw (U"No events.");
		ERPTier_checkEventNumber (me, eventNumber);
		ERPPoint event = my points.at [eventNumber];
		integer numberOfChannels = event -> erp -> ny;
		integer numberOfSamples = event -> erp -> nx;
		autoERP thee = Thing_new (ERP);
		event -> erp -> structSound :: v_copy (thee.get());
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double *oldChannel = event -> erp -> z [ichannel];
			double *newChannel = thy z [ichannel];
			for (integer isample = 1; isample <= numberOfSamples; isample ++) {
				newChannel [isample] = oldChannel [isample];
			}
		}
		thy channelNames = NUMvector <char32 *> (1, thy ny);
		for (integer ichan = 1; ichan <= thy ny; ichan ++) {
			thy channelNames [ichan] = Melder_dup (my channelNames [ichan]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": ERP not extracted.");
	}
}

autoERP ERPTier_to_ERP_mean (ERPTier me) {
	try {
		integer numberOfEvents = my points.size;
		if (numberOfEvents < 1)
			Melder_throw (U"No events.");
		ERPPoint firstEvent = my points.at [1];
		integer numberOfChannels = firstEvent -> erp -> ny;
		integer numberOfSamples = firstEvent -> erp -> nx;
		autoERP mean = Thing_new (ERP);
		firstEvent -> erp -> structSound :: v_copy (mean.get());
		for (integer ievent = 2; ievent <= numberOfEvents; ievent ++) {
			ERPPoint event = my points.at [ievent];
			for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
				double *erpChannel = event -> erp -> z [ichannel];
				double *meanChannel = mean -> z [ichannel];
				for (integer isample = 1; isample <= numberOfSamples; isample ++) {
					meanChannel [isample] += erpChannel [isample];
				}
			}
		}
		double factor = 1.0 / numberOfEvents;
		for (integer ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double *meanChannel = mean -> z [ichannel];
			for (integer isample = 1; isample <= numberOfSamples; isample ++) {
				meanChannel [isample] *= factor;
			}
		}
		mean -> channelNames = NUMvector <char32 *> (1, mean -> ny);
		for (integer ichan = 1; ichan <= mean -> ny; ichan ++) {
			mean -> channelNames [ichan] = Melder_dup (my channelNames [ichan]);
		}
		return mean;
	} catch (MelderError) {
		Melder_throw (me, U": mean not computed.");
	}
}

autoERPTier ERPTier_extractEventsWhereColumn_number (ERPTier me, Table table, integer columnNumber, kMelder_number which, double criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (table, columnNumber);
		Table_numericize_Assert (table, columnNumber);   // extraction should work even if cells are not defined
		if (my points.size != table -> rows.size)
			Melder_throw (me, U" & ", table, U": the number of rows in the table (", table -> rows.size,
				U") doesn't match the number of events (", my points.size, U").");
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.get(), my xmin, my xmax);
		thy numberOfChannels = my numberOfChannels;
		thy channelNames = NUMvector <char32 *> (1, thy numberOfChannels);
		for (integer ichan = 1; ichan <= thy numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_dup (my channelNames [ichan]);
		}
		for (integer ievent = 1; ievent <= my points.size; ievent ++) {
			ERPPoint oldEvent = my points.at [ievent];
			TableRow row = table -> rows.at [ievent];
			if (Melder_numberMatchesCriterion (row -> cells [columnNumber]. number, which, criterion)) {
				autoERPPoint newEvent = Data_copy (oldEvent);
				thy points. addItem_move (newEvent.move());
			}
		}
		if (thy points.size == 0) {
			Melder_warning (U"No event matches criterion.");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": events not extracted.");
	}
}

autoERPTier ERPTier_extractEventsWhereColumn_string (ERPTier me, Table table,
	integer columnNumber, kMelder_string which, const char32 *criterion)
{
	try {
		Table_checkSpecifiedColumnNumberWithinRange (table, columnNumber);
		if (my points.size != table -> rows.size)
			Melder_throw (me, U" & ", table, U": the number of rows in the table (", table -> rows.size,
				U") doesn't match the number of events (", my points.size, U").");
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.get(), my xmin, my xmax);
		thy numberOfChannels = my numberOfChannels;
		thy channelNames = NUMvector <char32 *> (1, thy numberOfChannels);
		for (integer ichan = 1; ichan <= thy numberOfChannels; ichan ++) {
			thy channelNames [ichan] = Melder_dup (my channelNames [ichan]);
		}
		for (integer ievent = 1; ievent <= my points.size; ievent ++) {
			ERPPoint oldEvent = my points.at [ievent];
			TableRow row = table -> rows.at [ievent];
			if (Melder_stringMatchesCriterion (row -> cells [columnNumber]. string, which, criterion)) {
				autoERPPoint newEvent = Data_copy (oldEvent);
				thy points. addItem_move (newEvent.move());
			}
		}
		if (thy points.size == 0) {
			Melder_warning (U"No event matches criterion.");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": events not extracted.");
	}
}

/* End of file ERPTier.cpp */
