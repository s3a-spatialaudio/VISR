/* Copyright Institute of Sound and Vibration Research - All rights reserved */

/**
 * @file object_type.hpp
 */

#ifndef VISR_OBJECTMODEL_OBJECT_TYPE_HPP_INCLUDED
#define VISR_OBJECTMODEL_OBJECT_TYPE_HPP_INCLUDED

#include <cstdint>
#include <string>


namespace visr
{
namespace objectmodel
{

using ObjectTypeIntegerRepresentation = std::uint8_t;

/**
 * A numeric id to uniquely describe object types.
 */
enum class ObjectTypeId: ObjectTypeIntegerRepresentation
{
  PointSource = 0,   /**< Simple point-like source (monopole) */
  PlaneWave = 1,     /**< Straight plane-wave source type */
  DiffuseSource = 2,  /**< Totally diffuse source type */
  PointSourceWithDiffuseness = 3, /**< Point-source-like audio object with an addditional "diffuseness" attribute 
                                  controlling the fraction of the source that is reproduced diffusely.*/
  ExtendedSource = 4, /**< Source type with controllable extent, i.e. width and height.*/
  HoaSource = 5, /**< Higher Order Ambbisonics object, sound field representation based on spherical harmonics */ 
  ChannelObject = 6 /**< Source type representing a single or multiple channels routed to a set of loudspeaker channels. */
  // to be continued.
};

/**
 * Convert an object type id into its string representation
 * @param type 
 * @throw std::logic_error Happens only in case of an internal inconsistency, i.e.,
 * if the type is not found in the lookup table.
 */
const std::string& objectTypeToString( ObjectTypeId type );

/**
 * @param typeString
 * @return The object id of the type correspoonding to the string representation
 * @throw std::invalid_argument If typeStr does not correspond to an existing object type.
 */
ObjectTypeId stringToObjectType( std::string const & typeString );

} // namespace objectmodel
} // namespace visr

#endif // VISR_OBJECTMODEL_OBJECT_TYPE_HPP_INCLUDED
