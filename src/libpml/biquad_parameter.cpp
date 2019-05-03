/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "biquad_parameter.hpp"

#include <libvisr/detail/compose_message_string.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <istream>
#include <iostream>
#include <sstream>
#include <string>

namespace visr
{
namespace pml
{

template<typename CoeffType>
BiquadParameterMatrix<CoeffType>::BiquadParameterMatrix( MatrixParameterConfig const & config )
  : BiquadParameterMatrix( config.numberOfRows(), config.numberOfColumns() )
{
}

template<typename CoeffType>
BiquadParameterMatrix<CoeffType>::BiquadParameterMatrix( ParameterConfigBase const & config )
  : BiquadParameterMatrix( dynamic_cast<MatrixParameterConfig const &>( config ) )
{
}

template<typename CoeffType>
BiquadParameterMatrix<CoeffType>::~BiquadParameterMatrix() = default;

// explicit instantiation
// template class BiquadParameter<float>;
// template class BiquadParameterList<float>;
template class BiquadParameterMatrix<float>;

// template class BiquadParameter<double>;
// template class BiquadParameterList<double>;
template class BiquadParameterMatrix<double>;

} // namespace pml
} // namespace visr
