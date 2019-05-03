/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "external_base.hpp"

namespace visr
{
namespace maxmsp
{

ExternalBase::ExternalBase( t_pxobject & maxProxy )
 : mMaxProxy( maxProxy )
{
}

ExternalBase::~ExternalBase( )
{
}

} // namespace visr
} // namespace maxmsp
