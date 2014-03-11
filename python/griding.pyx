import cython
cdef extern from "purify_sparsemat.h":
    ctypedef struct _SparseMatRow "purify_sparsemat_row":
        int nrows
        int ncols
        int nvals
        int real
        double *vals
        double complex *cvals
        int *colind
        int *rowptr

    cdef void purify_sparsemat_freer(_SparseMatRow *mat)

    #  For debugging purposes. 
    #  This function is used to check that the C versions wraps the python version correctly.
    cdef void purify_sparsemat_fwd_realr(double *y, double *x, _SparseMatRow *A)

    #  For debugging purposes. 
    #  This function is used to check that the C versions wraps the python version correctly.
    cdef void purify_sparsemat_fwd_complexr(double complex *y, double complex *x, _SparseMatRow *A)

cdef object _convert_sparsemat(_SparseMatRow *sparse):
    """ Converts C sparse matrix to scipy sparse matrix 
    
        This function copies the data.
    """
    from scipy.sparse import csr_matrix
    from numpy import asarray

    shape = (sparse.nrows, sparse.ncols)
    dtype = "complex" if sparse.real == 0 else "double"
    indices = asarray(<int[:sparse.nvals:1]> sparse.colind)
    indptr = asarray(<int[:sparse.nrows+1:1]> sparse.rowptr)
    data = asarray(<double complex[:sparse.nvals:1]> sparse.cvals) if sparse.real == 0 \
           else asarray(<double[:sparse.nvals:1]> sparse.vals)

    return csr_matrix((data, indices, indptr), shape=(sparse.nrows, sparse.ncols))

cdef void _wrap_sparsemat(object py_sparse, _SparseMatRow* c_sparse) except *:
    """ Wrapse C sparse matrix around a scipy sparse matrix 
    
        This function does *not* copies the data. The C sparse matrix should *not* be deallocated.
        Nor should it be used beyond the lifetime of the python object. Nor should the python object
        be changed during the lifetime of the C matrix.
    """
    if py_sparse.format != 'csr':
        raise TypeError('Expected CSR format matrix')
    if py_sparse.dtype != "double" and py_sparse.dtype != "complex":
        raise TypeError("Expected a complex or real matrix.")

    c_sparse.nrows = py_sparse.shape[0]
    c_sparse.ncols = py_sparse.shape[1]
    c_sparse.nvals = py_sparse.nnz
    c_sparse.real = 0 if py_sparse.dtype == "complex" else 1

    cdef unsigned char[::1] data = py_sparse.data.data
    if c_sparse.real:
        c_sparse.vals = <double*>&data[0]
        c_sparse.cvals = NULL
    else:
        c_sparse.vals = NULL
        c_sparse.cvals = <double complex *>&data[0]

    cdef int[::1] colind = py_sparse.indices
    cdef int[::1] rowptr = py_sparse.indptr
    c_sparse.colind = &colind[0]
    c_sparse.rowptr = &rowptr[0]

def _debug_multiply(matrix, vector):
    """ Multiplies a sparse matrix by a vector using C implementation. 

        This function is meant only to check that we know how to convert/wrap sparse matrices from C
        to python and back.

        Since this is for testing only, the input is not checked for sanity.

        :Parameters:
            matrix: 
                A scipy CSC sparse-matrix
            vector: 
                A numpy vector
    """
    from numpy import zeros
    cdef _SparseMatRow c_sparse
    _wrap_sparsemat(matrix, &c_sparse)
    result = zeros(matrix.shape[0], dtype=matrix.dtype)
    cdef unsigned char[::1] y = result.data
    cdef unsigned char[::1] x = vector.data

    if matrix.dtype == "double":
        purify_sparsemat_fwd_realr(<double*>&y[0], <double*>&x[0], &c_sparse)
    elif matrix.dtype == "complex":
        purify_sparsemat_fwd_complexr(<double complex*>&y[0], <double complex*>&x[0], &c_sparse)

    return result

def _cycle_python_to_c_to_python(matrix):
    """ Wraps input to C, and converts it back to python """
    cdef _SparseMatRow c_sparse
    _wrap_sparsemat(matrix, &c_sparse)
    return _convert_sparsemat(&c_sparse)
