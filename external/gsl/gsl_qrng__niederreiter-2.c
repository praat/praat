/* Author: G. Jungman
 */
/* Implement Niederreiter base 2 generator.
 * See:
 *   Bratley, Fox, Niederreiter, ACM Trans. Model. Comp. Sim. 2, 195 (1992)
 */
#include "gsl__config.h"
#include "gsl_qrng.h"


#define NIED2_CHARACTERISTIC 2
#define NIED2_BASE 2

#define NIED2_MAX_DIMENSION 12
#define NIED2_MAX_PRIM_DEGREE 5
#define NIED2_MAX_DEGREE 50

#define NIED2_BIT_COUNT 30
#define NIED2_NBITS (NIED2_BIT_COUNT+1)

#define MAXV (NIED2_NBITS + NIED2_MAX_DEGREE)

/* Z_2 field operations */
#define NIED2_ADD(x,y) (((x)+(y))%2)
#define NIED2_MUL(x,y) (((x)*(y))%2)
#define NIED2_SUB(x,y) NIED2_ADD((x),(y))


static size_t nied2_state_size(unsigned int dimension);
static int nied2_init(void * state, unsigned int dimension);
static int nied2_get(void * state, unsigned int dimension, double * v);


static const gsl_qrng_type nied2_type = 
{
  "niederreiter-base-2",
  NIED2_MAX_DIMENSION,
  nied2_state_size,
  nied2_init,
  nied2_get
};

const gsl_qrng_type * gsl_qrng_niederreiter_2 = &nied2_type;

/* primitive polynomials in binary encoding */
static const int primitive_poly[NIED2_MAX_DIMENSION+1][NIED2_MAX_PRIM_DEGREE+1] =
{
  { 1, 0, 0, 0, 0, 0 },  /*  1               */
  { 0, 1, 0, 0, 0, 0 },  /*  x               */
  { 1, 1, 0, 0, 0, 0 },  /*  1 + x           */
  { 1, 1, 1, 0, 0, 0 },  /*  1 + x + x^2     */
  { 1, 1, 0, 1, 0, 0 },  /*  1 + x + x^3     */
  { 1, 0, 1, 1, 0, 0 },  /*  1 + x^2 + x^3   */
  { 1, 1, 0, 0, 1, 0 },  /*  1 + x + x^4     */
  { 1, 0, 0, 1, 1, 0 },  /*  1 + x^3 + x^4   */
  { 1, 1, 1, 1, 1, 0 },  /*  1 + x + x^2 + x^3 + x^4   */
  { 1, 0, 1, 0, 0, 1 },  /*  1 + x^2 + x^5             */
  { 1, 0, 0, 1, 0, 1 },  /*  1 + x^3 + x^5             */
  { 1, 1, 1, 1, 0, 1 },  /*  1 + x + x^2 + x^3 + x^5   */
  { 1, 1, 1, 0, 1, 1 }   /*  1 + x + x^2 + x^4 + x^5   */
};

/* degrees of primitive polynomials */
static const int poly_degree[NIED2_MAX_DIMENSION+1] =
{
  0, 1, 1, 2, 3, 3, 4, 4, 4, 5, 5, 5, 5
};


typedef struct
{
  unsigned int sequence_count;
  int cj[NIED2_NBITS][NIED2_MAX_DIMENSION];
  int nextq[NIED2_MAX_DIMENSION];
} nied2_state_t;


static size_t nied2_state_size(unsigned int dimension)
{
  return sizeof(nied2_state_t);
}


/* Multiply polynomials over Z_2.
 * Notice use of a temporary vector,
 * side-stepping aliasing issues when
 * one of inputs is the same as the output
 * [especially important in the original fortran version, I guess].
 */
