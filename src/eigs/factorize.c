/*******************************************************************************
 *   PRIMME PReconditioned Iterative MultiMethod Eigensolver
 *   Copyright (C) 2015 College of William & Mary,
 *   James R. McCombs, Eloy Romero Alcalde, Andreas Stathopoulos, Lingfei Wu
 *
 *   This file is part of PRIMME.
 *
 *   PRIMME is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   PRIMME is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *******************************************************************************
 * File: factorize.c
 *
 * Purpose - Functions to factorize and back-solve a hermitian matrix M.
 *  
 ******************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "numerical.h"
#include "factorize.h"

/******************************************************************************
 * Function UDUDecompose - This function computes an UDU decomposition of the
 *   matrix M.  See LAPACK routine dsytrf for more information on how the
 *   decomposition is performed.
 *
 *
 * Input Parameters
 * ----------------
 * M  A (numOrthoConst+numEvals) x (numOrthoConst+numEvals) array that contains 
 *    the upper triangular portion of a dimM x dimM hermitian matrix.  
 *    The leading dimension of the array is numOrthoConst+numEvals.
 *
 * dimM  The dimension of the matrix M
 *
 * rwork Real work array of dimension at least dimM.  Optimal size is dimM*NB 
 *       where NB is the block size returned by LAPACK routine ilaenv.
 * 
 *
 * Output Parameters
 * -----------------
 * UDU  Array of dimension dimM x dimM containing the UDU decomposition of M.
 *
 * ipivot  Integer array of length dimM containing pivot mapping
 *
 *
 * Return Value
 * ------------
 * int error code
 ******************************************************************************/

TEMPLATE_PLEASE
int UDUDecompose_Sprimme(SCALAR *M, int ldM, SCALAR *UDU, int ldUDU,
      int *ipivot, int dimM, SCALAR *rwork, size_t *rworkSize,
      primme_params *primme) {

   int info;

   /* TODO: this is not a proper PRIMME function, so it may belong to   */
   /* numerical.c or as a static function in init.c or restart.c.       */
   (void)primme; /* unused paramter */

   /* Quick return for M with dimension 0 */

   if (dimM == 0) return 0;

   /* if ld is zero, change by the matrix size */
   if (ldUDU == 0) ldUDU = dimM;

   /* Return memory requirement */

   if (M == NULL) {
      SCALAR w;
      Num_hetrf_Sprimme("U", dimM, UDU, ldUDU, ipivot, &w, -1, &info);
      *rworkSize = max(*rworkSize, (size_t)REAL_PART(w));
      return 0;
    }

   /* Quick return for M with dimension 1 */

   if (dimM <= 1) {
      *UDU = *M;
      info = 0;
   }
   else {

      /* Copy the upper triangular portion of M into UDU */

      Num_copy_trimatrix_Sprimme(M, dimM, dimM, ldM, 0 /* up */, 0, UDU,
            ldUDU, 0);

      /* Perform the decomposition */
      CHKERR((Num_hetrf_Sprimme("U", dimM, UDU, ldUDU, ipivot, rwork,
                  TO_INT(*rworkSize), &info), info), -1);
   }

   return 0;
}

/******************************************************************************
 * Function UDUSolve - This function solves a dense hermitian linear system
 *   given a right hand side (rhs) and a UDU factorization.
 *
 *
 * Input Parameters
 * ----------------
 * UDU     Two-dimensional of dimension dim and leading dimension dim.
 *         Contains block diagonal and multipliers necessary to construct
 *         the upper triangular matrix U.  See LAPACK routine dsytrf for more
 *         details.
 *
 * ipivot  Permutation array that determines how rows and columns of the
 *         factorization were permuted for stability.
 *
 * dim     The dimension of the linear system
 *
 * rhs     The right hand side of the linear system
 *
 * primme  Structure containing various solver parameters
 *
 *
 * Output Parameters
 * -----------------
 * sol     The solution of the linear system 
 *
 ******************************************************************************/

TEMPLATE_PLEASE
int UDUSolve_Sprimme(SCALAR *UDU, int *ipivot, int dim, SCALAR *rhs, 
   SCALAR *sol, primme_params *primme) {

   int info;

   /* TODO: this is not a proper PRIMME function, so it may belong to   */
   /* numerical.c or as a static function in init.c or restart.c.       */

   if (dim == 1) {
      *sol = *rhs/(*UDU); 
      info = 0;
   }
   else {
      Num_copy_Sprimme(dim, rhs, 1, sol, 1);
      CHKERR((Num_hetrs_Sprimme("U", dim, 1, UDU, dim, ipivot, sol, dim,
                  &info), info), -1);
   }

   return 0;

}