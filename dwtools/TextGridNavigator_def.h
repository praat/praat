/* TextGridNavigator_def.h
 *
 * Copyright (C) 2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#define ooSTRUCT TierNavigationContext
oo_DEFINE_CLASS (TierNavigationContext, NavigationContext)

	oo_INTEGER (tierNumber)
	oo_INTEGER (leftContextFrom)
	oo_INTEGER (leftContextTo)

	oo_INTEGER (rightContextFrom)
	oo_INTEGER (rightContextTo)

	oo_INTEGER (current)  // (offLeft) 0 <= current <= size + 1 : (offRight)
	oo_INTEGER (maximumLookAhead) // don't go further than this number of steps from current
	oo_INTEGER (maximumLookBack) // don't go back  more than this number of steps from current

	oo_ENUM (kNavigatableTier_match, matchCriterion) // how does a 'match' in this tier relate to the navigation tier

	#if oo_DECLARING
		void v_info ()
			override;
		virtual integer v_getSize (Function tier);
		virtual integer v_getIndexFromTime (Function tier, double time);
		virtual double v_getLeftTime (Function tier, integer index);
		virtual double v_getRightTime (Function tier, integer index);
		virtual conststring32 v_getLabel (Function tier, integer index);
	#endif

oo_END_CLASS (TierNavigationContext)
#undef ooSTRUCT

#define ooSTRUCT TextGridNavigator
oo_DEFINE_CLASS (TextGridNavigator, Function)

	oo_OBJECT (TextGrid, 0, textgrid)
	oo_COLLECTION_OF (OrderedOf, tierNavigationContext, TierNavigationContext, 0)

	#if oo_DECLARING
		void v_info ()
			override;
	#endif

oo_END_CLASS (TextGridNavigator)
#undef ooSTRUCT


 /* End of file TextGridNavigator_def.h */