static void poly_multiply(
  const int pa[], int pa_degree,
  const int pb[], int pb_degree,
  int pc[], int  * pc_degree
  )
{
  int j, k;
  int pt[NIED2_MAX_DEGREE+1];
  int pt_degree = pa_degree + pb_degree;

  for(k=0; k<=pt_degree; k++) {
    int term = 0;
    for(j=0; j<=k; j++) {
      const int conv_term = NIED2_MUL(pa[k-j], pb[j]);
      term = NIED2_ADD(term, conv_term);
    }
    pt[k] = term;
  }

  for(k=0; k<=pt_degree; k++) {
    pc[k] = pt[k];
  }
  for(k=pt_degree+1; k<=NIED2_MAX_DEGREE; k++) {
    pc[k] = 0;
  }

  *pc_degree = pt_degree;
}


/* Calculate the values of the constants V(J,R) as
 * described in BFN section 3.3.
 *
 *   px = appropriate irreducible polynomial for current dimension
 *   pb = polynomial defined in section 2.3 of BFN.
 * pb is modified
 */
static void calculate_v(
  const int px[], int px_degree,
  int pb[], int * pb_degree,
  int v[], int maxv
  )
{
  const int nonzero_element = 1;    /* nonzero element of Z_2  */
  const int arbitrary_element = 1;  /* arbitray element of Z_2 */

  /* The polynomial ph is px**(J-1), which is the value of B on arrival.
   * In section 3.3, the values of Hi are defined with a minus sign:
   * don't forget this if you use them later !
   */
  int ph[NIED2_MAX_DEGREE+1];
  /* int ph_degree = *pb_degree; */
  int bigm = *pb_degree;      /* m from section 3.3 */
  int m;                      /* m from section 2.3 */
  int r, k, kj;

  for(k=0; k<=NIED2_MAX_DEGREE; k++) {
    ph[k] = pb[k];
  }

  /* Now multiply B by PX so B becomes PX**J.
   * In section 2.3, the values of Bi are defined with a minus sign :
   * don't forget this if you use them later !
   */
   poly_multiply(px, px_degree, pb, *pb_degree, pb, pb_degree);
   m = *pb_degree;

  /* Now choose a value of Kj as defined in section 3.3.
   * We must have 0 <= Kj < E*J = M.
   * The limit condition on Kj does not seem very relevant
   * in this program.
   */
  /* Quoting from BFN: "Our program currently sets each K_q
   * equal to eq. This has the effect of setting all unrestricted
   * values of v to 1."
   * Actually, it sets them to the arbitrary chosen value.
   * Whatever.
   */
  kj = bigm;

  /* Now choose values of V in accordance with
   * the conditions in section 3.3.
   */
  for(r=0; r<kj; r++) {
    v[r] = 0;
  }
  v[kj] = 1;


  if(kj >= bigm) {
    for(r=kj+1; r<m; r++) {
      v[r] = arbitrary_element;
    }
  }
  else {
    /* This block is never reached. */

    int term = NIED2_SUB(0, ph[kj]);

    for(r=kj+1; r<bigm; r++) {
      v[r] = arbitrary_element;

      /* Check the condition of section 3.3,
       * remembering that the H's have the opposite sign.  [????????]
       */
      term = NIED2_SUB(term, NIED2_MUL(ph[r], v[r]));
    }

    /* Now v[bigm] != term. */
    v[bigm] = NIED2_ADD(nonzero_element, term);

    for(r=bigm+1; r<m; r++) {
      v[r] = arbitrary_element;
    }
  }

  /* Calculate the remaining V's using the recursion of section 2.3,
   * remembering that the B's have the opposite sign.
   */
  for(r=0; r<=maxv-m; r++) {
    int term = 0;
    for(k=0; k<m; k++) {
      term = NIED2_SUB(term, NIED2_MUL(pb[k], v[r+k]));
    }
    v[r+m] = term;
  }
}


