#include "purify/MeasurementOperator_mpi.h"

namespace purify {
  namespace mpi {

    void MeasurementOperator::create_mask() {

      std::set<t_int> M_local_set;
      // parallel sparse matrix multiplication with a sparse vector.
// #pragma omp parallel for
      for(t_int k = 0; k < G.outerSize(); ++k)
        for(Sparse<t_complex>::InnerIterator it(G, k); it; ++it) {
          M_local_set.insert(it.col());
        }
      M_local.resize(M_local_set.size());
      std::copy(M_local_set.begin(), M_local_set.end(), M_local.data());
      sizes = comm.gather<t_int>(M_local.size());
      M = comm.gather(M_local, sizes);
    }

    Vector<t_complex> MeasurementOperator::degrid(const Image<t_complex> &eigen_image) const {
      // Performs degridding with mpi
      if (M_local.size() == 0)
        throw std::runtime_error("Have not set and distirbuted M_local before using measurement operator.");
      Vector<t_complex> ft_vector;
      if (comm.is_root()) {
        Matrix<t_complex> ft_grid(ftsizev_, ftsizeu_);
        Matrix<t_complex> padded_image = Matrix<t_complex>::Zero(floor(imsizey_ * oversample_factor_),
            floor(imsizex_ * oversample_factor_));
        t_int x_start = std::max(0, static_cast<t_int>(std::floor(imsizex_ * (oversample_factor_ - 1) * 0.5)));
        t_int y_start = std::max(0, static_cast<t_int>(std::floor(imsizey_ * (oversample_factor_ - 1) * 0.5)));

        // zero padding and gridding correction
        assert(y_start + imsizey_ <= padded_image.rows());
        assert(x_start + imsizex_ <= padded_image.cols());
        assert(eigen_image.rows() == imsizey_);
        assert(eigen_image.cols() == imsizex_);
        padded_image.block(y_start, x_start, imsizey_, imsizex_)
          = utilities::parallel_multiply_image(S, eigen_image);
        ft_grid = utilities::re_sample_ft_grid(fftoperator_.forward(padded_image),
            resample_factor);
        ft_grid.resize(ftsizeu_ * ftsizev_, 1); // using conservativeResize does not work, it garbles
        std::cout << " AFTER FFT " << ft_grid.col(0).head(6).transpose() << std::endl;
        //Distirbute fft grid
        ft_vector = Vector<t_complex>::Zero(M.size());
        for (t_int i = 0; i < M.size(); i++)
          ft_vector(i) = ft_grid(M(i));
        ft_vector = comm.scatterv(ft_vector, sizes);
      } else {
        ft_vector = comm.scatterv<t_complex>(static_cast<t_int>(M_local.size()));
      }
      Vector<t_complex> ft_sparse = Vector<t_complex>::Zero(ftsizeu_ * ftsizev_);
      for (t_int i = 0; i < ft_vector.size(); i++)
        ft_sparse(M_local(i)) = ft_vector(i);
      std::cout << "!! ! " << ft_sparse.head(6).transpose() << std::endl;

      Vector<t_complex> const result = utilities::sparse_multiply_matrix(G, ft_sparse).array() * W / norm;
      std::cout << "AFTER DERGRID " << result.head(6).transpose() << std::endl;

      return result;
    }

    Image<t_complex> MeasurementOperator::grid(const Vector<t_complex> &visibilities) const {
      if (M_local.size() == 0)
        throw std::runtime_error("Have not set and distirbuted M_local before using measurement operator.");
      Matrix<t_complex> ft_grid_local = utilities::sparse_multiply_matrix(
          G.adjoint(), (visibilities.array() * W.conjugate()).matrix())
        / norm;
      //Collect non zero parts of grid for gathering by root
      Vector<t_complex> ft_vector_local = Vector<t_complex>::Zero(M_local.size());
      for (int i = 0; i < ft_vector_local.size(); i++)
        ft_vector_local(i) = ft_grid_local(M_local(i));

      Vector<t_complex> ft_vector = comm.gather(ft_vector_local, sizes);
      Image<t_complex> image_out = Image<t_complex>::Zero(imsizey_, imsizex_);
      if (comm.is_root()) {
        Matrix<t_complex> ft_grid = Vector<t_complex>::Zero(ftsizev_ * ftsizeu_);
        for (t_int i = 0; i < ft_vector.size(); i++)
          ft_grid(M(i)) += ft_vector(i);
        ft_grid.resize(ftsizev_, ftsizeu_);
        ft_grid = utilities::re_sample_ft_grid(ft_grid, 1. / resample_factor);
        Image<t_complex> padded_image = fftoperator_.inverse(ft_grid); // the fftshift is not needed because of the phase shift in the gridding kernel
        t_int x_start = floor(floor(imsizex_ * oversample_factor_) * 0.5 - imsizex_ * 0.5);
        t_int y_start = floor(floor(imsizey_ * oversample_factor_) * 0.5 - imsizey_ * 0.5);
        image_out = utilities::parallel_multiply_image(
            S, padded_image.block(y_start, x_start, imsizey_, imsizex_));
      }
      image_out = comm.broadcast(image_out);
      //std::cout << image_out.col(6).head(6) << std::endl;
      return image_out;
    }

    sopt::LinearTransform<sopt::Vector<sopt::t_complex>>
      linear_transform(MeasurementOperator const &measurements, t_uint nvis) {
        auto const height = measurements.imsizey();
        auto const width = measurements.imsizex();
        auto direct = [&measurements, width, height](Vector<t_complex> &out, Vector<t_complex> const &x) {
          assert(x.size() == width * height);
          auto const image = Image<t_complex>::Map(x.data(), height, width);
          out = measurements.degrid(image);
        };
        auto adjoint
          = [&measurements, width, height](Vector<t_complex> &out, Vector<t_complex> const &x) {
            auto image = Image<t_complex>::Map(out.data(), height, width);
            image = measurements.grid(x);
            //comm.all_sum_all(out);
          };
        return sopt::linear_transform<Vector<t_complex>>(direct, {{0, 1, static_cast<t_int>(nvis)}},
            adjoint,
            {{0, 1, static_cast<t_int>(width * height)}});
      }

  }

}
