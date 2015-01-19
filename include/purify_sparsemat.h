
#ifndef PURIFY_SPARSEMAT
#define PURIFY_SPARSEMAT
#include "purify_config.h"

#include <complex.h>


/*!  
 * Definition of a sparse matrix stored in compressed column storage
 * format.
 *
 * \note
 * See Numerical Recipes for description of compressed column storage.
 */
typedef struct {
  /*! Number of rows. */ 
  int nrows; 
  /*! Number of columns. */
  int ncols; 
  /*! Number of non-zero elements. */
  int nvals; 
  /*! Real or complex flag. 1 if real, 0 if complex. */
  int real; 
  /*! Non-zero elements traversed column by column. Real case*/
  double *vals;
  /*! Non-zero elements traversed column by column. Real case*/
  complex double *cvals;
  /*! Row index of each non-zero entry. */
  int *rowind;
  /*! Locations in \ref vals and \ref rowind of each new column. */
  int *colptr;
} purify_sparsemat;

/*!  
 * Definition of a sparse matrix stored in compressed row storage
 * format.
 *
 * \note
 * See Numerical Recipes for description of compressed row storage.
 */
typedef struct {
  /*! Number of rows. */ 
  int nrows; 
  /*! Number of columns. */
  int ncols; 
  /*! Number of non-zero elements. */
  int nvals; 
  /*! Real or complex flag. 1 if real, 0 if complex. */
  int real; 
  /*! Non-zero elements traversed row by row. Real case */
  double *vals;
  /*! Non-zero elements traversed row by row. Complex case. */
  complex double *cvals;
  /*! Column index of each non-zero entry. */
  int *colind;
  /*! Locations in \ref vals and \ref colind of each new row. */
  int *rowptr;
} purify_sparsemat_row;


void purify_sparsemat_free(purify_sparsemat *mat);
void purify_sparsemat_explictmat(double **A, purify_sparsemat *S);
void purify_sparsemat_fwd_real(double *y, double *x, purify_sparsemat *A);
void purify_sparsemat_adj_real(double *y, double *x, purify_sparsemat *A);
void purify_sparsemat_fwd_complex(complex double *y, complex double *x, 
				  purify_sparsemat *A);
void purify_sparsemat_adj_complex(complex double *y, complex double *x, 
				  purify_sparsemat *A);

void purify_sparsemat_freer(purify_sparsemat_row *mat);
void purify_sparsemat_explictmatr(double **A, purify_sparsemat_row *S);
void purify_sparsemat_fwd_realr(double *y, double *x, purify_sparsemat_row *A);
void purify_sparsemat_adj_realr(double *y, double *x, purify_sparsemat_row *A);
void purify_sparsemat_fwd_complexr(complex double *y, complex double *x, 
          purify_sparsemat_row *A);
void purify_sparsemat_adj_complexr(complex double *y, complex double *x, 
          purify_sparsemat_row *A);
void purify_sparsemat_fwd_complexrsc(complex double *y, complex double *x, 
          purify_sparsemat_row *A, complex double *d);
void purify_sparsemat_adj_complexrsc(complex double *y, complex double *x, 
          purify_sparsemat_row *A, complex double *d);


#endif
