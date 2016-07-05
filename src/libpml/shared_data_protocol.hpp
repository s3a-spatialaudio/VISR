/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED
#define VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED

#include <libril/communication_protocol_base.hpp>
#include <libril/communication_protocol_type.hpp>

#include <libril/parameter_type.hpp>
#include <libril/parameter_config_base.hpp>

// Temporary hack
#include <libpml/string_parameter.hpp>

#include <deque>
#include <stdexcept>

namespace visr
{
namespace pml
{

/**
 * A FIFO-type message queue template class for storing and passing message data.
 * @tparam MessageTypeT Type of the contained elements.
 * @note This class does provide the same level of thread safety as, e.g., the STL.
 * I.e., calling code from different thread must ensure that concurrent accesses
 * to the same instances are appropriately secured against race conditions.
 */
template< typename MessageTypeT >
class SharedDataProtocol: public ril::CommunicationProtocolBase
{
public:

  /**
   * Make the message type available to code using this template.
   */
  using MessageType = MessageTypeT;

  class Input
  {
  public:
    /**
    * Default constructor.
    */
    Input( ) {}

    MessageType data( ) const
    {
      return mParent->data( );
    }
  private:
    SharedDataProtocol * mParent;
  };


  /**
   * Provide alias for parameter configuration class type for the contained parameter values.
   */
  using ParameterConfigType = typename ril::ParameterToConfigType<MessageTypeT>::ConfigType;

  class Output
  {
  public:
    /**
     * Default constructor.
     */
    Output() {}

    MessageType & data()
    {
      return mParent->data( );
    }

    /**
     * This whould not be accessible from the component.
     */
  private:
    SharedDataProtocol * mParent;
  };

  explicit SharedDataProtocol( ril::ParameterConfigBase const & config );

  explicit SharedDataProtocol( ParameterConfigType const & config );

  ril::ParameterType type() const override { return ril::ParameterToId<MessageType>::id; }

  MessageType & data()
  {
    return mData;
  }

  MessageType const & data( ) const
  {
    return mData;
  }

  void setData( MessageType const & newData )
  {
    // TODO: Check compatibility
    // call assign operator
  }

private:
  ParameterConfigType const mConfig;

  /**
   * The internal data representation.
   */
  MessageTypeT mData;
};

template< typename MessageTypeT >
inline SharedDataProtocol< MessageTypeT >::SharedDataProtocol( ril::ParameterConfigBase const & config )
: SharedDataProtocol( dynamic_cast<ParameterConfigType const &>(config) )
{
}

template< typename MessageTypeT >
inline SharedDataProtocol< MessageTypeT >::SharedDataProtocol( ParameterConfigType const & config )
  : mConfig( config )
  , mData( config )
{
}

} // namespace pml
} // namespace visr

DEFINE_COMMUNICATION_PROTOCOL_TYPE( visr::pml::SharedDataProtocol, visr::ril::CommunicationProtocolType::SharedData );

#endif // VISR_PML_SHARED_DATA_PROTOCOL_HPP_INCLUDED
