#include "MeasurementOperator.h"

namespace purify {

  Vector<> MeasurementOperator::operator()(t_real _a) const {
    return _a * something();
  }

}
