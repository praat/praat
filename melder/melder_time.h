#ifndef _melder_time_h_
#define _melder_time_h_
/* melder_time.h
 *
 * Copyright (C) 1992-2016,2018,2020 Paul Boersma
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

double Melder_stopwatch ();

void Melder_sleep (double duration);

double Melder_clock ();   // typically the number of seconds since system start-up, with microsecond precision

autostring32 STRdate ();

/* End of file melder_time.h */
#endif
