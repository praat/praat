/* TextGridTierNavigator_enums.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

enums_begin (kMatchDomain, 1)
	enums_add (kMatchDomain, 1, MATCH_START_TO_MATCH_END, U"Match start to Match end")
	enums_add (kMatchDomain, 2, TOPIC_START_TO_TOPIC_END, U"Topic start to Topic end")
	enums_add (kMatchDomain, 3, BEFORE_START_TO_TOPIC_END, U"Before start to Topic end")
	enums_add (kMatchDomain, 4, BEFORE_START_TO_AFTER_END, U"Before start to After end")
	enums_add (kMatchDomain, 5, TOPIC_START_TO_AFTER_END, U"Topic start to After end")
	enums_add (kMatchDomain, 6, BEFORE_START_TO_BEFORE_END, U"Before start to Before end")
	enums_add (kMatchDomain, 7, AFTER_START_TO_AFTER_END, U"After start to After end")
enums_end (kMatchDomain, 7, MATCH_START_TO_MATCH_END)

/* End of file TextGridTierNavigator_enums.h */
