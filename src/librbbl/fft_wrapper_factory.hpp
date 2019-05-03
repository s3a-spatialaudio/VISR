/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FFT_WRAPPER_FACTORY_HPP_INCLUDED
#define VISR_LIBRBBL_FFT_WRAPPER_FACTORY_HPP_INCLUDED

#include "export_symbols.hpp"

#include <map>
#include <memory>
#include <string>

#include <boost/algorithm/string.hpp> // for string conversion to lowercase
#include <boost/function.hpp>

namespace visr
{
namespace rbbl
{

// forward declaration 
template<typename SampleType>
class FftWrapperBase;

/**
 * Factory class to construct FftWrapper objects of different types.
 * It is intended to be used only through its static function interface.
 * @tparam DataType The value type for the FFT implementations - generally float or double.
 */
template<typename SampleType> 
class VISR_RBBL_LIBRARY_SYMBOL FftWrapperFactory
{
public:
  /**
   * Creation function for FftWrapper objects.
   * @param wrapperName The name of the FFT library to be instantiated, or "default"
   * to instantiate the default choice for this platform.
   * @param fftSize The size of the FFTs (number of real samples used as input to the forward FFT).
   * @param alignElements Alignment of the vectors passed to the FFT (in number of samples).
   */
  static std::unique_ptr<FftWrapperBase<SampleType> >
  create( std::string const & wrapperName, std::size_t fftSize, std::size_t alignElements );

  /**
   * Template function to register a FFT wrapper withing the factory.
   */
  template< class FftWrapper >
  static void registerWrapper( std::string const & wrapperName );

  /**
   * Return a list of strings containing the names of the FFT wrappers available on this platform 
   * (and for this data type).
   */
  static std::string listImplementations();

private:
  struct Creator
  {
    using CreateFunction = boost::function< FftWrapperBase<SampleType>* ( std::size_t, std::size_t ) >;

    explicit Creator( CreateFunction fcn );

    std::unique_ptr< FftWrapperBase<SampleType> > create( std::size_t fftSize,
                                                          std::size_t alignElements) const;
  private:
    CreateFunction mCreateFunction;
  };

  template< class WrapperType >
  class TCreator: public Creator
  {
  public:
    TCreator( )
      : Creator( &TCreator<WrapperType>::construct )
    {
    }

    static WrapperType* construct( std::size_t fftSize, std::size_t alignmentElement )
    {
      return new WrapperType( fftSize, alignmentElement );
    }
  };

  using CreatorTable = std::map<std::string, Creator >;

  static CreatorTable & creatorTable();

  /**
   * Private constructor without implementation to prevent instantiation of this class.
   */
  FftWrapperFactory();
};

template< typename SampleType >
template< class WrapperType >
void FftWrapperFactory<SampleType>::registerWrapper( std::string const & wrapperName )
{
  std::string lowerName(wrapperName); // convert the name to lower case.
  boost::algorithm::to_lower(lowerName);
  creatorTable().insert( std::make_pair( lowerName,
                         TCreator<WrapperType>() ) );
}

} // namespace rbbl
} // namespace visr

#endif // #ifndef VISR_LIBRBBL_FFT_WRAPPER_FACTORY_HPP_INCLUDED
