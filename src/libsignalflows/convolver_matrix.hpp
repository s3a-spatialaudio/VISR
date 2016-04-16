/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_CONVOLVER_MATRIX_HPP_INCLUDED
#define VISR_SIGNALFLOWS_CONVOLVER_MATRIX_HPP_INCLUDED

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

class ConvolverMatrix: public ril::AudioSignalFlow
{
public:
  explicit ConvolverMatrix( std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       std::size_t filterLength,
                       std::size_t maxFilters,
                       std::size_t maxRoutings,
                       efl::BasicMatrix<ril::SampleType> const & initialFilters,
                       pml::FilterRoutingList const & initialRoutings,
                       std::size_t period,
                       ril::SamplingFrequencyType samplingFrequency );

  ~ConvolverMatrix();

  /*virtual*/ void process( );

private:
  rcl::FirFilterMatrix mConvolver;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_CONVOLVER_MATRIX_HPP_INCLUDED
