/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parametric_iir_coefficient.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <istream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

namespace visr
{
namespace pml
{

class ParametricIirCoefficientBase::TypeIdTranslator
{
private:
  using ForwardMap = std::map<Type, std::string>;
  using ReverseMap = std::map<std::string, Type >;

  ForwardMap mFwdMap;

  ReverseMap mReverseMap;

public:
  TypeIdTranslator();

  /**
   * @throw std::invalid_argument id id is not a valid typename.
   */
  Type idToType( const std::string & id ) const
  {
    ReverseMap::const_iterator findIt = mReverseMap.find( id );
    if( findIt == mReverseMap.end() )
    {
      throw std::invalid_argument( "Invalid IIR parameter string type" );
    }
    return findIt->second;
  }

  std::string const & typeToId( Type typeStr ) const
  {
    ForwardMap::const_iterator findIt = mFwdMap.find( typeStr );
    if( findIt == mFwdMap.end() )
    {
      // Should not happen because of enum class 
      throw std::invalid_argument( "Invalid IIR parameter type." );
    }
    return findIt->second;
  }
};

  ParametricIirCoefficientBase::TypeIdTranslator::TypeIdTranslator()
    : mFwdMap( { { Type::lowpass, "lowpass" },{ Type::highpass, "highpass" },
    { Type::bandpass, "bandpass" },{ Type::bandstop, "bandstop" },
    { Type::peak, "peak" },{ Type::notch, "notch" },
    { Type::lowshelf, "lowshelf" },{ Type::highshelf, "highshelf" },
    { Type::allpass, "allpass" } } )
  {
    for( ForwardMap::value_type const v : mFwdMap )
    {
      mReverseMap.insert( std::make_pair( v.second, v.first ) );
    }
    // Todo: Add abbreviations
  }

  /*static*/ ParametricIirCoefficientBase::TypeIdTranslator const &
    ParametricIirCoefficientBase::translator()
  {
    static const TypeIdTranslator sTranslator;
    return sTranslator;
  }

  /*static*/ std::string const &
    ParametricIirCoefficientBase::typeIdToString( ParametricIirCoefficientBase::Type typeId )
  {
    return translator().typeToId( typeId );
  }

