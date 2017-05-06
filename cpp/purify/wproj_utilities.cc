#include "purify/wproj_utilities.h"

#define EIGEN_DONT_PARALLELIZE
#define EIGEN_NO_AUTOMATIC_RESIZING
namespace purify {
namespace wproj_utilities {

const t_real pi = constant::pi;

Matrix<t_complex> generate_chirp(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                 const t_int &x_size, const t_int &y_size) {

  // return chirp image fourier transform for w component
  // w:: w-term in units of lambda
  // celly:: size of y pixel in arcseconds
  // cellx:: size of x pixel in arcseconds
  // x_size:: number of pixels along x-axis
  // y_size:: number of pixels along y-axis

  const t_real theta_FoV_L = cell_x * x_size;
  const t_real theta_FoV_M = cell_y * y_size;

  const t_real L = 2 * std::sin(pi / 180. * theta_FoV_L / (60. * 60.) * 0.5);
  const t_real M = 2 * std::sin(pi / 180. * theta_FoV_M / (60. * 60.) * 0.5);

  const t_real delt_x = 1 * L / x_size;
  const t_real delt_y = 1 * M / y_size;
  Image<t_complex> chirp(y_size, x_size);
  t_complex I(0, 1);
  t_real nz = ((t_real)y_size * x_size * 1.0);

#pragma omp parallel for collapse(2)
  for(t_int l = 0; l < x_size; ++l) {
    for(t_int m = 0; m < y_size; ++m) {
      t_real x = (l + 0.5 - x_size * 0.5) * delt_x;
      t_real y = (m + 0.5 - y_size * 0.5) * delt_y;
      t_complex val = (std::exp(1 * pi * I * w_rate * (x * x + y * y)))
                      * std::exp(-2 * pi * I * (l * 0.5 + m * 0.5)) / nz;
      if(std::abs(val) > 1e-16) {
        if(std::abs(val.imag()) < 1e-10)
          val = val.real() + I * 0.0;
        chirp(l, m) = val;
      } else {
        chirp(l, m) = 0.0;
      }
    }
  }

  return chirp;
}

Sparse<t_complex> create_chirp_row(const t_real &w_rate, const t_real &cell_x, const t_real &cell_y,
                                   const t_real &ftsizev, const t_real &ftsizeu,
                                   const t_real &energy_fraction,
                                   const std::shared_ptr<FFTOperator> &fftop_) {

  const t_int Npix = ftsizeu * ftsizev;
  auto chirp_image = wproj_utilities::generate_chirp(w_rate, cell_x, cell_y, ftsizeu, ftsizev);
  Matrix<t_complex> rowC;
#pragma omp critical(fft)
  rowC = fftop_->forward(chirp_image);
  rowC.resize(1, Npix);
  t_real thres = sparsify_row_dense_thres(rowC, energy_fraction);
  typedef Eigen::Triplet<t_complex> T;
  std::vector<T> tripletList;
  tripletList.reserve(Npix);
  t_int sp = 0;
  for(t_int kk; kk < Npix; kk++) {
    if(std::abs(rowC(0, kk)) > thres) {
      sp++;
      tripletList.push_back(T(0, kk, rowC(0, kk)));
    }
  }
  Sparse<t_complex> chirp_row(1, Npix);
  chirp_row.setFromTriplets(tripletList.begin(), tripletList.end());
  // PURIFY_HIGH_LOG("Chirp kernels: energy  SP [{}] thres{} \n",sp,thres);
  assert(chirp_row.nonZeros() > 0);
  return chirp_row;
}

Sparse<t_complex> row_wise_convolution(Eigen::SparseVector<t_complex> &Grid_,
                                       Sparse<t_complex> &chirp_, const t_int &Nx,
                                       const t_int &Ny) {

  // if (chirp_.nonZeros() ==1)
  //    return Grid_.transpose();

  typedef Eigen::Triplet<t_complex> T;
  std::vector<T> tripletList;

  const t_int row_sz = 4 * (Grid_.nonZeros() + chirp_.nonZeros());
  tripletList.reserve(floor(row_sz));

  t_int Nx2 = Nx / 2;
  t_int Ny2 = Ny / 2;
#pragma omp parallel for
  for(t_int k = 0; k < Nx * Ny; ++k) {
    Vector<t_int> indexes = utilities::ind2sub(k, Nx, Ny);
    t_int i = indexes(0);
    t_int j = indexes(1);

    Eigen::SparseVector<t_complex> Chirp = chirp_.row(0);
    Eigen::SparseVector<t_complex> Grid = Grid_;
    t_complex temp(0.0, 0.0);

    for(Eigen::SparseVector<t_complex>::InnerIterator pix(Grid); pix; ++pix) {
      Vector<t_int> image_row_col = utilities::ind2sub(pix.index(), Nx, Ny);
      t_int ii = image_row_col(0);
      t_int jj = image_row_col(1);

      t_int oldpixi = ii - i;
      if(ii < Nx2)
        oldpixi += Nx; // fftshift
      if((oldpixi >= 0 and oldpixi < Nx)) {
        t_int oldpixj = jj - j;
        if(jj < Ny2)
          oldpixj += Ny; // fftshift
        if((oldpixj >= 0 and oldpixj < Ny)) {
          t_int pos = oldpixi * Ny + oldpixj;
          t_complex chirp_val = Chirp.coeffRef(pos);
          if(std::abs(chirp_val) > 1e-15)
            temp += pix.value() * chirp_val;
        }
      }
    }

    if(std::abs(temp) > 1e-16) {
      t_int iii, jjj;
      if(i < Nx2)
        iii = i + Nx2;
      else {
        iii = i - Nx2;
      }
      if(j < Ny2)
        jjj = j + Ny2;
      else {
        jjj = j - Ny2;
      }
      t_int pos = utilities::sub2ind(iii, jjj, Nx, Ny);
#pragma omp critical(load1)
      tripletList.push_back(T(0, pos, temp));
    }
  }
  Sparse<t_complex> output_row(1, Nx * Ny);
  output_row.setFromTriplets(tripletList.begin(), tripletList.end());
  return output_row;
}

Sparse<t_complex>
wprojection_matrix(const Sparse<t_complex> &Grid, const t_int &Nx, const t_int &Ny,
                   const Vector<t_real> &w_components, const t_real &cell_x, const t_real &cell_y,
                   const t_real &energy_fraction_chirp, const t_real &energy_fraction_wproj) {

  typedef Eigen::Triplet<t_complex> T;
  std::vector<T> tripletList;
  const t_int Npix = Nx * Ny;
  const t_int Nvis = w_components.size();

  PURIFY_HIGH_LOG("Hard-thresholding of the Chirp kernels: energy [{}] ", energy_fraction_chirp);
  PURIFY_HIGH_LOG("Hard-thresholding of the rows of G: energy [{}]  ", energy_fraction_wproj);

  const t_int fft_flag = (FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
  std::shared_ptr<FFTOperator> fftop_
      = std::make_shared<FFTOperator>(purify::FFTOperator().fftw_flag(fft_flag));
  tripletList.reserve(floor(Npix * Nvis * 0.2));
#pragma omp parallel for
  for(t_int m = 0; m < Grid.outerSize(); ++m) {
    PURIFY_DEBUG("CURRENT WPROJ - Kernel index [{}]", m);
    Sparse<t_complex> chirp(1, Npix);
    chirp
        = create_chirp_row(w_components(m), cell_x, cell_y, Nx, Ny, energy_fraction_chirp, fftop_);
    Eigen::SparseVector<t_complex> G_bis = Grid.row(m);

    Sparse<t_complex> kernel(1, Npix);
    kernel = row_wise_convolution(G_bis, chirp, Nx, Ny);

    Sparse<t_real> absRow(1, Npix);
    absRow = kernel.cwiseAbs();
    const t_real thres = sparsify_row_thres(absRow, energy_fraction_wproj);
    for(Sparse<t_complex>::InnerIterator itr(kernel, 0); itr; ++itr) {
      if(std::abs(itr.value()) > thres) {
#pragma omp critical(load1)
        tripletList.push_back(T(m, itr.index(), itr.value()));
      }
    }
    PURIFY_DEBUG("DONE - Kernel index [{}]", m);
  }
  Sparse<t_complex> Gmat(Nvis, Npix);
  Gmat.setFromTriplets(tripletList.begin(), tripletList.end());
  PURIFY_DEBUG("\nBuilding the rows of G.. DONE!\n");
  return Gmat;
}

t_real sparsify_row_thres(const Sparse<t_real> &row, const t_real &energy) {
  /*
     Takes in a row of G and returns indexes of coeff to keep in the row sparse version
     energy:: how much energy - in l2 sens - to keep after hard-thresholding
     */
  t_real thres = 1e-16;
  if(energy < 1) {
    const t_real abs_row_total_energy = (row.cwiseProduct(row)).sum();
    t_real tau = 0.5;
    t_real old_tau = 1;
    t_int niters = 200;
    t_real min_tau = 0.0;
    t_real max_tau = 1;
    t_real abs_row_max = 0;
    for(Sparse<t_real>::InnerIterator itr(row, 0); itr; ++itr) {
      if(itr.value() > abs_row_max)
        abs_row_max = itr.value();
    }
    /* calculating threshold  */
    t_real energy_sum = 0;
    t_real tau__ = 0;
    bool converge_ = false;
    for(t_int i = 0; i < niters; ++i) {
      energy_sum = 0;
      tau__ = tau * abs_row_max;
      for(Sparse<t_real>::InnerIterator itr(row, 0); itr; ++itr) {
        if(itr.value() > tau__)
          energy_sum += itr.value() * itr.value() / abs_row_total_energy;
      }
      if((std::abs(tau - old_tau) / std::abs(old_tau) < 1e-3)
         and ((energy_sum >= energy) and (((energy_sum / energy) - 1) < 0.01))) {
        converge_ = true;
        break;
      }
      old_tau = tau;
      if(energy_sum > energy)
        min_tau = tau;
      else {
        max_tau = tau;
      }
      tau = (max_tau + min_tau) * 0.5;
    }
    if(!converge_) {
      tau = min_tau;
    }
    thres = tau * abs_row_max;
  }
  return thres;
}

t_real sparsify_row_dense_thres(const Matrix<t_complex> &row, const t_real &energy) {
  /*
     Takes in a row of G and returns indexes of coeff to keep in the row sparse version
     energy:: how much energy - in l2 sens - to keep after hard-thresholding
     */

  t_real thres = 1e-16;
  if(energy < 1) {
    Sparse<t_real> row_abs(row.rows(), row.cols());
    const t_real abs_row_max = row.cwiseAbs().maxCoeff();
    row_abs = (row.cwiseAbs()).sparseView(1e-16);

    const t_real abs_row_total_energy = (row_abs.cwiseProduct(row_abs)).sum();
    t_real tau = 0.5;
    t_real old_tau = 1;
    t_int niters = 200;
    t_real min_tau = 0.0;
    t_real max_tau = 1;

    /* calculating threshold  */
    t_real energy_sum = 0;
    t_real tau__ = 0;
    bool converge_ = false;
    for(t_int i = 0; i < niters; ++i) {
      energy_sum = 0;
      tau__ = tau * abs_row_max;
      for(Sparse<t_real>::InnerIterator itr(row_abs, 0); itr; ++itr) {
        if(itr.value() > tau__)
          energy_sum += itr.value() * itr.value() / abs_row_total_energy;
      }

      if((std::abs(tau - old_tau) / std::abs(old_tau) < 1e-3)
         and ((energy_sum >= energy) and ((energy_sum / energy - 1) < 0.01))) {
        converge_ = true;
        break;
      }
      old_tau = tau;
      if(energy_sum > energy)
        min_tau = tau;
      else {
        max_tau = tau;
      }
      tau = (max_tau + min_tau) * 0.5;
    }
    if(!converge_) {
      tau = min_tau;
    }
    thres = tau * abs_row_max;
  }
  return thres;
}

t_real sparsity_sp(const Sparse<t_complex> &Gmat) {

  const t_int Nvis = Gmat.rows();
  const t_int Npix = Gmat.cols();
  t_real val = Gmat.nonZeros() / (t_real)(Nvis * Npix);

  PURIFY_DEBUG(" Sparsity perc:  {}", val);
  return val;
}
t_real sparsity_im(const Image<t_complex> &Cmat) {
  /*
     returs  nber on non zero elts/ total nber of elts in Image
     */
  const t_int Nvis = Cmat.rows();
  const t_int Npix = Cmat.cols();
  t_real sparsity = 0;
  for(t_int m = 0; m < Nvis; m++) {
    for(t_int n = 0; n < Npix; n++) {
      if(std::abs(Cmat(m, n)) > 1e-16)
        sparsity++;
    }
  }
  t_real val = (sparsity / (t_real)(Nvis * Npix));
  PURIFY_DEBUG(" Sparsity perc:  {}", val);
  return val;
}
t_real snr_metric(const Image<t_real> &model, const Image<t_real> &solution) {
  /*
     Returns SNR of the estimated model image
     */
  t_real nm = model.matrix().norm();
  t_real ndiff = (model - solution).matrix().norm();
  t_real val = 20 * std::log10(nm / ndiff);
  return val;
}
t_real mr_metric(const Image<t_real> &model, const Image<t_real> &solution) {
  /*
     Returns SNR of the estimated model image
     */
  t_int Npix = model.rows() * model.cols();
  Image<t_real> model_ = model.array() + 1e-10;
  Image<t_real> solution_ = solution.array() + 1e-10;
  Image<t_real> model_sol = model_.matrix().cwiseQuotient(solution_.matrix());
  Image<t_real> sol_model = solution_.matrix().cwiseQuotient(model_.matrix());
  Image<t_real> min_ratio = sol_model.matrix().cwiseMin(model_sol.matrix());
  t_real val = min_ratio.array().sum() / Npix;
  return val;
}

Sparse<t_complex>
generate_vect(const t_int &x_size, const t_int &y_size, const t_real &sigma, const t_real &mean) {

  // t_real sigma = 3;
  t_int W = x_size;
  Matrix<t_complex> chirp = Image<t_complex>::Zero(y_size, x_size);
  // t_real mean = 0;
  t_complex I(0, 1);
  t_real step = W / 2;

  // t_complex sum = 0.0; // For accumulating the kernel values
  for(int x = 0; x < W; ++x) {
    for(int y = 0; y < W; ++y) {
      t_int xx = x - step;
      xx = xx * xx;
      t_int yy = y - step;
      yy = yy * yy;
      t_complex val
          = exp(-0.5
                * (xx / (sigma * sigma)
                   + yy / (sigma * sigma))); //* std::exp(- 2 * pi * I * (x * 0.5 + y * 0.5));
      if(std::abs(val) > 1e-5)
        chirp(x, y) = val;
      else
        chirp(x, y) = 0.0;
    }
  }

  chirp.resize(1, x_size * y_size);
  Sparse<t_complex> chirp_ = chirp.sparseView(1e-5, 1);
  return chirp_;
}

t_real upsample_ratio_sim(const utilities::vis_params &uv_vis, const t_real &L, const t_real &M,
                          const t_int &x_size, const t_int &y_size, const t_int &multipleOf) {
  /*
     returns the upsampling (in Fourier domain) ratio
     */
  const Vector<t_real> &u = uv_vis.u.cwiseAbs();
  const Vector<t_real> &v = uv_vis.v.cwiseAbs();
  const Vector<t_real> &w = uv_vis.w.cwiseAbs();
  Vector<t_real> uvdist = (u.array() * u.array() + v.array() * v.array()).sqrt();
  t_real bandwidth = 2 * uvdist.maxCoeff();
  // std::cout<<"\nDEBUG: bandwidth "<<bandwidth;
  Vector<t_real> bandwidth_up_vector = 2 * (uvdist + w * L * 0.5);
  t_real bandwidth_up = bandwidth_up_vector.maxCoeff();
  t_real ratio = bandwidth_up / bandwidth;
  // std::cout<<"\nDEBUG: bandwidthUP "<<bandwidth_up;

  // std::cout<<"DEBUG:Initially calculated Upsampling ratio:"<<ratio <<"\n";

  // sets up dimensions for new image - even size
  t_int new_x = floor(x_size * ratio) + utilities::mod(floor(x_size * ratio), 2);
  t_int new_y = floor(y_size * ratio) + utilities::mod(floor(y_size * ratio), 2);
  if(utilities::mod(new_x, multipleOf) != 0)
    new_x = multipleOf * floor((t_real)new_x / multipleOf) + multipleOf;
  if(utilities::mod(new_y, multipleOf) != 0)
    new_y = multipleOf * floor((t_real)new_y / multipleOf) + multipleOf;
  // if (mod(new_y,multipleOf) !=0) std::cout <<"\nDEBUG: (in upsample_ratio_sim) ERROR!!!!!!  ---
  // IMAGE SIZE y\n "; if (mod(new_x,multipleOf) !=0) std::cout <<"\nDEBUG: (in upsample_ratio_sim)
  // ERROR!!!!!!  --- IMAGE SIZE x\n ";

  // !!!!!!! assuming same ratio on x and y for now !!!!!!!
  ratio = ((t_real)new_x) / ((t_real)x_size);

  // std::cout<<"DEBUG: (in upsample_ratio_sim) new image size"<< new_x<<" old image size "<<
  // x_size<<"\n";

  return ratio;
}
} // namespace wproj_utilities
} // namespace purify
