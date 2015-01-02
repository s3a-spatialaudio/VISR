/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED

#include "object.hpp"

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
   * Default constructor, creates an empty object vector
   */
  ObjectVector();

  /**
   * Explicitly defined default copy constructor.
   * @todo Check whether copy construction is sensible for this type of object.
   */
  ObjectVector( ObjectVector const & rhs ) = delete;

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
   * Return a reference to an audio object in the vector (const version)
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


private:
  ObjectContainer mObjects;
};

} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED
