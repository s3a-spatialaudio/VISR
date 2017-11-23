/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "interpolation_parameter.hpp"

#include <initializer_list>

namespace visr
{
namespace pml
{

namespace // unnamed
{
} // unnamed namespace

InterpolationParameterConfig::InterpolationParameterConfig( std::size_t numberOfInterpolants )
  : mNumberOfInterpolants( numberOfInterpolants )
{
}

InterpolationParameterConfig::~InterpolationParameterConfig() = default;

std::unique_ptr< ParameterConfigBase > InterpolationParameterConfig::clone() const
{
  return std::unique_ptr< InterpolationParameterConfig >( new InterpolationParameterConfig( *this ) );
}

bool InterpolationParameterConfig::compare( ParameterConfigBase const & rhs ) const
{
  try
  {
    // InterpolationParameterConfig const & iParam = dynamic_cast<InterpolationParameterConfig const &>(rhs);
    return true; // compare( iParam );
  }
  catch( std::bad_cast const & /*ex*/ )
  {
    throw std::invalid_argument( "InterpolationParameterConfig::compare(): Called with incompatible config object." );
  }
}

bool InterpolationParameterConfig::compare( InterpolationParameterConfig const & rhs ) const
{
  return rhs.numberOfInterpolants() == numberOfInterpolants();
}

InterpolationParameter::InterpolationParameter( InterpolationParameter const & rhs ) = default;

InterpolationParameter::InterpolationParameter( std::size_t numberOfInterpolants )
  : mIndices( numberOfInterpolants, cInvalidIndex )
  , mWeights( numberOfInterpolants, static_cast<WeightType>(0.0) )
{
}

InterpolationParameter::InterpolationParameter(ParameterConfigBase const & config)
  : InterpolationParameter( dynamic_cast<InterpolationParameterConfig const &>(config) )
{
}

InterpolationParameter::InterpolationParameter( InterpolationParameterConfig const & config )
  : InterpolationParameter( config.numberOfInterpolants() )
{
}


InterpolationParameter::InterpolationParameter( IndexContainer const & indices,
                                                WeightContainer const & weights )
  : mIndices( indices )
  , mWeights( weights )
{
  if( indices.size() != weights.size() )
  {
    throw std::invalid_argument( "InterpolationParameter: The \"indices\" and \"weights\" parameter must have the same length." );
  }
}

InterpolationParameter::InterpolationParameter( std::initializer_list<IndexType> const & indices,
                                                std::initializer_list<WeightType> const & weights )
  : mIndices( indices.begin(), indices.end() )
  , mWeights( weights.begin(), weights.end() )
{
  if( indices.size() != weights.size() )
  {
    throw std::invalid_argument( "InterpolationParameter: The \"indices\" and \"weights\" parameter must have the same length." );
  }
}

InterpolationParameter::~InterpolationParameter() = default;

std::size_t InterpolationParameter::numberOfInterpolants() const
{
  return mWeights.size();
}

InterpolationParameter::IndexType InterpolationParameter::index( std::size_t idx ) const
{
  return mIndices.at( idx );
}

InterpolationParameter::IndexContainer const & InterpolationParameter::indices() const
{
  return mIndices;
}

InterpolationParameter::WeightContainer const & InterpolationParameter::weights() const
{
  return mWeights;
}

InterpolationParameter::WeightType InterpolationParameter::weight( std::size_t idx ) const
{
  return mWeights.at( idx );
}

void InterpolationParameter::setIndex( std::size_t idx, IndexType newIndex )
{
  mIndices.at( idx ) = newIndex;
}

void InterpolationParameter::setIndices( IndexContainer const & newIndices )
{
  if( newIndices.size() != numberOfInterpolants() )
  {
    throw std::invalid_argument( "InterpolationParameter::setIndices(): new index vector has wrong number of elements." );
  }
  mIndices = newIndices;
}

void InterpolationParameter::setIndices( std::initializer_list<IndexType> const & newIndices )
{
  if( newIndices.size() != numberOfInterpolants() )
  {
    throw std::invalid_argument( "InterpolationParameter::setIndices(): new index vector has wrong number of elements." );
  }
  mIndices.assign( newIndices.begin(), newIndices.end() );
}

void InterpolationParameter::setWeight( std::size_t idx, WeightType weight )
{
  mWeights.at( idx ) = weight;
}

void InterpolationParameter::setWeights( WeightContainer const & newWeights )
{
  if( newWeights.size() != numberOfInterpolants() )
  {
    throw std::invalid_argument( "InterpolationParameter::setWeights(): new weight vector has wrong number of elements." );
  }
  mWeights = newWeights;
}

void InterpolationParameter::setWeights( std::initializer_list<WeightType> const & newWeights )
{
  if( newWeights.size() != numberOfInterpolants() )
  {
    throw std::invalid_argument( "InterpolationParameter::setWeights(): new weight vector has wrong number of elements." );
  }
  mWeights.assign( newWeights.begin(), newWeights.end() );
}

} // namespace pml
} // namespace visr
