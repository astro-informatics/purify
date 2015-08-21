from purify.sensing cimport SensingOperator


cdef extern from "sopt_prox.h":
    # Parameters for TV proximity operator
    ctypedef struct sopt_prox_tvparam:
        int verbose
        int max_iter
        double rel_obj
    ctypedef struct sopt_prox_l1param:
        int verbose
        int max_iter
        double rel_obj
        double nu
        int tight
        int pos


cdef extern from "sopt_l1.h":
    # Parameters for solving the l1 optimisation problem using SDMM.
    # Must have the same order as the C structure.
    ctypedef struct sopt_l1_sdmmparam:
        int verbose
        int max_iter
        double rel_obj
        double gamma
        double epsilon
        double epsilon_tol
        int real_data
        int cg_max_iter
        double cg_tol

    ctypedef struct sopt_l1_param_padmm:
        int verbose
        int max_iter
        double rel_obj
        double gamma
        double epsilon
        int real_out
        int real_meas
        sopt_prox_l1param paraml1
        double epsilon_tol_scale
        double lagrange_update_scale
        double nu

    # Extra parameters for solving the reweighted l1 problem using SDMM.
    # Must have the same order as the C structure.
    ctypedef struct sopt_l1_rwparam:
        int verbose
        int max_iter
        double rel_var
        double sigma
        int init_sol


cdef extern from "sopt_tv.h":
    # Parameters for solving the tv optimisation problem using SDMM.
    # Must have the same order as the C structure.
    ctypedef struct sopt_tv_sdmmparam:
        int verbose
        int max_iter
        double rel_obj
        double gamma
        double epsilon
        double epsilon_tol
        int real_data
        int cg_max_iter
        double cg_tol
        sopt_prox_tvparam paramtv

    # Extra parameters for solving the reweighted tv problem using SDMM.
    # Must have the same order as the C structure.
    ctypedef struct sopt_tv_rwparam:
        int verbose
        int max_iter
        double rel_var
        double sigma
        int init_sol



# Populates C parameter structures from python
cdef void _convert_l1_sdmm_param( sopt_l1_sdmmparam* c_params, sdmm,
                                  SensingOperator sensing_op,
                                  visibility ) except *
cdef void _convert_rwparams( sopt_l1_rwparam *c_params, sdmm,
        SensingOperator sensing_op, visibility ) except *
cdef void _convert_tvparam( sopt_tv_sdmmparam* c_params, sdmm,
                            SensingOperator sensing_op,
                            visibility ) except *
cdef void _convert_l1_padmm_param(
        sopt_l1_param_padmm* c_params, padmm,
        SensingOperator sensing_op, visibility) except *

# Computes default sigma value from visibilities
cdef double _default_sigma(visibilities) except *
