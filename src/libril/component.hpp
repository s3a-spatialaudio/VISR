/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_COMPONENT_HPP_INCLUDED
#define VISR_COMPONENT_HPP_INCLUDED

#include <libril/constants.hpp>

#include <cstddef>
#include <string>
#include <sstream>
#include <memory>

// TODO: Remove ASAP!
#include <iostream>

namespace visr
{

// Forward declaration(s)
class AudioPortBase;
class CompositeComponent;
class ParameterPortBase;
class SignalFlowContext;
enum class Status: unsigned char;

namespace impl
{
class ComponentImplementation;
}

/**
 *
 *
 */
class Component
{
public:

  /**
   * Constructor, constructs a component.
   */
  explicit Component( SignalFlowContext& context,
                      char const * componentName,
                      CompositeComponent * parent );

  /**
   * Constructor.
   * Convenvience function, accepts a standard string instead of a C chararacter pointer.
   */
  explicit Component( SignalFlowContext& context,
                      std::string const & componentName,
                      CompositeComponent * parent);

  /**
   * Deleted copy constructor to avoid copy construction of this and derived classes.
   */
  Component( Component const & ) = delete;

   /**
   * Deleted move constructor to avoid moving of this and derived classes.
   */
  Component( Component && ) = delete;

  /**
   * Deleted assignment operator to prohibit (copy) assignment of this and derived classes.
   */
  Component & operator=( Component const & ) = delete;

  /**
  * Deleted assignment operator to prohibit move assignment of this and derived classes.
  */
  Component & operator=( Component && ) = delete;

  /**
   * Separator used to form hierarchical names.
   */
  static const std::string & nameSeparator();

  /**
   * Destructor (virtual)
   */
  virtual ~Component();

  /**
   * Return the 'local', non-hierarchical name.
   */
  std::string const & name() const;

  /**
   * Return the full, hierarchical name of the component.
   */
  std::string fullName() const;

  /**
   * Signal informational messages or the error conditions.
   * Depending on the value of the \p status parameter, this might result
   * in a message conveyed to the user or abortion of the audio processing.
   * @param status The class of the status message
   * @param message An informational message string.
   */
  void status( Status status, char const * message )
  // Mock implementation, remove ASAP.
  {
    std::cout << message << std::endl;
  }


  template<typename ... MessageArgs >
  void status( Status statusId, MessageArgs ... args );


  /**
   * Query whether this component is atomic (i.e., a piece of code implementing a rendering 
   * functionality) or a composite consisting of an interconnection of atomic (or further composite) components.
   */
  bool isComposite() const;

  AudioPortBase& audioPort( char const * portName );

  AudioPortBase const& audioPort( char const * portName ) const;

  AudioPortBase& audioPort( std::string const & portName );

  AudioPortBase const& audioPort( std::string const & portName ) const;


  ParameterPortBase& parameterPort( char const * portName );

  ParameterPortBase const& parameterPort( char const * portName ) const;

  ParameterPortBase& parameterPort( std::string const & portName );

  ParameterPortBase const& parameterPort( std::string const & portName ) const;


  /**
   * Return the sampling frequency of the containing signal flow.
   */
  SamplingFrequencyType samplingFrequency() const;

  /**
   * Return the period of the containing signal processing graph,
   * i.e., the number of samples processed in each invocation of the
   * process function of the derived audio components.
   * This methods can be called at any point of the lifetime of the
   * derived component, i.e., for instance in the constructor.
   * @todo: Check whether this should be made inline again (adding the dependency to the runtime container (aka SignalFlow).
   */
  std::size_t period() const;

  //@}

  /**
   * Query whether the component is at the top level of a signal flow.
   * @note Not needed for user API
   */
  bool isTopLevel() const;

  impl::ComponentImplementation & implementation();

  impl::ComponentImplementation const & implementation() const;

protected:

  SignalFlowContext & context();
  SignalFlowContext const & context( ) const;

protected:
  /**
   * Constructor that receives the internal implementation object.
   * This overload has to be called by the other constructors (including those of subclasses) to make 
   * sure that the implementation object is instantiated.
   * The motivation for this constructor is to provide different implementation objects for different subclasses.
   */
  explicit Component( std::unique_ptr<impl::ComponentImplementation> && impl );

private:
  /**
   * Pointer to the private implementation object.
   * The type of the impl object might differ due to the actual type of the component.
   */
  std::unique_ptr<impl::ComponentImplementation> mImpl;
};

namespace
{

template< typename MessageType >
void write( std::ostream & str, MessageType const & msg )
{
  str << msg;
}

template< typename MessageType, typename... MessageRest>
void write( std::ostream & str, MessageType const & msg, MessageRest ... rest )
{
  str << msg;
  write( str, rest );
}

} // unnamed namespace

template<typename ... MessageArgs >
inline void Component::status( Status statusId, MessageArgs ... args )
{
  std::stringstream str;
  write( str, args );
  status( statusId, str.str() );
}

} // namespace visr

#endif // #ifndef VISR_COMPONENT_HPP_INCLUDED
