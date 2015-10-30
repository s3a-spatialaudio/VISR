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
  boost::property_tree::ptree const biquadTree = tree.get_child( "biquad" );

  boost::optional<CoeffType> const a0Optional = biquadTree.get_optional<CoeffType>( "a0" );
  CoeffType const a0 = a0Optional ? *a0Optional : static_cast<CoeffType>(1.0f);
  at( 0 ) = biquadTree.get<CoeffType>( "b0" ) / a0;
  at( 1 ) = biquadTree.get<CoeffType>( "b1" ) / a0;
  at( 2 ) = biquadTree.get<CoeffType>( "b2" ) / a0;
  at( 3 ) = biquadTree.get<CoeffType>( "a1" ) / a0;
  at( 4 ) = biquadTree.get<CoeffType>( "a2" ) / a0;
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

///////////////////////////////////////////////////////////////////////////////

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromJson( boost::property_tree::ptree const & tree )
{
  typename BiquadParameterList<CoeffType> ret;
  ret.loadJson( tree );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromJson( std::basic_istream<char> & stream )
{
  typename BiquadParameterList<CoeffType> ret;
  ret.loadJson( stream );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromJson( std::string const & str )
{
  typename BiquadParameterList<CoeffType> ret;
  ret.loadJson( str );
  return ret;

}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromXml( boost::property_tree::ptree const & tree )
{
  typename BiquadParameterList<CoeffType> ret;
  ret.loadXml( tree );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromXml( std::basic_istream<char> & stream )
{
  typename BiquadParameterList<CoeffType> ret;
  ret.loadXml( stream );
  return ret;
}

template< typename CoeffType >
/*static*/ BiquadParameterList<CoeffType>
BiquadParameterList<CoeffType>::fromXml( std::string const & str )
{
  typename BiquadParameterList<CoeffType> ret;
  ret.loadXml( str );
  return ret;
}

template< typename CoeffType >
void BiquadParameterList<CoeffType>::loadJson( boost::property_tree::ptree const & tree )
{
  auto const biquadNodes = tree.equal_range( "biquad" );
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
void BiquadParameterList<CoeffType>::loadJson( std::basic_istream<char> & stream )
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
void BiquadParameterList<CoeffType>::loadJson( std::string const & str )
{
  std::stringstream stream( str );
  loadXml( stream );
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
  typename BiquadParameterList<CoeffType> & row = mRows[filterIdx];
  std::copy( &newFilter[0], &newFilter[0] + newFilter.size(), &row[0] );
  std::fill_n( &row[0] + newFilter.size(), numberOfSections() - newFilter.size(), pml::BiquadParameter<CoeffType>() );
}

// explicit instantiation
template class BiquadParameter<float>;
template class BiquadParameterList<float>;
template class BiquadParameterMatrix<float>;


} // namespace pml
} // namespace visr
