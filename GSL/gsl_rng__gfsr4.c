/* This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.  You should have received
   a copy of the GNU General Public License along with this program;
   if not, write to the Free Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA

   From Robert M. Ziff, "Four-tap shift-register-sequence
   random-number generators," Computers in Physics 12(4), Jul/Aug
   1998, pp 385-392.  A generalized feedback shift-register (GFSR)
   is basically an xor-sum of particular past lagged values.  A 
   four-tap register looks like:
      ra[nd] = ra[nd-A] ^ ra[nd-B] ^ ra[nd-C] ^ ra[nd-D]
   
   Ziff notes that "it is now widely known" that two-tap registers
   have serious flaws, the most obvious one being the three-point
   correlation that comes from the defn of the generator.  Nice
   mathematical properties can be derived for GFSR's, and numerics
   bears out the claim that 4-tap GFSR's with appropriately chosen
   offsets are as random as can be measured, using the author's test.

   This implementation uses the values suggested the the author's
   example on p392, but altered to fit the GSL framework.  The "state"
   is 2^14 longs, or 64Kbytes; 2^14 is the smallest power of two that
   is larger than D, the largest offset.  We really only need a state
   with the last D values, but by going to a power of two, we can do a
   masking operation instead of a modulo, and this is presumably
   faster, though I haven't actually tried it.  The article actually
   suggested a short/fast hack:

   #define RandomInteger (++nd, ra[nd&M]=ra[(nd-A)&M]\
                          ^ra[(nd-B)&M]^ra[(nd-C)&M]^ra[(nd-D)&M])

   so that (as long as you've defined nd,ra[M+1]), then you ca do things
   like: 'if (RandomInteger < p) {...}'.

   Note that n&M varies from 0 to M, *including* M, so that the
   array has to be of size M+1.  Since M+1 is a power of two, n&M
   is a potentially quicker implementation of the equivalent n%(M+1).

   This implementation copyright (C) 1998 James Theiler, based on
   the example mt.c in the GSL, as implemented by Brian Gough.
*/

#include "gsl__config.h"
#include <stdlib.h>
#include "gsl_rng.h"

static inline unsigned long int gfsr4_get (void *vstate);
static double gfsr4_get_double (void *vstate);
static void gfsr4_set (void *state, unsigned long int s);

/* Magic numbers */
#define A 471
#define B 1586
#define C 6988
#define D 9689
#define M 16383 /* = 2^14-1 */
/* #define M 0x0003fff */

typedef struct
  {
    int nd;
    unsigned long ra[M+1];
  }
gfsr4_state_t;

static inline unsigned long
gfsr4_get (void *vstate)
{
  gfsr4_state_t *state = (gfsr4_state_t *) vstate;

  state->nd = ((state->nd)+1)&M;
  return state->ra[(state->nd)] =
      state->ra[((state->nd)+(M+1-A))&M]^
      state->ra[((state->nd)+(M+1-B))&M]^
      state->ra[((state->nd)+(M+1-C))&M]^
      state->ra[((state->nd)+(M+1-D))&M];
  
}

static double
gfsr4_get_double (void * vstate)
{
  return gfsr4_get (vstate) / 4294967296.0 ;
}

static void
gfsr4_set (void *vstate, unsigned long int s)
{
  gfsr4_state_t *state = (gfsr4_state_t *) vstate;
  int i, j;
  /* Masks for turning on the diagonal bit and turning off the
     leftmost bits */
  unsigned long int msb = 0x80000000UL;
  unsigned long int mask = 0xffffffffUL;

  if (s == 0)
    s = 4357;   /* the default seed is 4357 */

  /* We use the congruence s_{n+1} = (69069*s_n) mod 2^32 to
     initialize the state. This works because ANSI-C unsigned long
     integer arithmetic is automatically modulo 2^32 (or a higher
     power of two), so we can safely ignore overflow. */

#define LCG(n) ((69069 * n) & 0xffffffffUL)

  /* Brian Gough suggests this to avoid low-order bit correlations */
  for (i = 0; i <= M; i++)
    {
      unsigned long t = 0 ;
      unsigned long bit = msb ;
      for (j = 0; j < 32; j++)
        {
          s = LCG(s) ;
          if (s & msb) 
            t |= bit ;
          bit >>= 1 ;
        }
      state->ra[i] = t ;
    }

  /* Perform the "orthogonalization" of the matrix */
  /* Based on the orthogonalization used in r250, as suggested initially
   * by Kirkpatrick and Stoll, and pointed out to me by Brian Gough
   */

  /* BJG: note that this orthogonalisation doesn't have any effect
     here because the the initial 6695 elements do not participate in
     the calculation.  For practical purposes this orthogonalisation
     is somewhat irrelevant, because the probability of the original
     sequence being degenerate should be exponentially small. */

  for (i=0; i<32; ++i) {
      int k=7+i*3;
      state->ra[k] &= mask;     /* Turn off bits left of the diagonal */
      state->ra[k] |= msb;      /* Turn on the diagonal bit           */
      mask >>= 1;
      msb >>= 1;
  }

  state->nd = i;
}

static const gsl_rng_type gfsr4_type =
{"gfsr4",                       /* name */
 0xffffffffUL,                  /* RAND_MAX  */
 0,                             /* RAND_MIN  */
 sizeof (gfsr4_state_t),
 &gfsr4_set,
 &gfsr4_get,
 &gfsr4_get_double};

const gsl_rng_type *gsl_rng_gfsr4 = &gfsr4_type;





