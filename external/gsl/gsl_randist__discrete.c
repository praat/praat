/* randist/discrete.c
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

/*
   Random Discrete Events
   
   Given K discrete events with different probabilities P[k]
   produce a value k consistent with its probability.

   This program is free software; you can redistribute it and/or
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
*/

/*
 * Based on: Alastair J Walker, An efficient method for generating
 * discrete random variables with general distributions, ACM Trans
 * Math Soft 3, 253-256 (1977).  See also: D. E. Knuth, The Art of
 * Computer Programming, Volume 2 (Seminumerical algorithms), 3rd
 * edition, Addison-Wesley (1997), p120.

 * Walker's algorithm does some preprocessing, and provides two
 * arrays: floating point F[k] and integer A[k].  A value k is chosen
 * from 0..K-1 with equal likelihood, and then a uniform random number
 * u is compared to F[k].  If it is less than F[k], then k is
 * returned.  Otherwise, A[k] is returned.
   
 * Walker's original paper describes an O(K^2) algorithm for setting
 * up the F and A arrays.  I found this disturbing since I wanted to
 * use very large values of K.  I'm sure I'm not the first to realize
 * this, but in fact the preprocessing can be done in O(K) steps.

 * A figure of merit for the preprocessing is the average value for
 * the F[k]'s (that is, SUM_k F[k]/K); this corresponds to the
 * probability that k is returned, instead of A[k], thereby saving a
 * redirection.  Walker's O(K^2) preprocessing will generally improve
 * that figure of merit, compared to my cheaper O(K) method; from some
 * experiments with a perl script, I get values of around 0.6 for my
 * method and just under 0.75 for Walker's.  Knuth has pointed out
 * that finding _the_ optimum lookup tables, which maximize the
 * average F[k], is a combinatorially difficult problem.  But any
 * valid preprocessing will still provide O(1) time for the call to
 * gsl_ran_discrete().  I find that if I artificially set F[k]=1 --
 * ie, better than optimum! -- I get a speedup of maybe 20%, so that's
 * the maximum I could expect from the most expensive preprocessing.
 * Folding in the difference of 0.6 vs 0.75, I'd estimate that the
 * speedup would be less than 10%.

 * I've not implemented it here, but one compromise is to sort the
 * probabilities once, and then work from the two ends inward.  This
 * requires O(K log K), still lots cheaper than O(K^2), and from my
 * experiments with the perl script, the figure of merit is within
 * about 0.01 for K up to 1000, and no sign of diverging (in fact,
 * they seemed to be converging, but it's hard to say with just a
 * handful of runs).

 * The O(K) algorithm goes through all the p_k's and decides if they
 * are "smalls" or "bigs" according to whether they are less than or
 * greater than the mean value 1/K.  The indices to the smalls and the
 * bigs are put in separate stacks, and then we work through the
 * stacks together.  For each small, we pair it up with the next big
 * in the stack (Walker always wanted to pair up the smallest small
 * with the biggest big).  The small "borrows" from the big just
 * enough to bring the small up to mean.  This reduces the size of the
 * big, so the (smaller) big is compared again to the mean, and if it
 * is smaller, it gets "popped" from the big stack and "pushed" to the
 * small stack.  Otherwise, it stays put.  Since every time we pop a
 * small, we are able to deal with it right then and there, and we
 * never have to pop more than K smalls, then the algorithm is O(K).

 * This implementation sets up two separate stacks, and allocates K
 * elements between them.  Since neither stack ever grows, we do an
 * extra O(K) pass through the data to determine how many smalls and
 * bigs there are to begin with and allocate appropriately.  In all
 * there are 2*K*sizeof(double) transient bytes of memory that are
 * used than returned, and K*(sizeof(int)+sizeof(double)) bytes used
 * in the lookup table.
   
 * Walker spoke of using two random numbers (an integer 0..K-1, and a
 * floating point u in [0,1]), but Knuth points out that one can just
 * use the integer and fractional parts of K*u where u is in [0,1].
 * In fact, Knuth further notes that taking F'[k]=(k+F[k])/K, one can
 * directly compare u to F'[k] without having to explicitly set
 * u=K*u-int(K*u).

 * Usage:

 * Starting with an array of probabilities P, initialize and do
 * preprocessing with a call to:

 *    gsl_rng *r;
 *    gsl_ran_discrete_t *f;
 *    f = gsl_ran_discrete_preproc(K,P);
   
 * Then, whenever a random index 0..K-1 is desired, use

 *    k = gsl_ran_discrete(r,f);

 * Note that several different randevent struct's can be
 * simultaneously active.

 * Aside: A very clever alternative approach is described in
 * Abramowitz and Stegun, p 950, citing: Marsaglia, Random variables
 * and computers, Proc Third Prague Conference in Probability Theory,
 * 1962.  A more accesible reference is: G. Marsaglia, Generating
 * discrete random numbers in a computer, Comm ACM 6, 37-38 (1963).
 * If anybody is interested, I (jt) have also coded up this version as
 * part of another software package.  However, I've done some
 * comparisons, and the Walker method is both faster and more stingy
 * with memory.  So, in the end I decided not to include it with the
 * GSL package.
   
 * Written 26 Jan 1999, James Theiler, jt@lanl.gov
 * Adapted to GSL, 30 Jan 1999, jt

 */

