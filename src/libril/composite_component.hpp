/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
#define VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED

#include "component.hpp"

#include <set>

namespace visr
{
namespace ril
{

/**
 *
 *
 */
class CompositeComponent: public Component
{
public:

  explicit CompositeComponent( AudioSignalFlow& container, char const * name );

  /**
   * Destructor
   */
  ~CompositeComponent();

  /**
   * Query whether this component is composite.
   * @return true
   */
  virtual bool isComposite() override;

  struct CompareComponents
  {
  public:
    bool operator()(Component const * lhs, Component const * rhs)
    {
      return lhs->name() < rhs->name();
    }
  };

  using ComponentTable = std::set < Component const *, CompareComponents >;

  std::size_t numberOfComponents() const;

  ComponentTable::const_iterator componentBegin() const;

  ComponentTable::const_iterator componentEnd( ) const;



protected:
  void registerChildComponent( Component const * child );

private:
  ComponentTable mComponents;
};

} // namespace ril
} // namespace visr

#endif // #ifndef VISR_LIBRIL_COMPOSITE_COMPONENT_HPP_INCLUDED
