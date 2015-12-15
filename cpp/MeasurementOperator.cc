#include "MeasurementOperator.h"

namespace purify {

  t_int MeasurementOperator::sub2ind(const t_int row, const t_int col, const t_int rows, const t_int cols) {
    /*
      Converts (row, column) of a matrix to a single index. This does the same as the matlab funciton sub2ind, converts subscript to index.

      row:: row of matrix
      col:: column of matrix
      cols:: number of columns for matrix
      rows:: number of rows for matrix
     */
    return row * cols + col;
  }

  void MeasurementOperator::ind2sub(const t_int sub, const t_int cols, const t_int rows, t_int* row, t_int* col) {
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

  Vector<t_real> MeasurementOperator::omega_to_k(const Vector<t_real> omega) {
    /*
      Maps fourier coordinates (u or v) to integer grid coordinates.

      omega:: fourier coordinates for a signle axis (u or v axis)
    */
    return omega.unaryExpr(std::ptr_fun<double,double>(std::floor));     
  }

  void MeasurementOperator::writefits2d(Image<t_real> eigen_image, const std::string fits_name) 
  {
    /*
      Writes an image to a fits file.
      image:: image data, a 2d Image.
      fits_name:: string contating the file name of the fits file.
    */
    std::auto_ptr<CCfits::FITS> pFits(0);
    long naxes[2] = {static_cast<long>(eigen_image.rows()), static_cast<long>(eigen_image.cols())};
    pFits.reset(new CCfits::FITS(fits_name, DOUBLE_IMG, 2, naxes));
    long fpixel ( 1 );
    std::vector<long> extAx;
    extAx.push_back(2); 
    extAx.push_back(naxes[0]);
    extAx.push_back(naxes[1]);
    CCfits::ExtHDU* imageExt = pFits->addImage(fits_name, DOUBLE_IMG, extAx);
    eigen_image.resize(naxes[0]*naxes[1], 1);
    std::valarray<double> image(naxes[0]*naxes[1]);
    for (int i = 0; i < static_cast<int>(naxes[0]*naxes[1]); ++i)
    {
      image[i] = eigen_image(i);
    }
    imageExt->write(fpixel, naxes[0]*naxes[1], image);
  }
  
  Image<t_real> MeasurementOperator::readfits2d(std::string fits_name)
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
    Image<t_real> eigen_image(ax1, ax2);
    t_int index;
    for (t_int i = 0; i < ax1; ++i)
    {
      for (t_int j = 0; j < ax2; ++j)
      {
        index = MeasurementOperator::sub2ind(i, j, ax1, ax2);
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
  std::vector<t_tripletList> tripletList;
  tripletList.reserve(rows*Ju*Jv);
  for (int m = 0; m < rows; ++m)
    {
      for (int ju = 1; ju <= Ju; ++ju)
       {
         q = (t_int) std::fmod(k_u(m) + ju, ftsizeu);
        for (int jv = 1; jv <= Jv; ++jv)
          {
            p = (t_int) std::fmod(k_v(m) + jv, ftsizev);
            index = MeasurementOperator::sub2ind(q, p, ftsizeu, ftsizev);
            tripletList.push_back(t_tripletList(m, index, kernelu(u(m)-(k_u(m)+ju)) * kernelv(v(m)-(k_v(m)+jv))));
          }
        }
      }    
    Sparse<t_real> interpolation_matrix(rows, cols);
    interpolation_matrix.setFromTriplets(tripletList.begin(), tripletList.end());
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
        S(i ,j) = ftkernelu(i - ftsizeu * 0.5) * ftkernelv(j - ftsizev * 0.5);
      }
    }
    return S;

  }

  MeasurementOperator::operator_params MeasurementOperator::init_nufft2d(Vector<t_real> u, Vector<t_real> v, const t_int Ju, const t_int Jv, const t_int kernel, const t_int imsizex, const t_int imsizey, const t_int oversample_factor)
  {
    /*
      Generates tools/operators needed for gridding and degridding.

      u:: visibilities in units of 2*pi;

    */
    MeasurementOperator::operator_params st;
    st.imsizex = imsizex;
    st.imsizey = imsizey;
    st.ftsizeu = oversample_factor * imsizex;
    st.ftsizev = oversample_factor * imsizey;
    u = u / (2 * pi) * st.ftsizeu;
    v = v / (2 * pi) * st.ftsizev;
    auto kernelu = [&] (t_real x) { return 1; };
    auto kernelv = [&] (t_real x) { return 1; };
    auto ftkernelu = [&] (t_real x) { return 1; };
    auto ftkernelv = [&] (t_real x) { return 1; };
    switch(kernel){
      case 0:
      {

      }
      break;
      case 1:
      {
        auto kernelu = [&] (t_real x) { return MeasurementOperator::gaussian(x, Ju); };
        auto kernelv = [&] (t_real x) { return MeasurementOperator::gaussian(x, Jv); };
        auto ftkernelu = [&] (t_real x) { return MeasurementOperator::ftgaussian(x / st.ftsizeu, Ju); };
        auto ftkernelv = [&] (t_real x) { return MeasurementOperator::ftgaussian(x / st.ftsizev, Jv); };
      }
      break;
    }
    st.S = MeasurementOperator::MeasurementOperator::init_correction2d(ftkernelu, ftkernelv, st.ftsizeu, st.ftsizev);
    st.G = MeasurementOperator::init_interpolation_matrix2d(u, v, Ju, Jv, kernelu, kernelv, st.ftsizeu, st.ftsizev);
    return st;
  }

  t_real MeasurementOperator::kaiser_bessel(const t_real x, const t_int J)
  {
      return 0;
  }

  t_real MeasurementOperator::gaussian(const t_real x, const t_int J)
  {
      t_real sigma = 0.31 * std::pow(J, 0.52); // Optimal sigma according to fessler et al.
      t_real a = x / sigma;
      return std::exp(-a * a * 0.5);
  }
  
  t_real MeasurementOperator::ftgaussian(const t_real x, const t_int J)
  {
      t_real sigma = 0.31 * std::pow(J, 0.52);
      t_real a = x * sigma;
      return std::exp(-a * a * 2);
  }
}
