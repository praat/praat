/* Network.cpp
 *
 * Copyright (C) 2009-2012,2013 Paul Boersma
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
	structData :: v_info ();
	MelderInfo_writeLine (L"Spreading rate: ", Melder_double (d_spreadingRate));
	MelderInfo_writeLine (L"Activity clipping rule: ", kNetwork_activityClippingRule_getText (d_activityClippingRule));
	MelderInfo_writeLine (L"Minimum activity: ", Melder_double (d_minimumActivity));
	MelderInfo_writeLine (L"Maximum activity: ", Melder_double (d_maximumActivity));
	MelderInfo_writeLine (L"Activity leak: ", Melder_double (d_activityLeak));
	MelderInfo_writeLine (L"Learning rate: ", Melder_double (d_learningRate));
	MelderInfo_writeLine (L"Minimum weight: ", Melder_double (d_minimumWeight));
	MelderInfo_writeLine (L"Maximum weight: ", Melder_double (d_maximumWeight));
	MelderInfo_writeLine (L"Weight leak: ", Melder_double (d_weightLeak));
	MelderInfo_writeLine (L"Number of nodes: ", Melder_integer (d_numberOfNodes));
	MelderInfo_writeLine (L"Number of connections: ", Melder_integer (d_numberOfConnections));
}

Thing_implement (Network, Data, 6);

void structNetwork :: f_init (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	d_spreadingRate = spreadingRate;
	d_activityClippingRule = activityClippingRule;
	d_minimumActivity = minimumActivity;
	d_maximumActivity = maximumActivity;
	d_activityLeak = activityLeak;
	d_learningRate = learningRate;
	d_minimumWeight = minimumWeight;
	d_maximumWeight = maximumWeight;
	d_weightLeak = weightLeak;
	d_instar = 0.0;
	d_outstar = 0.0;
	d_xmin = xmin;
	d_xmax = xmax;
	d_ymin = ymin;
	d_ymax = ymax;
	d_numberOfNodes = numberOfNodes;
	d_nodes = NUMvector <structNetworkNode> (1, numberOfNodes);
	d_numberOfConnections = numberOfConnections;
	d_connections = NUMvector <structNetworkConnection> (1, numberOfConnections);
}

Network Network_create (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	try {
		autoNetwork me = Thing_new (Network);
		me -> f_init (spreadingRate, activityClippingRule, minimumActivity, maximumActivity, activityLeak,
			learningRate, minimumWeight, maximumWeight, weightLeak,
			xmin, xmax, ymin, ymax, numberOfNodes, numberOfConnections);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Network not created.");
	}
}

double structNetwork :: f_getActivity (long nodeNumber) {
	try {
		if (nodeNumber <= 0 || nodeNumber > d_numberOfNodes)
			Melder_throw (this, ": node number (", nodeNumber, ") out of the range 1..", d_numberOfNodes, ".");
		return d_nodes [nodeNumber]. activity;
	} catch (MelderError) {
		Melder_throw (this, ": activity not gotten.");
	}
}

void structNetwork :: f_setActivity (long nodeNumber, double activity) {
	try {
		if (nodeNumber <= 0 || nodeNumber > d_numberOfNodes)
			Melder_throw (this, ": node number (", nodeNumber, ") out of the range 1..", d_numberOfNodes, ".");
		d_nodes [nodeNumber]. activity = d_nodes [nodeNumber]. excitation = activity;
	} catch (MelderError) {
		Melder_throw (this, ": activity not set.");
	}
}

double structNetwork :: f_getWeight (long connectionNumber) {
	try {
		if (connectionNumber <= 0 || connectionNumber > d_numberOfConnections)
			Melder_throw (this, ": connection number (", connectionNumber, ") out of the range 1..", d_numberOfConnections, ".");
		return d_connections [connectionNumber]. weight;
	} catch (MelderError) {
		Melder_throw (this, ": weight not gotten.");
	}
}

void structNetwork :: f_setWeight (long connectionNumber, double weight) {
	try {
		if (connectionNumber <= 0 || connectionNumber > d_numberOfConnections)
			Melder_throw (this, ": connection number (", connectionNumber, ") out of the range 1..", d_numberOfConnections, ".");
		d_connections [connectionNumber]. weight = weight;
	} catch (MelderError) {
		Melder_throw (this, ": weight not set.");
	}
}

void structNetwork :: f_setClamping (long nodeNumber, bool clamped) {
	try {
		if (nodeNumber <= 0 || nodeNumber > d_numberOfNodes)
			Melder_throw (this, ": node number (", nodeNumber, ") out of the range 1..", d_numberOfNodes, ".");
		d_nodes [nodeNumber]. clamped = clamped;
	} catch (MelderError) {
		Melder_throw (this, ": clamping not set.");
	}
}

void structNetwork :: f_spreadActivities (long numberOfSteps) {
	for (long istep = 1; istep <= numberOfSteps; istep ++) {
		for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
			NetworkNode node = & d_nodes [inode];
			if (! node -> clamped)
				node -> excitation -= d_spreadingRate * d_activityLeak * node -> excitation;
		}
		for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
			NetworkConnection connection = & d_connections [iconn];
			NetworkNode nodeFrom = & d_nodes [connection -> nodeFrom];
			NetworkNode nodeTo = & d_nodes [connection -> nodeTo];
			double shunting = d_connections [iconn]. weight >= 0.0 ? d_shunting : 0.0;   // only for excitatory connections
			if (! nodeFrom -> clamped)
				nodeFrom -> excitation += d_spreadingRate * nodeTo -> activity * (d_connections [iconn]. weight - shunting * nodeFrom -> excitation);
			if (! nodeTo -> clamped)
				nodeTo -> excitation += d_spreadingRate * nodeFrom -> activity * (d_connections [iconn]. weight - shunting * nodeTo -> excitation);
		}
		for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
			NetworkNode node = & d_nodes [inode];
			if (! node -> clamped) {
				switch (d_activityClippingRule) {
					case kNetwork_activityClippingRule_SIGMOID:
						node -> activity = d_minimumActivity +
							(d_maximumActivity - d_minimumActivity) * NUMsigmoid (node -> excitation - 0.5 * (d_minimumActivity + d_maximumActivity));
					break;
					case kNetwork_activityClippingRule_LINEAR:
						if (node -> excitation < d_minimumActivity) {
							node -> activity = d_minimumActivity;
						} else if (node -> excitation > d_maximumActivity) {
							node -> activity = d_maximumActivity;
						} else {
							node -> activity = node -> excitation;
						}
					break;
					case kNetwork_activityClippingRule_TOP_SIGMOID:
						if (node -> excitation <= d_minimumActivity) {
							node -> activity = d_minimumActivity;
						} else {
							node -> activity = d_minimumActivity +
								(d_maximumActivity - d_minimumActivity) * (2.0 * NUMsigmoid (2.0 * (node -> excitation - d_minimumActivity) / (d_maximumActivity - d_minimumActivity)) - 1.0);
							trace ("excitation %f, activity %f", node -> excitation, node -> activity);
						}
					break;
				}
			}
		}
	}
}

void structNetwork :: f_zeroActivities (long nodeMin, long nodeMax) {
	if (d_numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = d_numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > d_numberOfNodes) nodeMax = d_numberOfNodes;
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		d_nodes [inode]. activity = d_nodes [inode]. excitation = 0.0;
	}
}

void structNetwork :: f_normalizeActivities (long nodeMin, long nodeMax) {
	if (d_numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = d_numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > d_numberOfNodes) nodeMax = d_numberOfNodes;
	if (nodeMax < nodeMin) return;
	double sum = 0.0;
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		sum += d_nodes [inode]. activity;
	}
	double average = sum / (nodeMax - nodeMin + 1);
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		d_nodes [inode]. activity -= average;
	}	
}

void structNetwork :: f_updateWeights () {
	for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
		NetworkConnection connection = & d_connections [iconn];
		NetworkNode nodeFrom = & d_nodes [connection -> nodeFrom];
		NetworkNode nodeTo = & d_nodes [connection -> nodeTo];
		connection -> weight += connection -> plasticity * d_learningRate *
			(nodeFrom -> activity * nodeTo -> activity - (d_instar * nodeTo -> activity + d_outstar * nodeFrom -> activity + d_weightLeak) * connection -> weight);
		if (connection -> weight < d_minimumWeight) connection -> weight = d_minimumWeight;
		else if (connection -> weight > d_maximumWeight) connection -> weight = d_maximumWeight;
	}
}

void structNetwork :: f_normalizeWeights (long nodeMin, long nodeMax, long nodeFromMin, long nodeFromMax, double newSum) {
	if (d_numberOfNodes < 1) return;
	if (nodeMax == 0) { nodeMin = 1; nodeMax = d_numberOfNodes; }
	if (nodeMin < 1) nodeMin = 1;
	if (nodeMax > d_numberOfNodes) nodeMax = d_numberOfNodes;
	if (nodeMax < nodeMin) return;
	for (long inode = nodeMin; inode <= nodeMax; inode ++) {
		double sum = 0.0;
		for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
			NetworkConnection connection = & d_connections [iconn];
			if (connection -> nodeTo == inode && connection -> nodeFrom >= nodeFromMin && connection -> nodeFrom <= nodeFromMax) {
				sum += connection -> weight;
			}
		}
		if (sum != 0.0) {
			double factor = newSum / sum;
			for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
				NetworkConnection connection = & d_connections [iconn];
				if (connection -> nodeTo == inode && connection -> nodeFrom >= nodeFromMin && connection -> nodeFrom <= nodeFromMax) {
					connection -> weight *= factor;
				}
			}
		}
	}
}

Network Network_create_rectangle (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
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
		for (long inode = 1; inode <= my d_numberOfNodes; inode ++) {
			NetworkNode node = & my d_nodes [inode];
			node -> x = (inode - 1) % numberOfColumns + 0.5;
			node -> y = (inode - 1) / numberOfColumns + 0.5;
			node -> clamped = bottomRowClamped && inode <= numberOfColumns;
			node -> activity = NUMrandomUniform (my d_minimumActivity, my d_maximumActivity);
		}
		/*
		 * Define connections.
		 */
		long iconn = 0;
		for (long irow = 1; irow <= numberOfRows; irow ++) {
			for (long icol = 1; icol <= numberOfColumns - 1; icol ++) {
				NetworkConnection conn = & my d_connections [++ iconn];
				conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
				conn -> nodeTo = conn -> nodeFrom + 1;
				conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
				conn -> plasticity = 1.0;
			}
		}
		for (long irow = 1; irow <= numberOfRows - 1; irow ++) {
			for (long icol = 1; icol <= numberOfColumns; icol ++) {
				NetworkConnection conn = & my d_connections [++ iconn];
				conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
				conn -> nodeTo = conn -> nodeFrom + numberOfColumns;
				conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
				conn -> plasticity = 1.0;
			}
		}
		Melder_assert (iconn == my d_numberOfConnections);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Rectangular network not created.");
	}
}

