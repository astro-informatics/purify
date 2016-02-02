#include "MeasurementOperator.h"

namespace purify {


  MeasurementOperator::vis_params MeasurementOperator::read_visibility(const std::string& vis_name)
  {
    /*
      Reads an csv file with u, v, visibilities and returns the vectors.

      vis_name:: name of input text file containing [u, v, real(V), imag(V)] (separated by ' ').
    */
    std::ifstream temp_file(vis_name);
    t_int row = 0;
    std::string line;
    //counts size of vis file
    while (std::getline(temp_file, line))
      ++row;
    Vector<t_real> utemp(row);
    Vector<t_real> vtemp(row);
    Vector<t_complex> vistemp(row);
    Vector<t_complex> weightstemp(row);
    std::ifstream vis_file(vis_name);

    // reads in vis file
    row = 0;
    t_real real;
    t_real imag;
    std::string s;
    std::string  entry;
    while (vis_file)
    {
      if (!std::getline(vis_file, s)) break;
      std::istringstream ss(s);
      std::getline(ss, entry, ' ');
      utemp(row) = std::stod(entry);
      std::getline(ss, entry, ' ');
      vtemp(row) = std::stod(entry);
      std::getline(ss, entry, ' ');
      real = std::stod(entry);
      std::getline(ss, entry, ' ');
      imag = std::stod(entry);
      vistemp(row) = t_complex(real, imag);
      std::getline(ss, entry, ' ');
      weightstemp(row) = 1/(std::stod(entry) * std::stod(entry));
      ++row;
    }
    MeasurementOperator::vis_params uv_vis;
    uv_vis.u = utemp;
    uv_vis.v = -vtemp; // found that a reflection is needed for the orientation of the gridded image to be correct
    uv_vis.vis = vistemp;
    uv_vis.weights = weightstemp;
    return uv_vis;
  }

  MeasurementOperator::vis_params MeasurementOperator::set_cell_size(const MeasurementOperator::vis_params& uv_vis, t_real cell_size_u, t_real cell_size_v)
  {
      /*
        Converts the units of visibilities to units of 2 * pi, while scaling for the size of a pixel (cell_size)

        uv_vis:: visibilities
        cell_size:: size of a pixel in arcseconds
      */

      MeasurementOperator::vis_params scaled_vis;

      if (cell_size_u == 0 and cell_size_v == 0)
      {
        Vector<t_real> u_dist = uv_vis.u.array() * uv_vis.u.array();
        t_real max_u = std::sqrt(u_dist.maxCoeff());
        cell_size_u = (180 * 3600) / max_u / purify_pi / 3 * 1.02; //Calculate cell size if not given one

        Vector<t_real> v_dist = uv_vis.v.array() * uv_vis.v.array();
        t_real max_v = std::sqrt(v_dist.maxCoeff());
        cell_size_v = (180 * 3600) / max_v / purify_pi / 3  * 1.02; //Calculate cell size if not given one
        std::cout << "PSF has a FWHM of " << cell_size_u * 3 << " x " << cell_size_v * 3 << " arcseconds" << '\n';
      }
      if (cell_size_v == 0)
      {
        cell_size_v = cell_size_u;
      }

      
      std::cout << "Using a pixel size of " << cell_size_u << " x " << cell_size_v << " arcseconds" << '\n';

      t_real scale_factor_u = 180 * 3600 / cell_size_u / purify_pi;
      t_real scale_factor_v = 180 * 3600 / cell_size_v / purify_pi;
      scaled_vis.u = uv_vis.u / scale_factor_u * 2 * purify_pi;
      scaled_vis.v = uv_vis.v / scale_factor_v * 2 * purify_pi;
      scaled_vis.vis = uv_vis.vis;
      scaled_vis.weights = uv_vis.weights;
      return scaled_vis;
  }

  Vector<t_complex> MeasurementOperator::apply_weights(const Vector<t_complex> visiblities, const Vector<t_complex> weights)
  {
    /*
      Applies weights to visiblities, assuming they are 1/sigma^2.
    */
    Vector<t_complex> weighted_vis;
    weighted_vis = (visiblities.array() * weights.array()).matrix();
    return weighted_vis;
  }

  MeasurementOperator::vis_params MeasurementOperator::uv_scale(const MeasurementOperator::vis_params& uv_vis, const t_int& ftsizeu, const t_int& ftsizev)
  {
    /*
      scales the uv coordinates from being in units of 2 * pi to units of pixels.
    */
      MeasurementOperator::vis_params scaled_vis;
      scaled_vis.u = uv_vis.u / (2 * purify_pi) * ftsizeu;
      scaled_vis.v = uv_vis.v / (2 * purify_pi) * ftsizev;
      scaled_vis.vis = uv_vis.vis;
      scaled_vis.weights = uv_vis.weights;
      return scaled_vis;
  }

