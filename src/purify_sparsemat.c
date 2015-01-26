/*! 
 * \file purify_sparsemat.c
 * Functionality to perform operations with sparse matrices.
 */
#include "purify_config.h"

#include <stdlib.h>
#include "purify_sparsemat.h"
#include "purify_error.h"


/*!
 * Free all memory used to store a spare matrix.
 *
 * \param[in] mat Sparse matrix to free.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_free(purify_sparsemat *mat) {

  if(mat->vals != NULL) free(mat->vals);
  if(mat->cvals != NULL) free(mat->cvals);
  if(mat->rowind != NULL) free(mat->rowind);
  if(mat->colptr != NULL) free(mat->colptr);
  mat->vals = NULL;
  mat->cvals = NULL;
  mat->rowind = NULL;
  mat->colptr = NULL;
  mat->nrows = 0;
  mat->ncols = 0;
  mat->nvals = 0;
  mat->real = 0;

}


/*!
 * Compute explicit representation of a sparse matrix.
 *
 * \param[out] A Explicit matrix representation of sparse matrix S.
 * The matrix A is stored in column-major order, i.e. can be accessed as
 * A[col_index * nrows + row_index].
 * \param[in] S Sparse matrix to representation explicitly (passed by
 * reference).
 *
 * \note Space for the output matrix A is allocated herein and must be
 * freed by the calling routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_explictmat(double **A, purify_sparsemat *S) {

  int c, rr;

  // Allocate space for explicit matrix (initialised with zeros).
  *A = (double*)calloc(S->nrows * S->ncols, sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(*A);

  // Construct explicit matrix.
  for (c = 0; c < S->ncols; c++)
    for (rr = S->colptr[c]; rr < S->colptr[c+1]; rr++)
      (*A)[c * S->nrows + S->rowind[rr]] = S->vals[rr];

}


/*!
 * Multiple a real vector by a real sparse matrix, i.e. compute \f$y =
 * A x\f$.
 *
 * \param[out] y Ouput vector of length nrows.
 * \param[in] x Input vector of length ncols.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_fwd_real(double *y, double *x, purify_sparsemat *A) {

  int r, rr, c;

  for (r = 0; r < A->nrows; r++)
    y[r] = 0.0;

  for (c = 0; c < A->ncols; c++)
    for (rr = A->colptr[c]; rr < A->colptr[c+1]; rr++)
      y[A->rowind[rr]] += A->vals[rr] * x[c];
 
}


/*!
 * Multiple a real vector by the adjoint of a real sparse matrix,
 * i.e. compute \f$y = A^H x\f$, where \f$H\f$ is the Hermitian operator.
 *
 * \param[out] y Ouput vector of length ncols.
 * \param[in] x Input vector of length nrows.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_adj_real(double *y, double *x, purify_sparsemat *A) {

  int rr, c;

  for (c = 0; c < A->ncols; c++) {
    y[c] = 0.0;
    for (rr = A->colptr[c]; rr < A->colptr[c+1]; rr++)
      y[c] += A->vals[rr] * x[A->rowind[rr]];
  }

}


/*!
 * Multiple a complex vector by a real sparse matrix, i.e. compute
 * \f$y = A x\f$.
 *
 * \param[out] y Ouput vector of length nrows.
 * \param[in] x Input vector of length ncols.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_fwd_complex(complex double *y, complex double *x, 
				  purify_sparsemat *A) {

  int r, rr, c;

  for (r = 0; r < A->nrows; r++)
    y[r] = 0.0 + 0.0*I;

  if (A->real == 1){
    for (c = 0; c < A->ncols; c++)
      for (rr = A->colptr[c]; rr < A->colptr[c+1]; rr++)
        y[A->rowind[rr]] += A->vals[rr] * x[c];
  }
  else{
    for (c = 0; c < A->ncols; c++)
      for (rr = A->colptr[c]; rr < A->colptr[c+1]; rr++)
        y[A->rowind[rr]] += A->cvals[rr] * x[c];
  }
}


/*!
 * Multiple a complex vector by the adjoint of a real sparse matrix,
 * i.e. compute \f$y = A^H x\f$, where \f$H\f$ is the Hermitian operator.
 *
 * \param[out] y Ouput vector of length ncols.
 * \param[in] x Input vector of length nrows.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_adj_complex(complex double *y, complex double *x, 
				  purify_sparsemat *A) {

  int rr, c;

  if (A->real == 1){
    for (c = 0; c < A->ncols; c++) {
      y[c] = 0.0 + 0.0*I;
      for (rr = A->colptr[c]; rr < A->colptr[c+1]; rr++)
        y[c] += A->vals[rr] * x[A->rowind[rr]];
    }
  }
  else{
    for (c = 0; c < A->ncols; c++) {
      y[c] = 0.0 + 0.0*I;
      for (rr = A->colptr[c]; rr < A->colptr[c+1]; rr++)
        y[c] += conj(A->cvals[rr]) * x[A->rowind[rr]];
    } 
  }

}

/*!
 * Free all memory used to store a spare matrix.
 *
 * \param[in] mat Sparse matrix to free.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_freer(purify_sparsemat_row *mat) {

  if(mat->vals != NULL) free(mat->vals);
  if(mat->cvals != NULL) free(mat->cvals);
  if(mat->colind != NULL) free(mat->colind);
  if(mat->rowptr != NULL) free(mat->rowptr);
  mat->nrows = 0;
  mat->ncols = 0;
  mat->nvals = 0;
  mat->real = 0;

}


/*!
 * Compute explicit representation of a sparse matrix.
 *
 * \param[out] A Explicit matrix representation of sparse matrix S.
 * The matrix A is stored in row-major order, i.e. can be accessed as
 * A[row_index * ncols + col_index].
 * \param[in] S Sparse matrix to representation explicitly (passed by
 * reference).
 *
 * \note Space for the output matrix A is allocated herein and must be
 * freed by the calling routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_explictmatr(double **A, purify_sparsemat_row *S) {

  int c, rr;

  // Allocate space for explicit matrix (initialised with zeros).
  *A = (double*)calloc(S->nrows * S->ncols, sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(*A);

  // Construct explicit matrix.
  for (c = 0; c < S->nrows; c++)
    for (rr = S->rowptr[c]; rr < S->rowptr[c+1]; rr++)
      (*A)[c * S->ncols + S->colind[rr]] = S->vals[rr];

}


/*!
 * Multiple a real vector by a real sparse matrix, i.e. compute \f$y =
 * A x\f$.
 *
 * \param[out] y Ouput vector of length nrows.
 * \param[in] x Input vector of length ncols.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_fwd_realr(double *y, double *x, purify_sparsemat_row *A) {

  int rr, c;

  for (c = 0; c < A->nrows; c++) {
    y[c] = 0.0;
    for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
      y[c] += A->vals[rr] * x[A->colind[rr]];
  }
}


/*!
 * Multiple a real vector by the adjoint of a real sparse matrix,
 * i.e. compute \f$y = A^H x\f$, where \f$H\f$ is the Hermitian operator.
 *
 * \param[out] y Ouput vector of length ncols.
 * \param[in] x Input vector of length nrows.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_adj_realr(double *y, double *x, purify_sparsemat_row *A) {

  int r, rr, c;

  for (r = 0; r < A->ncols; r++)
    y[r] = 0.0;

  for (c = 0; c < A->nrows; c++)
    for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
      y[A->colind[rr]] += A->vals[rr] * x[c];

}


/*!
 * Multiply a complex vector by a real sparse matrix, i.e. compute
 * \f$y = A x\f$.
 *
 * \param[out] y Ouput vector of length nrows.
 * \param[in] x Input vector of length ncols.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_fwd_complexr(complex double *y, complex double *x, 
          purify_sparsemat_row *A) {

  int rr, c;

  if (A->real == 1){
    for (c = 0; c < A->nrows; c++) {
      y[c] = 0.0 + 0.0*I;
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[c] += A->vals[rr] * x[A->colind[rr]];
    }
  }
  else{
    for (c = 0; c < A->nrows; c++) {
      y[c] = 0.0 + 0.0*I;
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[c] += A->cvals[rr] * x[A->colind[rr]];
    }
  }
}


/*!
 * Multiply a complex vector by the adjoint of a real sparse matrix,
 * i.e. compute \f$y = A^H x\f$, where \f$H\f$ is the Hermitian operator.
 *
 * \param[out] y Ouput vector of length ncols.
 * \param[in] x Input vector of length nrows.
 * \param[in] A Sparse matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_sparsemat_adj_complexr(complex double *y, complex double *x, 
          purify_sparsemat_row *A) {

  int r, rr, c;

  for (r = 0; r < A->ncols; r++)
    y[r] = 0.0 + 0.0*I;

  if (A->real == 1){
    for (c = 0; c < A->nrows; c++)
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[A->colind[rr]] += A->vals[rr] * x[c];
  }
  else{
    for (c = 0; c < A->nrows; c++)
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[A->colind[rr]] += conj(A->cvals[rr]) * x[c];
  }

}

/*!
 * Multiply a complex vector by a real sparse matrix and a diagonal
 * complex matrix i.e. compute
 * \f$y = DA x\f$.
 *
 * \param[out] y Ouput vector of length nrows.
 * \param[in] x Input vector of length ncols.
 * \param[in] A Sparse matrix (passed by reference).
 * \param[in] d Vector containing the elements of the diagonal
 *            matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors Rafael Carrillo
 */
