/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "panning_gain_matrix.hpp"

#include <libefl/vector_functions.hpp>

#include <libpml/matrix_parameter.hpp>

#include <ciso646>

namespace visr
{
namespace panningdsp
{

PanningGainMatrix::PanningGainMatrix( SignalFlowContext const & context,
  char const * name,
  CompositeComponent * parent,
  std::size_t numberOfObjects,
  std::size_t numberOfLoudspeakers,
  visr::efl::BasicMatrix<SampleType> const & initialGains )
 : AtomicComponent( context, name, parent )
 , mAudioInput( "in", *this, numberOfObjects )
 , mAudioOutput( "out", *this, numberOfLoudspeakers )
{
}

PanningGainMatrix::~PanningGainMatrix() = default;

void PanningGainMatrix::process()
{
  void processParameters();

  void processAudio();
}

void PanningGainMatrix::processParameters()
{

}

void PanningGainMatrix::processAudio()
{

}

} // namespace rcl
} // namespace visr
