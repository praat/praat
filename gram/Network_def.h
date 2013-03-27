/* Network_def.h
 *
 * Copyright (C) 2009-2011,2012,2013 Paul Boersma
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
	oo_FROM (3)
		oo_BYTE (d_dummyActivitySpreadingRule)
	oo_ENDFROM
	oo_FROM (4)
		oo_DOUBLE (d_shunting)
		oo_ENUM (kNetwork_activityClippingRule, d_activityClippingRule)
	oo_ENDFROM
	oo_DOUBLE (d_spreadingRate)
	oo_DOUBLE (d_activityLeak)
	oo_DOUBLE (d_minimumWeight)
	oo_DOUBLE (d_maximumWeight)
	oo_FROM (2)
		oo_BYTE (d_dummyWeightUpdateRule)
	oo_ENDFROM
	oo_DOUBLE (d_learningRate)
	oo_FROM (5)
		oo_DOUBLE (d_instar)
		oo_DOUBLE (d_outstar)
	oo_ENDFROM
	oo_DOUBLE (d_weightLeak)
	#if oo_READING
		if (localVersion < 6) {
			if (localVersion < 5) {
				if (d_learningRate != 0.0) d_weightLeak /= d_learningRate;
				if (d_dummyWeightUpdateRule == 1) d_instar = 1.0, d_outstar = 0.0;
				if (d_dummyWeightUpdateRule == 2) d_instar = 0.0, d_outstar = 1.0;
				if (d_dummyWeightUpdateRule == 3) d_instar = 0.5, d_outstar = 0.5;
			}
			d_activityLeak = - d_activityLeak;   // convert self-excitation to activity leak
		}
	#endif
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
			void f_init (
				double spreadingRate, enum kNetwork_activityClippingRule activityClippingRule,
				double minimumActivity, double maximumActivity, double activityLeak,
				double learningRate, double minimumWeight, double maximumWeight, double weightLeak,
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
			void f_normalizeWeights (long nodeMin, long nodeMax, long nodeFromMin, long nodeFromMax, double newSum);
			void f_setInstar (double instar);
			void f_setOutstar (double outstar);
			void f_setWeightLeak (double weightLeak);
			void f_setActivityLeak (double activityLeak);
			void f_setShunting (double shunting);
			void f_setActivityClippingRule (enum kNetwork_activityClippingRule activityClippingRule);
			Table f_nodes_downto_Table (long fromNodeNumber, long toNodeNumber,
				bool includeNodeNumbers,
				bool includeX, bool includeY, int positionDecimals,
				bool includeClamped,
				bool includeActivity, bool includeExcitation, int activityDecimals);
			void f_listNodes (long fromNodeNumber, long toNodeNumber,
				bool includeNodeNumbers,
				bool includeX, bool includeY, int positionDecimals,
				bool includeClamped,
				bool includeActivity, bool includeExcitation, int activityDecimals);

		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (Network)
#undef ooSTRUCT


/* End of file Network_def.h */
