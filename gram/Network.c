/* Network.c
 *
 * Copyright (C) 2009 Paul Boersma
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

static void classNetwork_info (I) {
	iam (Network);
	classData -> info (me);
	MelderInfo_writeLine2 (L"Leak: ", Melder_double (my leak));
	MelderInfo_writeLine2 (L"Number of nodes: ", Melder_integer (my numberOfNodes));
	MelderInfo_writeLine2 (L"Number of connections: ", Melder_integer (my numberOfConnections));
}

class_methods (Network, Data)
	class_method_local (Network, destroy)
	class_method_local (Network, info)
	class_method_local (Network, description)
	class_method_local (Network, copy)
	class_method_local (Network, equal)
	class_method_local (Network, canWriteAsEncoding)
	class_method_local (Network, writeText)
	class_method_local (Network, readText)
	class_method_local (Network, writeBinary)
	class_method_local (Network, readBinary)
class_methods_end

void Network_init_e (Network me, double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	my minimumActivity = minimumActivity;
	my maximumActivity = maximumActivity;
	my spreadingRate = spreadingRate;
	my selfExcitation = selfExcitation;
	my minimumWeight = minimumWeight;
	my maximumWeight = maximumWeight;
	my learningRate = learningRate;
	my leak = leak;
	my xmin = xmin;
	my xmax = xmax;
	my ymin = ymin;
	my ymax = ymax;
	my numberOfNodes = numberOfNodes;
	my nodes = NUMstructvector (NetworkNode, 1, numberOfNodes); cherror
	my numberOfConnections = numberOfConnections;
	my connections = NUMstructvector (NetworkConnection, 1, numberOfConnections); cherror
end:
	return;
}

Network Network_create_e (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections)
{
	Network me = new (Network); cherror
	Network_init_e (me, minimumActivity, maximumActivity, spreadingRate,
		selfExcitation, minimumWeight, maximumWeight, learningRate, leak,
		xmin, xmax, ymin, ymax, numberOfNodes, numberOfConnections); cherror
end:
	iferror forget (me);
	return me;
}

double Network_getActivity_e (Network me, long inode) {
	double activity = NUMundefined;
	if (inode < 0 || inode > my numberOfNodes)
		error4 (L"Node number (", Melder_integer (inode), L" out of the range 1..", Melder_integer (my numberOfNodes))
	activity = my nodes [inode]. activity;
end:
	iferror Melder_error1 (L"Network: activity not gotten.");
	return activity;
}

void Network_setActivity_e (Network me, long inode, double activity) {
	if (inode < 0 || inode > my numberOfNodes)
		error5 (L"(Network: Set activity:) Node number (", Melder_integer (inode), L") out of the range 1..", Melder_integer (my numberOfNodes), L".")
	my nodes [inode]. activity = activity;
end:
	iferror Melder_error1 (L"Network: activity not set.");
}

void Network_setClamping_e (Network me, long inode, bool clamped) {
	if (inode < 0 || inode > my numberOfNodes)
		error5 (L"(Network: Set clamping:) Node number (", Melder_integer (inode), L") out of the range 1..", Melder_integer (my numberOfNodes), L".")
	my nodes [inode]. clamped = clamped;
end:
	iferror Melder_error1 (L"Network: clamping not set.");
}

void Network_spreadActivities (Network me, long numberOfSteps) {
	for (long istep = 1; istep <= numberOfSteps; istep ++) {
		for (long inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			node -> excitation = node -> activity * my selfExcitation;
		}
		for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
			NetworkConnection connection = & my connections [iconn];
			NetworkNode nodeFrom = & my nodes [connection -> nodeFrom];
			NetworkNode nodeTo = & my nodes [connection -> nodeTo];
			nodeFrom -> excitation += nodeTo -> activity * my connections [iconn]. weight;
			nodeTo -> excitation += nodeFrom -> activity * my connections [iconn]. weight;
		}
		for (long inode = 1; inode <= my numberOfNodes; inode ++) {
			NetworkNode node = & my nodes [inode];
			if (! node -> clamped) {
				node -> activity = (my maximumActivity - my minimumActivity) *
					NUMsigmoid (my spreadingRate * node -> excitation) + my minimumActivity;
			}
		}
	}
}

void Network_updateWeights (Network me) {
	for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
		NetworkConnection connection = & my connections [iconn];
		NetworkNode nodeFrom = & my nodes [connection -> nodeFrom];
		NetworkNode nodeTo = & my nodes [connection -> nodeTo];
		connection -> weight = (1.0 - my leak) * connection -> weight + my learningRate * nodeFrom -> activity * nodeTo -> activity;
		if (connection -> weight < my minimumWeight) connection -> weight = my minimumWeight;
		else if (connection -> weight > my maximumWeight) connection -> weight = my maximumWeight;
	}
}

Network Network_create_rectangle_e (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	Network me = Network_create_e (minimumActivity, maximumActivity, spreadingRate,
		selfExcitation, minimumWeight, maximumWeight, learningRate, leak,
		0.0, numberOfColumns, 0.0, numberOfRows, numberOfRows * numberOfColumns,
		numberOfRows * (numberOfColumns - 1) + numberOfColumns * (numberOfRows - 1)); cherror
	/*
	 * Define nodes.
	 */
	for (long inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		node -> x = (inode - 1) % numberOfColumns + 0.5;
		node -> y = (inode - 1) / numberOfColumns + 0.5;
		node -> clamped = bottomRowClamped && inode <= numberOfColumns;
		node -> activity = NUMrandomUniform (my minimumActivity, my maximumActivity);
	}
	/*
	 * Define connections.
	 */
	long iconn = 0;
	for (long irow = 1; irow <= numberOfRows; irow ++) {
		for (long icol = 1; icol <= numberOfColumns - 1; icol ++) {
			NetworkConnection conn = & my connections [++ iconn];
			conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
			conn -> nodeTo = conn -> nodeFrom + 1;
			conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
		}
	}
	for (long icol = 1; icol <= numberOfColumns; icol ++) {
		for (long irow = 1; irow <= numberOfRows - 1; irow ++) {
			NetworkConnection conn = & my connections [++ iconn];
			conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
			conn -> nodeTo = conn -> nodeFrom + numberOfColumns;
			conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
		}
	}
	Melder_assert (iconn == my numberOfConnections);
