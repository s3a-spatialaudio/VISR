/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_RIL_COMMUNICATION_AREA_HPP_INCLUDED
#define VISR_RIL_COMMUNICATION_AREA_HPP_INCLUDED

#include <libefl/aligned_array.hpp>

#include <valarray>

namespace visr
{
namespace ril
{

/**
 *
 */
template<typename SampleType>
class CommunicationArea
{
public:
  /**
   * Constructor.
   * @param numberOfSignals
   * @param signalLength
   * @param alignElements [optional]
   */
  explicit CommunicationArea( std::size_t numberOfSignals, std::size_t signalLength,
                              std::size_t alignmentElements /* = 0 */ );
  /** Destructor */
  ~CommunicationArea();

  std::size_t numberOfSignals() const { return mNumberOfSignals; }

  std::size_t signalLength() const { return mSignalLength; }

  std::size_t signalStride() const { return mSignalStride; }
private:
  const std::size_t mNumberOfSignals;
  const std::size_t mSignalLength;
  const std::size_t mSignalStride;
  efl::AlignedArray<SampleType> mStorage;

};

} // namespace ril
} // namespace visr

#endif // VISR_RIL_COMMUNICATION_AREA_HPP_INCLUDED