Network Network_create_rectangle_vertical (double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
	double minimumActivity, double maximumActivity, double activityLeak,
	double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
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
		for (long inode = 1; inode <= my d_numberOfNodes; inode ++) {
			NetworkNode node = & my d_nodes [inode];
			node -> x = (inode - 1) % numberOfColumns + 0.5;
			node -> y = (inode - 1) / numberOfColumns + 0.5;
			node -> clamped = bottomRowClamped && inode <= numberOfColumns;
			node -> activity = NUMrandomUniform (my d_minimumActivity, my d_maximumActivity);
		}
		/*
		 * Define connections.
		 */
		long iconn = 0;
		for (long icol = 1; icol <= numberOfColumns; icol ++) {
			for (long jcol = 1; jcol <= numberOfColumns; jcol ++) {
				for (long irow = 1; irow <= numberOfRows - 1; irow ++) {
					NetworkConnection conn = & my d_connections [++ iconn];
					conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
					conn -> nodeTo = irow * numberOfColumns + jcol;
					conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
					conn -> plasticity = 1.0;
				}
			}
		}
		Melder_assert (iconn == my d_numberOfConnections);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Vertical rectangular network not created.");
	}
}

void structNetwork :: f_draw (Graphics graphics, bool colour) {
	double saveLineWidth = Graphics_inqLineWidth (graphics);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, d_xmin, d_xmax, d_ymin, d_ymax);
	if (colour) {
		Graphics_setColour (graphics, Graphics_SILVER);
		Graphics_fillRectangle (graphics, d_xmin, d_xmax, d_ymin, d_ymax);
	}
	/*
	 * Draw connections.
	 */
	for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
		NetworkConnection conn = & d_connections [iconn];
		if (fabs (conn -> weight) >= 0.01) {
			NetworkNode nodeFrom = & d_nodes [conn -> nodeFrom];
			NetworkNode nodeTo = & d_nodes [conn -> nodeTo];
			Graphics_setLineWidth (graphics, fabs (conn -> weight) * 6.0);
			Graphics_setColour (graphics, conn -> weight < 0.0 ? (colour ? Graphics_WHITE : Graphics_SILVER) : Graphics_BLACK);
			Graphics_line (graphics, nodeFrom -> x, nodeFrom -> y, nodeTo -> x, nodeTo -> y);
		}
	}
	Graphics_setLineWidth (graphics, 1.0);
	/*
	 * Draw the backgrounds of the nodes.
	 */
	for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
		NetworkNode node = & d_nodes [inode];
		Graphics_setColour (graphics, colour ? Graphics_SILVER : Graphics_WHITE);
		Graphics_fillCircle_mm (graphics, node -> x, node -> y, 5.0);
	}
	/*
	 * Draw the edges of the nodes.
	 */
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setLineWidth (graphics, 2.0);
	for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
		NetworkNode node = & d_nodes [inode];
		Graphics_setLineType (graphics, node -> clamped ? Graphics_DRAWN : Graphics_DOTTED);
		Graphics_circle_mm (graphics, node -> x, node -> y, 5.2);
	}
	/*
	 * Draw the activities of the nodes.
	 */
	for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
		NetworkNode node = & d_nodes [inode];
		double activity = fabs (node -> activity);
		if (activity >= 1.0) activity = sqrt (activity);
		double diameter = activity * 5.0;
		if (diameter != 0.0) {
			Graphics_setColour (graphics,
				colour ? ( node -> activity < 0.0 ? Graphics_BLUE : Graphics_RED )
				: ( node -> activity < 0.0 ? Graphics_SILVER : Graphics_BLACK));
			Graphics_fillCircle_mm (graphics, node -> x, node -> y, diameter);
		}
	}
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setLineWidth (graphics, saveLineWidth);
	Graphics_setLineType (graphics, Graphics_DRAWN);
	Graphics_unsetInner (graphics);
}

