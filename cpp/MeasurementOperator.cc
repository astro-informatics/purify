#include "MeasurementOperator.h"
#include "FFTOperator.h"

namespace purify {
  Vector<t_complex> MeasurementOperator::degrid(const Image<t_complex>& eigen_image)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      eigen_image:: input image to be degridded
      st:: gridding parameters
    */
      Matrix<t_complex> padded_image = Matrix<t_complex>::Zero(ftsizeu, ftsizev);
      Matrix<t_complex> ft_vector(ftsizeu, ftsizev);
      t_int x_start = floor(ftsizeu * 0.5 - imsizex * 0.5);
      t_int y_start = floor(ftsizev * 0.5 - imsizey * 0.5);

      // zero padding and gridding correction
      padded_image.block(y_start, x_start, imsizey, imsizex)
          = S * eigen_image;
      

      // create fftgrid
      ft_vector = fftop.forward(padded_image); // the fftshift is not needed because of the phase shift in the gridding kernel
      // turn into vector
      ft_vector.resize(ftsizeu*ftsizev, 1); // using conservativeResize does not work, it grables the image. Also, it is not what we want.
      // get visibilities
      return (G * ft_vector).array() * W/norm;
      
  }

  Image<t_complex> MeasurementOperator::grid(const Vector<t_complex>& visibilities)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      visibilities:: input visibilities to be gridded
      st:: gridding parameters
    */
      Matrix<t_complex> ft_vector = G.adjoint() * (visibilities.array() * W).matrix();
      ft_vector.resize(ftsizeu, ftsizev); // using conservativeResize does not work, it grables the image. Also, it is not what we want.
      Image<t_complex> padded_image = fftop.inverse(ft_vector); // the fftshift is not needed because of the phase shift in the gridding kernel
      t_int x_start = floor(ftsizeu * 0.5 - imsizex * 0.5);
      t_int y_start = floor(ftsizev * 0.5 - imsizey * 0.5);
      return S * padded_image.block(y_start, x_start, imsizey, imsizex)/norm;
  }



  Vector<t_real> MeasurementOperator::omega_to_k(const Vector<t_real>& omega) {
    /*
      Maps fourier coordinates (u or v) to integer grid coordinates.

      omega:: fourier coordinates for a signle axis (u or v axis)
    */
    return omega.unaryExpr(std::ptr_fun<double,double>(std::floor));     
  }


  Sparse<t_complex> MeasurementOperator::init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, 
          const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv) 
  {
    /* 
      Given u and v coordinates, creates a gridding interpolation matrix that maps between visibilities and the fourier transform grid.

      u:: fourier coordinates of visibilities for u axis
      v:: fourier coordinates of visibilities for v axis
      Ju:: support of kernel for u axis
      Jv:: support of kernel for v axis
      kernelu:: lambda function for kernel on u axis
      kernelv:: lambda function for kernel on v axis
      ftsizeu:: size of grid along u axis
      ftsizev:: size of grid along v axis
    */

  // Need to write exception for u.size() != v.size()
  t_real rows = u.size();
  t_real cols = ftsizeu * ftsizev;
  t_int q;
  t_int p;
  t_int index;
  Vector<t_real> ones = u * 0; ones.setOnes();
  Vector<t_real> k_u = MeasurementOperator::omega_to_k(u - ones * Ju * 0.5);
  Vector<t_real> k_v = MeasurementOperator::omega_to_k(v - ones * Jv * 0.5);
  std::vector<t_tripletList> entries;
  entries.reserve(rows * Ju * Jv);
  for (t_int m = 0; m < rows; ++m)
    {
      for (t_int ju = 1; ju <= Ju; ++ju)
       {
         q = utilities::mod(k_u(m) + ju, ftsizeu);
        for (t_int jv = 1; jv <= Jv; ++jv)
          {
            p = utilities::mod(k_v(m) + jv, ftsizev);
            index = utilities::sub2ind(p, q, ftsizev, ftsizeu);
            const t_complex I(0, 1);
            entries.emplace_back(m, index, std::exp(-2 * purify_pi * I *((k_u(m) + ju) * 0.5+ (k_v(m) + jv) * 0.5 )) * kernelu(u(m)-(k_u(m)+ju)) * kernelv(v(m)-(k_v(m)+jv)));
          }
      }
    }

  //    
  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.setFromTriplets(entries.begin(), entries.end());

  return interpolation_matrix; 
  }

  Image<t_real> MeasurementOperator::init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv)
  {
    /*
      Given the fourier transform of a gridding kernel, creates the scaling image for gridding correction.

    */
    t_int x_start = floor(ftsizeu * 0.5 - imsizex * 0.5);
    t_int y_start = floor(ftsizev * 0.5 - imsizey * 0.5);
    Array<t_real> range;
    range.setLinSpaced(std::max(ftsizeu, ftsizev), 0.5, std::max(ftsizeu, ftsizev) - 0.5);
    return (1e0 / range.segment(x_start, imsizex).unaryExpr(ftkernelu)).matrix() * (1e0 / range.segment(y_start, imsizey).unaryExpr(ftkernelv)).matrix().transpose();
  }

  Image<t_real> MeasurementOperator::init_correction2d_fft(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int Ju, const t_int Jv)
  {
    /*
      Given the gridding kernel, creates the scaling image for gridding correction using an fft.

    */
    Matrix<t_complex> K= Matrix<t_complex>::Zero(ftsizeu, ftsizev);
    for (int i = 0; i < Ju; ++i)
    {
      t_int n = utilities::mod(i - Ju/2, ftsizeu);
      for (int j = 0; j < Jv; ++j)
      {
        t_int m = utilities::mod(j - Jv/2, ftsizev);
        const t_complex I(0, 1);
        K(n, m) = kernelu(i - Ju/2) * kernelv(j - Jv/2) * std::exp(-2 * purify_pi * I * ((i - Ju/2)  * 0.5 + (j - Jv/2) * 0.5 ));
      }
    }
    t_int x_start = floor(ftsizeu * 0.5 - imsizex * 0.5);
    t_int y_start = floor(ftsizev * 0.5 - imsizey * 0.5);
    Image<t_real> S = fftop.inverse(K).array().real().block(y_start, x_start, imsizey, imsizex); // probably really slow!
    return 1/S;

  }  

  Array<t_complex> MeasurementOperator::init_weights(const Vector<t_real>& u, const Vector<t_real>& v, const Vector<t_complex>& weights, const t_real & oversample_factor, const std::string& weighting_type, const t_real& R)
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
      Matrix<t_complex> gridded_weights = Matrix<t_complex>::Zero(ftsizev, ftsizeu);
      for (t_int i = 0; i < weights.size(); ++i)
      {
        t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
        t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
        gridded_weights(p, q) += weights(i);
      }
      t_complex mean_gridded_weights = (gridded_weights.array() * gridded_weights.array()).sum();
      t_complex robust_scale = mean_weights/mean_gridded_weights * 12.5 * std::pow(10, -2 * R); // Need to check formula
      
      for (t_int i = 0; i < weights.size(); ++i)
      {
        t_int q = utilities::mod(floor(u(i) * scale), ftsizeu);
        t_int p = utilities::mod(floor(v(i) * scale), ftsizev);
        if (weighting_type == "uniform")
          out_weights(i) = weights(i) / gridded_weights(p, q);
        if (weighting_type == "robust"){
          out_weights(i) = weights(i) /(1. + robust_scale * gridded_weights(p, q));
        }
      }
      out_weights = out_weights/out_weights.sum();
    }
    return out_weights.array();
  }

  t_real MeasurementOperator::power_method(const t_int niters){
    /*
     attempt at coding the power method, returns the largest eigen value of a linear operator
    */
     t_real estimate_eigen_value = 1;
     Image<t_complex> estimate_eigen_vector = Image<t_complex>::Random(imsizey, imsizex);
     std::cout << "Starting power method " << '\n';
     std::cout << "Iteration: " << 0 << ", norm = " << estimate_eigen_value << '\n';
     for (t_int i = 0; i < niters; ++i)
     {
      auto new_estimate_eigen_vector = MeasurementOperator::grid(MeasurementOperator::degrid(estimate_eigen_vector));
      estimate_eigen_value = new_estimate_eigen_vector.matrix().norm();
      estimate_eigen_vector = new_estimate_eigen_vector/estimate_eigen_value;
      std::cout << "Iteration: " << i + 1 << ", norm = " << estimate_eigen_value << '\n';
     }
     return estimate_eigen_value;
  }

  Image<t_complex> MeasurementOperator::sparsify_chirp(const Image<t_complex>& row, const t_real& energy){
    /*
      Takes in fourier transform of chirp, and returns sparsified version
      w_prog:: input fourier transform of chirp
      energy:: how much energy to keep after sparsifying 
    */
      //there is probably a way to get eigen to do this without a loop
      t_real tau = 0.5;
      t_int niters = 10;
      Image<t_real> abs_row = row.coeff.cwiseAbs();
      t_real min_energy = 0;
      t_real max_energy = 1;
      //calculating threshold
      for (t_int i = 0; i < niters; ++i)
      {
        t_real energy_sum = 0;
        for (t_int i = 0; i < abs_row.size(); ++i)
        {

          if (abs_row(i) * abs_row(i) > tau)
          {
            energy_sum = energy_sum + abs_row(i) * abs_row(i);
          }
        }

        if (energy_sum > energy)
        {
          tau = (max_energy - tau) * 0.5 + tau;
        }else{
          tau = (tau - min_energy) * 0.5 + min_energy;
        }
      }
  }

  Image<t_complex> MeasurementOperator::generate_chirp(const t_real w_term, const t_real cellx, const t_real celly, const t_int x_size, const t_int y_size){
    /*
      return chirp image fourier transform for w component

      w:: w-term in units of lambda
      celly:: size of y pixel in arcseconds
      cellx:: size of x pixel in arcseconds
      x_size:: number of pixels along x-axis
      y_size:: number of pixels along y-axis

    */

    const t_real max_fov = 180. * 60 * 60; //maximum field of view for a hemisphere

    const t_real delt_x = cellx / max_fov;
    const t_real delt_y = celly / max_fov;

    Image<t_complex> chirp(y_size, x_size);
    t_complex I(0, 1);
    for (t_int l = 0; l < x_size; ++l)
    {
      for (t_int m = 0; m < y_size; ++m)
      {
        t_real x = (l + 0.5 - x_size * 0.5) * delt_x;
        t_real y = (m + 0.5 - y_size * 0.5) * delt_y;
        chirp(m, l) = std::exp(- 2 * purify_pi * I * w_term * (std::sqrt(1 - x*x - y*y) - 1)) * std::exp(- 2 * purify_pi * I * (l * 0.5 + m * 0.5));
      }
    }
    return chirp;
  }

  Sparse<t_complex> MeasurementOperator::init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const Vector<t_real>& w, const t_int Ju, 
          const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv) {
      /* 
        Given u and v coordinates, creates a gridding interpolation matrix that maps between visibilities and the fourier transform grid.

        u:: fourier coordinates of visibilities for u axis
        v:: fourier coordinates of visibilities for v axis
        Ju:: support of kernel for u axis
        Jv:: support of kernel for v axis
        kernelu:: lambda function for kernel on u axis
        kernelv:: lambda function for kernel on v axis
        ftsizeu:: size of grid along u axis
        ftsizev:: size of grid along v axis
      */

    // Need to write exception for u.size() != v.size()
    t_real rows = u.size();
    t_real cols = ftsizeu * ftsizev;
    t_int q;
    t_int p;
    t_int index;
    Vector<t_real> ones = u * 0; ones.setOnes();
    Vector<t_real> k_u = MeasurementOperator::omega_to_k(u - ones * Ju * 0.5);
    Vector<t_real> k_v = MeasurementOperator::omega_to_k(v - ones * Jv * 0.5);
    std::vector<t_tripletList> entries;
    entries.reserve(rows * Ju * Jv);
    for (t_int m = 0; m < rows; ++m)
      {
        Image<t_complex> kernel_image(Jv, Ju);
        for (t_int ju = 1; ju <= Ju; ++ju)
         {
           q = utilities::mod(k_u(m) + ju, ftsizeu);
          for (t_int jv = 1; jv <= Jv; ++jv)
            {
              p = utilities::mod(k_v(m) + jv, ftsizev);
              index = utilities::sub2ind(p, q, ftsizev, ftsizeu);
              const t_complex I(0, 1);
              //Calculating image of convolution kernel
              kernel_image(jv, ju) = std::exp(-2 * purify_pi * I *((k_u(m) + ju) * 0.5+ (k_v(m) + jv) * 0.5 )) * kernelu(u(m)-(k_u(m)+ju)) * kernelv(v(m)-(k_v(m)+jv));
            }
        }
        //Calcluating chirp term
        Image<t_complex> chirp = MeasurementOperator::generate_chirp(w(m), cellx, celly, x_size, y_size);
        Image<t_complex> w_proj = fftop.inverse(chirp);
        //Convolution of kernel and chirp
        Matrix<t_complex> interp_row = utilities::convolution_operator(kernel_image, w_proj);
        //Caclulating row
        interp_row.resize(interp_row.size(), 1);
      }

    //    
    Sparse<t_complex> interpolation_matrix(rows, cols);
    interpolation_matrix.setFromTriplets(entries.begin(), entries.end());

    return interpolation_matrix; 
  }

  MeasurementOperator::MeasurementOperator(const utilities::vis_params& uv_vis, const t_int &Ju, const t_int &Jv,
      const std::string &kernel_name, const t_int &imsizex, const t_int &imsizey, const t_real &oversample_factor, const std::string& weighting_type, const t_real& R, bool use_w_term, bool fft_grid_correction)
      : imsizex(imsizex), imsizey(imsizey), ftsizeu(floor(oversample_factor * imsizex)), ftsizev(floor(oversample_factor * imsizey))
    
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
    std::cout << "------" << '\n';
    std::cout << "Constructing Gridding Operator" << '\n';


    std::function<t_real(t_real)> kernelu;
    std::function<t_real(t_real)> kernelv;
    std::function<t_real(t_real)> ftkernelu;
    std::function<t_real(t_real)> ftkernelv;

    std::cout << "Kernel Name: " << kernel_name << '\n';
    std::cout << "Number of visibilities: " << u.size() <<'\n';
    std::cout << "Ju: " << Ju << '\n';
    std::cout << "Jv: " << Jv << '\n';

    S = Image<t_real>::Zero(imsizey, imsizex);

    const t_int norm_iterations = 20; // number of iterations for power method

    //samples for kb_interp
    if (kernel_name == "kb_interp")
    {

      t_real kb_interp_alpha = purify_pi * std::sqrt(Ju * Ju/(oversample_factor * oversample_factor) * (oversample_factor - 0.5) * (oversample_factor - 0.5) - 0.8);
      t_int total_samples = 2e6 * Ju;
      auto kb_general = [&] (t_real x) { return MeasurementOperator::kaiser_bessel_general(x, Ju, kb_interp_alpha); };
      Vector<t_real> samples = MeasurementOperator::kernel_samples(total_samples, kb_general, Ju);
      auto kb_interp = [&] (t_real x) { return MeasurementOperator::kernel_linear_interp(samples, x, Ju); };
      kernelu = kb_interp;
      kernelv = kb_interp;
      //Since kb_interp needs the pre-samples, all calculations need to be done within scope of this if statement. Otherwise massif gets a segfault.
      //S = MeasurementOperator::MeasurementOperator::init_correction2d_fft(kernelu, kernelv, Ju, Jv);
      auto ftkb = [&] (t_real x) { return MeasurementOperator::ft_kaiser_bessel_general(x/ftsizeu - 0.5, Ju, kb_interp_alpha); };
      ftkernelu = ftkb;
      ftkernelv = ftkb;
      S = MeasurementOperator::MeasurementOperator::init_correction2d(ftkernelu, ftkernelv); // Does gridding correction using analytic formula

      G = MeasurementOperator::init_interpolation_matrix2d(u, v, Ju, Jv, kernelu, kernelv);
      std::cout << "Calculating weights" << '\n';
      W = MeasurementOperator::init_weights(u, v, weights, oversample_factor, weighting_type, R);
      norm = std::sqrt(MeasurementOperator::power_method(norm_iterations));
      std::cout << "Gridding Operator Constructed" << '\n';
      std::cout << "------" << '\n';
      return;
    }

    if ((kernel_name == "pswf") and (Ju != 6 or Jv != 6))
    {
      std::cout << "Error: Only a support of 6 is implimented for PSWFs.";
    }
    if (kernel_name == "kb")
    {
      auto kbu = [&] (t_real x) { return MeasurementOperator::kaiser_bessel(x, Ju); };
      auto kbv = [&] (t_real x) { return MeasurementOperator::kaiser_bessel(x, Jv); };
      auto ftkbu = [&] (t_real x) { return MeasurementOperator::ft_kaiser_bessel(x/ftsizeu - 0.5, Ju); };
      auto ftkbv = [&] (t_real x) { return MeasurementOperator::ft_kaiser_bessel(x/ftsizev - 0.5, Jv); };
      kernelu = kbu;
      kernelv = kbv;
      ftkernelu = ftkbu;
      ftkernelv = ftkbv;
    }
    if (kernel_name == "pswf")
    {
      auto pswfu = [&] (t_real x) { return MeasurementOperator::pswf(x, Ju); };
      auto pswfv = [&] (t_real x) { return MeasurementOperator::pswf(x, Jv); };
      auto ftpswfu = [&] (t_real x) { return MeasurementOperator::ft_pswf(x/ftsizeu - 0.5, Ju); };
      auto ftpswfv = [&] (t_real x) { return MeasurementOperator::ft_pswf(x/ftsizev - 0.5, Jv); };
      kernelu = pswfu;
      kernelv = pswfv;
      ftkernelu = ftpswfu;
      ftkernelv = ftpswfv;
    }
    if (kernel_name == "gauss")
    {
      auto gaussu = [&] (t_real x) { return MeasurementOperator::gaussian(x, Ju); };
      auto gaussv = [&] (t_real x) { return MeasurementOperator::gaussian(x, Jv); };
      auto ftgaussu = [&] (t_real x) { return MeasurementOperator::ft_gaussian(x/ftsizeu - 0.5, Ju); };
      auto ftgaussv = [&] (t_real x) { return MeasurementOperator::ft_gaussian(x/ftsizev - 0.5, Jv); };      
      kernelu = gaussu;
      kernelv = gaussv;
      ftkernelu = ftgaussu;
      ftkernelv = ftgaussv;
    }
    
    if ( fft_grid_correction == true )
    {
      S = MeasurementOperator::MeasurementOperator::init_correction2d_fft(kernelu, kernelv, Ju, Jv); // Does gridding correction with FFT
    }
    if ( fft_grid_correction == false )
    {
      S = MeasurementOperator::MeasurementOperator::init_correction2d(ftkernelu, ftkernelv); // Does gridding correction using analytic formula
    }
    
    G = MeasurementOperator::init_interpolation_matrix2d(u, v, Ju, Jv, kernelu, kernelv);
    std::cout << "Calculating weights" << '\n';
    W = MeasurementOperator::init_weights(u, v, weights, oversample_factor, weighting_type, R);
    norm = std::sqrt(MeasurementOperator::power_method(norm_iterations));
    std::cout << "Found a norm of " << norm << '\n';
    std::cout << "Gridding Operator Constructed" << '\n';
    std::cout << "------" << '\n';
  }

  t_real MeasurementOperator::kaiser_bessel(const t_real& x, const t_int& J)
  {
      /*
        kaiser bessel gridding kernel
      */
      t_real a = 2 * x / J;
      t_real alpha = 2.34 * J;
      return boost::math::cyl_bessel_i(0, std::real(alpha * std::sqrt(1 - a * a))) / boost::math::cyl_bessel_i(0, alpha);
  }

  t_real MeasurementOperator::kaiser_bessel_general(const t_real& x, const t_int& J, const t_real& alpha)
  {
      /*
        kaiser bessel gridding kernel
      */
      t_real a = 2 * x / J;
      return boost::math::cyl_bessel_i(0, std::real(alpha * std::sqrt(1 - a * a))) / boost::math::cyl_bessel_i(0, alpha);
  }

  t_real MeasurementOperator::ft_kaiser_bessel_general(const t_real& x, const t_int& J, const t_real& alpha)
  {
      /*
        Fourier transform of kaiser bessel gridding kernel
      */
      
      t_complex eta = std::sqrt(static_cast<t_complex>((purify_pi * x * J)*(purify_pi * x * J) - alpha * alpha));
      t_real normalisation = 38828.11016883; //Factor that keeps it consistent with fessler formula

      return std::real(std::sin(eta) / eta) / normalisation; //simple way of doing the calculation, the boost bessel funtions do not support complex valued arguments
  }

  t_real MeasurementOperator::ft_kaiser_bessel(const t_real& x, const t_int& J)
  {
      /*
        Fourier transform of kaiser bessel gridding kernel
      */

      t_real alpha = 2.34 * J; // value said to be optimal in Fessler et. al. 2003
      t_complex eta = std::sqrt(static_cast<t_complex>((purify_pi * x * J)*(purify_pi * x * J) - alpha * alpha));
      t_real normalisation = 38828.11016883; //Factor that keeps it consistent with fessler formula

      return std::real(std::sin(eta) / eta) / normalisation; //simple way of doing the calculation, the boost bessel funtions do not support complex valued arguments
  }

  t_real MeasurementOperator::gaussian(const t_real& x, const t_int& J)
  {
    /*
      Guassian gridding kernel

      x:: value to evaluate
      J:: support size
    */
      t_real sigma = 0.31 * std::pow(J, 0.52); // Optimal sigma according to fessler et al.
      t_real a = x / sigma;
      return std::exp(-a * a * 0.5);
  }

  t_real MeasurementOperator::ft_gaussian(const t_real& x, const t_int& J)
  {
    /*
      Fourier transform of guassian gridding kernel

      x:: value to evaluate
      J:: support size of gridding kernel
    */
      t_real sigma = 0.31 * std::pow(J, 0.52);
      t_real a = x * sigma * purify_pi;
      return std::sqrt(purify_pi / 2) / sigma * std::exp(-a * a * 2);
  }


  t_real MeasurementOperator::calc_for_pswf(const t_real& eta0, const t_int& J, const t_real& alpha)
  {
    //polynomial coefficients for prolate spheriodal wave function rational approximation
    const std::array<t_real, 6> p1 = {8.203343e-2, -3.644705e-1, 6.278660e-1, -5.335581e-1, 2.312756e-1, 2*0.0};
    const std::array<t_real, 6> p2 = {4.028559e-3, -3.697768e-2, 1.021332e-1, -1.201436e-1, 6.412774e-2, 2*0.0};
    const std::array<t_real, 3> q1 = {1., 8.212018e-1, 2.078043e-1};
    const std::array<t_real, 3> q2 = {1., 9.599102e-1, 2.918724e-1};


      if (J != 6 or alpha != 1)
      {
        return 0;
      }
      //Calculating numerator and denominator using Horner's rule.
      // PSWF = numerator / denominator

      auto fraction = [](t_real eta, std::array<t_real, 6> const&p, std::array<t_real, 3> const &q) {
        auto const p_size = sizeof(p) / sizeof(p[0]) - 1;
        auto const q_size = sizeof(q) / sizeof(q[0]) - 1;

        auto numerator = p[p_size];
        for (t_int i = 1; i <= p_size; ++i)
        {
          numerator = eta * numerator + p[p_size - i];
        }

        auto denominator = q[q_size];
        for (t_int i = 1; i <= q_size; ++i)
        {
          denominator = eta * denominator + q[q_size - i];
        }
        return numerator / denominator;
      };
      if (0 <= std::abs(eta0) and std::abs(eta0) <= 0.75)
        return fraction(eta0 * eta0 - 0.75 * 0.75, p1, q1);
      if (0.75 < std::abs(eta0) and std::abs(eta0) <= 1)
        return fraction(eta0 * eta0 - 1 * 1, p2, q2);
  
      return 0;
  }

  t_real MeasurementOperator::pswf(const t_real& x, const t_int& J)
  {
    /*
      Calculates the standard PSWF for radio astronomy, with a support of J = 6 and alpha = 1.

      x:: value to evaluate
      J:: support size of gridding kernel

      The tailored prolate spheroidal wave functions for gridding radio astronomy.
      Details are explained in Optimal Gridding of Visibility Data in Radio
      Astronomy, F. R. Schwab 1983.

    */
      const t_real eta0 = 2 * x / J;
      const t_real alpha = 1;
      return MeasurementOperator::calc_for_pswf(eta0, J, alpha) * std::pow(1 - eta0 * eta0, alpha);
  }

  t_real MeasurementOperator::ft_pswf(const t_real& x, const t_int& J)
  {
    /*
      Calculates the fourier transform of the standard PSWF for radio astronomy, with a support of J = 6 and alpha = 1.

      x:: value to evaluate
      J:: support size of gridding kernel

      The tailored prolate spheroidal wave functions for gridding radio astronomy.
      Details are explained in Optimal Gridding of Visibility Data in Radio
      Astronomy, F. R. Schwab 1983.

    */
      // Does not produce the correct scaling factor!!! Use the fft gridding correction method!

      const t_real alpha = 1;
      const t_real eta0 = 2 * x;

      return MeasurementOperator::calc_for_pswf(eta0, J, alpha);

  }

  Vector<t_real> MeasurementOperator::kernel_samples(const t_int& total_samples, const std::function<t_real(t_real)> kernelu, const t_int& J)
  {
    /*
      Pre-calculates samples of a kernel, that can be used with linear interpolation (see Rapid gridding reconstruction with a minimal oversampling ratio, Beatty et. al. 2005)
    */
      Vector<t_real> samples(total_samples);
      for (t_real i = 0; i < total_samples; ++i)
      {
        samples(i) = kernelu(i/total_samples * J - J/2);
      }
      return samples;
  }

  t_real MeasurementOperator::kernel_linear_interp(const Vector<t_real>& samples, const t_real& x, const t_int& J)
  {
    /*
      Calculates kernel using linear interpolation between pre-calculated samples. (see Rapid gridding reconstruction with a minimal oversampling ratio, Beatty et. al. 2005)
    */
    t_int total_samples = samples.size();

    t_real i_effective = (x + J/2) * total_samples / J;

    t_real i_0 = floor(i_effective);
    t_real i_1 = ceil(i_effective);
    //case where i_effective is a sample point
    if ( std::abs(i_0 - i_1) == 0 )
    {
      return samples(i_0);
    }
    //linearly interpolate from nearest neighbour
    t_real y_0; 
    t_real y_1;
    if (i_0 < 0 or i_0 >= total_samples)
    {
      y_0 = 0;
    }else{
      y_0 = samples(i_0);
    }
    if (i_1 < 0 or i_1 >= total_samples)
    {
      y_1 = 0;
    }else{
      y_1 = samples(i_1);
    }
    t_real output = y_0 + (y_1 - y_0)/(i_1 - i_0) * (i_effective - i_0);
    return output;
  }
}
