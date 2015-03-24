/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_ARRAY_CONFIGURATION_HPP_INCLUDED
#define VISR_PML_ARRAY_CONFIGURATION_HPP_INCLUDED

#include <libefl/basic_matrix.hpp>

#include <algorithm>
#include <array>
#include <initializer_list>
#include <stdexcept>
#include <set>
#include <string>

namespace visr
{
namespace efl
{
template< typename ValueType > class BasicVector;
}

namespace pml
{

class ArrayConfiguration
{
public:
  struct Speaker
  {
  public:
    explicit Speaker( std::size_t pId, double pGainAdjustment, double pDelayAdjustment )
      : id( pId ), gainAdjustment( pGainAdjustment ), delayAdjustment( pDelayAdjustment )
    {}

    std::size_t id;
    double gainAdjustment;
    double delayAdjustment;
  };

  struct CompareSpeakers
  {
    bool operator()(Speaker const & lhs, Speaker const & rhs) const
    {
      return lhs.id < rhs.id;
    }
  };

  using ArrayType = std::set<Speaker, CompareSpeakers>;

  ArrayConfiguration();

  ~ArrayConfiguration();

  /**
   * @throw std::invalid_argument if the array file is inconsistent.
   */
  void loadXml( std::string const & filePath );

  std::size_t numberOfSpeakers() const { return mArray.size(); }

  template< typename ValueType >
  void getGains( efl::BasicVector<ValueType> & gains ) const
  {
    if( gains.size() != numberOfSpeakers() )
    {
      throw std::invalid_argument( "ArrayConfiguration::getGains(): Size of outputMatrix does not match the array size." );
    }
    std::size_t outIdx( 0 );
    for( ArrayType::const_iterator arrayIt( mArray.begin() ); arrayIt != mArray.end(); ++arrayIt, ++outIdx )
    {
      gains[outIdx] = static_cast<ValueType>(arrayIt->gainAdjustment);
    }
  }
private:
  ArrayType mArray;
};

} // namespace pml
} // namespace visr


#endif // VISR_PML_ARRAY_CONFIGURATION_HPP_INCLUDED
