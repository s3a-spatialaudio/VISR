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
      m_triplet[iTri][0] = l1;
      m_triplet[iTri][1] = l2;
      m_triplet[iTri][2] = l3;
      return 0;
    }

    TripletType & getTriplet( std::size_t iTri ) { return m_triplet[iTri]; }

    TripletType const & getTriplet( std::size_t iTri ) const { return m_triplet[iTri]; }

    std::size_t getNumSpeakers() const { return m_position.size(); };

    std::size_t getNumTriplets() const { return m_triplet.size(); };

    bool is2D() const { return m_is2D; };
    bool isInfinite() const { return m_isInfinite; };

    /**
     * Subwoofer configuration support.
     * Supported only by the XML configuration format.
     */
    //@{
    std::size_t getNumSubwoofers() const { return m_subwooferGains.numberOfRows(); }

    ChannelIndex const * getSubwooferChannels() const { return &m_subwooferChannels[0]; }

    ChannelIndex getSubwooferChannel( std::size_t subIdx ) const { return m_subwooferChannels.at( subIdx ); }

    /**
     * Retrieve the gain matrix for panning loudspeaker signals into the subwoofers.
     * @return Reference to gain matrix, dimension number of subwoofers * number of physical loudspeakers.
     */
    efl::BasicMatrix<Afloat> const & getSubwooferGains() const { return m_subwooferGains; }

    /**
     * Retrieve the matrix gain from a given loudspeaker to a given subwoofer.
     * @param subIdx Subwoofer index (zero-offset)
     * @param spkIdx Loudspeaker index (zero-offset) This is the logical speaker index, not the channel index used for routing.
     * @return Linear-scale gain value (not incorporating gain corrections applied to the channel signal).
     * @throw std::out_of_range if either \p subIdx or \p spkIdx is out of the respective admissible range.
     */
    Afloat getSubwooferGain( std::size_t subIdx, std::size_t spkIdx ) const { return m_subwooferGains.at( subIdx, spkIdx ); }

    //@}
  private:

    bool m_is2D, m_isInfinite;

    std::vector<XYZ> m_position;

    std::vector< TripletType > m_triplet;

    std::vector<ChannelIndex> m_channel;

    /**
    * Subwoofer configuration support.
    */
    //@{
    std::vector<ChannelIndex> m_subwooferChannels;

    efl::BasicMatrix<Afloat> m_subwooferGains;
    //@}
  };

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
