/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_MATRIX_CONVOLVER_HPP_INCLUDED
#define VISR_SIGNALFLOWS_MATRIX_CONVOLVER_HPP_INCLUDED

#include <libril/audio_signal_flow.hpp>
#include <libril/constants.hpp>

#include <librcl/fir_filter_matrix.hpp>

#include <libefl/basic_matrix.hpp>

namespace visr
{
// Forward declarations
namespace pml
{
template< typename DataType > class BasicMatrix;
class FilterRoutingList;
}

namespace signalflows
{

class MatrixConvolver: public ril::AudioSignalFlow
{
public:
  explicit MatrixConvolver( std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       std::size_t filterLength,
                       std::size_t maxFilters,
                       std::size_t maxRoutings,
                       efl::BasicMatrix<ril::SampleType> const & initialFilters,
                       pml::FilterRoutingList const & initialRoutings,
                       char const * fftImplementation,
                       std::size_t period,
                       ril::SamplingFrequencyType samplingFrequency );

  ~MatrixConvolver();

  /*virtual*/ void process( );

private:
  rcl::FirFilterMatrix mConvolver;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_MATRIX_CONVOLVER_HPP_INCLUDED
