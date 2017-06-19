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

#include <libril/constants.hpp>

#include <array>
#include <string>
#include <vector>
#include <map>

namespace visr
{

  // Forward declaration
namespace pml
{
  template<typename CoeffType>
  class BiquadParameterMatrix;
}

namespace panning
{

  class LoudspeakerArray
  {
 
    public:
      /**
       * Type for specifying loudspeaker indices in triplets.
       * At the moment, we use 'int' to let negative numbers denote invalid/unused triplets.
       */
      using LoudspeakerIndexType = std::size_t;
      using SampleType = visr::SampleType;
      using TripletType = std::array<LoudspeakerIndexType, 3>;
      using ChannelIndex = std::size_t;

    public:
      /**
       * Default contructor, initialises numbers of elements and channel indices to safe values.
       */
      LoudspeakerArray();

      /**
       * Construct an initialised array configuration.
       * @throw std::invalid_argument if the file \p xmlConfiguration is not found or inconsistent.
       */
      explicit LoudspeakerArray( std::string const & xmlConfiguration );

      /**
       * Assignment operator
       * We need an explicitly defined assignment operator because some members (BasicMatrix)
       * intentionally do not have a copy constructor.
       */
      LoudspeakerArray const & operator=( LoudspeakerArray const & rhs );

      /**
       * Destructor.
       * @note We need to provide a destructor implementation because we use
       * a unique_ptr to the forward-declared class BiquadParameterMatrix.
       */
      ~LoudspeakerArray();

      std::size_t load( FILE *file );

      /**
       *
       */
      void loadXmlFile( std::string const & filePath );

      void loadXmlStream( std::istream & stream );
      void loadXmlString( std::string xmlString );

      /**
       * Return the position of a loudspeaker based on its sorted position.
       */
      XYZ & getPosition( std::size_t iSpk ) { return m_position[iSpk]; };

      XYZ const & getPosition( std::size_t iSpk ) const { return m_position[iSpk]; };

      /**
      * Return the position of a loudspeaker given its string id
      *
      */
      XYZ & getPosition( std::string id ) { return m_position[m_id.at( id )]; };

      XYZ const & getPosition( std::string id ) const { return m_position[m_id.at( id )]; };



      XYZ* getPositions() { return m_position.data(); };

      XYZ const * getPositions() const { return m_position.data(); };



      /**
       * Return the loudspeaker index associated with a loudspeaker at a given
       * position in the loudspeaker array.
       * At the moment, the loudspeaker indices are integers consecutively numbered from 1.
       * That means that there is a fixed relation between the two indices.
       * @todo Allow arbitrary indices (or labels), maybe of different types.
       */
      //LoudspeakerIndexType getLoudspeakerIndex( std::size_t arrayIndex ) const;
      ChannelIndex channelIndex( std::size_t spkIndex ) const { return m_channel[spkIndex]; }

      LoudspeakerIndexType getSpeakerIndexFromId( std::string id ) const { return m_id.at(id); };
      LoudspeakerIndexType getSpeakerIndexFromChn( ChannelIndex chn ) const { return m_id.at(mIdsOrder.at(chn)); };

      ChannelIndex getSpeakerChannel( std::size_t spkIndex ) const { return m_channel[spkIndex]; }
      ChannelIndex getSpeakerChannelFromId( std::string id ) const { return m_channel[m_id.at( id )]; }


      std::size_t setTriplet( std::size_t iTri, std::size_t l1, std::size_t l2, std::size_t l3 )
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

      ChannelIndex const * getLoudspeakerChannels() const { return &m_channel[0]; }

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
      * Virtual speaker rerouting matrix configuration support.
      *
      */
      //@{


      /**
      * Retrieve the matrix of rereouting coefficients from virtual to real loudspeakers
      * @return Reference to rereouting matrix, dimension number of virtual loudspeakers * number of physical loudspeakers.
      */
      efl::BasicMatrix<SampleType> const & getReroutingCoefficients() const { return m_reRoutingCoeff; }

