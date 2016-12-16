/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "communication_area.hpp"

#include <libefl/alignment.hpp>
#include <libefl/vector_functions.hpp>

namespace visr
{
namespace rrl
{
  template<typename SampleType>
  CommunicationArea<SampleType>::CommunicationArea( std::size_t numberOfSignals, std::size_t signalLength,
                                                    std::size_t alignmentElements /* = 0 */ )
   : mNumberOfSignals( numberOfSignals )
   , mSignalLength( signalLength )
   , mSignalStride( efl::nextAlignedSize( signalLength, alignmentElements ) )
   , mStorage( numberOfSignals*mSignalStride, alignmentElements )
  {
#if 1
    // zero-initialize the content.
    efl::vectorFill( static_cast<SampleType>(0), mStorage.data(), 
                     numberOfSignals*mSignalStride, alignmentElements );
#endif
  }

  template< typename SampleType>
  CommunicationArea<SampleType>::~CommunicationArea()
  {
  }

  // Explicit instantiation of CommunicationArea for SampleTypes float and double
  template class CommunicationArea<float>;
  template class CommunicationArea<double>;

} // namespace rrl
} // namespace visr
