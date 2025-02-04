#ifndef SETUP_UTILS_H
#define SETUP_UTILS_H

#include "purify/types.h"
#include "purify/logging.h"
#include "purify/measurement_operator_factory.h"
#include "purify/pfitsio.h"
#include "purify/read_measurements.h"
#include "purify/wavelet_operator_factory.h"
#include "purify/yaml-parser.h"
#include <sopt/differentiable_func.h>
#include <sopt/non_differentiable_func.h>

using namespace purify;

struct waveletInfo {
  std::shared_ptr<const sopt::LinearTransform<Eigen::VectorXcd>> transform;
  t_uint sara_size;
};

waveletInfo createWaveletOperator(YamlParser &params,
                                  const factory::distributed_wavelet_operator &wop_algo);

struct OperatorsInfo {
  factory::distributed_measurement_operator mop_algo;
  factory::distributed_wavelet_operator wop_algo;
  bool using_mpi;
};

OperatorsInfo selectOperators(YamlParser &params);

struct inputData {
  utilities::vis_params uv_data;
  t_real sigma;
  Vector<t_complex> measurement_op_eigen_vector;
  std::vector<t_int> image_index;
  std::vector<t_real> w_stacks;
};

inputData getInputData(const YamlParser &params,
                       const factory::distributed_measurement_operator mop_algo,
                       const factory::distributed_wavelet_operator wop_algo, const bool using_mpi);

struct measurementOpInfo {
  std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> measurement_transform;
  t_real operator_norm;
};

measurementOpInfo createMeasurementOperator(
    const YamlParser &params, const factory::distributed_measurement_operator mop_algo,
    const factory::distributed_wavelet_operator wop_algo, const bool using_mpi,
    const std::vector<t_int> &image_index, const std::vector<t_real> &w_stacks,
    const utilities::vis_params &uv_data, Vector<t_complex> &measurement_op_eigen_vector);

void setupCostFunctions(const YamlParser &params, std::unique_ptr<DifferentiableFunc<t_complex>> &f,
                        std::unique_ptr<NonDifferentiableFunc<t_complex>> &g, t_real sigma,
                        sopt::LinearTransform<Vector<t_complex>> &Phi);

void initOutDirectoryWithConfig(YamlParser &params);

struct Headers {
  pfitsio::header_params solution_header;
  pfitsio::header_params residuals_header;
  pfitsio::header_params def_header;
};

Headers genHeaders(const YamlParser &params, const utilities::vis_params &uv_data);

void saveMeasurementEigenVector(const YamlParser &params,
                                const Vector<t_complex> &measurement_op_eigen_vector);

void savePSF(
    const YamlParser &params, const pfitsio::header_params &def_header,
    const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> &measurements_transform,
    const utilities::vis_params &uv_data, const t_real flux_scale, const t_real sigma,
    const t_real operator_norm, const t_real beam_units);

void saveDirtyImage(
    const YamlParser &params, const pfitsio::header_params &def_header,
    const std::shared_ptr<sopt::LinearTransform<Vector<t_complex>>> &measurements_transform,
    const utilities::vis_params &uv_data, const t_real beam_units);

#endif