/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "biquad_parameter.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <istream>
#include <iostream>
#include <sstream>
#include <string>

namespace visr
{
namespace pml
{

template< typename CoeffType >
/*static*/ BiquadParameter<CoeffType>
BiquadParameter<CoeffType>::fromJson( boost::property_tree::ptree const & tree )
{
  BiquadParameter<CoeffType> res;
  res.loadJson( tree );
  return res;
}

template< typename CoeffType >
/*static*/ BiquadParameter<CoeffType>
BiquadParameter<CoeffType>::fromJson( std::basic_istream<char> & stream )
{
  BiquadParameter<CoeffType> res;
  res.loadJson( stream );
  return res;
}

template< typename CoeffType >
/*static*/ BiquadParameter<CoeffType>
BiquadParameter<CoeffType>::fromXml( boost::property_tree::ptree const & tree )
{
  BiquadParameter<CoeffType> res;
  res.loadXml( tree );
  return res;
}

template< typename CoeffType >
/*static*/ BiquadParameter<CoeffType>
BiquadParameter<CoeffType>::fromXml( std::basic_istream<char> & stream )
{
  BiquadParameter<CoeffType> res;
  res.loadXml( stream );
  return res;
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::loadJson( boost::property_tree::ptree const & tree )
{
  // boost::property_tree::ptree const biquadTree = tree.get_child( "biquad" );

  CoeffType const a0 = tree.get<CoeffType>( "a0", static_cast<CoeffType>(1.0f) );
  at( 0 ) = tree.get<CoeffType>( "b0" ) / a0;
  at( 1 ) = tree.get<CoeffType>( "b1" ) / a0;
  at( 2 ) = tree.get<CoeffType>( "b2" ) / a0;
  at( 3 ) = tree.get<CoeffType>( "a1" ) / a0;
  at( 4 ) = tree.get<CoeffType>( "a2" ) / a0;
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::loadJson( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_json( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a json BiquadParameter node: " ) + ex.what( ) );
  }
  loadJson( tree );
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::loadXml( boost::property_tree::ptree const & tree )
{
  // Hack: Depending from where this method is called, we don't know
  boost::property_tree::ptree const biquadTree = tree.count( "biquad" ) == 0
    ? tree : tree.get_child( "biquad" );

  boost::optional<CoeffType> const a0Optional = biquadTree.get_optional<CoeffType>( "<xmlattr>.a0" );
  CoeffType const a0 = a0Optional ? *a0Optional : static_cast<CoeffType>(1.0f);
  CoeffType const a1 = biquadTree.get<CoeffType>( "<xmlattr>.a1" );
  CoeffType const a2 = biquadTree.get<CoeffType>( "<xmlattr>.a2" );
  CoeffType const b0 = biquadTree.get<CoeffType>( "<xmlattr>.b0" );
  CoeffType const b1 = biquadTree.get<CoeffType>( "<xmlattr>.b1" );
  CoeffType const b2 = biquadTree.get<CoeffType>( "<xmlattr>.b2" );

  at( 0 ) = b0 / a0;
  at( 1 ) = b1 / a0;
  at( 2 ) = b2 / a0;
  at( 3 ) = a1 / a0;
  at( 4 ) = a2 / a0;
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::loadXml( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_xml( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a XML BiquadParameter node: " ) + ex.what( ) );
  }
  loadXml( tree );
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::writeJson( boost::property_tree::ptree & tree ) const
{
  tree.put( "b0", b0() );
  tree.put( "b1", b1( ) );
  tree.put( "b2", b2( ) );
  tree.put( "a1", a1( ) );
  tree.put( "a2", a2( ) );
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::writeJson( std::basic_ostream<char> & stream ) const
{
  boost::property_tree::ptree tree;
  writeJson( tree );
  try
  {
    write_json( stream, tree);
  } catch (std::exception const & ex)
  {
    throw( "Error while writing BiquadParameter message to JSON" );
  }
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::writeJson( std::string & str ) const
{
  std::stringstream stream;
  writeJson( stream );
  str = stream.str();
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::writeXml( boost::property_tree::ptree & tree ) const
{
  // TODO: implement me!
  assert( false );
}

template< typename CoeffType >
void BiquadParameter<CoeffType>::writeXml( std::basic_ostream<char> & stream ) const
{
  boost::property_tree::ptree tree;
  writeJson( tree );

}

template< typename CoeffType >
void BiquadParameter<CoeffType>::writeXml( std::string & str ) const
{
}

///////////////////////////////////////////////////////////////////////////////

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromJson( boost::property_tree::ptree const & tree )
{
  BiquadParameterList<CoeffType> ret;
  ret.loadJson( tree );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromJson( std::basic_istream<char> & stream )
{
  BiquadParameterList<CoeffType> ret;
  ret.loadJson( stream );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromJson( std::string const & str )
{
  BiquadParameterList<CoeffType> ret;
  ret.loadJson( str );
  return ret;

}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromXml( boost::property_tree::ptree const & tree )
{
  BiquadParameterList<CoeffType> ret;
  ret.loadXml( tree );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromXml( std::basic_istream<char> & stream )
{
  BiquadParameterList<CoeffType> ret;
  ret.loadXml( stream );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromXml( std::string const & str )
{
  BiquadParameterList<CoeffType> ret;
  ret.loadXml( str );
  return ret;
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::loadJson( boost::property_tree::ptree const & tree )
{
  auto const biquadNodes = tree.equal_range( "" );
  std::size_t const numBiquads = std::distance( biquadNodes.first, biquadNodes.second );
  resize( numBiquads );
  std::size_t biqIndex = 0;
  for( boost::property_tree::ptree::const_assoc_iterator treeIt( biquadNodes.first ); treeIt != biquadNodes.second; ++treeIt, ++biqIndex )
  {
    boost::property_tree::ptree const childTree = treeIt->second;
    at( biqIndex ).loadJson( childTree );
  }
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::loadJson( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_json( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a JSON BiquadParameterList representation: " ) + ex.what( ) );
  }
  loadJson( tree );
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::loadJson( std::string const & str )
{
  std::stringstream stream( str );
  loadJson( stream );
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::loadXml( boost::property_tree::ptree const & tree )
{
  // Hack: Depending from where this method is called, we don't know
  boost::property_tree::ptree const specNode = tree.count( "filterSpec" ) == 0
    ? tree : tree.get_child( "filterSpec" );

  auto const biquadNodes = specNode.equal_range( "biquad" );
  std::size_t const numBiquads = std::distance( biquadNodes.first, biquadNodes.second );
  resize( numBiquads );
  std::size_t biqIndex = 0;
  for( boost::property_tree::ptree::const_assoc_iterator treeIt( biquadNodes.first ); treeIt != biquadNodes.second; ++treeIt, ++biqIndex )
  {
    boost::property_tree::ptree const childTree = treeIt->second;
    at( biqIndex ).loadXml( childTree );
  }
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::loadXml( std::basic_istream<char> & stream )
{
  boost::property_tree::ptree tree;
  try
  {
    read_xml( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing a XML BiquadParameter node: " ) + ex.what( ) );
  }
  loadXml( tree );
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::loadXml( std::string const & str )
{
  std::stringstream stream( str );
  loadXml( stream );
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::writeJson( boost::property_tree::ptree & tree ) const
{
  for( std::size_t idx( 0 ); idx < size(); ++idx )
  {
    boost::property_tree::ptree child;
    at( idx ).writeJson( child );
    tree.push_back( std::make_pair( "", child ) );
  }
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::writeJson( std::basic_ostream<char> & stream ) const
{
  boost::property_tree::ptree tree;
  writeJson( tree );
  try
  {
    write_json( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while writing a BiquadParameterList to JSON: " ) + ex.what( ) );
  }
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::writeJson( std::string & str ) const
{
  std::stringstream stream;
  writeJson( stream );
  str = stream.str();
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::writeXml( boost::property_tree::ptree & tree ) const
{
  // TODO: implement me!
  assert( false );
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::writeXml( std::basic_ostream<char> & stream ) const
{
  boost::property_tree::ptree tree;
  writeXml( tree );
  try
  {
    write_xml( stream, tree );
  }
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while writing BiquadParameterList to XML: " ) + ex.what( ) );
  }
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::writeXml( std::string & str ) const
{
  std::stringstream stream;
  writeXml( stream );
  str = stream.str();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename CoeffType>
BiquadParameterMatrix<CoeffType>::BiquadParameterMatrix( MatrixParameterConfig const & config )
  : BiquadParameterMatrix( config.numberOfRows(), config.numberOfColumns() )
{
}

template<typename CoeffType>
BiquadParameterMatrix<CoeffType>::BiquadParameterMatrix( ParameterConfigBase const & config )
  : BiquadParameterMatrix( dynamic_cast<MatrixParameterConfig const &>( config ) )
{
}

template<typename CoeffType>
BiquadParameterMatrix<CoeffType>::BiquadParameterMatrix( std::size_t numberOfFilters, std::size_t numberOfBiquads )
{
  resize( numberOfFilters, numberOfBiquads );
}

template<typename CoeffType>
BiquadParameterMatrix<CoeffType>::~BiquadParameterMatrix()
{
}

template<typename CoeffType>
void BiquadParameterMatrix<CoeffType>::resize( std::size_t numberOfFilters, std::size_t numberOfBiquads )
{
  // Define the initial entry for all filter rows (numberOfBiquads 'default' biquad specs) 
  BiquadParameterList<CoeffType> templ( numberOfBiquads );
  mRows.resize( numberOfFilters, templ );
}

template<typename CoeffType>
void BiquadParameterMatrix<CoeffType>::setFilter( std::size_t filterIdx, BiquadParameterList<CoeffType> const & newFilter )
{
  if( filterIdx >= numberOfFilters() )
  {
    throw std::out_of_range( "BiquadParameterMatrix::setFilter(): Filter index exceeds maximum admissible value." );
  }
  if( newFilter.size() > numberOfSections() )
  {
    throw std::invalid_argument( "BiquadParameterMatrix::setFilter(): The number index exceeds maximum admissible value." );
  }
  BiquadParameterList<CoeffType> & row = mRows[filterIdx];
  std::copy( &newFilter[0], &newFilter[0] + newFilter.size(), &row[0] );
  std::fill_n( &row[0] + newFilter.size(), numberOfSections() - newFilter.size(), pml::BiquadParameter<CoeffType>() );
}

// explicit instantiation
template class BiquadParameter<float>;
template class BiquadParameterList<float>;
template class BiquadParameterMatrix<float>;

template class BiquadParameter<double>;
template class BiquadParameterList<double>;
template class BiquadParameterMatrix<double>;

} // namespace pml
} // namespace visr
