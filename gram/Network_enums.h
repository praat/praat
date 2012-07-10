/* Network_enums.h
 *
 * Copyright (C) 2012 Paul Boersma
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

enums_begin (kNetwork_activityClippingRule, 0)
	enums_add (kNetwork_activityClippingRule, 0, SIGMOID, L"sigmoid")
	enums_add (kNetwork_activityClippingRule, 1, LINEAR, L"linear")
	enums_add (kNetwork_activityClippingRule, 2, TOP_SIGMOID, L"top-sigmoid")
enums_end (kNetwork_activityClippingRule, 2, LINEAR)

/* End of file Network_enums.h */
