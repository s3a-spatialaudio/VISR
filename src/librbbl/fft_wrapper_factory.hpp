/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#ifndef VISR_LIBRBBL_FFT_WRAPPER_FACTORY_HPP_INCLUDED
#define VISR_LIBRBBL_FFT_WRAPPER_FACTORY_HPP_INCLUDED

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

template<typename SampleType> 
class FftWrapperFactory
{
public:
  static std::unique_ptr<FftWrapperBase<SampleType> >
  create( std::string const & wrapperName, std::size_t fftSize, std::size_t alignElements );

  template< class FftWrapper >
  static void registerWrapper( std::string const & wrapperName );

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
