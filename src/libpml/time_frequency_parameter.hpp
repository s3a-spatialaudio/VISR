/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_TIME_FREQUENCY_PARAMETER_HPP_INCLUDED
#define VISR_PML_TIME_FREQUENCY_PARAMETER_HPP_INCLUDED

#include "time_frequency_parameter_config.hpp"

#include <libefl/basic_matrix.hpp>

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <complex>
#include <initializer_list>
#include <istream>

namespace visr
{
namespace pml
{

namespace // unnamed
{
  /**
   * Type trait to assign a unique ParameterType value to each concrete TimeFrequencyParameter type.
   */
  template<typename ElementType> struct TimeFrequencyParameterType {};

  template<> struct TimeFrequencyParameterType<float>
  {
    static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "TimeFrequencyParameterFloat" ); }
  };
  template<> struct TimeFrequencyParameterType<double>
  {
    static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "TimeFrequencyParameterDouble" ); }
  };
} // unnamed

/**
 * A type for passing matrixes between processing components.
 * The template class is explicitly instantiated for the element types float and double.
 * @tparam ElementType The data type of the elements of the matrix.
 */
template<typename ElementType >
class TimeFrequencyParameter: public TypedParameterBase<TimeFrequencyParameter<ElementType>, TimeFrequencyParameterConfig, TimeFrequencyParameterType<ElementType>::ptype() >
{
public:
  /**
   * Default constructor, creates an empty matrix of dimension 0 x 0.
   * @param alignment The alignment of the data, given in in multiples of the eleement size.
   */
  TimeFrequencyParameter( std::size_t alignment = 0 );

  /**
   * Construct a time-frequency parameter from a generic ParameterConfigBase object.
   * @param config The configuration object, must have the dynamic type TimeFrequencyParameterConfig.
   * @throw std::invalid_argument if the dynamic type of \p config does not match TimeFrequencyParameterConfig.
   */
  explicit TimeFrequencyParameter(ParameterConfigBase const & config);

  /**
   * Construct a tie-frequency parameter from a dedicated TimeFrequencyParameterConfig config object.
   * @param config The configuration object.
   */
  explicit TimeFrequencyParameter(TimeFrequencyParameterConfig const & config);

  /**
   * Construct a time-frequency parameter.
   * @param dftSize The size of the DFT transform.
   * @param numDftSamples The number of DFT vectors containe in one parameter
   * @param numChannels The number of audio channels that are simultaneously transmitted 
   * in a time-frequency parameter.
   * @param alignment The alignment of the data, given in in multiples of the element size.
   */
  explicit TimeFrequencyParameter( std::size_t dftSize,
                                   std::size_t numDftSamples, 
                                   std::size_t numChannels, 
                                   size_t alignment = 0 );

  /**
   * Copy constructor.
   * Basically needed to enable the 'named constructor' functions below.
   * @param rhs The object to be copied.
   */
  TimeFrequencyParameter( TimeFrequencyParameter<ElementType> const & rhs );

  TimeFrequencyParameter& operator=( TimeFrequencyParameter<ElementType> const & rhs );

  /**
   * Change the matrix dimension.
   * The content of the matrix is not kept, but reset to zeros.
   * @param dftSize Size of the DFT.
   * @param numDftSamples The number of DFT coefficient sets in a single TimeFrequencyParameter
   * @param numChannels Number of parallel audio channels conted in one parameter.
   * @throw std::bad_alloc If the creation of the new matrix fails.
   */
  void resize( std::size_t dftSize, std::size_t numDftSamples, std::size_t numChannels );

  std::size_t alignment() const { return mData.alignmentElements(); }

  std::size_t dftSize() const { return mData.numberOfColumns(); }

  std::size_t dftSamples() const { return mData.numberOfRows() / mNumberOfChannels; }

  std::size_t channelStride() const { return mData.stride(); }

  std::size_t dftSampleStride() const { return mData.stride() * numberOfChannels(); }

  std::size_t numberOfChannels() const { return mNumberOfChannels; }

  std::complex< ElementType > const *  dftSlice( std::size_t channelIdx, std::size_t dftSampleIdx ) const
  {
    return mData.row( dftSampleIdx * numberOfChannels() + channelIdx );
  }

  std::complex< ElementType > *  dftSlice( std::size_t channelIdx, std::size_t dftSampleIdx )
  {
    return mData.row( dftSampleIdx * numberOfChannels() + channelIdx );
  }
private:
  efl::BasicMatrix< std::complex< ElementType > > mData;

  std::size_t mNumberOfChannels;
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::TimeFrequencyParameter<float>, visr::pml::TimeFrequencyParameter<float>::staticType(), visr::pml::TimeFrequencyParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::TimeFrequencyParameter<double>, visr::pml::TimeFrequencyParameter<double>::staticType(), visr::pml::TimeFrequencyParameterConfig )

#endif // VISR_PML_TIME_FREQUENCY_PARAMETER_HPP_INCLUDED
