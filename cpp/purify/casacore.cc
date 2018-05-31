#include "purify/casacore.h"
#include "purify/config.h"
#include <sstream>
#include <casacore/casa/Arrays/IPosition.h>
#include <casacore/tables/TaQL/ExprNode.h>
#include "purify/logging.h"
#include "purify/types.h"

namespace purify {
namespace casa {
std::string const MeasurementSet::default_filter = "WHERE NOT ANY(FLAG)";
MeasurementSet &MeasurementSet::filename(std::string const &filename) {
  clear();
  filename_ = filename;
  return *this;
}

::casacore::Table const &MeasurementSet::table(std::string const &name) const {
  auto const tabname = name == "" ? filename() : filename() + "/" + name;
  auto i_result = tables_->find(tabname);
  if(i_result == tables_->end())
    i_result = tables_->emplace(tabname, ::casacore::Table(tabname)).first;

  return i_result->second;
}

std::size_t MeasurementSet::size() const {
  if(table().nrow() == 0)
    return 0;
  auto const column = array_column<::casacore::Double>("CHAN_FREQ", "SPECTRAL_WINDOW");
  auto const orig = column.shape(0);
  for(t_uint i(1); i < column.nrow(); ++i)
    if(orig != column.shape(i))
      throw std::runtime_error("Can only do rectangular measurement set for now");
  return orig(0);
}

MeasurementSet::const_iterator MeasurementSet::begin(std::string const &filter) const {
  return const_iterator(0, *this, filter);
}
MeasurementSet::const_iterator MeasurementSet::end(std::string const &filter) const {
  return const_iterator(size(), *this, filter);
}
MeasurementSet::ChannelWrapper MeasurementSet::operator[](t_uint i) const {
  return ChannelWrapper(i, *this, "");
}

MeasurementSet::ChannelWrapper MeasurementSet::
operator[](std::tuple<t_uint, std::string> const &i) const {
  if(std::get<0>(i) >= size())
    throw std::out_of_range("Not that many channels");
  return ChannelWrapper(std::get<0>(i), *this, std::get<1>(i));
}

std::string MeasurementSet::ChannelWrapper::filter() const {
  std::ostringstream sstr;
  sstr << "WHERE NOT any(FLAG[" << channel_ << ",])";
  if(not filter_.empty())
    sstr << "AND " << filter_;
  return sstr.str();
}
std::string MeasurementSet::ChannelWrapper::index(std::string const &variable) const {
  std::ostringstream sstr;
  sstr << variable << "[" << channel_ << ",]";
  return sstr.str();
}

Vector<t_real>
MeasurementSet::ChannelWrapper::joined_spectral_window(std::string const &column) const {
  auto const raw = raw_spectral_window(column);
  auto const ids = ms_.column<::casacore::Int>("DATA_DESC_ID", filter());
  auto const spids
      = table_column<::casacore::Int>(ms_.table("DATA_DESCRIPTION"), "SPECTRAL_WINDOW_ID");
  Vector<t_real> result(ids.size());
  for(Eigen::DenseIndex i(0); i < ids.size(); ++i) {
    assert(ids(i) < spids.size());
    assert(spids(ids(i)) < raw.size());
    result(i) = raw(spids(ids(i)));
  }
  return result;
}

bool MeasurementSet::ChannelWrapper::is_valid() const {
  std::ostringstream sstr;
  sstr << "USING STYLE PYTHON SELECT FLAG[" << channel_ << ",] as R FROM $1 WHERE NOT any(FLAG["
       << channel_ << ",])";
  if(not filter_.empty())
    sstr << "AND " << filter_;
  auto const taql_table = ::casacore::tableCommand(sstr.str(), ms_.table());
  return taql_table.table().nrow() > 0;
}

std::string
MeasurementSet::ChannelWrapper::stokes(std::string const &pol, std::string const &column) const {
  std::ostringstream sstr;
  sstr << "mscal.stokes(" << column << ", '" << pol << "')";
  return sstr.str();
}

Vector<t_real> MeasurementSet::ChannelWrapper::raw_spectral_window(std::string const &stuff) const {
  std::ostringstream sstr;
  sstr << stuff << "[" << channel_ << "]";
  return table_column<t_real>(ms_.table("SPECTRAL_WINDOW"), sstr.str());
}

MeasurementSet::Direction
MeasurementSet::direction(t_real tolerance, std::string const &filter) const {
  auto const field_ids_raw = column<::casacore::Int>("FIELD_ID", filter);
  auto const source_ids_raw = table_column<::casacore::Int>(table("FIELD"), "SOURCE_ID");
  std::set<::casacore::Int> source_ids;
  for(Eigen::DenseIndex i(0); i < field_ids_raw.size(); ++i) {
    assert(field_ids_raw(i) < source_ids_raw.size());
    source_ids.insert(source_ids_raw(field_ids_raw(i)));
  }
  if(source_ids.size() == 0 and source_ids_raw.size() > 0) {
    PURIFY_DEBUG("Could not find sources. Try different filter, no matching data in channel. "
                 "Currently using filter: "
                 + filter);
    Vector<t_real> original(2);
    original(0) = 0.;
    original(1) = 0.;
    return original;
  } else if(source_ids_raw.size() == 0)
    throw std::runtime_error("Could not find sources. Cannot determine direction");
  auto const directions = table_column<::casacore::Double>(table("SOURCE"), "DIRECTION");
  auto const original = directions.row(*source_ids.begin());
  for(auto const other : source_ids)
    if(not directions.row(other).isApprox(original, tolerance))
      throw std::runtime_error("Found more than one direction");
  return original;
}

MeasurementSet::const_iterator &MeasurementSet::const_iterator::operator++() {
  ++channel_;
  wrapper_ = std::make_shared<value_type>(channel_, ms_, filter_);
  return *this;
}

MeasurementSet::const_iterator MeasurementSet::const_iterator::operator++(int) {
  operator++();
  return const_iterator(channel_ - 1, ms_, filter_);
}

bool MeasurementSet::const_iterator::operator==(const_iterator const &c) const {
  if(not same_measurement_set(c))
    throw std::runtime_error("Iterators are not over the same measurement set");
  return channel_ == c.channel_;
}

utilities::vis_params
read_measurementset(std::string const &filename,
                    const stokes polarization,
                    const std::vector<t_int> &channels_input, std::string const &filter) {
  auto const ms_file = purify::casa::MeasurementSet(filename);
  return read_measurementset(ms_file, polarization, channels_input, filter);
};
utilities::vis_params read_measurementset(std::string const &filename, const utilities::vis_params &uv1,
                                          const stokes pol,
                                          const std::vector<t_int> &channels,
                                          std::string const &filter){
  utilities::vis_params uv;
  const auto uv2 = read_measurementset(filename, pol, channels, filter);
  if(std::abs(uv1.ra - uv2.ra) > 1e-6)
    throw std::runtime_error(filename + ": wrong RA in pointing.");
  if(std::abs(uv1.dec - uv2.dec) > 1e-6)
    throw std::runtime_error(filename + ": wrong DEC in pointing.");
  uv.ra = uv1.ra;
  uv.dec = uv1.dec;
  uv.u = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.v = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.w = Vector<t_real>::Zero(uv1.size() + uv2.size());
  uv.vis = Vector<t_complex>::Zero(uv1.size() + uv2.size());
  uv.weights = Vector<t_complex>::Zero(uv1.size() + uv2.size());
  uv.u.segment(0, uv1.size()) = uv1.u;
  uv.v.segment(0, uv1.size()) = uv1.v;
  uv.w.segment(0, uv1.size()) = uv1.w;
  uv.vis.segment(0, uv1.size()) = uv1.vis;
  uv.weights.segment(0, uv1.size()) = uv1.weights;
  uv.u.segment(uv1.size(), uv2.size()) = uv2.u;
  uv.v.segment(uv1.size(), uv2.size()) = uv2.v;
  uv.w.segment(uv1.size(), uv2.size()) = uv2.w;
  uv.vis.segment(uv1.size(), uv2.size()) = uv2.vis;
  uv.weights.segment(uv1.size(), uv2.size()) = uv2.weights;
  return uv;

}
utilities::vis_params read_measurementset(std::vector<std::string> const &filename,
                                          const stokes pol,
                                          const std::vector<t_int> &channel,
                                          std::string const &filter){
   utilities::vis_params output = read_measurementset(filename.at(0), pol, channel, filter);
  if(filename.size() == 1)
    return output;
  for(int i = 1; i < filename.size(); i++)
    output = read_measurementset(filename.at(i), output, pol, channel, filter);
  return output;
 
}
utilities::vis_params
read_measurementset(MeasurementSet const &ms_file,
                    const stokes polarization,
                    const std::vector<t_int> &channels_input, std::string const &filter) {

  utilities::vis_params uv_data;
  t_uint rows = 0;
  std::vector<t_int> channels = channels_input;
  if(channels.empty()) {
    PURIFY_LOW_LOG("All Channels = {}", ms_file.size());
    Vector<t_int> temp_vector = Vector<t_int>::LinSpaced(ms_file.size(), 0, ms_file.size());
    if(temp_vector.size() == 1) // fixing unwanted behavior of LinSpaced when ms_file.size() = 1
      temp_vector(0) = 0;
    channels = std::vector<t_int>(temp_vector.data(), temp_vector.data() + temp_vector.size());
  }

  // counting number of rows
  for(auto channel_number : channels) {
    rows += ms_file[channel_number].size();
  }

  PURIFY_LOW_LOG("Visibilities = {}", rows);
  uv_data.u = Vector<t_real>::Zero(rows);
  uv_data.v = Vector<t_real>::Zero(rows);
  uv_data.w = Vector<t_real>::Zero(rows);
  uv_data.vis = Vector<t_complex>::Zero(rows);
  uv_data.weights = Vector<t_complex>::Zero(rows);
  uv_data.ra = ms_file[channels[0]].right_ascension(); // convert directions from radians to degrees
  uv_data.dec = ms_file[channels[0]].declination();
  // calculate average frequency
  uv_data.average_frequency = average_frequency(ms_file, filter, channels);

  // add data to channel
  t_uint row = 0;

  for(auto channel_number : channels) {
    PURIFY_DEBUG("Adding channel {} to plane...", channel_number);
    if(channel_number < ms_file.size()) {
      auto const channel = ms_file[channel_number];
      if(channel.size() > 0) {
        if(uv_data.ra != channel.right_ascension() or uv_data.dec != channel.declination())
          throw std::runtime_error("Channels contain multiple pointings.");
        Vector<t_real> const frequencies = channel.frequencies();
        uv_data.u.segment(row, channel.size()) = channel.lambda_u();
        uv_data.v.segment(row, channel.size()) = -channel.lambda_v();
        uv_data.w.segment(row, channel.size()) = channel.lambda_w();
        t_real const the_casa_factor = 2;
        switch(polarization) {
        case stokes::I:
          uv_data.vis.segment(row, channel.size()) = channel.I("DATA") * 0.5;
          uv_data.weights.segment(row, channel.size()).real()
              = channel.wI(MeasurementSet::ChannelWrapper::Sigma::OVERALL)
                * the_casa_factor; // go for sigma rather than sigma_spectrum
          break;
        case stokes::Q:
          uv_data.vis.segment(row, channel.size()) = channel.Q("DATA") * 0.5;
          uv_data.weights.segment(row, channel.size()).real()
              = channel.wQ(MeasurementSet::ChannelWrapper::Sigma::OVERALL)
                * the_casa_factor; // go for sigma rather than sigma_spectrum
          break;
        case stokes::U:
          uv_data.vis.segment(row, channel.size()) = channel.U("DATA") * 0.5;
          uv_data.weights.segment(row, channel.size()).real()
              = channel.wU(MeasurementSet::ChannelWrapper::Sigma::OVERALL)
                * the_casa_factor; // go for sigma rather than sigma_spectrum
          break;
        case stokes::V:
          uv_data.vis.segment(row, channel.size()) = channel.V("DATA") * 0.5;
          uv_data.weights.segment(row, channel.size()).real()
              = channel.wV(MeasurementSet::ChannelWrapper::Sigma::OVERALL)
                * the_casa_factor; // go for sigma rather than sigma_spectrum
          break;
        case stokes::LL:
          uv_data.vis.segment(row, channel.size()) = channel.LL("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wLL(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::LR:
          uv_data.vis.segment(row, channel.size()) = channel.LR("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wRL(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::RL:
          uv_data.vis.segment(row, channel.size()) = channel.RL("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wRL(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::RR:
          uv_data.vis.segment(row, channel.size()) = channel.RR("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wRR(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::XX:
          uv_data.vis.segment(row, channel.size()) = channel.XX("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wXX(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::XY:
          uv_data.vis.segment(row, channel.size()) = channel.XY("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wXY(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::YX:
          uv_data.vis.segment(row, channel.size()) = channel.YX("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wYX(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::YY:
          uv_data.vis.segment(row, channel.size()) = channel.YY("DATA");
          uv_data.weights.segment(row, channel.size()).real() = channel.wYY(
              MeasurementSet::ChannelWrapper::Sigma::OVERALL); // go for sigma rather than
          // sigma_spectrum
          break;
        case stokes::P:
          t_complex I(0., 1.);
          uv_data.vis.segment(row, channel.size()) = channel.Q("DATA") + I * channel.U("DATA");
          uv_data.weights.segment(row, channel.size()).real()
              = (channel.wQ(MeasurementSet::ChannelWrapper::Sigma::OVERALL).array()
                     * channel.wQ(MeasurementSet::ChannelWrapper::Sigma::OVERALL).array()
                 + channel.wU(MeasurementSet::ChannelWrapper::Sigma::OVERALL).array()
                       * channel.wU(MeasurementSet::ChannelWrapper::Sigma::OVERALL).array())
                    .sqrt(); // go for sigma rather than
          // sigma_spectrum
          break;
        }
        row += channel.size();
      }
    }
  }
  // make consistent with vis file format exported from casa
  uv_data.weights = 1. / uv_data.weights.array();
  uv_data.units = utilities::vis_units::lambda;
  return uv_data;
}

std::vector<utilities::vis_params>
read_measurementset_channels(std::string const &filename,
                             const stokes pol,
                             const t_int &channel_width, std::string const &filter) {
  // Read and average the channels into a vector of vis_params
  std::vector<utilities::vis_params> channels_vis;
  auto const ms_file = purify::casa::MeasurementSet(filename);
  t_int const total_channels = ms_file.size();
  t_int const planes = (channel_width == 0) ? 1 : std::floor(total_channels / channel_width);
  PURIFY_DEBUG("Number of planes {} ...", planes);
  for(int i = 0; i < planes; i++) {
    PURIFY_DEBUG("Reading plane {} ...", i);
    t_int const end = std::min((i + 1) * channel_width, total_channels);
    Vector<t_int> temp_block = Vector<t_int>::LinSpaced(channel_width, i * channel_width, end);
    if(channel_width == 1 or total_channels == i)
      temp_block(0) = i;
    auto const block = std::vector<t_int>(temp_block.data(), temp_block.data() + temp_block.size());
    channels_vis.push_back(read_measurementset(ms_file, pol, block, filter));
  }
  return channels_vis;
};

t_real average_frequency(const purify::casa::MeasurementSet &ms_file, std::string const &filter,
                         const std::vector<t_int> &channels) {

  // calculate average frequency
  t_real frequency_sum = 0;
  t_real width_sum = 0.;
  for(auto channel_number : channels) {
    auto const channel = ms_file[channel_number];
    auto const frequencies = channel.frequencies();
    auto const width = channel.width();
    frequency_sum += (frequencies.array() * width.array()).sum();
    width_sum += width.sum();
  }
  return frequency_sum / width_sum / 1e6;
}

t_uint MeasurementSet::ChannelWrapper::size() const {
  if(ms_.table().nrow() == 0)
    return 0;
  std::ostringstream sstr;
  sstr << "USING STYLE PYTHON SELECT FLAG[" << channel_ << ",] as R FROM $1 WHERE NOT any(FLAG["
       << channel_ << ",])";
  if(not filter_.empty())
    sstr << "AND " << filter_;
  auto const taql_table = ::casacore::tableCommand(sstr.str(), ms_.table());
  auto const vtable = taql_table.table();
  return vtable.nrow();
}

MeasurementSet::const_iterator &MeasurementSet::const_iterator::operator+=(t_int n) {
  channel_ += n;
  return *this;
}

bool MeasurementSet::const_iterator::operator>(const_iterator const &c) const {
  if(not same_measurement_set(c))
    throw std::runtime_error("Iterators are not over the same measurement set");
  return channel_ > c.channel_;
}

bool MeasurementSet::const_iterator::operator>=(const_iterator const &c) const {
  if(not same_measurement_set(c))
    throw std::runtime_error("Iterators are not over the same measurement set");
  return channel_ >= c.channel_;
}
} // namespace casa
} // namespace purify
