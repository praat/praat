/* This function computes the solution to the least squares system

   phi = [ A x =  b , lambda D x = 0 ]^2
    
   where A is an M by N matrix, D is an N by N diagonal matrix, lambda
   is a scalar parameter and b is a vector of length M.

   The function requires the factorization of A into A = Q R P^T,
   where Q is an orthogonal matrix, R is an upper triangular matrix
   with diagonal elements of non-increasing magnitude and P is a
   permuation matrix. The system above is then equivalent to

   [ R z = Q^T b, P^T (lambda D) P z = 0 ]

   where x = P z. If this system does not have full rank then a least
   squares solution is obtained.  On output the function also provides
   an upper triangular matrix S such that

   P^T (A^T A + lambda^2 D^T D) P = S^T S

   Parameters,
   
   r: On input, contains the full upper triangle of R. On output the
   strict lower triangle contains the transpose of the strict upper
   triangle of S, and the diagonal of S is stored in sdiag.  The full
   upper triangle of R is not modified.

   p: the encoded form of the permutation matrix P. column j of P is
   column p[j] of the identity matrix.

   lambda, diag: contains the scalar lambda and the diagonal elements
   of the matrix D

   qtb: contains the product Q^T b

   x: on output contains the least squares solution of the system

   wa: is a workspace of length N

   */

static int
qrsolv (gsl_matrix * r, const gsl_permutation * p, const double lambda, 
        const gsl_vector * diag, const gsl_vector * qtb, 
        gsl_vector * x, gsl_vector * sdiag, gsl_vector * wa)
{
  size_t n = r->size2;

  size_t i, j, k, nsing;

  /* Copy r and qtb to preserve input and initialise s. In particular,
     save the diagonal elements of r in x */

  for (j = 0; j < n; j++)
    {
      double rjj = gsl_matrix_get (r, j, j);
      double qtbj = gsl_vector_get (qtb, j);

      for (i = j + 1; i < n; i++)
        {
          double rji = gsl_matrix_get (r, j, i);
          gsl_matrix_set (r, i, j, rji);
        }

      gsl_vector_set (x, j, rjj);
      gsl_vector_set (wa, j, qtbj);
    }

  /* Eliminate the diagonal matrix d using a Givens rotation */

  for (j = 0; j < n; j++)
    {
      double qtbpj;

      size_t pj = gsl_permutation_get (p, j);

      double diagpj = lambda * gsl_vector_get (diag, pj);

      if (diagpj == 0)
        {
          continue;
        }

      gsl_vector_set (sdiag, j, diagpj);

      for (k = j + 1; k < n; k++)
        {
          gsl_vector_set (sdiag, k, 0.0);
        }

      /* The transformations to eliminate the row of d modify only a
         single element of qtb beyond the first n, which is initially
         zero */

      qtbpj = 0;

      for (k = j; k < n; k++)
        {
          /* Determine a Givens rotation which eliminates the
             appropriate element in the current row of d */

          double sine, cosine;

          double wak = gsl_vector_get (wa, k);
          double rkk = gsl_matrix_get (r, k, k);
          double sdiagk = gsl_vector_get (sdiag, k);

          if (sdiagk == 0)
            {
              continue;
            }

          if (fabs (rkk) < fabs (sdiagk))
            {
              double cotangent = rkk / sdiagk;
              sine = 0.5 / sqrt (0.25 + 0.25 * cotangent * cotangent);
              cosine = sine * cotangent;
            }
          else
            {
              double tangent = sdiagk / rkk;
              cosine = 0.5 / sqrt (0.25 + 0.25 * tangent * tangent);
              sine = cosine * tangent;
            }

          /* Compute the modified diagonal element of r and the
             modified element of [qtb,0] */

          {
            double new_rkk = cosine * rkk + sine * sdiagk;
            double new_wak = cosine * wak + sine * qtbpj;
            
            qtbpj = -sine * wak + cosine * qtbpj;

            gsl_matrix_set(r, k, k, new_rkk);
            gsl_vector_set(wa, k, new_wak);
          }

          /* Accumulate the transformation in the row of s */

          for (i = k + 1; i < n; i++)
            {
              double rik = gsl_matrix_get (r, i, k);
              double sdiagi = gsl_vector_get (sdiag, i);
              
              double new_rik = cosine * rik + sine * sdiagi;
              double new_sdiagi = -sine * rik + cosine * sdiagi;
              
              gsl_matrix_set(r, i, k, new_rik);
              gsl_vector_set(sdiag, i, new_sdiagi);
            }
        }

      /* Store the corresponding diagonal element of s and restore the
         corresponding diagonal element of r */

      {
        double rjj = gsl_matrix_get (r, j, j);
        double xj = gsl_vector_get(x, j);
        
        gsl_vector_set (sdiag, j, rjj);
        gsl_matrix_set (r, j, j, xj);
      }

    }

  /* Solve the triangular system for z. If the system is singular then
     obtain a least squares solution */

  nsing = n;

  for (j = 0; j < n; j++)
    {
      double sdiagj = gsl_vector_get (sdiag, j);

      if (sdiagj == 0)
        {
          nsing = j;
          break;
        }
    }

  for (j = nsing; j < n; j++)
    {
      gsl_vector_set (wa, j, 0.0);
    }

  for (k = 0; k < nsing; k++)
    {
      double sum = 0;

      j = (nsing - 1) - k;

      for (i = j + 1; i < nsing; i++)
        {
          sum += gsl_matrix_get(r, i, j) * gsl_vector_get(wa, i);
        }

      {
        double waj = gsl_vector_get (wa, j);
        double sdiagj = gsl_vector_get (sdiag, j);

        gsl_vector_set (wa, j, (waj - sum) / sdiagj);
      }
    }

  /* Permute the components of z back to the components of x */

  for (j = 0; j < n; j++)
    {
      size_t pj = gsl_permutation_get (p, j);
      double waj = gsl_vector_get (wa, j);

      gsl_vector_set (x, pj, waj);
    }

  return GSL_SUCCESS;
}
