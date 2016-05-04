#include "MeasurementOperator.h"

namespace purify {
  Vector<t_complex> MeasurementOperator::degrid(const Image<t_complex>& eigen_image)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      eigen_image:: input image to be degridded
      st:: gridding parameters
    */
      Matrix<t_complex> padded_image = Matrix<t_complex>::Zero(floor(imsizex * oversample_factor), floor(imsizey * oversample_factor));
      Matrix<t_complex> ft_vector(ftsizeu, ftsizev);
      t_int x_start = floor(floor(imsizex * oversample_factor) * 0.5 - imsizex * 0.5);
      t_int y_start = floor(floor(imsizey * oversample_factor) * 0.5 - imsizey * 0.5);

      // zero padding and gridding correction
      padded_image.block(y_start, x_start, imsizey, imsizex)
          = S * eigen_image;
      

      // create fftgrid
      ft_vector = utilities::re_sample_ft_grid(fftop.forward(padded_image), resample_factor); // the fftshift is not needed because of the phase shift in the gridding kernel
      // turn into vector
      ft_vector.resize(ftsizeu*ftsizev, 1); // using conservativeResize does not work, it garbles the image. Also, it is not what we want.
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
      Matrix<t_complex> ft_vector = G.adjoint() * (visibilities.array() * W).matrix()/norm;
      ft_vector.resize(ftsizeu, ftsizev); // using conservativeResize does not work, it garbles the image. Also, it is not what we want.
      ft_vector = utilities::re_sample_ft_grid(ft_vector, 1./resample_factor);
      Image<t_complex> padded_image = fftop.inverse(ft_vector); // the fftshift is not needed because of the phase shift in the gridding kernel
      t_int x_start = floor(floor(imsizex * oversample_factor) * 0.5 - imsizex * 0.5);
      t_int y_start = floor(floor(imsizey * oversample_factor) * 0.5 - imsizey * 0.5);
      return S * padded_image.block(y_start, x_start, imsizey, imsizex);
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
    /*
      Calculate the weights to be applied to the visibilities in the measurement operator. It does none, whiten, natural, uniform, and robust.
    */
    Vector<t_complex> out_weights(weights.size());
    t_complex mean_weights = weights.sum();
    if (weighting_type == "none")
    {
      out_weights = weights.array() * 0 + 1;
    } else if (weighting_type == "whiten"){
      out_weights = weights.array().sqrt();
    }
    else if (weighting_type == "natural")
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

  Image<t_real> MeasurementOperator::init_primary_beam(const std::string & primary_beam){
    /*
      Calcualte primary beam, A, for the measurement operator.
    */

      return Image<t_real>::Zero(imsizey, imsizex) + 1.;
  }

  t_real MeasurementOperator::power_method(const t_int niters){
    /*
     Attempt at coding the power method, returns the largest eigen value of a linear operator
    */
     t_real estimate_eigen_value = 1;
     Image<t_complex> estimate_eigen_vector = Image<t_complex>::Random(imsizey, imsizex);
     //std::cout << "Starting power method " << '\n';
     //std::cout << "Iteration: " << 0 << ", norm = " << estimate_eigen_value << '\n';
     for (t_int i = 0; i < niters; ++i)
     {
      auto new_estimate_eigen_vector = MeasurementOperator::grid(MeasurementOperator::degrid(estimate_eigen_vector));
      estimate_eigen_value = new_estimate_eigen_vector.matrix().norm();
      estimate_eigen_vector = new_estimate_eigen_vector/estimate_eigen_value;
      //std::cout << "Iteration: " << i + 1 << ", norm = " << estimate_eigen_value << '\n';
     }
     return estimate_eigen_value;
  }


  Matrix<t_complex> MeasurementOperator::create_chirp_matrix(const Vector<t_real> & w_components, const t_real cell_x, const t_real cell_y, const t_real& energy_fraction){

    const t_int total_rows = w_components.size();
    const t_int total_cols = ftsizeu * ftsizev;
    //std::vector<t_tripletList> entries;
    //entries.reserve(total_rows * total_cols);

    Matrix<t_complex> chirp_matrix = Matrix<t_complex>::Zero(total_rows, total_cols);
    std::cout << "Generating chirp matrix with " << total_rows << " x " << total_cols << '\n';
    for (t_int m = 0; m < total_rows; ++m)
    {
      Image<t_complex> chirp_image = utilities::generate_chirp(w_components(m), cell_x, cell_y, ftsizeu, ftsizev);
      
      Matrix<t_complex> row = fftop.forward(chirp_image);
      row = utilities::sparsify_chirp(row, energy_fraction);

      row.resize(1, total_cols);
      chirp_matrix.row(m) = row;
      //for (t_int j = 0; j < row.size(); ++j)
      //{
        //if (std::abs(row(j)) == 0)
        //{
        //  entries.emplace_back(m, j, row(j));
        //}
      //}
    }
    //Sparse<t_complex> chirp_matrix(total_rows, total_cols);
    //chirp_matrix.setFromTriplets(entries.begin(), entries.end());
    
    return chirp_matrix;
  }

  Image<t_complex> MeasurementOperator::covariance_calculation(const Image<t_complex> & vector){
    /*
      Calculates a new representation of the covariance matrix, using propogation of uncertainty A Sigma A^T

      pixel:: determines the column to calculate for the covariance matrix.
    */
      Array<t_real> covariance = 1./(W.real());
      return MeasurementOperator::grid(covariance * MeasurementOperator::degrid(vector).array());

  }

  MeasurementOperator::MeasurementOperator(const utilities::vis_params& uv_vis_input, const t_int &Ju, const t_int &Jv,
      const std::string &kernel_name, const t_int &imsizex, const t_int &imsizey, const t_real &oversample_factor, const t_real & cell_x, const t_real & cell_y,
       const std::string& weighting_type, const t_real& R, bool use_w_term, const t_real& energy_fraction,const std::string & primary_beam, bool fft_grid_correction)
      : imsizex(imsizex), imsizey(imsizey), ftsizeu(floor(oversample_factor * imsizex)), ftsizev(floor(oversample_factor * imsizey)), use_w_term(use_w_term), oversample_factor(oversample_factor)
    
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
    
    if (use_w_term){
      resample_factor = utilities::upsample_ratio(uv_vis_input, cell_x, cell_y, ftsizeu, ftsizev);
      ftsizeu = ftsizeu * resample_factor;
      ftsizev = ftsizev * resample_factor;
    }
    utilities::vis_params uv_vis = uv_vis_input;
    if (uv_vis.units == "lambda")
      uv_vis = utilities::set_cell_size(uv_vis_input, cell_x, cell_y);
    if (uv_vis.units == "radians")
      uv_vis = utilities::uv_scale(uv_vis, floor(oversample_factor * imsizex), floor(oversample_factor * imsizey));
    

    //t_real new_upsample = utilities::upsample_ratio(uv_vis, ,);
    std::printf("------ \n");
    std::printf("Constructing Gridding Operator \n");

    std::printf("Oversampling Factor: %f \n", oversample_factor);
    std::function<t_real(t_real)> kernelu;
    std::function<t_real(t_real)> kernelv;
    std::function<t_real(t_real)> ftkernelu;
    std::function<t_real(t_real)> ftkernelv;
    if (use_w_term)
      std::printf("Resampling Factor: %f \n", resample_factor);
    std::printf("Kernel Name: %s \n", kernel_name.c_str());
    std::printf("Number of visibilities: %ld \n", uv_vis.u.size());
    std::printf("Ju: %d \n", Ju);
    std::printf("Jv: %d \n", Jv);

    S = Image<t_real>::Zero(imsizey, imsizex);
    const t_int norm_iterations = 20; // number of iterations for power method

    //samples for kb_interp
    if (kernel_name == "kb_interp")
    {

      const t_real kb_interp_alpha = purify_pi * std::sqrt(Ju * Ju/(oversample_factor * oversample_factor) * (oversample_factor - 0.5) * (oversample_factor - 0.5) - 0.8);
      const t_int sample_density = 7280;
      const t_int total_samples = sample_density * Ju;
      auto kb_general = [&] (t_real x) { return kernels::kaiser_bessel_general(x, Ju, kb_interp_alpha); };
      Vector<t_real> samples = kernels::kernel_samples(total_samples, kb_general, Ju);
      auto kb_interp = [&] (t_real x) { return kernels::kernel_linear_interp(samples, x, Ju); };
      kernelu = kb_interp;
      kernelv = kb_interp;
      //Since kb_interp needs the pre-samples, all calculations need to be done within scope of this if statement. Otherwise massif gets a segfault.
      //S = MeasurementOperator::MeasurementOperator::init_correction2d_fft(kernelu, kernelv, Ju, Jv);
      auto ftkb = [&] (t_real x) { return kernels::ft_kaiser_bessel_general(x/ftsizeu - 0.5, Ju, kb_interp_alpha); };
      ftkernelu = ftkb;
      ftkernelv = ftkb;
      S = MeasurementOperator::init_correction2d(ftkernelu, ftkernelv); // Does gridding correction using analytic formula
      G = MeasurementOperator::init_interpolation_matrix2d(uv_vis.u, uv_vis.v, Ju, Jv, kernelu, kernelv);
      if (use_w_term)
      {
        C = MeasurementOperator::create_chirp_matrix(uv_vis.w, cell_x, cell_y, energy_fraction);
        G = utilities::convolution(G, C, ftsizeu, ftsizev, uv_vis.w.size());
      }
      
      std::printf("Calculating weights \n");
      W = MeasurementOperator::init_weights(uv_vis.u, uv_vis.v, uv_vis.weights, oversample_factor, weighting_type, R);
      auto A = MeasurementOperator::init_primary_beam(primary_beam);
      S = S * A;
      std::printf("Doing power method \n");
      norm = std::sqrt(MeasurementOperator::power_method(norm_iterations));
      std::printf("Gridding Operator Constructed \n");
      std::printf("------ \n");
      return;
    }

    if ((kernel_name == "pswf") and (Ju != 6 or Jv != 6))
    {
      std::cout << "Error: Only a support of 6 is implemented for PSWFs.";
    }
    if (kernel_name == "kb")
    {
      auto kbu = [&] (t_real x) { return kernels::kaiser_bessel(x, Ju); };
      auto kbv = [&] (t_real x) { return kernels::kaiser_bessel(x, Jv); };
      auto ftkbu = [&] (t_real x) { return kernels::ft_kaiser_bessel(x/ftsizeu - 0.5, Ju); };
      auto ftkbv = [&] (t_real x) { return kernels::ft_kaiser_bessel(x/ftsizev - 0.5, Jv); };
      kernelu = kbu;
      kernelv = kbv;
      ftkernelu = ftkbu;
      ftkernelv = ftkbv;
    }
    if (kernel_name == "kb_min")
    {
      const t_real kb_interp_alpha_Ju = purify_pi * std::sqrt(Ju * Ju/(oversample_factor * oversample_factor) * (oversample_factor - 0.5) * (oversample_factor - 0.5) - 0.8);
      const t_real kb_interp_alpha_Jv = purify_pi * std::sqrt(Jv * Jv/(oversample_factor * oversample_factor) * (oversample_factor - 0.5) * (oversample_factor - 0.5) - 0.8);
      auto kbu = [&] (t_real x) { return kernels::kaiser_bessel_general(x, Ju, kb_interp_alpha_Ju); };
      auto kbv = [&] (t_real x) { return kernels::kaiser_bessel_general(x, Jv, kb_interp_alpha_Jv); };
      auto ftkbu = [&] (t_real x) { return kernels::ft_kaiser_bessel_general(x/ftsizeu - 0.5, Ju, kb_interp_alpha_Ju);  };
      auto ftkbv = [&] (t_real x) { return kernels::ft_kaiser_bessel_general(x/ftsizev - 0.5, Jv, kb_interp_alpha_Jv);  };
      kernelu = kbu;
      kernelv = kbv;
      ftkernelu = ftkbu;
      ftkernelv = ftkbv;
    }
    if (kernel_name == "pswf")
    {
      auto pswfu = [&] (t_real x) { return kernels::pswf(x, Ju); };
      auto pswfv = [&] (t_real x) { return kernels::pswf(x, Jv); };
      auto ftpswfu = [&] (t_real x) { return kernels::ft_pswf(x/ftsizeu - 0.5, Ju); };
      auto ftpswfv = [&] (t_real x) { return kernels::ft_pswf(x/ftsizev - 0.5, Jv); };
      kernelu = pswfu;
      kernelv = pswfv;
      ftkernelu = ftpswfu;
      ftkernelv = ftpswfv;
    }
    if (kernel_name == "gauss")
    {
      auto gaussu = [&] (t_real x) { return kernels::gaussian(x, Ju); };
      auto gaussv = [&] (t_real x) { return kernels::gaussian(x, Jv); };
      auto ftgaussu = [&] (t_real x) { return kernels::ft_gaussian(x/ftsizeu - 0.5, Ju); };
      auto ftgaussv = [&] (t_real x) { return kernels::ft_gaussian(x/ftsizev - 0.5, Jv); };      
      kernelu = gaussu;
      kernelv = gaussv;
      ftkernelu = ftgaussu;
      ftkernelv = ftgaussv;
    }
    if (kernel_name == "box")
    {
      auto boxu = [&] (t_real x) { return kernels::pill_box(x, Ju); };
      auto boxv = [&] (t_real x) { return kernels::pill_box(x, Jv); };
      auto ftboxu = [&] (t_real x) { return kernels::ft_pill_box(x/ftsizeu - 0.5, Ju); };
      auto ftboxv = [&] (t_real x) { return kernels::ft_pill_box(x/ftsizev - 0.5, Jv); };      
      kernelu = boxu;
      kernelv = boxv;
      ftkernelu = ftboxu;
      ftkernelv = ftboxv;
    }
    if (kernel_name == "gauss_alt")
    {
      const t_real sigma = 1; //In units of radians, Rafael uses sigma = 2 * pi / ftsizeu. However, this should be 1 in units of pixels.
      auto gaussu = [&] (t_real x) { return kernels::gaussian_general(x, Ju, sigma); };
      auto gaussv = [&] (t_real x) { return kernels::gaussian_general(x, Jv, sigma); };
      auto ftgaussu = [&] (t_real x) { return kernels::ft_gaussian_general(x/ftsizeu - 0.5, Ju, sigma); };
      auto ftgaussv = [&] (t_real x) { return kernels::ft_gaussian_general(x/ftsizev - 0.5, Jv, sigma); };      
      kernelu = gaussu;
      kernelv = gaussv;
      ftkernelu = ftgaussu;
      ftkernelv = ftgaussv;
    }
    if ( fft_grid_correction == true )
    {
      S = MeasurementOperator::init_correction2d_fft(kernelu, kernelv, Ju, Jv); // Does gridding correction with FFT
    }
    if ( fft_grid_correction == false )
    {
      S = MeasurementOperator::init_correction2d(ftkernelu, ftkernelv); // Does gridding correction using analytic formula
    }

    G = MeasurementOperator::init_interpolation_matrix2d(uv_vis.u, uv_vis.v, Ju, Jv, kernelu, kernelv);
    if (use_w_term)
    {
      C = MeasurementOperator::create_chirp_matrix(uv_vis.w, cell_x, cell_y, energy_fraction);
      G = utilities::convolution(G, C, ftsizeu, ftsizev, uv_vis.w.size());
    }
    std::printf("Calculating weights \n");
    W = MeasurementOperator::init_weights(uv_vis.u, uv_vis.v, uv_vis.weights, oversample_factor, weighting_type, R);

    //It makes sense to included the primary beam at the same time the gridding correction is performed.
    std::printf("Calculating the primary beam \n");
    auto A = MeasurementOperator::init_primary_beam(primary_beam);
    S = S * A;
    std::printf("Doing power method \n");
    norm = std::sqrt(MeasurementOperator::power_method(norm_iterations));
    std::printf("Found a norm of %f \n", norm);
    std::printf("Gridding Operator Constructed \n");
    std::printf("------ \n");
  }


}
