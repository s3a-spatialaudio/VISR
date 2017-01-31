/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_PARAMETRIC_IIR_COEFFICIENT_HPP_INCLUDED
#define VISR_PML_PARAMETRIC_IIR_COEFFICIENT_HPP_INCLUDED

#include <boost/property_tree/ptree_fwd.hpp>

#include <iosfwd>
#include <iostream>
#include <string>
#include <vector>

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
  static ParametricIirCoefficient fromJson( boost::property_tree::ptree const & tree );

  static ParametricIirCoefficient fromJson( std::basic_istream<char> & stream );

  static ParametricIirCoefficient fromJson( std::string const & str );

  static ParametricIirCoefficient fromXml( boost::property_tree::ptree const & tree );

  static ParametricIirCoefficient fromXml( std::basic_istream<char> & stream );

  static ParametricIirCoefficient fromXml( std::string const & str );
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

template< typename CoefficientType >
class ParametricIirCoefficientList
{
public:
  using Element = ParametricIirCoefficient< CoefficientType >;
  using Container = std::vector< Element >;
  using const_iterator = typename Container::const_iterator;
  using iterator = typename Container::iterator;

  /**
   * Default constructor, creates an empty list.
   */
  ParametricIirCoefficientList() = default;

  /**
   * Copy constructor (default)
   */
  ParametricIirCoefficientList( ParametricIirCoefficientList<CoefficientType> const &) = default;

  /**
  * Move constructor (default)
  */
  ParametricIirCoefficientList( ParametricIirCoefficientList<CoefficientType> && ) = default;

  ~ParametricIirCoefficientList() = default;

  /**
   * Assignment operator (default)
   */
  ParametricIirCoefficientList<CoefficientType> & operator=( ParametricIirCoefficientList<CoefficientType> const & ) = default;

  /**
   * Constructor, create a list of identical elements (or defult-constructed elements)
   */
  explicit ParametricIirCoefficientList( std::size_t numCoeffs,
                                         ParametricIirCoefficient<CoefficientType> const & initialValue )
    : mCoeffs( numCoeffs, initialValue )
  {
  }

  explicit ParametricIirCoefficientList( std::initializer_list< ParametricIirCoefficient< CoefficientType > > initList )
    : mCoeffs( initList )
  {
  }

  void loadJson( boost::property_tree::ptree const & tree );

  void loadJson( std::basic_istream<char> & stream );

  void loadJson( std::string const & str );
  
  void loadXml( boost::property_tree::ptree const & tree );

  void loadXml( std::basic_istream<char> & stream );

  void loadXml( std::string const & str );

  void writeJson( boost::property_tree::ptree & tree ) const;

  void writeJson( std::basic_ostream<char> & stream ) const;

  void writeJson( std::string & str ) const;

  void writeXml( boost::property_tree::ptree & tree ) const;

  void writeXml( std::basic_ostream<char> & stream ) const;

  void writeXml( std::string & str ) const;

  std::size_t size() const { return mCoeffs.size(); }

  void resize( std::size_t newSize )
  { 
    mCoeffs.resize( newSize );
    std::fill( begin(), end(), Element() );
  }

  bool empty() const { return mCoeffs.empty(); }

  Element const & operator[]( std::size_t index ) const { return mCoeffs[index]; }
  Element & operator[]( std::size_t index ) { return mCoeffs[index]; }

  Element const & at( std::size_t index ) const { return mCoeffs.at(index); }
  Element & at( std::size_t index ) { return mCoeffs.at( index ); }


  const_iterator begin() const { return mCoeffs.begin(); }
  const_iterator end() const { return mCoeffs.end(); }

  iterator begin() { return mCoeffs.begin(); }
  iterator end() { return mCoeffs.end(); }



private:
  Container mCoeffs;
};

} // namespace pml
} // namespace visr

#endif // VISR_PML_PARAMETRIC_IIR_COEFFICIENT_HPP_INCLUDED
