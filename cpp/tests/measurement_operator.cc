#include "catch.hpp"
#include "MeasurementOperator.h"
#include <iostream>

using namespace purify;

TEST_CASE("Measurement Operator [Gridding]", "[Gridding]") {

  /*
    Tests each kernel by gridding visibility data and a point spread function, then comparing the gridded images to the same images generated from matlab.

    So far the maximum difference between the gridded images of at166B are 0.00015% (kb), 0.1071% (pswf), and 0.0362% (guass).

    Since there is small difference between the end results, it suggests that the C++ and Matlab codes perform the same process.
    But, it is not clear why the pswf and guass kernels are not as close to the matlab versions as the kb kernel.

  */
  MeasurementOperator op;
  MeasurementOperator::vis_params uv_vis;
  t_real max;
  t_real max_diff;
  t_int over_sample;
  t_real cellsize;
  std::string kernel;
  MeasurementOperator::operator_params st;
  

  //Gridding example
  over_sample = 2;
  t_int J = 6;
  uv_vis = op.read_visibility("../data/vla/at166B.3C129.c0.vis"); // visibility data being read in
  cellsize = 0.3;
  uv_vis = op.set_cell_size(uv_vis, cellsize); // scale uv coordinates to correct pixel size and to units of 2pi
  uv_vis = op.uv_scale(uv_vis, 1024 * over_sample, 1024 * over_sample); // scale uv coordinates to units of Fourier grid size
  uv_vis = op.uv_symmetry(uv_vis); // Enforce condjugate symmetry by reflecting measurements in uv coordinates
  Vector<t_complex> point_source = uv_vis.vis * 0; point_source.setOnes();  // Creating model visibilities for point source
  Image<t_complex> psf;
  //Amount in which images are shifted relative to each other
  t_int j_shift = - 0;
  t_int i_shift = - 1;

  SECTION("Kaiser Bessel Gridding") {
    kernel = "kb";
    st = op.init_nufft2d(uv_vis.u, uv_vis.v, J, J, kernel, 1024, 1024, over_sample); // Generating gridding matrix

    psf = op.grid(point_source, st);
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "kb_psf.fits", true, false);


    Image<t_real> kb_img = op.grid(uv_vis.vis, st).real();
    max = kb_img.maxCoeff();
    kb_img = kb_img / max;
    op.writefits2d(kb_img.real(), "grid_image_real_kb_6.fits", true, false);


    Image<t_real> kb_test_image = op.readfits2d("../data/expected/gridding/at166BtestJ6kb.fits").real();
    
    max_diff = 0;
    for (t_int i = 2; i < kb_test_image.cols() - 2; ++i)
    {
      for (t_int j = 2; j < kb_test_image.rows() - 2; ++j)
      {
        if (std::abs(kb_img(j, i) - kb_test_image(j + j_shift, i + i_shift)) > max_diff)
        {
          max_diff = std::abs(kb_img(j, i) - kb_test_image(j + j_shift, i + i_shift));
        }
        if (std::abs(kb_img(j, i) - kb_test_image(j + j_shift, i + i_shift)) >= 0.03)
        {
          std::cout << i << " " << j << '\n';
        }
        CHECK( std::abs(kb_img(j, i) - kb_test_image(j + j_shift, i + i_shift)) < 0.005 );
      }
    }
    std::cout << "Percentage max difference in Kaiser Bessel gridding: " << max_diff * 100 << "%" << '\n';
  }
  SECTION("Prolate Spheroidal Wave Functon Gridding") {
    kernel = "pswf";
    st = op.init_nufft2d(uv_vis.u, uv_vis.v, J, J, kernel, 1024, 1024, over_sample); // Generating gridding matrix
    Image<t_real> pswf_img = op.grid(uv_vis.vis, st).real();
    max = pswf_img.maxCoeff();
    pswf_img = pswf_img / max;

    Image<t_real> pswf_test_image = op.readfits2d("../data/expected/gridding/at166BtestJ6pswf.fits").real();
    Image<t_real> pswf_difference = pswf_img - pswf_test_image;
    max_diff = pswf_difference.abs().maxCoeff();
    
    max_diff = 0;
    for (t_int i = 1; i < pswf_test_image.cols() - 1; ++i)
    {
      for (t_int j = 1; j < pswf_test_image.rows() - 1; ++j)
      {
        if (std::abs(pswf_img(j, i) - pswf_test_image(j + j_shift, i + i_shift)) > max_diff)
        {
          max_diff = std::abs(pswf_img(j, i) - pswf_test_image(j + j_shift, i + i_shift));
        }
        if (std::abs(pswf_img(j, i) - pswf_test_image(j + j_shift, i + i_shift)) >= 0.03)
        {
          std::cout << i << " " << j << '\n';
        }
        CHECK( std::abs(pswf_img(j, i) - pswf_test_image(j + j_shift, i + i_shift)) < 0.005 );
      }
    }
    std::cout << "Percentage max difference in Prolate Spheroidal Wave Functon gridding: " << max_diff * 100 << "%" << '\n';
  }
  SECTION("Gaussian Gridding") {
    kernel = "gauss";
    st = op.init_nufft2d(uv_vis.u, uv_vis.v, J, J, kernel, 1024, 1024, over_sample); // Generating gridding matrix
    Image<t_real> gauss_img = op.grid(uv_vis.vis, st).real();
    max = gauss_img.maxCoeff();
    gauss_img = gauss_img / max;

    Image<t_real> gauss_test_image = op.readfits2d("../data/expected/gridding/at166BtestJ6gauss.fits").real();
    Image<t_real> gauss_difference = gauss_img - gauss_test_image;
    max_diff = gauss_difference.abs().maxCoeff();
    
    max_diff = 0;
    for (t_int i = 1; i < gauss_test_image.cols() - 1; ++i)
    {
      for (t_int j = 1; j < gauss_test_image.rows() - 1; ++j)
      {
        if (std::abs(gauss_img(j, i) - gauss_test_image(j + j_shift, i + i_shift)) > max_diff)
        {
          max_diff = std::abs(gauss_img(j, i) - gauss_test_image(j + j_shift, i + i_shift));
        }
        if (std::abs(gauss_img(j, i) - gauss_test_image(j + j_shift, i + i_shift)) >= 0.03)
        {
          std::cout << i << " " << j << '\n';
        }
        CHECK( std::abs(gauss_img(j, i) - gauss_test_image(j + j_shift, i + i_shift)) < 0.005 );
      }
    }
    std::cout << "Percentage max difference in Gaussian gridding: " << max_diff * 100 << "%" << '\n';
  }  

}

TEST_CASE("Measurement Operator [Degridding]", "[Degridding]") {
  /*
    Degrids a model point source, and then compares how close the degridded visibilities are to that of a point source.

    Also generates a degridded image of M31.
  
    We find the maximum difference in visibilities is 0.001% (kb), 0.842% (pswf), and 0.056% (gauss).

    Again, the kb Kernel gives closer to the expected result. PSWF gives a result that is worse than expected.
  */
  MeasurementOperator op;
  MeasurementOperator::vis_params uv_vis;
  t_real max;
  t_real max_diff;
  t_int over_sample;
  t_real cellsize;
  std::string kernel;
  MeasurementOperator::operator_params st;
  Image<t_complex> img;
  Image<t_complex> point;
  Vector<t_complex> psf_vis;
  Image<t_complex> psf;

  SECTION("Kaiser Bessel Degridding") {
    kernel = "kb";
    over_sample = 2;
    t_int J = 6;
    uv_vis = op.read_visibility("../data/expected/degridding/M31_J6kb.vis");
    img = op.readfits2d("../data/images/M31.fits");
    uv_vis = op.uv_scale(uv_vis, img.cols() * over_sample, img.rows() * over_sample);
    st = op.init_nufft2d(uv_vis.u, -uv_vis.v, J, J, kernel, img.cols(), img.rows(), over_sample); // Calculates gridding matrix

    point = Image<t_complex>::Zero(img.cols(), img.rows()); point(floor(img.cols()/2), floor(img.rows()/2)) = 1; // Creating model of point source in image
    psf_vis = op.degrid(point, st); // Creating visibilities from model
    max = psf_vis.cwiseAbs().maxCoeff();
    psf_vis = psf_vis / max; // normalized visibilities
    psf = op.grid(psf_vis, st); // gridding model visibilities into image
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "kb_psf_M31_degridding.fits", true, false); // saving image of degridded point source


    Vector<t_complex> kb_vis = op.degrid(img, st); // visibilities of degridded model
    Image<t_complex> kb_img = op.grid(kb_vis, st); // image of degridded model
    max = kb_img.real().maxCoeff();
    kb_img = kb_img / max; // normalisation of degridded model

    op.writefits2d(kb_img.real(), "kb_test.fits", true, false);

    max = kb_vis.cwiseAbs().maxCoeff();
    kb_vis = kb_vis / max;
    max_diff = 0;

    for (int i = 0; i < psf_vis.size(); ++i)
    {
      if (std::abs(std::abs(psf_vis(i)) - 1) > max_diff)
      {
        max_diff = std::abs(std::abs(psf_vis(i)) - 1);
      }
      // checking visibilities of degridded point source
      CHECK( std::abs(std::abs(psf_vis(i)) - 1) < 0.0001);
    }
    std::cout << "Percentage max difference in Kaiser Bessel degridding: " << max_diff * 100 << "%" << '\n';
    psf_vis.setOnes();
    psf = op.grid(psf_vis, st);
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "kb_psf_M31_gridding.fits", true, false); // saving image of degridded point source
  }

  SECTION("Prolate Spheroidal Wave Functon Degridding") {
    kernel = "pswf";
    over_sample = 2;
    t_int J = 6;
    uv_vis = op.read_visibility("../data/expected/degridding/M31_J6kb.vis");
    img = op.readfits2d("../data/images/M31.fits");
    uv_vis = op.uv_scale(uv_vis, img.cols() * over_sample, img.rows() * over_sample);
    st = op.init_nufft2d(uv_vis.u, -uv_vis.v, J, J, kernel, img.cols(), img.rows(), over_sample); // Calculates gridding matrix

    point = Image<t_complex>::Zero(img.cols(), img.rows()); point(floor(img.cols()/2), floor(img.rows()/2)) = 1; // Creating model of point source in image
    psf_vis = op.degrid(point, st); // Creating visibilities from model
    max = psf_vis.cwiseAbs().maxCoeff();
    psf_vis = psf_vis / max; // normalized visibilities
    psf = op.grid(psf_vis, st); // gridding model visibilities into image
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "pswf_psf_M31_degridding.fits", true, false); // saving image of degridded point source


    Vector<t_complex> pswf_vis = op.degrid(img, st); // visibilities of degridded model
    Image<t_complex> pswf_img = op.grid(pswf_vis, st); // image of degridded model
    max = pswf_img.real().maxCoeff();
    pswf_img = pswf_img / max; // normalisation of degridded model

    op.writefits2d(pswf_img.real(), "pswf_test.fits", true, false);

    max = pswf_vis.cwiseAbs().maxCoeff();
    pswf_vis = pswf_vis / max;
    max_diff = 0;

    for (int i = 0; i < psf_vis.size(); ++i)
    {
      if (std::abs(std::abs(psf_vis(i)) - 1) > max_diff)
      {
        max_diff = std::abs(std::abs(psf_vis(i)) - 1);
      }
      // checking visibilities of degridded point source
      CHECK( std::abs(std::abs(psf_vis(i)) - 1) < 0.01);
    }
    std::cout << "Percentage max difference in Prolate Spheroidal Wave Functon degridding: " << max_diff * 100 << "%" << '\n';
    psf_vis.setOnes();
    psf = op.grid(psf_vis, st);
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "pswf_psf_M31_gridding.fits", true, false); // saving image of degridded point source
  }

  SECTION("Gaussian Degridding") {
    kernel = "gauss";
    over_sample = 2;
    t_int J = 6;
    uv_vis = op.read_visibility("../data/expected/degridding/M31_J6kb.vis");
    img = op.readfits2d("../data/images/M31.fits");
    uv_vis = op.uv_scale(uv_vis, img.cols() * over_sample, img.rows() * over_sample);
    st = op.init_nufft2d(uv_vis.u, -uv_vis.v, J, J, kernel, img.cols(), img.rows(), over_sample); // Calculates gridding matrix

    point = Image<t_complex>::Zero(img.cols(), img.rows()); point(floor(img.cols()/2), floor(img.rows()/2)) = 1; // Creating model of point source in image
    psf_vis = op.degrid(point, st); // Creating visibilities from model
    max = psf_vis.cwiseAbs().maxCoeff();
    psf_vis = psf_vis / max; // normalized visibilities
    psf = op.grid(psf_vis, st); // gridding model visibilities into image
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "gauss_psf_M31_degridding.fits", true, false); // saving image of degridded point source


    Vector<t_complex> gauss_vis = op.degrid(img, st); // visibilities of degridded model
    Image<t_complex> gauss_img = op.grid(gauss_vis, st); // image of degridded model
    max = gauss_img.real().maxCoeff();
    gauss_img = gauss_img / max; // normalisation of degridded model

    op.writefits2d(gauss_img.real(), "gauss_test.fits", true, false);

    max = gauss_vis.cwiseAbs().maxCoeff();
    gauss_vis = gauss_vis / max;
    max_diff = 0;

    for (int i = 0; i < psf_vis.size(); ++i)
    {
      if (std::abs(std::abs(psf_vis(i)) - 1) > max_diff)
      {
        max_diff = std::abs(std::abs(psf_vis(i)) - 1);
      }
      // checking visibilities of degridded point source
      CHECK( std::abs(std::abs(psf_vis(i)) - 1) < 0.01);
    }
    std::cout << "Percentage max difference in Gaussian degridding: " << max_diff * 100 << "%" << '\n';
    psf_vis.setOnes();
    psf = op.grid(psf_vis, st);
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "gauss_psf_M31_gridding.fits", true, false); // saving image of degridded point source
  }
}