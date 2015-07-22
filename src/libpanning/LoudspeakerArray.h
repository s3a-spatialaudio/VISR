//
//  LoudspeakerArray.h
//
//  Created by Dylan Menzies on 18/11/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//


#ifndef __S3A_renderer_dsp__LoudspeakerArray__


#define __S3A_renderer_dsp__LoudspeakerArray__

#include "defs.h"
#include "XYZ.h"

#include <libefl/basic_matrix.hpp>
#include <libefl/basic_vector.hpp>

#include <array>
#include <string>
#include <vector>

namespace visr
{
namespace panning
{

  class LoudspeakerArray
  {
  public:
    /**
     * Type for specifying loudspeaker indices in triplets.
     * At the moment, we use 'int' to let negative numbers denote invalid/unused triplets.
     */
    using LoudspeakerIndexType = int;

    using TripletType = std::array<LoudspeakerIndexType, 3>;

    using ChannelIndex = int;
  private:

    bool m_is2D, m_isInfinite;

    std::vector<XYZ> m_position;

    std::vector< TripletType > m_triplet;

    std::vector<ChannelIndex> m_channel;
  public:
    /**
     * Default contructor, initialises numbers of elements and channel indices to safe values.
     */
    LoudspeakerArray();

    int load( FILE *file );

    /**
     *
     */
    void loadXml( std::string const & filePath );

    int setPosition( int id, Afloat x, Afloat y, Afloat z, bool inf )
    {
      m_position[id - 1].set( x, y, z, inf );
      return 0;
    };

    int setChannel( int id, int chan )
    {
      m_channel[id - 1] = chan;
      return 0;
    };

    /**
     * TODO: Consider using references!
     */
    XYZ & getPosition( int iSpk ) { return m_position[iSpk]; };

    XYZ const & getPosition( int iSpk ) const { return m_position[iSpk]; };

    XYZ* getPositions() { return m_position.data(); };

    XYZ const * getPositions() const { return m_position.data(); };

    ChannelIndex channelIndex( std::size_t spkIndex ) const { return m_channel[spkIndex]; }

  int setTriplet( int iTri, int l1, int l2, int l3 )
  {
    m_triplet[ iTri ][ 0 ] = l1;
    m_triplet[ iTri ][ 1 ] = l2;
    m_triplet[ iTri ][ 2 ] = l3;
    return 0;
  }

  TripletType & getTriplet( std::size_t iTri ) { return m_triplet[ iTri ]; }

  TripletType const & getTriplet( std::size_t iTri ) const { return m_triplet[ iTri ]; }

  std::size_t getNumSpeakers() const { return m_position.size(); };

  std::size_t getNumTriplets( ) const { return m_triplet.size(); };

  bool is2D() const { return m_is2D; };
  bool isInfinite() const { return m_isInfinite; };

};

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