void purify_sparsemat_fwd_complexrsc(complex double *y, complex double *x, 
          purify_sparsemat_row *A, complex double *d) {

  int rr, c;

  if (A->real == 1){
    for (c = 0; c < A->nrows; c++) {
      y[c] = 0.0 + 0.0*I;
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[c] += A->vals[rr] * x[A->colind[rr]];
      y[c] = d[c] * y[c];
    }
  }
  else{
    for (c = 0; c < A->nrows; c++) {
      y[c] = 0.0 + 0.0*I;
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[c] += A->cvals[rr] * x[A->colind[rr]];
      y[c] = d[c] * y[c];
    }
  }
}


/*!
 * Multiply a complex vector by the adjoint of a real sparse matrix
 * and a diagonal complex matrix i.e. computes
 * \f$y = A^H Dx\f$, where \f$H\f$ is the Hermitian operator.
 *
 * \param[out] y Ouput vector of length ncols.
 * \param[in] x Input vector of length nrows.
 * \param[in] A Sparse matrix (passed by reference).
 * \param[in] d Vector containing the elements of the diagonal
 *            matrix (passed by reference).
 *
 * \note Space for the output vector must be allocated by the calling
 * routine.
 *
 * \authors Rafael Carrillo
 */
void purify_sparsemat_adj_complexrsc(complex double *y, complex double *x, 
          purify_sparsemat_row *A, complex double *d) {

  int r, rr, c;
  complex double temp;

  for (r = 0; r < A->ncols; r++)
    y[r] = 0.0 + 0.0*I;

  if (A->real == 1){
    for (c = 0; c < A->nrows; c++){
      temp = conj(d[c]) * x[c];
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[A->colind[rr]] += A->vals[rr] * temp;
    }
  }
  else{
    for (c = 0; c < A->nrows; c++){
      temp = conj(d[c]) * x[c];
      for (rr = A->rowptr[c]; rr < A->rowptr[c+1]; rr++)
        y[A->colind[rr]] += conj(A->cvals[rr]) * temp;
    }
  }

}
