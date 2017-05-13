//
//  VBAP.cpp
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "VBAP.h"

#include <cmath>
#include <numeric>
#include <limits>

namespace visr
{
  namespace panning
  {

    VBAP::VBAP( const LoudspeakerArray &lsarray, SampleType x, SampleType y, SampleType z )
    {
      is2D = lsarray.is2D();
      isInfinite = lsarray.isInfinite();
      mInvMatrix.resize( lsarray.getNumTriplets(), 9 );
      mPositions.resize( lsarray.getNumSpeakers(), 3 );
      mGain.resize( lsarray.getNumSpeakers() );
      mTriplets.resize( lsarray.getNumTriplets() );
      mReroutingMatrix.resize( lsarray.getNumSpeakers() - lsarray.getNumRegularSpeakers(), lsarray.getNumRegularSpeakers() );
      //mPositions 
      for( int i = 0; i < lsarray.getNumSpeakers(); i++ )
      {
        mPositions( i, 0 ) = lsarray.getPosition( i ).x;
        mPositions( i, 1 ) = lsarray.getPosition( i ).y;
        mPositions( i, 2 ) = lsarray.getPosition( i ).z;
      }

      for( int i = 0; i < lsarray.getNumTriplets(); i++ )
      {
        mTriplets[i][0] = lsarray.getTriplet( i )[0];
        mTriplets[i][1] = lsarray.getTriplet( i )[1];
        mTriplets[i][2] = lsarray.getTriplet( i )[2];
      }
      mListenerPos[0] = x;
      mListenerPos[1] = y;
      mListenerPos[2] = z;

      for( int i = 0; i < mReroutingMatrix.numberOfRows(); i++ )
      {
        for( int j = 0; j < mReroutingMatrix.numberOfColumns(); j++ )
        {
          mReroutingMatrix( i, j ) = lsarray.getReroutingCoefficient( i, j );
        }
      }

      calcInvMatrices();
    }

    namespace
    {
      void powerNormalisation( SampleType const * in, SampleType * out, std::size_t numberOfElements )
      {
        SampleType const sigPower = std::accumulate( in, in + numberOfElements, 0.0f,
          []( SampleType acc, SampleType val ) { return acc + val*val; } );
        SampleType const normFactor = 1.0f / std::max( std::sqrt( sigPower ), std::numeric_limits<SampleType>::epsilon() );
        efl::ErrorCode res = efl::vectorMultiplyConstant( normFactor, in, out, numberOfElements, 0/*alignment*/ );
        if( res != efl::noError )
        {
          throw std::runtime_error( "VBAP: error diuring power normalisation." );
        }
      }
    } // unnamed namespace

    void VBAP::calculateGains( SampleType x, SampleType y, SampleType z, SampleType * gains ) const
    {
      calcPlainVBAP( XYZ( x, y, z ) );
      applyRerouting();
      powerNormalisation( &mGain[0], gains, getNumSpeakers() );
    }

    void VBAP::setListenerPosition( SampleType x, SampleType y, SampleType z )
    {
      mListenerPos = std::array<SampleType, 3>{ x, y, z };
      calcInvMatrices();
    }



    int VBAP::calcInvMatrices()
    {
      XYZ l1, l2, l3;
      SampleType det, temp;
      SampleType* inv;

#ifdef VBAP_DEBUG_MESSAGES
      printf( "calcInvMatrices()\n" );   //!
#endif

      for( std::size_t i = 0; i < mTriplets.size(); i++ )
      {

        LoudspeakerArray::TripletType const & triplet = mTriplets[i];
        if( triplet[0] == -1 ) continue;  //  triplet unused

        l1.x = mPositions( triplet[0], 0 );
        l1.y = mPositions( triplet[0], 1 );
        l1.z = mPositions( triplet[0], 2 );

        l2.x = mPositions( triplet[1], 0 );
        l2.y = mPositions( triplet[1], 1 );
        l2.z = mPositions( triplet[1], 2 );

        if( is2D )
        {
          l3.x = 0; l3.y = 0; l3.z = 1;
        }  // adapt 3D calc for 2D array
        else
        {
          l3.x = mPositions( triplet[2], 0 );
          l3.y = mPositions( triplet[2], 1 );
          l3.z = mPositions( triplet[2], 2 );
        }

        // calc speaker positions relative to listener.
        l1.x = l1.x - mListenerPos[0];
        l1.y = l1.y - mListenerPos[1];
        l1.z = l1.z - mListenerPos[2];
        l2.x = l2.x - mListenerPos[0];
        l2.y = l2.y - mListenerPos[1];
        l2.z = l2.z - mListenerPos[2];
        l3.x = l3.x - mListenerPos[0];
        l3.y = l3.y - mListenerPos[1];
        l3.z = l3.z - mListenerPos[2];

        l1.normalise();
        l2.normalise();
        l3.normalise();

        temp = (l1.x * ((l2.y * l3.z) - (l2.z * l3.y))
          - l1.y * ((l2.x * l3.z) - (l2.z * l3.x))
          + l1.z * ((l2.x * l3.y) - (l2.y * l3.x)));

        if( temp != 0.0f ) det = 1.0f / temp;
        else det = 1.0f;

        inv = mInvMatrix.row( i );
        inv[0] = ((l2.y * l3.z) - (l2.z * l3.y)) * det;
        inv[3] = ((l1.y * l3.z) - (l1.z * l3.y)) * -det;
        inv[6] = ((l1.y * l2.z) - (l1.z * l2.y)) * det;
        inv[1] = ((l2.x * l3.z) - (l2.z * l3.x)) * -det;
        inv[4] = ((l1.x * l3.z) - (l1.z * l3.x)) * det;
        inv[7] = ((l1.x * l2.z) - (l1.z * l2.x)) * -det;
        inv[2] = ((l2.x * l3.y) - (l2.y * l3.x)) * det;
        inv[5] = ((l1.x * l3.y) - (l1.y * l3.x)) * -det;
        inv[8] = ((l1.x * l2.y) - (l1.y * l2.x)) * det;
      }
      return 0;
    }


