/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PANNINGDSP_PANNING_MATRIX_PARAMETER_HPP_INCLUDED
#define VISR_PANNINGDSP_PANNING_MATRIX_PARAMETER_HPP_INCLUDED

#include "export_symbols.hpp"
#include "types.hpp"

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
  /**
   * Default constructor, creates a zero-initialised tranisitions structure.
   * @param numberOfObjects The number of audio objects processed in the panning gain mattrix.
   * @param numberOfLoudspeakers Numer of loudspeaker gains per object.
   * @param alignment The alignment of the data, given in in multiples of the element size.
   */
  explicit PanningMatrixParameter( std::size_t numberOfObjects, std::size_t numberOfLoudspeakers,
    std::size_t alignment = 0 );

  explicit PanningMatrixParameter(visr::efl::BasicMatrix<SampleType> const & gains,
    visr::efl::AlignedArray<TimeType> const & timeStamps,
    visr::efl::AlignedArray<TimeType> const & transitionTimes );

  explicit PanningMatrixParameter(visr::efl::BasicMatrix<SampleType> const & gains,
    std::initializer_list<TimeType> const & timeStamps,
    std::initializer_list<TimeType> const & transitionTimes );

  explicit PanningMatrixParameter(
    std::initializer_list < std::initializer_list <SampleType > > const & gains,
    std::initializer_list<TimeType> const & timeStamps,
    std::initializer_list<TimeType> const & transitionTimes,
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
   * Return the alignment of the gain matrix.
   */
  std::size_t alignmentElements() const;

  /**
   * Return the gain mtrix, const version.
   * Dimension numberOfObjects x numberOfLoudpeakers.
   */
  GainMatrixType const & gains() const;

  /**
   * Return the gain mtrix, nonconst version.
   * Dimension numberOfObjects x numberOfLoudpeakers.
   */
  GainMatrixType & gains();

  /**
   * Return the start time stamps of the transitions, const version
   * Dimension: numberOfLoudpeakers
   * A value of cTimeStampInfinity denotes that there is no transition
   * for th object at this index.
   */
  TimeVector const & timeStamps() const;

  /**
   * Return the start time stamps of the transitions.
   * Dimension: numberOfLoudpeakers
   * A value of cTimeStampInfinity denotes that there is no transition
   * for th object at this index.
   */
  TimeVector & timeStamps();

  TimeVector const & transitionTimes() const;

  TimeVector & transitionTimes();

private:
  GainMatrixType mGains;

  TimeVector mTimeStamps;

  TimeVector mTransitionTimes;
};

} // namespace panningdsp
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::panningdsp::PanningMatrixParameter, detail::compileTimeHashFNV1("panningdsp::PanningMatrixParameter"), visr::pml::MatrixParameterConfig )

#endif // VISR_PANNINGDSP_PANNING_MATRIX_PARAMETER_HPP_INCLUDED
