#include "RMOperator.h"

namespace purify {
  Vector<t_complex> RMOperator::degrid(const Vector<t_complex>& eigen_image)
  {
    /*
      An operator that degrids a 1d image and returns a vector of visibilities.

      eigen_image:: input image to be degridded
      st:: gridding parameters
    */
      Vector<t_complex> padded_image = Vector<t_complex>::Zero(ftsize);
      Vector<t_complex> ft_vector(ftsize);
      const t_int x_start = floor(ftsize * 0.5 - imsize * 0.5);

      // zero padding and gridding correction
      padded_image.segment(x_start, imsize) = S * eigen_image.array();
      
       fft.fwd(ft_vector, padded_image); 
      return (G * ft_vector).array() * W/norm;
      
  }

  Vector<t_complex> RMOperator::grid(const Vector<t_complex>& visibilities)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      visibilities:: input visibilities to be gridded
      st:: gridding parameters
    */
      Vector<t_complex> ft_vector = G.adjoint() * (visibilities.array() * W).matrix();
      Vector<t_complex> padded_image;
      fft.inv(padded_image, ft_vector); // the fftshift is not needed because of the phase shift in the gridding kernel
      t_int x_start = floor(ftsize * 0.5 - imsize * 0.5);
      return S * padded_image.segment(x_start, imsize).array()/norm;
  }



  Vector<t_real> RMOperator::omega_to_k(const Vector<t_real>& omega) {
    /*
      Maps fourier coordinates (u or v) to integer grid coordinates.

      omega:: fourier coordinates for a signle axis (u or v axis)
    */
    return omega.unaryExpr(std::ptr_fun<double,double>(std::floor));     
  }


  Sparse<t_complex> RMOperator::init_interpolation_matrix1d(const Vector<t_real>& u, const t_int J, 
          const std::function<t_real(t_real)> kernelu) 
  {
    /* 
      Given u and v coordinates, creates a gridding interpolation matrix that maps between visibilities and the fourier transform grid.

      u:: fourier coordinates of visibilities for u axis
      J:: support of kernel for u axis
      kernelu:: lambda function for kernel on u axis
    */

  
  t_real rows = u.size();
  t_real cols = ftsize;
  t_int q;
  Vector<t_real> ones = u * 0; ones.setOnes();
  Vector<t_real> k_u = RMOperator::omega_to_k(u - ones * J * 0.5);
  std::vector<t_tripletList> entries;
  entries.reserve(rows * J);
  for (t_int m = 0; m < rows; ++m)
    {
      for (t_int j = 1; j <= J; ++j)
       {
         q = utilities::mod(k_u(m) + j, ftsize);
         const t_complex I(0, 1);
         entries.emplace_back(m, q, std::exp(-2 * purify_pi * I * ((k_u(m) + j) * 0.5)) * kernelu(u(m)-(k_u(m)+j)));
      }
    }

  //    
  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.setFromTriplets(entries.begin(), entries.end());

  return interpolation_matrix; 
  }

  Array<t_real> RMOperator::init_correction1d(const std::function<t_real(t_real)> ftkernelu)
  {
    /*
      Given the fourier transform of a gridding kernel, creates the scaling image for gridding correction.

    */
    t_int x_start = floor(ftsize * 0.5 - imsize * 0.5);
    Array<t_real> range;
    range.setLinSpaced(ftsize, 0.5, ftsize - 0.5);
    return 1e0 / range.segment(x_start, imsize).unaryExpr(ftkernelu);
  }

  Array<t_real> RMOperator::init_correction1d_fft(const std::function<t_real(t_real)> kernelu, const t_int Ju)
  {
    /*
      Given the gridding kernel, creates the scaling image for gridding correction using an fft.

    */
    Vector<t_complex> K = Vector<t_complex>::Zero(ftsize);
    for (int i = 0; i < Ju; ++i)
    {
      t_int n = utilities::mod(i - Ju/2, ftsize);
      const t_complex I(0, 1);
      K(n) = kernelu(i - Ju/2) * std::exp(-2 * purify_pi * I * (i - Ju/2.)  * 0.5);
    }
    t_int x_start = floor(ftsize * 0.5 - imsize * 0.5);
    Array<t_real> S = fft.inv(K).real().segment(x_start, imsize); // probably really slow!
    return 1/S;

  }  

  Array<t_complex> RMOperator::init_weights(const Vector<t_real>& u, const Vector<t_complex>& weights, const t_real & oversample_factor, const std::string& weighting_type, const t_real& R)
  {
    Vector<t_complex> out_weights(weights.size());
    t_complex mean_weights = weights.sum();
    if (weighting_type == "none")
    {
      out_weights = weights.array() * 0 + 1;
    } else if (weighting_type == "natural")
    {
      out_weights = weights / mean_weights;
    } else {
      auto step_function = [&] (t_real x) { return 1; };
      t_real scale = 1./oversample_factor; //scale for fov
      Vector<t_complex> gridded_weights = Vector<t_complex>::Zero(ftsize);
      for (t_int i = 0; i < weights.size(); ++i)
      {
        t_int q = utilities::mod(floor(u(i) * scale), ftsize);
        gridded_weights(q) += weights(i);
      }
      t_complex mean_gridded_weights = (gridded_weights.array() * gridded_weights.array()).sum();
      t_complex robust_scale = mean_weights/mean_gridded_weights * 12.5 * std::pow(10, -2 * R); // Need to check formula
      
      for (t_int i = 0; i < weights.size(); ++i)
      {
        t_int q = utilities::mod(floor(u(i) * scale), ftsize);
        if (weighting_type == "uniform")
          out_weights(i) = weights(i) / gridded_weights(q);
        if (weighting_type == "robust"){
          out_weights(i) = weights(i) /(1. + robust_scale * gridded_weights(q));
        }
      }
      out_weights = out_weights/out_weights.sum();
    }
    return out_weights.array();
  }

  t_real RMOperator::power_method(const t_int niters){
    /*
     attempt at coding the power method, returns the largest eigen value of a linear operator
    */
     t_real estimate_eigen_value = 1;
     Vector<t_complex> estimate_eigen_vector = Vector<t_complex>::Random(imsize);
     //std::cout << "Starting power method " << '\n';
     //std::cout << "Iteration: " << 0 << ", norm = " << estimate_eigen_value << '\n';
     for (t_int i = 0; i < niters; ++i)
     {
      auto new_estimate_eigen_vector = RMOperator::grid(RMOperator::degrid(estimate_eigen_vector));
      estimate_eigen_value = new_estimate_eigen_vector.matrix().norm();
      estimate_eigen_vector = new_estimate_eigen_vector/estimate_eigen_value;
      //std::cout << "Iteration: " << i + 1 << ", norm = " << estimate_eigen_value << '\n';
     }
     return estimate_eigen_value;
  }



  Vector<t_complex> RMOperator::covariance_calculation(const Vector<t_complex> & vector){
    /*
      Calculates a new representation of the covariance matrix, using propogation of uncertainty A Sigma A^T

      pixel:: determines the column to calculate for the covariance matrix.
    */
      Array<t_real> covariance = 1./(W.real());
      return RMOperator::grid(covariance * RMOperator::degrid(vector).array());

  }

  RMOperator::RMOperator(const utilities::rm_params& rm_vis_input, const t_int & Ju, 
        const std::string & kernel_name, const t_int & imsize, const t_real & oversample_factor, 
        const t_real & cell_size, const std::string& weighting_type, const t_real& R, bool fft_grid_correction)
      : imsize(imsize), ftsize(floor(oversample_factor * imsize)), oversample_factor(oversample_factor)
  {
    /*
      Generates tools/operators needed for gridding and degridding.

      u:: visibilities in units of ftsizeu
      v:: visibilities in units of ftsizev
      Ju:: support size for u axis
      Jv:: support size for v axis
      kernel_name:: flag that determines what kernel to use (gauss, pswf, kb)
      imsizex:: size of image along xaxis
      imsizey:: size of image along yaxis
      oversample_factor:: factor for oversampling the FFT grid

    */
    
    utilities::rm_params rm_vis = rm_vis_input;
    if (rm_vis.units == "MHz"){
      rm_vis.frequency = (purify_c / rm_vis.frequency.array()) * (purify_c / rm_vis.frequency.array()) * 1e-12;
  	  rm_vis.units = "wavelength_squared";
	}
    if (rm_vis.units == "wavelength_squared"){
      rm_vis.frequency = rm_vis.frequency * cell_size; 
      rm_vis.units = "radians";
    }
    if (rm_vis.units == "radians")
    {
      rm_vis.frequency = rm_vis.frequency / purify_pi * ftsize;
      rm_vis.units = "pixels";
    }
    

    //t_real new_upsample = utilities::upsample_ratio(uv_vis, ,);
    std::printf("------ \n");
    std::printf("Constructing RM Operator \n");

    std::printf("Oversampling Factor: %f \n", oversample_factor);
    std::function<t_real(t_real)> kernelu;
    
    std::function<t_real(t_real)> ftkernelu;
    
    std::printf("Kernel Name: %s \n", kernel_name.c_str());
    std::printf("Number of visibilities: %ld \n", rm_vis.frequency.size());
    std::printf("Ju: %d \n", Ju);

    S = Array<t_real>::Zero(imsize);

    const t_int norm_iterations = 20; // number of iterations for power method

    //samples for kb_interp
    if (kernel_name == "kb_interp")
    {

      t_real kb_interp_alpha = purify_pi * std::sqrt(Ju * Ju/(oversample_factor * oversample_factor) * (oversample_factor - 0.5) * (oversample_factor - 0.5) - 0.8);
      const t_int total_samples = 2e6 * Ju;
      auto kb_general = [&] (t_real x) { return kernels::kaiser_bessel_general(x, Ju, kb_interp_alpha); };
      Vector<t_real> samples = kernels::kernel_samples(total_samples, kb_general, Ju);
      auto kb_interp = [&] (t_real x) { return kernels::kernel_linear_interp(samples, x, Ju); };
      kernelu = kb_interp;
      //Since kb_interp needs the pre-samples, all calculations need to be done within scope of this if statement. Otherwise massif gets a segfault.
      //S = MeasurementOperator::MeasurementOperator::init_correction2d_fft(kernelu, kernelv, Ju, Jv);
      auto ftkb = [&] (t_real x) { return kernels::ft_kaiser_bessel_general(x/ftsize - 0.5, Ju, kb_interp_alpha); };
      ftkernelu = ftkb;
      
      S = RMOperator::init_correction1d(ftkernelu); // Does gridding correction using analytic formula
      G = RMOperator::init_interpolation_matrix1d(rm_vis.frequency, Ju, kernelu);
      
      std::printf("Calculating weights \n");
      W = RMOperator::init_weights(rm_vis.frequency, rm_vis.weights, oversample_factor, weighting_type, R);
      std::printf("Doing power method \n");
      norm = std::sqrt(RMOperator::power_method(norm_iterations));
      std::printf("Gridding Operator Constructed \n");
      std::printf("------ \n");
      return;
    }

    if ((kernel_name == "pswf") and (Ju != 6))
    {
      std::cout << "Error: Only a support of 6 is implimented for PSWFs.";
    }
    if (kernel_name == "kb")
    {
      auto kbu = [&] (t_real x) { return kernels::kaiser_bessel(x, Ju); };
      auto ftkbu = [&] (t_real x) { return kernels::ft_kaiser_bessel(x/ftsize - 0.5, Ju); };
      kernelu = kbu;
      ftkernelu = ftkbu;
    }
    if (kernel_name == "pswf")
    {
      auto pswfu = [&] (t_real x) { return kernels::pswf(x, Ju); };
      auto ftpswfu = [&] (t_real x) { return kernels::ft_pswf(x/ftsize - 0.5, Ju); };
      kernelu = pswfu;
      ftkernelu = ftpswfu;
    }
    if (kernel_name == "gauss")
    {
      auto gaussu = [&] (t_real x) { return kernels::gaussian(x, Ju); };
      auto ftgaussu = [&] (t_real x) { return kernels::ft_gaussian(x/ftsize - 0.5, Ju); };     
      kernelu = gaussu;
      ftkernelu = ftgaussu;
    }
    
    if ( fft_grid_correction == true )
    {
      S = RMOperator::init_correction1d_fft(kernelu, Ju); // Does gridding correction with FFT
    }
    if ( fft_grid_correction == false )
    {
      S = RMOperator::init_correction1d(ftkernelu); // Does gridding correction using analytic formula
    }

    G = RMOperator::init_interpolation_matrix1d(rm_vis.frequency, Ju, kernelu);

    std::printf("Calculating weights \n");
    W = RMOperator::init_weights(rm_vis.frequency, rm_vis.weights, oversample_factor, weighting_type, R);
    std::printf("Doing power method \n");
    norm = std::sqrt(RMOperator::power_method(norm_iterations));
    std::printf("Found a norm of %f \n", norm);
    std::printf("Gridding Operator Constructed \n");
    std::printf("------ \n");
  }


}