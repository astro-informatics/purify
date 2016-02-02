/*! 
 * \file example_realdata.c
 * Image recontruction example from continuos visibilities.
 * Image dimension: 2048x2048
 * Coverage: VLA simulation with M=140918 visibilities.
 *
 */
#include "purify_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>  // Must be before fftw3.h
#include <fftw3.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <time.h> 
#ifdef _OPENMP 
  #include <omp.h>
#endif
#include PURIFY_BLAS_H
#include "purify_visibility.h"
#include "purify_sparsemat.h"
#include "purify_image.h"
#include "purify_measurement.h"
#include "purify_types.h"
#include "purify_error.h"
#include "purify_ran.h"
#include "sopt_image.h"
#include "sopt_utility.h"
#include "sopt_prox.h" 
#include "sopt_tv.h"
#include "sopt_l1.h"
#include "sopt_wavelet.h"
#include "sopt_sara.h" 
#include "sopt_ran.h" 
#include "purify_utils.h" 

#define VERBOSE 1

int main(int argc, char *argv[]) {

  
 

  int i, j, Nx, Ny, Nr, Nb;
  int seedn=54;
  double sigma;
  double a;
  double mse;
  double snr;
  double snr_out;
  double gamma=0.001;
  double aux1, aux2, aux3, aux4;
  complex double alpha;
  

  purify_image img, img_copy;
  purify_visibility_filetype filetype_vis;
  purify_image_filetype filetype_img;
  char filename_vis[PURIFY_STRLEN];
  complex double *xinc;
  complex double *y0;
  complex double *y;
  double *xout;
  double *w_l1, *w_l2;
  complex double *xoutc;
  double *dummyr;
  complex double *dummyc;

//JDM
  double dx, umax, uscale;
  //double *vis_noise_std;

  //parameters for the continuos Fourier Transform
  double *deconv;
  purify_sparsemat_row gmat;
  purify_visibility vis_test;
  purify_measurement_cparam param_m1;
  purify_measurement_cparam param_m2;
  complex double *fft_temp1;
  complex double *fft_temp2;
  void *datafwd[6];
  void *dataadj[6];
  fftw_plan planfwd;
  fftw_plan planadj;
  complex double *shifts;


  //Structures for sparsity operator
  sopt_wavelet_type *dict_types;
  sopt_sara_param param1;
  void *datas[1];

  //Structures for the opmization problems
  sopt_l1_sdmmparam param4;
  //sopt_l1_rwparam param5;

  sopt_prox_l1param param_l1param;
  sopt_l1_param_padmm param_padmm;


  clock_t start, stop;
  double t = 0.0;
  double start1, stop1;
  int dimy, dimx;
  
  // Image dimension of the zero padded image
  // (dimensions should be power of 2).


   dimx = 2048;
   dimy = 2048;


  // Define parameters.
  filetype_vis = PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS_NODUMMY;
  filetype_img = PURIFY_IMAGE_FILETYPE_FITS;
  strcpy(filename_vis, "data/vla/at166B.3C129.c0.vis");

  // Read visibilities.
  purify_visibility_readfile(&vis_test,
			     filename_vis,
			     filetype_vis); 

  // Rescale the visibilities.
  dx = 0.3; // arcsec
  dx = dx / 60.0 / 60.0 / 180.0 * PURIFY_PI; // convert to radians
  umax = 1.0 / (2.0 * dx);
  uscale = 2.0*PURIFY_PI / umax;
  for (i=0; i < vis_test.nmeas; i++) {
    vis_test.u[i] = vis_test.u[i] * uscale;
    vis_test.v[i] = vis_test.v[i] * uscale;
  }
 
  
//JDM: remove
  // Input image.
  img.fov_x = 1.0 / 180.0 * PURIFY_PI;
  img.fov_y = 1.0 / 180.0 * PURIFY_PI;
  img.nx = 2048;
  img.ny = 2048;

 

  // Define parameters for the convolutional gridding.
  param_m1.nmeas = vis_test.nmeas;
  param_m1.ny1 = dimy;
  param_m1.nx1 = dimx;
  param_m1.ofy = 1;  // Changing the oversampling factor from 2 to 1
  param_m1.ofx = 1;
  param_m1.ky = 24;  
  param_m1.kx = 24;

  // Define parameters for the convolutional gridding.
//JDM: remove one of these sets of convolution gridding parameters?
  param_m2.nmeas = vis_test.nmeas;
  param_m2.ny1 = dimy;
  param_m2.nx1 = dimx;
  param_m2.ofy = 1;
  param_m2.ofx = 1;
  param_m2.ky = 24;
  param_m2.kx = 24;

  // Define dimensional parameters.
  Nb = 9;
  Nx = dimx * dimy;
  Nr = Nb * Nx;
  Ny = vis_test.nmeas;


  // Memory allocation for the different variables.
  deconv = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(deconv);
  xinc = (complex double*)malloc((Nx) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(xinc);
  xout = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(xout);
  y = (complex double*)malloc((vis_test.nmeas) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(y);
  y0 = (complex double*)malloc((vis_test.nmeas) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(y0);
  w_l1 = (double*)malloc((Nr) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(w_l1);
  w_l2 = (double*)malloc((Ny) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(w_l2);
  xoutc = (complex double*)malloc((Nx) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(xoutc);
  shifts = (complex double*)malloc((vis_test.nmeas) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(shifts);


  dummyr = malloc(Nr * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(dummyr);
  dummyc = malloc(Nr * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(dummyc);


  // x in complex
  for (i=0; i < Nx; i++){
    xinc[i] = 0.0 + 0.0*I;
  }

 
 
  // Initialize griding matrix.
  assert((start = clock())!=-1);
  purify_measurement_init_cft(&gmat, deconv, shifts, vis_test.u, vis_test.v, &param_m1);
  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;
  printf("Time initalization: %f \n\n", t);
  
  // Memory allocation for the fft.
  i = Nx * param_m1.ofy * param_m1.ofx;
  fft_temp1 = (complex double*)malloc((i) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(fft_temp1);
  fft_temp2 = (complex double*)malloc((i) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(fft_temp2);

  // Plan FFTs.
  planfwd = fftw_plan_dft_2d(param_m1.nx1*param_m1.ofx, param_m1.ny1*param_m1.ofy, 
              fft_temp1, fft_temp1, 
              FFTW_FORWARD, FFTW_MEASURE);

  planadj = fftw_plan_dft_2d(param_m1.nx1*param_m1.ofx, param_m1.ny1*param_m1.ofy, 
              fft_temp2, fft_temp2, 
              FFTW_BACKWARD, FFTW_MEASURE);

  // Set up data for forward FFT.
  datafwd[0] = (void*)&param_m1;
  datafwd[1] = (void*)deconv;
  datafwd[2] = (void*)&gmat;
  datafwd[3] = (void*)&planfwd;
  datafwd[4] = (void*)fft_temp1;
  datafwd[5] = (void*)shifts;

  // Set up data for adjoint FFT.
  dataadj[0] = (void*)&param_m2;
  dataadj[1] = (void*)deconv;
  dataadj[2] = (void*)&gmat;
  dataadj[3] = (void*)&planadj;
  dataadj[4] = (void*)fft_temp2;
  dataadj[5] = (void*)shifts;


  printf("FFT planing complete \n\n");
  
  // Copy measured visibilities to y.
  for (i=0; i < vis_test.nmeas; i++){
    y[i] =  vis_test.y[i]; 
  } 


// Rescale the measurements and deconv operator to improve speed of CG solver.

//Estimate operator norm usin the power method.
  aux4 = purify_measurement_pow_meth(&purify_measurement_cftfwd,
                                     datafwd,
                                      &purify_measurement_cftadj,
                                      dataadj);

   printf("Operator norm = %f \n\n", aux4);

  //This is no longer needed. We just need to compute the operator norm.
  /* 
  for (i=0; i < Ny; i++) {
    y[i] = y[i] / sqrt(aux4);
  } 
  for (i=0; i < Nx; i++) {
    deconv[i] = deconv[i] / sqrt(aux4);
  } */

  // Output image.x

  img_copy.nx = param_m1.nx1;
  img_copy.ny = param_m1.ny1;
  img_copy.fov_x = img_copy.nx / (2.0 * umax);
  img_copy.fov_y = img_copy.fov_x;
  for (i=0; i < Nx; i++){
    xoutc[i] = 0.0 + 0.0*I;

  }
  

  printf("FoV = %f arcmin \n\n", img_copy.fov_x / PURIFY_PI * 180.0 * 60.0);


  // Compute dirty image.
  purify_measurement_cftadj((void*)xoutc, (void*)y, dataadj);

  // Copy dirty image pixel data to img.
  //And copy the real part to xout
  img_copy.pix = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(img_copy.pix);
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xoutc[i]);
    xout[i] = creal(xoutc[i]);
  }  
  purify_image_writefile(&img_copy, "data/vla/dirty_image_padmm.fits", filetype_img);

  
  //L2 norm of the data
  aux1 = cblas_dznrm2(Ny, (void*)y, 1);
  printf("Norm of y = %f \n\n", aux1);  


  
  //SARA structure initialization

  param1.ndict = Nb;
  param1.real = 1;  //For the admm we consider real images thus real sparsity operators

  dict_types = malloc(param1.ndict * sizeof(sopt_wavelet_type));
  PURIFY_ERROR_MEM_ALLOC_CHECK(dict_types);


  dict_types[0] = SOPT_WAVELET_DB1;
  dict_types[1] = SOPT_WAVELET_DB2;
  dict_types[2] = SOPT_WAVELET_DB3;
  dict_types[3] = SOPT_WAVELET_DB4;
  dict_types[4] = SOPT_WAVELET_DB5;
  dict_types[5] = SOPT_WAVELET_DB6;
  dict_types[6] = SOPT_WAVELET_DB7;
  dict_types[7] = SOPT_WAVELET_DB8;
  dict_types[8] = SOPT_WAVELET_Dirac;
    

  sopt_sara_initop(&param1, param_m1.ny1, param_m1.nx1, 4, dict_types);

  datas[0] = (void*)&param1;


  //Scaling constants in the diferent representation domains

  sopt_sara_analysisop((void*)dummyr, (void*)xout, datas);
  //Not needed
  
  for (i=0; i < Nr; i++) {
    dummyr[i] = fabs(dummyr[i]);
  } 

  aux2 = purify_utils_maxarray(dummyr, Nr);

  printf("Scale of gamma = %f \n\n", aux2);

 
  // Output image.
  img_copy.nx = param_m1.nx1;
  img_copy.ny = param_m1.ny1;
  img_copy.fov_x = img_copy.nx / (2.0 * umax);
  img_copy.fov_y = img_copy.fov_x;  


  //Initial solution and weights
  for (i=0; i < Nx; i++) {
      xout[i] = 0.0;
  }
  for (i=0; i < Nr; i++){   
    w_l1[i] = 1.0;
  }



  // Compute whitening weights.
  double TOL = 1e-10;
  for (i=0; i < Ny; i++) {
    w_l2[i] = creal(vis_test.noise_std[i]);
    assert(fabs(w_l2[i]) > TOL);
    w_l2[i] = 1.0 / w_l2[i];
  }
 
  

  printf("**********************\n");
  printf("BPSA reconstruction\n");
  printf("**********************\n");

  gamma = 0.001;
  a = 0.5*aux4;

  //Structure for the L1 prox
  param_l1param.verbose = 2;
  param_l1param.max_iter = 20;
  param_l1param.rel_obj = 0.05;
  param_l1param.nu = 1.0;
  param_l1param.tight = 0;
  param_l1param.pos = 1;
    
  //Structure for the L1 solver    
  param_padmm.verbose = 2;
  param_padmm.max_iter = 500;
  param_padmm.gamma = gamma*aux2/a;
  param_padmm.rel_obj = 0.0005;
  param_padmm.epsilon = sqrt(Ny + 2*sqrt(Ny));//0.1*aux1;//sqrt(Ny + 2*sqrt(Ny))*sigma;
  param_padmm.real_out = 1;
  param_padmm.real_meas = 0;
  param_padmm.paraml1 = param_l1param;
    
  param_padmm.epsilon_tol_scale = 1.001;
  param_padmm.lagrange_update_scale = 0.9;
  param_padmm.nu = a; 



  #ifdef _OPENMP 
    start1 = omp_get_wtime();
  #else
    assert((start = clock())!=-1);
  #endif
  /*
  sopt_l1_sdmm((void*)xoutc, Nx,
                   &purify_measurement_cftfwd,
                   datafwd,
                   &purify_measurement_cftadj,
                   dataadj,
                   &sopt_sara_synthesisop,
                   datas,
                   &sopt_sara_analysisop,
                   datas,
                   Nr,
                   (void*)y, Ny, w, param4);
  */
  sopt_l1_solver_padmm((void*)xout, Nx,
		       &purify_measurement_cftfwd,
		       datafwd,
		       &purify_measurement_cftadj,
		       dataadj,
		       &sopt_sara_synthesisop,
		       datas,
		       &sopt_sara_analysisop,
		       datas,
		       Nr,
		       (void*)y, Ny, w_l1, w_l2, param_padmm);
  

  #ifdef _OPENMP 
    stop1 = omp_get_wtime();
    t = stop1 - start1;
  #else
    stop = clock();
    t = (double) (stop-start)/CLOCKS_PER_SEC;
  #endif

  printf("Time BPSA: %f \n\n", t); 

 
    
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = xout[i];
    xoutc[i] = xout[i] + 0.0*I;
  }
  
  purify_image_writefile(&img_copy, "data/vla/bpsa_rec_padmm.fits", filetype_img);


  //Residual image
  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/vla/residual_padmm.fits", filetype_img);
  
  
  /*

  printf("**********************\n");
  printf("SARA reconstruction\n");
  printf("**********************\n");

  

  //Structure for the RWL1 solver    
  param5.verbose = 2;
  param5.max_iter = 5;
  param5.rel_var = 0.001;
  param5.sigma = sigma*sqrt((double)Ny/(double)Nr);
  param5.init_sol = 1;

  #ifdef _OPENMP 
    start1 = omp_get_wtime();
  #else
    assert((start = clock())!=-1);
  #endif
  sopt_l1_rwsdmm((void*)xoutc, Nx,
                   &purify_measurement_cftfwd,
                   datafwd,
                   &purify_measurement_cftadj,
                   dataadj,
                   &sopt_sara_synthesisop,
                   datas,
                   &sopt_sara_analysisop,
                   datas,
                   Nr,
                   (void*)y, Ny, param4, param5);

  #ifdef _OPENMP 
    stop1 = omp_get_wtime();
    t = stop1 - start1;
  #else
    stop = clock();
    t = (double) (stop-start)/CLOCKS_PER_SEC;
  #endif

  printf("Time SARA: %f \n\n", t); 

  
    //SNR
    for (i=0; i < Nx; i++) {
        error[i] = creal(xoutc[i])-xout[i];
    }
    mse = cblas_dnrm2(Nx, error, 1);
    a = cblas_dnrm2(Nx, xout, 1);
    snr_out = 20.0*log10(a/mse);
    printf("SNR: %f dB\n\n", snr_out);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xoutc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31sara.fits", filetype_img);

  

   //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31sarares.fits", filetype_img);

    
  */
  
  //Free all memory
  purify_image_free(&img);
  purify_image_free(&img_copy);
  free(deconv);
  purify_visibility_free(&vis_test);
  free(y);
  free(xinc);
  free(xout);
  free(w_l1);
  free(w_l2);
  free(y0);
  free(xoutc);

  sopt_sara_free(&param1);
  free(dict_types);

  free(fft_temp1);
  free(fft_temp2);
  fftw_destroy_plan(planfwd);
  fftw_destroy_plan(planadj);
  purify_sparsemat_freer(&gmat);
  free(shifts);

  free(dummyr);
  free(dummyc);


//JDM
  //free(vis_noise_std);


  return 0;

}
