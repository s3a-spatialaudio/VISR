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

AllRAD::AllRAD()
 : m_regDecode( 0 )
 , m_decode( 0 )
 , m_nHarms( 0 )
 , m_nSpkSources( 0 )
    , m_regArray( 0 )
{
}

AllRAD::AllRAD( LoudspeakerArray const * regularArray,
                efl::BasicMatrix<Afloat> const & decodeCoeffs,
                unsigned int hoaOrder )
 // Note: No copy construction of BasicMatrix objects, so we copy afterwards.
 : m_regDecode( decodeCoeffs.numberOfRows( ), decodeCoeffs.numberOfColumns( ), decodeCoeffs.alignmentElements( ) )
 , m_decode( (hoaOrder+1) * (hoaOrder+1), regularArray->getNumSpeakers(), decodeCoeffs.alignmentElements() )
 , m_nHarms( (hoaOrder + 1) * (hoaOrder + 1) )
 , m_nSpkSources( regularArray->getNumSpeakers( ) )
 , m_regArray( regularArray )
{
  m_regDecode.copy( decodeCoeffs );
}

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
};





int AllRAD::calcDecodeGains(VBAP* vbap){
    
    // In vbap first do externally: setListenerPosition, then calcInvMatrix.
    
    vbap->setSourcePositions( m_regArray->getPositions()  );
    
    vbap->setNumSources(m_regArray->getNumSpeakers());
    
    //! Make sure the isAtInfinity flag is set for all regArray speakers
    
    vbap->calcGains();
    
    efl::BasicMatrix<Afloat> const & vbapGain = vbap->getGains( );
    
    std::size_t const nSpks = vbap->getNumSpeakers();
    
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
    return 0;
};

} // namespace panning
} // namespace visr
