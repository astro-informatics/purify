#include "catch.hpp"
#include "MeasurementOperator.h"
#include <iostream>

using namespace purify;

TEST_CASE("Measurement Operator [Gridding]", "[Gridding]") {
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
  uv_vis = op.read_visibility("../data/vla/at166B.3C129.c0.vis");
  cellsize = 0.3;
  uv_vis = op.set_cell_size(uv_vis, cellsize);
  uv_vis = op.uv_scale(uv_vis, 1024 * over_sample, 1024 * over_sample);
  uv_vis = op.uv_symmetry(uv_vis);
  Vector<t_complex> point_source = uv_vis.vis * 0; point_source.setOnes();  
  Image<t_complex> psf;
  //Amount in which images are shifted relative to each other
  t_int j_shift = - 0;
  t_int i_shift = - 1;

  SECTION("Kaiser Bessel Gridding") {
    kernel = "kb";
    st = op.init_nufft2d(uv_vis.u, uv_vis.v, J, J, kernel, 1024, 1024, over_sample);

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
    st = op.init_nufft2d(uv_vis.u, uv_vis.v, J, J, kernel, 1024, 1024, over_sample);
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
  SECTION("Guassian Gridding") {
    kernel = "gauss";
    st = op.init_nufft2d(uv_vis.u, uv_vis.v, J, J, kernel, 1024, 1024, over_sample);
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
    std::cout << "Percentage max difference in Guassian gridding: " << max_diff * 100 << "%" << '\n';
  }  

}

TEST_CASE("Measurement Operator [Degridding]", "[Degridding]") {
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

  SECTION("Kaiser Bessel Gridding") {
    kernel = "kb";
    over_sample = 2;
    t_int J = 6;
    uv_vis = op.read_visibility("../data/expected/degridding/M31_J6kb.vis");
    img = op.readfits2d("../data/images/M31.fits");
    uv_vis = op.uv_scale(uv_vis, img.cols() * over_sample, img.rows() * over_sample);
    st = op.init_nufft2d(uv_vis.u, -uv_vis.v, J, J, kernel, img.cols(), img.rows(), over_sample);

    point = Image<t_complex>::Zero(img.cols(), img.rows()); point(floor(img.cols()/2), floor(img.rows()/2)) = 1;
    psf_vis = op.degrid(point, st);
    psf = op.grid(psf_vis, st);
    max = psf.real().maxCoeff();
    psf = psf / max;
    op.writefits2d(psf.real(), "kb_psf_M31_degridding.fits", true, false);

    Vector<t_complex> kb_vis = op.degrid(img, st);
    Image<t_complex> kb_img = op.grid(kb_vis, st);
    op.writefits2d(kb_img.real(), "kb_test.fits", true, false);
    max = kb_vis.cwiseAbs().maxCoeff();
    kb_vis = kb_vis / max;
    Vector<t_complex> kb_test_vis = uv_vis.vis;
    max = kb_test_vis.cwiseAbs().maxCoeff();
    kb_test_vis = kb_test_vis / max;
    Vector<t_complex> kb_vis_difference = kb_vis - kb_test_vis;
    max_diff = kb_vis_difference.cwiseAbs().maxCoeff();
    for (int i = 0; i < uv_vis.vis.size(); ++i)
    {
      //std::cout << kb_test_vis(i) << " " << kb_vis(i) << '\n';
    }
    for (t_int i = 0; i < kb_vis_difference.size(); ++i)
    {
      //CHECK( std::abs(kb_vis_difference(i)) < 0.5 );
    }
    std::cout << "Percentage max difference in Kaiser Bessel degridding: " << max_diff * 100 << "%" << '\n';
  }
}