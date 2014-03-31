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

    ctypedef struct sopt_l1_rwparam:
        int verbose
        int max_iter
        double rel_var
        double sigma
        int init_sol


cdef extern from "sopt_prox.h":
    ctypedef struct sopt_prox_tvparam:
        int verbose
        int max_iter
        double rel_obj

cdef extern from "sopt_tv.h":
    ctypedef struct sopt_tv_sdmmparam:
        int verbose
        int max_iter
        double gamma
        double rel_obj
        double epsilon
        double epsilon_tol
        int real_data
        int cg_max_iter
        double cg_tol
        sopt_prox_tvparam paramtv

cdef void _convert_sdmm_params( sopt_l1_sdmmparam* c_params, sdmm,
                                MeasurementOperator measurements,
                                visibility ) except *
cdef void _convert_rwparams( sopt_l1_rwparam *c_params, sdmm,
        MeasurementOperator measurements, visibility ) except *

cdef double _default_sigma(measurements) except *
