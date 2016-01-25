#include "MeasurementOperator.h"
#include "FFTOperator.h"

namespace purify {
  namespace {
    //polynomial coefficients for prolate spheriodal wave function rational approximation
    const std::array<t_real, 6> p1 = {8.203343e-2, -3.644705e-1, 6.278660e-1, -5.335581e-1, 2.312756e-1, 2*0.0};
    const std::array<t_real, 6> p2 = {4.028559e-3, -3.697768e-2, 1.021332e-1, -1.201436e-1, 6.412774e-2, 2*0.0};
    const std::array<t_real, 3> q1 = {1., 8.212018e-1, 2.078043e-1};
    const std::array<t_real, 3> q2 = {1., 9.599102e-1, 2.918724e-1};
  }


  Vector<t_complex> MeasurementOperator::degrid(const Image<t_complex>& eigen_image)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      eigen_image:: input image to be degridded
      st:: gridding parameters
    */
      Matrix<t_complex> padded_image(operator_params.ftsizeu, operator_params.ftsizev);
      Matrix<t_complex> ft_vector(operator_params.ftsizeu, operator_params.ftsizev);
      t_int x_start = floor(operator_params.ftsizeu * 0.5 - operator_params.imsizex * 0.5);
      t_int y_start = floor(operator_params.ftsizev * 0.5 - operator_params.imsizey * 0.5);
      t_int x_end = floor(operator_params.ftsizeu * 0.5 + operator_params.imsizex * 0.5);
      t_int y_end = floor(operator_params.ftsizev * 0.5 + operator_params.imsizey * 0.5);

