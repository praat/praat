/* Network.cpp
 *
 * Copyright (C) 2009,2011-2017 Paul Boersma
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

/*
 * pb 2009/02/27 created
 * pb 2009/03/05 setClamping
 * pb 2009/05/14 zeroActivities, normalizeActivities
 * pb 2009/06/11 connection plasticities
 * pb 2011/03/29 C++
 * pb 2012/03/18 more weight update rules: instar, outstar, inoutstar
 * pb 2012/04/19 more activation clipping rules: linear
 * pb 2012/06/02 activation spreading rules: sudden, gradual
 */

#include "Network.h"

#include "oo_DESTROY.h"
#include "Network_def.h"
#include "oo_COPY.h"
#include "Network_def.h"
#include "oo_EQUAL.h"
#include "Network_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Network_def.h"
#include "oo_WRITE_TEXT.h"
#include "Network_def.h"
#include "oo_READ_TEXT.h"
#include "Network_def.h"
#include "oo_WRITE_BINARY.h"
#include "Network_def.h"
#include "oo_READ_BINARY.h"
#include "Network_def.h"
#include "oo_DESCRIPTION.h"
#include "Network_def.h"

#include "enums_getText.h"
#include "Network_enums.h"
#include "enums_getValue.h"
#include "Network_enums.h"

void structNetwork :: v_info ()
{
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Spreading rate: ", our spreadingRate);
	MelderInfo_writeLine (U"Activity clipping rule: ", kNetwork_activityClippingRule_getText (our activityClippingRule));
	MelderInfo_writeLine (U"Minimum activity: ", our minimumActivity);
	MelderInfo_writeLine (U"Maximum activity: ", our maximumActivity);
	MelderInfo_writeLine (U"Activity leak: ", our activityLeak);
	MelderInfo_writeLine (U"Learning rate: ", our learningRate);
	MelderInfo_writeLine (U"Minimum weight: ", our minimumWeight);
	MelderInfo_writeLine (U"Maximum weight: ", our maximumWeight);
	MelderInfo_writeLine (U"Weight leak: ", our weightLeak);
	MelderInfo_writeLine (U"Number of nodes: ", our numberOfNodes);
	MelderInfo_writeLine (U"Number of connections: ", our numberOfConnections);
}

Thing_implement (Network, Daata, 6);

void Network_init (Network me, double spreadingRate, kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, integer numberOfNodes, integer numberOfConnections)
{
	my spreadingRate = spreadingRate;
	my activityClippingRule = activityClippingRule;
	my minimumActivity = minimumActivity;
	my maximumActivity = maximumActivity;
	my activityLeak = activityLeak;
	my learningRate = learningRate;
	my minimumWeight = minimumWeight;
	my maximumWeight = maximumWeight;
	my weightLeak = weightLeak;
	my instar = 0.0;
	my outstar = 0.0;
	my xmin = xmin;
	my xmax = xmax;
	my ymin = ymin;
	my ymax = ymax;
	my numberOfNodes = numberOfNodes;
	my nodes = NUMvector <structNetworkNode> (1, numberOfNodes);
	my numberOfConnections = numberOfConnections;
	my connections = NUMvector <structNetworkConnection> (1, numberOfConnections);
}

autoNetwork Network_create (double spreadingRate, kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, integer numberOfNodes, integer numberOfConnections)
{
	try {
		autoNetwork me = Thing_new (Network);
		Network_init (me.get(), spreadingRate, activityClippingRule, minimumActivity, maximumActivity, activityLeak,
			learningRate, minimumWeight, maximumWeight, weightLeak,
			xmin, xmax, ymin, ymax, numberOfNodes, numberOfConnections);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Network not created.");
	}
}

double Network_getActivity (Network me, integer nodeNumber) {
	try {
		if (nodeNumber <= 0 || nodeNumber > my numberOfNodes)
			Melder_throw (me, U": node number (", nodeNumber, U") out of the range 1..", my numberOfNodes, U".");
		return my nodes [nodeNumber]. activity;
	} catch (MelderError) {
		Melder_throw (me, U": activity not gotten.");
	}
}

