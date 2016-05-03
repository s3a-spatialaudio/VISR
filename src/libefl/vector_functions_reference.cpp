/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_functions_reference_impl.hpp"

#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
// #include <boost/mpl/vector.hpp>
// For testing
#include <iostream>

namespace visr
{
namespace efl
{
  namespace reference
  {

#define FLOAT_TYPES (float)(double)
#define NUMERIC_TYPES (float)(double)(std::complex<float>)(std::complex<double>)

// using NumericTypesList = boost::mpl::vector<BOOST_PP_SEQ_ENUM(NUMERIC_TYPES)>;

#define EXPLICIT_INSTANTIATION_FOR_TYPE_LIST( functionName, d, typeList ) \
  BOOST_PP_CAT( template ErrorCode, functionName);
//   #define __FUNCTION_MACRO__( r, d, __type__ ) template ErrorCode  functionName; \
//   BOOST_PP_SEQ_FOR_EACH( __FUNCTION_MACRO__, _, types )
//   #undef __FUNCTION_MACRO__

//Here we should use our API macro
#define EXPLICIT_INSTANTIATION_VECTOR_ZERO(r, d, __type__) \
  template ErrorCode vectorZero<__type__>( __type__ * const, std::size_t, std::size_t );

#define EXPLICIT_INSTANTIATION_FUNCTION(r, d, __type__) \
  template ErrorCode vectorZero<__type__>( __type__ * const, std::size_t, std::size_t );



// class Foo {
// public:
//   template<class T> void read(T& value) { std::cout << value; }
// };

// #define EXPLICIT_INSTANTIATION(r, d, __type__) \
//   template void Foo::read<__type__>(__type__&);

// BOOST_PP_SEQ_FOR_EACH(EXPLICIT_INSTANTIATION, _, FLOAT_TYPES)

BOOST_PP_SEQ_FOR_EACH(EXPLICIT_INSTANTIATION_VECTOR_ZERO, _, NUMERIC_TYPES)


// template ErrorCode vectorZero<float>( float * const, std::size_t, std::size_t );
// template ErrorCode vectorZero<double>( double * const, std::size_t, std::size_t );
// template ErrorCode vectorZero<std::complex<float> >( std::complex<float> * const, std::size_t, std::size_t );
// template ErrorCode vectorZero<std::complex<double> >( std::complex<double> * const, std::size_t, std::size_t );

template ErrorCode vectorFill<float>( float const, float * const, std::size_t, std::size_t );
template ErrorCode vectorFill<double>( double const, double * const, std::size_t, std::size_t );
template ErrorCode vectorFill<std::complex<float> >( std::complex<float> const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorFill<std::complex<double> >( std::complex<double> const, std::complex<double> * const, std::size_t, std::size_t );


// explicit instantiations
template ErrorCode vectorRamp( float * const, std::size_t, float, float, bool, bool, std::size_t );
template ErrorCode vectorRamp( double * const, std::size_t, double, double, bool, bool, std::size_t );

template ErrorCode vectorCopy<float>( float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorCopy<double>( double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorCopy<std::complex<float> >( std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorCopy<std::complex<double> >( std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorAdd<float>( float const * const, float const * const ,
               float * const, std::size_t, std::size_t );
template ErrorCode vectorAdd<double>( double const * const, double const * const ,
          double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorAddInplace<float>( float const * const,
                                            float * const, std::size_t, std::size_t);
template ErrorCode vectorAddInplace<double>( double const * const,
                                             double * const, std::size_t, std::size_t);

/** Explicit instantiation for types float and double */
template ErrorCode vectorAddConstant<float>( float, float const * const,
                               float * const, std::size_t, std::size_t);
template ErrorCode vectorAddConstant<double>( double, double const * const,
                                            double * const, std::size_t, std::size_t);

/** Explicit instantiation for types float and double */
template ErrorCode vectorAddConstantInplace<float>( float, float * const, std::size_t, std::size_t );
template ErrorCode vectorAddConstantInplace<double>( double, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorSubtract( float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubtract( double const * const, double const * const, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorSubtractInplace( float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubtractInplace( double const * const, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorSubtractConstant( float, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubtractConstant( double, double const * const, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorSubConstantInplace( float, float * const, std::size_t, std::size_t );
template ErrorCode vectorSubConstantInplace( double, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiply( float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiply( double const * const, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiply( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiply( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyInplace( float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyInplace( double const * const, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstant( float, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstant( double, double const * const, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstantInplace( float, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantInplace( double, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyAdd( float const * const, float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAdd( double const * const, double const * const, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAdd( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAdd( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyAddInplace( float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAddInplace( double const * const, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAddInplace( std::complex<float> const * const, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyAddInplace( std::complex<double> const * const, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstantAdd( float, float const * const, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAdd( double, double const * const, double const * const, double * const, std::size_t, std::size_t );

/** Explicit instantiation for types float and double */
template ErrorCode vectorMultiplyConstantAddInplace( float, float const * const, float * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAddInplace( double, double const * const, double * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAddInplace( std::complex<double>, std::complex<double> const * const, std::complex<double> * const, std::size_t, std::size_t );
template ErrorCode vectorMultiplyConstantAddInplace( std::complex<float>, std::complex<float> const * const, std::complex<float> * const, std::size_t, std::size_t );

} // namespace reference
} // namespace efl
} // namespace visr
