/*! 
 * \file purify_measurement.c
 * Functionality to define measurement operators.
 */
#include "purify_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <complex.h>  // Must be before fftw3.h
#include <fftw3.h>
#include PURIFY_BLAS_H
#include "purify_image.h"
#include "purify_sparsemat.h"
#include "purify_visibility.h"
#include "purify_error.h"
#include "purify_types.h"
#include "purify_utils.h" 
#include "purify_measurement.h" 
#include "purify_ran.h"  


/*!
 * Compute forward Fouier transform of real signal.  A real-to-complex
 * FFT is used (for speed optimisation) but the complex output signal
 * is filled to its full size through conjugate symmetry.
 * 
 * \param[out] out (complex double*) Forward Fourier transform of input signal.
 * \param[in] in (double*) Real input signal.
 * \param[in] data 
 * - data[0] (fftw_plan*): The real-to-complex FFTW plan to use when
 *      computing the Fourier transform (passed as an input so that the
 *      FFTW can be FFTW_MEASUREd beforehand).
 * - data[1] (purify_image*): The image defining the size of the Fourier
 *      transform.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_measurement_fft_real(void *out, void *in, 
				 void **data) {

  fftw_plan *plan;
  int iu, iv, ind, ind_half;
  int iu_neg, iv_neg, ind_neg;
  double complex *y, *y_half;
  purify_image *img;

  // Cast intput pointers.
  y = (double complex*)out;
  plan = (fftw_plan*)data[0];
  img = (purify_image*)data[1];

  // Allocate space for output of real-to-complex FFT before compute
  // full plane through conjugate symmetry.
  y_half = (complex double*)malloc(img->nx*img->ny*sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(y_half);

  // Perform real-to-complex FFT.
  fftw_execute_dft_r2c(*plan, 
		       (double*)in, 
		       y_half);

  // Compute other half of complex plane through conjugate symmetry.
  for (iu = 0; iu < img->nx; iu++) {
    for (iv = 0; iv < img->ny/2+1; iv++) {

      ind_half = iu*(img->ny/2+1) + iv;
      purify_visibility_iuiv2ind(&ind, iu, iv, 
				 img->nx, img->ny);

      // Copy current data element.
      y[ind] = y_half[ind_half];

      // Compute reflected data through conjugate symmetry if
      // necessary.
      if (iu == 0 && iv == 0) {
	// Do nothing for DC component.
      } 
      else if (iu == 0) {
	// Reflect along line iu = 0.
	iv_neg = img->ny - iv;
	purify_visibility_iuiv2ind(&ind_neg, iu, iv_neg, 
				 img->nx, img->ny);
	if (ind != ind_neg) y[ind_neg] = conj(y_half[ind_half]);
      }
      else if (iv == 0) {
	// Reflect along line iu = 0.
	iu_neg = img->nx - iu;
	purify_visibility_iuiv2ind(&ind_neg, iu_neg, iv, 
				 img->nx, img->ny);
	if (ind != ind_neg) y[ind_neg] = conj(y_half[ind_half]);
      }
      else {
	// Reflect along diagonal.
	iv_neg = img->ny - iv;
	iu_neg = img->nx - iu;
	purify_visibility_iuiv2ind(&ind_neg, iu_neg, iv_neg, 
				 img->nx, img->ny);
	if (ind != ind_neg) y[ind_neg] = conj(y_half[ind_half]);
      }
    }
  }
  
  // Free temporary memory.
  free(y_half);

}


/*!
 * Compute forward Fouier transform of complex signal.
 *
 * \param[out] out (complex double*) Forward Fourier transform of input signal.
 * \param[in] in (complex double*) Complex input signal.
 * \param[in] data 
 * - data[0] (fftw_plan*): The real-to-complex FFTW plan to use when
 *      computing the Fourier transform (passed as an input so that the
 *      FFTW can be FFTW_MEASUREd beforehand).
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_measurement_fft_complex(void *out, void *in, 
				    void **data) {

  fftw_plan *plan;

  plan = (fftw_plan*)data[0];
  fftw_execute_dft(*plan, 
		   (complex double*)in, 
		   (complex double*)out);

}


/*!
 * Forward visibility masking operator.
 * 
 * \param[out] out (complex double*) Output vector of masked visibilities.
 * \param[in] in (complex double*) Input vector of full visibilities to mask.
 * \param[in] data 
 * - data[0] (purify_sparsemat*): The sparse matrix defining the
 *      masking operator.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_measurement_mask_opfwd(void *out, 
				  void *in, 
				  void **data) {

  purify_sparsemat_fwd_complex((complex double*)out, 
			       (complex double*)in,
			       (purify_sparsemat*)data[0]);

}


/*!
 * Adjoint visibility masking operator.
 * 
 * \param[out] out (complex double*) Output vector of full
 * visibilities after adjoint of masking.
 * \param[in] in (complex double*) Input vector of masked visibilities
 * prior to adjoint of masking.
 * \param[in] data 
 * - data[0] (purify_sparsemat*): The sparse matrix defining the
 *      masking operator.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_measurement_mask_opadj(void *out, 
				 void *in, 
				 void **data) {

  purify_sparsemat_adj_complex((complex double*)out, 
			       (complex double*)in,
			       (purify_sparsemat*)data[0]);

}


/*!
 * Define measurement operator (currently includes Fourier transform
 * and masking only).
 *
 * \param[out] out (complex double*) Measured visibilities.
 * \param[in] in (double*) Real input image.
 * \param[in] data 
 * - data[0] (fftw_plan*): The real-to-complex FFTW plan to use when
 *      computing the Fourier transform (passed as an input so that the
 *      FFTW can be FFTW_MEASUREd beforehand).
 * - data[1] (purify_image*): The image defining the size of the Fourier
 *      transform.
 * - data[2] (purify_sparsemat*): The sparse matrix defining the
 *      masking operator.
 *
 * \authors <a href="http://www.jasonmcewen.org">Jason McEwen</a>
 */
