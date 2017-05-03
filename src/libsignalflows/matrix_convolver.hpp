/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_MATRIX_CONVOLVER_HPP_INCLUDED
#define VISR_SIGNALFLOWS_MATRIX_CONVOLVER_HPP_INCLUDED

#include <libril/audio_input.hpp>
#include <libril/audio_output.hpp>
#include <libril/composite_component.hpp>
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

class MatrixConvolver: public CompositeComponent
{
public:
  explicit MatrixConvolver( SignalFlowContext const & context,
                            char const * name,
                            CompositeComponent * parent,
                            std::size_t numberOfInputs,
                            std::size_t numberOfOutputs,
                            std::size_t filterLength,
                            std::size_t maxFilters,
                            std::size_t maxRoutings,
                            efl::BasicMatrix<SampleType> const & initialFilters,
                            pml::FilterRoutingList const & initialRoutings,
                            char const * fftImplementation );

  ~MatrixConvolver();

private:
  AudioInputT<SampleType> mAudioIn;
  AudioOutputT<SampleType> mAudioOut;

  rcl::FirFilterMatrix mConvolver;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_MATRIX_CONVOLVER_HPP_INCLUDED
