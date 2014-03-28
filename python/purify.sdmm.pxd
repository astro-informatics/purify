from purify.measurements cimport MeasurementOperator
cdef extern from "sopt_l1.h":
    ctypedef struct sopt_l1_sdmmparam:
        int verbose
        int max_iter
        double gamma
        double rel_obj
        double epsilon
        double epsilon_tol
        int real_data
        int cg_max_iter
        double cg_tol

cdef double _default_sigma(measurements) except *
cdef void _convert_sdmm_params(
        sopt_l1_sdmmparam *c_params,
        sdmm, MeasurementOperator measurements, visibility ) except *
