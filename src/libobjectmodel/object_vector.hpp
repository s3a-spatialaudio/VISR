/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED

#include "audio_object.hpp"

#include <map>
#include <memory>

namespace visr
{
namespace objectmodel
{

/**
 *
 */
class ObjectVector
{
public:
  /**
   * The type used to store the contents
   */
  using ObjectContainer = std::map< ObjectId, std::unique_ptr<AudioObject > >;

  /**
   * Default constructor, creates an empty object vector
   */
  ObjectVector();

  /**
   * Check: Explicitly define default copy constructor
   */
  ObjectVector( ObjectVector const & rhs ) = default;

  ObjectVector( ObjectVector && rhs ) = default;


  ObjectVector& operator=( ObjectVector const & rhs ) = default;

  ObjectVector& operator=( ObjectVector && rhs ) = default;

  /**
   *
   */
  ~ObjectVector();

  /**
   * Return a reference to an audio object in the vector.
   * @param The object id of the object to retrieved
   * @throw std::invalid_argument If no object with the given \p id exists in the vector.
   */
  AudioObject const & at( ObjectId id ) const;

  AudioObject & at( ObjectId id );


  void set( ObjectId id, AudioObject const &  obj );

private:
  ObjectContainer mObjects;
};

} // namespace objectmodel
} // namespace visr

#endif // #ifndef VISR_OBJECTMODEL_OBJECT_VECTOR_HPP_INCLUDED
