//
//  LoudspeakerArray.cpp
//
//  Created by Dylan Menzies on 18/11/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "LoudspeakerArray.h"

#include <libefl/degree_radian_conversion.hpp>
#include <libefl/cartesian_spherical_conversion.hpp>

#include <libpml/biquad_parameter.hpp>
#include <libpml/index_sequence.hpp>
#include <libpml/float_sequence.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <ciso646>
#include <cstdio>
#include <iterator>
#include <map>
#include <set>
#include <tuple>

#define PI 3.1412659f

namespace visr
{
namespace panning
{


LoudspeakerArray::LoudspeakerArray()
 : m_is2D( false )
 , m_isInfinite( false )
{
}

LoudspeakerArray const &  LoudspeakerArray::operator=(LoudspeakerArray const & rhs)
{
  m_is2D = rhs.m_is2D;
  m_isInfinite = rhs.m_isInfinite;
  m_position = rhs.m_position;
  m_triplet = rhs.m_triplet;
  m_channel = rhs.m_channel;
  m_subwooferChannels = rhs.m_subwooferChannels;
  m_subwooferGains.resize( rhs.m_subwooferGains.numberOfRows(), rhs.m_subwooferGains.numberOfColumns() );
  m_subwooferGains.copy( rhs.m_subwooferGains );
  m_gainAdjustment.assign( rhs.m_gainAdjustment );
  m_delayAdjustment.assign( rhs.m_delayAdjustment );
  return *this;
}

int LoudspeakerArray::load( FILE *file )
{
  int n, i, chan;
  char c;
  Afloat x, y, z;
  Afloat az, el, r;
  int l1, l2, l3;
  int nSpk, nTri;

  i = nSpk = nTri = 0;

  struct SpkStruct
  {
    LoudspeakerIndexType id;
    XYZ pos;
    ChannelIndex channel;
  };
  struct CmpSpkStruct
  {
    bool operator()( SpkStruct const & lhs, SpkStruct const & rhs ) const
    {
      return lhs.id < rhs.id;
    }
  };
  std::set<SpkStruct, CmpSpkStruct>  tmpSpeakers;
  std::vector< TripletType > tmpTriplets;

  m_is2D = false;
  m_isInfinite = false;

  if( file == 0 ) return -1;

  std::size_t numRegularSpeakers = 0; // Counter that excludes virtual loudspeakers

  do
  {
    c = fgetc( file );

    if( c == 'c' )
    {        // cartesians
      n = fscanf( file, "%d %d %f %f %f\n", &i, &chan, &x, &y, &z );
      if( n != 5 )
      {
        return -1;
      }
      SpkStruct newSpk;
      newSpk.id = i;
      newSpk.pos.set( x, y, z, m_isInfinite );
      newSpk.channel = chan;
      // Insert the new speaker description 
      bool insRet;
      std::tie( std::ignore, insRet ) = tmpSpeakers.insert( newSpk );
      if( !insRet )
      {
        return -1; // Insertion failed, i.e., duplicated index
      }
      if( chan > 0 )
      {
        ++numRegularSpeakers;
      }
    }
    else if( c == 'p' )
    {   // polars, using degrees
      n = fscanf( file, "%d %d %f %f %f\n", &i, &chan, &az, &el, &r );
      if( n != 5 )
      {
        return -1;
      }
      std::tie( x, y, z ) = efl::spherical2cartesian( efl::degree2radian( az ),
                                                      efl::degree2radian( el ),
                                                      r );
      SpkStruct newSpk;
      newSpk.id = i;
      newSpk.pos.set( x, y, z, m_isInfinite );
      newSpk.channel = chan;
      // Insert the new speaker description 
      bool insRet;
      std::tie( std::ignore, insRet ) = tmpSpeakers.insert( newSpk );
      if( !insRet )
      {
        return -1; // Insertion failed, i.e., duplicated index
      }
      if( chan > 0 )
      {
        ++numRegularSpeakers;
      }
    }
    else if( c == 't' )
    {    // tuplet - triplet or duplet
      n = fscanf( file, "%d %d %d %d\n", &i, &l1, &l2, &l3 );
      if( n < 3 || n > 4 )
      {
        return -1;
      }
      // The triplet index does not matter.
      std::array<LoudspeakerIndexType, 3> triplet{ { l1, l2, l3 } };
      tmpTriplets.push_back( triplet );
    }
    else if( c == '2' )
    {    // switch to '2D' mode
      m_is2D = true;
    }
    else if( c == 'i' )
    {    // switch to 'infinite' mode
      m_isInfinite = true;
    }
    else if( c == '%' )
    {    // comment
      while( fgetc( file ) != '\n' && !feof( file ) );
    }

  }
  while( !feof( file ) );

  std::size_t const numSpeakers = tmpSpeakers.size(); // including virtual speakers
  m_position.resize( numSpeakers );
  m_channel.resize( numRegularSpeakers );
  m_subwooferGains.resize( 0, numRegularSpeakers );
  m_subwooferChannels.clear( );

  // As the txt configuration file does not support these features, set them to neutral default values.
  m_gainAdjustment.resize( numRegularSpeakers );
  m_gainAdjustment.fillValue( 1.0f );
  m_delayAdjustment.resize( numRegularSpeakers );
  m_delayAdjustment.fillValue( 0.0f );

  // The plain text configuration does not support an equalisation configuration, so we reset the configuration data
  // to an empty state..
  mOutputEqs.reset();

  LoudspeakerIndexType spkIdx = 0;
  for( auto const & v : tmpSpeakers )
  {
    // Check that the (ordered) speakers are consecutively ordered from 1.
    if( v.id != spkIdx+1 ) // Speakers are still one-indexed.
    {
      return -1;
    }
    if( v.channel > 0 )
    {
      m_channel[spkIdx] = v.channel;
    }
    m_position[spkIdx] = v.pos;
    ++spkIdx;
  }

  std::size_t const numTriplets = tmpTriplets.size();
  m_triplet.resize( numTriplets );
  for( std::size_t tripletIdx( 0 ); tripletIdx < numTriplets; ++tripletIdx )
  {
    std::array<LoudspeakerIndexType, 3> const & src = tmpTriplets[tripletIdx];
    TripletType & dest = getTriplet( tripletIdx );
    dest[0] = src[0] - 1; // the collected indices are one-offset, so we need to convert them.
    dest[1] = src[1] - 1;
    dest[2] = src[2] - 1;
  }

  return 0;
}

namespace // unnamed
{

/**
 * Local function to parse the coordinate of either a normal or a virtual loudspeaker.
 * @param node The speaker node
 * @param isInfinite The global infinity flag for the array, i.e., whether loudspeakers are considered to emit plane waves.
 * @return Reference to return the parsed position.
 * @throw std::invalid_argument If parsing fails.
 */
XYZ parseCoordNode( boost::property_tree::ptree const & node, bool isInfinite )
{
  XYZ pos(0.0f, 0.0f, 0.0f, isInfinite );
  using namespace boost::property_tree;
  std::size_t numCartCoords = node.count( "cart" );
  std::size_t numPolarCoords = node.count( "polar" );

  if( numCartCoords > 1 or numPolarCoords > 1 or not( (numCartCoords == 1) xor( numPolarCoords == 1 ) ) )
  {
    throw std::invalid_argument( "LoudspeakerArray::loadXml(): For each speaker, exactly one \"cart\" or \"polar\" node must exist.`" );
  }
  if( numCartCoords == 1 )
  {
    ptree::const_assoc_iterator cartIt = node.find( "cart" );
    assert( cartIt != node.not_found( ) );
    ptree const coordNode = cartIt->second;
    pos.x = coordNode.get<Afloat>( "<xmlattr>.x" );
    pos.y = coordNode.get<Afloat>( "<xmlattr>.y" );
    pos.z = coordNode.get<Afloat>( "<xmlattr>.z" );
  }
  else
  {
    ptree::const_assoc_iterator polarIt = node.find( "polar" );
    assert( polarIt != node.not_found( ) );
    ptree const coordNode = polarIt->second;
    Afloat const az = coordNode.get<Afloat>( "<xmlattr>.az" );
    Afloat const el = coordNode.get<Afloat>( "<xmlattr>.el" );
    Afloat const r = coordNode.get<Afloat>( "<xmlattr>.r" );
    std::tie( pos.x, pos.y, pos.z ) = efl::spherical2cartesian( efl::degree2radian( az ), efl::degree2radian( el ), r );
  }
  return pos;
}

/**
 * Local function to parse the gain and delay adjustment out of either a loudspeaker or a subwoofer node.
 * @param node The XML element containing the attributes
 * @param [out] gain The parsed gain value, linear scale.
 * @param [out] delay The parsed delay value, in seconds.
 * @throw std::invalid_argument if the parsing fails.
 */
void parseGainDelayAdjustments( boost::property_tree::ptree const & node, Afloat & gain, Afloat & delay )
{
  boost::optional<Afloat> const gainLinear = node.get_optional<Afloat>( "<xmlattr>.gain" );
  boost::optional<Afloat> const gainDB = node.get_optional<Afloat>( "<xmlattr>.gainDB" );
  if( gainLinear and gainDB )
  {
    throw std::invalid_argument( "ArrayConfiguration::loadXml(): The \"gain\" or \"gainDB\" attributes are mutually exclusive." );
  }
  else if( gainLinear )
  {
    gain = *gainLinear;
  }
  else if( gainDB )
  {
    gain = std::pow( 10.0f, *gainDB / 20.0f );
  }
  else // no gain specified, use default value.
  {
    gain = 1.0f;
  }
  boost::optional<Afloat> const delayOpt = node.get_optional<Afloat>( "<xmlattr>.delay" );
  delay  = delayOpt ? *delayOpt : 0.0f;
}

using ChannelEqLookupTable = std::map<std::string, pml::BiquadParameterList<Afloat> >;

/**
 * Local function to parse the optional node "outputEqConfiguration"
 * @param tree
 */
void parseChannelLookup( boost::property_tree::ptree const & tree, ChannelEqLookupTable & table, std::size_t numEqSections )
{
  using boost::property_tree::ptree;
  table.clear();
  auto const filterSpecNodes = tree.equal_range( "filterSpec" );
  for( ptree::const_assoc_iterator nodeIt( filterSpecNodes.first ); nodeIt != filterSpecNodes.second; ++nodeIt )
  {
    ptree const nodeTree = nodeIt->second;
    std::string const specName = nodeTree.get<std::string>( "<xmlattr>.name" );
    pml::BiquadParameterList<Afloat> biqList = pml::BiquadParameterList<Afloat>::fromXml( nodeTree );

    bool insertRes;
    std::tie( std::ignore, insertRes ) = table.insert( std::make_pair( specName, biqList ) );
  }
}

void parseEqFilter( boost::property_tree::ptree const & tree,
                    ChannelEqLookupTable const & table,
                    std::size_t channelIndex,
                    pml::BiquadParameterMatrix<Afloat> & eqMatrix )
{
  boost::optional<std::string> const eqId = tree.get_optional<std::string>( "<xmlattr>.eq" );
  if( eqId )
  {
    ChannelEqLookupTable::const_iterator findEq = table.find( *eqId );
    if( findEq == table.end( ) )
    {
      throw std::invalid_argument( "LoudspeakerArray::loadXml(): ." );
    }
    eqMatrix.setFilter( channelIndex, findEq->second );
  }
  else
  {
    eqMatrix.setFilter( channelIndex, pml::BiquadParameterList<Afloat>( ) );
  }
}

} // unnamed namespace


void LoudspeakerArray::loadXml( std::string const & filePath )
{
  using namespace boost::property_tree;
  boost::filesystem::path path( filePath );
  if( not exists( path ) or is_directory( path ) )
  {
    throw std::invalid_argument( "ArrayConfiguration::loadXml(): File does not exist." );
  }
  std::ifstream fileStream( path.string( ) );
  if( not fileStream )
  {
    throw std::invalid_argument( "ArrayConfiguration::loadXml(): Invalid file path." );
  }
  boost::property_tree::ptree parseTree;
  boost::property_tree::read_xml( fileStream, parseTree );

  boost::property_tree::ptree treeRoot = parseTree.get_child( "panningConfiguration" );

  boost::optional<std::string> const dimension = treeRoot.get_optional<std::string>( "<xmlattr>.dimension" );
  if( dimension )
  {
    if( *dimension == "2" )
    {
      m_is2D = true;
    }
    else if( *dimension == "3" )
    {
      m_is2D = false;
    }
    else
    {
      throw std::invalid_argument( "LoudspeakerArray::loadXml(): If given, the \"dimension\" atttribute must be either \"2\" or \"3\"." );
    }
  }
  else
  {
    m_is2D = false;
  }
  boost::optional<bool> const infinity = treeRoot.get_optional<bool>( "<xmlattr>.infinite" );
  m_isInfinite = infinity ? *infinity : false;

  auto const speakerNodes = treeRoot.equal_range( "loudspeaker" );
  auto const virtualSpeakerNodes = treeRoot.equal_range( "virtualspeaker" );
  auto const subwooferNodes = treeRoot.equal_range( "subwoofer" );

  std::size_t const numRegularSpeakers = std::distance( speakerNodes.first, speakerNodes.second );
  std::size_t const numVirtualSpeakers = std::distance( virtualSpeakerNodes.first, virtualSpeakerNodes.second );
  std::size_t const numTotalSpeakers = numRegularSpeakers + numVirtualSpeakers;
  std::size_t const numSubwoofers = std::distance( subwooferNodes.first, subwooferNodes.second );

  m_position.resize( numTotalSpeakers );
  m_channel.resize( numRegularSpeakers );

  m_gainAdjustment.resize( numRegularSpeakers + numSubwoofers);
  m_delayAdjustment.resize( numRegularSpeakers + numSubwoofers );

  const ChannelIndex cInvalidChannel = std::numeric_limits<ChannelIndex>::max();
  std::fill( m_channel.begin(), m_channel.end(), cInvalidChannel ); // assign special value to check afterwards if every speaker index has been assigned.

  std::size_t const numEqConfigs = treeRoot.count("outputEqConfiguration");
  if( numEqConfigs >= 2 )
  {
    throw std::invalid_argument( "LoudspeakerArray::loadXml(): Either zero or one \"outputEqConfiguration\" nodes are admissible." );
  }
  bool const eqConfigPresent = (numEqConfigs == 1);
  ChannelEqLookupTable eqLookupTable;
  std::size_t numEqSections = 0;
  if( eqConfigPresent )
  {
    ptree const eqNode = treeRoot.get_child( "outputEqConfiguration" );
    numEqSections = eqNode.get<std::size_t>( "<xmlattr>.numberOfBiquads" );
    parseChannelLookup( treeRoot.get_child( "outputEqConfiguration" ), eqLookupTable, numEqSections );
    mOutputEqs.reset( new pml::BiquadParameterMatrix<Afloat>( numRegularSpeakers + numSubwoofers, numEqSections ) );
  }
  else
  {
    mOutputEqs.reset( ); // Make it a null ptr to flag 'no EQ config present'
  }
  // TODO: If we leave the instantiation here, the EQ configuration
  // will always be present, albeit with zero biquad sections. However, the
  // renderer should work in this case as well (and currently does).
  // The current setting distinguished between the cases.
  // mOutputEqs.reset( new pml::BiquadParameterMatrix<Afloat>( numRegularSpeakers, numEqSections ) );

  // The maximum admissible loudspeaker index as used in the file,
  // i.e., one-offset.
  LoudspeakerIndexType const maxSpeakerIndexOneOffset
    = static_cast<LoudspeakerIndexType>(numTotalSpeakers);

  for( ptree::const_assoc_iterator treeIt( speakerNodes.first ); treeIt != speakerNodes.second; ++treeIt )
  {
    ptree const childTree = treeIt->second;
    int id = childTree.get<int>( "<xmlattr>.id" );
    if( id < 1 or id > maxSpeakerIndexOneOffset )
    {
      throw std::invalid_argument( "LoudspeakerArray::loadXml(): The loudspeaker id exceeds the number of loudspeakers." );
    }
    int idZeroOffset = id - 1;
    if( m_channel[idZeroOffset] != cInvalidChannel )
    {
      throw std::invalid_argument( "LoudspeakerArray::loadXml(): Each speaker id must be used exactly once." );
    }
    ChannelIndex const chIdx = childTree.get<ChannelIndex>( "<xmlattr>.channel");
    if( chIdx < 1 )
    {
      throw std::invalid_argument( "LoudspeakerArray::loadXml(): The channel id must be greater or equal than one." );
    }
    m_channel[idZeroOffset] = chIdx - 1;
    m_position[idZeroOffset] = parseCoordNode( childTree, m_isInfinite );

    parseGainDelayAdjustments( childTree, m_gainAdjustment[idZeroOffset], m_delayAdjustment[idZeroOffset] );

    if( eqConfigPresent )
    {
      parseEqFilter( childTree, eqLookupTable, idZeroOffset, *mOutputEqs );
    }
  }
  // Same for the virtual speaker nodes, except there is no 'channel' field and no gain/delay adjustments.
  for( ptree::const_assoc_iterator treeIt( virtualSpeakerNodes.first ); treeIt != virtualSpeakerNodes.second; ++treeIt )
  {
    ptree const childTree = treeIt->second;
    int id = childTree.get<int>( "<xmlattr>.id" );
    if( id < 1 or id > maxSpeakerIndexOneOffset )
    {
      throw std::invalid_argument( "LoudspeakerArray::loadXml(): The loudspeaker id exceeds the number of loudspeakers." );
    }
    int idZeroOffset = id - 1;
    m_position[idZeroOffset] = parseCoordNode( childTree, m_isInfinite );
  }
  // The checks above (all speaker indices are between 1 and numTotalSpeakers && the indices are unique) are 
  // sufficient to ensure that the speaker indices are consecutive.

  // parse the triplet config
  auto const  tripletNodes = treeRoot.equal_range( "triplet" );
  std::size_t const numTriplets = std::distance( tripletNodes.first, tripletNodes.second );
  m_triplet.clear();
  m_triplet.reserve( numTriplets );
  // The maximim admissible index in a triplet. These are zero-offset values
  LoudspeakerIndexType const maxSpeakerIndex = maxSpeakerIndexOneOffset - 1;
  for( ptree::const_assoc_iterator tripletIt( tripletNodes.first ); tripletIt != tripletNodes.second; ++tripletIt )
  {
    ptree const childTree = tripletIt->second;
    std::array<LoudspeakerIndexType, 3> triplet;
    triplet[0] = childTree.get<LoudspeakerIndexType>( "<xmlattr>.l1" ) - 1;
    triplet[1] = childTree.get<LoudspeakerIndexType>( "<xmlattr>.l2" ) - 1;
    if( m_is2D )
    {
      triplet[2] = -1; // special value reserved for 'unused'
    }
    else
    {
      triplet[2] = childTree.get<LoudspeakerIndexType>( "<xmlattr>.l3" ) - 1;
    }
    if( (triplet[0] < 0) or (triplet[0] > maxSpeakerIndex ) 
      or ( triplet[1] < 0 ) or( triplet[1] > maxSpeakerIndex )
      or ( (not m_is2D) and ( (triplet[2] < 0 ) or( triplet[2] > maxSpeakerIndex )) ) )
    {
      throw std::invalid_argument( "LoudspeakerArray::loadXml(): Triplet references non-existing speaker index." );
    }
    m_triplet.push_back( triplet );
  }
  assert( m_triplet.size() == numTriplets );

  // Subwoofer configuration
  m_subwooferChannels.resize( numSubwoofers );
  m_subwooferGains.resize( numSubwoofers, numRegularSpeakers );
  std::size_t subIdx( 0 );
  for( ptree::const_assoc_iterator subIt( subwooferNodes.first ); subIt != subwooferNodes.second; ++subIt, ++subIdx )
  {
    ptree const subNode = subIt->second;
    ChannelIndex const subChannel = subNode.get<int>( "<xmlattr>.channel" ) - 1;
    m_subwooferChannels[subIdx] = subChannel;

    std::string const speakerIndicesStr = subNode.get<std::string>( "<xmlattr>.assignedLoudspeakers" );
    pml::IndexSequence const speakerIndices( speakerIndicesStr );
    if( std::find_if( speakerIndices.begin(), speakerIndices.end(),
      [&]( pml::IndexSequence::IndexType val ) { return (val < 1) or( val > numRegularSpeakers ); } ) != speakerIndices.end( ) )
    {
      throw std::invalid_argument( "A loudspeaker index exceeds the set of regular loudspeakers." );
    }
    boost::optional<std::string> const weightStr = subNode.get_optional<std::string>( "<xmlattr>.weights" );
    if( weightStr )
    {
      pml::FloatSequence<Afloat> speakerWeights( *weightStr );
      if( speakerWeights.size() != speakerIndices.size() )
      {
        throw std::invalid_argument( "The loudspeaker index list and the weight vector must have the same length." );
      }
      for( std::size_t entryIdx( 0 ); entryIdx < speakerIndices.size(); ++entryIdx )
      {
        m_subwooferGains( subIdx, speakerIndices[entryIdx] - 1 ) = speakerWeights[entryIdx];
      }
    }
    else // No weight vector given, assign ones to all channels indexed by the assignedLoudspeakers string.
    {
      for( std::size_t entryIdx( 0 ); entryIdx < speakerIndices.size( ); ++entryIdx )
      {
        m_subwooferGains( subIdx, speakerIndices[entryIdx] - 1 ) = 1.0f;
      }
    }
    parseGainDelayAdjustments( subNode, m_gainAdjustment[numRegularSpeakers+subIdx],
      m_delayAdjustment[numRegularSpeakers + subIdx] );

    if( eqConfigPresent )
    {
      parseEqFilter( subNode, eqLookupTable, numRegularSpeakers + subIdx, *mOutputEqs );
    }
  }
  // Check the subwoofer and the subwoofer indices whether all indices are unique.
  std::vector<ChannelIndex> spkIndicesSorted( m_channel );
  std::vector<ChannelIndex> subIndicesSorted( m_subwooferChannels );
  std::sort( spkIndicesSorted.begin(), spkIndicesSorted.end() );
  std::sort( subIndicesSorted.begin( ), subIndicesSorted.end( ) );
  if( std::adjacent_find( spkIndicesSorted.begin(), spkIndicesSorted.end() ) != spkIndicesSorted.end() )
  {
    // Technically, this isn't possible, as the implementation above allows only consecutive ranges 
    // from 1...numRegularSpeakers.
    throw std::invalid_argument( "LoudspeakerArray::loadXml(): Duplicated loudspeaker channel indices." );
  }
  if( std::adjacent_find( subIndicesSorted.begin( ), subIndicesSorted.end( ) ) != subIndicesSorted.end( ) )
  {
    throw std::invalid_argument( "LoudspeakerArray::loadXml(): Duplicated subwoofer channel indices." );
  }
  std::vector<ChannelIndex> collidingChannelIndices;
  std::set_intersection( spkIndicesSorted.begin( ), spkIndicesSorted.end( ),
                         subIndicesSorted.begin( ), subIndicesSorted.end( ),
                         std::back_inserter(collidingChannelIndices) );
  if( not collidingChannelIndices.empty() )
  {
    throw std::invalid_argument( "LoudspeakerArray::loadXml(): Loudspeaker and subwoofer channels indices are not exclusive." );
  }
}

bool LoudspeakerArray::outputEqualisationPresent() const
{
  return bool( mOutputEqs ); // Operator bool checks whether pointer is assigned.
}

/**
* Query the number of biquads per output channel.
* Returns 0 if no equalisation configuration is present.
*/
std::size_t LoudspeakerArray::outputEqualisationNumberOfBiquads() const
{
  return outputEqualisationPresent() ? mOutputEqs->numberOfSections() : 0;
}

/**
* Return a matrix containing the biquad parameters for all output sections.
* The dimension of the matrix is
* \p (numRegularSpeakers()+getNumSubwoofers()) x outputEqualisationNumberOfBiquads()
* @throw std::logic_error if no output EQ configuration is present, i.e., outputEqualisationPresent() is \b false.
*/
pml::BiquadParameterMatrix<Afloat> const & 
LoudspeakerArray::outputEqualisationBiquads() const
{
  if( not outputEqualisationPresent() )
  {
    throw std::logic_error( "LoudspeakerArray::::outputEqualisationBiquads(): No EQ configuration present." );
  }
  return *mOutputEqs;
}

} // namespace panning
} // namespace visr

