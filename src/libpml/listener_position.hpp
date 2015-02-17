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
  class Quaternion
  {
  public:
  private:
  };

  /**
   * Default constructor, creates a position with coordinates (0,0,0).
   */
  ListenerPosition();

  void parse( std::istream &  inputStream );

  float x() const { return mX; }
  float y() const { return mY; }
  float z() const { return mZ; }
private:
	float mX;
	float mY;
	float mZ;

	std::uint64_t mTimeNS;
		
	unsigned int mFaceID;
};

std::ostream & operator<<(std::ostream & stream, const ListenerPosition & pos);

} // namespace pml
} // namespace visr


#endif // VISR_PML_LISTENER_POSITION_HPP_INCLUDED