end:
	iferror forget (me);
	return me;
}

Network Network_create_rectangle_vertical_e (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight)
{
	Network me = Network_create_e (minimumActivity, maximumActivity, spreadingRate,
		selfExcitation, minimumWeight, maximumWeight, learningRate, leak,
		0.0, numberOfColumns, 0.0, numberOfRows, numberOfRows * numberOfColumns,
		numberOfColumns * numberOfColumns * (numberOfRows - 1)); cherror
	/*
	 * Define nodes.
	 */
	for (long inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
		node -> x = (inode - 1) % numberOfColumns + 0.5;
		node -> y = (inode - 1) / numberOfColumns + 0.5;
		node -> clamped = bottomRowClamped && inode <= numberOfColumns;
		node -> activity = NUMrandomUniform (my minimumActivity, my maximumActivity);
	}
	/*
	 * Define connections.
	 */
	long iconn = 0;
	for (long icol = 1; icol <= numberOfColumns; icol ++) {
		for (long jcol = 1; jcol <= numberOfColumns; jcol ++) {
			for (long irow = 1; irow <= numberOfRows - 1; irow ++) {
				NetworkConnection conn = & my connections [++ iconn];
				conn -> nodeFrom = (irow - 1) * numberOfColumns + icol;
				conn -> nodeTo = irow * numberOfColumns + jcol;
				conn -> weight = NUMrandomUniform (initialMinimumWeight, initialMaximumWeight);
			}
		}
	}
	Melder_assert (iconn == my numberOfConnections);
end:
	iferror forget (me);
	return me;
}

void Network_draw (Network me, Graphics graphics, bool colour) {
	double saveLineWidth = Graphics_inqLineWidth (graphics);
	Graphics_setInner (graphics);
	Graphics_setWindow (graphics, my xmin, my xmax, my ymin, my ymax);
	Graphics_setColour (graphics, Graphics_SILVER);
	Graphics_fillRectangle (graphics, my xmin, my xmax, my ymin, my ymax);
	/*
	 * Draw connections.
	 */
	for (long iconn = 1; iconn <= my numberOfConnections; iconn ++) {
		NetworkConnection conn = & my connections [iconn];
		if (conn -> weight != 0.0) {
			NetworkNode nodeFrom = & my nodes [conn -> nodeFrom];
			NetworkNode nodeTo = & my nodes [conn -> nodeTo];
			Graphics_setLineWidth (graphics, fabs (conn -> weight) * 6.0);
			Graphics_setColour (graphics, conn -> weight < 0.0 ? Graphics_WHITE : Graphics_BLACK);
			Graphics_line (graphics, nodeFrom -> x, nodeFrom -> y, nodeTo -> x, nodeTo -> y);
		}
	}
	Graphics_setLineWidth (graphics, 1.0);
	/*
	 * Draw nodes.
	 */
	for (long inode = 1; inode <= my numberOfNodes; inode ++) {
		NetworkNode node = & my nodes [inode];
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

void Network_addNode_e (Network me, double x, double y, double activity, bool clamped) {
	NUMstructvector_append_e (NetworkNode, & my nodes, 1, & my numberOfNodes); cherror
	my nodes [my numberOfNodes]. x = x;
	my nodes [my numberOfNodes]. y = y;
	my nodes [my numberOfNodes]. activity = activity;
	my nodes [my numberOfNodes]. clamped = clamped;
end:
	return;
}

void Network_addConnection_e (Network me, long nodeFrom, long nodeTo, double weight) {
	NUMstructvector_append_e (NetworkConnection, & my connections, 1, & my numberOfConnections); cherror
	my connections [my numberOfConnections]. nodeFrom = nodeFrom;
	my connections [my numberOfConnections]. nodeTo = nodeTo;
	my connections [my numberOfConnections]. weight = weight;
end:
	return;
}

/* End of file Network.c */
