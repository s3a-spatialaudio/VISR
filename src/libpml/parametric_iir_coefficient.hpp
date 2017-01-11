/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_PARAMETRIC_IIR_COEFFICIENT_HPP_INCLUDED
#define VISR_PML_PARAMETRIC_IIR_COEFFICIENT_HPP_INCLUDED

#include <boost/property_tree/ptree_fwd.hpp>

#include <iosfwd>
#include <iostream>
#include <string>

namespace visr
{
namespace pml
{

class ParametricIirCoefficientBase
{
public:
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

private:
  class TypeIdTranslator;

  static TypeIdTranslator const & translator();
};

template< typename CoefficientType = float>
class ParametricIirCoefficient: public ParametricIirCoefficientBase
{
public:
  /**
   * Create an default object corresponding to a flat EQ.
   */
  ParametricIirCoefficient();

  ParametricIirCoefficient( ParametricIirCoefficient const & rhs );

  ParametricIirCoefficient( ParametricIirCoefficient && rhs );

  explicit ParametricIirCoefficient( Type typeId,
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
   * Create a ParametricIirCoefficient objects from JSON and XML representations.
   */
  //@{
  static ParametricIirCoefficient && fromJson( boost::property_tree::ptree const & tree );

  static ParametricIirCoefficient && fromJson( std::basic_istream<char> & stream );

  static ParametricIirCoefficient && fromJson( std::string const & str );

  static ParametricIirCoefficient && fromXml( boost::property_tree::ptree const & tree );

  static ParametricIirCoefficient && fromXml( std::basic_istream<char> & stream );
  //@}


  ParametricIirCoefficient & operator=( ParametricIirCoefficient const & rhs ) = default;

  void loadJson( boost::property_tree::ptree const & tree );

  void loadJson( std::basic_istream<char> & stream );

  void loadJson( std::string const & str );


  void loadXml( boost::property_tree::ptree const & tree );

  void loadXml( std::basic_istream<char> & stream );

  void loadXml( std::string const & str);


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

} // namespace pml
} // namespace visr

#endif // VISR_PML_PARAMETRIC_IIR_COEFFICIENT_HPP_INCLUDED
