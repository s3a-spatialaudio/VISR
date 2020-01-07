/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "time.hpp"

#include <libvisr/impl/time_implementation.hpp>

namespace visr
{

Time::Time( std::shared_ptr<impl::TimeImplementation> impl )
 : mImpl( impl )
{}

Time::~Time() = default;

std::size_t Time::sampleCount() const
{
  return mImpl->sampleCount();
}

std::size_t Time::blockCount() const
{
  return mImpl->blockCount();
}

Time::ContinuousTimeType
Time::renderingTime() const
{
  return mImpl->renderingTime();
}

impl::TimeImplementation const &
Time::implementation() const
{
  return *mImpl;
}

impl::TimeImplementation &
Time::implementation()
{
  return *mImpl;
}

} // namespace visr