void purify_measurement_opfwd(void *out, void *in, void **data) {

  fftw_plan *plan;
  purify_image *img;
  purify_sparsemat *mask;

  void *data_fft[2];
  void *data_mask[1];  
  double complex* vis_full;

  plan = (fftw_plan *)data[0];
  img = (purify_image *)data[1];
  mask = (purify_sparsemat*)data[2];

  vis_full = malloc(img->nx * img->ny * sizeof(double complex));
  PURIFY_ERROR_MEM_ALLOC_CHECK(vis_full);

  data_fft[0] = (void *)plan;
  data_fft[1] = (void *)img;
  purify_measurement_fft_real((void *)vis_full, in, data_fft);

  data_mask[0] = (void *)mask;
  purify_measurement_mask_opfwd(out, (void *)vis_full, data_mask);

  free(vis_full);

}

/*!
 * Initialization for the continuos Fourier transform operator.
 * 
 * \param[out] mat (purify_sparsemat_row*) Sparse matrix containing
 * the interpolation kernels for each visibility. The matrix is 
 * stored in compressed row storage format.
 * \param[out] deconv (double*) Deconvolution kernel in real space
 * \param[out] shifts (complex double*) Vector with the shifts for computing
 *             centered Fourier transform.
 * \param[in] u (double*) u coodinates between -pi and pi
 * \param[in] v (double*) v coodinates between -pi and pi
 * \param[in] param structure storing information for the operator
 *
 * \authors Rafael Carrillo
 */
