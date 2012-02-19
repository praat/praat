/* Author: G. Jungman
 */
/* Implementation for Sobol generator.
 * See
 *   [Bratley+Fox, TOMS 14, 88 (1988)]
 *   [Antonov+Saleev, USSR Comput. Maths. Math. Phys. 19, 252 (1980)]
 */
#include "gsl__config.h"
#include "gsl_qrng.h"


/* maximum allowed space dimension */
#define SOBOL_MAX_DIMENSION 40

/* bit count; assumes sizeof(int) >= 32-bit */
#define SOBOL_BIT_COUNT 30

/* prototypes for generator type functions */
static size_t sobol_state_size(unsigned int dimension);
static int sobol_init(void * state, unsigned int dimension);
static int sobol_get(void * state, unsigned int dimension, double * v);

/* global Sobol generator type object */
static const gsl_qrng_type sobol_type = 
{
  "sobol",
  SOBOL_MAX_DIMENSION,
  sobol_state_size,
  sobol_init,
  sobol_get
};
const gsl_qrng_type * gsl_qrng_sobol = &sobol_type;


/* primitive polynomials in binary encoding
 */
static const int primitive_polynomials[SOBOL_MAX_DIMENSION] = 
{
  1,     3,   7,  11,  13,  19,  25,  37,  59,  47,
  61,   55,  41,  67,  97,  91, 109, 103, 115, 131,
  193, 137, 145, 143, 241, 157, 185, 167, 229, 171,
  213, 191, 253, 203, 211, 239, 247, 285, 369, 299
};

/* degrees of the primitive polynomials */
static const int degree_table[SOBOL_MAX_DIMENSION] = 
{
  0, 1, 2, 3, 3, 4, 4, 5, 5, 5,
  5, 5, 5, 6, 6, 6, 6, 6, 6, 7,
  7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
  7, 7, 7, 7, 7, 7, 7, 8, 8, 8
};


/* initial values for direction tables, following
 * Bratley+Fox, taken from [Sobol+Levitan, preprint 1976]
 */
static const int v_init[8][SOBOL_MAX_DIMENSION] =
{
  {
    0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
  },
  {
    0, 0, 1, 3, 1, 3, 1, 3, 3, 1,
    3, 1, 3, 1, 3, 1, 1, 3, 1, 3,
    1, 3, 1, 3, 3, 1, 3, 1, 3, 1,
    3, 1, 1, 3, 1, 3, 1, 3, 1, 3
  }, 
  {
    0, 0, 0, 7, 5, 1, 3, 3, 7, 5,
    5, 7, 7, 1, 3, 3, 7, 5, 1, 1,
    5, 3, 3, 1, 7, 5, 1, 3, 3, 7,
    5, 1, 1, 5, 7, 7, 5, 1, 3, 3
  }, 
  {
    0,  0,  0,  0,  0,  1,  7,  9, 13, 11,
    1,  3,  7,  9,  5, 13, 13, 11,  3, 15,
    5,  3, 15,  7,  9, 13,  9,  1, 11,  7,
    5, 15,  1, 15, 11,  5,  3,  1,  7,  9
  }, 
  {
     0,  0,  0,  0,  0,  0,  0,  9,  3, 27,
    15, 29, 21, 23, 19, 11, 25,  7, 13, 17,
     1, 25, 29,  3, 31, 11,  5, 23, 27, 19,
    21,  5,  1, 17, 13,  7, 15,  9, 31,  9
  }, 
  {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
     0,  0,  0, 37, 33,  7,  5, 11, 39, 63,
    27, 17, 15, 23, 29,  3, 21, 13, 31, 25,
     9, 49, 33, 19, 29, 11, 19, 27, 15, 25
  }, 
  {
     0,   0,  0,  0,  0,  0,    0,  0,  0,   0,
     0,   0,  0,  0,  0,  0,    0,  0,  0,  13,
    33, 115, 41, 79, 17,  29, 119, 75, 73, 105,
     7,  59, 65, 21,  3, 113,  61, 89, 45, 107
  }, 
  {
    0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    0, 0, 0, 0, 0, 0, 0, 0,  0,  0,
    0, 0, 0, 0, 0, 0, 0, 7, 23, 39
  }
};


/* Sobol generator state.
 *   sequence_count       = number of calls with this generator
 *   last_numerator_vec   = last generated numerator vector
 *   last_denominator_inv = 1/denominator for last numerator vector
 *   v_direction          = direction number table
 */
typedef struct
{
  unsigned int  sequence_count;
  double        last_denominator_inv;
  int           last_numerator_vec[SOBOL_MAX_DIMENSION];
  int           v_direction[SOBOL_BIT_COUNT][SOBOL_MAX_DIMENSION];
} sobol_state_t;

/* NOTE: I fixed the size for the preliminary implementation.
   This could/should be relaxed, as an optimization.
 */

static size_t sobol_state_size(unsigned int dimension)
{
  return sizeof(sobol_state_t);
}


static int sobol_init(void * state, unsigned int dimension)
{
  sobol_state_t * s_state = (sobol_state_t *) state;
  unsigned int i_dim;
  int j, k;
  int ell;

  if(dimension < 1 || dimension > SOBOL_MAX_DIMENSION) {
    return GSL_EINVAL;
  }

  /* Initialize direction table in dimension 0. */
  for(k=0; k<SOBOL_BIT_COUNT; k++) s_state->v_direction[k][0] = 1;

  /* Initialize in remaining dimensions. */
  for(i_dim=1; i_dim<dimension; i_dim++) {

    const int poly_index = i_dim;
    const int degree_i = degree_table[poly_index];
    int includ[8];

    /* Expand the polynomial bit pattern to separate
     * components of the logical array includ[].
     */
    int p_i = primitive_polynomials[poly_index];
    for(k = degree_i-1; k >= 0; k--) {
      includ[k] = ((p_i % 2) == 1);
      p_i /= 2;
    }

    /* Leading elements for dimension i come from v_init[][]. */
    for(j=0; j<degree_i; j++) s_state->v_direction[j][i_dim] = v_init[j][i_dim];

    /* Calculate remaining elements for this dimension,
     * as explained in Bratley+Fox, section 2.
     */
    for(j=degree_i; j<SOBOL_BIT_COUNT; j++) {
      int newv = s_state->v_direction[j-degree_i][i_dim];
      ell = 1;
      for(k=0; k<degree_i; k++) {
        ell *= 2;
        if(includ[k]) newv ^= (ell * s_state->v_direction[j-k-1][i_dim]);
      }
      s_state->v_direction[j][i_dim] = newv;
    }
  }

  /* Multiply columns of v by appropriate power of 2. */
  ell = 1;
  for(j=SOBOL_BIT_COUNT-1-1; j>=0; j--) {
    ell *= 2;
    for(i_dim=0; i_dim<dimension; i_dim++) {
      s_state->v_direction[j][i_dim] *= ell;
    }
  }

  /* 1/(common denominator of the elements in v_direction) */
  s_state->last_denominator_inv = 1.0 /(2.0 * ell);

  /* final setup */
  s_state->sequence_count = 0;
  for(i_dim=0; i_dim<dimension; i_dim++) s_state->last_numerator_vec[i_dim] = 0;

  return GSL_SUCCESS;
}


static int sobol_get(void * state, unsigned int dimension, double * v)
{
  sobol_state_t * s_state = (sobol_state_t *) state;

  unsigned int i_dimension;

  /* Find the position of the least-significant zero in count. */
  int ell = 0;
  int c = s_state->sequence_count;
  while(1) {
    ++ell;
    if((c % 2) == 1) c /= 2;
    else break;
  }

  /* Check for exhaustion. */
  if(ell > SOBOL_BIT_COUNT) return GSL_EFAILED; /* FIXME: good return code here */

  for(i_dimension=0; i_dimension<dimension; i_dimension++) {
    const int direction_i     = s_state->v_direction[ell-1][i_dimension];
    const int old_numerator_i = s_state->last_numerator_vec[i_dimension];
    const int new_numerator_i = old_numerator_i ^ direction_i;
    s_state->last_numerator_vec[i_dimension] = new_numerator_i;
    v[i_dimension] = new_numerator_i * s_state->last_denominator_inv;
  }

  s_state->sequence_count++;

  return GSL_SUCCESS;
}