#include "gsl__config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gsl_errno.h"
#include "gsl_rng.h"
#include "gsl_randist.h"
#define DEBUG 0
#define KNUTH_CONVENTION 1      /* Saves a few steps of arithmetic
                                 * in the call to gsl_ran_discrete()
                                 */

/*** Begin Stack (this code is used just in this file) ***/

/* Stack code converted to use unsigned indices (i.e. s->i == 0 now
   means an empty stack, instead of -1), for consistency and to give a
   bigger allowable range. BJG */

typedef struct {
    size_t N;                      /* max number of elts on stack */
    size_t *v;                     /* array of values on the stack */
    size_t i;                      /* index of top of stack */
} gsl_stack_t;

static gsl_stack_t *
new_stack(size_t N) {
    gsl_stack_t *s;
    s = (gsl_stack_t *)malloc(sizeof(gsl_stack_t));
    s->N = N;
    s->i = 0;                  /* indicates stack is empty */
    s->v = (size_t *)malloc(sizeof(size_t)*N);
    return s;
}

static void
push_stack(gsl_stack_t *s, size_t v)
{
    if ((s->i) >= (s->N)) {
        fprintf(stderr,"Cannot push stack!\n");
        abort();                /* FIXME: fatal!! */
    }
    (s->v)[s->i] = v;
    s->i += 1;
}

static size_t pop_stack(gsl_stack_t *s)
{
    if ((s->i) == 0) {
        fprintf(stderr,"Cannot pop stack!\n");
        abort();               /* FIXME: fatal!! */
    }
    s->i -= 1;
    return ((s->v)[s->i]);
}

static inline size_t size_stack(const gsl_stack_t *s)
{
    return s->i;
}

static void free_stack(gsl_stack_t *s)
{
    free((char *)(s->v));
    free((char *)s);
}

/*** End Stack ***/


/*** Begin Walker's Algorithm ***/

