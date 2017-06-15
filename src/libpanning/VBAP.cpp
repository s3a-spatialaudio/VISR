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
    
    
    numTotLoudspeakers = lsarray.getNumSpeakers();
    numRegLoudspeakers = lsarray.getNumRegularSpeakers();
    numVirtLoudspeakers = numTotLoudspeakers - numRegLoudspeakers;
    numTriplets = lsarray.getNumTriplets();
    
    is2D = lsarray.is2D();
    isInfinite = lsarray.isInfinite();
    mInvMatrix.resize( numTriplets, 9 );
    mPositions.resize( numTotLoudspeakers, 3 );
    mGain.resize( numTotLoudspeakers );
    mTriplets.resize( numTriplets );
    mReroutingMatrix.resize( numVirtLoudspeakers, numRegLoudspeakers );
    
    
    //mPositions
    for( int i = 0; i < numTotLoudspeakers; i++ )
    {
      mPositions( i, 0 ) = lsarray.getPosition( i ).x;
      mPositions( i, 1 ) = lsarray.getPosition( i ).y;
      mPositions( i, 2 ) = lsarray.getPosition( i ).z;
    }
    
    for( int i = 0; i < numTriplets; i++ )
    {
      mTriplets[i][0] = lsarray.getTriplet( i )[0];
      mTriplets[i][1] = lsarray.getTriplet( i )[1];
      mTriplets[i][2] = lsarray.getTriplet( i )[2];
    }
    mListenerPos[0] = x;
    mListenerPos[1] = y;
    mListenerPos[2] = z;
    
    for( int i = 0; i < numVirtLoudspeakers; i++ )
    {
      for( int j = 0; j < numRegLoudspeakers; j++ )
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
        throw std::runtime_error( "VBAP: error during power normalisation." );
      }
    }
    void normalise( SampleType &a1, SampleType &a2, SampleType &a3)
    {
      Afloat l = std::sqrt(a1*a1 + a2*a2 + a3*a3);
      if (l != 0.0f ) {
        a1 /= l;
        a2 /= l;
        a3 /= l;
      }
      
    }
  } // unnamed namespace
  
  void VBAP::calculateGains( SampleType x, SampleType y, SampleType z, SampleType * gains ) const
  {
    calcPlainVBAP( x, y, z);
    applyRerouting();
    powerNormalisation( &mGain[0], gains, numRegLoudspeakers );
  }
  
  void VBAP::setListenerPosition( SampleType x, SampleType y, SampleType z )
  {
    mListenerPos = std::array<SampleType, 3>{{x,y,z}};
    calcInvMatrices();
  }
  
  
  
  int VBAP::calcInvMatrices()
  {
    
    
#ifdef VBAP_DEBUG_MESSAGES
    printf( "calcInvMatrices()\n" );   //!
#endif
    
    SampleType l1X, l1Y, l1Z,
    l2X, l2Y, l2Z,
    l3X, l3Y, l3Z;
    SampleType det, temp;
    SampleType* inv;
    for( std::size_t i = 0; i < mTriplets.size(); i++ )
    {
      
      LoudspeakerArray::TripletType const & triplet = mTriplets[i];
     
      
      l1X = mPositions( triplet[0], 0 );
      l1Y = mPositions( triplet[0], 1 );
      l1Z = mPositions( triplet[0], 2 );
      
      l2X = mPositions( triplet[1], 0 );
      l2Y = mPositions( triplet[1], 1 );
      l2Z = mPositions( triplet[1], 2 );
      
      if( is2D )
      {
        l3X = 0; l3Y = 0; l3Z = 1;
      }  // adapt 3D calc for 2D array
      else
      {
        l3X = mPositions( triplet[2], 0 );
        l3Y = mPositions( triplet[2], 1 );
        l3Z = mPositions( triplet[2], 2 );
      }
      
      // calc speaker positions relative to listener.
      l1X = l1X - mListenerPos[0];
      l1Y = l1Y - mListenerPos[1];
      l1Z = l1Z - mListenerPos[2];
      l2X = l2X - mListenerPos[0];
      l2Y = l2Y - mListenerPos[1];
      l2Z = l2Z - mListenerPos[2];
      l3X = l3X - mListenerPos[0];
      l3Y = l3Y - mListenerPos[1];
      l3Z = l3Z - mListenerPos[2];
      
      normalise(l1X,l1Y,l1Z);
      normalise(l2X,l2Y,l2Z);
      normalise(l3X,l3Y,l3Z);
      
      temp = (l1X * ((l2Y * l3Z) - (l2Z * l3Y))
              - l1Y * ((l2X * l3Z) - (l2Z * l3X))
              + l1Z * ((l2X * l3Y) - (l2Y * l3X)));
      
      if( temp != 0.0f ) det = 1.0f / temp;
      else det = 1.0f;
      
      inv = mInvMatrix.row( i );
      inv[0] = ((l2Y * l3Z) - (l2Z * l3Y)) * det;
      inv[3] = ((l1Y * l3Z) - (l1Z * l3Y)) * -det;
      inv[6] = ((l1Y * l2Z) - (l1Z * l2Y)) * det;
      inv[1] = ((l2X * l3Z) - (l2Z * l3X)) * -det;
      inv[4] = ((l1X * l3Z) - (l1Z * l3X)) * det;
      inv[7] = ((l1X * l2Z) - (l1Z * l2X)) * -det;
      inv[2] = ((l2X * l3Y) - (l2Y * l3X)) * det;
      inv[5] = ((l1X * l3Y) - (l1Y * l3X)) * -det;
      inv[8] = ((l1X * l2Y) - (l1Y * l2X)) * det;
    }
    return 0;
  }
  
  
  void VBAP::calcPlainVBAP( SampleType posX, SampleType posY, SampleType posZ ) const
  {
    
    
    std::fill( mGain.begin(), mGain.end(), 0.0f );
    
    //! Slow triplet search
    // Find triplet with highest minimum-gain-in-triplet (may be negative)
    
    
    SampleType  gmin, g1min, g2min, g3min;
    SampleType x, y, z;
    int invalid = std::numeric_limits<int>::max();
    int jmin = invalid;// indicate currently no triplet candidate.
    
#ifdef VBAP_DEBUG_MESSAGES
    printf( "setListenerPosition %f %f %f\n", mListenerPos[0], mListenerPos[1], mListenerPos[2] );
#endif
    gmin = g1min = g2min = g3min = 0.0;
    
    x = posX;
    y = posY;
    z = posZ;
    
    if( isInfinite )
    {
      x -= mListenerPos[0];
      y -= mListenerPos[1];
      z -= mListenerPos[2];
    }
    if( is2D ) z = 0; //! temp fix. no fade from 2D plane.
    
    int l1, l2, l3;
    SampleType g1, g2, g3;
    for( std::size_t j = 0; j < mTriplets.size(); j++ )
    {
      
      
      
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
         (jmin == invalid || (g1 > gmin && g2 > gmin && g3 > gmin))
         )
      {
        jmin = j;
        g1min = g1; g2min = g2; g3min = g3;
        gmin = g1;
        if( g2 < gmin ) { gmin = g2; }
        if( g3 < gmin ) { gmin = g3; }
      }
    }
    
    if( jmin == invalid )
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
    for( int i = 0; i < numRegLoudspeakers; i++ )
    {
      for( int j = 0; j < numVirtLoudspeakers; j++ )
      {
        mGain[i] = mGain[i] + mReroutingMatrix( j, i ) * mGain[numRegLoudspeakers + j];
      }
    }
  }
  
 
} // namespace panning
} // namespace visr