  MeasurementOperator::vis_params MeasurementOperator::uv_symmetry(const MeasurementOperator::vis_params& uv_vis)
  {
    /*
      Adds in reflection of the fourier plane using the condjugate symmetry for a real image.

      uv_vis:: uv coordinates for the fourier plane
    */
    t_int total = uv_vis.u.size();
    Vector<t_real> utemp(2 * total);
    Vector<t_real> vtemp(2 * total);
    Vector<t_complex> vistemp(2 * total);
    Vector<t_complex> weightstemp(2 * total);
    MeasurementOperator::vis_params conj_vis;
    for (t_int i = 0; i < uv_vis.u.size(); ++i)
    {
      utemp(i) = uv_vis.u(i);
      vtemp(i) = uv_vis.v(i);
      vistemp(i) = uv_vis.vis(i);
      weightstemp(i) = uv_vis.weights(i);
    }
    for (t_int i = total; i < 2 * total; ++i)
    {
      utemp(i) = -uv_vis.u(i - total);
      vtemp(i) = -uv_vis.v(i - total);
      vistemp(i) = std::conj(uv_vis.vis(i - total));
      weightstemp(i) = uv_vis.weights(i - total);
    }
    conj_vis.u = utemp;
    conj_vis.v = vtemp;
    conj_vis.vis = vistemp;
    conj_vis.weights = weightstemp;
    return conj_vis;
  }

  Vector<t_complex> MeasurementOperator::fftshift_1d(const Vector<t_complex> input) 
  {
    /*
      Performs a 1D fftshift on a vector and returns the shifted vector

      input:: vector to perform fftshift on.
    */
  
    t_int input_size = input.size();
    Vector<t_complex> output(input_size);
    t_int NF = std::floor(input_size/2.0);
    t_int NC = std::ceil(input_size/2.0);

    
    for (t_int i=0; i<NF; i++)
      output(i)=input(i + NC);
    
    for (t_int i=NF; i<input_size; i++)
      output(i)=input(i - NF);

    return output;
  }

  Matrix<t_complex> MeasurementOperator::fftshift_2d(Matrix<t_complex> input) 
  {
    /*
      Performs a 1D fftshift on a vector and returns the shifted vector

      input:: vector to perform fftshift on.
    */
      t_int rows = input.rows();
      t_int cols = input.cols();
      for (t_int i = 0; i < cols; ++i)
        input.col(i) = MeasurementOperator::fftshift_1d(input.col(i));

      for (t_int i = 0; i < rows; ++i)
        input.row(i) = MeasurementOperator::fftshift_1d(input.row(i));

      return input;
  }

  Matrix<t_complex> MeasurementOperator::ifftshift_2d(Matrix<t_complex> input) 
  {
    /*
      Performs a 1D fftshift on a vector and returns the shifted vector

      input:: vector to perform fftshift on.
    */
      t_int rows = input.rows();
      t_int cols = input.cols();
      for (t_int i = 0; i < cols; ++i)
        input.col(i) = MeasurementOperator::ifftshift_1d(input.col(i));

      for (t_int i = 0; i < rows; ++i)
        input.row(i) = MeasurementOperator::ifftshift_1d(input.row(i));

      return input;
  }

  Vector<t_complex> MeasurementOperator::ifftshift_1d(Vector<t_complex> input) 
  {
    /*
      Performs a 1D ifftshift on a vector and returns the shifted vector

      input:: vector to perform ifftshift on.
    */
  
    t_int input_size = input.size();
    Vector<t_complex> output(input_size);
    t_int NF = std::floor(input_size/2.0);
    t_int NC = std::ceil(input_size/2.0);

    
    for (t_int i=0; i<NC; i++)
      output(i)=input(i + NF);
    
    for (t_int i=NC; i<input_size; i++)
      output(i)=input(i - NC);

    return output;
  }

  Vector<t_complex> MeasurementOperator::degrid(const Image<t_complex>& eigen_image, const MeasurementOperator::operator_params st)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      eigen_image:: input image to be degridded
      st:: gridding parameters
    */
      Matrix<t_complex> padded_image(st.ftsizeu, st.ftsizev);
      Matrix<t_complex> ft_vector(st.ftsizeu, st.ftsizev);
      t_int x_start = floor(st.ftsizeu * 0.5 - st.imsizex * 0.5);
      t_int y_start = floor(st.ftsizev * 0.5 - st.imsizey * 0.5);
      t_int x_end = floor(st.ftsizeu * 0.5 + st.imsizex * 0.5);
      t_int y_end = floor(st.ftsizev * 0.5 + st.imsizey * 0.5);

      // zero padding and gridding correction
      for (t_int i = 0; i < st.ftsizeu; ++i)
      {
        for (t_int j = 0; j < st.ftsizev; ++j)
        {
          if ( (i >= x_start) and (i < x_end) and (j >= y_start) and (j < y_end))
          {
            padded_image(j, i) = st.S(j, i) * eigen_image(j - y_start, i - x_start);
          }else{
            padded_image(j, i) = 0;
          }
        }
      }
      // create fftgrid
      ft_vector = MeasurementOperator::fft2d(MeasurementOperator::fftshift_2d(padded_image));
      // turn into vector
      ft_vector.resize(st.ftsizeu*st.ftsizev, 1);
      // get visibilities
      Vector<t_complex> visibilities = st.G * ft_vector;
      return visibilities;
      
  }

  Image<t_complex> MeasurementOperator::grid(const Vector<t_complex>& visibilities, const MeasurementOperator::operator_params st)
  {
    /*
      An operator that degrids an image and returns a vector of visibilities.

      visibilities:: input visibilities to be gridded
      st:: gridding parameters
    */
      Matrix<t_complex> ft_vector = st.G.adjoint() * visibilities;
      ft_vector.resize(st.ftsizeu, st.ftsizev);
      Matrix<t_complex> padded_image = MeasurementOperator::ifftshift_2d(MeasurementOperator::ifft2d(ft_vector));
      Image<t_complex> eigen_image(st.imsizex, st.imsizey);
      t_int x_start = floor(st.ftsizeu * 0.5 - st.imsizex * 0.5);
      t_int y_start = floor(st.ftsizev * 0.5 - st.imsizey * 0.5);
      t_int x_end = floor(st.ftsizeu * 0.5 + st.imsizex * 0.5);
      t_int y_end = floor(st.ftsizev * 0.5 + st.imsizey * 0.5);
      for (t_int i = 0; i < st.imsizex; ++i)
      {
        for (t_int j = 0; j < st.imsizey; ++j)
        {
            eigen_image(j, i) = st.S(j + y_start, i + x_start) * padded_image(j + y_start, i + x_start);
        }
      }
      return eigen_image;
      
  }

  Matrix<t_complex> MeasurementOperator::fft2d(const Matrix<t_complex>& input)
  {
    /*
      Returns FFT of a 2D matrix.

      input:: complex valued image
    */
    Eigen::FFT<t_real> fft;
    t_int dim_x = input.rows();
    t_int dim_y = input.cols();
    Matrix<t_complex> output(dim_x, dim_y);
    for (t_int k = 0; k < dim_x; k++) {
      Vector<t_complex> tmpOut(dim_y);
      Vector<t_complex> tmpIn = input.row(k);
      fft.fwd(tmpOut, tmpIn);
      output.row(k) = tmpOut;
    }

    for (t_int k = 0; k < dim_y; k++) {
      Vector<t_complex> tmpOut(dim_x);
      Vector<t_complex> tmpIn = output.col(k);
      fft.fwd(tmpOut, tmpIn);
      output.col(k) = tmpOut;
    }
    return output;
  }

  Matrix<t_complex> MeasurementOperator::ifft2d(const Matrix<t_complex>& input)
  {
    /*
      Returns FFT of a 2D matrix.

      input:: complex valued image
    */
    Eigen::FFT<t_real> fft;
    t_int dim_x = input.rows();
    t_int dim_y = input.cols();
    Matrix<t_complex> output(dim_x, dim_y);
    for (t_int k = 0; k < dim_x; k++) {
      Vector<t_complex> tmpOut(dim_y);
      Vector<t_complex> tmpIn = input.row(k);
      fft.inv(tmpOut, tmpIn);
      output.row(k) = tmpOut;
    }

    for (t_int k = 0; k < dim_y; k++) {
      Vector<t_complex> tmpOut(dim_x);
      Vector<t_complex> tmpIn = output.col(k);
      fft.inv(tmpOut, tmpIn);
      output.col(k) = tmpOut;
    }
    return output;
  }

  t_int MeasurementOperator::sub2ind(const t_int& row, const t_int& col, const t_int& rows, const t_int& cols) 
  {
    /*
      Converts (row, column) of a matrix to a single index. This does the same as the matlab funciton sub2ind, converts subscript to index.

      row:: row of matrix (y)
      col:: column of matrix (x)
      cols:: number of columns for matrix
      rows:: number of rows for matrix
     */
    return row * cols + col;
  }

  void MeasurementOperator::ind2sub(const t_int sub, const t_int cols, const t_int rows, t_int* row, t_int* col) 
  {
    /*
      Converts index of a matrix to (row, column). This does the same as the matlab funciton sub2ind, converts subscript to index.
      
      sub:: subscript of entry in matrix
      cols:: number of columns for matrix
      rows:: number of rows for matrix
      row:: output row of matrix
      col:: output column of matrix

     */
    *col = sub % cols;
    *row = (sub - *col) / cols;
  }

  t_int MeasurementOperator::mod(const t_real& x, const t_real& y) 
  {
    /*
      returns x % y, and warps circularly around y for negative values.
    */
      t_real r = std::fmod(x, y);
      if (r < 0)
        r = y + r;
      return static_cast<t_int>(r);
  }

  Vector<t_real> MeasurementOperator::omega_to_k(const Vector<t_real>& omega) {
    /*
      Maps fourier coordinates (u or v) to integer grid coordinates.

      omega:: fourier coordinates for a signle axis (u or v axis)
    */
    return omega.unaryExpr(std::ptr_fun<double,double>(std::floor));     
  }

  void MeasurementOperator::writefits2d(Image<t_real> eigen_image, const std::string& fits_name, const bool& overwrite, const bool& flip) 
  {
    /*
      Writes an image to a fits file.

      image:: image data, a 2d Image.
      fits_name:: string contating the file name of the fits file.
    */
    if (overwrite == true) {remove(fits_name.c_str());};
    std::auto_ptr<CCfits::FITS> pFits(0);
    long naxes[2] = {static_cast<long>(eigen_image.rows()), static_cast<long>(eigen_image.cols())};
    pFits.reset(new CCfits::FITS(fits_name, FLOAT_IMG, 2, naxes));
    long fpixel (1);
    std::vector<long> extAx; 
    extAx.push_back(naxes[0]);
    extAx.push_back(naxes[1]);
    CCfits::ExtHDU* imageExt = pFits->addImage(fits_name, FLOAT_IMG, extAx);
    if (flip == true)
    {
      eigen_image = eigen_image.rowwise().reverse().eval();
    }
    eigen_image.resize(naxes[0]*naxes[1], 1);
    std::valarray<double> image(naxes[0]*naxes[1]);
    for (int i = 0; i < static_cast<int>(naxes[0]*naxes[1]); ++i)
    {
      image[i] = static_cast<float>(eigen_image(i));
    }
    imageExt->write(fpixel, naxes[0]*naxes[1], image);
  }
  
  Image<t_complex> MeasurementOperator::readfits2d(const std::string& fits_name)
  {
    /*
      Reads in an image from a fits file and returns the image.

      fits_name:: name of fits file
    */

    std::auto_ptr<CCfits::FITS> pInfile(new CCfits::FITS(fits_name, CCfits::Read, true));
    std::valarray<t_real>  contents;
    CCfits::PHDU& image = pInfile->pHDU();
    image.read(contents);
    t_int ax1(image.axis(0));
    t_int ax2(image.axis(1));
    Image<t_complex> eigen_image(ax1, ax2);
    t_int index;
    for (t_int i = 0; i < ax1; ++i)
    {
      for (t_int j = 0; j < ax2; ++j)
      {
        index = MeasurementOperator::sub2ind(j, i, ax2, ax1);
        eigen_image(i, j) = contents[index];
      }
    }
    return eigen_image;
  }

  Sparse<t_real> MeasurementOperator::init_interpolation_matrix2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev) 
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
         q = MeasurementOperator::mod(k_u(m) + ju, ftsizeu);
        for (t_int jv = 1; jv <= Jv; ++jv)
          {
            p = MeasurementOperator::mod(k_v(m) + jv, ftsizev);
            index = MeasurementOperator::sub2ind(p, q, ftsizev, ftsizeu);
            entries.push_back(t_tripletList(m, index, kernelu(u(m)-(k_u(m)+ju)) * kernelv(v(m)-(k_v(m)+jv))));
          }
        }
      }    
    Sparse<t_real> interpolation_matrix(rows, cols);
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
      t_int n = MeasurementOperator::mod(i - Ju/2, ftsizeu);
      for (int j = 0; j < Jv; ++j)
      {
        t_int m = MeasurementOperator::mod(j - Jv/2, ftsizev);
        K(n, m) = kernelu(i - Ju/2) * kernelv(j - Jv/2);
      }
    }
    Image<t_real> S = MeasurementOperator::fftshift_2d(MeasurementOperator::ifft2d(K)).array().real();
    return 1/S;

  }  

  MeasurementOperator::operator_params MeasurementOperator::init_nufft2d(const Vector<t_real>& u, const Vector<t_real>& v, const t_int Ju, const t_int Jv, const std::string kernel_name, const t_int imsizex, const t_int imsizey, const t_real oversample_factor, bool fft_grid_correction)
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


    MeasurementOperator::operator_params st;
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
