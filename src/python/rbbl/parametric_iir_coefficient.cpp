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
    py::class_<ParametricIirCoefficient<CoeffType> >( m, className )
      .def( py::init<>(), "Default constructor" )
      .def( py::init<ParametricIirCoefficient<CoeffType> const & >(), py::arg( "rhs" ) )
      .def( py::init< ParametricIirCoefficient<CoeffType>::Type, CoeffType, CoeffType, CoeffType>(),
         py::arg("type"),
         py::arg("centreFrequency"), py::arg("quality"), py::arg("gain") = static_cast<CoeffType>(0.0) )
      .def_property( "type", &ParametricIirCoefficient<CoeffType>::type, &ParametricIirCoefficient<CoeffType>::setType )
#if 0
    CoefficientType frequency() const { return mFrequency; }

    CoefficientType quality() const { return mQuality; }

    CoefficientType gain() const { return mGain; }

    void setType(Type newType);

    void setFrequency(CoefficientType newFrequency);

    void setQuality(CoefficientType newQuality);

    void setGain(CoefficientType newGain);

    /**
     * Create a ParametricIirCoefficient objects from JSON and XML representations.
     */
     //@{
    static ParametricIirCoefficient fromJson(boost::property_tree::ptree const & tree);

    static ParametricIirCoefficient fromJson(std::basic_istream<char> & stream);

    static ParametricIirCoefficient fromJson(std::string const & str);

    static ParametricIirCoefficient fromXml(boost::property_tree::ptree const & tree);

    static ParametricIirCoefficient fromXml(std::basic_istream<char> & stream);

    static ParametricIirCoefficient fromXml(std::string const & str);
    //@}


    ParametricIirCoefficient & operator=(ParametricIirCoefficient const & rhs) = default;

    void loadJson(boost::property_tree::ptree const & tree);

    void loadJson(std::basic_istream<char> & stream);

    void loadJson(std::string const & str);


    void loadXml(boost::property_tree::ptree const & tree);

    void loadXml(std::basic_istream<char> & stream);

    void loadXml(std::string const & str);


    /**
     *
     */
     //@{
    void writeJson(boost::property_tree::ptree & tree) const;

    void writeJson(std::basic_ostream<char> & stream) const;

    void writeJson(std::string & str) const;

    void writeXml(boost::property_tree::ptree & tree) const;

    void writeXml(std::basic_ostream<char> & stream) const;

    void writeXml(std::string & str) const;
#endif
    ;
  }

} // unnamed namespace

void exportParametricIirCoefficient( pybind11::module & m )
{
  py::class_<ParametricIirCoefficientBase > base(m, "ParametricIirCoefficientBase");
  base.def_static("stringTypeToId", &ParametricIirCoefficientBase::stringToTypeId, py::arg("name"))
      .def_static("typeToIdString", &ParametricIirCoefficientBase::typeIdToString, py::arg("typeId"));

  py::enum_<ParametricIirCoefficientBase::Type >(m, "Type")
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
