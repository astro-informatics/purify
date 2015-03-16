/*! 
 * \file purify_visibility.h
 * Functionality to operate on vsibilities.
 */

#ifndef PURIFY_VISIBILITY
#define PURIFY_VISIBILITY
#include "purify_config.h"

#include <complex.h>
#include "purify_error.h"
#include "purify_sparsemat.h"
#include "purify_image.h"

/*!  
 * Definition of noisy visibility measurements.
 */
typedef struct {
  /*! Number of visbility measurements. */
  int nmeas;
  /*! Fourier u coordinates of visibility. */
  double *u;
  /*! Fourier v coordinates of visibility. */
  double *v;
  /*! Fourier w coordinates of visibility. */
  double *w;
  /*! Noise standard deviation of each visibility measurement. */
  complex double *noise_std;
  /*! Measured visibility value. */
  complex double *y;
} purify_visibility;


/*! Visibility filetypes supported by PURIFY. */
typedef enum
  {
    /*! PURIFY's own visibility file format. */
    PURIFY_VISIBILITY_FILETYPE_VIS = 0,
    /*! PROFILE's visibility file format that only includes u and v coordinates. */
    PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS,
    /*! PROFILE's visibility file format that only includes u and v
        coordinates (without a dummy counter). */
    PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS_NODUMMY,
    /*! PROFILE's visibility file format that also includes w coordinates. */
    PURIFY_VISIBILITY_FILETYPE_PROFILE_WIS,
  } purify_visibility_filetype;


inline void purify_visibility_iuiv2ind(int *ind, int iu, int iv, 
				       int nx, int ny) {

  if (iu >= nx || iv >= ny)
    PURIFY_ERROR_GENERIC("Visibility index too large.");

  *ind = iu * ny + iv;

}

inline void purify_visibility_ind2iuiv(int *iu, int *iv, int ind, 
				       int nx, int ny) {

  if (ind >= nx*ny)
    PURIFY_ERROR_GENERIC("Visibility index too large");

  *iu = ind / ny; // Integer division.
  *iv = ind - *iu * ny;

}

inline double purify_visibility_du(double fov) { return 1e0 / fov; }

inline double purify_visibility_umax(double fov, int n) {
  return (double)n / (double)(2.0 * fov);
}


void purify_visibility_alloc(purify_visibility *vis, int nmeas);

void purify_visibility_copy(purify_visibility *copy, 
			    purify_visibility *orig);

void purify_visibility_free(purify_visibility *vis);

int purify_visibility_compare(purify_visibility *vis1, 
			      purify_visibility *vis2, 
			      double tol);

void purify_visibility_genranmask(purify_sparsemat *mask,
				  int nmeas,
				  int nvis,
				  int seed);

void purify_visibility_mask2ivis(int **ivis,
				 purify_sparsemat *mask);

void purify_visibility_mask2vis(purify_visibility *vis,
				purify_sparsemat *mask,
				purify_image *img);

void purify_visibility_ivis2mask(purify_sparsemat *mask,
				 int *ivis,
				 int nmeas,
				 int nvis);

void purify_visibility_ivis2vis(purify_visibility *vis,
				int *ivis,
				int nmeas,
				purify_image *img);

void purify_visibility_snap2grid(purify_visibility *vis,
				 purify_image *img);

void purify_visibility_vis2mask(purify_sparsemat *mask, 
				purify_visibility *vis,
				purify_image *img);

int purify_visibility_readfile(purify_visibility *vis, 
			       const char *filename, 
			       purify_visibility_filetype filetype);

int purify_visibility_writefile(purify_visibility *vis, 
				const char *filename, 
				purify_visibility_filetype filetype);

int purify_visibility_vdsmask(purify_sparsemat *mask, 
			      purify_image *img, 
			      double p, int nmeas_tol, 
			      int maxiter_pdf, int maxiter_nmeas, 
			      int seed);

#endif
