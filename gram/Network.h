#ifndef _Network_h_
#define _Network_h_
/* Network.h
 *
 * Copyright (C) 2009-2012,2013,2014 Paul Boersma
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Table.h"

#include "Network_enums.h"

#include "Network_def.h"

void Network_init (Network me,
	double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections);

autoNetwork Network_create (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections);

autoNetwork Network_create_rectangle (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight);

autoNetwork Network_create_rectangle_vertical (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight);

void Network_addNode (Network me, double x, double y, double activity, bool clamped);
void Network_addConnection (Network me, long fromNodeNumber, long toNodeNumber, double weight, double plasticity);
void Network_draw (Network me, Graphics graphics, bool colour);
double Network_getActivity (Network me, long nodeNumber);
void Network_setActivity (Network me, long nodeNumber, double activity);
double Network_getWeight (Network me, long connectionNumber);
void Network_setWeight (Network me, long connectionNumber, double weight);
void Network_setClamping (Network me, long nodeNumber, bool clamped);
void Network_zeroActivities (Network me, long fromNodeNumber, long toNodeNumber);
void Network_normalizeActivities (Network me, long fromNodeNumber, long toNodeNumber);
void Network_spreadActivities (Network me, long numberOfSteps);
void Network_updateWeights (Network me);
void Network_normalizeWeights (Network me, long nodeMin, long nodeMax, long nodeFromMin, long nodeFromMax, double newSum);
void Network_setInstar (Network me, double instar);
void Network_setOutstar (Network me, double outstar);
void Network_setWeightLeak (Network me, double weightLeak);
void Network_setActivityLeak (Network me, double activityLeak);
void Network_setShunting (Network me, double shunting);
void Network_setActivityClippingRule (Network me, enum kNetwork_activityClippingRule activityClippingRule);
autoTable Network_nodes_downto_Table (Network me, long fromNodeNumber, long toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals);
void Network_listNodes (Network me, long fromNodeNumber, long toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals);

/* End of file Network.h */
#endif
