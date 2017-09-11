/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED

#include "object.hpp"

#include "export_symbols.hpp"
#include "diffuse_source.hpp"

#include <set>
#include <memory>

namespace visr
{
namespace objectmodel
{

/**
 * A class representing a set of audio objects of potentially different types.
 */
class VISR_OBJECTMODEL_LIBRARY_SYMBOL ObjectVector
{
private:
  /**
   * Internal data structure to enable storing of the object vectoes in a set.
   * * An wrapper is necessary to store polymorphic objects.
   * * Provides a means to search by Id without constructing a new object for the search.
   */
  struct Containee
  {
  public:
    Containee( ObjectId id )
      : mId( id )
    {
    }

    Containee( Containee const & rhs )
      : mVal( rhs.mVal->clone() )
      , mId( mVal->id() )
    {
    }

    Containee( Containee && rhs )
      : mVal( std::move( rhs.mVal ) )
      , mId( mVal->id() )
    {
    }

    Containee( std::unique_ptr<Object> && obj )
      : mVal( std::move( obj ) )
      , mId( mVal->id() )
    {
    }

    Containee& operator=(Containee const & rhs )
    {
      mVal = rhs.mVal->clone();
      mId = mVal->id();
      return *this;
    }

    bool operator<( Containee const & rhs ) const
    {
      return mId < rhs.mId;
    }

    std::unique_ptr<Object> mVal;

    ObjectId mId;
  };

  /**
  * The type used to store the the different object types polymorphically.
  */
  using ObjectContainer = std::set< Containee >;

public:

  /**
   * Default constructor, creates an empty object vector
   */
  ObjectVector();

  /**
  * Copy constructor.
  * Made explicit to prevent unintentially copying
  */
  explicit ObjectVector( ObjectVector const & rhs );

  /**
  * Explicitly defined move constructor.
  */
  ObjectVector( ObjectVector && rhs );

  /**
  * Explicit definition of default assignment operator.
  */
  ObjectVector& operator=( ObjectVector const & rhs );

  /**
  * Explicit definition of move assignment operator.
  */
  ObjectVector& operator=( ObjectVector && rhs );

  /**
  * Destructor.
  */
  ~ObjectVector();

  /**
   * Assign member function as an explicit alternative to an assignment operator.
   */
  void assign( ObjectVector const & rhs );

  /**
   * Exchange the contents of this object with \p rhs.
   * Invalidates all iterators into this and \p rhs
   */
  void swap( ObjectVector & rhs );

  /**
   * Return the number of contained audio objects.
   */
  std::size_t size() const { return mObjects.size(); }

  /**
   * Return whether the object vector is empty.
   */
  bool empty() const { return mObjects.empty(); }

  /**
   * Return a reference to an audio object in the vector.
   * @param id The object id of the object to retrieved
   * @throw std::invalid_argument If no object with the given \p id exists in the vector.
   */
  Object & at( ObjectId id );

  /**
   * Return a reference to an audio object in the vector (constant version)
   * @param id The object id of the object to retrieved
   * @throw std::invalid_argument If no object with the given \p id exists in the vector.
   */
  Object const & at( ObjectId id ) const;

  /**
  * Iterator class for ObjectVector.
  * It models the BidirectionalIterator concept (like std::set iterators)
  */
  class iterator
  {
  public:
    friend class ObjectVector;
    using value_type = Object&;
    using pointer = Object*;
    value_type operator*() const { return *(mImpl->mVal); }

    iterator() = default;
    iterator( iterator const & rhs ) = default;
    iterator( iterator && rhs ) = default;

    pointer operator->() const { return mImpl->mVal.get(); };
    bool operator==( const iterator& rhs ) const { return mImpl == rhs.mImpl; }
    bool operator!=( const iterator& rhs ) const { return mImpl != rhs.mImpl; }

    /**
    * Pre-increment operator
    */
    iterator& operator++() { ++mImpl; return *this; }

