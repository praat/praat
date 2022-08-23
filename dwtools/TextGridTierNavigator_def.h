/* TextGridTierNavigator_def.h
 *
 * Copyright (C) 2021 David Weenink
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

#define ooSTRUCT IntegerRange
oo_DEFINE_STRUCT (IntegerRange)	
	oo_INTEGER (first)
	oo_INTEGER (last)
	#if oo_DECLARING
		integer size () const {
			integer result = last - first + 1;
			return std::max (result, 0_integer);
		}
	#endif
oo_END_STRUCT (IntegerRange)
#undef ooSTRUCT

#define ooSTRUCT TextGridTierNavigator
oo_DEFINE_CLASS (TextGridTierNavigator, Function)

	oo_INTEGER (tierNumber) // we need to know where it came from
	oo_OBJECT (Function, 0, tier)
	oo_OBJECT (NavigationContext, 0, navigationContext)
	oo_STRUCT (IntegerRange, beforeRange)
	oo_STRUCT (IntegerRange, afterRange)

	oo_INTEGER (currentTopicIndex)  // maintain state: index of current match, (offLeft) 0 <= currentTopicIndex <= size + 1 : (offRight)

	oo_ENUM (kMatchDomain, matchDomain) // what is to be considered as the domain (startTime, endTime) of the match? 
	oo_ENUM (kMatchDomainAlignment, matchDomainAlignment) // relation to match in possible other navigators
	
	#if oo_DECLARING
		void v1_info () override;
		virtual integer v_getSize ();
		virtual integer v_timeToLowIndex (double time);
		virtual integer v_timeToIndex (double time);
		virtual integer v_timeToHighIndex (double time);
		virtual double v_getStartTime (integer index);
		virtual double v_getEndTime (integer index);
		virtual conststring32 v_getLabel (integer index);
	#endif

oo_END_CLASS (TextGridTierNavigator)
#undef ooSTRUCT


 /* End of file TextGridTierNavigator_def.h */
