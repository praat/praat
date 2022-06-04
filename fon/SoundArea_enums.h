/* SoundArea_enums.h
 *
 * Copyright (C) 2012,2015,2022 Paul Boersma
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

enums_begin (kSoundArea_scalingStrategy, 1)
	enums_add (kSoundArea_scalingStrategy, 1, BY_WHOLE, U"by whole")
	enums_add (kSoundArea_scalingStrategy, 2, BY_WINDOW, U"by window")
	enums_add (kSoundArea_scalingStrategy, 3, BY_WINDOW_AND_CHANNEL, U"by window and channel")
	enums_add (kSoundArea_scalingStrategy, 4, FIXED_HEIGHT, U"fixed height")
	enums_add (kSoundArea_scalingStrategy, 5, FIXED_RANGE, U"fixed range")
enums_end (kSoundArea_scalingStrategy, 5, BY_WINDOW)

/* End of file SoundArea_enums.h */
