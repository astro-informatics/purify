#include "MeasurementOperator.h"

namespace purify {

  Vector<> MeasurementOperator::operator()(t_real _a) const {
    return _a * something();
  }
  t_uint MeasurementOperator::sub2ind(const t_uint row, const t_uint col, const t_uint cols, const t_uint rows) {
    return row * cols + col;
  }
  void MeasurementOperator::ind2sub(const t_uint sub, const t_uint cols, const t_uint rows, t_uint* row, t_uint* col) {
    *col = sub % cols;
    *row = (sub - *col) / cols;
  }
}
