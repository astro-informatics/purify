/*! 
 * \file example_m31.c
 * Image recontruction example from continuos visibilities.
 * Test image: M31 256x256.
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
  double *w_l1, *w_l2;
  double *error;
  complex double *xoutc;
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
  //sopt_l1_rwparam param5;
  //sopt_prox_tvparam param6;
  //sopt_tv_sdmmparam param7;
  //sopt_tv_rwparam param8;

  sopt_prox_l1param param_l1param;
  sopt_l1_param_padmm param_padmm;

  clock_t start, stop;
  double t = 0.0;
#ifdef _OPENMP 
  double start1, stop1;
#endif
  int dimy, dimx;
  
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
  w_l1 = (double*)malloc((Nr) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(w_l1);
  error = (double*)malloc((Nx) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(error);
  xoutc = (complex double*)malloc((Nx) * sizeof(complex double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(xoutc);

  w_l2 = (double*)malloc((Ny) * sizeof(double));
  PURIFY_ERROR_MEM_ALLOC_CHECK(w_l2);
  

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

    
  FILE *dirty_vis = fopen("data/dirty_vis.txt", "w");
  for (i=0; i < Ny; i++) {
    y[i] = y0[i] + noise[i];
    fprintf(dirty_vis, "%24.14e %24.14e\n", *((double *)y + 2*i), *((double*)y + 2*i+ 1));
  }
  fclose(dirty_vis);

  //Rescaling the measurements

  //aux4 = (double)Ny/(double)Nx;
  //Estimate operator norm usin the power method.
  aux4 = purify_measurement_pow_meth(&purify_measurement_cftfwd,
                                     datafwd,
				     &purify_measurement_cftadj,
				     dataadj);

  printf("Operator norm: %f \n\n", aux4);

  /*for (i=0; i < Ny; i++) {
    y[i] = y[i]/sqrt(aux4);
    }

    for (i=0; i < Nx; i++) {
    deconv[i] = deconv[i]/sqrt(aux4);
    }*/
  
  
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
    xout[i] = creal(xoutc[i]);
  }
  
  /* purify_image_writefile(&img_copy, "data/test/m31dirty_padmm.fits", filetype_img); */
  

  
  //SARA structure initialization

  param1.ndict = Nb;
  param1.real = 1;

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
  param2.real = 1;

  dict_types1 = malloc(param2.ndict * sizeof(sopt_wavelet_type));
  PURIFY_ERROR_MEM_ALLOC_CHECK(dict_types1);

  dict_types1[0] = SOPT_WAVELET_DB8;
    
  sopt_sara_initop(&param2, param_m1.ny1, param_m1.nx1, 4, dict_types1);

  datas1[0] = (void*)&param2;

  //Dirac structure initialization

  param3.ndict = 1;
  param3.real = 1;

  dict_types2 = malloc(param3.ndict * sizeof(sopt_wavelet_type));
  PURIFY_ERROR_MEM_ALLOC_CHECK(dict_types2);

  dict_types2[0] = SOPT_WAVELET_Dirac;
    
  sopt_sara_initop(&param3, param_m1.ny1, param_m1.nx1, 4, dict_types2);

  datas2[0] = (void*)&param3;

  //Scaling constants in the diferent representation domains

  sopt_sara_analysisop((void*)dummyr, (void*)xout, datas);

  aux2 = purify_utils_maxarray(dummyr, Nr);

  sopt_sara_analysisop((void*)dummyr, (void*)xout, datas1);

  aux3 = purify_utils_maxarray(dummyr, Nx);

  


  // Output image.
  img_copy.fov_x = 1.0 / 180.0 * PURIFY_PI;
  img_copy.fov_y = 1.0 / 180.0 * PURIFY_PI;
  img_copy.nx = param_m1.nx1;
  img_copy.ny = param_m1.ny1;



  //Initial solution and weights
  for (i=0; i < Nx; i++) {
    xout[i] = 0.0;
  }
  for (i=0; i < Nr; i++){
    w_l1[i] = 1.0;
  }
  for (i=0; i < Ny; i++){
    w_l2[i] = 1.0;
  }
  
