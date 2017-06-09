/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED

#include "object.hpp"

#include <libril/export_symbols.hpp>

#include <map>
#include <memory>

namespace visr
{
namespace objectmodel
{

/**
 * A class representing a set of audio objects of potentially different types.
 */
class ObjectVector
{
public:
  /**
   * The type used to store the the different object types polymorphically.
   */
  using ObjectContainer = std::map< ObjectId, std::unique_ptr<Object > >;

  /**
   * Provide a limited subset to the STL-type access functions of the underlying container type (map)
   */
  //@{
  using iterator = ObjectContainer::iterator;
  using const_iterator = ObjectContainer::const_iterator;

  using key_type = ObjectContainer::key_type;
  using mapped_type = ObjectContainer::mapped_type;
  using value_type = ObjectContainer::value_type;

  iterator begin() { return mObjects.begin(); }

  iterator end() { return mObjects.end(); }

  const_iterator begin() const { return mObjects.begin(); }

  const_iterator end() const { return mObjects.end(); }

  const_iterator find( ObjectId id ) const { return mObjects.find( id ); }

  /**
   * Return a non-const iterator to the element with key \p id
   * @note In the object dereferenced by <tt>return value</tt>->second, the id must not be changed, as it would destroy the integrity between the
   * id in the key value and the id of the contained object.
   */
  iterator find( ObjectId id ) { return mObjects.find( id ); }
  //@}

  /**
   * Default constructor, creates an empty object vector
   */
  ObjectVector();

  /**
   * Explicitly defined default copy constructor.
   * @todo Check whether copy construction is sensible for this type of object.
   */
  ObjectVector( ObjectVector const & rhs ) = delete;

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
   * Explicitly defined default copy constructor, move constructor flavour.
   * @todo Check whether copy construction is sensible for this object.
   */
//  ObjectVector( ObjectVector && rhs ) = default;

  /**
   * Explicit definition of default assignment operator.
   */
  ObjectVector& operator=( ObjectVector const & rhs ) = delete;

  /**
   * Explicit definition of default assignment operator, move constructor flavour.
   */
//  ObjectVector& operator=( ObjectVector && rhs ) = default;

  /**
   * Destructor.
   */
  ~ObjectVector();

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
   * Add a new audio object to the vector, possibly replacing an existing one with the same id.
   * The object is copied, and the ObjectVector takes ownership of the copy.
   * @param id The object id of the audio object.
   * @param obj The object to be inserted (copied).
   */
  void set( ObjectId id, Object const &  obj );

  /**
   * Add a new audio object to the vector, possibly replacing an existing one with the same id, move constructor flavour.
   * The object is copied, and the ObjectVector takes ownership of the copy.
   * @param id The object id of the audio object.
   * @param obj The object to be inserted (copied).
   */
  void set( ObjectId id, Object &&  obj );
  
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
