/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_BIQUAD_PARAMETER_HPP_INCLUDED
#define VISR_PML_BIQUAD_PARAMETER_HPP_INCLUDED

#include "matrix_parameter_config.hpp" // might be a forward declaration

#include <libril/typed_parameter_base.hpp>
#include <libril/parameter_type.hpp>

#include <boost/property_tree/ptree_fwd.hpp>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <iosfwd>
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
   * Create an default object corresponding to a flat EQ.
   */
  BiquadParameter()
  {
    mCoeffs.fill( static_cast<CoeffType>(0.0) );
    mCoeffs[0] = static_cast<CoeffType>(1.0);
  }

  BiquadParameter( BiquadParameter<CoeffType> const & rhs ) = default;

  /**
   * Create a BiquadParameter objects from JSON and XML representations.
   */
  //@{
  static BiquadParameter fromJson( boost::property_tree::ptree const & tree );

  static BiquadParameter fromJson( std::basic_istream<char> & stream );

  static BiquadParameter fromXml( boost::property_tree::ptree const & tree );

  static BiquadParameter fromXml( std::basic_istream<char> & stream );
  //@}

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

  // BiquadParameter( const BiquadParameter & rhs ) = default;

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

  void loadJson( boost::property_tree::ptree const & tree );

  void loadJson( std::basic_istream<char> & );

  void loadXml( boost::property_tree::ptree const & tree );

  void loadXml( std::basic_istream<char> & );

  /**
   * 
   */
  //@{
  void writeJson( boost::property_tree::ptree & tree ) const;

  void writeJson( std::basic_ostream<char> & stream ) const;

  void writeJson( std::string & str ) const;

  void writeXml( boost::property_tree::ptree & tree ) const;

  void writeXml ( std::basic_ostream<char> & stream ) const;

  void writeXml ( std::string & str ) const;

  //@}
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

  explicit BiquadParameterList( const std::size_t initialSize )
    : mBiquads( initialSize )
  {
  }

  /**
   * Default copy constructor (required for use within aSTL data structure as, for instance, in BiquadParameterList.
   */
  BiquadParameterList( BiquadParameterList const & rhs ) = default;

  BiquadParameterList( std::initializer_list<BiquadParameter<CoeffType> > const & initList )
    : mBiquads( initList )
  {
  }

  static BiquadParameterList fromJson( boost::property_tree::ptree const & tree );

  static BiquadParameterList fromJson( std::basic_istream<char> & stream );

  static BiquadParameterList fromJson( std::string const & str );

  static BiquadParameterList fromXml( boost::property_tree::ptree const & tree );

  static BiquadParameterList fromXml( std::basic_istream<char> & stream );

  static BiquadParameterList fromXml( std::string const & str );

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

  void loadJson( boost::property_tree::ptree const & tree );

  void loadJson( std::basic_istream<char> & stream );

  void loadJson( std::string const & str );

  void loadXml( boost::property_tree::ptree const & tree );

  void loadXml( std::basic_istream<char> & stream );

  void loadXml( std::string const & str );

  /**
  *
  */
  //@{
  void writeJson( boost::property_tree::ptree & tree ) const;

  void writeJson( std::basic_ostream<char> & stream ) const;

  void writeJson( std::string & str ) const;

  void writeXml( boost::property_tree::ptree & tree ) const;

  void writeXml( std::basic_ostream<char> & stream ) const;

  void writeXml( std::string & str ) const;
  //@}


private:
  std::vector< BiquadParameter< CoeffType > > mBiquads;
};

namespace // unnamed
{
/**
 * Type trait to assign a unique type to each concrete BiquadParameterMatrix type.
 */
template<typename ElementType> struct BiquadMatrixParameterType{};

template<> struct BiquadMatrixParameterType<float>
{
  static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "BiquadFloatMatrix" ); }
};
template<> struct BiquadMatrixParameterType<double>
{
  static constexpr const ParameterType ptype() { return detail::compileTimeHashFNV1( "BiquadDoubleMatrix" ); }
};
} // unnamed

template<typename CoeffType >
class BiquadParameterMatrix: public TypedParameterBase<BiquadParameterMatrix<CoeffType>, MatrixParameterConfig, BiquadMatrixParameterType<CoeffType>::ptype() >
{
public:
  explicit BiquadParameterMatrix( MatrixParameterConfig const & config );

  explicit BiquadParameterMatrix( ParameterConfigBase const & config );

  explicit BiquadParameterMatrix( std::size_t numberOfFilters, std::size_t numberOfBiquads );

  virtual ~BiquadParameterMatrix() override;

  std::size_t numberOfFilters() const { return mRows.size(); }
  std::size_t numberOfSections() const { return mRows.empty() ? 0 : mRows[0].size(); }

  void resize( std::size_t numberOfFilters, std::size_t numberOfBiquads );
  BiquadParameterList<CoeffType> const & operator[]( std::size_t rowIdx ) const { return mRows[rowIdx]; }
  BiquadParameterList<CoeffType> & operator[]( std::size_t rowIdx ) { return mRows[rowIdx]; }

  BiquadParameter<CoeffType> const & operator()( std::size_t rowIdx, std::size_t colIdx ) const { return mRows[rowIdx][colIdx]; }
  BiquadParameter<CoeffType> & operator()( std::size_t rowIdx, std::size_t colIdx ) { return mRows[rowIdx][colIdx]; }

  /**
   * Set the biquad sections for a complete filter specification (a row in the matrix)
   * If \p newFilter has fewer sections than the matrix, the rest is filled with default values.
   * @throw std::out_of_range If \p filterIdx exceeds the number of biquad sections.
   * @throw std::invalid_argument If \p nnewFilters has more elements than the column number of the matrix.
   */
  void setFilter( std::size_t filterIdx, BiquadParameterList<CoeffType> const & newFilter );

private:
  using ContainerType = std::vector< BiquadParameterList<CoeffType> >;

  ContainerType mRows;
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::BiquadParameterMatrix<float>, visr::pml::BiquadParameterMatrix<float>::staticType(), visr::pml::MatrixParameterConfig )
DEFINE_PARAMETER_TYPE( visr::pml::BiquadParameterMatrix<double>, visr::pml::BiquadParameterMatrix<double>::staticType(), visr::pml::MatrixParameterConfig )

#endif // VISR_PML_BIQUAD_PARAMETER_HPP_INCLUDED
