
#ifndef PURIFY_MEASUREMENT
#define PURIFY_MEASUREMENT
#include "purify_config.h"

/*!  
 * Structure storing parametrs for the interpolation operator.
 *
 */
typedef struct {
  /*! Number of measurements. */ 
  int nmeas; 
  /*! Number of rows in the discrete image. */
  int ny1; 
  /*! Number of columns in the discrete image. */
  int nx1; 
  /*! Oversampling factor in the row dimension. */
  int ofy; 
  /*! Oversampling factor in the column dimension. */
  int ofx; 
  /*! Number of rows in the interpolation kernel. */
  int ky; 
  /*! Number of columns in the interpolation kernel. */
  int kx; 
  
} purify_measurement_cparam;

void purify_measurement_fft_real(void *out, 
				 void *in, 
				 void **data);

void purify_measurement_fft_complex(void *out, 
				    void *in, 
				    void **data);

void purify_measurement_mask_opfwd(void *out, 
				   void *in, 
				   void **data);

void purify_measurement_mask_opadj(void *out, 
				   void *in, 
				   void **data);

void purify_measurement_opfwd(void *out, 
			      void *in, 
			      void **data);

void purify_measurement_init_cft(purify_sparsemat_row *mat, 
                                 double *deconv, complex double *shifts,
                                 double *u, double *v, 
                                 purify_measurement_cparam *param);

void purify_measurement_cftfwd(void *out, void *in, void **data);

void purify_measurement_cftadj(void *out, void *in, void **data);

double purify_measurement_pow_meth(void (*A)(void *out, void *in, void **data), 
                                   void **A_data,
                                   void (*At)(void *out, void *in, void **data), 
                                   void **At_data);

void purify_measurement_symcftfwd(void *out, void *in, void **data);

void purify_measurement_symcftadj(void *out, void *in, void **data);

#endif
