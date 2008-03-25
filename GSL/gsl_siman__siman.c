/* siman/siman.c
 * 
 * Copyright (C) 2007 Brian Gough
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Mark Galassi
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
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "gsl_machine.h"
#include "gsl_rng.h"
#include "gsl_siman.h"

static inline double
boltzmann(double E, double new_E, double T, gsl_siman_params_t *params)
{
  double x = -(new_E - E) / (params->k * T);
  /* avoid underflow errors for large uphill steps */
  return (x < GSL_LOG_DBL_MIN) ? 0.0 : exp(x);
}

static inline void
copy_state(void *src, void *dst, size_t size, gsl_siman_copy_t copyfunc)
{
  if (copyfunc) {
    copyfunc(src, dst);
  } else {
    memcpy(dst, src, size);
  }
}
      
/* implementation of a basic simulated annealing algorithm */

void 
gsl_siman_solve (const gsl_rng * r, void *x0_p, gsl_siman_Efunc_t Ef,
                 gsl_siman_step_t take_step,
                 gsl_siman_metric_t distance,
                 gsl_siman_print_t print_position,
                 gsl_siman_copy_t copyfunc,
                 gsl_siman_copy_construct_t copy_constructor,
                 gsl_siman_destroy_t destructor,
                 size_t element_size,
                 gsl_siman_params_t params)
{
  void *x, *new_x, *best_x;
  double E, new_E, best_E;
  int i;
  double T, T_factor;
  int n_evals = 1, n_iter = 0, n_accepts, n_rejects, n_eless;

  /* this function requires that either the dynamic functions (copy,
     copy_constructor and destrcutor) are passed, or that an element
     size is given */
  assert((copyfunc != NULL && copy_constructor != NULL && destructor != NULL)
         || (element_size != 0));

  distance = 0 ; /* This parameter is not currently used */
  E = Ef(x0_p);

  if (copyfunc) {
    x = copy_constructor(x0_p);
    new_x = copy_constructor(x0_p);
    best_x = copy_constructor(x0_p);
  } else {
    x = (void *) malloc (element_size);
    memcpy (x, x0_p, element_size);
    new_x = (void *) malloc (element_size);
    best_x =  (void *) malloc (element_size);
    memcpy (best_x, x0_p, element_size);
  }

  best_E = E;

  T = params.t_initial;
  T_factor = 1.0 / params.mu_t;

  if (print_position) {
    printf ("#-iter  #-evals   temperature     position   energy\n");
  }

  while (1) {

    n_accepts = 0;
    n_rejects = 0;
    n_eless = 0;

    for (i = 0; i < params.iters_fixed_T; ++i) {

      copy_state(x, new_x, element_size, copyfunc);

      take_step (r, new_x, params.step_size);
      new_E = Ef (new_x);

      if(new_E <= best_E){
        if (copyfunc) {
          copyfunc(new_x,best_x);
        } else {
          memcpy (best_x, new_x, element_size);
        }
        best_E=new_E;
      }

      ++n_evals;                /* keep track of Ef() evaluations */
      /* now take the crucial step: see if the new point is accepted
         or not, as determined by the boltzmann probability */
      if (new_E < E) {

	if (new_E < best_E) {
	  copy_state(new_x, best_x, element_size, copyfunc);
	  best_E = new_E;
	}

        /* yay! take a step */
	copy_state(new_x, x, element_size, copyfunc);
        E = new_E;
        ++n_eless;

      } else if (gsl_rng_uniform(r) < boltzmann(E, new_E, T, &params)) {
        /* yay! take a step */
	copy_state(new_x, x, element_size, copyfunc);
        E = new_E;
        ++n_accepts;

      } else {
        ++n_rejects;
      }
    }

    if (print_position) {
      /* see if we need to print stuff as we go */
      /*       printf("%5d %12g %5d %3d %3d %3d", n_iter, T, n_evals, */
      /*           100*n_eless/n_steps, 100*n_accepts/n_steps, */
      /*           100*n_rejects/n_steps); */
      printf ("%5d   %7d  %12g", n_iter, n_evals, T);
      print_position (x);
      printf ("  %12g  %12g\n", E, best_E);
    }

    /* apply the cooling schedule to the temperature */
    /* FIXME: I should also introduce a cooling schedule for the iters */
    T *= T_factor;
    ++n_iter;
    if (T < params.t_min) {
      break;
    }
  }

  /* at the end, copy the result onto the initial point, so we pass it
     back to the caller */
  copy_state(best_x, x0_p, element_size, copyfunc);

  if (copyfunc) {
    destructor(x);
    destructor(new_x);
    destructor(best_x);
  } else {
    free (x);
    free (new_x);
    free (best_x);
  }
}