void Network_setActivity (Network me, integer nodeNumber, double activity) {
	try {
		if (nodeNumber <= 0 || nodeNumber > my numberOfNodes)
			Melder_throw (me, U": node number (", nodeNumber, U") out of the range 1..", my numberOfNodes, U".");
		my nodes [nodeNumber]. activity = my nodes [nodeNumber]. excitation = activity;
	} catch (MelderError) {
		Melder_throw (me, U": activity not set.");
	}
}

double Network_getWeight (Network me, integer connectionNumber) {
	try {
		if (connectionNumber <= 0 || connectionNumber > my numberOfConnections)
			Melder_throw (me, U": connection number (", connectionNumber, U") out of the range 1..", my numberOfConnections, U".");
		return my connections [connectionNumber]. weight;
	} catch (MelderError) {
		Melder_throw (me, U": weight not gotten.");
	}
}

void Network_setWeight (Network me, integer connectionNumber, double weight) {
	try {
		if (connectionNumber <= 0 || connectionNumber > my numberOfConnections)
			Melder_throw (me, U": connection number (", connectionNumber, U") out of the range 1..", my numberOfConnections, U".");
		my connections [connectionNumber]. weight = weight;
	} catch (MelderError) {
		Melder_throw (me, U": weight not set.");
	}
}

void Network_setClamping (Network me, integer nodeNumber, bool clamped) {
	try {
		if (nodeNumber <= 0 || nodeNumber > my numberOfNodes)
			Melder_throw (me, U": node number (", nodeNumber, U") out of the range 1..", my numberOfNodes, U".");
		my nodes [nodeNumber]. clamped = clamped;
	} catch (MelderError) {
		Melder_throw (me, U": clamping not set.");
	}
}

void Network_spreadActivities (Network me, integer numberOfSteps) {
	for (integer istep = 1; istep <= numberOfSteps; istep ++) {
		for (integer inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			if (! node -> clamped)
				node -> excitation -= my spreadingRate * my activityLeak * node -> excitation;
		}
		for (integer iconn = 1; iconn <= my numberOfConnections; iconn ++) {
			NetworkConnection connection = & my connections [iconn];
			NetworkNode nodeFrom = & my nodes [connection -> nodeFrom];
			NetworkNode nodeTo = & my nodes [connection -> nodeTo];
			double shunting = my connections [iconn]. weight >= 0.0 ? my shunting : 0.0;   // only for excitatory connections
			if (! nodeFrom -> clamped)
				nodeFrom -> excitation += my spreadingRate * nodeTo -> activity * (my connections [iconn]. weight - shunting * nodeFrom -> excitation);
			if (! nodeTo -> clamped)
				nodeTo -> excitation += my spreadingRate * nodeFrom -> activity * (my connections [iconn]. weight - shunting * nodeTo -> excitation);
		}
		for (integer inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			if (! node -> clamped) {
				switch (my activityClippingRule) {
					case kNetwork_activityClippingRule::SIGMOID:
						node -> activity = my minimumActivity +
							(my maximumActivity - my minimumActivity) * NUMsigmoid (node -> excitation - 0.5 * (my minimumActivity + my maximumActivity));
					break;
					case kNetwork_activityClippingRule::LINEAR:
						if (node -> excitation < my minimumActivity) {
							node -> activity = my minimumActivity;
						} else if (node -> excitation > my maximumActivity) {
							node -> activity = my maximumActivity;
						} else {
							node -> activity = node -> excitation;
						}
					break;
					case kNetwork_activityClippingRule::TOP_SIGMOID:
						if (node -> excitation <= my minimumActivity) {
							node -> activity = my minimumActivity;
						} else {
							node -> activity = my minimumActivity +
								(my maximumActivity - my minimumActivity) * (2.0 * NUMsigmoid (2.0 * (node -> excitation - my minimumActivity) / (my maximumActivity - my minimumActivity)) - 1.0);
							trace (U"excitation ", node -> excitation, U", activity ", node -> activity);
						}
					break;
				}
			}
		}
	}
}

void Network_zeroActivities (Network me, integer nodeMin, integer nodeMax) {
	if (my numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = my numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > my numberOfNodes) nodeMax = my numberOfNodes;
	for (integer inode = nodeMin; inode <= nodeMax; inode ++) {
		my nodes [inode]. activity = my nodes [inode]. excitation = 0.0;
	}
}