  /*static*/ ParametricIirCoefficientBase::Type
    ParametricIirCoefficientBase::stringToTypeId( std::string const & name )
  {
    return translator().idToType( name );
  }


template< typename CoefficientType >
ParametricIirCoefficient< CoefficientType >::ParametricIirCoefficient()
  : mType( Type::allpass )
  , mFrequency( static_cast<CoefficientType>(1.0) )
  , mQuality( static_cast<CoefficientType>(1.0) )
  , mGain( static_cast<CoefficientType>(0.0) )
{
}

template< typename CoefficientType >
ParametricIirCoefficient< CoefficientType >::ParametricIirCoefficient( ParametricIirCoefficient const & rhs ) = default;

template< typename CoefficientType >
ParametricIirCoefficient< CoefficientType >::ParametricIirCoefficient( ParametricIirCoefficient && rhs ) = default;

template< typename CoefficientType >
ParametricIirCoefficient< CoefficientType >::ParametricIirCoefficient( Type typeId,
    CoefficientType centerFrequency,
    CoefficientType quality,
    CoefficientType gain = static_cast<CoefficientType>(0.0) )
 : mType( typeId )
 , mFrequency( centerFrequency )
 , mQuality( quality )
 , mGain( gain )
{
}

template< typename CoefficientType > 
void ParametricIirCoefficient< CoefficientType >::setType( Type newType )
{
  mType = newType;
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::setFrequency( CoefficientType newFrequency )
{
  mFrequency = newFrequency;
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::setQuality( CoefficientType newQuality )
{
  mQuality = newQuality;
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::setGain( CoefficientType newGain )
{
  mGain = newGain;
}

template< typename CoefficientType >
/*static*/ ParametricIirCoefficient< CoefficientType > &&
ParametricIirCoefficient< CoefficientType >::fromJson( boost::property_tree::ptree const & tree )
{
  ParametricIirCoefficient< CoefficientType > res;
  res.loadJson( tree );
  return std::move(res);
}

template< typename CoefficientType >
/*static*/ ParametricIirCoefficient< CoefficientType > &&
ParametricIirCoefficient< CoefficientType >::fromJson( std::basic_istream<char> & stream )
{
  ParametricIirCoefficient< CoefficientType > res;
  res.loadJson( stream );
  return std::move(res);
}

template< typename CoefficientType >
/*static*/ ParametricIirCoefficient< CoefficientType > &&
ParametricIirCoefficient< CoefficientType >::fromJson( std::string const & str )
{
  ParametricIirCoefficient< CoefficientType > res;
  res.loadJson( str );
  return std::move( res );
}

template< typename CoefficientType >
/*static*/ ParametricIirCoefficient< CoefficientType > &&
ParametricIirCoefficient< CoefficientType >::fromXml( boost::property_tree::ptree const & tree )
{
  ParametricIirCoefficient< CoefficientType > res;
  res.loadXml( tree );
  return std::move( res );
}

template< typename CoefficientType >
/*static*/ ParametricIirCoefficient< CoefficientType > &&
ParametricIirCoefficient< CoefficientType >::fromXml( std::basic_istream<char> & stream )
{
  ParametricIirCoefficient< CoefficientType > res;
  res.loadXml( stream );
  return std::move( res );
}

template< typename CoefficientType >
/*static*/ ParametricIirCoefficient< CoefficientType > && 
ParametricIirCoefficient< CoefficientType >::fromJson( std::string const & str );


template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::loadJson( boost::property_tree::ptree const & tree )
{
  std::string const typeStr = tree.get<std::string>( "type" );
  CoefficientType const frequency = tree.get<CoefficientType>( "f" );
  CoefficientType const quality = tree.get<CoefficientType>( "q" );
  boost::optional<CoefficientType> const gainOpt = tree.get<CoefficientType>( "gain" );
  Type const typeId = stringToTypeId( typeStr ); // might throw
  setType( typeId );
  setFrequency( frequency );
  setQuality( quality );
  setGain( gainOpt ? *gainOpt : static_cast<CoefficientType >(0.0) );
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::loadJson( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_json( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a json ParametricIirCoefficient node: " ) + ex.what( ) );
  }
  loadJson( tree );
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::loadJson( std::string const & str )
{
  std::stringstream stream(str);
  loadJson( stream );
}


template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::loadXml( boost::property_tree::ptree const & tree )
{
  // Hack: Depending from where this method is called, we don't know
  boost::property_tree::ptree const biquadTree = tree.count( "iir" ) == 0
    ? tree : tree.get_child( "iir" );
#if 0
  boost::optional const a0Optional = biquadTree.get_optional( "<xmlattr>.a0" );
  CoeffType const a0 = a0Optional ? *a0Optional : static_cast(1.0f);
  CoeffType const a1 = biquadTree.get( "<xmlattr>.a1" );
  CoeffType const a2 = biquadTree.get( "<xmlattr>.a2" );
  CoeffType const b0 = biquadTree.get( "<xmlattr>.b0" );
  CoeffType const b1 = biquadTree.get( "<xmlattr>.b1" );
  CoeffType const b2 = biquadTree.get( "<xmlattr>.b2" );

  at( 0 ) = b0 / a0;
  at( 1 ) = b1 / a0;
  at( 2 ) = b2 / a0;
  at( 3 ) = a1 / a0;
  at( 4 ) = a2 / a0;
#endif
}

template< typename CoefficientType >
void ParametricIirCoefficient<CoefficientType>::loadXml( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_xml( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a XML ParametricIirCoefficient node: " ) + ex.what( ) );
  }
  loadXml( tree );
}

template< typename CoefficientType >
void ParametricIirCoefficient<CoefficientType>::loadXml( std::string const & str )
{
  boost::property_tree::ptree tree;
  try
  {
    read_xml( str, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a XML ParametricIirCoefficient node: " ) + ex.what() );
  }
  loadXml( tree );
}

template< typename CoefficientType >
void ParametricIirCoefficient<CoefficientType>::writeJson( boost::property_tree::ptree & tree ) const
{
#if 0
  tree.put( "b0", b0() );
  tree.put( "b1", b1( ) );
  tree.put( "b2", b2( ) );
  tree.put( "a1", a1( ) );
  tree.put( "a2", a2( ) );
#endif
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::writeJson( std::basic_ostream<char> & stream ) const
{
  boost::property_tree::ptree tree;
  writeJson( tree );
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::writeJson( std::string & str ) const
{
  std::stringstream stream;
  writeJson( stream );
  str = stream.str();
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::writeXml( boost::property_tree::ptree & tree ) const
{
  // TODO: implement me!
  assert( false );
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::writeXml( std::basic_ostream<char> & stream ) const
{
}

template< typename CoefficientType >
void ParametricIirCoefficient< CoefficientType >::writeXml( std::string & str ) const
{
}

// Explicit instantiations
template class ParametricIirCoefficient< float >;
template class ParametricIirCoefficient< double >;
template class ParametricIirCoefficient< long double >;

} // namespace pml
} // namespace visr

