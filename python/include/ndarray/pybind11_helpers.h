/****************************/
/* NDArray pybdin11 helpers */
/****************************/

#ifndef __NDARRAY_PYBIND11_HELPERS_H__
#define __NDARRAY_PYBIND11_HELPERS_H__

#include <pybind11/numpy.h>

#include "ndarray.h"

pybind11::array py_array_from_dtype(std::vector<size_t> shape, ndarray::DType dtype, void* data = nullptr);


#endif // __NDARRAY_PYBIND11_HELPERS_H__