void Network_normalizeActivities (Network me, integer nodeMin, integer nodeMax) {
	if (my numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = my numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > my numberOfNodes) nodeMax = my numberOfNodes;
	if (nodeMax < nodeMin) return;
	real80 sum = 0.0;
	for (integer inode = nodeMin; inode <= nodeMax; inode ++) {
		sum += my nodes [inode]. activity;
	}
	double average = (real) sum / (nodeMax - nodeMin + 1);
	for (integer inode = nodeMin; inode <= nodeMax; inode ++) {
		my nodes [inode]. activity -= average;
	}	
}

void Network_updateWeights (Network me) {
	for (integer iconn = 1; iconn <= my numberOfConnections; iconn ++) {
		NetworkConnection connection = & my connections [iconn];
		NetworkNode nodeFrom = & my nodes [connection -> nodeFrom];
		NetworkNode nodeTo = & my nodes [connection -> nodeTo];
		connection -> weight += connection -> plasticity * my learningRate *
			(nodeFrom -> activity * nodeTo -> activity - (my instar * nodeTo -> activity + my outstar * nodeFrom -> activity + my weightLeak) * connection -> weight);
		if (connection -> weight < my minimumWeight) connection -> weight = my minimumWeight;
		else if (connection -> weight > my maximumWeight) connection -> weight = my maximumWeight;
	}
}

void Network_normalizeWeights (Network me, integer nodeMin, integer nodeMax, integer nodeFromMin, integer nodeFromMax, double newSum) {
	if (my numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = my numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > my numberOfNodes) nodeMax = my numberOfNodes;
	if (nodeMax < nodeMin) return;
	for (integer inode = nodeMin; inode <= nodeMax; inode ++) {
		real80 sum = 0.0;
		for (integer iconn = 1; iconn <= my numberOfConnections; iconn ++) {
			NetworkConnection connection = & my connections [iconn];
			if (connection -> nodeTo == inode && connection -> nodeFrom >= nodeFromMin && connection -> nodeFrom <= nodeFromMax) {
				sum += connection -> weight;
			}
		}
		if (sum != 0.0) {
			double factor = newSum / (real) sum;
			for (integer iconn = 1; iconn <= my numberOfConnections; iconn ++) {
				NetworkConnection connection = & my connections [iconn];
				if (connection -> nodeTo == inode && connection -> nodeFrom >= nodeFromMin && connection -> nodeFrom <= nodeFromMax) {
					connection -> weight *= factor;
				}
			}
		}
	}
}

autoNetwork Network_create_rectangle (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	integer numberOfRows, integer numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	try {
		autoNetwork me = Network_create (spreadingRate, activityClippingRule, minimumActivity, maximumActivity, activityLeak,
			learningRate, minimumWeight, maximumWeight, weightLeak,
			0.0, numberOfColumns, 0.0, numberOfRows, numberOfRows * numberOfColumns,
			numberOfRows * (numberOfColumns - 1) + numberOfColumns * (numberOfRows - 1));
		/*
		 * Define nodes.
		 */
		for (integer inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			node -> x = (inode - 1) % numberOfColumns + 0.5;
			node -> y = (inode - 1) / numberOfColumns + 0.5;
			node -> clamped = bottomRowClamped && inode <= numberOfColumns;
			node -> activity = NUMrandomUniform (my minimumActivity, my maximumActivity);
		}
		/*
		 * Define connections.
		 */
		integer iconn = 0;
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			for (integer icol = 1; icol <= numberOfColumns - 1; icol ++) {
				NetworkConnection conn = & my connections [++ iconn];
				conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
				conn -> nodeTo = conn -> nodeFrom + 1;
				conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
				conn -> plasticity = 1.0;
			}
		}
		for (integer irow = 1; irow <= numberOfRows - 1; irow ++) {
			for (integer icol = 1; icol <= numberOfColumns; icol ++) {
				NetworkConnection conn = & my connections [++ iconn];
				conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
				conn -> nodeTo = conn -> nodeFrom + numberOfColumns;
				conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
				conn -> plasticity = 1.0;
			}
		}
		Melder_assert (iconn == my numberOfConnections);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Rectangular network not created.");
	}
}

