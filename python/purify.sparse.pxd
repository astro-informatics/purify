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

cdef object _convert_sparsemat(_SparseMatRow *sparse)
cdef void _wrap_sparsemat(object py_sparse, _SparseMatRow* c_sparse) except *