/* implementation of a simulated annealing algorithm with many tries */

void 
gsl_siman_solve_many (const gsl_rng * r, void *x0_p, gsl_siman_Efunc_t Ef,
                      gsl_siman_step_t take_step,
                      gsl_siman_metric_t distance,
                      gsl_siman_print_t print_position,
                      size_t element_size,
                      gsl_siman_params_t params)
{
  /* the new set of trial points, and their energies and probabilities */
  void *x, *new_x;
  double *energies, *probs, *sum_probs;
  double Ex;                    /* energy of the chosen point */
  double T, T_factor;           /* the temperature and a step multiplier */
  int i;
  double u;                     /* throw the die to choose a new "x" */
  int n_iter;

  if (print_position) {
    printf ("#-iter    temperature       position");
    printf ("         delta_pos        energy\n");
  }

  x = (void *) malloc (params.n_tries * element_size);
  new_x = (void *) malloc (params.n_tries * element_size);
  energies = (double *) malloc (params.n_tries * sizeof (double));
  probs = (double *) malloc (params.n_tries * sizeof (double));
  sum_probs = (double *) malloc (params.n_tries * sizeof (double));

  T = params.t_initial;
  T_factor = 1.0 / params.mu_t;

  memcpy (x, x0_p, element_size);

  n_iter = 0;
  while (1)
    {
      Ex = Ef (x);
      for (i = 0; i < params.n_tries - 1; ++i)
        {                       /* only go to N_TRIES-2 */
          /* center the new_x[] around x, then pass it to take_step() */
          sum_probs[i] = 0;
          memcpy ((char *)new_x + i * element_size, x, element_size);
          take_step (r, (char *)new_x + i * element_size, params.step_size);
          energies[i] = Ef ((char *)new_x + i * element_size);
          probs[i] = boltzmann(Ex, energies[i], T, &params);
        }
      /* now add in the old value of "x", so it is a contendor */
      memcpy ((char *)new_x + (params.n_tries - 1) * element_size, x, element_size);
      energies[params.n_tries - 1] = Ex;
      probs[params.n_tries - 1] = boltzmann(Ex, energies[i], T, &params);

      /* now throw biased die to see which new_x[i] we choose */
      sum_probs[0] = probs[0];
      for (i = 1; i < params.n_tries; ++i)
        {
          sum_probs[i] = sum_probs[i - 1] + probs[i];
        }
      u = gsl_rng_uniform (r) * sum_probs[params.n_tries - 1];
      for (i = 0; i < params.n_tries; ++i)
        {
          if (u < sum_probs[i])
            {
              memcpy (x, (char *) new_x + i * element_size, element_size);
              break;
            }
        }
      if (print_position)
        {
          printf ("%5d\t%12g\t", n_iter, T);
          print_position (x);
          printf ("\t%12g\t%12g\n", distance (x, x0_p), Ex);
        }
      T *= T_factor;
      ++n_iter;
      if (T < params.t_min)
	{
	  break;
        }
    }

  /* now return the value via x0_p */
  memcpy (x0_p, x, element_size);

  /*  printf("the result is: %g (E=%g)\n", x, Ex); */

  free (x);
  free (new_x);
  free (energies);
  free (probs);
  free (sum_probs);
}
