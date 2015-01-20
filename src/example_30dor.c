/*! 
 * \file example_30dor.c
 * Image recontruction example from continuos visibilities.
 * Test image: 30Dor 251x251, zero-padded to 256x256.
 * Coverage: random variable density with M=0.2N visibilities.
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
  double aux1, aux2, aux3, aux4;
  complex double alpha;
  

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
  double *wdx;
  double *wdy;
  double *dummyr;
  complex double *dummyc;

  
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
  sopt_wavelet_type *dict_types1;
  sopt_wavelet_type *dict_types2;
  sopt_sara_param param1;
  sopt_sara_param param2;
  sopt_sara_param param3;
  void *datas[1];
  void *datas1[1];
  void *datas2[1];

  //Structures for the opmization problems
  sopt_l1_sdmmparam param4;
  sopt_l1_rwparam param5;
  sopt_prox_tvparam param6;
  sopt_tv_sdmmparam param7;
  sopt_tv_rwparam param8;

  clock_t start, stop;
  double t = 0.0;
  #ifdef _OPENMP 
  double start1, stop1;
  #endif
  int dimy, dimx;

  // seed SOPT's random number generator
  sopt_ran_ran2(-seedn);
  
  //Image dimension of the zero padded image
  //Dimensions should be power of 2
  dimx = 256;
  dimy = 256;

  //Define parameters
  filetype_vis = PURIFY_VISIBILITY_FILETYPE_PROFILE_VIS;
  filetype_img = PURIFY_IMAGE_FILETYPE_FITS;

  //Read coverage
  purify_visibility_readfile(&vis_test,
             "./data/images/Coverages/cont_sim4.vis",
             filetype_vis); 
  printf("Number of visibilities: %i \n\n", vis_test.nmeas);  

   

  // Input image.
  img.fov_x = 1.0 / 180.0 * PURIFY_PI;
  img.fov_y = 1.0 / 180.0 * PURIFY_PI;
  img.nx = 4;
  img.ny = 4;

  //Read input image
  purify_image_readfile(&img, "data/images/M31.fits", filetype_img);
  printf("Image dimension: %i, %i \n\n", img.nx, img.ny); 

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
  shifts = (complex double*)malloc((vis_test.nmeas) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(shifts);
  w = (double*)malloc((Nr) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(w);
  error = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(error);
  xoutc = (complex double*)malloc((Nx) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(xoutc);

  wdx = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(wdx);
  wdy = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(wdy);

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
  
  //Initialize griding matrix
  start = clock();
  assert(start != -1);
  purify_measurement_init_cft(&gmat, deconv, shifts,
      vis_test.u, vis_test.v, &param_m1);
  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;
  printf("Time initalization: %f \n\n", t);
  
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
  
  
  start = clock();
  assert(start != -1);
  purify_measurement_cftfwd((void*)y0, (void*)xinc, datafwd);
  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;
  printf("Time forward operator: %f \n\n", t);

  
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

  aux4 = (double)Ny/(double)Nx;

  for (i=0; i < Ny; i++) {
    y[i] = y[i]/sqrt(aux4);
  }

  for (i=0; i < Nx; i++) {
    deconv[i] = deconv[i]/sqrt(aux4);
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
  purify_measurement_cftadj((void*)xoutc, (void*)y, dataadj);

  for (i=0; i < Nx; i++) {
    xout[i] = creal(xoutc[i]);
  }

  aux1 = purify_utils_maxarray(xout, Nx);

  

   img_copy.pix = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(img_copy.pix);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xoutc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31dirty.fits", filetype_img);
  

  
  //SARA structure initialization

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

    //Db8 structure initialization

    param2.ndict = 1;
    param2.real = 0;

    dict_types1 = malloc(param2.ndict * sizeof(sopt_wavelet_type));
    PURIFY_ERROR_MEM_ALLOC_CHECK(dict_types1);

    dict_types1[0] = SOPT_WAVELET_DB8;
    
    sopt_sara_initop(&param2, param_m1.ny1, param_m1.nx1, 4, dict_types1);

  datas1[0] = (void*)&param2;

  //Dirac structure initialization

    param3.ndict = 1;
    param3.real = 0;

    dict_types2 = malloc(param3.ndict * sizeof(sopt_wavelet_type));
    PURIFY_ERROR_MEM_ALLOC_CHECK(dict_types2);

    dict_types2[0] = SOPT_WAVELET_Dirac;
    
    sopt_sara_initop(&param3, param_m1.ny1, param_m1.nx1, 4, dict_types2);

  datas2[0] = (void*)&param3;

  //Scaling constants in the diferent representation domains

  sopt_sara_analysisop((void*)dummyc, (void*)xoutc, datas);

  for (i=0; i < Nr; i++) {
    dummyr[i] = creal(dummyc[i]);
  }

  aux2 = purify_utils_maxarray(dummyr, Nr);

  sopt_sara_analysisop((void*)dummyc, (void*)xoutc, datas1);

  for (i=0; i < Nr; i++) {
    dummyr[i] = creal(dummyc[i]);
  }

  aux3 = purify_utils_maxarray(dummyr, Nx);

  


  // Output image.
  img_copy.fov_x = 1.0 / 180.0 * PURIFY_PI;
  img_copy.fov_y = 1.0 / 180.0 * PURIFY_PI;
  img_copy.nx = param_m1.nx1;
  img_copy.ny = param_m1.ny1;



  //Initial solution and weights
  for (i=0; i < Nx; i++) {
      xoutc[i] = 0.0 + 0.0*I;
      wdx[i] = 1.0;
      wdy[i] = 1.0;
  }
  for (i=0; i < Nr; i++){
    w[i] = 1.0;
  }
  //Copy true image in xout
  for (i=0; i < Nx; i++) {
    xout[i] = creal(xinc[i]);
  }



  printf("**********************\n");
  printf("BPSA reconstruction\n");
  printf("**********************\n");

  
    
  //Structure for the L1 solver      
  param4.verbose = 2;
  param4.max_iter = 300;
  param4.gamma = gamma*aux2;
  param4.rel_obj = 0.001;
  param4.epsilon = sqrt(Ny + 2*sqrt(Ny))*sigma/sqrt(aux4);
  param4.epsilon_tol = 0.01;
  param4.real_data = 0;
  param4.cg_max_iter = 100;
  param4.cg_tol = 0.000001;

  
  //Initial solution
  for (i=0; i < Nx; i++) {
      xoutc[i] = 0.0 + 0.0*I;
  }
 
  #ifdef _OPENMP 
    start1 = omp_get_wtime();
  #else
    start = clock();
    assert(start != -1);
  #endif
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

  #ifdef _OPENMP 
    stop1 = omp_get_wtime();
    t = stop1 - start1;
  #else
    stop = clock();
    t = (double) (stop-start)/CLOCKS_PER_SEC;
  #endif

  printf("Time BPSA: %f \n\n", t); 

  
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
  
  purify_image_writefile(&img_copy, "data/test/m31bpsa.fits", filetype_img);

  //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31bpsares.fits", filetype_img);
  
  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31bpsaerror.fits", filetype_img);
  


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
    start = clock();
    assert(start != -1);
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

  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31saraerror.fits", filetype_img);

  
    printf("**********************\n");
    printf("TV reconstruction\n");
    printf("**********************\n");

    
    //Structure for the TV prox
    param6.verbose = 1;
    param6.max_iter = 50;
    param6.rel_obj = 0.0001;

    
    //Structure for the TV solver    
    param7.verbose = 2;
    param7.max_iter = 300;
    param7.gamma = gamma*aux1;
    param7.rel_obj = 0.001;
    param7.epsilon = sqrt(Ny + 2*sqrt(Ny))*sigma/sqrt(aux4);
    param7.epsilon_tol = 0.01;
    param7.real_data = 0;
    param7.cg_max_iter = 100;
    param7.cg_tol = 0.000001;
    param7.paramtv = param6;

    //Initial solution and weights
    for (i=0; i < Nx; i++) {
        xoutc[i] = 0.0 + 0.0*I;
        wdx[i] = 1.0;
        wdy[i] = 1.0;
    }

    
    start = clock();
    assert(start != -1);
    sopt_tv_sdmm((void*)xoutc, dimx, dimy,
                   &purify_measurement_cftfwd,
                   datafwd,
                   &purify_measurement_cftadj,
                   dataadj,
                   (void*)y, Ny, wdx, wdy, param7);
    stop = clock();
    t = (double) (stop-start)/CLOCKS_PER_SEC;
    
    printf("Time TV: %f \n\n", t); 

    
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
  
  purify_image_writefile(&img_copy, "data/test/m31tv.fits", filetype_img);

   //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31tvres.fits", filetype_img);

  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31tverror.fits", filetype_img);

  printf("**********************\n");
  printf("RWTV reconstruction\n");
  printf("**********************\n");

  

  //Structure for the RWTV solver    
  param8.verbose = 2;
  param8.max_iter = 5;
  param8.rel_var = 0.001;
  param8.sigma = sigma*sqrt(Ny/(2*Nx));
  param8.init_sol = 1;

  
  start = clock();
  assert(start != -1);
    sopt_tv_rwsdmm((void*)xoutc, dimx, dimy,
                   &purify_measurement_cftfwd,
                   datafwd,
                   &purify_measurement_cftadj,
                   dataadj,
                   (void*)y, Ny, param7, param8);
    stop = clock();
    t = (double) (stop-start)/CLOCKS_PER_SEC;

  printf("Time RWTV: %f \n\n", t); 

  

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
  
  purify_image_writefile(&img_copy, "data/test/m31rwtv.fits", filetype_img);

   //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31rwtvres.fits", filetype_img);

  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31rwtverror.fits", filetype_img);

  printf("**********************\n");
  printf("Db8 reconstruction\n");
  printf("**********************\n");

  //Initial solution
  for (i=0; i < Nx; i++) {
        xoutc[i] = 0.0 + 0.0*I;
  }
  
  param4.gamma = gamma*aux3;

  start = clock();
  assert(start != -1);
   sopt_l1_sdmm((void*)xoutc, Nx,
                   &purify_measurement_cftfwd,
                   datafwd,
                   &purify_measurement_cftadj,
                   dataadj,
                   &sopt_sara_synthesisop,
                   datas1,
                   &sopt_sara_analysisop,
                   datas1,
                   Nx,
                   (void*)y, Ny, w, param4);

  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;

  printf("Time BPDb8: %f \n\n", t); 

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
  
  purify_image_writefile(&img_copy, "data/test/m31db8.fits", filetype_img);

   //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31db8res.fits", filetype_img);

  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31db8error.fits", filetype_img);

  printf("**********************\n");
  printf("RWBPDb8 reconstruction\n");
  printf("**********************\n");

  

  //Structure for the RWL1 solver    
  param5.verbose = 2;
  param5.max_iter = 5;
  param5.rel_var = 0.001;
  param5.sigma = sigma*sqrt((double)Ny/(double)Nx);
  param5.init_sol = 1;

  
  start = clock();
  assert(start != -1);
  sopt_l1_rwsdmm((void*)xoutc, Nx,
                   &purify_measurement_cftfwd,
                   datafwd,
                   &purify_measurement_cftadj,
                   dataadj,
                   &sopt_sara_synthesisop,
                   datas1,
                   &sopt_sara_analysisop,
                   datas1,
                   Nx,
                   (void*)y, Ny, param4, param5);

  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;

  printf("Time RWBPDb8: %f \n\n", t); 

  

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
  
  purify_image_writefile(&img_copy, "data/test/m31rwdb8.fits", filetype_img);

   //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31rwdb8res.fits", filetype_img);

  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31rwdb8error.fits", filetype_img);



  printf("**********************\n");
  printf("BP reconstruction\n");
  printf("**********************\n");

  param4.gamma = gamma*aux1;

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
                   datas2,
                   &sopt_sara_analysisop,
                   datas2,
                   Nx,
                   (void*)y, Ny, w, param4); 

  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;

  printf("Time BP: %f \n\n", t); 

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
  
  purify_image_writefile(&img_copy, "data/test/m31bp.fits", filetype_img);

   //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31bpres.fits", filetype_img);

  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31bperror.fits", filetype_img);

  printf("**********************\n");
  printf("RWBP reconstruction\n");
  printf("**********************\n");

  

  //Structure for the RWL1 solver    
  param5.verbose = 2;
  param5.max_iter = 5;
  param5.rel_var = 0.001;
  param5.sigma = sigma*sqrt((double)Ny/(double)Nx);
  param5.init_sol = 1;

  
  start = clock();
  assert(start != -1);
  sopt_l1_rwsdmm((void*)xoutc, Nx,
                   &purify_measurement_cftfwd,
                   datafwd,
                   &purify_measurement_cftadj,
                   dataadj,
                   &sopt_sara_synthesisop,
                   datas2,
                   &sopt_sara_analysisop,
                   datas2,
                   Nx,
                   (void*)y, Ny, param4, param5);

  stop = clock();
  t = (double) (stop-start)/CLOCKS_PER_SEC;

  printf("Time RWBP: %f \n\n", t); 

  

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
  
  purify_image_writefile(&img_copy, "data/test/m31rwbp.fits", filetype_img);

   //Residual image

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xinc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xinc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31rwbpres.fits", filetype_img);

  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  
  purify_image_writefile(&img_copy, "data/test/m31rwbperror.fits", filetype_img);


  
  
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
  free(wdx);
  free(wdy);
  free(shifts);

  sopt_sara_free(&param1);
  sopt_sara_free(&param2);
  sopt_sara_free(&param3);
  free(dict_types);
  free(dict_types1);
  free(dict_types2);

  free(fft_temp1);
  free(fft_temp2);
  fftw_destroy_plan(planfwd);
  fftw_destroy_plan(planadj);
  purify_sparsemat_freer(&gmat);

  free(dummyr);
  free(dummyc);


  return 0;

}
