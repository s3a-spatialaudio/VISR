/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_LISTENER_POSITION_HPP_INCLUDED
#define VISR_PML_LISTENER_POSITION_HPP_INCLUDED

#include <cstdint>
#include <iosfwd>
#include <istream>

namespace visr
{
namespace pml
{

class ListenerPosition
{
public:
  using TimeType = std::uint64_t;
  using IdType = unsigned int;

  class Quaternion
  {
  public:
  private:
  };

  /**
   * Default constructor, creates a position with coordinates (0,0,0).
   */
  ListenerPosition();

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


#endif // VISR_PML_LISTENER_POSITION_HPP_INCLUDED
