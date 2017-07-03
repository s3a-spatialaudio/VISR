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

#include "VBAP.h"

#include "export_symbols.hpp"

#include <libefl/basic_matrix.hpp>

namespace visr
{

namespace panning
{
// forward declaration
class LoudspeakerArray;

class VISR_PANNING_LIBRARY_SYMBOL AllRAD
{
public:

  /**
   * Constructor to initialise an AllRAD object.
   * @param regularArray A regular spherical array used for the initial stage of decoding.
   * @param realArray The real, physical loudspeaker arry used for reproduction.
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
   * Adjust the listener position. This triggers a recalculation of the decoding coefficients.
   */
  void setListenerPosition( SampleType x, SampleType y, SampleType z );

  /**
   * Return the current matrix of decoding gains.
   * Dimension: numberOfHarmonics x mumberOfRelarLoudspeakers
   */
  efl::BasicMatrix<Afloat> const & decodingGains() const
        {
    return mRealDecodeCoefficients;
        }
private:

  /**
   * Internal function to recalculate the spherical harmonics coefficients for the real loudspeaker setup.
   * Called in constructor and after updating the listener position.
   */
  void updateDecodingCoefficients();


  /**
   * The positions of the loudspeakers of the regular array.
   * Dimension: numberOfRegularLoudspeakers x 3
   */
  efl::BasicMatrix<SampleType> mRegularLoudspeakerPositions;

  /**
   * The VBAP parameter calculator for the real loudspeaker array.
   * During runtime, this object is used only if the listener position changes.
   */
  VBAP mRealDecoder;

  /**
   * Number of harmonics, i.e., (hoaOrder+1)^2
   */
  std::size_t const mNumberOfHarmonics;

  /**
   * Matrix containing the Ambisonics coefficients for the regular array.
   * Dimension mNumberOfHarmonics x regularArray.numRegularSpeakers()_
   * Harmonic ordering is "Ambisonic Channel Numbering" (ACN) i = n^2 + n + m for Y_i = Y_{n m}
   */
  efl::BasicMatrix<SampleType> mRegularDecodeCoefficients;

  /**
   * Matrix of decode coefficients for the real loudspeaker
   * Dimension: numberOfHarmonics x mumberOfRelarLoudspeakers
   */
  efl::BasicMatrix<SampleType> mRealDecodeCoefficients;

  /**
   * Number of loudspeakers in the regular array.
   */
  std::size_t const mRegularArraySize;

  /**
   * Matrix holding the VBAP gains from the regular to the real loudspeaker positions.
   * Used only internally and temporarily.
   * Dimension: numberOfRegularLoudspeaker x numberOfRealLoudspeakers
   */
  efl::BasicMatrix<SampleType> mRegularToRealDecodeCoefficients;
};

} // namespace panning
} // namespace visr

#endif /* defined(__S3A_renderer_dsp__AllRAD__) */