static void calculate_cj(nied2_state_t * ns, unsigned int dimension)
{
  int ci[NIED2_NBITS][NIED2_NBITS];
  int v[MAXV+1];
  int r;
  unsigned int i_dim;

  for(i_dim=0; i_dim<dimension; i_dim++) {

    const int poly_index = i_dim + 1;
    int j, k;

    /* Niederreiter (page 56, after equation (7), defines two
     * variables Q and U.  We do not need Q explicitly, but we
     * do need U.
     */
    int u = 0;

    /* For each dimension, we need to calculate powers of an
     * appropriate irreducible polynomial, see Niederreiter
     * page 65, just below equation (19).
     * Copy the appropriate irreducible polynomial into PX,
     * and its degree into E.  Set polynomial B = PX ** 0 = 1.
     * M is the degree of B.  Subsequently B will hold higher
     * powers of PX.
     */
    int pb[NIED2_MAX_DEGREE+1];
    int px[NIED2_MAX_DEGREE+1];
    int px_degree = poly_degree[poly_index];
    int pb_degree = 0;

    for(k=0; k<=px_degree; k++) {
      px[k] = primitive_poly[poly_index][k];
      pb[k] = 0;
    }

    for (;k<NIED2_MAX_DEGREE+1;k++) {
      px[k] = 0;
      pb[k] = 0;
    }

    pb[0] = 1;

    for(j=0; j<NIED2_NBITS; j++) {

      /* If U = 0, we need to set B to the next power of PX
       * and recalculate V.
       */
      if(u == 0) calculate_v(px, px_degree, pb, &pb_degree, v, MAXV);

      /* Now C is obtained from V.  Niederreiter
       * obtains A from V (page 65, near the bottom), and then gets
       * C from A (page 56, equation (7)).  However this can be done
       * in one step.  Here CI(J,R) corresponds to
       * Niederreiter's C(I,J,R).
       */
      for(r=0; r<NIED2_NBITS; r++) {
        ci[r][j] = v[r+u];
      }

      /* Advance Niederreiter's state variables. */
      ++u;
      if(u == px_degree) u = 0;
    }

    /* The array CI now holds the values of C(I,J,R) for this value
     * of I.  We pack them into array CJ so that CJ(I,R) holds all
     * the values of C(I,J,R) for J from 1 to NBITS.
     */
    for(r=0; r<NIED2_NBITS; r++) {
      int term = 0;
      for(j=0; j<NIED2_NBITS; j++) {
        term = 2*term + ci[r][j];
      }
      ns->cj[r][i_dim] = term;
    }

  }
}


static int nied2_init(void * state, unsigned int dimension)
{
  nied2_state_t * n_state = (nied2_state_t *) state;
  unsigned int i_dim;

  if(dimension < 1 || dimension > NIED2_MAX_DIMENSION) return GSL_EINVAL;

  calculate_cj(n_state, dimension);

  for(i_dim=0; i_dim<dimension; i_dim++) n_state->nextq[i_dim] = 0;
  n_state->sequence_count = 0;

  return GSL_SUCCESS;
}


static int nied2_get(void * state, unsigned int dimension, double * v)
{
  static const double recip = 1.0/(double)(1U << NIED2_NBITS); /* 2^(-nbits) */
  nied2_state_t * n_state = (nied2_state_t *) state;
  int r;
  int c;
  unsigned int i_dim;

  /* Load the result from the saved state. */
  for(i_dim=0; i_dim<dimension; i_dim++) {
    v[i_dim] = n_state->nextq[i_dim] * recip;
  }

  /* Find the position of the least-significant zero in sequence_count.
   * This is the bit that changes in the Gray-code representation as
   * the count is advanced.
   */
  r = 0;
  c = n_state->sequence_count;
  while(1) {
    if((c % 2) == 1) {
      ++r;
      c /= 2;
    }
    else break;
  }

  if(r >= NIED2_NBITS) return GSL_EFAILED; /* FIXME: better error code here */

  /* Calculate the next state. */
  for(i_dim=0; i_dim<dimension; i_dim++) {
    n_state->nextq[i_dim] ^= n_state->cj[r][i_dim];
  }

  n_state->sequence_count++;

  return GSL_SUCCESS;
}
