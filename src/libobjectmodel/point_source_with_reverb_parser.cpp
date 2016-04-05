/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "point_source_with_reverb_parser.hpp"

#include "point_source_parser.hpp"
#include "point_source_with_reverb.hpp"

#include <libpml/float_sequence.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/foreach.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

namespace visr
{
namespace objectmodel
{

/**
 * Unnamed namespace containing functions and function objects for dB<->linear conversion.
 * @todo Consider moving to a library for public use.
 */
namespace // unnamed
{
template< typename T >
T dB2Lin( T dB ) { return std::pow( static_cast<T>(10.0), static_cast<T>(0.05)*dB ); }

template< typename T >
T lin2dB( T lin ) { return static_cast<T>(20.0) * std::log10( lin ); }

template< typename T >
struct DB2Lin
{
  void operator()( T & val ) const { val = dB2Lin( val ); }
};

template< typename T >
struct Lin2dB
{
  void operator()( T & val ) const { val = lin2dB( val ); }
};

}


/*virtual*/ void PointSourceWithReverbParser::
parse( boost::property_tree::ptree const & tree, Object & src ) const
{
  using ptree = boost::property_tree::ptree;
  try
  {
    // note: cannot check for object type id since src might be a subclass of PointSource
    PointSourceWithReverb & reverbPointSrc = dynamic_cast<PointSourceWithReverb&>(src);

    // Parse all members inherited from the base class PointSource
    PointSourceParser::parse( tree, reverbPointSrc );

    // Parse data members specific to PointSourceWithReverb
    if( tree.count( "room" ) != 1 )
    {
      throw std::invalid_argument( "Object must contain exactly one \"room\" element." );
    }
    ptree const & roomTree = tree.get_child( "room" );

    std::size_t earlyIndex( 0 );
    ptree const & ereflTree = roomTree.get_child( "ereflect" );
    std::size_t const numEarlyReflections = ereflTree.count( "" );
    reverbPointSrc.setNumberOfDiscreteReflections( numEarlyReflections );
    auto const earlyNodes = ereflTree.equal_range( "" );
    for( ptree::const_assoc_iterator treeIt( earlyNodes.first ); treeIt != earlyNodes.second; ++treeIt,++earlyIndex )
    {
      PointSourceWithReverb::DiscreteReflection & refl = reverbPointSrc.discreteReflection( earlyIndex );
      ptree const earlyTree = treeIt->second;
      PointSourceWithReverb::Coordinate posX, posY, posZ;
      PointSourceParser::parsePosition( earlyTree.get_child( "position" ), posX, posY, posZ );
      refl.setPosition( posX, posY, posZ );
      refl.setDelay( earlyTree.get<ril::SampleType>( "delay" ) );
      refl.setDelay( earlyTree.get<LevelType>( "level" ) );

      ptree const & biquadTree = earlyTree.get_child( "biquadsos" );
      pml::BiquadParameterList<ril::SampleType> biqList;
      biqList.loadJson( biquadTree );
      if( biqList.size() > PointSourceWithReverb::cNumDiscreteReflectionBiquads )
      {
        throw std::invalid_argument( "PointSourceWithReverbParser: The number of biquad sections for an early reflection exceeds the maximum admissible value." );
      }
      refl.setReflectionFilters( biqList );
    }
    if( roomTree.count( "lreverb" ) != 1 )
    {
      throw std::invalid_argument( "Object must contain exactly one \"room.lreverb\" element." );
    }
    ptree const & lateTree = roomTree.get_child( "lreverb" );
    reverbPointSrc.setLateReverbOnset( lateTree.get<ril::SampleType>( "delay" ) );
    std::string const lateLevelString = lateTree.get<std::string>( "level" );
    pml::FloatSequence<ril::SampleType> lateLevels( lateLevelString );
    if( lateLevels.size() != PointSourceWithReverb::cNumberOfSubBands )
    {
      throw std::invalid_argument( "The number of elements in the \"lreverb.late\" attribute must match the fixed number of subbands." );
    }
    std::for_each( lateLevels.begin(), lateLevels.end(), DB2Lin<LevelType>() );
    reverbPointSrc.setLateReverbLevels( lateLevels.values( ), lateLevels.size( ) );
    
    std::string const lateAttacksString = lateTree.get<std::string>( "attacktime" );
    pml::FloatSequence<ril::SampleType> const lateAttacks( lateAttacksString );
    if( lateAttacks.size( ) != PointSourceWithReverb::cNumberOfSubBands )
    {
      throw std::invalid_argument( "The number of elements in the \"lreverb.late\" attribute must match the fixed number of subbands." );
    }
    reverbPointSrc.setLateReverbAttackTimes(lateAttacks.values( ), lateAttacks.size( ) );
    
    std::string const lateDecaysString = lateTree.get<std::string>( "decayconst" );
    pml::FloatSequence<ril::SampleType> const lateDecays( lateDecaysString );
    if( lateDecays.size( ) != PointSourceWithReverb::cNumberOfSubBands )
    {
      throw std::invalid_argument( "The number of elements in the \"lreverb.late\" attribute must match the fixed number of subbands." );
    }
    reverbPointSrc.setLateReverbDecayCoeffs( lateDecays.values( ), lateDecays.size( ) );
  }
  // TODO: distinguish between boost property_tree parse errors and bad dynamic casts.
  catch( std::exception const & ex )
  {
    throw std::invalid_argument( std::string( "Error while parsing point source with reverb: object: ") + ex.what() );
  }
}

/*virtual*/ void PointSourceWithReverbParser::
write( Object const & obj, boost::property_tree::ptree & tree ) const
{
  using boost::property_tree::ptree;

  // note: cannot check for object type id since obj might be a subclass of PointSource
  PointSourceWithReverb const& pswdObj = dynamic_cast<PointSourceWithReverb const&>(obj);

  PointSourceParser::write( obj, tree );
  ptree roomTree;
  std::size_t const numEarlies = pswdObj.numberOfDiscreteReflections();
  for( std::size_t earlyIdx( 0 ); earlyIdx < numEarlies; ++earlyIdx )
  {
    PointSourceWithReverb::DiscreteReflection const & refl = pswdObj.discreteReflection( earlyIdx );
    ptree earlyTree;
    earlyTree.put<ril::SampleType>( "delay", refl.delay() );
    earlyTree.put<LevelType>( "level", refl.level() );

    ptree biquadTree;
    refl.reflectionFilters().writeJson( biquadTree );
    earlyTree.add_child( "biquadsos", biquadTree );

    roomTree.add_child( "ereflect", earlyTree );
  }
  ptree lateTree;

  lateTree.put<ril::SampleType>( "delay", pswdObj.lateReverbOnset() );
  PointSourceWithReverb::LateReverbCoeffs const & lateLevels = pswdObj.lateReverbLevels( );
  pml::FloatSequence<ril::SampleType> lateLevelCoeffs( &lateLevels[0], lateLevels.size( ) );
  std::for_each( lateLevelCoeffs.begin( ), lateLevelCoeffs.end( ), Lin2dB<LevelType>( ) );
  std::string const lateLevelStr( lateLevelCoeffs.toString( ", " ) );
  lateTree.put<std::string>( "levels", lateLevelStr );

  PointSourceWithReverb::LateReverbCoeffs const & lateDecays = pswdObj.lateReverbDecayCoeffs( );
  pml::FloatSequence<ril::SampleType> lateDecayCoeffs( &lateDecays[0], lateDecays.size( ) );
  std::string const lateDecayStr( lateDecayCoeffs.toString( ", " ) );
  lateTree.put<std::string>( "decayconst", lateDecayStr );

  roomTree.put_child( "lreverb", lateTree );
  tree.put_child( "room", roomTree );
}

} // namespace objectmodel
} // namespace visr
