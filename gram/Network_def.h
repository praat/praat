/* Network_def.h
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


#define ooSTRUCT NetworkNode
oo_DEFINE_STRUCT (NetworkNode)

	oo_DOUBLE (x)
	oo_DOUBLE (y)
	oo_BOOLEAN (clamped)
	oo_DOUBLE (activity)
	#if ! oo_READING && ! oo_WRITING
		oo_DOUBLE (excitation)
	#endif

oo_END_STRUCT (NetworkNode)
#undef ooSTRUCT


#define ooSTRUCT NetworkConnection
oo_DEFINE_STRUCT (NetworkConnection)

	oo_LONG (nodeFrom)
	oo_LONG (nodeTo)
	oo_DOUBLE (weight)
	oo_FROM (1)
		oo_DOUBLE (plasticity)
	oo_ENDFROM
	#if oo_READING
		if (localVersion < 1) {
			plasticity = 1.0;
		}
	#endif

oo_END_STRUCT (NetworkConnection)
#undef ooSTRUCT


#define ooSTRUCT Network
oo_DEFINE_CLASS (Network, Data)

	oo_DOUBLE (d_minimumActivity)
	oo_DOUBLE (d_maximumActivity)
	oo_DOUBLE (d_spreadingRate)
	oo_DOUBLE (d_selfExcitation)
	oo_DOUBLE (d_minimumWeight)
	oo_DOUBLE (d_maximumWeight)
	oo_FROM (2)
		oo_ENUM (kNetwork_weightUpdateRule, d_weightUpdateRule)
	oo_ENDFROM
	oo_DOUBLE (d_learningRate)
	oo_DOUBLE (d_leak)
	oo_DOUBLE (d_xmin)
	oo_DOUBLE (d_xmax)
	oo_DOUBLE (d_ymin)
	oo_DOUBLE (d_ymax)
	oo_LONG (d_numberOfNodes)
	oo_STRUCT_VECTOR (NetworkNode, d_nodes, d_numberOfNodes)
	oo_LONG (d_numberOfConnections)
	oo_STRUCT_VECTOR (NetworkConnection, d_connections, d_numberOfConnections)

	#if oo_DECLARING
		// functions:
			void f_init (double minimumActivity, double maximumActivity, double spreadingRate,
				double selfExcitation, double minimumWeight, double maximumWeight, double learningRate, double leak,
				double xmin, double xmax, double ymin, double ymax, long numberOfNodes, long numberOfConnections);
			void f_addNode (double x, double y, double activity, bool clamped);
			void f_addConnection (long nodeFrom, long nodeTo, double weight, double plasticity);
			void f_draw (Graphics graphics, bool colour);
			double f_getActivity (long inode);
			void f_setActivity (long inode, double activity);
			double f_getWeight (long iconn);
			void f_setWeight (long iconn, double weight);
			void f_setClamping (long inode, bool clamped);
			void f_zeroActivities (long nodeMin, long nodeMax);
			void f_normalizeActivities (long nodeMin, long nodeMax);
			void f_spreadActivities (long numberOfSteps);
			void f_updateWeights ();
			void f_setWeightUpdateRule (enum kNetwork_weightUpdateRule weightUpdateRule);

		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (Network)
#undef ooSTRUCT


/* End of file Network_def.h */
