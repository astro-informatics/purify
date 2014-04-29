from purify.numpy_interface cimport untyped_pointer_to_data, import_array
import_array()
from purify.sparse cimport _convert_sparsemat, _wrap_sparsemat, _SparseMatRow

#  For debugging purposes. 
#  This function is used to check that the C versions wraps the python version
#  correctly.
cdef extern from "purify_sparsemat.h":
    cdef void purify_sparsemat_fwd_realr(double *y, double *x, _SparseMatRow *A)
    cdef void purify_sparsemat_fwd_complexr(double complex *y,
        double complex *x, _SparseMatRow *A)

def _debug_multiply(matrix, vector):
    """ Multiplies a sparse matrix by a vector using C implementation. 

        This function is meant only to check that we know how to convert/wrap
        sparse matrices from C to python and back.

        Since this is for testing only, the input is not checked for sanity.

        :Parameters:
            matrix: 
                A scipy CSC sparse-matrix
            vector: 
                A numpy vector
    """
    from numpy import zeros
    if matrix.dtype != vector.dtype:
        raise TypeError("Matrix and vector have different types")
    if matrix.dtype != "complex" and matrix.dtype != "double":
        raise TypeError("Matrix should be of type complex or double")
    cdef _SparseMatRow c_sparse
    result = zeros(matrix.shape[0], dtype=matrix.dtype, order="C")
    _wrap_sparsemat(matrix, &c_sparse)
    cdef void* y = untyped_pointer_to_data(result)
    cdef void* x = untyped_pointer_to_data(vector)

    if matrix.dtype == "double":
        purify_sparsemat_fwd_realr(<double*>y, <double*>x, &c_sparse)
    elif matrix.dtype == "complex":
        purify_sparsemat_fwd_complexr(<double complex*>y,
            <double complex*>x, &c_sparse)

    return result

def _cycle_python_to_c_to_python(matrix):
    """ Wraps input to C, and converts it back to python """
    cdef _SparseMatRow c_sparse
    _wrap_sparsemat(matrix, &c_sparse)
    return _convert_sparsemat(&c_sparse)