autoNetwork Network_create_rectangle_vertical (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	integer numberOfRows, integer numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	try {
		autoNetwork me = Network_create (spreadingRate, activityClippingRule, minimumActivity, maximumActivity, activityLeak,
			learningRate, minimumWeight, maximumWeight, weightLeak,
			0.0, numberOfColumns, 0.0, numberOfRows, numberOfRows * numberOfColumns,
			numberOfColumns * numberOfColumns * (numberOfRows - 1));
		/*
		 * Define nodes.
		 */
		for (integer inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			node -> x = (inode - 1) % numberOfColumns + 0.5;
			node -> y = (inode - 1) / numberOfColumns + 0.5;
			node -> clamped = bottomRowClamped && inode <= numberOfColumns;
			node -> activity = NUMrandomUniform (my minimumActivity, my maximumActivity);
		}
		/*
		 * Define connections.
		 */
		integer iconn = 0;
		for (integer icol = 1; icol <= numberOfColumns; icol ++) {
			for (integer jcol = 1; jcol <= numberOfColumns; jcol ++) {
				for (integer irow = 1; irow <= numberOfRows - 1; irow ++) {
					NetworkConnection conn = & my connections [++ iconn];
					conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
					conn -> nodeTo = irow * numberOfColumns + jcol;
					conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
					conn -> plasticity = 1.0;
				}
			}
		}
		Melder_assert (iconn == my numberOfConnections);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Vertical rectangular network not created.");
	}
}

void Network_draw (Network me, Graphics graphics, bool useColour) {
	double saveLineWidth = Graphics_inqLineWidth (graphics);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, my xmin, my xmax, my ymin, my ymax);
	if (useColour) {
		Graphics_setColour (graphics, Graphics_SILVER);
		Graphics_fillRectangle (graphics, my xmin, my xmax, my ymin, my ymax);
	}
	/*
	 * Draw connections.
	 */
	for (integer iconn = 1; iconn <= my numberOfConnections; iconn ++) {
		NetworkConnection conn = & my connections [iconn];
		if (fabs (conn -> weight) >= 0.01) {
			NetworkNode nodeFrom = & my nodes [conn -> nodeFrom];
			NetworkNode nodeTo = & my nodes [conn -> nodeTo];
			Graphics_setLineWidth (graphics, fabs (conn -> weight) * 6.0);
			Graphics_setColour (graphics, conn -> weight < 0.0 ? ( useColour ? Graphics_WHITE : Graphics_SILVER ) : Graphics_BLACK);
			Graphics_line (graphics, nodeFrom -> x, nodeFrom -> y, nodeTo -> x, nodeTo -> y);
		}
	}
	Graphics_setLineWidth (graphics, 1.0);
	/*
	 * Draw the backgrounds of the nodes.
	 */
	for (integer inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		Graphics_setColour (graphics, useColour ? Graphics_SILVER : Graphics_WHITE);
		Graphics_fillCircle_mm (graphics, node -> x, node -> y, 5.0);
	}
	/*
	 * Draw the edges of the nodes.
	 */
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setLineWidth (graphics, 2.0);
	for (integer inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		Graphics_setLineType (graphics, node -> clamped ? Graphics_DRAWN : Graphics_DOTTED);
		Graphics_circle_mm (graphics, node -> x, node -> y, 5.2);
	}
	/*
	 * Draw the activities of the nodes.
	 */
	for (integer inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		double activity = fabs (node -> activity);
		if (activity >= 1.0) activity = sqrt (activity);
		double diameter = activity * 5.0;
		if (diameter != 0.0) {
			Graphics_setColour (graphics,
				useColour ? ( node -> activity < 0.0 ? Graphics_BLUE : Graphics_RED )
				: ( node -> activity < 0.0 ? Graphics_SILVER : Graphics_BLACK));
			Graphics_fillCircle_mm (graphics, node -> x, node -> y, diameter);
		}
	}
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setLineWidth (graphics, saveLineWidth);
	Graphics_setLineType (graphics, Graphics_DRAWN);
	Graphics_unsetInner (graphics);
}

void Network_addNode (Network me, double x, double y, double activity, bool clamped) {
	try {
		integer numberOfNodes = my numberOfNodes;
		NUMvector_append (& my nodes, 1, & numberOfNodes);
		my numberOfNodes = numberOfNodes;
		my nodes [my numberOfNodes]. x = x;
		my nodes [my numberOfNodes]. y = y;
		my nodes [my numberOfNodes]. activity = my nodes [my numberOfNodes]. excitation = activity;
		my nodes [my numberOfNodes]. clamped = clamped;
	} catch (MelderError) {
		Melder_throw (me, U": node not added.");
	}
}