    void VBAP::calcPlainVBAP( XYZ pos ) const
    {
      int jmin, l1, l2, l3;
      SampleType g1, g2, g3, g, gmin, g1min, g2min, g3min, x, y, z;
      std::fill( mGain.begin(), mGain.end(), 0.0f );

      //! Slow triplet search
      // Find triplet with highest minimum-gain-in-triplet (may be negative)

      jmin = -1; // indicate currently no triplet candidate.
#ifdef VBAP_DEBUG_MESSAGES
      printf( "setListenerPosition %f %f %f\n", mListenerPos.x, mListenerPos.y, mListenerPos.z );
#endif
      gmin = g1min = g2min = g3min = 0.0;

      x = pos.x;
      y = pos.y;
      z = pos.z;

      if( isInfinite )
      {
        x -= mListenerPos[0];
        y -= mListenerPos[1];
        z -= mListenerPos[2];
      }
      if( is2D ) z = 0; //! temp fix. no fade from 2D plane.
      for( std::size_t j = 0; j < mTriplets.size(); j++ )
      {

        if( mTriplets[j][0] == -1 ) continue;  //  triplet unused

        SampleType const * inv = mInvMatrix.row( j );
        g1 = x*inv[0] + y*inv[1] + z*inv[2];
        g2 = x*inv[3] + y*inv[4] + z*inv[5];
        g3 = x*inv[6] + y*inv[7] + z*inv[8];

        if( g1 >= 0 && g2 >= 0 && (g3 >= 0 || is2D) )  // inside triplet, or edge in 2D case.
        {
          jmin = j;
          g1min = g1; g2min = g2; g3min = g3;
          break;  // should only be at most one triplet with all positive gains.
        }

        // Update gmin if lowest gain in triplet is higher than gmin.
        // Triplet must have at most 1 -ve gain.
        //! failure possible: if panning outside a 'naked corner' / large z swapping to other triangles.
        //! better geometric solution needed.
        //! 'dead'-speakers provide a posssible solution.

        if( ((g1 < 0) + (g2 < 0) + (g3 < 0) <= 1) &&   // at most one negative gain
          (jmin == -1 || (g1 > gmin && g2 > gmin && g3 > gmin))
          )
        {
          jmin = j;
          g1min = g1; g2min = g2; g3min = g3;
          gmin = g1;
          if( g2 < gmin ) { gmin = g2; }
          if( g3 < gmin ) { gmin = g3; }
        }
      }

      if( jmin == -1 )
      {
        throw std::runtime_error( "calcPlainVBAP: no triplet with a correct gain." );
      }
      // gains of triplet with highest minimum-gain-in-triplet
      g1 = g1min; g2 = g2min; g3 = g3min;

      // in 2D case g3 != 0 when source is out of 2D plane.
      // Normalization causes fade with distance from plane unless g3 set to 0 first.
      //if (m_array->is2D()) g3 = 0;

      l1 = mTriplets[jmin][0];
      l2 = mTriplets[jmin][1];
      l3 = mTriplets[jmin][2];
     // std::cout << "INDX1: " << l1 << "VAL: "<< mGain[l1] << "INDX2: " << l2 << "VAL: " << mGain[l2] << "INDX3: " << l3 << "VAL: " << mGain[l3]<<std::endl;
      //std::cout << "INDX1: " << l1 << "INDX2: " << l2 <<  "INDX3: " << l3 << std::endl;

      mGain[l1] = g1;
      mGain[l2] = g2;
      if( !is2D )  mGain[l3] = g3;     // l3 undefined in 2D case 

#ifdef VBAP_DEBUG_MESSAGES
      printf( "%d  %f %f %f   %d  %d %d %d  %f %f %f \n", i, x, y, z, jmin, l1, l2, l3, g1, g2, g3 );
#endif

    }

    void VBAP::applyRerouting() const
    {
      size_t numRegularSpeakers = mReroutingMatrix.numberOfColumns();
      size_t numVirtualSpeakers = getNumSpeakers() - numRegularSpeakers;
      for( int i = 0; i < numRegularSpeakers; i++ )
      {
        for( int j = 0; j < numVirtualSpeakers; j++ )
        {
          mGain[i] = mGain[i] + mReroutingMatrix( j, i ) * mGain[numRegularSpeakers + j];
        }
      }
    }

    void normalise( std::vector<SampleType> &gains )
    {
      SampleType g;
      for( size_t i = 0; i < gains.size(); i++ )
      {
        g += std::pow( gains[i], 2 );
      }

      g = sqrt( g );
      // g = g1+g2+g3; //! more appropriate for low freq sounds / close speakers.
      if( g > 0 )
      {
        for( size_t i = 0; i < gains.size(); i++ )
        {
          gains[i] /= g;
        }
      }

      // Remove -ve gain if present, moves image to edge of triplet.
      // after normalization: gains fade with distance from boundary edge.
      // before normalization: gain fixed whether on or off a boundary edge.
      for( size_t i = 0; i < gains.size(); i++ )
      {
        if( gains[i] < 0 ) gains[i] = 0;
      }
    }
  } // namespace panning
} // namespace visr
