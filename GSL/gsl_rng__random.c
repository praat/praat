/* rng/random.c
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2007 James Theiler, Brian Gough
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

/* This file provides support for random() generators. There are three
   versions in widespread use today,

   - The original BSD version, e.g. on SunOS 4.1 and FreeBSD.

   - The Linux libc5 version, which is differs from the BSD version in
   its seeding procedure, possibly due to the introduction of a typo
   in the multiplier.

   - The GNU glibc2 version, which has a new (and better) seeding
   procedure.

   They all produce different numbers, due to the different seeding
   algorithms, but the algorithm for the generator is the same in each
   case.
  
 */

static inline long int random_get (int * i, int * j, int n, long int * x);

static inline unsigned long int random8_get (void *vstate);
static inline unsigned long int random32_get (void *vstate);
static inline unsigned long int random64_get (void *vstate);
static inline unsigned long int random128_get (void *vstate);
static inline unsigned long int random256_get (void *vstate);

static double random8_get_double (void *vstate);
static double random32_get_double (void *vstate);
static double random64_get_double (void *vstate);
static double random128_get_double (void *vstate);
static double random256_get_double (void *vstate);

static void random8_glibc2_set (void *state, unsigned long int s);
static void random32_glibc2_set (void *state, unsigned long int s);
static void random64_glibc2_set (void *state, unsigned long int s);
static void random128_glibc2_set (void *state, unsigned long int s);
static void random256_glibc2_set (void *state, unsigned long int s);

static void random8_libc5_set (void *state, unsigned long int s);
static void random32_libc5_set (void *state, unsigned long int s);
static void random64_libc5_set (void *state, unsigned long int s);
static void random128_libc5_set (void *state, unsigned long int s);
static void random256_libc5_set (void *state, unsigned long int s);

static void random8_bsd_set (void *state, unsigned long int s);
static void random32_bsd_set (void *state, unsigned long int s);
static void random64_bsd_set (void *state, unsigned long int s);
static void random128_bsd_set (void *state, unsigned long int s);
static void random256_bsd_set (void *state, unsigned long int s);

static void bsd_initialize (long int * x, int n, unsigned long int s);
static void libc5_initialize (long int * x, int n, unsigned long int s);
static void glibc2_initialize (long int * x, int n, unsigned long int s);

typedef struct
  {
    long int x;
  }
random8_state_t;

typedef struct
  {
    int i, j;
    long int x[7];
  }
random32_state_t;

typedef struct
  {
    int i, j;
    long int x[15];
  }
random64_state_t;

typedef struct
  {
    int i, j;
    long int x[31];
  }
random128_state_t;

typedef struct
  {
    int i, j;
    long int x[63];
  }
random256_state_t;

static inline unsigned long int
random8_get (void *vstate)
{
  random8_state_t *state = (random8_state_t *) vstate;

  state->x = (1103515245 * state->x + 12345) & 0x7fffffffUL;
  return state->x;
}

static inline long int
random_get (int * i, int * j, int n, long int * x)
{
  long int k ;

  x[*i] += x[*j] ;
  k = (x[*i] >> 1) & 0x7FFFFFFF ;
  
  (*i)++ ;
  if (*i == n)
    *i = 0 ;
  
  (*j)++ ;
  if (*j == n)
    *j = 0 ;

  return k ;
}

static inline unsigned long int
random32_get (void *vstate)
{
  random32_state_t *state = (random32_state_t *) vstate;
  unsigned long int k = random_get (&state->i, &state->j, 7, state->x) ; 
  return k ;
}

static inline unsigned long int
random64_get (void *vstate)
{
  random64_state_t *state = (random64_state_t *) vstate;
  long int k = random_get (&state->i, &state->j, 15, state->x) ; 
  return k ;
}

static inline unsigned long int
random128_get (void *vstate)
{
  random128_state_t *state = (random128_state_t *) vstate;
  unsigned long int k = random_get (&state->i, &state->j, 31, state->x) ; 
  return k ;
}

static inline unsigned long int
random256_get (void *vstate)
{
  random256_state_t *state = (random256_state_t *) vstate;
  long int k = random_get (&state->i, &state->j, 63, state->x) ; 
  return k ;
}

static double
random8_get_double (void *vstate)
{
  return random8_get (vstate) / 2147483648.0 ;
}

static double
random32_get_double (void *vstate)
{
  return random32_get (vstate) / 2147483648.0 ;
}

static double
random64_get_double (void *vstate)
{
  return random64_get (vstate) / 2147483648.0 ;
}

static double
random128_get_double (void *vstate)
{
  return random128_get (vstate) / 2147483648.0 ;
}

static double
random256_get_double (void *vstate)
{
  return random256_get (vstate) / 2147483648.0 ;
}

static void
random8_bsd_set (void *vstate, unsigned long int s)
{
  random8_state_t *state = (random8_state_t *) vstate;
  
  if (s == 0) 
    s = 1;

  state->x = s;
}

