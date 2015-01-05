/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_HPP_INCLUDED

#include "object_type.hpp"

#include <cstdint>
#include <climits>
#include <memory>

namespace visr
{
namespace objectmodel
{

// move to header object_id.hpp later (or make it a member of AudioObject)
using ObjectId = unsigned int;


using GroupId = unsigned int;

/**
 * Type use for level (gain, volume) settings, linear scale
 */
using LevelType = float;

/**
 *
 */
class Object
{
public:
  /**
   * Data types.
   */
  //@{
  /**
   * Data type used by default for all data members such as positions or angles.
   */
  using Coordinate = float;
  /**
   * Priority. Low values denote high priority (0 is highest priority)
   */
  using Priority = unsigned char;
  //@}

  /**
   * @note The solution using numeric_limits::max() would be preferable, but cannot be used since MSVC does not support const_expr yet.
   */
  static const ObjectId cInvalidObjectId = UINT_MAX; // std::numeric_limits<ObjectId>::max();

  static const GroupId cDefaultGroupId = 0;

  Object();

  explicit Object( ObjectId id );

  virtual ~Object() = 0;

  virtual ObjectTypeId type() const = 0;

  ObjectId id() const { return mObjectId; }

  GroupId groupId() const { return mGroupId; }

  void setObjectId( ObjectId newId );

  void setGroupId( ObjectId newId );

  LevelType level() const;

  void setLevel( LevelType newLevel );

  Priority priority() const;

  void setPriority( Priority newPriority );

  /**
   * Must be implemented in every derived instantiated class.
   */
  virtual std::unique_ptr<Object> clone() const = 0;

protected:

private:
  ObjectId mObjectId;

  GroupId mGroupId;

  LevelType mLevel;

  Priority mPriority;
};

} // namespace objectmodel
} // namespace visr




#endif // VISR_OBJECTMODEL_OBJECT_HPP_INCLUDED
