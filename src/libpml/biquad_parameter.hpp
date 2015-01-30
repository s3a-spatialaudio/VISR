/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_BIQUAD_PARAMETER_HPP_INCLUDED
#define VISR_PML_BIQUAD_PARAMETER_HPP_INCLUDED

#include <algorithm>
#include <array>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace visr
{
namespace pml
{

/**
 * A FIFO-type message queue template class for storing and passing message data.
 * @tparam CoeffType Type of the contained elements.
 * @note This class does provide the same level of thread safety as, e.g., the STL.
 * I.e., calling code from different thread must ensure that concurrent accesses
 * to the same instances are appropriately secured against race conditions.
 */
template< typename CoeffType >
class BiquadParameter
{
public:
  /**
   * The number of coefficients to describe one biquad section.
   */
  static const std::size_t cNumberOfCoeffs = 5;

  /**
   * Default constructor, creates an empty message queue.
   */
  BiquadParameter()
  {
    mCoeffs.fill( static_cast<CoeffType>(0.0) );
  }

  BiquadParameter( CoeffType b0, CoeffType b1, CoeffType b2,
                   CoeffType a1, CoeffType a2 )
  {
    mCoeffs[0] = b0;
    mCoeffs[1] = b1;
    mCoeffs[2] = b2;
    mCoeffs[3] = a1;
    mCoeffs[4] = a2;
  }

  BiquadParameter( std::initializer_list< CoeffType > const & coeffs )
  {
    if( coeffs.size() != mCoeffs.size() )
    {
      throw std::invalid_argument( "BiquadParameter: Initialisation vector has wrong number of elements." );
    }
    std::copy( coeffs.begin(), coeffs.end(), mCoeffs.begin() );
  }

  BiquadParameter( const BiquadParameter & rhs ) = default;

  BiquadParameter & operator=( BiquadParameter const & rhs )
  {
    mCoeffs = rhs.mCoeffs;
    return *this;
  }

  CoeffType const * data() const { return &mCoeffs[0]; }

  CoeffType * data() { return &mCoeffs[0]; }

  CoeffType const & operator[]( std::size_t idx ) const
  {
    return mCoeffs[ idx ];
  }
  
  CoeffType & operator[]( std::size_t idx )
  {
    return mCoeffs[ idx ];
  }

  CoeffType const & at( std::size_t idx ) const
  {
    return mCoeffs.at( idx );
  }

  CoeffType & at( std::size_t idx )
  {
    return mCoeffs.at( idx );
  }

  
  CoeffType const & b0() const { return mCoeffs[0]; }

  CoeffType const & b1() const { return mCoeffs[1]; }

  CoeffType const & b2() const { return mCoeffs[2]; }

  CoeffType const & a1() const { return mCoeffs[3]; }
  
  CoeffType const & a2() const { return mCoeffs[4]; }


  CoeffType& b0() { return mCoeffs[0]; }

  CoeffType& b1() { return mCoeffs[1]; }

  CoeffType& b2() { return mCoeffs[2]; }

  CoeffType& a1() { return mCoeffs[3]; }
  
  CoeffType& a2() { return mCoeffs[4]; }

private:
  /**
   * The internal data representation.
   */
  std::array<CoeffType,5> mCoeffs;
};

template< class CoeffType >
class BiquadParameterList
{
public:
  /**
   * Default constructor, creates an empty list of biquad parameters.
   */
  BiquadParameterList() = default;

  /**
   * Default copy constructor (required for use within aSTL data structure as, for instance, in BiquadParameterList.
   */
  BiquadParameterList( BiquadParameterList const & rhs ) = default;

  BiquadParameterList( std::initializer_list<BiquadParameter<CoeffType> > const & initList )
    : mBiquads( initList )
  {
  }

  /**
   * Assign the content from another BiquadParameterList with consistent size.
   * @throw std::invalid_argument If \p rhs has a different size.
   */
  BiquadParameterList & operator=(BiquadParameterList const & rhs)
  {
    if( rhs.size() != size() )
    {
      std::invalid_argument( "BiquadParameterList: Size of argument to be assigned does not match." );
    }
    mBiquads = rhs.mBiquads;
    return *this;
  }

  std::size_t size() const { return mBiquads.size(); }

  void resize( std::size_t newSize )
  {
    mBiquads.resize( newSize, BiquadParameter<CoeffType>() );
  }

  BiquadParameter<CoeffType> const & operator[]( std::size_t idx ) const { return mBiquads[idx]; }
  BiquadParameter<CoeffType> & operator[]( std::size_t idx ) { return mBiquads[idx]; }

  BiquadParameter<CoeffType> const & at( std::size_t idx ) const { return mBiquads.at( idx ); }
  BiquadParameter<CoeffType> & at( std::size_t idx ) { return mBiquads.at( idx ); }

private:
  std::vector< BiquadParameter< CoeffType > > mBiquads;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_BIQUAD_PARAMETER_HPP_INCLUDED
