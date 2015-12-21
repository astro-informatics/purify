#include "MeasurementOperator.h"

namespace purify {

  MeasurementOperator::vis_params MeasurementOperator::read_visibility(const std::string vis_name)
  {
    /*
      Reads an csv file with u, v, visibilities and returns the vectors.

      vis_name:: name of input text file containing [u, v, real(V), imag(V)] (separated by ' ').
    */
    std::ifstream temp_file(vis_name);
    t_int row = 0;
    std::string line;
    while (std::getline(temp_file, line))
      ++row;
    Vector<t_real> utemp(row);
    Vector<t_real> vtemp(row);
    Vector<t_complex> vistemp(row);
    std::ifstream vis_file(vis_name);

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
      ++row;
    }
    MeasurementOperator::vis_params uv_vis;
    uv_vis.u = utemp;
    uv_vis.v = vtemp;
    uv_vis.vis = vistemp;
    return uv_vis;
  }

  MeasurementOperator::vis_params MeasurementOperator::uv_symmetry(const MeasurementOperator::vis_params uv_vis)
  {
    t_int total = uv_vis.u.size();
    Vector<t_real> utemp(2 * total);
    Vector<t_real> vtemp(2 * total);
    Vector<t_complex> vistemp(2 * total);
    MeasurementOperator::vis_params conj_vis;
    for (t_int i = 0; i < uv_vis.u.size(); ++i)
    {
      utemp(i) = uv_vis.u(i);
      vtemp(i) = uv_vis.v(i);
      vistemp(i) = uv_vis.vis(i);
    }
    for (t_int i = total; i < 2 * total; ++i)
    {
      utemp(i) = -uv_vis.u(i - total);
      vtemp(i) = -uv_vis.v(i - total);
      vistemp(i) = std::conj(uv_vis.vis(i - total));
    }
    conj_vis.u = utemp;
    conj_vis.v = vtemp;
    conj_vis.vis = vistemp;
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

  Vector<t_complex> MeasurementOperator::degrid(Image <t_complex> eigen_image, const MeasurementOperator::operator_params st)
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
            padded_image(i, j) = st.S(i, j) * eigen_image(i - x_start, j - y_start);
          }else{
            padded_image(i, j) = 0;
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
  Image<t_complex> MeasurementOperator::grid(Vector <t_complex> visibilities, const MeasurementOperator::operator_params st)
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
            eigen_image(i, j) = st.S(i + x_start, j + y_start) * padded_image(i + x_start, j + y_start);
        }
      }
      return eigen_image;
      
  }

  Matrix<t_complex> MeasurementOperator::fft2d(const Matrix<t_complex> input)
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

  Matrix<t_complex> MeasurementOperator::ifft2d(const Matrix<t_complex> input)
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
  t_int MeasurementOperator::sub2ind(const t_int row, const t_int col, const t_int rows, const t_int cols) 
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
  t_int MeasurementOperator::mod(const t_real x, const t_real y) 
  {
    /*
      returns x % y, and warps circularly around y for negative values.
    */
      t_real r = std::fmod(x, y);
      if (r < 0)
        r = y + r;
      return static_cast<t_int>(r);
  }
  Vector<t_real> MeasurementOperator::omega_to_k(const Vector<t_real> omega) {
    /*
      Maps fourier coordinates (u or v) to integer grid coordinates.

      omega:: fourier coordinates for a signle axis (u or v axis)
    */
    return omega.unaryExpr(std::ptr_fun<double,double>(std::floor));     
  }



  void MeasurementOperator::writefits2d(Image<t_real> eigen_image, const std::string fits_name, const bool overwrite) 
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
    eigen_image.resize(naxes[0]*naxes[1], 1);
    std::valarray<double> image(naxes[0]*naxes[1]);
    for (int i = 0; i < static_cast<int>(naxes[0]*naxes[1]); ++i)
    {
      image[i] = static_cast<float>(eigen_image(i));
    }
    imageExt->write(fpixel, naxes[0]*naxes[1], image);
  }
  
  Image<t_complex> MeasurementOperator::readfits2d(const std::string fits_name)
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

  Sparse<t_real> MeasurementOperator::init_interpolation_matrix2d(const Vector<t_real> u, const Vector<t_real> v, const t_int Ju, const t_int Jv, const std::function<t_real(t_real)> kernelu, const std::function<t_real(t_real)> kernelv, const t_int ftsizeu, const t_int ftsizev) 
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
  Vector<t_real> ones = u * 0;
  ones.setOnes();  
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
            if (index < 0)
            {
              std::cout << q << ", " << p << '\n';
            }
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
        S(i ,j) = 1/(ftkernelu(i) * ftkernelv(j));
      }
    }
    return S;

  }

  MeasurementOperator::operator_params MeasurementOperator::init_nufft2d(Vector<t_real> u, Vector<t_real> v, const t_int Ju, const t_int Jv, const std::string kernel_name, const t_int imsizex, const t_int imsizey, const t_int oversample_factor)
  {
    /*
      Generates tools/operators needed for gridding and degridding.

      u:: visibilities in units of 2*pi;

    */
    std::cout << "Constructing Gridding Operator" << '\n';
    MeasurementOperator::operator_params st;
    st.imsizex = imsizex;
    st.imsizey = imsizey;
    st.ftsizeu = oversample_factor * imsizex;
    st.ftsizev = oversample_factor * imsizey;
    u = u / (2 * pi) * st.ftsizeu;
    v = v / (2 * pi) * st.ftsizev;
    auto kernelu = [&] (t_real x) { return MeasurementOperator::choose_kernel(x, Ju, kernel_name); };
    auto kernelv = [&] (t_real x) { return MeasurementOperator::choose_kernel(x, Jv, kernel_name); };
    auto ftkernelu = [&] (t_real x) { return MeasurementOperator::choose_ftkernel(x/st.ftsizeu - 0.5, Ju, kernel_name); };
    auto ftkernelv = [&] (t_real x) { return MeasurementOperator::choose_ftkernel(x/st.ftsizev - 0.5, Jv, kernel_name); };
    std::cout << "Support of Kernel" << '\n';
    std::cout << "Ju: " << Ju << '\n';
    std::cout << "Jv: " << Jv << '\n';
    st.S = MeasurementOperator::MeasurementOperator::init_correction2d(ftkernelu, ftkernelv, st.ftsizeu, st.ftsizev);
    st.G = MeasurementOperator::init_interpolation_matrix2d(u, v, Ju, Jv, kernelu, kernelv, st.ftsizeu, st.ftsizev);
    return st;
  }

  t_real MeasurementOperator::choose_kernel(const t_real x, const t_int J, const std::string kernel_name)
  {
    /*
      Chooses gridding kernel to use, and returns a value of that kernel

      x:: value to evaluate of kernel
      J:: support size of kernel
      kernel_name:: name used to choose kernel to evaluate
    */
      t_real output = 0;

      if (kernel_name == "gauss")
      {
        output = MeasurementOperator::gaussian(x, J);
      }
      else if (kernel_name == "kb")
      {
        output = MeasurementOperator::kaiser_bessel(x, J);
      }
      return output;
  }

  t_real MeasurementOperator::choose_ftkernel(const t_real x, const t_int J, const std::string kernel_name)
  {
    /*
      Chooses fourier transform of gridding kernel to use, and returns a value of that kernel

      x:: value to evaluate of kernel
      J:: support size of kernel
      kernel_name:: name used to choose kernel to evaluate
    */
      t_real output = 0;

      if (kernel_name == "gauss")
      {
        output = MeasurementOperator::ftgaussian(x, J);
      }
      else if (kernel_name == "kb")
      {
        output = MeasurementOperator::ftkaiser_bessel(x, J);
      }
      return output;
  }

  t_real MeasurementOperator::kaiser_bessel(const t_real x, const t_int J)
  {
      //need boost to create function
      return 0;
  }
  t_real MeasurementOperator::ftkaiser_bessel(const t_real x, const t_int J)
  {
      //need boost to create function
      return 0;
  }
  t_real MeasurementOperator::gaussian(const t_real x, const t_int J)
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

  t_real MeasurementOperator::ftgaussian(const t_real x, const t_int J)
  {
    /*
      Fourier transform of guassian gridding kernel

      x:: value to evaluate
      J:: support size of gridding kernel
    */
      t_real sigma = 0.31 * std::pow(J, 0.52);
      t_real a = x * sigma * pi;
      return std::sqrt(pi / 2) / sigma * std::exp(-a * a * 2);
  }
}