gsl_ran_discrete_t *
gsl_ran_discrete_preproc(size_t Kevents, const double *ProbArray)
{
    size_t k,b,s;
    gsl_ran_discrete_t *g;
    size_t nBigs, nSmalls;
    gsl_stack_t *Bigs;
    gsl_stack_t *Smalls;
    double *E;
    double pTotal = 0.0, mean, d;
    
    if (Kevents < 1) {
      /* Could probably treat Kevents=1 as a special case */

      GSL_ERROR_VAL ("number of events must be a positive integer", 
                        GSL_EINVAL, 0);
    }

    /* Make sure elements of ProbArray[] are positive.
     * Won't enforce that sum is unity; instead will just normalize
     */

    for (k=0; k<Kevents; ++k) {
        if (ProbArray[k] < 0) {
          GSL_ERROR_VAL ("probabilities must be non-negative",
                            GSL_EINVAL, 0) ;
        }
        pTotal += ProbArray[k];
    }

    /* Begin setting up the main "object" (just a struct, no steroids) */
    g = (gsl_ran_discrete_t *)malloc(sizeof(gsl_ran_discrete_t));
    g->K = Kevents;
    g->F = (double *)malloc(sizeof(double)*Kevents);
    g->A = (size_t *)malloc(sizeof(size_t)*Kevents);

    E = (double *)malloc(sizeof(double)*Kevents);

    if (E==NULL) {
      GSL_ERROR_VAL ("Cannot allocate memory for randevent", GSL_ENOMEM, 0);
    }

    for (k=0; k<Kevents; ++k) {
        E[k] = ProbArray[k]/pTotal;
    }

    /* Now create the Bigs and the Smalls */
    mean = 1.0/Kevents;
    nSmalls=nBigs=0;
    for (k=0; k<Kevents; ++k) {
        if (E[k] < mean) ++nSmalls;
        else             ++nBigs;
    }
    Bigs   = new_stack(nBigs);
    Smalls = new_stack(nSmalls);
    for (k=0; k<Kevents; ++k) {
        if (E[k] < mean) {
            push_stack(Smalls,k);
        }
        else {
            push_stack(Bigs,k);
        }
    }
    /* Now work through the smalls */
    while (size_stack(Smalls) > 0) {
        s = pop_stack(Smalls);
        if (size_stack(Bigs) == 0) {
            (g->A)[s]=s;
            (g->F)[s]=1.0;
            continue;
        }
        b = pop_stack(Bigs);
        (g->A)[s]=b;
        (g->F)[s]=Kevents*E[s];
#if DEBUG
        fprintf(stderr,"s=%2d, A=%2d, F=%.4f\n",s,(g->A)[s],(g->F)[s]);
#endif        
        d = mean - E[s];
        E[s] += d;              /* now E[s] == mean */
        E[b] -= d;
        if (E[b] < mean) {
            push_stack(Smalls,b); /* no longer big, join ranks of the small */
        }
        else if (E[b] > mean) {
            push_stack(Bigs,b); /* still big, put it back where you found it */
        }
        else {
            /* E[b]==mean implies it is finished too */
            (g->A)[b]=b;
            (g->F)[b]=1.0;
        }
    }
    while (size_stack(Bigs) > 0) {
        b = pop_stack(Bigs);
        (g->A)[b]=b;
        (g->F)[b]=1.0;
    }
    /* Stacks have been emptied, and A and F have been filled */

    if ( size_stack(Smalls) != 0) {
      GSL_ERROR_VAL ("Smalls stack has not been emptied",
                     GSL_ESANITY, 0 );
    }
    
#if 0
    /* if 1, then artificially set all F[k]'s to unity.  This will
     * give wrong answers, but you'll get them faster.  But, not
     * that much faster (I get maybe 20%); that's an upper bound
     * on what the optimal preprocessing would give.
     */
    for (k=0; k<Kevents; ++k) {
        (g->F)[k] = 1.0;
    }
#endif

#if KNUTH_CONVENTION
    /* For convenience, set F'[k]=(k+F[k])/K */
    /* This saves some arithmetic in gsl_ran_discrete(); I find that
     * it doesn't actually make much difference.
     */
    for (k=0; k<Kevents; ++k) {
        (g->F)[k] += k;
        (g->F)[k] /= Kevents;
    }
#endif    

    free_stack(Bigs);
    free_stack(Smalls);
    free((char *)E);

    return g;
}

size_t
gsl_ran_discrete(const gsl_rng *r, const gsl_ran_discrete_t *g)
{
    size_t c=0;
    double u,f;
    u = gsl_rng_uniform(r);
#if KNUTH_CONVENTION
    c = (u*(g->K));
#else
    u *= g->K;
    c = u;
    u -= c;
#endif
    f = (g->F)[c];
    /* fprintf(stderr,"c,f,u: %d %.4f %f\n",c,f,u); */
    if (f == 1.0) return c;

    if (u < f) {
        return c;
    }
    else {
        return (g->A)[c];
    }
}

void gsl_ran_discrete_free(gsl_ran_discrete_t *g)
{
    free((char *)(g->A));
    free((char *)(g->F));
    free((char *)g);
}

double
gsl_ran_discrete_pdf(size_t k, const gsl_ran_discrete_t *g)
{
    size_t i,K;
    double f,p=0;
    K= g->K;
    if (k>K) return 0;
    for (i=0; i<K; ++i) {
        f = (g->F)[i];
#if KNUTH_CONVENTION
        f = K*f-i;
#endif        
        if (i==k) {
            p += f;
        } else if (k == (g->A)[i]) {
            p += 1.0 - f;
        }
    }
    return p/K;
}