void Network_addConnection (Network me, integer nodeFrom, integer nodeTo, double weight, double plasticity) {
	try {
		integer numberOfConnections = my numberOfConnections;
		NUMvector_append (& my connections, 1, & numberOfConnections);
		my numberOfConnections = numberOfConnections;
		my connections [my numberOfConnections]. nodeFrom = nodeFrom;
		my connections [my numberOfConnections]. nodeTo = nodeTo;
		my connections [my numberOfConnections]. weight = weight;
		my connections [my numberOfConnections]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (me, U": connection not added.");
	}
}

void Network_setInstar (Network me, double instar) {
	my instar = instar;
}

void Network_setOutstar (Network me, double outstar) {
	my outstar = outstar;
}

void Network_setWeightLeak (Network me, double weightLeak) {
	my weightLeak = weightLeak;
}

void Network_setActivityLeak (Network me, double activityLeak) {
	my activityLeak = activityLeak;
	Network_zeroActivities (me, 0, 0);
}

void Network_setShunting (Network me, double shunting) {
	my shunting = shunting;
	Network_zeroActivities (me, 0, 0);
}

void Network_setActivityClippingRule (Network me, enum kNetwork_activityClippingRule activityClippingRule) {
	my activityClippingRule = activityClippingRule;
	Network_zeroActivities (me, 0, 0);
}

autoTable Network_nodes_downto_Table (Network me, integer fromNodeNumber, integer toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals)
{
	try {
		if (fromNodeNumber < 1) fromNodeNumber = 1;
		if (toNodeNumber > my numberOfNodes) toNodeNumber = my numberOfNodes;
		if (fromNodeNumber > toNodeNumber)
			fromNodeNumber = 1, toNodeNumber = my numberOfNodes;
		integer numberOfNodes = toNodeNumber - fromNodeNumber + 1;
		Melder_assert (numberOfNodes >= 1);
		autoTable thee = Table_createWithoutColumnNames (numberOfNodes,
			includeNodeNumbers + includeX + includeY + includeClamped + includeActivity + includeExcitation);
		integer icol = 0;
		if (includeNodeNumbers) Table_setColumnLabel (thee.get(), ++ icol, U"node");
		if (includeX)           Table_setColumnLabel (thee.get(), ++ icol, U"x");
		if (includeY)           Table_setColumnLabel (thee.get(), ++ icol, U"y");
		if (includeClamped)     Table_setColumnLabel (thee.get(), ++ icol, U"clamped");
		if (includeActivity)    Table_setColumnLabel (thee.get(), ++ icol, U"activity");
		if (includeExcitation)  Table_setColumnLabel (thee.get(), ++ icol, U"excitation");
		for (integer inode = fromNodeNumber; inode <= toNodeNumber; inode ++) {
			NetworkNode node = & my nodes [inode];
			icol = 0;
			if (includeNodeNumbers) Table_setNumericValue (thee.get(), inode, ++ icol, inode);
			if (includeX)           Table_setStringValue  (thee.get(), inode, ++ icol, Melder_fixed (node -> x, positionDecimals));
			if (includeY)           Table_setStringValue  (thee.get(), inode, ++ icol, Melder_fixed (node -> y, positionDecimals));
			if (includeClamped)     Table_setNumericValue (thee.get(), inode, ++ icol, node -> clamped);
			if (includeActivity)    Table_setStringValue  (thee.get(), inode, ++ icol, Melder_fixed (node -> activity,   activityDecimals));
			if (includeExcitation)  Table_setStringValue  (thee.get(), inode, ++ icol, Melder_fixed (node -> excitation, activityDecimals));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Table.");
	}
}

void Network_listNodes (Network me, integer fromNodeNumber, integer toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals)
{
	try {
		autoTable table = Network_nodes_downto_Table (me, fromNodeNumber, toNodeNumber, includeNodeNumbers,
			includeX, includeY, positionDecimals, includeClamped, includeActivity, includeExcitation, activityDecimals);
		Table_list (table.get(), false);
	} catch (MelderError) {
		Melder_throw (me, U": not listed.");
	}
}

/* End of file Network.cpp */
