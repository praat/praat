/* ERPTier.cpp
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

Thing_implement (ERPTier, Function, 0);

void structERPTier :: v_shiftX (double xfrom, double xto) {
	ERPTier_Parent :: v_shiftX (xfrom, xto);
	//if (d_sound    != NULL)  Function_shiftXTo (d_sound,    xfrom, xto);
	//if (d_textgrid != NULL)  Function_shiftXTo (d_textgrid, xfrom, xto);
}

void structERPTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	ERPTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	//if (d_sound    != NULL)  d_sound    -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	//if (d_textgrid != NULL)  d_textgrid -> v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
}

long structERPTier :: f_getChannelNumber (const wchar_t *channelName) {
	for (long ichan = 1; ichan <= d_numberOfChannels; ichan ++) {
		if (Melder_wcsequ (d_channelNames [ichan], channelName)) {
			return ichan;
		}
	}
	return 0;
}

double structERPTier :: f_getMean (long pointNumber, long channelNumber, double tmin, double tmax) {
	if (pointNumber < 1 || pointNumber > d_events -> size) return NUMundefined;
	if (channelNumber < 1 || channelNumber > d_numberOfChannels) return NUMundefined;
	ERPPoint point = f_peekEvent (pointNumber);
	return Vector_getMean (point -> d_erp, tmin, tmax, channelNumber);
}

double structERPTier :: f_getMean (long pointNumber, const wchar_t *channelName, double tmin, double tmax) {
	return f_getMean (pointNumber, f_getChannelNumber (channelName), tmin, tmax);
}

ERPTier EEG_to_ERPTier (EEG me, double fromTime, double toTime, int markerBit) {
	try {
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.peek(), fromTime, toTime);
		thy d_numberOfChannels = my d_numberOfChannels - my f_getNumberOfExtraSensors ();
		thy d_channelNames = NUMvector <wchar_t *> (1, thy d_numberOfChannels);
		for (long ichan = 1; ichan <= thy d_numberOfChannels; ichan ++) {
			thy d_channelNames [ichan] = Melder_wcsdup (my d_channelNames [ichan]);
		}
		autoPointProcess events = TextGrid_getStartingPoints (my d_textgrid, markerBit, kMelder_string_EQUAL_TO, L"1");
		long numberOfEvents = events -> nt;
		thy d_events = SortedSetOfDouble_create ();
		double soundDuration = toTime - fromTime;
		double samplingPeriod = my d_sound -> dx;
		long numberOfSamples = floor (soundDuration / samplingPeriod) + 1;
		if (numberOfSamples < 1)
			Melder_throw (L"Time window too short.");
		double midTime = 0.5 * (fromTime + toTime);
		double soundPhysicalDuration = numberOfSamples * samplingPeriod;
		double firstTime = midTime - 0.5 * soundPhysicalDuration + 0.5 * samplingPeriod;   // distribute the samples evenly over the time domain
		for (long ievent = 1; ievent <= numberOfEvents; ievent ++) {
			double eegEventTime = events -> t [ievent];
			autoERPPoint event = Thing_new (ERPPoint);
			event -> number = eegEventTime;
			event -> d_erp = Sound_create (thy d_numberOfChannels, fromTime, toTime, numberOfSamples, samplingPeriod, firstTime);
			double erpEventTime = 0.0;
			double eegSample = 1 + (eegEventTime - my d_sound -> x1) / samplingPeriod;
			double erpSample = 1 + (erpEventTime - firstTime) / samplingPeriod;
			long sampleDifference = round (eegSample - erpSample);
			for (long ichannel = 1; ichannel <= thy d_numberOfChannels; ichannel ++) {
				for (long isample = 1; isample <= numberOfSamples; isample ++) {
					long jsample = isample + sampleDifference;
					event -> d_erp -> z [ichannel] [isample] = jsample < 1 || jsample > my d_sound -> nx ? 0.0 : my d_sound -> z [ichannel] [jsample];
				}
			}
			Collection_addItem (thy d_events, event.transfer());
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": ERP analysis not performed.");
	}
}

void structERPTier :: f_subtractBaseline (double tmin, double tmax) {
	long numberOfEvents = d_events -> size;
	if (numberOfEvents < 1)
		return;   // nothing to do
	ERPPoint firstEvent = f_peekEvent (1);
	long numberOfChannels = firstEvent -> d_erp -> ny;
	long numberOfSamples = firstEvent -> d_erp -> nx;
	for (long ievent = 1; ievent <= numberOfEvents; ievent ++) {
		ERPPoint event = f_peekEvent (ievent);
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double mean = Vector_getMean (event -> d_erp, tmin, tmax, ichannel);
			double *channel = event -> d_erp -> z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				channel [isample] -= mean;
			}
		}
	}
}

void structERPTier :: f_rejectArtefacts (double threshold) {
	long numberOfEvents = d_events -> size;
	if (numberOfEvents < 1)
		return;   // nothing to do
	ERPPoint firstEvent = f_peekEvent (1);
	long numberOfChannels = firstEvent -> d_erp -> ny;
	long numberOfSamples = firstEvent -> d_erp -> nx;
	if (numberOfSamples < 1)
		return;   // nothing to do
	for (long ievent = numberOfEvents; ievent >= 1; ievent --) {   // cycle down because of removal
		ERPPoint event = f_peekEvent (ievent);
		double minimum = event -> d_erp -> z [1] [1];
		double maximum = minimum;
		for (long ichannel = 1; ichannel <= (numberOfChannels & ~ 15); ichannel ++) {
			double *channel = event -> d_erp -> z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				double value = channel [isample];
				if (value < minimum) minimum = value;
				if (value > maximum) maximum = value;
			}
		}
		if (minimum < - threshold || maximum > threshold) {
			Collection_removeItem (d_events, ievent);
		}
	}
}

ERP structERPTier :: f_extractERP (long eventNumber) {
	try {
		long numberOfEvents = d_events -> size;
		if (numberOfEvents < 1)
			Melder_throw ("No events.");
		f_checkEventNumber (eventNumber);
		ERPPoint event = f_peekEvent (eventNumber);
		long numberOfChannels = event -> d_erp -> ny;
		long numberOfSamples = event -> d_erp -> nx;
		autoERP thee = Thing_new (ERP);
		event -> d_erp -> structSound :: v_copy (thee.peek());
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double *oldChannel = event -> d_erp -> z [ichannel];
			double *newChannel = thy z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				newChannel [isample] = oldChannel [isample];
			}
		}
		thy d_channelNames = NUMvector <wchar_t *> (1, thy ny);
		for (long ichan = 1; ichan <= thy ny; ichan ++) {
			thy d_channelNames [ichan] = Melder_wcsdup (d_channelNames [ichan]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": ERP not extracted.");
	}
}

ERP structERPTier :: f_toERP_mean () {
	try {
		long numberOfEvents = d_events -> size;
		if (numberOfEvents < 1)
			Melder_throw ("No events.");
		ERPPoint firstEvent = f_peekEvent (1);
		long numberOfChannels = firstEvent -> d_erp -> ny;
		long numberOfSamples = firstEvent -> d_erp -> nx;
		autoERP mean = Thing_new (ERP);
		firstEvent -> d_erp -> structSound :: v_copy (mean.peek());
		for (long ievent = 2; ievent <= numberOfEvents; ievent ++) {
			ERPPoint event = f_peekEvent (ievent);
			for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
				double *erpChannel = event -> d_erp -> z [ichannel];
				double *meanChannel = mean -> z [ichannel];
				for (long isample = 1; isample <= numberOfSamples; isample ++) {
					meanChannel [isample] += erpChannel [isample];
				}
			}
		}
		double factor = 1.0 / numberOfEvents;
		for (long ichannel = 1; ichannel <= numberOfChannels; ichannel ++) {
			double *meanChannel = mean -> z [ichannel];
			for (long isample = 1; isample <= numberOfSamples; isample ++) {
				meanChannel [isample] *= factor;
			}
		}
		mean -> d_channelNames = NUMvector <wchar_t *> (1, mean -> ny);
		for (long ichan = 1; ichan <= mean -> ny; ichan ++) {
			mean -> d_channelNames [ichan] = Melder_wcsdup (d_channelNames [ichan]);
		}
		return mean.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": mean not computed.");
	}
}

ERPTier structERPTier :: f_extractEventsWhereColumn_number (Table table, long columnNumber, int which_Melder_NUMBER, double criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (table, columnNumber);
		Table_numericize_Assert (table, columnNumber);   // extraction should work even if cells are not defined
		if (d_events -> size != table -> rows -> size)
			Melder_throw (this, " & ", table, ": the number of rows in the table (", table -> rows -> size, ") doesn't match the number of events (", d_events -> size, ").");
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.peek(), this -> xmin, this -> xmax);
		thy d_numberOfChannels = this -> d_numberOfChannels;
		thy d_channelNames = NUMvector <wchar_t *> (1, thy d_numberOfChannels);
		for (long ichan = 1; ichan <= thy d_numberOfChannels; ichan ++) {
			thy d_channelNames [ichan] = Melder_wcsdup (this -> d_channelNames [ichan]);
		}
		thy d_events = SortedSetOfDouble_create ();
		for (long ievent = 1; ievent <= d_events -> size; ievent ++) {
			ERPPoint oldEvent = this -> f_peekEvent (ievent);
			TableRow row = table -> f_peekRow (ievent);
			if (Melder_numberMatchesCriterion (row -> cells [columnNumber]. number, which_Melder_NUMBER, criterion)) {
				autoERPPoint newEvent = Data_copy (oldEvent);
				Collection_addItem (thy d_events, newEvent.transfer());
			}
		}
		if (thy d_events -> size == 0) {
			Melder_warning ("No event matches criterion.");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": events not extracted.");
	}
}

ERPTier structERPTier :: f_extractEventsWhereColumn_string (Table table, long columnNumber, int which_Melder_STRING, const wchar_t *criterion) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (table, columnNumber);
		if (d_events -> size != table -> rows -> size)
			Melder_throw (this, " & ", table, ": the number of rows in the table (", table -> rows -> size, ") doesn't match the number of events (", d_events -> size, ").");
		autoERPTier thee = Thing_new (ERPTier);
		Function_init (thee.peek(), this -> xmin, this -> xmax);
		thy d_numberOfChannels = this -> d_numberOfChannels;
		thy d_channelNames = NUMvector <wchar_t *> (1, thy d_numberOfChannels);
		for (long ichan = 1; ichan <= thy d_numberOfChannels; ichan ++) {
			thy d_channelNames [ichan] = Melder_wcsdup (this -> d_channelNames [ichan]);
		}
		thy d_events = SortedSetOfDouble_create ();
		for (long ievent = 1; ievent <= d_events -> size; ievent ++) {
			ERPPoint oldEvent = this -> f_peekEvent (ievent);
			TableRow row = table -> f_peekRow (ievent);
			if (Melder_stringMatchesCriterion (row -> cells [columnNumber]. string, which_Melder_STRING, criterion)) {
				autoERPPoint newEvent = Data_copy (oldEvent);
				Collection_addItem (thy d_events, newEvent.transfer());
			}
		}
		if (thy d_events -> size == 0) {
			Melder_warning ("No event matches criterion.");
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": events not extracted.");
	}
}


/* End of file ERPTier.cpp */
