from cython.view import contiguous
# Needed because it is not included in sopt_wavevelet.h
cdef extern from "complex.h":
    pass

cdef extern from "sopt_wavelet.h":
    ctypedef enum sopt_wavelet_type:
        SOPT_WAVELET_Dirac
        SOPT_WAVELET_DB1
        SOPT_WAVELET_DB2
        SOPT_WAVELET_DB3
        SOPT_WAVELET_DB4
        SOPT_WAVELET_DB5
        SOPT_WAVELET_DB6
        SOPT_WAVELET_DB7
        SOPT_WAVELET_DB8
        SOPT_WAVELET_DB9
        SOPT_WAVELET_DB10

cdef extern from "sopt_sara.h":
    ctypedef struct sopt_sara_param:
        int ndict
        int real
        sopt_wavelet_param *wav_params

    ctypedef struct sopt_wavelet_param:
        int nx1
        int nx2
        sopt_wavelet_type type
        double *h
        int h_size
        int nb_levels

    void sopt_sara_analysisop(void *, void *, void **)
    void sopt_sara_synthesisop(void *, void *, void **)

cdef class SparsityOperator:
    cdef sopt_sara_param _wavelets
    cdef set_wavelet_pointer(self, void **data)
    cpdef analyze(self, image)
    cpdef synthesize(self, inout)
