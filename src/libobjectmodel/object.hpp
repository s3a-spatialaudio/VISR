/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_OBJECTMODEL_OBJECT_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_HPP_INCLUDED

#include "object_type.hpp"

#include <librbbl/parametric_iir_coefficient.hpp>

#include <climits>
#include <cstdint>
#include <memory>
#include <valarray>

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

  /**
   * Type used for the audio channels associated with an object.
   */
  using ChannelIndex = unsigned int;
  //@}

  /**
   * @note The solution using numeric_limits::max() would be preferable, but cannot be used since MSVC does not support const_expr yet.
   */
  static const ObjectId cInvalidObjectId = UINT_MAX; // should be std::numeric_limits<ObjectId>::max();

  static const GroupId cDefaultGroupId = 0;

  /**
   * Special value to denote an invalid or unassigned audio channel index.
   * @todo Decide whether this type must be visible on the outside.
   */
  static const ChannelIndex cInvalidChannelIndex = UINT_MAX; // should be std::numeric_limits<ChannelIndex>::max();

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
   * Support for channels assigned to an audio objects.
   * The base class interface supports arbitrary channel numbers and layouts, but derived classes might offer
   * only restricted channel layouts.
   */
  //@{
  /**
   * Return the number of audio channels of this object.
   */
  std::size_t numberOfChannels() const;

  /**
   * Return the audio channel index for a particular channel.
   * @param index the array index within the array of channel indices.
   * @throw std::invalid_argument If index exceeds the number of available audio channels
   */
  ChannelIndex channelIndex( std::size_t index ) const;

  /**
   * Change the number of channels for the audio object.
   * This resets all entries to invalid channel indices (i.e., cInvalidChannelIndex values).
   */
  void resetNumberOfChannels( std::size_t numChannels );

  /**
   * Set the audio channel index for a particular
   * @throw std::invalid_argument if index exceeds the number of channels for this object.
   */
  void setChannelIndex( std::size_t index, ChannelIndex channelIndex );
  //@}

  rbbl::ParametricIirCoefficientList<Coordinate> const & eqCoefficients() const;

  void setEqCoefficients( rbbl::ParametricIirCoefficientList<Coordinate>  const & newCoeffs );


  /**
   * Clone function used to emulate 'virtual copy constructor' functionality.
   * Must be implemented in every derived instantiated class.
   */
  virtual std::unique_ptr<Object> clone() const = 0;

protected:

private:
  ObjectId mObjectId;

  GroupId mGroupId;

  LevelType mLevel;

  Priority mPriority;

  std::valarray<ChannelIndex> mChannelIndices;

  rbbl::ParametricIirCoefficientList<Coordinate> mEqCoefficients;
};

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_OBJECT_HPP_INCLUDED
