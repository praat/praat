/* dht/gsl_dht.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000 Gerard Jungman
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

/* Author:  G. Jungman
 */
#ifndef __GSL_DHT_H__
#define __GSL_DHT_H__

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS


struct gsl_dht_struct {
  size_t    size;  /* size of the sample arrays to be transformed    */
  double    nu;    /* Bessel function order                          */
  double    xmax;  /* the upper limit to the x-sampling domain       */
  double    kmax;  /* the upper limit to the k-sampling domain       */
  double *  j;     /* array of computed J_nu zeros, j_{nu,s} = j[s]  */
  double *  Jjj;   /* transform numerator, J_nu(j_i j_m / j_N)       */
  double *  J2;    /* transform denominator, J_{nu+1}^2(j_m)         */
};
typedef struct gsl_dht_struct gsl_dht;


/* Create a new transform object for a given size
 * sampling array on the domain [0, xmax].
 */
gsl_dht * gsl_dht_alloc(size_t size);
gsl_dht * gsl_dht_new(size_t size, double nu, double xmax);

/* Recalculate a transform object for given values of nu, xmax.
 * You cannot change the size of the object since the internal
 * allocation is reused.
 */
int gsl_dht_init(gsl_dht * t, double nu, double xmax);

/* The n'th computed x sample point for a given transform.
 * 0 <= n <= size-1
 */
double gsl_dht_x_sample(const gsl_dht * t, int n);


/* The n'th computed k sample point for a given transform.
 * 0 <= n <= size-1
 */
double gsl_dht_k_sample(const gsl_dht * t, int n);


/* Free a transform object.
 */
void gsl_dht_free(gsl_dht * t);


/* Perform a transform on a sampled array.
 * f_in[0] ... f_in[size-1] and similarly for f_out[]
 */
int gsl_dht_apply(const gsl_dht * t, double * f_in, double * f_out);


__END_DECLS

#endif /* __GSL_DHT_H__ */
