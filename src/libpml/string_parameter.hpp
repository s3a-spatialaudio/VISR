/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_STRING_PARAMETER_HPP_INCLUDED
#define VISR_PML_STRING_PARAMETER_HPP_INCLUDED

#include "string_parameter_config.hpp"

#include <libefl/basic_matrix.hpp>

#include <libril/parameter_type.hpp>
#include <libril/typed_parameter_base.hpp>

#include <initializer_list>
#include <istream>
#include <string>

namespace visr
{
  namespace pml
  {

    /**
     * A type for passing strings between processing components.
     */
    class StringParameter: public std::string,
                           public ril::TypedParameterBase<StringParameterConfig, ril::ParameterType::String >
    {
    public:
      explicit StringParameter( std::size_t maxLength = 0 );

      explicit StringParameter( ril::ParameterConfigBase const & config );

      explicit StringParameter( StringParameterConfig const & config );

      /**
       * @note Reconsider and possibly remove.
       */
      explicit StringParameter( std::string const & initStr );

      /**
       * Return the maximum length.
       */
      std::size_t maxLength() const { return mMaxLength; }

    private:
      std::size_t mMaxLength;
    };

} // namespace pml
} // namespace visr

DEFINE_PARAMETER_TYPE( visr::pml::StringParameter, visr::ril::ParameterType::String, visr::pml::StringParameterConfig )

#endif // VISR_PML_STRING_PARAMETER_HPP_INCLUDED
