#include <sstream>
#include <fstream>
#include "purify/directories.h"
#include "purify/distribute.h"
#include "purify/mpi_utilities.h"
#include <benchmarks/utilities.h>

using namespace purify;

namespace b_utilities {

  void Arguments(benchmark::internal::Benchmark* b) {
    int uv_size_max = 4096; // 4096
    int im_size_max = 10000000; // 1M, 10M, 100M
    int kernel_max = 16; // 16
    for (int i=128; i<=uv_size_max; i*=2)
      for (int j=1000000; j<=im_size_max; j*=10)
        for (int k=2; k<=kernel_max; k*=2)
          if (k*k<i)
            b->Args({i,j,k});
  }


  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end){
    auto elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    return elapsed_seconds.count();
  }

  double duration(std::chrono::high_resolution_clock::time_point start, std::chrono::high_resolution_clock::time_point end,
                  sopt::mpi::Communicator const &comm) {
    auto elapsed_seconds = duration(start,end);
    // Now get the max time across all procs: the slowest processor is the one that is
    // holding back the others in the benchmark.
    return comm.all_reduce(elapsed_seconds, MPI_MAX);
  }


  utilities::vis_params random_measurements(t_int size) {
    std::stringstream filename;
    filename << "random_" << size << ".vis";
    std::string const vis_file = visibility_filename(filename.str());
    std::ifstream vis_file_str(vis_file);

    if (vis_file_str.good()) {
      return utilities::read_visibility(vis_file, false);
    }
    else {
      t_real const sigma_m = constant::pi / 3;
      const t_real max_w = 100.; // lambda
      auto uv_data = utilities::random_sample_density(size, 0, sigma_m, max_w);
      uv_data.units = "radians";
      utilities::write_visibility(uv_data, vis_file);
      return uv_data;
    }
  }

  utilities::vis_params random_measurements(t_int size, sopt::mpi::Communicator const &comm) {
    if(comm.is_root()) {
      // Generate random measurements
      auto uv_data = random_measurements(size);
      if(comm.size() == 1)
        return uv_data;
    
      // Distribute them
      auto const order
        = distribute::distribute_measurements(uv_data, comm, "distance_distribution");
      uv_data = utilities::regroup_and_scatter(uv_data, order, comm);
      return uv_data;
    }
    return utilities::scatter_visibilities(comm);
  }

}
