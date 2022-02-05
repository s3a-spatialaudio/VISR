
import pytest

import pml
import rbbl

import numpy as np


def biquadCoefficientParameter( dtype ):
    if dtype == np.float32:
        return pml.BiquadCoefficientParameterFloat
    elif dtype == np.float64:
        return pml.BiquadCoefficientParameterDouble
    else:
        raise ValueError("Unsupported numeric type")


def biquadCoefficient( dtype ):
    if dtype == np.float32:
        return rbbl.BiquadCoefficientFloat
    elif dtype == np.float64:
        return pml.BiquadCoefficientDouble
    else:
        raise ValueError("Unsupported numeric type")


dtypes = [np.float32, np.float64]

@pytest.mark.parametrize("dtype", dtypes)
def test_biquad_coefficient_parameter_default_init(dtype):
    bcp = biquadCoefficientParameter(dtype)()
    assert bcp.a1 == 0.0
    assert bcp.a2 == 0.0
    assert bcp.b0 == 0.0
    assert bcp.b1 == 0.0
    assert bcp.b2 == 0.0


@pytest.mark.parametrize("dtype", dtypes)
def test_biquad_coefficient_parameter_list_init(dtype):
    b0, b1, b2, a1, a2 = 1.0, -2.0, 3.0, -4.0, 5.0
    bcp = biquadCoefficientParameter(dtype)([b0, b1, b2, a1, a2])
    assert bcp.a1 == a1
    assert bcp.a2 == a2
    assert bcp.b0 == b0
    assert bcp.b1 == b1
    assert bcp.b2 == b2
    

@pytest.mark.parametrize("dtype", dtypes)
def test_biquad_coefficient_parameter_scalar_init(dtype):
    b0, b1, b2, a1, a2 = 1.0, -2.0, 3.0, -4.0, 5.0
    bcp = biquadCoefficientParameter(dtype)(b0, b1, b2, a1, a2)
    assert bcp.a1 == a1
    assert bcp.a2 == a2
    assert bcp.b0 == b0
    assert bcp.b1 == b1
    assert bcp.b2 == b2


@pytest.mark.parametrize("dtype", dtypes)
def test_biquad_coefficient_parameter_scalar_init_kwargs(dtype):
    b0, b1, b2, a1, a2 = 1.0, -2.0, 3.0, -4.0, 5.0
    bcp = biquadCoefficientParameter(dtype)(a1=a1, a2=a2, b0=b0, b1=b1, b2=b2)
    assert bcp.a1 == a1
    assert bcp.a2 == a2
    assert bcp.b0 == b0
    assert bcp.b1 == b1
    assert bcp.b2 == b2


@pytest.mark.parametrize("dtype", dtypes)
def test_biquad_coefficient_parameter_init_from_rbbl(dtype):
    b0, b1, b2, a1, a2 = 1.0, -2.0, 3.0, -4.0, 5.0
    bc = biquadCoefficient(dtype)([b0, b1, b2, a1, a2])
    bcp = biquadCoefficientParameter(dtype)(bc)
    assert bcp.a1 == a1
    assert bcp.a2 == a2
    assert bcp.b0 == b0
    assert bcp.b1 == b1
    assert bcp.b2 == b2


# Allow running the tests as a script (as opposed to through pytest)
if __name__ == "__main__":
    for dtype in dtypes:
        test_biquad_coefficient_parameter_default_init(dtype)
        test_biquad_coefficient_parameter_list_init(dtype)
        test_biquad_coefficient_parameter_scalar_init(dtype)
        test_biquad_coefficient_parameter_scalar_init_kwargs(dtype)
        test_biquad_coefficient_parameter_init_from_rbbl(dtype)

