/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PANNINGDSP_PANNING_MATRIX_PARAMETER_HPP_INCLUDED
#define VISR_PANNINGDSP_PANNING_MATRIX_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"

#include <libefl/basic_matrix.hpp>

#include <libpml/matrix_parameter_config.hpp>

#include <libvisr/constants.hpp>
#include <libvisr/parameter_type.hpp>
#include <libvisr/typed_parameter_base.hpp>

#include <complex>
#include <initializer_list>
#include <istream>
#include <limits>

namespace visr
{
namespace panningdsp
{

static constexpr const ParameterType sPanningMatrixParameterType{
  detail::compileTimeHashFNV1("panningdsp::PanningMatrixParameter") };

/**
 * A type for passing matrixes between processing components.
 * The template class is explicitly instantiated for the element types float and double.
 */
class VISR_PANNINGDSP_LIBRARY_SYMBOL
  PanningMatrixParameter:
   public TypedParameterBase<PanningMatrixParameter, pml::MatrixParameterConfig,
     sPanningMatrixParameterType >
{
public:
  using DataType = ::visr::SampleType;

  /**
   * Type for representing time stamps.
   * @todo replace by global time representation (once it's merged into the mainline VISR)
   */
  using TimeType = std::size_t;

  /**
   * Type to represent the length of an gain interpolation.
   */
  using InterpolationIntervalType = TimeType;

  using TimeStampVector = visr::efl::AlignedArray< TimeType >;

  using InterpolationIntervalVector = visr::efl::AlignedArray< InterpolationIntervalType >;

  using GainMatrixType = visr::efl::BasicMatrix< DataType >;

  static constexpr std::size_t cTimeStampInfinity{ std::numeric_limits<TimeType>::max() };

  /**
   * Default constructor, creates an empty matrix of dimension 0 x 0.
   * @param alignment The alignment of the data, given in in multiples of the element size.
   */
  explicit PanningMatrixParameter( std::size_t numberObjects, std::size_t numberOfLoudspeakers,
    std::size_t alignment = 0 );

  explicit PanningMatrixParameter(visr::efl::BasicMatrix<DataType> const & gains,
    visr::efl::AlignedArray<TimeType> const & timeStamps,
    visr::efl::AlignedArray<TimeType> const & interpolationIntervals);

  explicit PanningMatrixParameter(visr::efl::BasicMatrix<DataType> const & gains,
    std::initializer_list<TimeType> const & timeStamps,
    std::initializer_list<TimeType> const & interpolationIntervals );

  explicit PanningMatrixParameter(
    std::initializer_list < std::initializer_list <DataType > > const & gains,
    std::initializer_list<TimeType> const & timeStamps,
    std::initializer_list<TimeType> const & interpolationIntervals,
    std::size_t alignment = 0 );

  explicit PanningMatrixParameter( ParameterConfigBase const & config );

  explicit PanningMatrixParameter( pml::MatrixParameterConfig const & config );

  /**
   * Copy constructor.
   * Basically needed to enable the 'named constructor' functions below.
   * @param rhs The object to be copied.
   */
  PanningMatrixParameter( PanningMatrixParameter const & rhs );

  virtual ~PanningMatrixParameter() override;

  PanningMatrixParameter& operator=(PanningMatrixParameter const & rhs);

  std::size_t numberOfObjects() const; 
  
  std::size_t numberOfLoudspeakers() const;

  /**
   */
  std::size_t alignmentElements() const;

  GainMatrixType const & gains() const;

  GainMatrixType & gains();

  TimeStampVector const & timeStamps() const;

  TimeStampVector & timeStamps();

  InterpolationIntervalVector const & interpolationIntervals() const;

  InterpolationIntervalVector & interpolationIntervals();

private:
  GainMatrixType mGains;

  TimeStampVector mTimeStamps;

  InterpolationIntervalVector mInterpolationIntervals;
};

} // namespace panningdsp
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::panningdsp::PanningMatrixParameter, detail::compileTimeHashFNV1("panningdsp::PanningMatrixParameter"), visr::pml::MatrixParameterConfig )

#endif // VISR_PANNINGDSP_PANNING_MATRIX_PARAMETER_HPP_INCLUDED
