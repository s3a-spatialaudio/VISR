/* Copyright 2019 AudioScenic Ltd - All rights reserved */

#include "filter_functions_reference_impl.hpp"

namespace visr
{
namespace efl
{
namespace reference
{

ErrorCode iirFilterBiquadsSingleChannel( float const *, float *,
					 float *, float const *,
					 std::size_t, std::size_t, std::size_t,
					 std::size_t, std::size_t );
ErrorCode iirFilterBiquadsSingleChannel( double const *, double *,
					 double *, double const *,
					 std::size_t, std::size_t, std::size_t,
					 std::size_t, std::size_t );

} // namespace reference
} // namespace efl
} // namespace visr
