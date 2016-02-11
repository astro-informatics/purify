
#ifndef PURIFY_SPARSEMAT
#define PURIFY_SPARSEMAT
#ifdef __cplusplus
extern "C" {
#endif
#include "purify_config.h"
#include "purify_types.h"
#include "purify_sparsemat.h"


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
  purify_complex_double *cvals;
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
  purify_complex_double *cvals;
  /*! Column index of each non-zero entry. */
  int *colind;
  /*! Locations in \ref vals and \ref colind of each new row. */
  int *rowptr;
} purify_sparsemat_row;


void purify_sparsemat_free(purify_sparsemat *mat);
void purify_sparsemat_explictmat(double **A, purify_sparsemat *S);
void purify_sparsemat_fwd_real(double *y, double *x, purify_sparsemat *A);
void purify_sparsemat_adj_real(double *y, double *x, purify_sparsemat *A);
void purify_sparsemat_fwd_complex(purify_complex_double *y, purify_complex_double *x, 
				  purify_sparsemat *A);
void purify_sparsemat_adj_complex(purify_complex_double *y, purify_complex_double *x, 
				  purify_sparsemat *A);

void purify_sparsemat_freer(purify_sparsemat_row *mat);
void purify_sparsemat_explictmatr(double **A, purify_sparsemat_row *S);
void purify_sparsemat_fwd_realr(double *y, double *x, purify_sparsemat_row *A);
void purify_sparsemat_adj_realr(double *y, double *x, purify_sparsemat_row *A);
void purify_sparsemat_fwd_complexr(purify_complex_double *y, purify_complex_double *x, 
          purify_sparsemat_row *A);
void purify_sparsemat_adj_complexr(purify_complex_double *y, purify_complex_double *x, 
          purify_sparsemat_row *A);
void purify_sparsemat_fwd_complexrsc(purify_complex_double *y, purify_complex_double *x, 
          purify_sparsemat_row *A, purify_complex_double *d);
void purify_sparsemat_adj_complexrsc(purify_complex_double *y, purify_complex_double *x, 
          purify_sparsemat_row *A, purify_complex_double *d);


#ifdef __cplusplus
}
#endif
#endif
