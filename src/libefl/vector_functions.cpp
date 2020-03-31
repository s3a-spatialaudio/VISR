/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "vector_functions.hpp"

#include "function_wrapper.hpp"

#include "reference/vector_functions.hpp"

namespace visr
{
  namespace efl
  {

    /**
     * Convenience macro to define the function pointers (explicit template instantiations)
     * for a given vector function and a data type.
     */
#define EFL_FUNCTION_WRAPPER_INSTANTIATION( Wrapper, DataType, referenceFunction )\
template<> VISR_EFL_LIBRARY_SYMBOL decltype(Wrapper< DataType >::sPtr) Wrapper< DataType >::sPtr{ &referenceFunction< DataType > };

     // TODO: Consider use of boost preprocessor.

     // Instantiate the funtion pointers for the different explicit instantiations of vectorZero.
    EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorZeroWrapper, float, reference::vectorZero)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorZeroWrapper, double, reference::vectorZero)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorZeroWrapper, std::complex<float>, reference::vectorZero)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorZeroWrapper, std::complex<double>, reference::vectorZero)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillWrapper, float, reference::vectorFill)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillWrapper, double, reference::vectorFill)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillWrapper, std::complex<float>, reference::vectorFill)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillWrapper, std::complex<double>, reference::vectorFill)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampWrapper, float, reference::vectorRamp)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampWrapper, double, reference::vectorRamp)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampWrapper, std::complex<float>, reference::vectorRamp)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampWrapper, std::complex<double>, reference::vectorRamp)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, float, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, double, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, long double, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, int8_t, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, uint8_t, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, int16_t, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, uint16_t, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, int32_t, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, uint32_t, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, std::complex<float>, reference::vectorCopy)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyWrapper, std::complex<double>, reference::vectorCopy)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddWrapper, float, reference::vectorAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddWrapper, double, reference::vectorAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddWrapper, std::complex<float>, reference::vectorAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddWrapper, std::complex<double>, reference::vectorAdd)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddInplaceWrapper, float, reference::vectorAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddInplaceWrapper, double, reference::vectorAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddInplaceWrapper, std::complex<float>, reference::vectorAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddInplaceWrapper, std::complex<double>, reference::vectorAddInplace)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantWrapper, float, reference::vectorAddConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantWrapper, double, reference::vectorAddConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantWrapper, std::complex<float>, reference::vectorAddConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantWrapper, std::complex<double>, reference::vectorAddConstant)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantInplaceWrapper, float, reference::vectorAddConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantInplaceWrapper, double, reference::vectorAddConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantInplaceWrapper, std::complex<float>, reference::vectorAddConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorAddConstantInplaceWrapper, std::complex<double>, reference::vectorAddConstantInplace)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractWrapper, float, reference::vectorSubtract)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractWrapper, double, reference::vectorSubtract)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractWrapper, std::complex<float>, reference::vectorSubtract)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractWrapper, std::complex<double>, reference::vectorSubtract)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractInplaceWrapper, float, reference::vectorSubtractInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractInplaceWrapper, double, reference::vectorSubtractInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractInplaceWrapper, std::complex<float>, reference::vectorSubtractInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractInplaceWrapper, std::complex<double>, reference::vectorSubtractInplace)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantWrapper, float, reference::vectorSubtractConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantWrapper, double, reference::vectorSubtractConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantWrapper, std::complex<float>, reference::vectorSubtractConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantWrapper, std::complex<double>, reference::vectorSubtractConstant)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantInplaceWrapper, float, reference::vectorSubtractConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantInplaceWrapper, double, reference::vectorSubtractConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantInplaceWrapper, std::complex<float>, reference::vectorSubtractConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorSubtractConstantInplaceWrapper, std::complex<double>, reference::vectorSubtractConstantInplace)


      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyWrapper, float, reference::vectorMultiply)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyWrapper, double, reference::vectorMultiply)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyWrapper, std::complex<float>, reference::vectorMultiply)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyWrapper, std::complex<double>, reference::vectorMultiply)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyInplaceWrapper, float, reference::vectorMultiplyInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyInplaceWrapper, double, reference::vectorMultiplyInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyInplaceWrapper, std::complex<float>, reference::vectorMultiplyInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyInplaceWrapper, std::complex<double>, reference::vectorMultiplyInplace)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantWrapper, float, reference::vectorMultiplyConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantWrapper, double, reference::vectorMultiplyConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantWrapper, std::complex<float>, reference::vectorMultiplyConstant)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantWrapper, std::complex<double>, reference::vectorMultiplyConstant)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantInplaceWrapper, float, reference::vectorMultiplyConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantInplaceWrapper, double, reference::vectorMultiplyConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantInplaceWrapper, std::complex<float>, reference::vectorMultiplyConstantInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantInplaceWrapper, std::complex<double>, reference::vectorMultiplyConstantInplace)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddWrapper, float, reference::vectorMultiplyAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddWrapper, double, reference::vectorMultiplyAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddWrapper, std::complex<float>, reference::vectorMultiplyAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddWrapper, std::complex<double>, reference::vectorMultiplyAdd)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddInplaceWrapper, float, reference::vectorMultiplyAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddInplaceWrapper, double, reference::vectorMultiplyAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddInplaceWrapper, std::complex<float>, reference::vectorMultiplyAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyAddInplaceWrapper, std::complex<double>, reference::vectorMultiplyAddInplace)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddWrapper, float, reference::vectorMultiplyConstantAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddWrapper, double, reference::vectorMultiplyConstantAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddWrapper, std::complex<float>, reference::vectorMultiplyConstantAdd)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddWrapper, std::complex<double>, reference::vectorMultiplyConstantAdd)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddInplaceWrapper, float, reference::vectorMultiplyConstantAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddInplaceWrapper, double, reference::vectorMultiplyConstantAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddInplaceWrapper, std::complex<float>, reference::vectorMultiplyConstantAddInplace)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorMultiplyConstantAddInplaceWrapper, std::complex<double>, reference::vectorMultiplyConstantAddInplace)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyStridedWrapper, float, reference::vectorCopyStrided)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyStridedWrapper, double, reference::vectorCopyStrided)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyStridedWrapper, std::complex<float>, reference::vectorCopyStrided)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorCopyStridedWrapper, std::complex<double>, reference::vectorCopyStrided)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillStridedWrapper, float, reference::vectorFillStrided)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillStridedWrapper, double, reference::vectorFillStrided)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillStridedWrapper, std::complex<float>, reference::vectorFillStrided)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorFillStridedWrapper, std::complex<double>, reference::vectorFillStrided)

      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampScalingWrapper, float, reference::vectorRampScaling)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampScalingWrapper, double, reference::vectorRampScaling)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampScalingWrapper, std::complex<float>, reference::vectorRampScaling)
      EFL_FUNCTION_WRAPPER_INSTANTIATION(VectorRampScalingWrapper, std::complex<double>, reference::vectorRampScaling)

} // namespace efl
} // namespace visr
