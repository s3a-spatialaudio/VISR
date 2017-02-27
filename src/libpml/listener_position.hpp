/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_LISTENER_POSITION_HPP_INCLUDED
#define VISR_PML_LISTENER_POSITION_HPP_INCLUDED

#include "empty_parameter_config.hpp"

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>


#include <cstdint>
#include <iosfwd>
#include <istream>

namespace visr
{
namespace pml
{

class ListenerPosition: public TypedParameterBase<EmptyParameterConfig, ParameterType::ListenerPosition >
{
public:
  using TimeType = std::uint64_t;
  using IdType = unsigned int;

  explicit ListenerPosition( ParameterConfigBase const & config );

  /**
   * Also acts as default constructor.
   */
  explicit ListenerPosition( EmptyParameterConfig const & config = EmptyParameterConfig() );

  virtual ~ListenerPosition() override;

  class Quaternion
  {
  public:
  private:
  };


  ListenerPosition(float x, float y, float z)
   : mX(x)
   , mY(y)
   , mZ(z)
  {}

  void parse( std::istream &  inputStream );

  float x() const { return mX; }
  float y() const { return mY; }
  float z() const { return mZ; }

  TimeType timeNs() const { return mTimeNs; }

  IdType faceID() const { return mFaceID; }

private:
  float mX;
  float mY;
  float mZ;

  TimeType mTimeNs;

  IdType mFaceID;
};

std::ostream & operator<<(std::ostream & stream, const ListenerPosition & pos);

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::ListenerPosition, visr::ParameterType::ListenerPosition, visr::pml::EmptyParameterConfig )

#endif // VISR_PML_LISTENER_POSITION_HPP_INCLUDED
