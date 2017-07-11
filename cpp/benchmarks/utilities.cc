#include <benchmarks/utilities.h>

namespace b_utilities {

  void Arguments(benchmark::internal::Benchmark* b) {
    int uv_size_max = 256; // 4096
    int im_size_max = 1000; // 1M, 10M, 100M
    int kernel_max = 4; // 16
    for (int i=128; i<=uv_size_max; i*=2)
      for (int j=1000; j<=im_size_max; j*=10)
        for (int k=2; k<=kernel_max; k*=2)
          b->Args({i,j,k});
  }

}
