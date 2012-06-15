/* Network.cpp
 *
 * Copyright (C) 2009-2012 Paul Boersma
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
	MelderInfo_writeLine2 (L"Leak: ", Melder_double (d_leak));
	MelderInfo_writeLine2 (L"Number of nodes: ", Melder_integer (d_numberOfNodes));
	MelderInfo_writeLine2 (L"Number of connections: ", Melder_integer (d_numberOfConnections));
}

Thing_implement (Network, Data, 4);

void structNetwork :: f_init (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	d_minimumActivity = minimumActivity;
	d_maximumActivity = maximumActivity;
	d_spreadingRate = spreadingRate;
	d_selfExcitation = selfExcitation;
	d_minimumWeight = minimumWeight;
	d_maximumWeight = maximumWeight;
	d_learningRate = learningRate;
	d_leak = leak;
	d_xmin = xmin;
	d_xmax = xmax;
	d_ymin = ymin;
	d_ymax = ymax;
	d_numberOfNodes = numberOfNodes;
	d_nodes = NUMvector <structNetworkNode> (1, numberOfNodes);
	d_numberOfConnections = numberOfConnections;
	d_connections = NUMvector <structNetworkConnection> (1, numberOfConnections);
}

Network Network_create (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	try {
		autoNetwork me = Thing_new (Network);
		me -> f_init (minimumActivity, maximumActivity, spreadingRate,
			selfExcitation, minimumWeight, maximumWeight, learningRate, leak,
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
		if (d_activationSpreadingRule == kNetwork_activationSpreadingRule_GRADUAL) {
			for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
				NetworkNode node = & d_nodes [inode];
				if (! node -> clamped)
					node -> excitation += d_spreadingRate * node -> activity * d_selfExcitation;
			}
			for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
				NetworkConnection connection = & d_connections [iconn];
				NetworkNode nodeFrom = & d_nodes [connection -> nodeFrom];
				NetworkNode nodeTo = & d_nodes [connection -> nodeTo];
				if (d_connections [iconn]. weight >= 0.0 && d_shunting > 0.0) {
					/*
					 * Excitatory connection: shunting.
					 */
					if (! nodeFrom -> clamped)
						nodeFrom -> excitation += d_spreadingRate * nodeTo -> activity * (d_connections [iconn]. weight - d_shunting * nodeFrom -> activity);
					if (! nodeTo -> clamped)
						nodeTo -> excitation += d_spreadingRate * nodeFrom -> activity * (d_connections [iconn]. weight - d_shunting * nodeTo -> activity);
				} else {
					/*
					 * Explicit inhibitory connection: additive.
					 */
					if (! nodeFrom -> clamped)
						nodeFrom -> excitation += d_spreadingRate * nodeTo -> activity * d_connections [iconn]. weight;
					if (! nodeTo -> clamped)
						nodeTo -> excitation += d_spreadingRate * nodeFrom -> activity * d_connections [iconn]. weight;
				}
			}
			for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
				NetworkNode node = & d_nodes [inode];
				if (! node -> clamped) {
					node -> activity = node -> excitation;
					switch (d_activationClippingRule) {
						case kNetwork_activationClippingRule_LINEAR:
							if (node -> activity < d_minimumActivity) {
								node -> activity = node -> excitation = d_minimumActivity;
							}
							if (node -> activity > d_maximumActivity) {
								node -> activity = node -> excitation = d_maximumActivity;
							}
						break;
						case kNetwork_activationClippingRule_TOP_SIGMOID:
							if (node -> activity <= d_minimumActivity) {
								node -> activity = node -> excitation = d_minimumActivity;
							} else {
								node -> activity = (d_maximumActivity - d_minimumActivity) * (2.0 * NUMsigmoid (2.0 * (node -> activity - d_minimumActivity)) - 1.0) + d_minimumActivity;
							}
						break;
					}
				}
			}
		} else {
			for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
				NetworkNode node = & d_nodes [inode];
				node -> excitation = node -> activity * d_selfExcitation;
			}
			for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
				NetworkConnection connection = & d_connections [iconn];
				NetworkNode nodeFrom = & d_nodes [connection -> nodeFrom];
				NetworkNode nodeTo = & d_nodes [connection -> nodeTo];
				nodeFrom -> excitation += nodeTo -> activity * d_connections [iconn]. weight;
				nodeTo -> excitation += nodeFrom -> activity * d_connections [iconn]. weight;
			}
			for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
				NetworkNode node = & d_nodes [inode];
				if (! node -> clamped) {
					node -> activity = d_spreadingRate * node -> excitation;
					switch (d_activationClippingRule) {
						case kNetwork_activationClippingRule_SIGMOID:
							node -> activity = (d_maximumActivity - d_minimumActivity) * NUMsigmoid (node -> activity) + d_minimumActivity;
						break;
						case kNetwork_activationClippingRule_LINEAR:
							if (node -> activity < d_minimumActivity) {
								node -> activity = d_minimumActivity;
							}
							if (node -> activity > d_maximumActivity) {
								node -> activity = d_maximumActivity;
							}
						break;
					}
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
		switch (d_weightUpdateRule) {
			case kNetwork_weightUpdateRule_HEBBIAN:
				connection -> weight += connection -> plasticity * d_learningRate *
					nodeFrom -> activity * nodeTo -> activity - d_leak * connection -> weight;
			break;
			case kNetwork_weightUpdateRule_INSTAR:
				connection -> weight += connection -> plasticity * d_learningRate *
					nodeTo -> activity * (nodeFrom -> activity - connection -> weight);
			break;
			case kNetwork_weightUpdateRule_OUTSTAR:
				connection -> weight += connection -> plasticity * d_learningRate *
					nodeFrom -> activity * (nodeTo -> activity - connection -> weight);
			break;
			case kNetwork_weightUpdateRule_INOUTSTAR:
				connection -> weight += connection -> plasticity * d_learningRate *
					(2 * nodeFrom -> activity * nodeTo -> activity - (nodeFrom -> activity + nodeTo -> activity + 2 * d_leak) * connection -> weight);
			break;
		}
		if (connection -> weight < d_minimumWeight) connection -> weight = d_minimumWeight;
		else if (connection -> weight > d_maximumWeight) connection -> weight = d_maximumWeight;
	}
}

