/* Network_enums.h
 *
 * Copyright (C) 2012,2015-2017 Paul Boersma
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

enums_begin (kNetwork_activityClippingRule, 0)
	enums_add (kNetwork_activityClippingRule, 0, SIGMOID, U"sigmoid")
	enums_add (kNetwork_activityClippingRule, 1, LINEAR, U"linear")
	enums_add (kNetwork_activityClippingRule, 2, TOP_SIGMOID, U"top-sigmoid")
	/*
		As this enumerated type occurs in data, you should add new activity clipping rules only at the end.
	*/
enums_end (kNetwork_activityClippingRule, 2, LINEAR)

/* End of file Network_enums.h */
