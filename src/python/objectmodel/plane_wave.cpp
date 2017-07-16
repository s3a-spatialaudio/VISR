/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/plane_wave.hpp>

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

void exportPlaneWave( pybind11::module & m )
{
  py::class_<PlaneWave, Object>( m, "PlaneWave" )
    .def( py::init<>(), "Default constructor" )
    .def( py::init<ObjectId>() )
    .def_property( "azimuth", &PlaneWave::incidenceAzimuth, &PlaneWave::setIncidenceAzimuth )
    .def_property( "elevation", &PlaneWave::incidenceElevation, &PlaneWave::setIncidenceElevation )
    .def_property( "referenceDistance", &PlaneWave::referenceDistance, &PlaneWave::setReferenceDistance )
    ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr

