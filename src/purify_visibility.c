/*! 
 * \file purify_visibility.c
 * Functionality to operate on vsibilities.
 */
#include "purify_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "purify_visibility.h"
#include "purify_sparsemat.h"
#include "purify_utils.h"
#include "purify_ran.h"
#include "purify_types.h"


int purify_compare_ints(const void *a, const void *b);
double purify_visibility_modifypdf(double *pdf, double *new_pdf, 
				   int num_elements, int nb_meas, 
				   int max_iter);



/*!
 * Compute 1D visibility index from 2D indices.
 * 
 * \param[out] ind 1D visibiliy index.
 * \param[in] iu First coordinate of 2D visibility index.
 * \param[in] iv Second coordinate of 2D visibility index.
 * \param[in] nx Number of visibility indices in first dimension. 
 * \param[in] ny Number of visibility indices in second dimension. 
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
extern inline void purify_visibility_iuiv2ind(int *ind, int iu, int iv, 
				       int nx, int ny);

/*!
 * Compute 2D visibility indices from 1D index.
 * 
 * \param[out] iu First coordinate of 2D visibility index.
 * \param[out] iv Second coordinate of 2D visibility index.
 * \param[in] ind 1D visibiliy index.
 * \param[in] nx Number of visibility indices in first dimension. 
 * \param[in] ny Number of visibility indices in second dimension. 
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
extern inline void purify_visibility_ind2iuiv(int *iu, int *iv, int ind, 
				       int nx, int ny);

/*!
 * Compute visibility spacing du from field-of-view.
 * 
 * \param[in] fov Field-of-view.
 * \retval du Visibility spacing.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
extern inline double purify_visibility_du(double fov);


/*!
 * Compute maximum visibility frequency from field-of-view and number
 * of samples in given dimension.
 * 
 * \param[in] fov Field-of-view.
 * \param[in] n Number of samples in given dimension.
 * \retval umax Maximum supported frequency.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
extern inline double purify_visibility_umax(double fov, int n);

/*!
 * Allocate space for nmeas visibilities.
 *
 * \param[out] vis Visibility object to allocate.
 * \param[in] nmeas Number of visibilities to allocate space for.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_alloc(purify_visibility *vis, int nmeas) {

  vis->nmeas = nmeas;
  vis->u = (double*)calloc(vis->nmeas, sizeof(double));
  vis->v = (double*)calloc(vis->nmeas, sizeof(double));
  vis->w = (double*)calloc(vis->nmeas, sizeof(double));
  vis->noise_std = (complex double*)calloc(vis->nmeas, 
					   sizeof(complex double));
  vis->y = (complex double*)calloc(vis->nmeas, 
				   sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(vis->u);
  PURIFY_ERROR_MEM_ALLOC_CHECK(vis->v);
  PURIFY_ERROR_MEM_ALLOC_CHECK(vis->w);
  PURIFY_ERROR_MEM_ALLOC_CHECK(vis->noise_std);
  PURIFY_ERROR_MEM_ALLOC_CHECK(vis->y);

}


/*!
 * Perform a deep copy of a visibility object.
 *
 * \param[out] copy Copied visibility object.
 * \param[in] orig Original visibility object to copy.
 *
 * \note Memory for the copied visibility object is allocated herein
 * and must be freed by the calling routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_copy(purify_visibility *copy, 
			    purify_visibility *orig) {

  // Allocate space.
  purify_visibility_alloc(copy, orig->nmeas);

  // Copy data.
  memcpy(copy->u, orig->u, orig->nmeas*sizeof(double));
  memcpy(copy->v, orig->v, orig->nmeas*sizeof(double));
  memcpy(copy->w, orig->w, orig->nmeas*sizeof(double));
  memcpy(copy->noise_std, orig->noise_std, 
	 orig->nmeas * sizeof(complex double));
  memcpy(copy->y, orig->y, 
	 orig->nmeas * sizeof(complex double));

}


/*!
 * Free all memory used to store visibilities.
 *
 * \param[in,out] vis Visibility object to free.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_free(purify_visibility *vis) {

  if(vis->u != NULL) free(vis->u);
  if(vis->v != NULL) free(vis->v);
  if(vis->w != NULL) free(vis->w);
  if(vis->noise_std != NULL) free(vis->noise_std);
  if(vis->y != NULL) free(vis->y);
  vis->nmeas = 0;

}


/*!
 * Compare two visibility objects to see whether they hold identical
 * data.
 *
 * \param[in] vis1 First visibility object.
 * \param[in] vis2 Second visibility object.
 * \param[in] tol Tolerance used when comparing doubles.
 * \retval comparison Zero if objects contain identical data, non-zero
 * otherwise.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
int purify_visibility_compare(purify_visibility *vis1, 
			      purify_visibility *vis2, 
			      double tol) {

  int i;

  if (vis1->nmeas != vis2->nmeas)
    return 1;

  for (i = 0; i < vis1->nmeas; i++) {

    if (fabs(vis1->u[i] - vis2->u[i]) > tol) return 1;
    if (fabs(vis1->v[i] - vis2->v[i]) > tol) return 1;
    if (fabs(vis1->w[i] - vis2->w[i]) > tol) return 1;
    if (fabs(creal(vis1->noise_std[i] - vis2->noise_std[i])) > tol) 
      return 1;
    if (fabs(cimag(vis1->noise_std[i] - vis2->noise_std[i])) > tol) 
      return 1;
    if (fabs(creal(vis1->y[i] - vis2->y[i])) > tol) 
      return 1;
    if (fabs(cimag(vis1->y[i] - vis2->y[i])) > tol) 
      return 1;

  }

  return 0;
}


/*!
 * Generate random mask.
 * 
 * \param[out] mask Sparse matrix representation of mask with
 * dimensions nmeas \f$\times\f$ nvis.
 * \param[in] nmeas Number of measurements to retain after masking.
 * \param[in] nvis Number of original visibilities.
 * \param[in] seed Seed for random number generator.
 *
 * \note Memory for the sparse matrix is allocated herein and must be
 * freed by the calling routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_genranmask(purify_sparsemat *mask,
				  int nmeas,
				  int nvis,
				  int seed) {

  int *ivis;

  // Allocate space for visibility indices.
  ivis = (int*)malloc(nmeas * sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(ivis);

  // Generate random indices of measured visibilities.
  purify_ran_knuthshuffle(ivis, nmeas, nvis, seed);

  // Sort visibility indices.
  qsort(ivis, nmeas, sizeof(int), purify_compare_ints);

  // Construct sparse matrix from indices of measured visibilities.
  purify_visibility_ivis2mask(mask, ivis, nmeas, nvis);

  // Free memory.
  free(ivis);

}

/*!
 * Generate indices of measured visibilities from mask.
 * 
 * \param[out] ivis Indices of measured visibilities.
 * \param[in] mask Sparse matrix representation of mask with
 * dimensions nmeas \f$\times\f$ nvis.
 *
 * \note Memory for the visibility indices is allocated herein and
 * must be freed by the calling routine.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_mask2ivis(int **ivis,
				 purify_sparsemat *mask) {

  int i;
  int *v;

  // Allocate space for ivis.
  v = (int*)calloc(mask->nvals, sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(v);
  *ivis = v;

  // Compute indices of measured visibilities (i.e. compute ivis).
  for (i = 1; i <= mask->ncols; i++) {
    if (mask->colptr[i] != mask->colptr[i-1]) {
      *v = i-1;
      v++;
    }
  }


}


/*!
 * Integer comparison function for qsort.
 *
 * \param[in] a First integer to compare.
 * \param[in] b Second integer to compare.
 * \retval diff Difference between integers (i.e. a - b).
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
int purify_compare_ints (const void *a, const void *b) {
  return ( *(int*)a - *(int*)b );
}


/*!
 * Compute continuous visibility locations from mask and image
 * parameters (i.e. size of image and field-of-view).
 * 
 * \param[out] vis Object containing continuous visibility locations.
 * \param[in] mask Sparse matrix representation of mask with
 * dimensions nmeas \f$\times\f$ nvis.
 * \param[in] img Image containing parameters used to compute
 * continuous visibility locations (i.e. size of image and
 * field-of-fiew).
 *
 * \note Memory for the visibilities is allocated herein and must be
 * freed by the calling routine (note space for measurements is also
 * allocated).
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_mask2vis(purify_visibility *vis,
				purify_sparsemat *mask,
				purify_image *img) {

  int *ivis;

  purify_visibility_mask2ivis(&ivis, mask);

  // Compute continuous visibilities.
  purify_visibility_ivis2vis(vis, ivis, mask->nvals, img);

  // Free memory.
  free(ivis);

}


/*!
 * Generate mask from indices of measured visibilities.
 * 
 * \note Memory for the sparse matrix is allocated herein and must be
 * freed by the calling routine.
 *
 * \param[out] mask Sparse matrix representation of mask with
 * dimensions nmeas \f$\times\f$ nvis.
 * \param[in,out] ivis Indices of measured visibilities (will be sorted
 * on ouput).
 * \param[in] nmeas Number of measurements to retain after masking.
 * \param[in] nvis Number of original visibilities.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_ivis2mask(purify_sparsemat *mask,
				 int *ivis,
				 int nmeas,
				 int nvis) {

  int i;

  // Sort visibility indices.
  qsort(ivis, nmeas, sizeof(int), purify_compare_ints);

  // Construct sparse matrix.
  mask->nrows = nmeas;
  mask->ncols = nvis;
  mask->nvals = nmeas;    

  mask->vals = (double*)malloc(mask->nvals * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(mask->vals);
  for (i = 0; i < mask->nvals; i++)
    mask->vals[i] = 1.0;

  mask->rowind = (int*)malloc(mask->nvals * sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(mask->rowind);
  for (i = 0; i < mask->nvals; i++)
    mask->rowind[i] = i;

  mask->colptr = (int*)malloc((mask->ncols+1) * sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(mask->colptr);
  
  int *ivisp;
  ivisp = ivis;
  mask->colptr[0] = 0;
  for (i = 1; i < mask->ncols; i++) {
    if (i == *ivisp + 1) {
      mask->colptr[i] = mask->colptr[i-1] + 1;
      ivisp++;
    }    
    else {
      mask->colptr[i] = mask->colptr[i-1];
    }
  }
  mask->colptr[mask->ncols] = mask->nvals;

}


/*!
 * Compute continuous visibility locations from list of visibility
 * indices and image parameters (i.e. size of image and
 * field-of-view).
 * 
 * \param[out] vis Object containing continuous visibility locations.
 * \param[in,out] ivis Indices of measured visibilities (will be sorted
 * on ouput).
 * \param[in] nmeas Number of measurements retained after masking.
 * \param[in] img Image containing parameters used to compute
 * continuous visibility locations (i.e. size of image and
 * field-of-fiew).
 *
 * \note Memory for the visibilities is allocated herein and must be
 * freed by the calling routine (note space for measurements is also
 * allocated).
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_ivis2vis(purify_visibility *vis,
				int *ivis,
				int nmeas,
				purify_image *img) {

  double du, dv, umax, vmax;
  int i, iu, iv;
  double epsilon_u, epsilon_v;

  // Sort visibility indices.
  qsort(ivis, nmeas, sizeof(int), purify_compare_ints);

  // Check maximum ivis is valid.
  if (ivis[nmeas-1] >= (img->nx * img->ny))
    PURIFY_ERROR_GENERIC("Measured visibility index too large.");

  // Allocate space for visibilities.
  purify_visibility_alloc(vis, nmeas);

  // Compute continous visibility coordinates.
  du = purify_visibility_du(img->fov_x);
  dv = purify_visibility_du(img->fov_y);
  umax = purify_visibility_umax(img->fov_x, img->nx);
  vmax = purify_visibility_umax(img->fov_y, img->ny);
  epsilon_u = du/4.0;
  epsilon_v = dv/4.0;
    
  for (i = 0; i < nmeas; i++) {

    // Get 2D index of visibility.
    purify_visibility_ind2iuiv(&iu, &iv, ivis[i], img->nx, img->ny);

    // Compute continuous coordinates.
    vis->u[i] = iu * du;
    vis->v[i] = iv * dv;
    vis->w[i] = 0.0;

    // Map visibilities greater than maximum band-limit to negative
    // frequencies.
    // Need to subtract an epsilon for the case nx (similarly ny) is
    // even.  In this case we want u=umax to be mapped to -umax
    // (i.e. we have one more negative frequency than positive
    // frequencies).  We can subtact stictly less than du/2 to ensure
    // this. For the odd case du/2 will create an additional negative
    // frequency hence we choose du/4.
    if (vis->u[i] > umax - epsilon_u)
      vis->u[i] -= 2*umax;
    if (vis->v[i] > vmax - epsilon_v)
      vis->v[i] -= 2*vmax;

  }

}


/*!
 * Snap continuous visibilities to grid.
 * 
 * \param[in,out] vis Object containing continuous visibility
 * locations. On output the u and v coordinates are snapped to lie on
 * the grid defined by the image parameters.
 * \param[in] img Image containing parameters used to compute
 * continuous visibility locations (i.e. size of image and
 * field-of-fiew).
 *
 * \note Only the u and v coordinates of the visibilities are altered
 * (the measurement y and noise standard deviation are NOT altered).
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_snap2grid(purify_visibility *vis,
				 purify_image *img) {

  int i;
  double du, dv;

  du = purify_visibility_du(img->fov_x);
  dv = purify_visibility_du(img->fov_y);

  for (i = 0; i < vis->nmeas; i++) {    
    vis->u[i] = round(vis->u[i] / du) * du;
    vis->v[i] = round(vis->v[i] / dv) * dv;
  }

}


/*!
 * Compute mask from continuous visibility locations and image
 * parameters (i.e. size of image and field-of-view).
 * 
 * \param[out] mask Sparse matrix representation of mask with
 * dimensions nmeas \f$\times\f$ nvis.
 * \param[in] vis Object containing continuous visibility locations.
 * \param[in] img Image containing parameters used to compute
 * continuous visibility locations (i.e. size of image and
 * field-of-fiew).
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_visibility_vis2mask(purify_sparsemat *mask, 
				purify_visibility *vis,
				purify_image *img) {

  int *ivis;
  int i, iu, iv, ind;
  double du, dv;

  // Allocate space for visibility indices.
  ivis = (int*)malloc(vis->nmeas * sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(ivis);

  // Construct array of visibility indices.
  du = purify_visibility_du(img->fov_x);
  dv = purify_visibility_du(img->fov_y);
  for (i = 0; i < vis->nmeas; i++) {

    // Find 2D indices of visibilities.
    iu = round(vis->u[i] / du);
    iv = round(vis->v[i] / dv);

    // Map negative frequencies to equivalent positive frequency.
    iu = (iu < 0 ? iu+img->nx : iu);
    iv = (iv < 0 ? iv+img->ny : iv);

    // Compute 1D index from 2D indices.
    purify_visibility_iuiv2ind(&ind, iu, iv, 
			       img->nx, img->ny);

    // Store 1D visibility index.
    ivis[i] = ind;

  }

  // Construct mask from visibility indices.
  purify_visibility_ivis2mask(mask, ivis, vis->nmeas, 
			      img->nx * img->ny);

  // Free memory.
  free(ivis);

}


/*!
 * Read continuous visibilities from file.
 *
 * \param[out] vis Visibilities read from the file.
 * \param[in] filename Name of the file to read.
 * \param[in] filetype Type of file to read.
 * \retval error Zero return indicates no errors.
 *
 * \note Memory for the visibilities is allocated herein and must be
 * freed by the calling routine (note space for measurements is also
 * allocated).
 * 
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
int purify_visibility_readfile(purify_visibility *vis, 
			       const char *filename, 
			       purify_visibility_filetype filetype) {


  FILE *file;
  char buffer[PURIFY_STRLEN];
  int i, nvis, itok;
  char *tok;
  char delimiters[] = " ,";

  // Open file.
  file = fopen(filename, "r");
  if (file == NULL) {
    sprintf(buffer, "Failed to open file %s", filename);
    PURIFY_ERROR_GENERIC(buffer);
  }

  // Read file to count number of visibilities to read.
  nvis = 0;
  while(fgets(buffer, PURIFY_STRLEN, file) != NULL) {
    nvis++;
    //puts(buffer);
  }

  // Allocate space for visibilities.
  purify_visibility_alloc(vis, nvis);

  // Read visibilities.
  rewind(file);
  switch (filetype) {
  case PURIFY_VISIBILITY_FILETYPE_VIS:
    i = 0;
    while(fgets(buffer, PURIFY_STRLEN, file) != NULL) {
      tok = strtok(buffer, delimiters);
      itok = 0;
      while (tok != NULL) {
	switch (itok) {
	case 0:
	  // dummy
	  break;
	case 1:
	  vis->u[i] = atof(tok);
	  break;
	case 2:
	  vis->v[i] = atof(tok);
	  break;
	case 3:
	  vis->w[i] = atof(tok);
	  break;
	case 4:
	  vis->y[i] = atof(tok);
	  break;
	case 5:
	  vis->y[i] += I * atof(tok);
	  break;
	case 6:
	  vis->noise_std[i] = atof(tok);
	  break;
	case 7:
	  vis->noise_std[i] = I * atof(tok);
	  break;
	default:
	  break;
	}
	itok++;
	tok = strtok(NULL, delimiters);
      }
      i++;
    }
    break;

  case PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS:
    i = 0;
    while(fgets(buffer, PURIFY_STRLEN, file) != NULL) {
      tok = strtok(buffer, delimiters);
      itok = 0;
      while (tok != NULL) {
	switch (itok) {
	case 0:
	  // dummy
	  break;
	case 1:
	  vis->u[i] = atof(tok);
	  break;
	case 2:
	  vis->v[i] = atof(tok);
	  break;
	case 3:
	  vis->y[i] = atof(tok);
	  break;
	case 4:
	  vis->y[i] += I * atof(tok);
	  break;
	case 5:
	  vis->noise_std[i] = (1 + I) * atof(tok) / PURIFY_SQRT2;
	  break;
	default:
	  break;
	}
	itok++;
	tok = strtok(NULL, delimiters);
      }
      i++;
    }
    break;

  case PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS_NODUMMY:
    i = 0;
    while(fgets(buffer, PURIFY_STRLEN, file) != NULL) {
        tok = strtok(buffer, delimiters);
        itok = 0;
        while (tok != NULL) {
          switch (itok) {
            case 0:
              vis->u[i] = atof(tok);
              break;
            case 1:
              vis->v[i] = atof(tok);
              break;
            case 2:
              vis->y[i] = atof(tok);
              break;
            case 3:
              vis->y[i] += I * atof(tok);
              break;
            case 4:
              vis->noise_std[i] = (1 + I) * atof(tok) / PURIFY_SQRT2;
              break;
            default:
              break;
          }
          itok++;
          tok = strtok(NULL, delimiters);
        }
        i++;
      }
      break;

  case PURIFY_VISIBILITY_FILETYPE_PROFILE_WIS:
    i = 0;
    while(fgets(buffer, PURIFY_STRLEN, file) != NULL) {
      tok = strtok(buffer, delimiters);
      itok = 0;
      while (tok != NULL) {
	switch (itok) {
	case 0:
	  // dummy
	  break;
	case 1:
	  vis->u[i] = atof(tok);
	  break;
	case 2:
	  vis->v[i] = atof(tok);
	  break;
	case 3:
	  vis->w[i] = atof(tok);
	  break;
	case 4:
	  vis->y[i] = atof(tok);
	  break;
	case 5:
	  vis->y[i] += I * atof(tok);
	  break;
	case 6:
	  vis->noise_std[i] = (1 + I) * atof(tok) / PURIFY_SQRT2;
	  break;
	default:
	  break;
	}
	itok++;
	tok = strtok(NULL, delimiters);
      }
      i++;
    }
    break;
    
  default:
    sprintf(buffer, 
	    "Visibility filetype with id %d is not supported", 
	    filetype);
    PURIFY_ERROR_GENERIC(buffer);
    break;

  }

  // Close file.
  fclose(file);

  return 0;

}


/*!
 * Write continuous visibilities to file.
 * 
 * \param[in] vis Visibilities to write to the file.
 * \param[in] filename Name of the file to write.
 * \param[in] filetype Type of file to write.
 * \retval error Zero return indicates no errors.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
int purify_visibility_writefile(purify_visibility *vis, 
				const char *filename, 
				purify_visibility_filetype filetype) {


  FILE *file;
  char buffer[PURIFY_STRLEN];
  int i;

  // Open file.
  file = fopen(filename, "w");
  if (file == NULL) {
    sprintf(buffer, "Failed to create file %s", filename);
    PURIFY_ERROR_GENERIC(buffer);
  }

  // Write file.
  switch (filetype) {

  case PURIFY_VISIBILITY_FILETYPE_VIS:
    for (i = 0; i < vis->nmeas; i++)
      fprintf(file, " %d %.10f %.10f %.10f %.10e %.10e %.10e %.10e\n", 
	      i+1, 
	      vis->u[i], vis->v[i], vis->w[i],
	      creal(vis->y[i]), cimag(vis->y[i]), 
	      creal(vis->noise_std[i]), cimag(vis->noise_std[i])) ;
    break;

  case PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS:
    for (i = 0; i < vis->nmeas; i++)
      fprintf(file, " %d %.4f %.4f %.7e %.7e %.7e\n", 
	      i+1, 
	      vis->u[i], vis->v[i], 
	      creal(vis->y[i]), cimag(vis->y[i]), 
	      cabs(vis->noise_std[i]));
    break;

  case PURIFY_VISIBILITY_FILETYPE_PROFILE_WIS:
    for (i = 0; i < vis->nmeas; i++)
      fprintf(file, " %d %.4f %.4f %.4f %.7e %.7e %.7e\n", 
	      i+1, 
	      vis->u[i], vis->v[i], vis->w[i],
	      creal(vis->y[i]), cimag(vis->y[i]), 
	      cabs(vis->noise_std[i]));
    break;

  default:
    sprintf(buffer, 
	    "Visibility filetype with id %d is not supported", 
	    filetype);
    PURIFY_ERROR_GENERIC(buffer);

    break;
  }

  // Close file.
  fclose(file);

  return 0;

}


/*!
 * Modify the pdf profile to get a determined number of measurements
 * for variable density sampling.
 * 
 * \param[in] pdf array of real numbers with the pdf profile (matrix).
 * \param[out] new_pdf new pdf profile generated by the function.
 * \param[in] num_elements number of elements in the array.
 * \param[in] nb_meas target number of measurements.
 * \param[in] max_iter maximum number of iterations allowed.
 * \retval alpha return constant of the pdf
 *
 * \authors Rafael Carrillo
 */