void purify_measurement_init_cft(purify_sparsemat_row *mat, 
                                 double *deconv, complex double *shifts, 
                                 double *u, double *v, 
                                 purify_measurement_cparam *param) {

  int i, j, k, l;
  int nx2, ny2;
  double u1;
  double v1;
  int g1, g2, h1, h2;
  int  g11, h11;
  double temp1, temp2, sigmax, sigmay;
  int row, st, numel;
  
  double *u2;
  double *v2;
  double *u3;
  double *v3;

  double *kernel;
  int *ptr;
  
  //Sparse matrix initialization
  nx2 = param->ofx*param->nx1;
  ny2 = param->ofy*param->ny1;

  mat->nrows = param->nmeas;
  mat->ncols = nx2*ny2;
  mat->nvals = param->kx*param->ky*param->nmeas;
  mat->real = 1;
  mat->cvals = NULL;
  numel = param->kx*param->ky;
 
  
  mat->vals = (double*)malloc(mat->nvals * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(mat->vals);
  mat->colind = (int*)malloc(mat->nvals * sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(mat->colind);
  mat->rowptr = (int*)malloc((mat->nrows + 1) * sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(mat->rowptr);


  //Discrete frequency grids
  u2 = (double*)malloc(nx2 * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(u2);
  v2 = (double*)malloc(ny2 * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(v2);
  u3 = (double*)malloc((nx2 + param->kx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(u3);
  v3 = (double*)malloc((ny2 + param->ky) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(v3);

  
  temp1 = 2*PURIFY_PI/(double)nx2;
  u2[0] = 0.0;
  for (i=1; i < nx2; i++){
    u2[i] = u2[i-1] + temp1;
  }
  u3[0] = -((double)param->kx/2.0)*temp1;
  for (i=1; i < (nx2 + param->kx); i++){
    u3[i] = u3[i-1] + temp1;
  }

  temp1 = 2*PURIFY_PI/(double)ny2;
  v2[0] = 0.0;
  for (i=1; i < ny2; i++){
    v2[i] = v2[i-1] + temp1;
  }
  v3[0] = -((double)param->ky/2.0)*temp1;
  for (i=1; i < (ny2 + param->ky); i++){
    v3[i] = v3[i-1] + temp1;
  }


  //Allocate memory for the kernel
  kernel = (double*)malloc((numel) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(kernel);

  ptr = (int*)malloc((numel) * sizeof(int));
  PURIFY_ERROR_MEM_ALLOC_CHECK(ptr);


  //Scale parameters for the Gaussian interpolation kernel
  sigmax = 1.0/(double)param->nx1;
  sigmay = 1.0/(double)param->ny1;


  //Main loop
  for (i=0; i < param->nmeas; i++){
    
    //Row pointer
    row = i*numel;
    
    //Shift by 2Pi for negative frecquencies (fftshift)
    if (u[i] < 0.0){
      u1 = u[i] + 2*PURIFY_PI;
    }
    else{
      u1 = u[i];
    }

    if (v[i] < 0.0){
      v1 = v[i] + 2*PURIFY_PI;
    }
    else{
      v1 = v[i];
    }
    
    //Find closest point in the discrete grid
    g1 = purify_utils_absearch(u2, nx2, u1);
    h1 = purify_utils_absearch(v2, ny2, v1);

    
    //Pointers in u2 and v2
    g2 = g1 + (param->kx/2);
    g1 = g1 - (param->kx/2) + 1;
    h2 = h1 + (param->ky/2);
    h1 = h1 - (param->ky/2) + 1;

 
    //Pointers in u3 and v3
    g11 = g1 + (param->kx/2);
    h11 = h1 + (param->ky/2);

    
    //Interpolation kernel evaluated on the discrete grid
    for (k=0; k < param->kx; k++){
      temp1 = (u3[g11 + k] - u1)/sigmax;
      temp1 = temp1*temp1;
      st = k*param->ky;
      for (j=0; j < param->ky; j++){
        temp2 = (v3[h11 + j] - v1)/sigmay;
        temp2 = temp2*temp2;
        temp2 = -(temp1 + temp2)/2.0;
        kernel[j + st] = exp(temp2);
      }
    }
  
    //Foldings for the circular shifts
    if ((g1 < 0)&&(h1 < 0)){
      //Case 1

      h11 = ny2+h1;
      j = param->ky - ny2;
      // 1 and 2
      for (l = 0; l <= g2; l++){
        st = l*param->ky;
        g11 = (l-g1)*param->ky;
        for (k = 0; k <= h2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k - h1 + g11];
        }
        for (k = h11; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h11 + g11];
        }
      }
      
      // 3 and 4
      for (l = nx2+g1; l <= nx2-1; l++){
        st = (l+param->kx-nx2)*param->ky;
        g11 = (l-nx2-g1)*param->ky;
        for (k = h11; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h11 + g11];
        }
        for (k = 0; k <= h2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k - h1 + g11];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    }  
    else if ((g1 < 0)&&(h1 >= 0)&&(h2 < ny2)){
      //Case 2

      //2
      for (l = 0; l <= g2; l++){
        st = l*param->ky;
        g11 = (l-g1)*param->ky;
        for (k = h1; k <= h2; k++){
          ptr[k - h1 + st] = k + l*ny2;
          mat->vals[k - h1 + st + row] = kernel[k - h1 + g11];
        }
      }
      
      //1
      for (l = nx2+g1; l <= nx2-1; l++){
        st = (l+param->kx-nx2)*param->ky;
        g11 = (l-nx2-g1)*param->ky;
        for (k = h1; k <= h2; k++){
          ptr[k - h1 + st] = k + l*ny2;
          mat->vals[k - h1 + st + row] = kernel[k - h1 + g11];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    } 
    else if ((g1 < 0)&&(h2 >= ny2)){
      //Case 3

      h11 = ny2-h1;
      j = param->ky - ny2;
      // 1 and 2
      for (l = 0; l <= g2; l++){
        st = l*param->ky;
        g11 = (l-g1)*param->ky;
        for (k = 0; k <= h2-ny2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k + h11 + g11];
        }
        for (k = h1; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h1 + g11];
        }
      }
      
      // 3 and 4
      for (l = nx2+g1; l <= nx2-1; l++){
        st = (l+param->kx-nx2)*param->ky;
        g11 = (l-nx2-g1)*param->ky;
        for (k = h1; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h1 + g11];
        }
        for (k = 0; k <= h2-ny2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k + h11 + g11];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    } 
    else if ((g1 >= 0)&&(g2 < nx2)&&(h1 < 0)){
      //Case 4
      h11 = ny2+h1;
      j = param->ky - ny2;
 
      for (l = g1; l <= g2; l++){
        st = (l - g1)*param->ky;
        //2
        for (k = 0; k <= h2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k - h1 + st];
          
        }
        //1
        for (k = h11; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h11 + st];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    }
    else if ((g1 >= 0)&&(g2 < nx2)&&(h1 >= 0)&&(h2 < ny2)){
      //Case 5

      j = 0;
      for (l = g1; l <= g2; l++){
        st = l*ny2;
        for (k = h1; k <= h2; k++){
          ptr[j] = k + st;
          j++;
        }
      }
      for (j = 0; j < numel; j++){
        mat->vals[j + row] = kernel[j];
        mat->colind[j + row] = ptr[j];
      }

    }
    else if ((g1 >= 0)&&(g2 < nx2)&&(h2 >= ny2)){
      //Case 6

      h11 = ny2-h1;
      j = param->ky - ny2;
      
      for (l = g1; l <= g2; l++){
        st = (l - g1)*param->ky;
        //2
        for (k = 0; k <= h2-ny2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k + h11 + st];
        }
        //1
        for (k = h1; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h1 + st];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    }  
    else if ((g2 >= nx2)&&(h1 < 0)){
      //Case 7

      h11 = ny2+h1;
      j = param->ky - ny2;
      // 1 and 2
      for (l = 0; l <= param->kx-nx2+g1-1; l++){
        st = l*param->ky;
        g11 = (l+nx2-g1)*param->ky;
        for (k = 0; k <= h2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k - h1 + g11];
        }
        for (k = h11; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h11 + g11];
        }
      }
      
      // 3 and 4
      for (l = g1; l <= nx2-1; l++){
        st = (l + param->kx - nx2)*param->ky;
        g11 = (l-g1)*param->ky;
        for (k = h11; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h11 + g11];
        }
        for (k = 0; k <= h2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k - h1 + g11];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    }  
    else if ((g2 >= nx2)&&(h1 >= 0)&&(h2 < ny2)){
      //Case 8

      //2
      for (l = 0; l <= param->kx-nx2+g1-1; l++){
        st = l*param->ky;
        g11 = (l+nx2-g1)*param->ky;
        for (k = h1; k <= h2; k++){
          ptr[k - h1 + st] = k + l*ny2;
          mat->vals[k - h1 + st + row] = kernel[k - h1 + g11];
        }
      }

      //1
      for (l = g1; l <= nx2-1; l++){
        st = (l + param->kx - nx2)*param->ky;
        g11 = (l-g1)*param->ky;
        for (k = h1; k <= h2; k++){
          ptr[k - h1 + st] = k + l*ny2;
          mat->vals[k - h1 + st + row] = kernel[k - h1 + g11];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    }  
    else if ((g2 >= nx2)&&(h2 >= ny2)){
      //Case 9

      h11 = ny2-h1;
      j = param->ky - ny2;
      // 1 and 2
      for (l = 0; l <= param->kx-nx2+g1-1; l++){
        st = l*param->ky;
        g11 = (l+nx2-g1)*param->ky;
        for (k = 0; k <= h2-ny2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k + h11 + g11];
        }
        for (k = h1; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h1 + g11];
        }
      }
      
      // 3 and 4
      for (l = g1; l <= nx2-1; l++){
        st = (l + param->kx - nx2)*param->ky;
        g11 = (l-g1)*param->ky;
        for (k = h1; k <= ny2-1; k++){
          ptr[k + j + st] = k + l*ny2;
          mat->vals[k + j + st + row] = kernel[k - h1 + g11];
        }
        for (k = 0; k <= h2-ny2; k++){
          ptr[k + st] = k + l*ny2;
          mat->vals[k + st + row] = kernel[k + h11 + g11];
        }
      }

      //Column pointer vector
      for (j = 0; j < numel; j++){
        mat->colind[j + row] = ptr[j];
      }

    }

    //Computation of diagonal matrix storing the shifts necessary for centered
    //Fourier transform
    temp1 = (double)param->nx1/2.0;
    temp2 = (double)param->ny1/2.0;
    shifts[i] = cexp(-I*(u[i]*temp1 + v[i]*temp2));
        
  }

  //Row pointer vector
  for (j = 0; j < mat->nrows + 1; j++){
    mat->rowptr[j] = j*numel;
  }

  //Deconvolution kernel in image domain
   
  u2[0] = -(double)param->nx1/2.0;
  for (i=1; i < param->nx1; i++){
    u2[i] = u2[i-1] + 1.0;
  }
 
  v2[0] = -(double)param->ny1/2.0;
  for (i=1; i < param->ny1; i++){
    v2[i] = v2[i-1] + 1.0;
  }
 
  for (k=0; k < param->nx1; k++){
    temp1 = u3[k]*sigmax;
    temp1 = temp1*temp1;
    st = k*param->ny1;
    for (j=0; j < param->ny1; j++){
      temp2 = v3[j]*sigmay;
      temp2 = temp2*temp2;
      temp2 = -(temp1 + temp2)/2.0;
      deconv[j + st] = 1.0/exp(temp2);
    }
  }

  //Free temporal memory
  free(u2);
  free(v2);
  free(u3);
  free(v3);
  free(kernel);
  free(ptr);

}

/*!
 * Define measurement operator for continuos visibilities
 * (currently includes continuos Fourier transform only).
 *
 * \param[out] out (complex double*) Measured visibilities.
 * \param[in] in (complex double*) Input image.
 * \param[in] data 
 * - data[0] (purify_measurement_cparam*): Parameters for the continuos
 *            Fourier transform.
 * - data[1] (double*): Matrix with the deconvolution kernel in image
 *            space.
 * - data[2] (purify_sparsemat_row*): The sparse matrix defining the
 *            convolution operator for the the interpolation.
 * - data[3] (fftw_plan*): The complex-to-complex FFTW plan to use when
 *      computing the Fourier transform (passed as an input so that the
 *      FFTW can be FFTW_MEASUREd beforehand).
 * - data[4] (complex double*) Temporal memory for the zero padding.
 * - data[5] (complex double*) Vector with the shifts for computing
 *            centered Fourier transform.
 *
 * \authors Rafael Carrillo
 */

void purify_measurement_cftfwd(void *out, void *in, void **data){

  int i, j, nx2, ny2;
  int st1, st2;
  double scale;
  purify_measurement_cparam *param;
  double *deconv;
  purify_sparsemat_row *mat;
  fftw_plan *plan;
  complex double *temp;
  complex double *xin;
  complex double *yout;
  complex double alpha;
  complex double *shifts;

  //Cast input pointers
  param = (purify_measurement_cparam*)data[0];
  deconv = (double*)data[1];
  mat = (purify_sparsemat_row*)data[2];
  plan = (fftw_plan*)data[3];
  temp = (complex double*)data[4];
  shifts = (complex double*)data[5];

  xin = (complex double*)in;
  yout = (complex double*)out;

  nx2 = param->ofx*param->nx1;
  ny2 = param->ofy*param->ny1;
  
  alpha = 0.0 + 0.0*I;
  //Zero padding and decovoluntion. 
  //Center part of the image corresponds to the original image.
  for (i=0; i < nx2*ny2; i++){
    *(temp + i) = alpha;
  }

  //Scaling
  scale = 1/sqrt((double)(nx2*ny2));

  //Offset for zero padding
  int xo,yo;

  xo = floor(nx2/2) - floor(param->nx1/2);
  yo = floor(ny2/2) - floor(param->ny1/2);

  for (j=0; j < param->nx1; j++){
    st1 = j*param->ny1;
    st2 = (j + xo)*ny2;
    for (i=0; i < param->ny1; i++){
      *(temp + st2 + i + yo) = *(xin + st1 + i)**(deconv + st1 + i)*scale;
    }
  }

  //FFT
  fftw_execute_dft(*plan, temp, temp);

  //Multiplication by the sparse matrix storing the interpolation kernel
  //and the shifts
  purify_sparsemat_fwd_complexrsc(yout, temp, mat, shifts);

}

/*!
 * Define adjoint measurement operator for continuos visibilities
 * (currently includes adjoint continuos Fourier transform only).
 *
 * \param[out] out (complex double*) Output image.
 * \param[in] in (complex double*) Input visibilities.
 * \param[in] data 
 * - data[0] (purify_measurement_cparam*): Parameters for the continuos
 *            Fourier transform.
 * - data[1] (double*): Matrix with the deconvolution kernel in image
 *            space.
 * - data[2] (purify_sparsemat_row*): The sparse matrix defining the
 *            convolution operator for the the interpolation.
 * - data[3] (fftw_plan*): The complex-to-complex FFTW plan to use when
 *            computing the inverse Fourier transform (passed as an input so 
 *            that the FFTW can be FFTW_MEASUREd beforehand).
 * - data[4] (complex double*) Temporal memory for the zero padding.
 * - data[5] (complex double*) Vector with the shifts for computing
 *            centered Fourier transform.
 *
 * \authors Rafael Carrillo
 */

void purify_measurement_cftadj(void *out, void *in, void **data){

  int i, j, nx2, ny2;
  int st1, st2;
  double scale;
  purify_measurement_cparam *param;
  double *deconv;
  purify_sparsemat_row *mat;
  fftw_plan *plan;
  complex double *temp;
  complex double *yin;
  complex double *xout;
  complex double *shifts;

  //Cast input pointers
  param = (purify_measurement_cparam*)data[0];
  deconv = (double*)data[1];
  mat = (purify_sparsemat_row*)data[2];
  plan = (fftw_plan*)data[3];
  temp = (complex double*)data[4];
  shifts = (complex double*)data[5];

  yin = (complex double*)in;
  xout = (complex double*)out;

  nx2 = param->ofx*param->nx1;
  ny2 = param->ofy*param->ny1;

  
  //Multiplication by the shifts and the adjoint of the 
  //sparse matrix storing the interpolation kernel
  purify_sparsemat_adj_complexrsc(temp, yin, mat, shifts);

  //Inverse FFT
  fftw_execute_dft(*plan, temp, temp);
  //Scaling
  scale = 1/sqrt((double)(nx2*ny2));
  
  //Cropping and decovoluntion. 
  //Center part of the image corresponds to the original image.

  //Offset for zero padding
  int xo,yo;

  xo = floor(nx2/2) - floor(param->nx1/2);
  yo = floor(ny2/2) - floor(param->ny1/2);

  for (j=0; j < param->nx1; j++){
    st1 = j*param->ny1;
    st2 = (j + xo)*ny2;
    for (i=0; i < param->ny1; i++){
      *(xout + st1 + i) = *(temp + st2 + i + yo)**(deconv + st1 + i)*scale;
    }
  }

}

/*!
 * Power method to compute the norm of the operator A.
 * 
 * \retval bound upper bound on norm of the continuos 
 * Fourier transform operator (double).
 * \param[in] A Pointer to the measurement operator.
 * \param[in] A_data Data structure associated to A.
 * \param[in] At Pointer to the the adjoint of the measurement operator.
 * \param[in] At_data Data structure associated to At.
 *
 * \authors Rafael Carrillo
 */
double purify_measurement_pow_meth(void (*A)(void *out, void *in, void **data), 
                                   void **A_data,
                                   void (*At)(void *out, void *in, void **data), 
                                   void **At_data) {

  int i, iter, nx, ny;
  int seedn = 51;
  double bound, norm, rel_ob;
  purify_measurement_cparam *param;
  complex double *y;
  complex double *x;

  
  //Cast input pointers
  param = (purify_measurement_cparam*)A_data[0];
  nx = param->nx1*param->ny1;
  ny = param->nmeas;
  iter = 0;

  y = (complex double*)malloc((ny) * sizeof( complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(y);
  x = (complex double*)malloc((nx) * sizeof( complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(x);

  if (param->nmeas > nx){
    for (i=0; i < nx; i++) {
        x[i] = purify_ran_gasdev2(seedn) + purify_ran_gasdev2(seedn)*I;
    }
    norm = cblas_dznrm2(nx, (void*)x, 1);
    for (i=0; i < nx; i++) {
        x[i] = x[i]/norm;
    }
    norm = 1.0;

    //main loop
    while (iter < 200){
      A((void*)y, (void*)x, A_data);
      At((void*)x, (void*)y, At_data);
      bound = cblas_dznrm2(nx, (void*)x, 1);
      rel_ob = (bound - norm)/norm;
      if (rel_ob <= 0.001)
        break;
      norm = bound;
      for (i=0; i < nx; i++) {
          x[i] = x[i]/norm;
      }
      iter++;
    }

  }
  else{
    for (i=0; i < ny; i++) {
        y[i] = purify_ran_gasdev2(seedn) + purify_ran_gasdev2(seedn)*I;
    }
    norm = cblas_dznrm2(ny, (void*)y, 1);
    for (i=0; i < ny; i++) {
        y[i] = y[i]/norm;
    }
    norm = 1.0;

    //main loop
    while (iter < 200){
      At((void*)x, (void*)y, At_data);
      A((void*)y, (void*)x, A_data);
      bound = cblas_dznrm2(ny, (void*)y, 1);
      rel_ob = (bound - norm)/norm;
      if (rel_ob <= 0.001)
        break;
      norm = bound;
      for (i=0; i < ny; i++) {
          y[i] = y[i]/norm;
      }
      iter++;
    }

  }

  free(y);
  free(x);

  return bound;

}

/*!
 * Define forward measurement operator for continuos visibilities
 * It takes advantage of signal reality and conjugate symmetry.
 * (currently includes adjoint continuos Fourier transform only).
 *
 * \param[out] out (complex double*) Output visibilities. 
 * \param[in] in (complex double*) Input image. Assumed real. Imaginary part
 *             set to zero.
 * \param[in] data 
 * - data[0] (purify_measurement_cparam*): Parameters for the continuos
 *            Fourier transform.
 * - data[1] (double*): Matrix with the deconvolution kernel in image
 *            space.
 * - data[2] (purify_sparsemat_row*): The sparse matrix defining the
 *            convolution operator for the the interpolation.
 * - data[3] (fftw_plan*): The complex-to-complex FFTW plan to use when
 *            computing the inverse Fourier transform (passed as an input so 
 *            that the FFTW can be FFTW_MEASUREd beforehand).
 * - data[4] (complex double*) Temporal memory for the zero padding.
 *
 * \authors Rafael Carrillo
 */

void purify_measurement_symcftfwd(void *out, void *in, void **data){

  int i, ny;
  purify_measurement_cparam *param;
  complex double *yout;

  purify_measurement_cftfwd(out, in, data);

  //Cast input pointers
  param = (purify_measurement_cparam*)data[0];
  yout = (complex double*)out;
  ny = param->nmeas;
 
   
  //Fill second half with conjugate of measurements.

  for (i=0; i < ny; i++){
    yout[i + ny] = conj(yout[i]);
    
  }

}

/*!
 * Define adjoint measurement operator for continuos visibilities
 * It takes advantage of signal reality and conjugate symmetry.
 * (currently includes adjoint continuos Fourier transform only).
 *
 * \param[out] out (complex double*) Output image. Imaginary part
 *             set to zero.
 * \param[in] in (complex double*) Input visibilities.
 * \param[in] data 
 * - data[0] (purify_measurement_cparam*): Parameters for the continuos
 *            Fourier transform.
 * - data[1] (double*): Matrix with the deconvolution kernel in image
 *            space.
 * - data[2] (purify_sparsemat_row*): The sparse matrix defining the
 *            convolution operator for the the interpolation.
 * - data[3] (fftw_plan*): The complex-to-complex FFTW plan to use when
 *            computing the inverse Fourier transform (passed as an input so 
 *            that the FFTW can be FFTW_MEASUREd beforehand).
 * - data[4] (complex double*) Temporal memory for the zero padding.
 *
 * \authors Rafael Carrillo
 */

void purify_measurement_symcftadj(void *out, void *in, void **data){

  int i, nx;
  purify_measurement_cparam *param;
  complex double *xout;

  purify_measurement_cftadj(out, in, data);

  //Cast input pointers
  param = (purify_measurement_cparam*)data[0];
  xout = (complex double*)out;
  nx = param->nx1*param->ny1;
 
   
  //Take real part and multiply by two.

  for (i=0; i < nx; i++){
    xout[i] = 2*creal(xout[i]) + 0.0*I;
    
  }

}