//complex double *ycopy = malloc(Ny * sizeof(*ycopy));
//cblas_zcopy(Ny, (void*)y, 1, (void*)ycopy, 1);

  printf("**********************\n");
  printf("BPSA reconstruction\n");
  printf("**********************\n");

  
    
  //Structure for the L1 solver  
      
  //param4.verbose = 2;
  //param4.max_iter = 300;
  //param4.gamma = gamma*aux2;
  //param4.rel_obj = 0.001;
  //param4.epsilon = sqrt(Ny + 2*sqrt(Ny))*sigma/sqrt(aux4);
  //param4.epsilon_tol = 0.01;
  //param4.real_data = 0;
  //param4.cg_max_iter = 100;
  //param4.cg_tol = 0.000001;
  

  //Structure for the L1 prox
  param_l1param.verbose = 0;
  param_l1param.max_iter = 20;
  param_l1param.rel_obj = 0.01;
  param_l1param.nu = 1.0;
  param_l1param.tight = 0;
  param_l1param.pos = 1;
    
  //Structure for the L1 solver    
  param_padmm.verbose = 2;
  param_padmm.max_iter = 5;
  param_padmm.gamma = 0.01;
  param_padmm.rel_obj = 0.0001;
  param_padmm.epsilon = sqrt(Ny + 2*sqrt(Ny))*sigma;
  param_padmm.real_out = 1;
  param_padmm.real_meas = 0;
  param_padmm.paraml1 = param_l1param;
    
  param_padmm.epsilon_tol_scale = 1.001;
  param_padmm.lagrange_update_scale = 0.9;
  param_padmm.nu = 1.0*aux4; 

  //param_padmm.nu = 0.88;
  

   
#ifdef _OPENMP 
  start1 = omp_get_wtime();
#else
  start = clock();
  assert(start != -1);
