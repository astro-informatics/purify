#ifndef PURIFY_MEASUREMENT_OPERATOR_H
#define PURIFY_MEASUREMENT_OPERATOR_H

#include "types.h"

namespace purify {

  //! This does something
  class MeasurementOperator {
    public:
      //! This creates something
      MeasurementOperator() : something_() {}

      //! This gets something
      Vector<> const & something() const { return something_; }
      //! Can chain a bunch of setters together
      MeasurementOperator& something(Vector<> const &b) { something_ = b; return *this; }

      //! Does the measurement?
      Vector<> operator()(t_real _a) const;

    protected:
      Vector<> something_;
  };
}

#endif
