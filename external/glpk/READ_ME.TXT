Praats/external/glpk/READ_ME.TXT
Paul Boersma, 26 June 2010
This file describes the adaptations to the glpk 4.44 sources
that are needed to make them compatible with Praat.

The .c and .h files are put into the single glpk directory.

The #include statements are flattened, e.g.
#include amd/amd.h becomes #include amd.h.

The redefinition of SIZE_T_MAX in amd_internal.h is ifndef'ed out.

In glpios09.c, char is changed to unsigned char in line 197.

GLPK is a heavy namespace polluter.
Because of a name clash with the C library in CodeWarrior
("error" is a member of the FILE structure),
I replaced the "error" macro with "mpl_error" in all glpmpl* files.


########################################################################

NOTE 1: The files named amd* are NOT part of the GLPK package, but
        are used with GLPK.

      The original code was modified according to GLPK requirements by
      Andrew Makhorin <mao@gnu.org>.
************************************************************************
AMD Version 2.2, Copyright (C) 2007 by Timothy A. Davis,
Patrick R. Amestoy, and Iain S. Duff.  All Rights Reserved.

Description:

   AMD is a set of routines for pre-ordering sparse matrices prior to
   Cholesky or LU factorization, using the approximate minimum degree
   ordering algorithm.  Written in ANSI/ISO C with a MATLAB interface,
   and in Fortran 77.

Authors:

   Timothy A. Davis (davis at cise.ufl.edu), University of Florida.
   Patrick R. Amestoy, ENSEEIHT, Toulouse, France.
   Iain S. Duff, Rutherford Appleton Laboratory, UK.

AMD License:

   Your use or distribution of AMD or any modified version of AMD
   implies that you agree to this License.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
   USA.

   Permission is hereby granted to use or copy this program under the
   terms of the GNU LGPL, provided that the Copyright, this License,
   and the Availability of the original version is retained on all
   copies.  User documentation of any code that uses this code or any
   modified version of this code must cite the Copyright, this License,
   the Availability note, and "Used by permission."  Permission to
   modify the code and to distribute modified code is granted, provided
   the Copyright, this License, and the Availability note are retained,
   and a notice that the code was modified is included.

   AMD is available under alternate licences; contact T. Davis for
   details.

Availability:

    http://www.cise.ufl.edu/research/sparse/amd


########################################################################

NOTE 2: The files named colamd* are NOT part of the GLPK package, but
        are used with GLPK.

      The original code was modified according to GLPK requirements by
      Andrew Makhorin <mao@gnu.org>.
************************************************************************
COLAMD/SYMAMD Version 2.7, Copyright (C) 1998-2007, Timothy A. Davis,
All Rights Reserved.

Description:

   colamd:  an approximate minimum degree column ordering algorithm,
            for LU factorization of symmetric or unsymmetric matrices,
            QR factorization, least squares, interior point methods for
            linear programming problems, and other related problems.

   symamd:  an approximate minimum degree ordering algorithm for
            Cholesky factorization of symmetric matrices.

Purpose:

   Colamd computes a permutation Q such that the Cholesky factorization
   of (AQ)'(AQ) has less fill-in and requires fewer floating point
   operations than A'A.  This also provides a good ordering for sparse
   partial pivoting methods, P(AQ) = LU, where Q is computed prior to
   numerical factorization, and P is computed during numerical
   factorization via conventional partial pivoting with row
   interchanges.  Colamd is the column ordering method used in SuperLU,
   part of the ScaLAPACK library.  It is also available as built-in
   function in MATLAB Version 6, available from MathWorks, Inc.
   (http://www.mathworks.com).  This routine can be used in place of
   colmmd in MATLAB.

   Symamd computes a permutation P of a symmetric matrix A such that
   the Cholesky factorization of PAP' has less fill-in and requires
   fewer floating point operations than A.  Symamd constructs a matrix
   M such that M'M has the same nonzero pattern of A, and then orders
   the columns of M using colmmd.  The column ordering of M is then
   returned as the row and column ordering P of A.

Authors:

   The authors of the code itself are Stefan I. Larimore and Timothy A.
   Davis (davis at cise.ufl.edu), University of Florida.  The algorithm
   was developed in collaboration with John Gilbert, Xerox PARC, and
   Esmond Ng, Oak Ridge National Laboratory.

Acknowledgements:

   This work was supported by the National Science Foundation, under
   grants DMS-9504974 and DMS-9803599.

License:

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   as published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
   USA.

   Permission is hereby granted to use or copy this program under the
   terms of the GNU LGPL, provided that the Copyright, this License,
   and the Availability of the original version is retained on all
   copies.  User documentation of any code that uses this code or any
   modified version of this code must cite the Copyright, this License,
   the Availability note, and "Used by permission."  Permission to
   modify the code and to distribute modified code is granted, provided
   the Copyright, this License, and the Availability note are retained,
   and a notice that the code was modified is included.

   COLAMD is also available under alternate licenses, contact T. Davis
   for details.

Availability:

   The colamd/symamd library is available at:

   http://www.cise.ufl.edu/research/sparse/colamd/

References:

   T. A. Davis, J. R. Gilbert, S. Larimore, E. Ng, An approximate
   column minimum degree ordering algorithm, ACM Transactions on
   Mathematical Software, vol. 30, no. 3., pp. 353-376, 2004.

   T. A. Davis, J. R. Gilbert, S. Larimore, E. Ng, Algorithm 836:
   COLAMD, an approximate column minimum degree ordering algorithm, ACM
   Transactions on Mathematical Software, vol. 30, no. 3., pp. 377-380,
   2004.