#endif
  

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

  
  //SNR
  for (i=0; i < Nx; i++) {
    error[i] = creal(xinc[i])-xout[i];
//JDM: isn't this -1 times the eror computed for the sdmm m31 example?
  }
  mse = cblas_dnrm2(Nx, error, 1);
  a = cblas_dznrm2(Nx, (void*)xinc, 1);
  printf("Norm image: %f \n\n", a);
  snr_out = 20.0*log10(a/mse);
  printf("SNR: %f dB\n\n", snr_out);


    
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = xout[i];
  }
  
  purify_image_writefile(&img_copy, "data/expected/m31bpsa_padmm.fits", filetype_img);

  //Residual image

  for (i=0; i < Nx; i++){
    xoutc[i] = xout[i] + 0.0*I;
  }

  purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd);
  alpha = -1.0 +0.0*I;
  cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1);
  purify_measurement_cftadj((void*)xoutc, (void*)y0, dataadj);

  for (i=0; i < Nx; i++){
    img_copy.pix[i] = creal(xoutc[i]);
  }
  
  purify_image_writefile(&img_copy, "data/test/m31bpsares_padmm.fits", filetype_img);
  
  //Error image
  for (i=0; i < Nx; i++){
    img_copy.pix[i] = error[i];
  }
  

  purify_image_writefile(&img_copy, "data/test/m31bpsaerror_padmm.fits", filetype_img); 



  /* printf("**********************\n"); */
  /* printf("Db8 reconstruction\n"); */
  /* printf("**********************\n"); */
  /*  */
  /* //Initial solution */
  /* for (i=0; i < Nx; i++) { */
  /*   xout[i] = 0.0; */
  /* } */
  /*  */
  /* //param4.gamma = gamma*aux3; */
  /*  */
  /* start = clock(); */
  /* assert(start != -1); */
  /*  */
  /* sopt_l1_solver_padmm((void*)xout, Nx, */
	/* 	       &purify_measurement_cftfwd, */
	/* 	       datafwd, */
	/* 	       &purify_measurement_cftadj, */
	/* 	       dataadj, */
	/* 	       &sopt_sara_synthesisop, */
	/* 	       datas1, */
	/* 	       &sopt_sara_analysisop, */
	/* 	       datas1, */
	/* 	       Nx, */
	/* 	       (void*)y, Ny, w_l1, w_l2, param_padmm); */
  /*  */
  /* stop = clock(); */
  /* t = (double) (stop-start)/CLOCKS_PER_SEC; */
  /*  */
  /* printf("Time BPDb8: %f \n\n", t);  */
  /*  */
  /*  */
  /* //SNR */
  /* for (i=0; i < Nx; i++) { */
  /*   error[i] = creal(xinc[i])-xout[i]; */
  /*   //JDM: isn't this -1 times the eror computed for the sdmm m31 example? */
  /* } */
  /* mse = cblas_dnrm2(Nx, error, 1); */
  /* a = cblas_dznrm2(Nx, (void*)xinc, 1); */
  /* printf("Norm image: %f \n\n", a); */
  /* snr_out = 20.0*log10(a/mse); */
  /* printf("SNR: %f dB\n\n", snr_out); */
  /*  */
  /* for (i=0; i < Nx; i++){ */
  /*   img_copy.pix[i] = xout[i]; */
  /* }    */
  /* purify_image_writefile(&img_copy, "data/test/m31db8_padmm.fits", filetype_img); */
  /*  */
  /* //Residual image */
  /*  */
  /*  */
  /* for (i=0; i < Nx; i++){ */
  /*   xoutc[i] = xout[i] + 0.0*I; */
  /* } */
  /*  */
  /* purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd); */
  /* alpha = -1.0 +0.0*I; */
  /* cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1); */
  /* purify_measurement_cftadj((void*)xoutc, (void*)y0, dataadj); */
  /*  */
  /* for (i=0; i < Nx; i++){ */
  /*   img_copy.pix[i] = creal(xoutc[i]); */
  /* } */
  /*  */
  /* purify_image_writefile(&img_copy, "data/test/m31db8res_padmm.fits", filetype_img); */
  /*  */
  /* //Error image */
  /* for (i=0; i < Nx; i++){ */
  /*   img_copy.pix[i] = error[i]; */
  /* } */
  /*  */
  /* purify_image_writefile(&img_copy, "data/test/m31db8error_padmm.fits", filetype_img);  */
  /*  */
  /*  */
  /*  */
  /*  */
  /* printf("**********************\n"); */
  /* printf("BP reconstruction\n"); */
  /* printf("**********************\n"); */
  /*  */
  /* param4.gamma = gamma*aux1; */
  /*  */
  /* //Initial solution */
  /* for (i=0; i < Nx; i++) { */
  /*   xout[i] = 0.0; */
  /* } */
  /*  */
  /* start = clock(); */
  /* assert(start != -1); */
  /*    */
  /* sopt_l1_solver_padmm((void*)xout, Nx, */
	/* 	       &purify_measurement_cftfwd, */
	/* 	       datafwd, */
	/* 	       &purify_measurement_cftadj, */
	/* 	       dataadj, */
	/* 	       &sopt_sara_synthesisop, */
	/* 	       datas2, */
	/* 	       &sopt_sara_analysisop, */
	/* 	       datas2, */
	/* 	       Nx, */
	/* 	       (void*)y, Ny, w_l1, w_l2, param_padmm); */
  /*  */
  /*    */
  /* stop = clock(); */
  /* t = (double) (stop-start)/CLOCKS_PER_SEC; */
  /*  */
  /* printf("Time BP: %f \n\n", t);  */
  /*  */
  /*  */
  /* //SNR */
  /* for (i=0; i < Nx; i++) { */
  /*   error[i] = creal(xinc[i])-xout[i]; */
  /*   //JDM: isn't this -1 times the eror computed for the sdmm m31 example? */
  /* } */
  /* mse = cblas_dnrm2(Nx, error, 1); */
  /* a = cblas_dznrm2(Nx, (void*)xinc, 1); */
  /* printf("Norm image: %f \n\n", a); */
  /* snr_out = 20.0*log10(a/mse); */
  /* printf("SNR: %f dB\n\n", snr_out); */
  /*  */
  /* for (i=0; i < Nx; i++){ */
  /*   img_copy.pix[i] = xout[i]; */
  /* }    */
  /*   */
  /* purify_image_writefile(&img_copy, "data/test/m31bp_padmm.fits", filetype_img); */
  /*  */
  /* //Residual image */
  /*  */
  /*    */
  /* for (i=0; i < Nx; i++){ */
  /*   xoutc[i] = xout[i] + 0.0*I; */
  /* } */
  /*  */
  /* purify_measurement_cftfwd((void*)y0, (void*)xoutc, datafwd); */
  /* alpha = -1.0 +0.0*I; */
  /* cblas_zaxpy(Ny, (void*)&alpha, y, 1, y0, 1); */
  /* purify_measurement_cftadj((void*)xoutc, (void*)y0, dataadj); */
  /*  */
  /* for (i=0; i < Nx; i++){ */
  /*   img_copy.pix[i] = creal(xoutc[i]); */
  /* } */
  /*  */
  /* purify_image_writefile(&img_copy, "data/test/m31bpres_padmm.fits", filetype_img); */
  /*  */
  /* //Error image */
  /* for (i=0; i < Nx; i++){ */
  /*   img_copy.pix[i] = error[i]; */
  /* } */
  /*  */
  /* purify_image_writefile(&img_copy, "data/test/m31bperror_padmm.fits", filetype_img);  */

  
  
  
  
  //Free all memory
  purify_image_free(&img);
  purify_image_free(&img_copy);
  free(deconv);
  purify_visibility_free(&vis_test);
  free(y);
  //free(ycopy);
  free(xinc);
  free(xout);
  free(w_l1);
  free(noise);
  free(y0);
  free(error);
  free(xoutc);
  free(w_l2);
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
