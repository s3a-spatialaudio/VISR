/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "matrix_parameter.hpp"

#include <sndfile.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

#include <ciso646>
#include <fstream>
#include <limits>
#include <regex>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace visr
{
namespace pml
{

template< typename ElementType >
MatrixParameter<ElementType>::MatrixParameter( std::size_t alignment /*= 0*/ )
  : efl::BasicMatrix<ElementType>( alignment )
{
}

template< typename ElementType >
MatrixParameter<ElementType>::MatrixParameter( std::size_t numRows, std::size_t numColumns, std::size_t alignment /*= 0*/ )
  : efl::BasicMatrix<ElementType>( numRows, numColumns, alignment )
{
}

template< typename ElementType >
MatrixParameter<ElementType>::MatrixParameter( std::size_t numRows, std::size_t numColumns,
                                               std::initializer_list<std::initializer_list<ElementType> > const & initMtx,
                                               std::size_t alignment /*= 0*/ )
  : efl::BasicMatrix< ElementType >( numRows, numColumns, initMtx, alignment )
{
}

template< typename ElementType >
MatrixParameter<ElementType>::MatrixParameter( MatrixParameter<ElementType> const & rhs )
: MatrixParameter( rhs.numberOfRows(), rhs.numberOfColumns(), rhs.alignmentElements() )
{
  efl::BasicMatrix<ElementType>::copy( rhs );
}

template< typename ElementType >
void MatrixParameter<ElementType>::resize( std::size_t numRows, std::size_t numColumns )
{
  efl::BasicMatrix< ElementType >::resize( numRows, numColumns );
}


template< typename ElementType >
/*static*/ MatrixParameter<ElementType>
MatrixParameter<ElementType>::fromString( std::string const & textMatrix, std::size_t alignment /*= 0*/ )
{
  std::stringstream stream( textMatrix );
  return fromStream( stream, alignment );
}

template< typename ElementType >
/*static*/ MatrixParameter<ElementType>
MatrixParameter<ElementType>::fromStream( std::istream & stream, std::size_t alignment /*= 0*/ )
{
  namespace qi = boost::spirit::qi;
  namespace phoenix = boost::phoenix;

  std::vector< std::vector<ElementType> > tmpStorage;
  std::size_t minSize = 0;
  std::size_t maxSize = std::numeric_limits<std::size_t>::max();
  while( not stream.eof() )
  {
    if( not stream.good( ) )
    {
      throw std::invalid_argument( "MatrixParameter::fromStream(): Error while reading line." );
    }
    std::string currLine;
    std::getline( stream, currLine );
    std::vector<ElementType> v;
    std::string::const_iterator startIt = currLine.begin();
    std::string::const_iterator endIt = currLine.end( );
    bool parseRes = qi::phrase_parse( startIt, endIt,
#if 1
      ( *(qi::real_parser<ElementType>()[phoenix::push_back( phoenix::ref( v ), qi::_1 )]
        % *(-qi::char_( ',' ) ))
        >> *(qi::char_( '%' ) >> *qi::char_) ),
#else
      (*(qi::real_parser<ElementType>( )[phoenix::push_back( phoenix::ref( v ), qi::_1 )]
      >> *(-qi::char_( ',' ) >> qi::real_parser<ElementType>( )[phoenix::push_back( phoenix::ref( v ), qi::_1 )]))
      >> *(qi::char_( '%' ) >> *qi::char_)),
#endif
      boost::spirit::ascii::space );
    if( !parseRes or (startIt != endIt ) )
    {
      throw std::invalid_argument( "MatrixParameter::fromStream(): Error while parsing line." );
    }
    std::size_t const numEl = v.size();
    if( numEl == 0 )
    {
      continue; // ignore empty lines (might consist purely of comments)
    }
    tmpStorage.push_back( v );
    minSize = std::max(minSize, numEl );
    maxSize = std::min(maxSize, numEl );
  }
  if( minSize != maxSize )
  {
    throw std::invalid_argument( "MatrixParameter::fromStream(): Matrix rows (text lines) have different numbers of elements." );
  }
  if( tmpStorage.size() == 0 )
  {
    // Zero rows means empty matrix (which is legal)
    return MatrixParameter<ElementType>( alignment );
  }
  std::size_t const numRows = tmpStorage.size();
  std::size_t const numCols = minSize;
  MatrixParameter<ElementType> result( numRows, numCols, alignment );
  for( std::size_t rowIdx( 0 ); rowIdx < numRows; ++rowIdx )
  {
    std::vector<ElementType> const & v( tmpStorage[rowIdx] );
    std::copy( v.begin(), v.end(), result.row( rowIdx ) );
  }
  return result;
}

namespace // unnamed
{
  // Template function with specialisations for double and float to call the correct version of sf_read_<datatype>.
  template<typename ElementType>
  sf_count_t read_samples( SNDFILE * file, efl::AlignedArray<ElementType> & result, std::size_t numElements );
#if 0
  {
    assert( false && "The method is not specialised for this data type." );
  }
#endif
  template<> sf_count_t read_samples<float>( SNDFILE * file, efl::AlignedArray<float> & result, std::size_t numElements )
  {
    return sf_read_float( file, result.data(), static_cast<sf_count_t>(numElements) );
  }

  template<> sf_count_t read_samples<double>( SNDFILE * file, efl::AlignedArray<double> & result, std::size_t numElements )
  {
    return sf_read_double( file, result.data( ), static_cast<sf_count_t>(numElements) );
  }
} // unnamed namespace

template< typename ElementType >
/*static*/ MatrixParameter<ElementType>
MatrixParameter<ElementType>::fromAudioFile( std::string const & fileName, std::size_t alignment /*= 0*/ )
{
  boost::filesystem::path file( fileName );
  if( not exists( file ) or is_directory( file ) )
  {
    throw std::invalid_argument( "MatrixParameter::fromAudioFile(): The specified file name does not exist." );
  }
  SF_INFO fileInfo;
  SNDFILE* fileHandle = nullptr;
  fileHandle = sf_open( file.string().c_str(), SFM_READ, &fileInfo );
  if( not fileHandle )
  {
    throw std::invalid_argument( "MatrixParameter::fromAudioFile(): The specified file name could not be opened." );
  }
  try
  {
    std::size_t const numRows = static_cast<std::size_t>(fileInfo.channels);
    std::size_t numCols = static_cast<std::size_t>(fileInfo.frames);
    MatrixParameter<ElementType> resultMtx( numRows, numCols, alignment );

    efl::AlignedArray<ElementType> tmpStorage( numRows*numCols, alignment );
    sf_count_t const numReadRaw = read_samples<ElementType>( fileHandle, tmpStorage, numRows*numCols );
    if( numReadRaw < 0 )
    {
      std::invalid_argument( "MatrixParameter::fromAudioFile(): Reading the audio data failed." );
    }
    std::size_t const numRead = static_cast<std::size_t>(numReadRaw);
    if( numRead != numRows * numCols )
    {
      std::invalid_argument( "MatrixParameter::fromAudioFile(): Reading the audio data reeturned a wrong number of arguments." );
    }
    // De-interleaving, might be implemented in a library function
    for( std::size_t rowIdx(0); rowIdx < numRows; ++rowIdx )
    {
      ElementType * rowData = resultMtx.row( rowIdx );
      for( std::size_t colIdx(0); colIdx < numCols; ++colIdx )
      {
        *(rowData + colIdx) = tmpStorage[colIdx * numRows + rowIdx];
      }
    }
    sf_close( fileHandle );
    fileHandle = nullptr;
    return resultMtx;
  }
  catch( std::exception const & /*ex*/ )
  {
    sf_close( fileHandle );
    fileHandle = nullptr;
    throw; // rethrow, i.e., pass the exception to the next level.
  }
}

template< typename ElementType >
/*static*/ MatrixParameter<ElementType>
MatrixParameter<ElementType>::fromTextFile( std::string const & fileName, std::size_t alignment /*= 0*/ )
{
  boost::filesystem::path file( fileName );
  if( not exists( file ) or is_directory( file ) )
  {
    throw std::invalid_argument( "MatrixParameter::fromTextFile(): The specified file name does not exist." );
  }
  std::ifstream stream( file.string(), std::ios_base::in );
  if( not stream )
  {
    throw std::invalid_argument( "MatrixParameter::fromTextFile(): Opening file for reading failed." );
  }
  return fromStream( stream, alignment );
}

// Explicit instantiations for sample types float and double
template class MatrixParameter<float>;
template class MatrixParameter<double>;

} // namespace pml
} // namespace visr
