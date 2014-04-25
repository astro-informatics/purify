/*! 
 * \file purify_util.c
 * Utility functions.
 */
#include "purify_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "purify_error.h"
#include "purify_types.h"


/*!
 * fftshift of 1D array. Swaps the second and first parts of the array.
 * 
 * \param[out] out shifted output array.
 * \param[in] in input array.
 * \param[in] n number of elements in the array.
 *
 * \authors Rafael Carrillo
 */
void purify_utils_fftshift_1d(double *out, double *in, int n) {
  
  int NF;
  int NC;
  int i;

  NF=(int)floor(n/2.0);
  NC=(int)ceil(n/2.0);
  
  for (i=0; i<NF; i++)
    out[i]=in[i+NC];
  
  for (i=NF; i<n; i++)
    out[i]=in[i-NF];

}


/*!
 * inverse fftshift of 1D array .
 * 
 * \param[out] out shifted output array.
 * \param[in] in input array.
 * \param[in] n number of elements in the array.
 *
 * \authors Rafael Carrillo
 */
void purify_utils_ifftshift_1d(double *out, double *in, int n){
  
  int NF;
  int NC;
  int i;

  NF=(int)floor(n/2.0);
  NC=(int)ceil(n/2.0);
     
  for (i=0; i<NC; i++)
    out[i]=in[i+NF];
  
  for (i=NC; i<n; i++)
    out[i]=in[i-NC];

}


/*!
 * Find the maximum of an array.
 * 
 * \param[in] a array of real numbers.
 * \param[in] num_elements number of elements in the array.
 * \retval max output value.
 *
 * \authors Rafael Carrillo
 */
double purify_utils_maxarray(double *a, int num_elements)
{
  int i;
  double max=a[0];

  for (i=1; i<num_elements; i++)
    if (a[i]>max)
      max=a[i];

  return(max);
}

/*!
 * Find the index of the closest memmber of an array to the input.
 * 
 * \param[in] x array of real numbers.
 * \param[in] nx number of elements in the array.
 * \param[in] key input real number to search for.
 * \retval ind output index.
 *
 * \authors Rafael Carrillo
 */
int purify_utils_absearch(double *x, int nx, double key){

  int min, max;
  int ind, i;

  min = 1;
  max = nx-1;
  
  if (key >= x[max]){
    ind = nx-1;
  }
  else if ((key >= x[0])&&(key < x[1])){
    ind = 0;
  }
  else {
    for (i = 0; i < nx; i++){
      ind = (int)floor((max + min)/2.0);
      if (x[ind] <= key){
        if (x[ind + 1] > key)
          break;
        else
          min = ind;
      }
      else {
        if (x[ind - 1] <= key){
          ind = ind - 1;
          break;
        } 
        else
           max = ind;
      }
    }
  }
  return(ind);
}


