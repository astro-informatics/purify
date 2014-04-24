from purify.numpy_interface cimport untyped_pointer_to_data, import_array
import_array()

cdef object _convert_sparsemat(_SparseMatRow *sparse):
    """ Converts C sparse matrix to scipy sparse matrix

        This function does *not* copy the data. It should not outlive the input
        C matrix.
    """
    from scipy.sparse import csr_matrix
    from numpy import asarray

    shape = (sparse.nrows, sparse.ncols)
    dtype = "complex" if sparse.real == 0 else "double"
    indices = asarray(<int[:sparse.nvals:1]> sparse.colind)
    indptr = asarray(<int[:sparse.nrows+1:1]> sparse.rowptr)
    data = asarray(<double[:sparse.nvals:1]> sparse.vals) if sparse.real \
           else asarray(<double complex[:sparse.nvals:1]> sparse.cvals)

    return csr_matrix((data, indices, indptr), shape=(sparse.nrows, sparse.ncols))

cdef void _wrap_sparsemat(object py_sparse, _SparseMatRow* c_sparse) except *:
    """ Wrapse C sparse matrix around a scipy sparse matrix

        This function does *not* copies the data. The C sparse matrix should
        *not* be deallocated.  Nor should it be used beyond the lifetime of the
        python object. Nor should the python object be changed during the
        lifetime of the C matrix.
    """
    from sys import path
    if py_sparse.format != 'csr':
        raise TypeError('Expected CSR format matrix')
    if py_sparse.dtype != "double" and py_sparse.dtype != "complex":
        raise TypeError("Expected a complex or real matrix.")

    c_sparse.nrows = py_sparse.shape[0]
    c_sparse.ncols = py_sparse.shape[1]
    c_sparse.nvals = py_sparse.nnz
    c_sparse.real = 1 if py_sparse.dtype == "double" else 0

    value = py_sparse.data
    if not (value.flags['C_CONTIGUOUS'] or value.flags['F_CONTIGUOUS']):
      msg = "Expected a C or Fortan contiguous numpy array on input"
      raise TypeError(msg)
    cdef void *np_data = untyped_pointer_to_data(value)
    if c_sparse.real:
        c_sparse.vals = <double *>np_data
        c_sparse.cvals = NULL
    else:
        c_sparse.vals = NULL
        c_sparse.cvals = <double complex *>np_data

    cdef int[::1] colind = py_sparse.indices
    cdef int[::1] rowptr = py_sparse.indptr
    c_sparse.colind = &colind[0]
    c_sparse.rowptr = &rowptr[0]
