/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include <libpanning/VBAP.h>
#include <libpanning/defs.h>
#include <libpanning/LoudspeakerArray.h>
#include <libpml/biquad_parameter.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace visr
{
	using panning::VBAP;
	using panning::LoudspeakerArray;
	namespace python
	{
		namespace panning
		{
			void exportVBAP(pybind11::module & m)
			{
				pybind11::class_<VBAP>(m, "VBAP")
					.def(pybind11::init<const LoudspeakerArray &, SampleType, SampleType, SampleType>(), pybind11::arg("loudspeakerArray"), pybind11::arg("listenerPosX"), pybind11::arg("listenerPosY"), pybind11::arg("listenerPosZ"))
					.def("calcGain", &VBAP::calculateGains, pybind11::arg("sourcePosX"), pybind11::arg( "sourcePosY" ) , pybind11::arg( "sourcePosZ" ) , pybind11::arg( "outGains" ) )
					;
			}
		} // namepace panning
	} // namespace python
} // namespace visr
