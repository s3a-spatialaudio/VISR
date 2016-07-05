/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_OBJECT_VECTOR_HPP_INCLUDED
#define VISR_PML_OBJECT_VECTOR_HPP_INCLUDED

#include "empty_parameter_config.hpp"

#include <libobjectmodel/object_vector.hpp>

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>


#include <cstdint>
#include <iosfwd>
#include <istream>

namespace visr
{
namespace pml
{

class ObjectVector: public objectmodel::ObjectVector,
                    public ril::TypedParameterBase<EmptyParameterConfig, ril::ParameterType::ObjectVector >
{
public:
  explicit ObjectVector( ril::ParameterConfigBase const & config );

  explicit ObjectVector( EmptyParameterConfig const & config );

  virtual ~ObjectVector() override;
};

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::ObjectVector, visr::ril::ParameterType::ObjectVector, visr::pml::EmptyParameterConfig )

#endif // VISR_PML_OBJECT_VECTOR_HPP_INCLUDED
