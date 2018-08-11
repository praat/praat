#ifndef _melder_quantity_h_
#define _melder_quantity_h_
/* melder_quantity.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

#define MelderQuantity_NONE  0
#define MelderQuantity_TIME_SECONDS  1
#define MelderQuantity_FREQUENCY_HERTZ  2
#define MelderQuantity_FREQUENCY_BARK  3
#define MelderQuantity_DISTANCE_FROM_GLOTTIS_METRES  4
#define MelderQuantity_NUMBER_OF_QUANTITIES  4
conststring32 MelderQuantity_getText (int quantity);   // e.g. "Time"
conststring32 MelderQuantity_getWithUnitText (int quantity);   // e.g. "Time (s)"
conststring32 MelderQuantity_getLongUnitText (int quantity);   // e.g. "seconds"
conststring32 MelderQuantity_getShortUnitText (int quantity);   // e.g. "s"

/* End of file melder_quantity.h */
#endif
