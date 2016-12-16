/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "parametric_iir_coefficients.hpp"

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

/*static*/ ParametricIirCoefficients
ParametricIirCoefficients::fromJson( boost::property_tree::ptree const & tree )
{
  ParametricIirCoefficients res;
  res.loadJson( tree );
  return res;
}

/*static*/ ParametricIirCoefficients
ParametricIirCoefficients::fromJson( std::basic_istream<char> & stream )
{
  ParametricIirCoefficients res;
  res.loadJson( stream );
  return res;
}

/*static*/ ParametricIirCoefficients
ParametricIirCoefficients::fromXml( boost::property_tree::ptree const & tree )
{
  ParametricIirCoefficients res;
  res.loadXml( tree );
  return res;
}

/*static*/ ParametricIirCoefficients
ParametricIirCoefficients::fromXml( std::basic_istream<char> & stream )
{
  ParametricIirCoefficients res;
  res.loadXml( stream );
  return res;
}

void ParametricIirCoefficients::loadJson( boost::property_tree::ptree const & tree )
{
}

void ParametricIirCoefficients::loadJson( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_json( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a json ParametricIirCoefficients node: " ) + ex.what( ) );
  }
  loadJson( tree );
}

void ParametricIirCoefficients::loadXml( boost::property_tree::ptree const & tree )
{
  // Hack: Depending from where this method is called, we don't know
  boost::property_tree::ptree const biquadTree = tree.count( "biquad" ) == 0
    ? tree : tree.get_child( "biquad" );
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

void ParametricIirCoefficients::loadXml( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_xml( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a XML ParametricIirCoefficients node: " ) + ex.what( ) );
  }
  loadXml( tree );
}

void ParametricIirCoefficients::writeJson( boost::property_tree::ptree & tree ) const
{
#if 0
  tree.put( "b0", b0() );
  tree.put( "b1", b1( ) );
  tree.put( "b2", b2( ) );
  tree.put( "a1", a1( ) );
  tree.put( "a2", a2( ) );
#endif
}

void ParametricIirCoefficients::writeJson( std::basic_ostream<char> & stream ) const
{
  boost::property_tree::ptree tree;
  writeJson( tree );
}

void ParametricIirCoefficients::writeJson( std::string & str ) const
{
  std::stringstream stream;
  writeJson( str );
  str = stream.str();
}

void ParametricIirCoefficients::writeXml( boost::property_tree::ptree & tree ) const
{
  // TODO: implement me!
  assert( false );
}

void ParametricIirCoefficients::writeXml( std::basic_ostream<char> & stream ) const
{
}

void ParametricIirCoefficients::writeXml( std::string & str ) const
{
}

class ParametricIirCoefficients::TypeIdTranslator
{
private:
  using ForwardMap = std::map<Type,std::string>;
  using ReverseMap = std::map<std::string, Type >;

  ForwardMap mFwdMap;

  ReverseMap mReverseMap;

public:
  TypeIdTranslator();

  /**
   * @throw std::invalid_argument id id is not a valid typename.
   */
  Type idToType( const std::string & id ) const;

  std::string const & typeToId( Type type ) const;

};

ParametricIirCoefficients::TypeIdTranslator::TypeIdTranslator()
  : mFwdMap( { {Type::lowpass, "lowpass"}, {Type::highpass, "highpass"},
               {Type::bandpass, "bandpass"}, {Type::bandstop, "bandstop"},
               {Type::peak, "peak"}, {Type::notch, "notch"},
               {Type::lowshelf, "lowshelf"}, {Type::highshelf, "highshelf"},
               {Type::allpass, "allpass"} } )
{
  for( ForwardMap::value_type const v: mFwdMap )
  {
    mReverseMap.insert( std::make_pair( v.second, v.first ) );
  }
  // Todo: Add abbreviations
}

/*static*/ ParametricIirCoefficients::TypeIdTranslator const & ParametricIirCoefficients::translator()
{
  static const TypeIdTranslator sTranslator;
  return sTranslator;
}

/*static*/ std::string const &
ParametricIirCoefficients::typeIdToString( Type typeId )
{
  return translator().typeToId( typeId );
}

/*static*/ ParametricIirCoefficients::Type
ParametricIirCoefficients::stringToTypeId( std::string const & name )
{
  return translator().idToType( name );
}


} // namespace pml
} // namespace visr