double purify_visibility_modifypdf(double *pdf, double *new_pdf, 
				   int num_elements, int nb_meas, 
				   int max_iter)
{
  double alpha=0;
  double alpha_min=-1;
  double alpha_max=1;
  int i;
  double max_pdf;
  double M;
  int iter=0;
	
  // Normalize the pdf profile
  max_pdf=purify_utils_maxarray(pdf, num_elements);
	
  for (i=0; i<num_elements; i++)
    {
      pdf[i]=pdf[i]/max_pdf;
    }	
	
  // Find alpha to get the desired number of measurements.	
  while (iter<max_iter) {
    alpha=(alpha_min+alpha_max)/2;
    M=0;
    for (i=0; i<num_elements; i++)
      {
	new_pdf[i]=pdf[i]+alpha;
	if (new_pdf[i]>1) {
	  new_pdf[i]=1;
	}
	if (new_pdf[i]<0) {
	  new_pdf[i]=0;
	}
	M=M+round(new_pdf[i]);
      }
    if (M>nb_meas) {
      alpha_max=alpha;
    }
    else if (M<nb_meas){
      alpha_min=alpha;
    }
    else {
      break;
    }

    iter++;
  }
  return alpha;
}


/*!
 * Generate the binary mask for variable density sampling in the discrete
 * case. We sample according to the pdf defined
 * by (1-r)^d, where d is choosen to give
 * the required number of samples.
 * 
 * \param[out] mask Sparse matrix representation of mask.
 * \param[in] img Image containing parameters defining full number of
 * visibilities (i.e. size of image).
 * \param[in] p Requested visibility coverage (0<p<1).  Note that the
 * number of actually number of measured visibilities will differ
 * slight to the requested number.
 * \param[in] nmeas_tol Tolerance on the number of actual visibilities
 * (compared to requested number).
 * \param[in] maxiter_pdf Maximum number of iterations when computing
 * d (typically set to 200).
 * \param[in] maxiter_nmeas Maximum number of iterations when testing
 * actual number of measurements.
 * \param[in] seed Positive integer seed for random number generator.
 * \retval nmeas Actual number of measurements for the VDS mask
 * constructed (may differ slightly to p*img->nx*img->ny).
 *
 * \note Memory for the mask is allocated herein and must be
 * freed by the calling routine.
 *
 * \authors Rafael Carrillo
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
int purify_visibility_vdsmask(purify_sparsemat *mask, purify_image *img, 
			      double p, int nmeas_tol, 
			      int maxiter_pdf, int maxiter_nmeas, 
			      int seed) {

  int num_el;
  int i, j, ind;
  int nmeas_req, nmeas;
  double alpha=-1;
  int iter=0;
  double d=1;
  double *r, *x, *y, *tmp;
  double dx, dy;
  double *pdf, *new_pdf;
  int *ivis;
  int N, M, Mon2;

  // Inialise sizes.
  N = img->nx;
  M = img->ny;
  num_el=N*M;
  nmeas_req=(int)round(p*num_el);
	
  // Check p<1.
  if (p >= 1 || p <= 0)
    PURIFY_ERROR_GENERIC("Require coverage percentage 0<p<1 in VDS");

  // Allocate space for the x grid.
  x = (double*)malloc(N * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(x);
	
  // Allocate space for the y grid.
  y = (double*)malloc(M * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(y);
	
  // Allocate space for the polar grid.
  r = (double*)malloc(num_el * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(r);
	
  // Allocate space for pdf profile.
  pdf = (double*)malloc(num_el * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(pdf);
	
  // Allocate space for the new pdf profile.
  new_pdf = (double*)malloc(num_el * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(new_pdf);
	
  // Construct cartesian grid centred on zero.
  dx=2.0/(double)(N-1);
  dy=2.0/(double)(M-1);	
  x[0]=-1;
  for (i=1; i<N; i++)
    x[i]=x[i-1]+dx;
  y[0]=-1;
  for (i=1; i<M; i++)
    y[i]=y[i-1]+dy;

  // iFftshift cartesian grid.
  tmp = (double*)malloc(N * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(tmp);
  purify_utils_ifftshift_1d(tmp, x, N);
  memcpy(x, tmp, N*sizeof(double));
  free(tmp);
  tmp = (double*)malloc(M * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(tmp);
  purify_utils_ifftshift_1d(tmp, y, M);
  memcpy(y, tmp, M*sizeof(double));
  free(tmp);

  // Generate polar grid.
  for (i=0; i<N; i++)
    for (j=0; j<M; j++)
      r[i+N*j]=sqrt(x[i]*x[i]+y[j]*y[j]);

  // Find d of the pdf profile to, on average, give the desired number
  // of samples.
  iter = 0;
  do {
    //Initialize the pdf profile
    for (i=0; i<num_el; i++)
      pdf[i]=pow(1-r[i],d);

    alpha = purify_visibility_modifypdf(pdf, new_pdf, 
					num_el, nmeas_req, 
					maxiter_nmeas);
    if (alpha<0)
      d=d+0.1;
    else
      d=d-0.1;
    iter++;    
  } while ( (abs(alpha)>0.01) && (iter<maxiter_pdf));

  // Construct indicies of measured visibilities.
  ivis = (int*)calloc(M*N, sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(ivis);
  purify_ran_ran2(-abs(seed));  
  iter = 0;
  do {
    ind = 0;
    Mon2 = (int)ceil(M/2.0 - 1.0);
    for (i=0; i<N; i++) 
      for (j=0; j<Mon2; j++) 
	if (i!=0 || j!=0) // Ignore dc component.
	  if (purify_ran_ran2(abs(seed))<new_pdf[i+N*j])
	    ivis[ind++] = i + N*j;
    nmeas = ind;
    iter++;
  } while (abs(nmeas - nmeas_req) > nmeas_tol && iter < maxiter_nmeas);

  // Construct sparse matrix representation of mask.
  purify_visibility_ivis2mask(mask, ivis, nmeas, num_el);
	
  // Free tempory memory.
  free(x);
  free(y);
  free(r);
  free(pdf);
  free(new_pdf);
  free(ivis);

  return nmeas;
		
}
