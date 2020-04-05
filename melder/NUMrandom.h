#ifndef _NUMrandom_h_
#define _NUMrandom_h_
/* NUMrandom.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

/********** Random numbers **********/

void NUMrandom_initializeSafelyAndUnpredictably ();
void NUMrandom_initializeWithSeedUnsafelyButPredictably (uint64 seed);

double NUMrandomFraction ();
double NUMrandomFraction_mt (int threadNumber);

double NUMrandomUniform (double lowest, double highest);

integer NUMrandomInteger (integer lowest, integer highest);

bool NUMrandomBernoulli (double probability);
double NUMrandomBernoulli_real (double probability);

double NUMrandomGauss (double mean, double standardDeviation);
double NUMrandomGauss_mt (int threadNumber, double mean, double standardDeviation);

double NUMrandomPoisson (double mean);

uint32 NUMhashString (conststring32 string);

/* End of file NUMrandom.h */
#endif
