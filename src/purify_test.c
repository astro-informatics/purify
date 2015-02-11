/*! 
 * \file purify_test.c
 * Test program to check purify routines.
 *
 */
#include "purify_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <complex.h>  // Must be before fftw3.h
#include <fftw3.h>
#include <math.h>
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
  double aux1, aux2;
  

  purify_image img, img_copy;
  purify_visibility_filetype filetype_vis;
  purify_image_filetype filetype_img;
  complex double *xinc;
  complex double *y0;
  complex double *y;
  complex double *noise;
  double *xout;
  double *w;
  double *error;
  complex double *xoutc;
  double *dummyr;
  complex double *dummyc;
  complex double *shifts;

  
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

  //Structures for sparsity operator
  sopt_wavelet_type *dict_types;
  sopt_sara_param param1;
  void *datas[1];
  

  //Structures for the opmization problems
  sopt_l1_sdmmparam param2;

  clock_t start, stop;
  double t = 0.0;
  int dimy, dimx;
  
  //Image dimension of the zero padded image
  //Dimensions should be power of 2
  dimx = 128;
  dimy = 128;

  //Define parameters
  filetype_vis = PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS;
  filetype_img = PURIFY_IMAGE_FILETYPE_FITS;
  
  printf("**********************\n");
  printf("Visibility module test\n");
  printf("**********************\n\n");
  //Read coverage
  printf("Reading u-v coverage\n\n");
  purify_visibility_readfile(&vis_test,
             "./data/images/Coverages/cont_sim2.vis",
             filetype_vis); 
  printf("Number of visibilities: %i \n\n", vis_test.nmeas);  
  printf("Visibility module test past\n\n"); 

   

  // Input image.
  img.fov_x = 1.0 / 180.0 * PURIFY_PI;
  img.fov_y = 1.0 / 180.0 * PURIFY_PI;
  img.nx = 4;
  img.ny = 4;

  printf("**********************\n");
  printf("Image module test\n");
  printf("**********************\n\n");

  //Read input image
  printf("Reading input image\n\n");
  purify_image_readfile(&img, "data/images/cluster.fits", filetype_img);
  printf("Image dimension: %i, %i \n\n", img.nx, img.ny); 
  printf("Image module test past\n\n"); 

  param_m1.nmeas = vis_test.nmeas;
  param_m1.ny1 = dimy;
  param_m1.nx1 = dimx;
  param_m1.ofy = 2;
  param_m1.ofx = 2;
  param_m1.ky = 24;
  param_m1.kx = 24;

  param_m2.nmeas = vis_test.nmeas;
  param_m2.ny1 = dimy;
  param_m2.nx1 = dimx;
  param_m2.ofy = 2;
  param_m2.ofx = 2;
  param_m2.ky = 24;
  param_m2.kx = 24;

  Nb = 9;
  Nx=param_m2.ny1*param_m2.nx1;
  Nr=Nb*Nx;
  Ny=param_m2.nmeas;

  //Memory allocation for the different variables
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
  noise = (complex double*)malloc((vis_test.nmeas) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(noise);
  w = (double*)malloc((Nr) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(w);
  shifts = (complex double*)malloc((vis_test.nmeas) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(shifts);
  error = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(error);
  xoutc = (complex double*)malloc((Nx) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(xoutc);
  shifts = (complex double*)malloc((vis_test.nmeas) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(shifts);

 
  dummyr = malloc(Nr * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(dummyr);
  dummyc = malloc(Nr * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(dummyc);

  for (i=0; i < Nx; i++){
    xinc[i] = 0.0 + 0.0*I;
  }

 

  for (i=0; i < img.ny; i++){
    for (j=0; j < img.nx; j++){
      xinc[i+j*param_m1.ny1] = img.pix[i+j*img.ny] + 0.0*I;
    }
  }
  
  printf("***********************\n");
  printf("Measurement module test\n");
  printf("***********************\n\n");
  //Initialize griding matrix
  printf("Initializing griding matrix\n\n");
  start = clock();
  assert(start != -1);
  purify_measurement_init_cft(
      &gmat, deconv, shifts, vis_test.u, vis_test.v, &param_m1);
  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;
  printf("Time griding matrix initalization: %f \n\n", t);
  
  //Memory allocation for the fft
  i = Nx*param_m1.ofy*param_m1.ofx;
  fft_temp1 = (complex double*)malloc((i) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(fft_temp1);
  fft_temp2 = (complex double*)malloc((i) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(fft_temp2);

  //FFT plan  
  planfwd = fftw_plan_dft_2d(param_m1.nx1*param_m1.ofx, param_m1.ny1*param_m1.ofy, 
              fft_temp1, fft_temp1, 
              FFTW_FORWARD, FFTW_MEASURE);

  planadj = fftw_plan_dft_2d(param_m1.nx1*param_m1.ofx, param_m1.ny1*param_m1.ofy, 
              fft_temp2, fft_temp2, 
              FFTW_BACKWARD, FFTW_MEASURE);


  datafwd[0] = (void*)&param_m1;
  datafwd[1] = (void*)deconv;
  datafwd[2] = (void*)&gmat;
  datafwd[3] = (void*)&planfwd;
  datafwd[4] = (void*)fft_temp1;
  datafwd[5] = (void*)shifts;

  dataadj[0] = (void*)&param_m2;
  dataadj[1] = (void*)deconv;
  dataadj[2] = (void*)&gmat;
  dataadj[3] = (void*)&planadj;
  dataadj[4] = (void*)fft_temp2;
  dataadj[5] = (void*)shifts;


  printf("FFT plan done \n\n");
  
  printf("Simulating visibilities \n\n");
  start = clock();
  assert(start != -1);
  purify_measurement_cftfwd((void*)y0, (void*)xinc, datafwd);
  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;
  printf("Time measurement operator: %f \n\n", t);

  
  //Noise realization
  //Input snr
  snr = 30.0;
  a = cblas_dznrm2(Ny, (void*)y0, 1);
  sigma = a*pow(10.0,-(snr/20.0))/sqrt(Ny);

    
  for (i=0; i < Ny; i++) {
      noise[i] = (sopt_ran_gasdev2(seedn) + sopt_ran_gasdev2(seedn)*I)*(sigma/sqrt(2));
      y[i] = y0[i] + noise[i];
  }

  //Rescaling the measurements

  aux2 = (double)Ny/(double)Nx;

  for (i=0; i < Ny; i++) {
    y[i] = y[i]/sqrt(aux2);
  }

  for (i=0; i < Nx; i++) {
    deconv[i] = deconv[i]/sqrt(aux2);
  }
  
  
  // Output image.
  img_copy.fov_x = 1.0 / 180.0 * PURIFY_PI;
  img_copy.fov_y = 1.0 / 180.0 * PURIFY_PI;
  img_copy.nx = param_m1.nx1;
  img_copy.ny = param_m1.ny1;

  for (i=0; i < Nx; i++){
    xoutc[i] = 0.0 + 0.0*I;
  }
  
  //Dirty image
  printf("Computing dirty image from visibilities \n\n");
  purify_measurement_cftadj((void*)xoutc, (void*)y, dataadj);
  img_copy.pix = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(img_copy.pix);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xoutc[i]);
  }
  printf("Writing dirty image in fits file\n\n");
  purify_image_writefile(&img_copy, "data/test/test_dirty.fits", filetype_img);

  printf("Measurement module test past\n\n"); 
  
  printf("***********************\n");
  printf("SOPT linking test\n");
  printf("***********************\n\n");
  
  //SARA structure initialization
  printf("SARA structure initialization\n\n");
  

    param1.ndict = Nb;
    param1.real = 0;

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

   printf("SARA structure initialization done\n\n");

  //Scaling constants in the representation domain

  sopt_sara_analysisop((void*)dummyc, (void*)xoutc, datas);

  for (i=0; i < Nr; i++) {
    dummyr[i] = creal(dummyc[i]);
  }

  aux1 = purify_utils_maxarray(dummyr, Nr);

  
  // Output image.
  img_copy.fov_x = 1.0 / 180.0 * PURIFY_PI;
  img_copy.fov_y = 1.0 / 180.0 * PURIFY_PI;
  img_copy.nx = param_m1.nx1;
  img_copy.ny = param_m1.ny1;



  //Initial solution and weights
  for (i=0; i < Nx; i++) {
      xoutc[i] = 0.0 + 0.0*I;
  }
  for (i=0; i < Nr; i++){
    w[i] = 1.0;
  }
  //Copy true image in xout
  for (i=0; i < Nx; i++) {
    xout[i] = creal(xinc[i]);
  }

  
  printf("BPSA reconstruction\n\n");
  
  
  //Structure for the L1 solver      
  param2.verbose = 2;
  param2.max_iter = 100;
  param2.gamma = gamma*aux1;
  param2.rel_obj = 0.001;
  param2.epsilon = sqrt(Ny + 2*sqrt(Ny))*sigma/sqrt(aux2);
  param2.epsilon_tol = 0.05;
  param2.real_data = 0;
  param2.cg_max_iter = 3;
  param2.cg_tol = 0.000001;

  
  //Initial solution
  for (i=0; i < Nx; i++) {
      xoutc[i] = 0.0 + 0.0*I;
  }
 
  start = clock();
  assert(start != -1);
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
                   (void*)y, Ny, w, param2);

  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;

  printf("Time BPSA reconstruction: %f \n\n", t); 

  
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
  
  printf("Writing reconstructed image in fits file\n\n");
  purify_image_writefile(&img_copy, "data/test/test_rec.fits", filetype_img);

  
  printf("SOPT linking test past\n\n"); 

  
  
  //Free all memory
  purify_image_free(&img);
  purify_image_free(&img_copy);
  free(deconv);
  purify_visibility_free(&vis_test);
  free(y);
  free(xinc);
  free(xout);
  free(w);
  free(noise);
  free(y0);
  free(error);
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


  return 0;

}
