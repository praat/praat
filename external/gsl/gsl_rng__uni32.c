/* rng/uni32.c
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

/**
  This is a lagged Fibonacci generator which supposedly excellent
  statistical properties (I do not concur)

  I got it from the net and translated into C.

* ======================================================================
* NIST Guide to Available Math Software.
* Fullsource for module UNI from package CMLIB.
* Retrieved from CAMSUN on Tue Oct  8 14:04:10 1996.
* ======================================================================

C***BEGIN PROLOGUE  UNI
C***DATE WRITTEN   810915
C***REVISION DATE  830805
C***CATEGORY NO.  L6A21
C***KEYWORDS  RANDOM NUMBERS, UNIFORM RANDOM NUMBERS
C***AUTHOR    BLUE, JAMES, SCIENTIFIC COMPUTING DIVISION, NBS
C             KAHANER, DAVID, SCIENTIFIC COMPUTING DIVISION, NBS
C             MARSAGLIA, GEORGE, COMPUTER SCIENCE DEPT., WASH STATE UNIV
C
C***PURPOSE  THIS ROUTINE GENERATES QUASI UNIFORM RANDOM NUMBERS ON [0,1
C             AND CAN BE USED ON ANY COMPUTER WITH WHICH ALLOWS INTEGERS
C             AT LEAST AS LARGE AS 32767.
C***DESCRIPTION
C
C       THIS ROUTINE GENERATES QUASI UNIFORM RANDOM NUMBERS ON THE INTER
C       [0,1).  IT CAN BE USED WITH ANY COMPUTER WHICH ALLOWS
C       INTEGERS AT LEAST AS LARGE AS 32767.
C
C
C   USE
C       FIRST TIME....
C                   Z = UNI(JD)
C                     HERE JD IS ANY  N O N - Z E R O  INTEGER.
C                     THIS CAUSES INITIALIZATION OF THE PROGRAM
C                     AND THE FIRST RANDOM NUMBER TO BE RETURNED AS Z.
C       SUBSEQUENT TIMES...
C                   Z = UNI(0)
C                     CAUSES THE NEXT RANDOM NUMBER TO BE RETURNED AS Z.
C
C
C..................................................................
C   NOTE: USERS WHO WISH TO TRANSPORT THIS PROGRAM FROM ONE COMPUTER
C         TO ANOTHER SHOULD READ THE FOLLOWING INFORMATION.....
C
C   MACHINE DEPENDENCIES...
C      MDIG = A LOWER BOUND ON THE NUMBER OF BINARY DIGITS AVAILABLE
C              FOR REPRESENTING INTEGERS, INCLUDING THE SIGN BIT.
C              THIS VALUE MUST BE AT LEAST 16, BUT MAY BE INCREASED
C              IN LINE WITH REMARK A BELOW.
C
C   REMARKS...
C     A. THIS PROGRAM CAN BE USED IN TWO WAYS:
C        (1) TO OBTAIN REPEATABLE RESULTS ON DIFFERENT COMPUTERS,
C            SET 'MDIG' TO THE SMALLEST OF ITS VALUES ON EACH, OR,
C        (2) TO ALLOW THE LONGEST SEQUENCE OF RANDOM NUMBERS TO BE
C            GENERATED WITHOUT CYCLING (REPEATING) SET 'MDIG' TO THE
C            LARGEST POSSIBLE VALUE.
C     B. THE SEQUENCE OF NUMBERS GENERATED DEPENDS ON THE INITIAL
C          INPUT 'JD' AS WELL AS THE VALUE OF 'MDIG'.
C          IF MDIG=16 ONE SHOULD FIND THAT
   Editors Note: set the seed using 152 in order to get uni(305)
   -jt
C            THE FIRST EVALUATION
C              Z=UNI(305) GIVES Z=.027832881...
C            THE SECOND EVALUATION
C              Z=UNI(0) GIVES   Z=.56102176...
C            THE THIRD EVALUATION
C              Z=UNI(0) GIVES   Z=.41456343...
C            THE THOUSANDTH EVALUATION
C              Z=UNI(0) GIVES   Z=.19797357...
C
C***REFERENCES  MARSAGLIA G., "COMMENTS ON THE PERFECT UNIFORM RANDOM
C                 NUMBER GENERATOR", UNPUBLISHED NOTES, WASH S. U.
C***ROUTINES CALLED  I1MACH,XERROR
C***END PROLOGUE  UNI

  **/

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

static inline unsigned long int uni32_get (void *vstate);
static double uni32_get_double (void *vstate);
static void uni32_set (void *state, unsigned long int s);

static const unsigned long int m1 = 2147483647;         /* 2^(MDIG-1) - 1 */
static const unsigned long int m2 = 65536;      /* 2^(MDIG/2) */

typedef struct
  {
    int i, j;
    unsigned long m[17];
  }
uni32_state_t;

static inline unsigned long
uni32_get (void *vstate)
{
  uni32_state_t *state = (uni32_state_t *) vstate;
  const long int i = state->i;
  const long int j = state->j;

  /* important k not be unsigned */
  long int k = state->m[i] - state->m[j];

  if (k < 0)
    k += m1;
  state->m[j] = k;

  if (i == 0)
    {
      state->i = 16;
    }
  else
    {
      (state->i)--;
    }

  if (j == 0)
    {
      state->j = 16;
    }
  else
    {
      (state->j)--;
    }

  return k;
}

static double
uni32_get_double (void *vstate)
{
  return uni32_get (vstate) / 2147483647.0 ;
}

static void
uni32_set (void *vstate, unsigned long int s)
{
  long int seed, k0, k1, j0, j1;
  int i;

  uni32_state_t *state = (uni32_state_t *) vstate;

  /* For this routine, the seeding is very elaborate! */
  /* A flaw in this approach is that seeds 1,2 give exactly the
     same random number sequence!  */

  /*s = 2*s+1; *//* enforce seed be odd */
  seed = (s < m1 ? s : m1);     /* seed should be less than m1 */
  seed -= (seed % 2 == 0 ? 1 : 0);

  k0 = 9069 % m2;
  k1 = 9069 / m2;
  j0 = seed % m2;
  j1 = seed / m2;

  for (i = 0; i < 17; i++)
    {
      seed = j0 * k0;
      j1 = (seed / m2 + j0 * k1 + j1 * k0) % (m2 / 2);
      j0 = seed % m2;
      state->m[i] = j0 + m2 * j1;
    }
  state->i = 4;
  state->j = 16;

  return;
}

static const gsl_rng_type uni32_type =
{"uni32",                       /* name */
 2147483646,                    /* RAND_MAX */
 0,                             /* RAND_MIN */
 sizeof (uni32_state_t),
 &uni32_set,
 &uni32_get,
 &uni32_get_double};

const gsl_rng_type *gsl_rng_uni32 = &uni32_type;
