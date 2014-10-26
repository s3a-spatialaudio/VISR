/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBEFL_ALIGNED_ARRAY_HPP_INCLUDED
#define VISR_LIBEFL_ALIGNED_ARRAY_HPP_INCLUDED

#include <cstddef> // for std::size_t
#include <memory>
#include <new>
#include <stdexcept>

// Workaround for the function std::align() missing in the GNU libstdc++ library
// (GCC <= 4.9). See https://gcc.gnu.org/bugzilla/show_bug.cgi?id=57350
// TODO: Remove as soon as feature has been implemented on all supported platforms
#ifdef __GLIBCXX__
#include <cstdint> // needed for the type std::uintptr_t used in the workarounf implementation
#endif

namespace visr
{
namespace efl
{

template< typename T >
class AlignedArray
{
public:
  explicit AlignedArray( std::size_t alignmentElements );

  explicit AlignedArray( std::size_t length, std::size_t alignmentElements );

  ~AlignedArray();

  void resize( std::size_t newLength );

  T* data() { return mAlignedStorage; }

  T const * data( ) const { return mAlignedStorage; }

  T& operator[]( std::size_t index ) { return mAlignedStorage[index]; }

  T const & operator[]( std::size_t index ) const { return mAlignedStorage[index]; }

  T& at( std::size_t index )
  { 
    if( index >= mLength )
    {
      throw std::out_of_range( "Array index exceeded" );
    }
    return operator[]( index );
  }

  T const & at( std::size_t index ) const
  {
    if( index >= mLength ) {
      throw std::out_of_range( "Array index exceeded" );
    }
    return (*this)[index];
  }

private:
  void allocate( std::size_t length );
  void deallocate( );

  const std::size_t mAlignment;

  std::size_t mLength;

  T* mRawStorage;
  T* mAlignedStorage;
};

template< typename T>
AlignedArray<T>::AlignedArray( std::size_t alignmentElements )
: mAlignment( alignmentElements )
, mLength( 0 )
, mRawStorage( nullptr )
, mAlignedStorage( nullptr )
{
}

template< typename T>
AlignedArray<T>::AlignedArray( std::size_t length, std::size_t alignmentElements )
: mAlignment( alignmentElements )
{
  allocate( length );
}

template< typename T>
AlignedArray<T>::~AlignedArray()
{
  deallocate();
}

template< typename T>
void AlignedArray<T>::resize( std::size_t newLength )
{
  deallocate();
  allocate( newLength );
}

#ifdef __GLIBCXX__
// Proposed standard-compliant implementation:
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=57350
// Published under the MIT license http://opensource.org/licenses/MIT
// Copyright notice (from https://code.google.com/p/c-plus/source/browse/src/util.h):
// // Cplus project, general-purpose utilities.
// // copyright David Krauss, created 8/26/11
static inline void *alignWorkaround( std::size_t alignment, std::size_t size,
  void *&ptr, std::size_t &space )
{
  std::uintptr_t pn = reinterpret_cast< std::uintptr_t >(ptr);
  std::uintptr_t aligned = (pn + alignment - 1) & -alignment;
  std::size_t padding = aligned - pn;
  if( space < size + padding ) return nullptr;
  space -= padding;
  return ptr = reinterpret_cast< void * >(aligned);
}
#endif

template< typename T>
void AlignedArray<T>::allocate( std::size_t length )
{
  const std::size_t worstCaseLengthElements( length + mAlignment - 1 );
  mRawStorage = new T[worstCaseLengthElements];
  void* retPtr = static_cast<void*>(mRawStorage);
  std::size_t space = worstCaseLengthElements*sizeof(T);
  // ignoring the return value is safe, because it is identical to the updated retPtr parameter.
#ifdef __GLIBCXX__
  alignWorkaround( mAlignment*sizeof(T), length*sizeof(T), retPtr, space );
#else
  std::align( mAlignment*sizeof(T), length*sizeof(T), retPtr, space );
#endif
  if( retPtr == nullptr )
  {
    deallocate(); // Set the object into a defined, valid state.
    throw std::bad_alloc();
  }
  // todo: should we check the updated 'space' parameter? 
  mAlignedStorage = static_cast<T*>(retPtr);
}

template< typename T>
void AlignedArray<T>::deallocate()
{
  delete[] mRawStorage;
  mRawStorage = nullptr;
  mAlignedStorage = nullptr;
  mLength = 0;
}

} // namespace efl
} // namespace visr

#endif // #ifndef VISR_LIBEFL_ALIGNED_ARRAY_HPP_INCLUDED
