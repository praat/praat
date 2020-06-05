/* NavigationContext_def.h
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

#define ooSTRUCT NavigationContext
oo_DEFINE_CLASS (NavigationContext, Daata)

	oo_OBJECT (Strings, 0, navigationLabels) // because oo_STRING_Vector needs size
	oo_ENUM (kMelder_string, navigationCriterion)
	
	oo_OBJECT (Strings, 0, leftContextLabels)
	oo_ENUM (kMelder_string, leftContextCriterion)
	
	oo_OBJECT (Strings, 0, rightContextLabels)
	oo_ENUM (kMelder_string, rightContextCriterion)	
	
	oo_ENUM (kContext_combination, combinationCriterion)
	oo_BOOLEAN (matchContextOnly)
	
	#if oo_DECLARING
		void v_info ()
			override;
	#endif
	
oo_END_CLASS (NavigationContext)
#undef ooSTRUCT

 /* End of file NavigationContext_def.h */