Network Network_create_rectangle (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	try {
		autoNetwork me = Network_create (minimumActivity, maximumActivity, spreadingRate,
			selfExcitation, minimumWeight, maximumWeight, learningRate, leak,
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

Network Network_create_rectangle_vertical (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	try {
		autoNetwork me = Network_create (minimumActivity, maximumActivity, spreadingRate,
			selfExcitation, minimumWeight, maximumWeight, learningRate, leak,
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
	Graphics_setColour (graphics, Graphics_SILVER);
	Graphics_fillRectangle (graphics, d_xmin, d_xmax, d_ymin, d_ymax);
	/*
	 * Draw connections.
	 */
	for (long iconn = 1; iconn <= d_numberOfConnections; iconn ++) {
		NetworkConnection conn = & d_connections [iconn];
		if (conn -> weight != 0.0) {
			NetworkNode nodeFrom = & d_nodes [conn -> nodeFrom];
			NetworkNode nodeTo = & d_nodes [conn -> nodeTo];
			Graphics_setLineWidth (graphics, fabs (conn -> weight) * 6.0);
			Graphics_setColour (graphics, conn -> weight < 0.0 ? Graphics_WHITE : Graphics_BLACK);
			Graphics_line (graphics, nodeFrom -> x, nodeFrom -> y, nodeTo -> x, nodeTo -> y);
		}
	}
	Graphics_setLineWidth (graphics, 1.0);
	/*
	 * Draw nodes.
	 */
	for (long inode = 1; inode <= d_numberOfNodes; inode ++) {
		NetworkNode node = & d_nodes [inode];
		double diameter = fabs (node -> activity) * 5.0;
		if (diameter != 0.0) {
			Graphics_setColour (graphics,
				colour ? ( node -> activity < 0.0 ? Graphics_BLUE : Graphics_RED )
				: ( node -> activity < 0.0 ? Graphics_WHITE : Graphics_BLACK));
			Graphics_fillCircle_mm (graphics, node -> x, node -> y, diameter);
		}
		if (node -> clamped) {
			Graphics_setColour (graphics, Graphics_BLACK);
			Graphics_setLineWidth (graphics, 2.0);
			Graphics_circle_mm (graphics, node -> x, node -> y, 5.0);
		}
	}
	Graphics_setColour (graphics, Graphics_BLACK);
	Graphics_setLineWidth (graphics, saveLineWidth);
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

void structNetwork :: f_setWeightUpdateRule (enum kNetwork_weightUpdateRule weightUpdateRule) {
	d_weightUpdateRule = weightUpdateRule;
}

void structNetwork :: f_setActivationSpreadingRule (enum kNetwork_activationSpreadingRule activationSpreadingRule) {
	d_activationSpreadingRule = activationSpreadingRule;
	f_zeroActivities (0, 0);
}

void structNetwork :: f_setShunting (double shunting) {
	d_shunting = shunting;
	f_zeroActivities (0, 0);
}

void structNetwork :: f_setActivationClippingRule (enum kNetwork_activationClippingRule activationClippingRule) {
	d_activationClippingRule = activationClippingRule;
	f_zeroActivities (0, 0);
}

/* End of file Network.cpp */
