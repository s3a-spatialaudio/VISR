//
//  AllRAD.cpp
//
//  Created by Dylan Menzies on 12/12/2014.
// Modified: Andreas Franck 23/09/2015
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

// avoid annoying warning about unsafe STL functions.
#ifdef _MSC_VER 
#pragma warning(disable: 4996)
#endif

#include "AllRAD.h"

#include <libefl/basic_matrix.hpp>

namespace visr
{
namespace panning
{

AllRAD::AllRAD(LoudspeakerArray const & regularArray,
               LoudspeakerArray const & realArray,
               efl::BasicMatrix<Afloat> const & decodeCoeffs,
               unsigned int maxHoaOrder )
 : mRegularDecoder( regularArray )
 , mRealDecoder( realArray )
 , mNumberOfHarmonics( (maxHoaOrder + 1) * (maxHoaOrder + 1) )
 , mRegularDecodeCoefficients( decodeCoeffs.numberOfRows( ), decodeCoeffs.numberOfColumns( ), decodeCoeffs.alignmentElements( ) )
 , mRealDecodeCoefficients( mNumberOfHarmonics, realArray.getNumRegularSpeakers(), decodeCoeffs.alignmentElements() )
 , mRegularArraySize( regularArray.getNumSpeakers( ) )
{
  mRegularDecodeCoefficients.copy( decodeCoeffs );
  updateDecodingCoefficients();
}

#if 0
int AllRAD::loadRegDecodeGains(FILE* file, int order, int nSpks){
    
    int const nHarms = (order+1)*(order+1);

    m_regDecode.resize( nHarms, nSpks );
    m_decode.resize( nHarms, nSpks );

    for( int i = 0; i < nHarms; i++ ) {
        for( int j = 0; j < nSpks; j++ ) {
            if (feof(file) == -1) {
                return -1;
            }
            fscanf(file, "%f", &(m_regDecode(i,j)));
        }
    }
    
    m_nHarms = nHarms;
    m_nSpkSources = nSpks;
    
    return 0;
}
#endif

void AllRAD::setListenerPosition( SampleType x, SampleType y, SampleType z )
{
  mRealDecoder.setListenerPosition( x, y, z );
  updateDecodingCoefficients();
}

void AllRAD::setListenerPosition( XYZ const & newPosition )
{
  setListenerPosition( newPosition.x, newPosition.y, newPosition.z );
}


void AllRAD::updateDecodingCoefficients()
{

}

#if 0
int AllRAD::calcDecodeGains(VBAP* vbap){
    
    // In vbap first do externally: setListenerPosition, then calcInvMatrix.
#if 0
    vbap->setSourcePositions( m_regArray->getPositions()  );
    
    vbap->setNumSources(m_regArray->getNumSpeakers());
    
    //! Make sure the isAtInfinity flag is set for all regArray speakers
    
    vbap->calcGains();
    
    efl::BasicMatrix<Afloat> const & vbapGain = vbap->getGains( );
    
    std::size_t const nSpks = vbap->getNumSpeakers();
    
    // NOTE: The size of the final gain matrix depends on the VBAP
    // configuration, which is not known beforehand. So the gain
    // matrix has to be resized within this call.
    // TODO: Change interface either to make the configuration known
    // beforehand, or to pass a matrix of matching size.
    m_decode.resize( m_nHarms, nSpks );

    // Find decode gains by matrix multiplication
    
    for( std::size_t i = 0; i < m_nHarms; i++) {
      for( std::size_t j = 0; j < nSpks; j++) {
            Afloat sum = 0.0f;
            for( std::size_t k = 0; k < m_nSpkSources; k++) {
                sum += m_regDecode( i, k ) * vbapGain( k, j );
            }
            m_decode( i, j ) = sum;
        }
    }
#endif
    return 0;
}
#endif

} // namespace panning
} // namespace visr
