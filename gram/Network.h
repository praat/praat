#ifndef _Network_h_
#define _Network_h_
/* Network.h
 *
 * Copyright (C) 2009-2011 Paul Boersma
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
 * pb 2011/03/08
 */

#include "Data.h"
#include "Graphics.h"

#ifdef __cplusplus
	extern "C" {
#endif

#include "Network_def.h"
#define Network_methods Data_methods
oo_CLASS_CREATE (Network, Data);

void Network_init_e (Network me, double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections);

Network Network_create_e (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections);

Network Network_create_rectangle_e (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight);
Network Network_create_rectangle_vertical_e (double minimumActivity, double maximumActivity, double spreadingRate,
	double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
	long numberOfRows, long numberOfColumns, bool bottomRowClamped,
	double initialMinimumWeight, double initialMaximumWeight);

void Network_addNode_e (Network me, double x, double y, double activity, bool clamped);
void Network_addConnection_e (Network me, long nodeFrom, long nodeTo, double weight, double plasticity);

void Network_draw (Network me, Graphics graphics, bool colour);

double Network_getActivity_e (Network me, long inode);
void Network_setActivity_e (Network me, long inode, double activity);
double Network_getWeight_e (Network me, long iconn);
void Network_setWeight_e (Network me, long iconn, double weight);
void Network_setClamping_e (Network me, long inode, bool clamped);

void Network_zeroActivities (Network me, long nodeMin, long nodeMax);
void Network_normalizeActivities (Network me, long nodeMin, long nodeMax);
void Network_spreadActivities (Network me, long numberOfSteps);
void Network_updateWeights (Network me);

#ifdef __cplusplus
	}
#endif

/* End of file Network.h */
#endif
