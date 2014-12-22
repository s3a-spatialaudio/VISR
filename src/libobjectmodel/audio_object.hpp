/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_AUDIO_OBJECT_HPP_INCLUDED
#define VISR_OBJECTMODEL_AUDIO_OBJECT_HPP_INCLUDED

#include <cstdint>
#include <memory>

namespace visr
{
namespace objectmodel
{

// move to header object_id.hpp later
using ObjectId = unsigned int;

using GroupId = unsigned int;

/**
 * A numeric id to uniquely describe object types.
 */
enum class ObjectTypeId : std::uint8_t
{
  PointSource = 0, /**< Simple point-like source (monopole) */
  PlaneWave = 1,
  DiffuseSource = 2
  // to be continued.
};

/**
 *
 */
class AudioObject
{
public:
  explicit AudioObject( ObjectId id );

  virtual ~AudioObject() = 0;

  virtual ObjectTypeId type() const = 0;

  ObjectId id() const { return mObjectId; }

  GroupId groupId() const { return mGroupId; }

  void setObjectId( ObjectId newId );

  void setGroupId( ObjectId newId );

  /**
   * Must be implemented in every derived instantiated class.
   */
  virtual std::unique_ptr<AudioObject> clone() const = 0;

protected:

private:
  ObjectId mObjectId;

  GroupId mGroupId;
};

} // namespace objectmodel
} // namespace visr




#endif // VISR_OBJECTMODEL_AUDIO_OBJECT_HPP_INCLUDED