      // zero padding and gridding correction
      for (t_int i = 0; i < operator_params.ftsizeu; ++i)
      {
        for (t_int j = 0; j < operator_params.ftsizev; ++j)
        {
          if ( (i >= x_start) and (i < x_end) and (j >= y_start) and (j < y_end))
          {
            padded_image(j, i) = operator_params.S(j, i) * eigen_image(j - y_start, i - x_start);
          }else{
            padded_image(j, i) = 0;
          }
        }
      }
      // create fftgrid
      //ft_vector = fftop.forward(fftop.shift(padded_image)); 
      ft_vector = fftop.forward(padded_image); // the fftshift is not needed because of the phase shift in the gridding kernel
      // turn into vector
      ft_vector.resize(operator_params.ftsizeu*operator_params.ftsizev, 1); // using conservativeResize does not work, it grables the image. Also, not sure it is what we want.
      // get visibilities
      Vector<t_complex> visibilities = operator_params.G * ft_vector;
      return visibilities;
      
  }

  Image<t_complex> MeasurementOperator::grid(const Vector<t_complex>& visibilities)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      visibilities:: input visibilities to be gridded
      st:: gridding parameters
    */
      Matrix<t_complex> ft_vector = operator_params.G.adjoint() * visibilities;
      ft_vector.resize(operator_params.ftsizeu, operator_params.ftsizev); // using conservativeResize does not work, it grables the image. Also, not sure it is what we want.
      
      //Matrix<t_complex> padded_image = fftop.ishift(fftop.inverse(ft_vector));
      Matrix<t_complex> padded_image = fftop.inverse(ft_vector); // the fftshift is not needed because of the phase shift in the gridding kernel
      Image<t_complex> eigen_image(operator_params.imsizex, operator_params.imsizey);
      t_int x_start = floor(operator_params.ftsizeu * 0.5 - operator_params.imsizex * 0.5);
      t_int y_start = floor(operator_params.ftsizev * 0.5 - operator_params.imsizey * 0.5);
      t_int x_end = floor(operator_params.ftsizeu * 0.5 + operator_params.imsizex * 0.5);
      t_int y_end = floor(operator_params.ftsizev * 0.5 + operator_params.imsizey * 0.5);
      for (t_int i = 0; i < operator_params.imsizex; ++i)
      {
        for (t_int j = 0; j < operator_params.imsizey; ++j)
        {
            eigen_image(j, i) = operator_params.S(j + y_start, i + x_start) * padded_image(j + y_start, i + x_start);
        }
      }
      return eigen_image;
      
  }



  Vector<t_real> MeasurementOperator::omega_to_k(const Vector<t_real>& omega) {
    /*
      Maps fourier coordinates (u or v) to integer grid coordinates.

      omega:: fourier coordinates for a signle axis (u or v axis)
    */
    return omega.unaryExpr(std::ptr_fun<double,double>(std::floor));     
  }



  Sparse<t_complex> MeasurementOperator::init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev) 
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
  t_real x0 = 0.5;
  t_real y0 = 0.5;
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
            entries.push_back(t_tripletList(m, index, std::exp(-2 * purify_pi * I *((k_u(m) + ju) * 0.5+ (k_v(m) + jv) * 0.5 )) * kernelu(u(m)-(k_u(m)+ju)) * kernelv(v(m)-(k_v(m)+jv))));
          }
        }
      }    
    Sparse<t_complex> interpolation_matrix(rows, cols);
    interpolation_matrix.setFromTriplets(entries.begin(), entries.end());
    return interpolation_matrix; 
  }

  Image<t_real> MeasurementOperator::init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv, const t_int ftsizeu, const t_int ftsizev)
  {
    /*
      Given the fourier transform of a gridding kernel, creates the scaling image for gridding correction.

    */
    Image<t_real> S(ftsizeu, ftsizev);
    for (int i = 0; i < ftsizeu; ++i)
    {
      for (int j = 0; j < ftsizev; ++j)
      {
        S(j ,i) = 1/(ftkernelu(i) * ftkernelv(j));
      }
    }
    return S;

  }

  Image<t_real> MeasurementOperator::init_correction2d_fft(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev, const t_int Ju, const t_int Jv)
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
        K(n, m) = kernelu(i - Ju/2) * kernelv(j - Jv/2);
      }
    }
    Image<t_real> S = fftop.shift(fftop.inverse(K)).array().real();
    return 1/S;

  }  

  MeasurementOperator::params MeasurementOperator::init_nufft2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::string kernel_name, const t_int imsizex, const t_int imsizey, const t_real oversample_factor, bool fft_grid_correction)
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


    MeasurementOperator::params st;
    st.imsizex = imsizex;
    st.imsizey = imsizey;
    st.ftsizeu = floor(oversample_factor * imsizex);
    st.ftsizev = floor(oversample_factor * imsizey);
    std::function<t_real(t_real)>  kernelu;
    std::function<t_real(t_real)> kernelv;
    std::function<t_real(t_real)> ftkernelu;
    std::function<t_real(t_real)> ftkernelv;


    //samples for kb_interp
    if (kernel_name == "kb_interp")
    {

      t_real kb_interp_alpha = purify_pi * std::sqrt(Ju * Ju/(oversample_factor * oversample_factor) * (oversample_factor - 0.5) * (oversample_factor - 0.5) - 0.8);
      t_int total_samples = 2e5 * Ju;
      auto kb_general = [&] (t_real x) { return MeasurementOperator::kaiser_bessel_general(x, Ju, kb_interp_alpha); };
      Vector<t_real> samples = MeasurementOperator::kernel_samples(total_samples, kb_general, Ju);
      auto kb_interp = [&] (t_real x) { return MeasurementOperator::kernel_linear_interp(samples, x, Ju); };
      kernelu = kb_interp;
      kernelv = kb_interp;
      if (fft_grid_correction == false)
      {
        fft_grid_correction = true;
      }
    }

    if ((kernel_name == "pswf") and (Ju != 6 or Jv != 6))
    {
      std::cout << "Error: Only a support of 6 is implimented for PSWFs.";
    }
    if (kernel_name == "kb")
    {
      auto kbu = [&] (t_real x) { return MeasurementOperator::kaiser_bessel(x, Ju); };
      auto kbv = [&] (t_real x) { return MeasurementOperator::kaiser_bessel(x, Jv); };
      auto ftkbu = [&] (t_real x) { return MeasurementOperator::ft_kaiser_bessel(x/st.ftsizeu - 0.5, Ju); };
      auto ftkbv = [&] (t_real x) { return MeasurementOperator::ft_kaiser_bessel(x/st.ftsizev - 0.5, Jv); };
      kernelu = kbu;
      kernelv = kbv;
      ftkernelu = ftkbu;
      ftkernelv = ftkbv;
    }
    if (kernel_name == "pswf")
    {
      auto pswfu = [&] (t_real x) { return MeasurementOperator::pswf(x, Ju); };
      auto pswfv = [&] (t_real x) { return MeasurementOperator::pswf(x, Jv); };
      auto ftpswfu = [&] (t_real x) { return MeasurementOperator::ft_pswf(x/st.ftsizeu - 0.5, Ju); };
      auto ftpswfv = [&] (t_real x) { return MeasurementOperator::ft_pswf(x/st.ftsizev - 0.5, Jv); };
      kernelu = pswfu;
      kernelv = pswfv;
      ftkernelu = ftpswfu;
      ftkernelv = ftpswfv;
    }
    if (kernel_name == "gauss")
    {
      auto gaussu = [&] (t_real x) { return MeasurementOperator::gaussian(x, Ju); };
      auto gaussv = [&] (t_real x) { return MeasurementOperator::gaussian(x, Jv); };
      auto ftgaussu = [&] (t_real x) { return MeasurementOperator::ft_gaussian(x/st.ftsizeu - 0.5, Ju); };
      auto ftgaussv = [&] (t_real x) { return MeasurementOperator::ft_gaussian(x/st.ftsizev - 0.5, Jv); };      
      kernelu = gaussu;
      kernelv = gaussv;
      ftkernelu = ftgaussu;
      ftkernelv = ftgaussv;
    }

    std::cout << "Support of Kernel " << kernel_name << '\n';
    std::cout << "Ju: " << Ju << '\n';
    std::cout << "Jv: " << Jv << '\n';
    st.S = Image<t_real>::Zero(st.ftsizev, st.ftsizeu);
    if ( fft_grid_correction == true )
    {
      st.S = MeasurementOperator::MeasurementOperator::init_correction2d_fft(kernelu, kernelv, st.ftsizeu, st.ftsizev, Ju, Jv); // Does gridding correction with FFT
    }
    if ( fft_grid_correction == false )
    {
      st.S = MeasurementOperator::MeasurementOperator::init_correction2d(ftkernelu, ftkernelv, st.ftsizeu, st.ftsizev); // Does gridding correction using analytic formula
    }
    
    st.G = MeasurementOperator::init_interpolation_matrix2d(u, v, Ju, Jv, kernelu, kernelv, st.ftsizeu, st.ftsizev);
    std::cout << "Gridding Operator Constructed" << '\n';
    std::cout << "------" << '\n';
    return st;
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

  t_real MeasurementOperator::ft_kaiser_bessel(const t_real& x, const t_int& J)
  {
      /*
        Fourier transform of kaiser bessel gridding kernel
      */

      t_real alpha = 2.34 * J; // value said to be optimal in Fessler et. al. 2003
      t_complex eta = std::sqrt(static_cast<t_complex>((purify_pi * x * J)*(purify_pi * x * J) - alpha * alpha));
      return std::real(std::sin(eta) / eta);
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


  t_real MeasurementOperator::calc_for_pswf(const t_real& x, const t_int& J, const t_real& alpha)
  {
    /*
      Calculates Horner's Rule the standard PSWF for radio astronomy, with a support of J = 6 and alpha = 1.
      
      x:: value to evaluate
      J:: support size of gridding kernel
      alpha:: type of special PSWF to calculate

      The tailored prolate spheroidal wave functions for gridding radio astronomy.
      Details are explained in Optimal Gridding of Visibility Data in Radio
      Astronomy, F. R. Schwab 1983.

    */

      if (J != 6 or alpha != 1)
      {
        return 0;
      }
      //Calculating numerator and denominator using Horner's rule.
      // PSWF = numerator / denominator
      t_real eta0 = x;
      t_real numerator = 0;
      t_real denominator = 1;

      t_int p_size = 0;
      t_int q_size = 0;
      if (0 <= std::abs(eta0) and std::abs(eta0) <= 0.75)
      {
        t_real eta = eta0 * eta0 - 0.75 * 0.75;
        p_size = sizeof(p1) / sizeof(p1[0]) - 1;
        q_size = sizeof(q1) / sizeof(q1[0]) - 1;

        numerator = p1[p_size];
        for (t_int i = 1; i <= p_size; ++i)
        {
          numerator = eta * numerator + p1[p_size - i];
        }

        denominator = q1[q_size];
        for (t_int i = 1; i <= q_size; ++i)
        {
          denominator = eta * denominator + q1[q_size - i];
        }

      }
      if (0.75 < std::abs(eta0) and std::abs(eta0) <= 1)
      {
        t_real eta = eta0 * eta0 - 1 * 1;
        p_size = sizeof(p2) / sizeof(p2[0]) - 1;
        q_size = sizeof(q2) / sizeof(q2[0]) - 1;
      
        numerator = p2[p_size];

        for (t_int i = 1; i <= p_size; ++i)
        {
          numerator = eta * numerator + p2[p_size - i];
        }

        denominator = q2[q_size];
        for (t_int i = 1; i <= q_size; ++i)
        {
          denominator = eta * denominator + q2[q_size - i];
        }
      }
      return numerator / denominator;
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
    if (i_0 == i_1)
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
