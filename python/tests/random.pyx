""" Access to sopt's random distribution functions """

cdef extern from "sopt_ran.h":
    double sopt_ran_gasdev2(int idum)
    double sopt_ran_ran2(int idum)

def gaussian_distribution(unsigned int seed = 54):
    """ Returns random number from gaussian distribution 
    
        There is no reason to use this function other than testing results against the C code. 
        If true random numbers are needed, it is better to use any one of the libraries provided for
        this in python or numpy.

        Parameter:
            seed: int
                The default seed is the one used in the C examples.
    """
    return sopt_ran_gasdev2(<int> seed)

def reset(unsigned int seed = 54):
    """ Resets random number generator """
    cdef int negseed = - <int> seed
    sopt_ran_ran2(negseed)
