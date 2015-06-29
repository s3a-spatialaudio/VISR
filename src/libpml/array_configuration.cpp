/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "array_configuration.hpp"

#include <boost/filesystem.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <ciso646>
#include <cmath>
#include <iostream>
#include <string>


namespace visr
{
namespace pml
{

ArrayConfiguration::ArrayConfiguration()
{
}

ArrayConfiguration::~ArrayConfiguration( )
{
}


void ArrayConfiguration::loadXml( std::string const & filePath )
{
  using namespace boost::property_tree;
  using namespace boost::lambda;

  boost::filesystem::path path( filePath );
  if( not exists( path ) or is_directory( path ) )
  {
    throw std::invalid_argument( "ArrayConfiguration::loadXml(): File does not exist." );
  }
  std::ifstream fileStream( path.string() );
  if( not fileStream )
  {
    throw std::invalid_argument( "ArrayConfiguration::loadXml(): Invalid file path." );
  }
  boost::property_tree::ptree parseTree;
  boost::property_tree::read_xml( fileStream, parseTree );

  boost::property_tree::ptree treeRoot = parseTree.get_child( "arrayconfiguration" );
  const auto speakerTrees = treeRoot.equal_range( "speaker" );

  for( ptree::const_assoc_iterator treeIt( speakerTrees.first ); treeIt != speakerTrees.second; ++treeIt )
  {
    ptree const childTree = treeIt->second;

    std::size_t const id = childTree.get<std::size_t>( "<xmlattr>.id" );
    boost::optional<double> const gainLinear = childTree.get_optional<double>( "<xmlattr>.gain" );
    boost::optional<double> const gainDB = childTree.get_optional<double>( "<xmlattr>.gainDB" );
    if( not (bool(gainLinear) xor bool(gainDB)) )
    {
      throw std::invalid_argument( "ArrayConfiguration::loadXml(): Each speaker node must contain exactly one \"gain\" or \"gainDB\" attribute." );
    }
    double const gain = bool( gainLinear ) ? *gainLinear : std::pow( 10.0, *gainDB / 20.0 );
    boost::optional<double> const delay = childTree.get_optional<double>( "<xmlattr>.delay" );

    Speaker const newSpeaker( id, gain, delay ? *delay : 0.0 );
    std::pair<ArrayType::iterator, bool> insertRes = mArray.insert( newSpeaker );
    if( !insertRes.second )
    {
      throw std::invalid_argument( "ArrayConfiguration::loadXml(): Duplicate speaker id." );
    }
  }
  // check whole array
  if( !mArray.empty() ) // empty arrays are admissible, albeit of limited usefullness.
  {
    if( mArray.begin()->id != 1 )
    {
      throw std::invalid_argument( "ArrayConfiguration::loadXml(): The lowest speaker id must be 1." );
    }
    if( std::adjacent_find( mArray.begin(), mArray.end(),
                            []( Speaker const & lhs, Speaker const & rhs ) { return lhs.id + 1 != rhs.id; } ) != mArray.end() )
    {
      throw std::invalid_argument( "ArrayConfiguration::loadXml(): Speaker indices must be adjacent." );
    }
  }

  // parse the subwoofer configuration
  const auto subTrees = treeRoot.equal_range( "subwoofer" );
  for( ptree::const_assoc_iterator treeIt( subTrees.first ); treeIt != subTrees.second; ++treeIt )
  {
    ptree const childTree = treeIt->second;
    std::size_t const channelIndex = childTree.get<std::size_t>( "<xmlattr>.channel" );
    double const gain = childTree.get<double>( "<xmlattr>.gain" );

    mSubwoofers.push_back( Subwoofer( channelIndex, gain ) );
  }

  std::vector<std::size_t> subwooferIndices;
  std::transform( mSubwoofers.begin(), mSubwoofers.end(), std::back_inserter( subwooferIndices ), []( Subwoofer const & sub ) { return sub.channelIndex; } );
  std::sort( subwooferIndices.begin(), subwooferIndices.end() );

  std::vector<std::size_t> spkIndices;
  std::transform( mArray.begin( ), mArray.end( ), std::back_inserter( spkIndices ), []( Speaker const & spk ) { return spk.id; } );
  std::sort( spkIndices.begin( ), spkIndices.end( ) );

  std::vector<std::size_t> clashIndices;
  std::set_intersection( spkIndices.begin(), spkIndices.end(), subwooferIndices.begin(), subwooferIndices.end(), std::back_inserter( clashIndices ) );
  if( not clashIndices.empty() )
  {
    throw std::invalid_argument( "ArrayConfiguration::loadXml(): The loudspeaker and subwoofer indices overlap." );
  }
}

} // namespace pml
} // namespace visr
