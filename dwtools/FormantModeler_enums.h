/* FormantModeler_enums.h
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

enums_begin (kFormantModelerTrackShift, 1)
	enums_add (kFormantModelerTrackShift, 1, NO_, U"No")
	enums_add (kFormantModelerTrackShift, 2, UP, U"Up")
	enums_add (kFormantModelerTrackShift, 3, DOWN, U"Down")
enums_end (kFormantModelerTrackShift, 3, DOWN)

enums_begin (kFormantModelerWeights, 1)
	enums_add (kFormantModelerWeights, 1, EQUAL_WEIGHTS, U"Equal")
	enums_add (kFormantModelerWeights, 2, ONE_OVER_BANDWIDTH, U"One over bandwidth")
	enums_add (kFormantModelerWeights, 3, ONE_OVER_SQRTBANDWIDTH, U"One over sqrt(bandwidth)")
	enums_add (kFormantModelerWeights, 4, Q_FACTOR, U"Q-factor")
enums_end (kFormantModelerWeights, 4, Q_FACTOR)

/* End of file FormantModeler_enums.h */
