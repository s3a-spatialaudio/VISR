/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/object_type.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportObjectType( pybind11::module & m )
{
  py::enum_<ObjectTypeId>(m, "ObjectType")
    .value( "PointSource", ObjectTypeId::PointSource)
    .value( "PlaneWave", ObjectTypeId::PlaneWave )
    .value( "DiffuseSource", ObjectTypeId::DiffuseSource )
    .value( "PointSourceWithDiffuseness", ObjectTypeId::PointSourceWithDiffuseness )
    .value( "ExtendedSource", ObjectTypeId::ExtendedSource )
    .value( "PointSourceWithReverb", ObjectTypeId::PointSourceWithReverb )
    .value( "PointSourceExtent", ObjectTypeId::PointSourceExtent )
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr

