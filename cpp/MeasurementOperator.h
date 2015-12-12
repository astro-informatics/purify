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
      //! Converts from subscript to index for matrix.
      t_uint sub2ind(const t_uint row, const t_uint col, const t_uint cols, const t_uint rows);
      //! Converts from index to subscript for matrix.
      void ind2sub(const t_uint sub, const t_uint cols, const t_uint rows, t_uint* row, t_uint* col);

    protected:
      Vector<> something_;
  };
}

#endif