    /**
    * Post-increment operator
    */
    iterator operator++( int )
    {
      iterator ret( *this );
      mImpl++;
      return ret;
    }

    /**
    * Pre-increment operator
    */
    iterator & operator--() { --mImpl; return *this; }

    /**
    * Post-decrement operator
    */
    iterator operator--( int )
    {
      iterator ret( *this );
      mImpl--;
      return ret;
    }
  private:
    explicit iterator( ObjectContainer::iterator const & internal )
      : mImpl( internal )
    {
    }
    explicit iterator( ObjectContainer::iterator && internal )
      : mImpl( internal )
    {
    }

    ObjectContainer::iterator mImpl;
  };

  /**
  * Constant iterator class for ObjectVector
  * It models the BidirectionalIterator concept (like std::set iterators)
  */
  class const_iterator
  {
  public:
    friend class ObjectVector;
    using value_type = Object&;
    using pointer = Object*;

    const_iterator() = default;
    const_iterator( const_iterator const & rhs ) = default;
    value_type operator*() const { return *(mImpl->mVal); }
    pointer operator->() const { return mImpl->mVal.get(); };
    bool operator==( const const_iterator& rhs ) const { return mImpl == rhs.mImpl; }
    bool operator!=( const const_iterator& rhs ) const { return mImpl != rhs.mImpl; }

    /**
    * Pre-increment operator
    */
    const_iterator& operator++() { ++mImpl; return *this; }

    /**
    * Post-increment operator
    */
    const_iterator operator++( int )
    {
      const_iterator ret( *this );
      mImpl++;
      return ret;
    }

    /**
    * Pre-increment operator
    */
    const_iterator & operator--() { --mImpl; return *this; }

    /**
    * Post-decrement operator
    */
    const_iterator operator--( int )
    {
      const_iterator ret( *this );
      mImpl--;
      return ret;
    }

  private:
    const_iterator( ObjectContainer::const_iterator const & internal )
      : mImpl( internal )
    {
    }

    const_iterator( ObjectContainer::const_iterator && internal )
      : mImpl( internal )
    {
    }

    ObjectContainer::const_iterator mImpl;
  };

  iterator begin() { return iterator( mObjects.begin() ); }

  iterator end() { return iterator( mObjects.end() ); }

  /**
  * Iterator begin/end for const objects.
  * @note For range-based for loops over constant object vectors, cbegin/cend are not sufficient.
  */
  //@{
  const_iterator begin() const { return mObjects.cbegin(); }

  const_iterator end() const { return mObjects.cend(); }
  //@} h

  const_iterator cbegin() const { return mObjects.cbegin(); }

  const_iterator cend() const { return mObjects.cend(); }

  const_iterator find( ObjectId id ) const { return mObjects.find( id ); }

  /**
  * Return a non-const iterator to the element with key \p id
  * @note In the object dereferenced by <tt>return value</tt>->second, the id must not be changed, as it would destroy the integrity between the
  * id in the key value and the id of the contained object.
  */
  iterator find( ObjectId id ) { return iterator( mObjects.find( id ) ); }
  //@}


  /**
   * Add a new audio object to the vector, possibly replacing an existing one with the same id.
   * The object is copied, and the ObjectVector takes ownership of the copy.
   * @param obj The object to be inserted (copied).
   */
  void insert( Object const &  obj );

  /**
   * Add a new audio object to the vector, possibly replacing an existing one with the same id, move constructor flavour.
   * The object is copied, and the ObjectVector takes ownership of the copy.
   * @param obj The object to be inserted (copied).
   */
  void insert( std::unique_ptr<Object> &&  obj );
  
  /**
   * Erase an object with a given object id from the vector.
   * @param id The id of the object to be deleted.
   * @throw std::invalid_argument if no object with the given id exists.
   */
  void remove( ObjectId id );
  
  /**
   * Erase all contained elements.
   */
  void clear();

private:
  ObjectContainer mObjects;
};

} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED
