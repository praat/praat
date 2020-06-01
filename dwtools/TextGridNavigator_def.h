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

#define ooSTRUCT NavigatableTier
oo_DEFINE_CLASS (NavigatableTier, Function)

	oo_OBJECT (NavigationContext, 0, navigationContext)
	oo_INTEGER (leftContextDistanceMin)
	oo_INTEGER (leftContextDistanceMax)
	
	oo_INTEGER (rightContextDistanceMin)
	oo_INTEGER (rightContextDistanceMax)
	
	oo_INTEGER (current)  // we can maintain state if TextGrid is fixed.
	oo_INTEGER (maximumLookAhead) // don't go further than this number of steps from current
	oo_INTEGER (maximumLookBack) // don't go back  more than this number of steps from current
	
	oo_ENUM (kBetweenTiers_itemOrientation, betweenTiersItemOrientation) // how does a 'match' in this tier relate to the navigation tier
	
	#if oo_DECLARING
		void v_info ()
			override;
	#endif
		
oo_END_CLASS (NavigatableTier)
#undef ooSTRUCT

#define ooSTRUCT TextGridNavigator
oo_DEFINE_CLASS (TextGridNavigator, Function)

	oo_OBJECT (TextGrid, 0, textgrid)
	oo_OBJECT (NavigatableTier, 0, navigationTier)
	oo_OBJECT (FunctionList, 0, otherNavigatableTiers)
	#if oo_DECLARING
		void v_info ()
			override;
	#endif		
oo_END_CLASS (TextGridNavigator)
#undef ooSTRUCT

 /* End of file TextGridNavigator_def.h */
