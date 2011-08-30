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

	oo_DOUBLE (minimumActivity)
	oo_DOUBLE (maximumActivity)
	oo_DOUBLE (spreadingRate)
	oo_DOUBLE (selfExcitation)
	oo_DOUBLE (minimumWeight)
	oo_DOUBLE (maximumWeight)
	oo_DOUBLE (learningRate)
	oo_DOUBLE (leak)
	oo_DOUBLE (xmin)
	oo_DOUBLE (xmax)
	oo_DOUBLE (ymin)
	oo_DOUBLE (ymax)
	oo_LONG (numberOfNodes)
	oo_STRUCT_VECTOR (NetworkNode, nodes, numberOfNodes)
	oo_LONG (numberOfConnections)
	oo_STRUCT_VECTOR (NetworkConnection, connections, numberOfConnections)

	#if oo_DECLARING
		// overridden methods:
			virtual void v_info ();
	#endif

oo_END_CLASS (Network)
#undef ooSTRUCT


/* End of file Network_def.h */
