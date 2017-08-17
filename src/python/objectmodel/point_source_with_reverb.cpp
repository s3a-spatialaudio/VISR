/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libobjectmodel/point_source_with_reverb.hpp>
#include <libobjectmodel/point_source.hpp>

#include <libpml/biquad_parameter.hpp>

#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <ciso646>
#include <stdexcept>
#include <vector>

namespace py = pybind11;

namespace visr
{
namespace objectmodel
{
namespace python
{

namespace // unnamed
{

void setReflectionPosition( PointSourceWithReverb::DiscreteReflection & obj, std::vector<PointSourceWithReverb::Coordinate> const & pos )
{
  if( pos.size() < 2 or pos.size() > 3 )
  {
    throw std::invalid_argument( "PointSourceWithReverb::DiscreteReflection: argument must be either a 2- or 3-element vector." );
  }
  PointSourceWithReverb::Coordinate const z = pos.size() == 3 ? pos[2] : static_cast<PointSourceWithReverb::Coordinate>(0.0);
  obj.setPosition( pos[0], pos[1], z );
}

std::vector<PointSourceWithReverb::Coordinate> getReflectionPosition( PointSourceWithReverb::DiscreteReflection & obj )
{
  std::vector<PointSourceWithReverb::Coordinate> pos( {obj.positionX(), obj.positionY(), obj.positionZ() } );
  return pos;
}

void setDiscreteReflections( PointSourceWithReverb & obj, std::vector<PointSourceWithReverb::DiscreteReflection> const & newReflections )
{
  std::size_t const numRefl = newReflections.size();
  obj.setNumberOfDiscreteReflections( numRefl );
  for( std::size_t idx(0); idx < numRefl; ++idx )
  {
    obj.discreteReflection( idx ) = newReflections[idx];
  }
}


pybind11::array_t<float> getVector( PointSourceWithReverb::LateReverbCoeffs const & coeffs )
{
  return pybind11::array_t<float>( coeffs.size(), coeffs.empty() ? nullptr : &coeffs[0] );
}

using CoeffSetter = void (PointSourceWithReverb::LateReverb::* )(PointSourceWithReverb::LateReverbCoeffs const & );

void setArray( PointSourceWithReverb::LateReverb & param,
               CoeffSetter method,
               pybind11::array_t<float> const & values )
{
  if( values.shape()[0] != PointSourceWithReverb::cNumberOfSubBands )
  {
    throw std::invalid_argument( "PointSourceWithReverb::LateReverb::setArray() called with wrong array length." );
  }
  PointSourceWithReverb::LateReverbCoeffs coeffs;
  for( std::size_t idx(0); idx < PointSourceWithReverb::cNumberOfSubBands; ++idx )
  {
    coeffs[idx] = * values.data(idx);
  }
  (param.*method)( coeffs );
}

} // unnamed ns

void exportPointSourceWithReverb( pybind11::module & m )
{
  py::class_<PointSourceWithReverb, PointSource> psReverb( m, "PointSourceWithReverb" );
  psReverb.def( py::init<>(), "Default constructor" )
    .def( py::init<ObjectId>() )
    .def_property_readonly( "lateReverb",
      static_cast<PointSourceWithReverb::LateReverb const&(PointSourceWithReverb::*)()const>(&PointSourceWithReverb::lateReverb),
      py::return_value_policy::reference )
    .def( "numberOfDiscreteReflections", &PointSourceWithReverb::numberOfDiscreteReflections )
    .def( "setNumberOfDiscreteReflections", &PointSourceWithReverb::setNumberOfDiscreteReflections, py::arg( "newNumber" ) )
    .def( "discreteReflection", static_cast<PointSourceWithReverb::DiscreteReflection&(PointSourceWithReverb::*)(std::size_t)>(&PointSourceWithReverb::discreteReflection),
      py::arg( "idx" ), py::return_value_policy::reference )
    .def( "setDiscreteReflections", &setDiscreteReflections, py::arg( "newReflections" ) )
  .def_property_readonly_static("numberOfDiscreteReflectionEqSections", []( py::object self ){ return PointSourceWithReverb::cNumDiscreteReflectionBiquads; }, "Number of wall reflection biquad sections." )
  .def_property_readonly_static("numberOfLateReflectionSubbands", []( py::object self ){ return PointSourceWithReverb::cNumberOfSubBands; }, "Number of filter subbands for the late reverb filter parameter." )
  ;

  py::class_<PointSourceWithReverb::DiscreteReflection>( psReverb, "DiscreteReflection" )
    .def( py::init<>() )
    .def( py::init<PointSourceWithReverb::DiscreteReflection const &>() )
    .def_property_readonly( "x", &PointSourceWithReverb::DiscreteReflection::positionX )
    .def_property_readonly( "y", &PointSourceWithReverb::DiscreteReflection::positionY )
    .def_property_readonly( "z", &PointSourceWithReverb::DiscreteReflection::positionZ )
    .def_property( "position", &getReflectionPosition, &setReflectionPosition )

    .def_property( "level", &PointSourceWithReverb::DiscreteReflection::level, &PointSourceWithReverb::DiscreteReflection::setLevel )
    .def_property( "delay", &PointSourceWithReverb::DiscreteReflection::delay, &PointSourceWithReverb::DiscreteReflection::setDelay )
    .def_property( "reflectionFilters", &PointSourceWithReverb::DiscreteReflection::reflectionFilters, &PointSourceWithReverb::DiscreteReflection::setReflectionFilters )
    .def( "reflectionFilter", &PointSourceWithReverb::DiscreteReflection::reflectionFilter,
      py::arg( "biquadIdx" ) )
    .def( "setReflectionFilter", &PointSourceWithReverb::DiscreteReflection::setReflectionFilter,
      py::arg( "biquadIdx" ), py::arg( "newFilter") )
  ;

  py::class_<PointSourceWithReverb::LateReverb>( psReverb, "LateReverb" )
  .def( py::init<>() ) // Default constructor
  .def( py::init<PointSourceWithReverb::LateReverb const &>(), py::arg( "rhs" ) )
  .def( py::init<SampleType, std::initializer_list<SampleType> const &,
        std::initializer_list<SampleType> const &, std::initializer_list<SampleType> const &>(),
        py::arg("onsetDelay"), py::arg("levels"), py::arg("attackTimes"), py::arg("decayCoeffs") )
  .def_property( "onsetDelay", &PointSourceWithReverb::LateReverb::onsetDelay, &PointSourceWithReverb::LateReverb::setOnsetDelay )
  .def_property( "levels", [](PointSourceWithReverb::LateReverb & inst ){ return getVector( inst.levels() );},
    [](PointSourceWithReverb::LateReverb & inst, pybind11::array_t<float> const & val ){ setArray( inst, &PointSourceWithReverb::LateReverb::setLevels, val ); },
    pybind11::return_value_policy::reference )
  .def_property( "attackTimes", [](PointSourceWithReverb::LateReverb & inst ){ return getVector( inst.attackTimes() );},
    [](PointSourceWithReverb::LateReverb & inst, pybind11::array_t<float> const & val ){ setArray( inst, &PointSourceWithReverb::LateReverb::setAttackTimes, val ); },
    pybind11::return_value_policy::reference )
  .def_property( "decayCoefficients", [](PointSourceWithReverb::LateReverb & inst ){ return getVector( inst.decayCoeffs() );},
    [](PointSourceWithReverb::LateReverb & inst, pybind11::array_t<float> const & val ){ setArray( inst, &PointSourceWithReverb::LateReverb::setDecayCoeffs, val ); },
    pybind11::return_value_policy::reference  )
  ;
}

} // namespace python
} // namepace objectmodel
} // namespace visr