static void
random32_bsd_set (void *vstate, unsigned long int s)
{
  random32_state_t *state = (random32_state_t *) vstate;
  int i;

  bsd_initialize (state->x, 7, s) ;

  state->i = 3;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 7 ; i++)
    random32_get (state) ; 
}

static void
random64_bsd_set (void *vstate, unsigned long int s)
{
  random64_state_t *state = (random64_state_t *) vstate;
  int i;

  bsd_initialize (state->x, 15, s) ;

  state->i = 1;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 15 ; i++)
    random64_get (state) ; 
}

static void
random128_bsd_set (void *vstate, unsigned long int s)
{
  random128_state_t *state = (random128_state_t *) vstate;
  int i;

  bsd_initialize (state->x, 31, s) ;

  state->i = 3;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 31 ; i++)
    random128_get (state) ; 
}

static void
random256_bsd_set (void *vstate, unsigned long int s)
{
  random256_state_t *state = (random256_state_t *) vstate;
  int i;

  bsd_initialize (state->x, 63, s) ;

  state->i = 1;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 63 ; i++)
    random256_get (state) ; 
}

static void 
bsd_initialize (long int * x, int n, unsigned long int s)
{
  int i; 

  if (s == 0)
    s = 1 ;

  x[0] = s;

  for (i = 1 ; i < n ; i++)
    x[i] = 1103515245 * x[i-1] + 12345 ;
}

static void 
libc5_initialize (long int * x, int n, unsigned long int s)
{
  int i; 

  if (s == 0)
    s = 1 ;

  x[0] = s;

  for (i = 1 ; i < n ; i++)
    x[i] = 1103515145 * x[i-1] + 12345 ;
}

static void 
glibc2_initialize (long int * x, int n, unsigned long int s)
{
  int i; 

  if (s == 0)
    s = 1 ;

  x[0] = s;

  for (i = 1 ; i < n ; i++)
    {
      const long int h = s / 127773;
      const long int t = 16807 * (s - h * 127773) - h * 2836;
      if (t < 0)
        {
          s = t + 2147483647 ;
        }
      else
        {
          s = t ;
        }

    x[i] = s ;
    }
}

static void
random8_glibc2_set (void *vstate, unsigned long int s)
{
  random8_state_t *state = (random8_state_t *) vstate;
  
  if (s == 0) 
    s = 1;

  state->x = s;
}

static void
random32_glibc2_set (void *vstate, unsigned long int s)
{
  random32_state_t *state = (random32_state_t *) vstate;
  int i;

  glibc2_initialize (state->x, 7, s) ;

  state->i = 3;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 7 ; i++)
    random32_get (state) ; 
}

static void
random64_glibc2_set (void *vstate, unsigned long int s)
{
  random64_state_t *state = (random64_state_t *) vstate;
  int i;

  glibc2_initialize (state->x, 15, s) ;

  state->i = 1;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 15 ; i++)
    random64_get (state) ; 
}

static void
random128_glibc2_set (void *vstate, unsigned long int s)
{
  random128_state_t *state = (random128_state_t *) vstate;
  int i;

  glibc2_initialize (state->x, 31, s) ;

  state->i = 3;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 31 ; i++)
    random128_get (state) ; 
}

static void
random256_glibc2_set (void *vstate, unsigned long int s)
{
  random256_state_t *state = (random256_state_t *) vstate;
  int i;

  glibc2_initialize (state->x, 63, s) ;

  state->i = 1;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 63 ; i++)
    random256_get (state) ; 
}


static void
random8_libc5_set (void *vstate, unsigned long int s)
{
  random8_state_t *state = (random8_state_t *) vstate;
  
  if (s == 0) 
    s = 1;

  state->x = s;
}

static void
random32_libc5_set (void *vstate, unsigned long int s)
{
  random32_state_t *state = (random32_state_t *) vstate;
  int i;

  libc5_initialize (state->x, 7, s) ;

  state->i = 3;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 7 ; i++)
    random32_get (state) ; 
}

static void
random64_libc5_set (void *vstate, unsigned long int s)
{
  random64_state_t *state = (random64_state_t *) vstate;
  int i;

  libc5_initialize (state->x, 15, s) ;

  state->i = 1;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 15 ; i++)
    random64_get (state) ; 
}

static void
random128_libc5_set (void *vstate, unsigned long int s)
{
  random128_state_t *state = (random128_state_t *) vstate;
  int i;

  libc5_initialize (state->x, 31, s) ;

  state->i = 3;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 31 ; i++)
    random128_get (state) ; 
}

static void
random256_libc5_set (void *vstate, unsigned long int s)
{
  random256_state_t *state = (random256_state_t *) vstate;
  int i;

  libc5_initialize (state->x, 63, s) ;

  state->i = 1;
  state->j = 0;
  
  for (i = 0 ; i < 10 * 63 ; i++)
    random256_get (state) ; 
}

static const gsl_rng_type random_glibc2_type =
{"random-glibc2",                       /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random128_state_t),
 &random128_glibc2_set,
 &random128_get,
 &random128_get_double};

static const gsl_rng_type random8_glibc2_type =
{"random8-glibc2",                      /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random8_state_t),
 &random8_glibc2_set,
 &random8_get,
 &random8_get_double};

static const gsl_rng_type random32_glibc2_type =
{"random32-glibc2",                     /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random32_state_t),
 &random32_glibc2_set,
 &random32_get,
 &random32_get_double};

static const gsl_rng_type random64_glibc2_type =
{"random64-glibc2",                     /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random64_state_t),
 &random64_glibc2_set,
 &random64_get,
 &random64_get_double};

static const gsl_rng_type random128_glibc2_type =
{"random128-glibc2",                    /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random128_state_t),
 &random128_glibc2_set,
 &random128_get,
 &random128_get_double};

static const gsl_rng_type random256_glibc2_type =
{"random256-glibc2",                    /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random256_state_t),
 &random256_glibc2_set,
 &random256_get,
 &random256_get_double};

static const gsl_rng_type random_libc5_type =
{"random-libc5",                        /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random128_state_t),
 &random128_libc5_set,
 &random128_get,
 &random128_get_double};

static const gsl_rng_type random8_libc5_type =
{"random8-libc5",                       /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random8_state_t),
 &random8_libc5_set,
 &random8_get,
 &random8_get_double};

static const gsl_rng_type random32_libc5_type =
{"random32-libc5",                      /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random32_state_t),
 &random32_libc5_set,
 &random32_get,
 &random32_get_double};

static const gsl_rng_type random64_libc5_type =
{"random64-libc5",                      /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random64_state_t),
 &random64_libc5_set,
 &random64_get,
 &random64_get_double};

static const gsl_rng_type random128_libc5_type =
{"random128-libc5",                     /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random128_state_t),
 &random128_libc5_set,
 &random128_get,
 &random128_get_double};

static const gsl_rng_type random256_libc5_type =
{"random256-libc5",                     /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random256_state_t),
 &random256_libc5_set,
 &random256_get,
 &random256_get_double};

static const gsl_rng_type random_bsd_type =
{"random-bsd",                  /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random128_state_t),
 &random128_bsd_set,
 &random128_get,
 &random128_get_double};

static const gsl_rng_type random8_bsd_type =
{"random8-bsd",                 /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random8_state_t),
 &random8_bsd_set,
 &random8_get,
 &random8_get_double};

static const gsl_rng_type random32_bsd_type =
{"random32-bsd",                        /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random32_state_t),
 &random32_bsd_set,
 &random32_get,
 &random32_get_double};

static const gsl_rng_type random64_bsd_type =
{"random64-bsd",                        /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random64_state_t),
 &random64_bsd_set,
 &random64_get,
 &random64_get_double};

static const gsl_rng_type random128_bsd_type =
{"random128-bsd",               /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random128_state_t),
 &random128_bsd_set,
 &random128_get,
 &random128_get_double};

static const gsl_rng_type random256_bsd_type =
{"random256-bsd",               /* name */
 0x7fffffffUL,                  /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (random256_state_t),
 &random256_bsd_set,
 &random256_get,
 &random256_get_double};

const gsl_rng_type *gsl_rng_random_libc5    = &random_libc5_type;
const gsl_rng_type *gsl_rng_random8_libc5   = &random8_libc5_type;
const gsl_rng_type *gsl_rng_random32_libc5  = &random32_libc5_type;
const gsl_rng_type *gsl_rng_random64_libc5  = &random64_libc5_type;
const gsl_rng_type *gsl_rng_random128_libc5 = &random128_libc5_type;
const gsl_rng_type *gsl_rng_random256_libc5 = &random256_libc5_type;

const gsl_rng_type *gsl_rng_random_glibc2    = &random_glibc2_type;
const gsl_rng_type *gsl_rng_random8_glibc2   = &random8_glibc2_type;
const gsl_rng_type *gsl_rng_random32_glibc2  = &random32_glibc2_type;
const gsl_rng_type *gsl_rng_random64_glibc2  = &random64_glibc2_type;
const gsl_rng_type *gsl_rng_random128_glibc2 = &random128_glibc2_type;
const gsl_rng_type *gsl_rng_random256_glibc2 = &random256_glibc2_type;

const gsl_rng_type *gsl_rng_random_bsd    = &random_bsd_type;
const gsl_rng_type *gsl_rng_random8_bsd   = &random8_bsd_type;
const gsl_rng_type *gsl_rng_random32_bsd  = &random32_bsd_type;
const gsl_rng_type *gsl_rng_random64_bsd  = &random64_bsd_type;
const gsl_rng_type *gsl_rng_random128_bsd = &random128_bsd_type;
const gsl_rng_type *gsl_rng_random256_bsd = &random256_bsd_type;




