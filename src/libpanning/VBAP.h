//
//  VBAP.h
//
//  Created by Dylan Menzies on 10/11/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

#ifndef __S3A_renderer_dsp__VBAP__
#define __S3A_renderer_dsp__VBAP__

// uncomment to enable debug output
// #define VBAP_DEBUG_MESSAGES

#include "defs.h"
#include "LoudspeakerArray.h"

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace panning
{
static void normalise(std::vector<SampleType> &gains);

class VBAP
{
private:
    
   // LoudspeakerArray const * m_array;

    efl::BasicMatrix<SampleType> mInvMatrix;
	efl::BasicMatrix<SampleType> mPositions;
	efl::BasicMatrix<SampleType> mReroutingMatrix;

	std::vector<LoudspeakerArray::TripletType> mTriplets;
	bool is2D;
	bool isInfinite;
	std::array<SampleType, 3> mListenerPos;
	std::vector<SampleType> mGain;
	/*XYZ ;
	
    XYZ const * m_sourcePos;
    std::size_t m_nSources;
 
    */
	int calcInvMatrices();
	void calcPlainVBAP(XYZ pos);
	void applyRerouting();
	
	
    
public:
  /**
   * Default constructor.
   * Sets object to save values (no array, zero sources).
   */
 // VBAP();

	explicit VBAP(const LoudspeakerArray &array, SampleType x, SampleType y, SampleType z);
	
	void calcGain(XYZ pos);
	
	
	std::size_t getNumSpeakers() const {
		return mPositions.numberOfRows();
	}

	
	std::vector<SampleType> const & getGains() const {
		return mGain;
	}
	
//    int setLoudspeakerArray(LoudspeakerArray const * array){
//      //  m_array = array;
//       // m_invMatrix.resize( array->getNumTriplets(), 9 );
//      //  m_gain.resize( m_nSources, array->getNumSpeakers( ) );
//        return 0;
//    }
//	//
//    int setListenerPosition(SampleType x, SampleType y, SampleType z){
//
//#ifdef VBAP_DEBUG_MESSAGES
//      printf("setListenerPosition %f %f %f\n",x,y,z);
//#endif
//
//        m_listenerPos.set(x, y, z);
//
//        return 0;
//    }


    //private
   
    //
 //   int setSourcePositions(XYZ const *sp ) {
 //       m_sourcePos = sp;
 //       return 0;
 //   }
 //   
 //   std::size_t setNumSources( std::size_t n) {
 //       m_nSources = n;
	//// Take care of the fact that the loudspeaker
	//// array might not been set yet.
	//	//std::size_t const numSpeakers = m_array ? m_array->getNumSpeakers() : 0;
 //      // m_gain.resize( m_nSources, numSpeakers );
 //       
	//	m_gain.resize(m_nSources, mPositions.numberOfRows());

	//	return n;
 //   }
    
 
    
   
 
};

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__VBAP__) */
