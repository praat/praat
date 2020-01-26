/* Network_def.h
 *
 * Copyright (C) 2009-2019 Paul Boersma
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

	oo_INTEGER (nodeFrom)
	oo_INTEGER (nodeTo)
	oo_DOUBLE (weight)

	oo_FROM (1)
		oo_DOUBLE (plasticity)
	oo_ENDFROM
	#if oo_READING
		oo_VERSION_UNTIL (1)
			our plasticity = 1.0;
		oo_VERSION_END
	#endif

oo_END_STRUCT (NetworkConnection)
#undef ooSTRUCT


#define ooSTRUCT Network
oo_DEFINE_CLASS (Network, Daata)

	oo_DOUBLE (minimumActivity)
	oo_DOUBLE (maximumActivity)
	oo_FROM (3)
		oo_BYTE (dummyActivitySpreadingRule)
	oo_ENDFROM
	oo_FROM (4)
		oo_DOUBLE (shunting)
		oo_ENUM (kNetwork_activityClippingRule, activityClippingRule)
	oo_ENDFROM
	oo_DOUBLE (spreadingRate)
	oo_DOUBLE (activityLeak)
	oo_DOUBLE (minimumWeight)
	oo_DOUBLE (maximumWeight)
	oo_FROM (2)
		oo_BYTE (dummyWeightUpdateRule)
	oo_ENDFROM
	oo_DOUBLE (learningRate)
	oo_FROM (5)
		oo_DOUBLE (instar)
		oo_DOUBLE (outstar)
	oo_ENDFROM
	oo_DOUBLE (weightLeak)

	#if oo_READING
		oo_VERSION_UNTIL (5)
			if (our learningRate != 0.0)
				our weightLeak /= our learningRate;
			if (our dummyWeightUpdateRule == 1) {
				our instar = 1.0;
				our outstar = 0.0;
			}
			if (our dummyWeightUpdateRule == 2) {
				our instar = 0.0;
				our outstar = 1.0;
			}
			if (our dummyWeightUpdateRule == 3) {
				our instar = 0.5;
				our outstar = 0.5;
			}
		oo_VERSION_END
		oo_VERSION_UNTIL (6)
			our activityLeak = - our activityLeak;   // convert self-excitation to activity leak
		oo_VERSION_END
	#endif

	oo_DOUBLE (xmin)
	oo_DOUBLE (xmax)
	oo_DOUBLE (ymin)
	oo_DOUBLE (ymax)
	oo_INTEGER (numberOfNodes)
	oo_STRUCTVEC (NetworkNode, nodes, numberOfNodes)
	oo_INTEGER (numberOfConnections)
	oo_STRUCTVEC (NetworkConnection, connections, numberOfConnections)

	#if oo_DECLARING
		void v_info ()
			override;
		void checkNodeNumber (integer nodeNumber) {
			Melder_require (nodeNumber >= 1 && nodeNumber <= our numberOfNodes,
				this, U": node number (", nodeNumber, U") out of the range 1..", our numberOfNodes, U".");
		}
		integer checkAndDefaultNodeRange (integer *p_fromNode, integer *p_toNode) {
			if (*p_fromNode == 0)
				*p_fromNode = 1;
			if (*p_toNode == 0)
				*p_toNode = our numberOfNodes;
			Melder_require (*p_fromNode >= 1 && *p_fromNode <= our numberOfNodes,
				this, U": from-node number (", *p_fromNode, U") out of the range 1..", our numberOfNodes, U".");
			Melder_require (*p_toNode >= 1 && *p_toNode <= our numberOfNodes,
				this, U": to-node number (", *p_toNode, U") out of the range 1..", our numberOfNodes, U".");
			return *p_toNode - *p_fromNode + 1;
		}
	#endif

oo_END_CLASS (Network)
#undef ooSTRUCT


/* End of file Network_def.h */
