/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_PARAMETRIC_IIR_COEFFICIENTS_HPP_INCLUDED
#define VISR_PML_PARAMETRIC_IIR_COEFFICIENTS_HPP_INCLUDED

#include "matrix_parameter_config.hpp" // might be a forward declaration

#include <libril/typed_parameter_base.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <iosfwd>
#include <iostream>
#include <string>

namespace visr
{
namespace pml
{

class ParametricIirCoefficients
{
public:
  using CoefficientType = float;

  enum class Type
  {
    lowpass,
    highpass,
    bandpass,
    bandstop,
    highshelf,
    lowshelf,
    peak,
    notch,
    allpass,
  };

  static Type stringToTypeId( std::string const & name );
  
  static std::string const & typeIdToString( Type typeId );

  /**
   * Create an default object corresponding to a flat EQ.
   */
  ParametricIirCoefficients();

  ParametricIirCoefficients( ParametricIirCoefficients const & rhs ) = default;

  explicit ParametricIirCoefficients( Type typeId,
                                      CoefficientType centerFrequency,
                                      CoefficientType quality,
                                     CoefficientType gain = static_cast<CoefficientType>(0.0) );

  Type type() const { return mType; }

  CoefficientType frequency() const { return mFrequency; }

  CoefficientType quality() const {return mQuality; }

  CoefficientType gain() const { return mGain; }

  void setType( Type newType );

  void setFrequency( CoefficientType newFrequency );

  void setQuality( CoefficientType newQuality );

  void setGain( CoefficientType newGain );

  /**
   * Create a ParametricIirCoefficients objects from JSON and XML representations.
   */
  //@{
  static ParametricIirCoefficients fromJson( boost::property_tree::ptree const & tree );

  static ParametricIirCoefficients fromJson( std::basic_istream<char> & stream );

  static ParametricIirCoefficients fromXml( boost::property_tree::ptree const & tree );

  static ParametricIirCoefficients fromXml( std::basic_istream<char> & stream );
  //@}


  ParametricIirCoefficients & operator=( ParametricIirCoefficients const & rhs ) = default;

  void loadJson( boost::property_tree::ptree const & tree );

  void loadJson( std::basic_istream<char> & );

  void loadXml( boost::property_tree::ptree const & tree );

  void loadXml( std::basic_istream<char> & );

  /**
   * 
   */
  //@{
  void writeJson( boost::property_tree::ptree & tree ) const;

  void writeJson( std::basic_ostream<char> & stream ) const;

  void writeJson( std::string & str ) const;

  void writeXml( boost::property_tree::ptree & tree ) const;

  void writeXml ( std::basic_ostream<char> & stream ) const;

  void writeXml ( std::string & str ) const;

  //@}
private:
  class TypeIdTranslator;

  static TypeIdTranslator const & translator();

  Type mType;

  /**
   * Cutoff or center frequency normalised with respect  to sampling frequency
   */
  CoefficientType mFrequency;

  CoefficientType mQuality;

  /**
   * Gain setting, used only in shelving, peak and noth filters.
   * Linear scale.
   */
  CoefficientType mGain;
};

 // DEFINE_PARAMETER_TYPE( visr::pml::ParametricIirCoefficientsMatrix<float>, visr::ril::ParameterType::BiquadMatrixFloat, visr::pml::MatrixParameterConfig )
 // DEFINE_PARAMETER_TYPE( visr::pml::ParametricIirCoefficientsMatrix<double>, visr::ril::ParameterType::BiquadMatrixDouble, visr::pml::MatrixParameterConfig )


} // namespace pml
} // namespace visr

#endif // VISR_PML_PARAMETRIC_IIR_COEFFICIENTS_HPP_INCLUDED