      /**
      * Retrieve the rereouting coefficient from a virtual to a real loudspeaker
      * @param virtIdx virtual loudspeaker index (zero-offset)
      * @param realIdx real loudspeaker index (zero-offset) This is the logical speaker index, not the channel index used for routing.
      * @return Rerouting coefficient as a linear-scale gain value (not incorporating gain corrections applied to the channel signal).
      * @throw std::out_of_range if either \p virtIdx or \p realIdx is out of the respective admissible range.
      */
      Afloat getReroutingCoefficient( std::size_t virtIdx, std::size_t realIdx ) const { return m_reRoutingCoeff.at( virtIdx, realIdx ); }
      //@}



        /**
         * Gain and delay adjustments.
         */
         //@{
      Afloat getLoudspeakerGainAdjustment( std::size_t spkIdx ) const { return m_gainAdjustment.at( spkIdx ); };

      Afloat getLoudspeakerDelayAdjustment( std::size_t spkIdx ) const { return m_delayAdjustment.at( spkIdx ); }

      Afloat getSubwooferGainAdjustment( std::size_t spkIdx ) const { return m_gainAdjustment.at( spkIdx + getNumRegularSpeakers() ); };

      Afloat getSubwooferDelayAdjustment( std::size_t spkIdx ) const { return m_delayAdjustment.at( spkIdx + getNumRegularSpeakers() ); }

      /**
       * Return the gain adjustment vector (linear scale) holding the gains for all regular loudspeakers and subwoofers.
       * The loudspeaker gains are ordered according to their zero-offset logical speaker numbers, followed by the subwoofers
       * in their order of definition in the configuration file.
       */
      efl::BasicVector<Afloat> const & getGainAdjustment() const
      {
        return m_gainAdjustment;
      }

      /**
      * Return the delay adjustment vector (in seconds) holding the delays for all regular loudspeakers and subwoofers.
      * The loudspeaker delays are ordered according to their zero-offset logical speaker numbers, followed by the subwoofers
      * in their order of definition in the configuration file.
      */
      efl::BasicVector<Afloat> const & getDelayAdjustment() const { return m_delayAdjustment; }

      //@}

      /**
       * Optional support for equalization of ouput channels.
       */
       //@{

       /**
        * Query if the array configuration contained an output equalisation configuration.
        * Technically, that means that the XML file contains a \b outputEqConfiguration section.
        */
      bool outputEqualisationPresent() const;

      /**
       * Query the number of biquads per output channel.
       * Returns 0 if no equalisation configuration is present.
       */
      std::size_t outputEqualisationNumberOfBiquads() const;

      /**
       * Return a matrix containing the biquad parameters for all output sections.
       * The dimension of the matrix is
       * \p (numRegularSpeakers()+getNumSubwoofers()) x outputEqualisationNumberOfBiquads()
       * @throw std::logic_error if no output EQ configuration is present, i.e., outputEqualisationPresent() is \b false.
       */
      pml::BiquadParameterMatrix<Afloat> const & outputEqualisationBiquads() const;
      //@}

    private:

      std::size_t setPosition( std::size_t id, Afloat x, Afloat y, Afloat z, bool inf )
      {
        m_position[id - 1].set( x, y, z, inf );
        return 0;
      };

      std::size_t setChannel( std::size_t id, std::size_t chan )
      {
        m_channel[id - 1] = chan;
        return 0;
      };

      bool m_is2D, m_isInfinite;

      std::vector<XYZ> m_position;

      std::vector< TripletType > m_triplet;

      std::vector<ChannelIndex> m_channel;
      std::map<std::string, LoudspeakerIndexType> m_id;
      std::map<ChannelIndex,std::string> mIdsOrder;
      /**
      * Subwoofer configuration support.
      */
      //@{
      std::vector<ChannelIndex> m_subwooferChannels;

      efl::BasicMatrix<Afloat> m_subwooferGains;

      efl::BasicMatrix<SampleType> m_reRoutingCoeff;
      //@}

      /**
       * Vector holding the gain adjustments for all loudspeakers.
       * Size: number of regular loudspeakers + number of subwoofers.
       * The gains are stored in order of their zero-offset logical loudspeaker index (not output signal index).
       * The subwoofer gains are after the regular speaker gains.
       */
      efl::BasicVector<Afloat> m_gainAdjustment;

      efl::BasicVector<Afloat> m_delayAdjustment;

      std::unique_ptr<pml::BiquadParameterMatrix<Afloat> > mOutputEqs;
  };

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__LoudspeakerArray__) */
