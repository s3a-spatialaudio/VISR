//
//  AllRAD.h
//
//  Calculate HOA decode matrix by feeding regular HOA decode to external VBAP of virtual louspeakers.
//  Suitable for fast update for listener tracking.
//
//  Created by Dylan Menzies on 12/12/2014.
//  Copyright (c) ISVR, University of Southampton. All rights reserved.
//

#ifndef __S3A_renderer_dsp__AllRAD__
#define __S3A_renderer_dsp__AllRAD__


#define MAX_ORDER 10
#define MAX_NUM_HARMONICS (1+MAX_ORDER)*(1+MAX_ORDER)



#include <iostream>

#include "LoudspeakerArray.h"
#include "VBAP.h"

#include <libefl/basic_matrix.hpp>

namespace visr
{

// forward declaration
namespace efl
{
template< typename T >
class BasicMatrix;
}

namespace panning
{

class AllRAD {

public:
    /**
     * Default constructor.
     * Creates an empty, unitialized object.
     */
//    AllRAD();

    /**
     * Constructor to initialise an AllRAD object.
     * @param regularArray A regular spherical array to which the HOA signals are decoded to.
     * The referenced object must persist for the lifetime of the AllRAD object.
     * @param decodeCoeffs Matrix coefficients for decoding the HOA signals to the regular array.
     * Dimension: (hoaOrder+1)^2 x regularArray.numRegularSpeakers().
     * @param maxHoaOrder The maximum Ambisonics order of the HOA decoder. If the same decoder is used for multiple HOA objects,
     * this must be the maximum order.
     */
    explicit AllRAD( LoudspeakerArray const & regularArray,
                     LoudspeakerArray const & realArray,
                     efl::BasicMatrix<Afloat> const & decodeCoeffs,
                     unsigned int maxHoaOrder );

    /**
     * Load the decoding matrix for decoding the HOA sound field to the regular array.
     * @param file File handle containing the decoding coefficients (real-valued floats) in text form.
     * @param order The order of Ambisonics decoder.
     * @param nSpks The number of loudspeakers in the regular array.
     * @return 0 in case of success, -1 in case of error
     */
    // int loadRegDecodeGains(FILE* file, int order, int nSpks);
    
    /**
     * Calculate the VBAP coefficients for decoding the HOA signals to the (real) loudspeaker configuration 
     * corresponding to the \p vbap object.
     * @param vbap The VBAP object to create panning gains from the regular array positions to the real loudspeaker array.
     * @note Ensure that the vbap object is initialiseed (i.e., calcInvMatrices() has been called beforehand).
     */
    // int calcDecodeGains(VBAP * vbap);

    void setListenerPosition( SampleType x, SampleType y, SampleType z );

    void setListenerPosition( XYZ const & newPosition );
    
    efl::BasicMatrix<Afloat> const & decodingGains() const
    {
      return mRealDecodeCoefficients;
    }
private:
    /**
     *
     */
    void updateDecodingCoefficients();

    VBAP mRegularDecoder;

    VBAP mRealDecoder;

    // Harmonic ordering is "Ambisonic Channel Numbering" (ACN) i = n^2 + n + m for Y_i = Y_{n m}
    efl::BasicMatrix<Afloat> mRegularDecodeCoefficients;

    /**
     * Matrix of decode coefficients for the real loudspeaker
     */
    efl::BasicMatrix<Afloat> mRealDecodeCoefficients;

    /**
     * Number of harmonics, i.e., (hoaOrder+1)^2
     */
    std::size_t const mNumberOfHarmonics;

    /**
     * Number of loudspeakers in the regular array.
     */
    std::size_t mRegularArraySize = 0;
    

};

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__AllRAD__) */
