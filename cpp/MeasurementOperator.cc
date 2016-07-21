#include "MeasurementOperator.h"

namespace purify {
  Vector<t_complex> MeasurementOperator::degrid(const Image<t_complex>& eigen_image)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      eigen_image:: input image to be degridded
      st:: gridding parameters
    */
      Matrix<t_complex> padded_image = Matrix<t_complex>::Zero(floor(imsizey_ * oversample_factor_), floor(imsizex_ * oversample_factor_));
      Matrix<t_complex> ft_vector(ftsizev_, ftsizeu_);
      t_int x_start = floor(floor(imsizex_ * oversample_factor_) * 0.5 - imsizex_ * 0.5);
      t_int y_start = floor(floor(imsizey_ * oversample_factor_) * 0.5 - imsizey_ * 0.5);

      // zero padding and gridding correction
      padded_image.block(y_start, x_start, imsizey_, imsizex_)
          = utilities::parallel_multiply_image(S, eigen_image);

      // create fftgrid
      ft_vector = wprojection::re_sample_ft_grid(fftop.forward(padded_image), resample_factor); // the fftshift is not needed because of the phase shift in the gridding kernel
      // turn into vector
      ft_vector.resize(ftsizeu_*ftsizev_, 1); // using conservativeResize does not work, it garbles the image. Also, it is not what we want.
      // get visibilities
      //return (G * ft_vector).array() * W/norm;
      return utilities::sparse_multiply_matrix(G, ft_vector).array() * W/norm;
  }

  Image<t_complex> MeasurementOperator::grid(const Vector<t_complex>& visibilities)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      visibilities:: input visibilities to be gridded
      st:: gridding parameters
    */
      //Matrix<t_complex> ft_vector = G.adjoint() * (visibilities.array() * W).matrix()/norm;
      Matrix<t_complex> ft_vector = utilities::sparse_multiply_matrix(G.adjoint(), (visibilities.array() * W).matrix())/norm;
      ft_vector.resize(ftsizev_, ftsizeu_); // using conservativeResize does not work, it garbles the image. Also, it is not what we want.
      ft_vector = wprojection::re_sample_ft_grid(ft_vector, 1./resample_factor);
      Image<t_complex> padded_image = fftop.inverse(ft_vector); // the fftshift is not needed because of the phase shift in the gridding kernel
      t_int x_start = floor(floor(imsizex_ * oversample_factor_) * 0.5 - imsizex_ * 0.5);
      t_int y_start = floor(floor(imsizey_ * oversample_factor_) * 0.5 - imsizey_ * 0.5);
      return utilities::parallel_multiply_image(S, padded_image.block(y_start, x_start, imsizey_, imsizex_));
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

  t_int rows = u.size();
  t_int cols = ftsizeu_ * ftsizev_;
  t_int q;
  t_int p;
  t_int index;
  Vector<t_real> ones = u * 0; ones.setOnes();
  Vector<t_real> k_u = MeasurementOperator::omega_to_k(u - ones * Ju * 0.5);
  Vector<t_real> k_v = MeasurementOperator::omega_to_k(v - ones * Jv * 0.5);

  Sparse<t_complex> interpolation_matrix(rows, cols);
  interpolation_matrix.reserve(Vector<t_int>::Constant(rows, Ju * Jv));

  #pragma omp simd collapse(3)
  for (t_int m = 0; m < rows; ++m)
    {
      // I should write this as a tensor product! It would reduce the number of calculations of kernelu and kernelv.
      for (t_int ju = 1; ju <= Ju; ++ju)
       {
        for (t_int jv = 1; jv <= Jv; ++jv)
          {
            q = utilities::mod(k_u(m) + ju, ftsizeu_);
            p = utilities::mod(k_v(m) + jv, ftsizev_);
            index = utilities::sub2ind(p, q, ftsizev_, ftsizeu_);
            const t_complex I(0, 1);
            interpolation_matrix.coeffRef(m, index) += std::exp(-2 * purify_pi * I *((k_u(m) + ju) * 0.5 + (k_v(m) + jv) * 0.5 )) * kernelu(u(m)-(k_u(m)+ju)) * kernelv(v(m)-(k_v(m)+jv));
          }
      }
    }
    
   return interpolation_matrix; 
  }
  Image<t_real> MeasurementOperator::init_correction2d(const std::function<t_real(t_real)> ftkernelu, const std::function<t_real(t_real)> ftkernelv)
  {
    /*
      Given the Fourier transform of a gridding kernel, creates the scaling image for gridding correction.

    */
    t_int x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
    t_int y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
    Array<t_real> range;
    range.setLinSpaced(std::max(ftsizeu_, ftsizev_), 0.5, std::max(ftsizeu_, ftsizev_) - 0.5);
    return (1e0 / range.segment(y_start, imsizey_).unaryExpr(ftkernelv)).matrix() * (1e0 / range.segment(x_start, imsizex_).unaryExpr(ftkernelu)).matrix().transpose();
  }

  Image<t_real> MeasurementOperator::init_correction2d_fft(const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int Ju, const t_int Jv)
  {
    /*
      Given the gridding kernel, creates the scaling image for gridding correction using an fft.

    */
    Matrix<t_complex> K = Matrix<t_complex>::Zero(ftsizeu_, ftsizev_);
    for (int i = 0; i < Ju; ++i)
    {
      t_int n = utilities::mod(i - Ju/2, ftsizeu_);
      for (int j = 0; j < Jv; ++j)
      {
        t_int m = utilities::mod(j - Jv/2, ftsizev_);
        const t_complex I(0, 1);
        K(n, m) = kernelu(i - Ju/2) * kernelv(j - Jv/2) * std::exp(-2 * purify_pi * I * ((i - Ju/2)  * 0.5 + (j - Jv/2) * 0.5 ));
      }
    }
    t_int x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
    t_int y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
    Image<t_real> S = fftop.inverse(K).array().real().block(y_start, x_start, imsizey_, imsizex_); // probably really slow!
    return 1/S;

  }  


  Image<t_real> MeasurementOperator::init_primary_beam(const std::string & primary_beam, const t_real& cell_x, const t_real& cell_y){
    /*
      Calcualte primary beam, A, for the measurement operator.
    */
      t_int x_start = std::floor(ftsizeu_ * 0.5 - imsizex_ * 0.5);
      t_int y_start = std::floor(ftsizev_ * 0.5 - imsizey_ * 0.5);
      Array<t_real> range;
      range.setLinSpaced(std::max(ftsizeu_, ftsizev_), 0.5, std::max(ftsizeu_, ftsizev_) - 0.5);
    
      if (primary_beam == "atca")
      {
        auto const ftx = ftsizeu_;
        auto const fty = ftsizev_;
        auto pbcorr_x = [&cell_x, &ftx](const t_real& x){
          auto x0 = std::floor(ftx / 2);
          return std::exp(-4 * std::log(2) * 42. / ((x - x0) * cell_x));
        };
        auto pbcorr_y = [&cell_y, &fty](const t_real& y){
          auto y0 = std::floor(fty / 2);
          return std::exp(-4 * std::log(2) * 42. / ((y - y0) * cell_y));
        };
        return (1e0 / range.segment(x_start, imsizex_).unaryExpr(pbcorr_x)).matrix() * (1e0 / range.segment(y_start, imsizey_).unaryExpr(pbcorr_y)).matrix().transpose();
      }
      return Image<t_real>::Zero(imsizey_, imsizex_) + 1.;
  }

  t_real MeasurementOperator::power_method(const t_int & niters, const t_real & relative_difference){
    /*
     Attempt at coding the power method, returns the largest eigen value of a linear operator
      niters:: max number of iterations
      relative_difference:: percentage difference at which eigen value has converged
    */
     t_real estimate_eigen_value = 1;
     t_real old_value = 0;
     Image<t_complex> estimate_eigen_vector = Image<t_complex>::Random(imsizey_, imsizex_);
     estimate_eigen_vector = estimate_eigen_vector / estimate_eigen_vector.matrix().norm();
     std::cout << "Starting power method " << '\n';
     std::cout << "Iteration: " << 0 << ", norm = " << estimate_eigen_value << '\n';
     for (t_int i = 0; i < niters; ++i)
     {
      auto new_estimate_eigen_vector = MeasurementOperator::grid(MeasurementOperator::degrid(estimate_eigen_vector));
      estimate_eigen_value = new_estimate_eigen_vector.matrix().norm();
      estimate_eigen_vector = new_estimate_eigen_vector/estimate_eigen_value;
      std::cout << "Iteration: " << i + 1 << ", norm = " << estimate_eigen_value << '\n';
      if (relative_difference > std::abs(old_value - estimate_eigen_value)/old_value)
        break;
      old_value = estimate_eigen_value;
     }
     return old_value;
  }
  MeasurementOperator::MeasurementOperator(const utilities::vis_params& uv_vis_input, const t_int &Ju, const t_int &Jv,
      const std::string &kernel_name, const t_int &imsizex, const t_int &imsizey, const t_int & norm_iterations, const t_real &oversample_factor, const t_real & cell_x, const t_real & cell_y,
       const std::string& weighting_type, const t_real& R, bool use_w_term, const t_real& energy_fraction,const std::string & primary_beam, bool fft_grid_correction){
          /*
      Generates operators needed for gridding and degridding.

      uv_vis_input:: coordinates, weights, and visibilities.
      Ju, Jv:: support size in cells. e.g. Ju = 4, Jv =4.
      kernel_name:: Name of kernel. e.g. "kb"
      imsizey, imsizex:: size of image
      norm_iterations:: number of max iterations in power method
      oversample_factor:: ratio of fourier grid size to image size
      cell_x, cell_y:: size of a pixel in arcseconds.
      weighting_type:: weighting schemes such as whiten, natural, uniform, robust.
      R:: robustness parameter, e.g. 0.
      use_w_term:: weither to include wterm.
      energy_fraction:: how much energy to keep for chirp matrix in w-projection
      primary_beam:: which primary beam model to use, e.g. "atca"
      fft_grid_correction:: Calculate grid correction using FFT or analytically


    */
      *this = MeasurementOperator::Ju(Ju)
      .Jv(Jv)
      .kernel_name(kernel_name)
      .imsizex(imsizex)
      .imsizey(imsizey)
      .norm_iterations(norm_iterations)
      .oversample_factor(oversample_factor)
      .cell_x(cell_x)
      .cell_y(cell_y)
      .weighting_type(weighting_type)
      .R(R)
      .use_w_term(use_w_term)
      .energy_fraction(energy_fraction)
      .primary_beam(primary_beam)
      .fft_grid_correction(fft_grid_correction);
      MeasurementOperator::init_operator(uv_vis_input);

      }
  MeasurementOperator::MeasurementOperator(){
    //Most basic constructor
  }

  void MeasurementOperator::operator() (const utilities::vis_params& uv_vis_input){
    //setting up operator
    MeasurementOperator::init_operator(uv_vis_input);
  }
  void MeasurementOperator::init_operator(const utilities::vis_params& uv_vis_input)
  {
    ftsizeu_ = floor(imsizex_ * oversample_factor_);
    ftsizev_ = floor(imsizey_ * oversample_factor_);
    
    if (use_w_term_){
      resample_factor = wprojection::upsample_ratio(uv_vis_input, cell_x_, cell_y_, ftsizeu_, ftsizev_);
      ftsizeu_ = ftsizeu_ * resample_factor;
      ftsizev_ = ftsizev_ * resample_factor;
    }
    utilities::vis_params uv_vis = uv_vis_input;
    if (uv_vis.units == "lambda")
      uv_vis = utilities::set_cell_size(uv_vis_input, cell_x_, cell_y_);
    if (uv_vis.units == "radians")
      uv_vis = utilities::uv_scale(uv_vis, floor(oversample_factor_ * imsizex_), floor(oversample_factor_ * imsizey_));
    

    std::printf("------ \n");
    std::printf("Planning FFT operator \n");
    fftop.set_up_multithread();
    fftop.init_plan(Matrix<t_complex>::Zero(ftsizev_, ftsizeu_));
    std::printf("Constructing Gridding Operator: G\n");

    std::printf("Oversampling Factor: %f \n", oversample_factor_);
    std::function<t_real(t_real)> kernelu;
    std::function<t_real(t_real)> kernelv;
    std::function<t_real(t_real)> ftkernelu;
    std::function<t_real(t_real)> ftkernelv;
    if (use_w_term_)
      std::printf("Resampling Factor: %f \n", resample_factor);
    std::printf("Kernel Name: %s \n", kernel_name_.c_str());
    std::printf("Number of visibilities: %ld \n", uv_vis.u.size());
    std::printf("Number of pixels: %d x %d \n", imsizex_, imsizey_);
    std::printf("Ju: %d \n", Ju_);
    std::printf("Jv: %d \n", Jv_);

    S = Image<t_real>::Zero(imsizey_, imsizex_);

    //samples for kb_interp
    if (kernel_name_ == "kb_interp")
    {

      const t_real kb_interp_alpha = purify_pi * std::sqrt(Ju_ * Ju_/(oversample_factor_ * oversample_factor_) * (oversample_factor_ - 0.5) * (oversample_factor_ - 0.5) - 0.8);
      const t_int sample_density = 7280;
      const t_int total_samples = sample_density * Ju_;
      auto kb_general = [&] (t_real x) { return kernels::kaiser_bessel_general(x, Ju_, kb_interp_alpha); };
      Vector<t_real> samples = kernels::kernel_samples(total_samples, kb_general, Ju_);
      auto kb_interp = [&] (t_real x) { return kernels::kernel_linear_interp(samples, x, Ju_); };
      kernelu = kb_interp;
      kernelv = kb_interp;
      //Since kb_interp needs the pre-samples, all calculations need to be done within scope of this if statement. Otherwise massif gets a segfault.
      //S = MeasurementOperator::MeasurementOperator::init_correction2d_fft(kernelu, kernelv, Ju_, Jv_);
      auto ftkb = [&] (t_real x) { return kernels::ft_kaiser_bessel_general(x/ftsizeu_ - 0.5, Ju_, kb_interp_alpha); };
      ftkernelu = ftkb;
      ftkernelv = ftkb;
      S = MeasurementOperator::init_correction2d(ftkernelu, ftkernelv); // Does gridding correction using analytic formula
      G = MeasurementOperator::init_interpolation_matrix2d(uv_vis.u, uv_vis.v, Ju_, Jv_, kernelu, kernelv);
      if (use_w_term_)
      {
        C = wprojection::create_chirp_matrix(uv_vis.w, cell_x_, cell_y_, ftsizeu_, ftsizev_, energy_fraction_);
        G = wprojection::convolution(G, C, ftsizeu_, ftsizev_, uv_vis.w.size());
      }
      
      std::printf("Calculating weights: W \n");
      W = utilities::init_weights(uv_vis.u, uv_vis.v, uv_vis.weights, oversample_factor_, weighting_type_, R_, ftsizeu_, ftsizev_);
      std::printf("Calculating the primary beam: A \n");
      auto A = MeasurementOperator::init_primary_beam(primary_beam_, cell_x_, cell_y_);
      S = S * A;
      std::printf("Doing power method: eta_{i+1}x_{i + 1} = Psi^T Psi x_i \n");
      norm = std::sqrt(MeasurementOperator::power_method(norm_iterations_));
      std::printf("Found a norm of eta = %f \n", norm);
      std::printf("Gridding Operator Constructed: WGFSA \n");
      std::printf("------ \n");
      return;
    }

    if ((kernel_name_ == "pswf") and (Ju_ != 6 or Jv_ != 6))
    {
      std::cout << "Error: Only a support of 6 is implemented for PSWFs.";
    }
    if (kernel_name_ == "kb")
    {
      auto kbu = [&] (t_real x) { return kernels::kaiser_bessel(x, Ju_); };
      auto kbv = [&] (t_real x) { return kernels::kaiser_bessel(x, Jv_); };
      auto ftkbu = [&] (t_real x) { return kernels::ft_kaiser_bessel(x/ftsizeu_ - 0.5, Ju_); };
      auto ftkbv = [&] (t_real x) { return kernels::ft_kaiser_bessel(x/ftsizev_ - 0.5, Jv_); };
      kernelu = kbu;
      kernelv = kbv;
      ftkernelu = ftkbu;
      ftkernelv = ftkbv;
    }
    if (kernel_name_ == "kb_min")
    {
      const t_real kb_interp_alpha_Ju = purify_pi * std::sqrt(Ju_ * Ju_/(oversample_factor_ * oversample_factor_) * (oversample_factor_ - 0.5) * (oversample_factor_ - 0.5) - 0.8);
      const t_real kb_interp_alpha_Jv = purify_pi * std::sqrt(Jv_ * Jv_/(oversample_factor_ * oversample_factor_) * (oversample_factor_ - 0.5) * (oversample_factor_ - 0.5) - 0.8);
      auto kbu = [&] (t_real x) { return kernels::kaiser_bessel_general(x, Ju_, kb_interp_alpha_Ju); };
      auto kbv = [&] (t_real x) { return kernels::kaiser_bessel_general(x, Jv_, kb_interp_alpha_Jv); };
      auto ftkbu = [&] (t_real x) { return kernels::ft_kaiser_bessel_general(x/ftsizeu_ - 0.5, Ju_, kb_interp_alpha_Ju);  };
      auto ftkbv = [&] (t_real x) { return kernels::ft_kaiser_bessel_general(x/ftsizev_ - 0.5, Jv_, kb_interp_alpha_Jv);  };
      kernelu = kbu;
      kernelv = kbv;
      ftkernelu = ftkbu;
      ftkernelv = ftkbv;
    }
    if (kernel_name_ == "pswf")
    {
      auto pswfu = [&] (t_real x) { return kernels::pswf(x, Ju_); };
      auto pswfv = [&] (t_real x) { return kernels::pswf(x, Jv_); };
      auto ftpswfu = [&] (t_real x) { return kernels::ft_pswf(x/ftsizeu_ - 0.5, Ju_); };
      auto ftpswfv = [&] (t_real x) { return kernels::ft_pswf(x/ftsizev_ - 0.5, Jv_); };
      kernelu = pswfu;
      kernelv = pswfv;
      ftkernelu = ftpswfu;
      ftkernelv = ftpswfv;
    }
    if (kernel_name_ == "gauss")
    {
      auto gaussu = [&] (t_real x) { return kernels::gaussian(x, Ju_); };
      auto gaussv = [&] (t_real x) { return kernels::gaussian(x, Jv_); };
      auto ftgaussu = [&] (t_real x) { return kernels::ft_gaussian(x/ftsizeu_ - 0.5, Ju_); };
      auto ftgaussv = [&] (t_real x) { return kernels::ft_gaussian(x/ftsizev_ - 0.5, Jv_); };      
      kernelu = gaussu;
      kernelv = gaussv;
      ftkernelu = ftgaussu;
      ftkernelv = ftgaussv;
    }
    if (kernel_name_ == "box")
    {
      auto boxu = [&] (t_real x) { return kernels::pill_box(x, Ju_); };
      auto boxv = [&] (t_real x) { return kernels::pill_box(x, Jv_); };
      auto ftboxu = [&] (t_real x) { return kernels::ft_pill_box(x/ftsizeu_ - 0.5, Ju_); };
      auto ftboxv = [&] (t_real x) { return kernels::ft_pill_box(x/ftsizev_ - 0.5, Jv_); };      
      kernelu = boxu;
      kernelv = boxv;
      ftkernelu = ftboxu;
      ftkernelv = ftboxv;
    }
    if (kernel_name_ == "gauss_alt")
    {
      const t_real sigma = 1; //In units of radians, Rafael uses sigma = 2 * pi / ftsizeu_. However, this should be 1 in units of pixels.
      auto gaussu = [&] (t_real x) { return kernels::gaussian_general(x, Ju_, sigma); };
      auto gaussv = [&] (t_real x) { return kernels::gaussian_general(x, Jv_, sigma); };
      auto ftgaussu = [&] (t_real x) { return kernels::ft_gaussian_general(x/ftsizeu_ - 0.5, Ju_, sigma); };
      auto ftgaussv = [&] (t_real x) { return kernels::ft_gaussian_general(x/ftsizev_ - 0.5, Jv_, sigma); };      
      kernelu = gaussu;
      kernelv = gaussv;
      ftkernelu = ftgaussu;
      ftkernelv = ftgaussv;
    }
    if ( fft_grid_correction_ == true )
    {
      S = MeasurementOperator::init_correction2d_fft(kernelu, kernelv, Ju_, Jv_); // Does gridding correction with FFT
    }
    if ( fft_grid_correction_ == false )
    {
      S = MeasurementOperator::init_correction2d(ftkernelu, ftkernelv); // Does gridding correction using analytic formula
    }

    G = MeasurementOperator::init_interpolation_matrix2d(uv_vis.u, uv_vis.v, Ju_, Jv_, kernelu, kernelv);
    if (use_w_term_)
    {
      C = wprojection::create_chirp_matrix(uv_vis.w, cell_x_, cell_y_, ftsizeu_, ftsizev_, energy_fraction_);
      G = wprojection::convolution(G, C, ftsizeu_, ftsizev_, uv_vis.w.size());
    }
    std::printf("Calculating weights: W \n");
    W = utilities::init_weights(uv_vis.u, uv_vis.v, uv_vis.weights, oversample_factor_, weighting_type_, R_, ftsizeu_, ftsizev_);

    //It makes sense to included the primary beam at the same time the gridding correction is performed.
    std::printf("Calculating the primary beam: A \n");
    auto A = MeasurementOperator::init_primary_beam(primary_beam_, cell_x_, cell_y_);
    S = S * A;
    std::printf("Doing power method: eta_{i+1}x_{i + 1} = Psi^T Psi x_i \n");
    norm = std::sqrt(MeasurementOperator::power_method(norm_iterations_));
    std::printf("Found a norm of eta = %f \n", norm);
    std::printf("Gridding Operator Constructed: WGFSA \n");
    std::printf("------ \n");
  }


}
