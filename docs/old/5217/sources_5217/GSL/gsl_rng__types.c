/* rng/types.c
 * 
 * Copyright (C) 2001, 2007 Brian Gough
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

#define N 100

const gsl_rng_type * gsl_rng_generator_types[N];

#define ADD(t) {if (i==N) abort(); gsl_rng_generator_types[i] = (t); i++; };

const gsl_rng_type **
gsl_rng_types_setup (void)
{
  int i = 0;

  ADD(gsl_rng_borosh13);
  ADD(gsl_rng_cmrg);
  ADD(gsl_rng_coveyou);
  ADD(gsl_rng_fishman18);
  ADD(gsl_rng_fishman20);
  ADD(gsl_rng_fishman2x);
  ADD(gsl_rng_gfsr4);
  ADD(gsl_rng_knuthran);
  ADD(gsl_rng_knuthran2);
  ADD(gsl_rng_knuthran2002);
  ADD(gsl_rng_lecuyer21);
  ADD(gsl_rng_minstd);
  ADD(gsl_rng_mrg);
  ADD(gsl_rng_mt19937);
  ADD(gsl_rng_mt19937_1999);
  ADD(gsl_rng_mt19937_1998);
  ADD(gsl_rng_r250);
  ADD(gsl_rng_ran0);
  ADD(gsl_rng_ran1);
  ADD(gsl_rng_ran2);
  ADD(gsl_rng_ran3);
  ADD(gsl_rng_rand);
  ADD(gsl_rng_rand48);
  ADD(gsl_rng_random128_bsd);
  ADD(gsl_rng_random128_glibc2);
  ADD(gsl_rng_random128_libc5);
  ADD(gsl_rng_random256_bsd);
  ADD(gsl_rng_random256_glibc2);
  ADD(gsl_rng_random256_libc5);
  ADD(gsl_rng_random32_bsd);
  ADD(gsl_rng_random32_glibc2);
  ADD(gsl_rng_random32_libc5);
  ADD(gsl_rng_random64_bsd);
  ADD(gsl_rng_random64_glibc2);
  ADD(gsl_rng_random64_libc5);
  ADD(gsl_rng_random8_bsd);
  ADD(gsl_rng_random8_glibc2);
  ADD(gsl_rng_random8_libc5);
  ADD(gsl_rng_random_bsd);
  ADD(gsl_rng_random_glibc2);
  ADD(gsl_rng_random_libc5);
  ADD(gsl_rng_randu);
  ADD(gsl_rng_ranf);
  ADD(gsl_rng_ranlux);
  ADD(gsl_rng_ranlux389);
  ADD(gsl_rng_ranlxd1);
  ADD(gsl_rng_ranlxd2);
  ADD(gsl_rng_ranlxs0);
  ADD(gsl_rng_ranlxs1);
  ADD(gsl_rng_ranlxs2);
  ADD(gsl_rng_ranmar);
  ADD(gsl_rng_slatec);
  ADD(gsl_rng_taus);
  ADD(gsl_rng_taus2);
  ADD(gsl_rng_taus113);
  ADD(gsl_rng_transputer);
  ADD(gsl_rng_tt800);
  ADD(gsl_rng_uni);
  ADD(gsl_rng_uni32);
  ADD(gsl_rng_vax);
  ADD(gsl_rng_waterman14);
  ADD(gsl_rng_zuf);
  ADD(0);

  return gsl_rng_generator_types;
}

