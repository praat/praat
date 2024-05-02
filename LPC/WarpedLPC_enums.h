#ifndef _WarpedLPC_enums_h_
#define _WarpedLPC_enums_h_

/* WarpedLPC_enums.h
 *
 * Copyright (C) 2024 David Weenink
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

enums_begin (kWarpedLPC_Analysis, 1)
	enums_add (kWarpedLPC_Analysis, 1, PLP, U"PLP")
enums_end (kWarpedLPC_Analysis, 1, PLP)

enums_begin (kWarpedLPC_FrequencyScale, 1)
	enums_add (kWarpedLPC_FrequencyScale, 1, MEL, U"Mel")
	enums_add (kWarpedLPC_FrequencyScale, 2, BARK, U"Bark")
enums_end (kWarpedLPC_FrequencyScale, 2, MEL)


#endif /* _WarpedLPC_enums_h_ */
