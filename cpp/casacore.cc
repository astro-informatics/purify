#include "casacore.h"
#include <casacore/casa/Arrays/IPosition.h>

namespace purify {
namespace casa {
 int MeasurementSet::spectral_window_id(int datadescid) const {
   auto const column = scalar_column<::casacore::Int>("SPECTRAL_WINDOW_ID", "DATA_DESCRIPTION");
   column.checkRowNumber(datadescid);
   return column(datadescid);
 }

 Array<t_real> MeasurementSet::frequencies(int datadescid) const {
   auto const column = array_column<::casacore::Double>("CHAN_FREQ", "SPECTRAL_WINDOW");
   auto const array = column.get(spectral_window_id(datadescid));
   if(array.ndim() != 1)
     throw std::runtime_error("CHAN_FREQ is not a 1d array");
   Array<t_real> result(array.shape()[0]);
   for(int i(0); i < result.size(); ++i)
     result(i) = static_cast<t_real>(array(::casacore::IPosition(1, i)));
   return result;
 }
}
}
