/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <librbbl/parametric_iir_coefficient.hpp>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <array>
#include <string>
#include <sstream>
#include <vector>

namespace visr
{
namespace rbbl
{
namespace python
{

namespace py = pybind11;

namespace 
{
  template< typename CoeffType >
  void exportParametricIirCoefficient( pybind11::module & m, char const * className )
  {
    py::class_<ParametricIirCoefficient<CoeffType>, ParametricIirCoefficientBase >( m, className )
      .def( py::init<>(), "Default constructor" )
      .def( py::init<ParametricIirCoefficient<CoeffType> const & >(), py::arg( "rhs" ) )
      .def( py::init< typename ParametricIirCoefficient<CoeffType>::Type, CoeffType, CoeffType, CoeffType>(),
         py::arg("type"),
         py::arg("frequency"), py::arg("quality"), py::arg("gain") = static_cast<CoeffType>(0.0) )
      .def( py::init( []( std::string const & typeStr, CoeffType frequency, CoeffType quality, CoeffType gain )
	    {
	      typename ParametricIirCoefficient<CoeffType>::Type const typeId{ParametricIirCoefficientBase::stringToTypeId( typeStr ) };
	      return new ParametricIirCoefficient<CoeffType>{ typeId, frequency, quality, gain };
	    } ),
            py::arg("type"),
            py::arg("frequency"), py::arg("quality"), py::arg("gain") = static_cast<CoeffType>(0.0) )

      .def( "__str__", [](ParametricIirCoefficient<CoeffType> const & self )
	    {
	      std::stringstream str;
	      str << ParametricIirCoefficientBase::typeIdToString(self.type()) << ", f: " << self.frequency()
	          << ", q: " << self.quality() << ", g: " << self.gain();
	      return str.str();
	    } )
      .def_property( "type", &ParametricIirCoefficient<CoeffType>::type, &ParametricIirCoefficient<CoeffType>::setType )
      .def_property( "frequency", &ParametricIirCoefficient<CoeffType>::frequency, &ParametricIirCoefficient<CoeffType>::setFrequency )
      .def_property( "quality", &ParametricIirCoefficient<CoeffType>::quality, &ParametricIirCoefficient<CoeffType>::setQuality )
      .def_property( "gain", &ParametricIirCoefficient<CoeffType>::gain, &ParametricIirCoefficient<CoeffType>::setGain )
      .def( "loadJson", static_cast<void(ParametricIirCoefficient<CoeffType>::*)(std::string const &)>
                   (&ParametricIirCoefficient<CoeffType>::loadJson ), py::arg("string") )
      .def( "loadXml", static_cast<void(ParametricIirCoefficient<CoeffType>::*)(std::string const &)>
                   (&ParametricIirCoefficient<CoeffType>::loadXml ), py::arg("string") )
      .def_static( "fromJson", static_cast<ParametricIirCoefficient<CoeffType>(*)(std::string const &)>
                    (&ParametricIirCoefficient<CoeffType>::fromJson ), py::arg("string") )
      .def_static( "fromXml", static_cast<ParametricIirCoefficient<CoeffType>(*)(std::string const &)>
                   (&ParametricIirCoefficient<CoeffType>::fromXml ), py::arg("string") )
      .def( "writeJson", static_cast<void(ParametricIirCoefficient<CoeffType>::*)(std::string &) const>
                   (&ParametricIirCoefficient<CoeffType>::writeJson ), py::arg("string") )
      .def( "writeXml", static_cast<void(ParametricIirCoefficient<CoeffType>::*)(std::string &) const>
                   (&ParametricIirCoefficient<CoeffType>::writeXml ), py::arg("string") )

    ;
  }

} // unnamed namespace

void exportParametricIirCoefficient( pybind11::module & m )
{
  py::class_<ParametricIirCoefficientBase > base(m, "ParametricIirCoefficientBase");
  base.def_static("stringToTypeId", &ParametricIirCoefficientBase::stringToTypeId, py::arg("name"))
      .def_static("typeToIdString", &ParametricIirCoefficientBase::typeIdToString, py::arg("typeId"));

  py::enum_<ParametricIirCoefficientBase::Type >(base, "Type")
    .value("lowpass", ParametricIirCoefficientBase::Type::lowpass)
    .value("highpass", ParametricIirCoefficientBase::Type::highpass)
    .value("bandpass", ParametricIirCoefficientBase::Type::bandpass)
    .value("bandstop", ParametricIirCoefficientBase::Type::bandstop)
    .value("highshelf", ParametricIirCoefficientBase::Type::highshelf)
    .value("lowshelf", ParametricIirCoefficientBase::Type::lowshelf)
    .value("peak", ParametricIirCoefficientBase::Type::peak)
    .value("allpass", ParametricIirCoefficientBase::Type::allpass);

  exportParametricIirCoefficient<float>( m, "ParametricIirCoefficientFloat" );
  exportParametricIirCoefficient<double>( m, "ParametricIirCoefficientDouble" );
}

} // namespace python
} // namepace rbbl
} // namespace visr
