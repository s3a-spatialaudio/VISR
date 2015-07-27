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

    /**
     * Assignment
     * We need an explicitly defined asssignment operator because some members (BasicMatrix) 
     * intentionally do not have a copy constructor.
     */
    LoudspeakerArray const & operator=( LoudspeakerArray const & rhs );

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
    XYZ & getPosition( std::size_t iSpk ) { return m_position[iSpk]; };

    XYZ const & getPosition( std::size_t iSpk ) const { return m_position[iSpk]; };

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

    /**
     * Return the total number of loudspeakers, including virtual speaker positions.
     * Subwoofers are not considered as regular louspeakers.
     */
    std::size_t getNumSpeakers() const { return m_position.size(); };

    /**
     * Retrieve the number of regular loudspeakers, excluding virtual loudspeakers and subwoofers.
     */
    std::size_t getNumRegularSpeakers() const { return m_channel.size(); }

    std::size_t getNumTriplets() const { return m_triplet.size(); };

    bool is2D() const { return m_is2D; };
    bool isInfinite() const { return m_isInfinite; };

    /**
     * Subwoofer configuration support.
     * Supported only by the XML configuration format.
     */
    //@{

    /**
     * Return the number of subwoofers.
     */
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

    /**
     * Gain and delay adjustments.
     */
    //@{
    Afloat getLoudspeakerGainAdjustment( std::size_t spkIdx ) const { return m_gainAdjustment.at( spkIdx ); };

    Afloat getLoudspeakerDelayAdjustment( std::size_t spkIdx ) const { return m_delayAdjustment.at( spkIdx ); }

    Afloat getSubwooferGainAdjustment( std::size_t spkIdx ) const { return m_gainAdjustment.at( spkIdx + getNumRegularSpeakers( ) ); };

    Afloat getSubwooferDelayAdjustment( std::size_t spkIdx ) const { return m_delayAdjustment.at( spkIdx + getNumRegularSpeakers()); }

    /**
     * Return the gain adjustment vector (linear scale) holding the gains for all regular loudspeakers and subwoofers.
     * The loudspeaker gains are ordered according to their zero-offset logical speaker numbers, followed by the subwoofers 
     * in their order of definition in the configuration file.
     */
    std::vector<Afloat> const & getGainAdjustment() const { return m_gainAdjustment; }

    /**
    * Return the delay adjustment vector (in seconds) holding the delays for all regular loudspeakers and subwoofers.
    * The loudspeaker delays are ordered according to their zero-offset logical speaker numbers, followed by the subwoofers
    * in their order of definition in the configuration file.
    */
    std::vector<Afloat> const & getDelayAdjustment( ) const { return m_delayAdjustment; }

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

    /**
     * Vector holding the gain adjustments for all loudspeakers.
     * Size: number of regular loudspeakers + number of subwoofers.
     * The gains are stored in order of their zero-offset logical loudspeaker index (not output signal index).
     * The subwoofer gains are after the regular speaker gains.
     */
    std::vector<Afloat> m_gainAdjustment;
    
    std::vector<Afloat> m_delayAdjustment;
  };

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
