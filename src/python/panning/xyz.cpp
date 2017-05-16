/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/XYZ.h>
#include <libpanning/defs.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{
	using panning::XYZ;
	namespace python
	{
		namespace panning
		{
			void exportXYZ(pybind11::module & m)
			{
				pybind11::class_<XYZ>(m, "XYZ")
					.def(pybind11::init<Afloat, Afloat, Afloat, bool>(), pybind11::arg("posX"), pybind11::arg("posY"), pybind11::arg("posZ"), pybind11::arg("bool") = false)
					.def("set", &XYZ::set, pybind11::arg("posX"), pybind11::arg("posY"), pybind11::arg("posZ"), pybind11::arg("bool") = false)
					.def("getLength", &XYZ::getLength)
					.def("normalse", &XYZ::normalise)
					; 

							}
			
		} // namepace panning
	} // namespace python
} // namespace visr