void structNetwork :: f_addNode (double x, double y, double activity, bool clamped) {
	try {
		NUMvector_append (& d_nodes, 1, & d_numberOfNodes);
		d_nodes [d_numberOfNodes]. x = x;
		d_nodes [d_numberOfNodes]. y = y;
		d_nodes [d_numberOfNodes]. activity = d_nodes [d_numberOfNodes]. excitation = activity;
		d_nodes [d_numberOfNodes]. clamped = clamped;
	} catch (MelderError) {
		Melder_throw (this, ": node not added.");
	}
}

void structNetwork :: f_addConnection (long nodeFrom, long nodeTo, double weight, double plasticity) {
	try {
		NUMvector_append (& d_connections, 1, & d_numberOfConnections);
		d_connections [d_numberOfConnections]. nodeFrom = nodeFrom;
		d_connections [d_numberOfConnections]. nodeTo = nodeTo;
		d_connections [d_numberOfConnections]. weight = weight;
		d_connections [d_numberOfConnections]. plasticity = plasticity;
	} catch (MelderError) {
		Melder_throw (this, ": connection not added.");
	}
}

void structNetwork :: f_setInstar (double instar) {
	d_instar = instar;
}

void structNetwork :: f_setOutstar (double outstar) {
	d_outstar = outstar;
}

void structNetwork :: f_setWeightLeak (double weightLeak) {
	d_weightLeak = weightLeak;
}

void structNetwork :: f_setActivityLeak (double activityLeak) {
	d_activityLeak = activityLeak;
	f_zeroActivities (0, 0);
}

void structNetwork :: f_setShunting (double shunting) {
	d_shunting = shunting;
	f_zeroActivities (0, 0);
}

void structNetwork :: f_setActivityClippingRule (enum kNetwork_activityClippingRule activityClippingRule) {
	d_activityClippingRule = activityClippingRule;
	f_zeroActivities (0, 0);
}

Table structNetwork :: f_nodes_downto_Table (long fromNodeNumber, long toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals)
{
	try {
		if (fromNodeNumber < 1) fromNodeNumber = 1;
		if (toNodeNumber > d_numberOfNodes) toNodeNumber = d_numberOfNodes;
		if (fromNodeNumber > toNodeNumber)
			fromNodeNumber = 1, toNodeNumber = d_numberOfNodes;
		long numberOfNodes = toNodeNumber - fromNodeNumber + 1;
		Melder_assert (numberOfNodes >= 1);
		autoTable thee = Table_createWithoutColumnNames (numberOfNodes,
			includeNodeNumbers + includeX + includeY + includeClamped + includeActivity + includeExcitation);
		long icol = 0;
		if (includeNodeNumbers) Table_setColumnLabel (thee.peek(), ++ icol, L"node");
		if (includeX)           Table_setColumnLabel (thee.peek(), ++ icol, L"x");
		if (includeY)           Table_setColumnLabel (thee.peek(), ++ icol, L"y");
		if (includeClamped)     Table_setColumnLabel (thee.peek(), ++ icol, L"clamped");
		if (includeActivity)    Table_setColumnLabel (thee.peek(), ++ icol, L"activity");
		if (includeExcitation)  Table_setColumnLabel (thee.peek(), ++ icol, L"excitation");
		for (long inode = fromNodeNumber; inode <= toNodeNumber; inode ++) {
			NetworkNode node = & d_nodes [inode];
			icol = 0;
			if (includeNodeNumbers) Table_setNumericValue (thee.peek(), inode, ++ icol, inode);
			if (includeX)           Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> x, positionDecimals));
			if (includeY)           Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> y, positionDecimals));
			if (includeClamped)     Table_setNumericValue (thee.peek(), inode, ++ icol, node -> clamped);
			if (includeActivity)    Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> activity,   activityDecimals));
			if (includeExcitation)  Table_setStringValue  (thee.peek(), inode, ++ icol, Melder_fixed (node -> excitation, activityDecimals));
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (this, ": not converted to Table.");
	}
}

void structNetwork :: f_listNodes (long fromNodeNumber, long toNodeNumber,
	bool includeNodeNumbers,
	bool includeX, bool includeY, int positionDecimals,
	bool includeClamped,
	bool includeActivity, bool includeExcitation, int activityDecimals)
{
	try {
		autoTable table = f_nodes_downto_Table (fromNodeNumber, toNodeNumber, includeNodeNumbers,
			includeX, includeY, positionDecimals, includeClamped, includeActivity, includeExcitation, activityDecimals);
		Table_list (table.peek(), false);
	} catch (MelderError) {
		Melder_throw (this, ": not listed.");
	}
}

/* End of file Network.cpp */
