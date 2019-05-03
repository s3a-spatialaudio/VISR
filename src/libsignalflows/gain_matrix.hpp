/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_SIGNALFLOWS_GAIN_MATRIX_HPP_INCLUDED
#define VISR_SIGNALFLOWS_GAIN_MATRIX_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libvisr/audio_input.hpp>
#include <libvisr/audio_output.hpp>
#include <libvisr/composite_component.hpp>
 
#include <librcl/gain_matrix.hpp>

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace signalflows
{

class VISR_SIGNALFLOWS_LIBRARY_SYMBOL GainMatrix: public CompositeComponent
{
public:
  explicit GainMatrix( SignalFlowContext const & context,
                       const char * name,
                       CompositeComponent * parent,
                       std::size_t numberOfInputs, 
                       std::size_t numberOfOutputs,
                       efl::BasicMatrix<SampleType> const & initialMatrix,
                       std::size_t interpolationPeriod );

  ~GainMatrix();

  /*virtual*/ void process( );

private:
  const std::size_t cNumberOfInputs;

  const std::size_t cNumberOfOutputs;

  const std::size_t cInterpolationSteps;

  rcl::GainMatrix mMatrix;

  AudioInput mInput;

  AudioOutput mOutput;
};

} // namespace signalflows
} // namespace visr

#endif // #ifndef VISR_SIGNALFLOWS_GAIN_MATRIX_HPP_INCLUDED
