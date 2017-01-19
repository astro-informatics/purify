# PURIFY 

## Description


This code has been forked from the purify respository (https://github.com/astro-informatics/purify) to
provide a more optimised and parallelised version that can be used for primal dual simulations.

##Usage
`purify` has two required arguments, `--measurement_set` and `--name`, the path of the measurement set and the prefix name of the output files.

`purify --measurement_set path/to/measurements.ms --name output/path/name`.

It is possible to add other options, such as limited the number of iterations to 100, and save output diagnostic images with each iteration. These

`purify --measurement_set path/to/measurements.ms --name output/path/name --niters 100 --diagnostic`


### Arguments
#### Important Arguments
* `--help` will print basic help information, showing what arguments are possible (more than this list).
* `--measurement_set` is the path to the CASA measurement set folder. `(required argument)`
* `--name` is the prefix name used to save the output model, residual, and dirty map. `(required argument)`
* `--diagnostic` will record variables and output images with each iteration. This is useful for testing or trial runs, but will take up some more computation in calculating and saving diagnostic updates. The update images and diagnostic file will be used as a checkpoint, in the case that purify locates the images from a previous run.
* `--l2_bound` this value can be used to scale the error on the model matching the measurements. `Default value is 1.4`.
* `--power_iterations` number of iterations needed to normalize the measurement operator. This is needed to ensure that the measurement operator reconstruct a model to the correct flux scale. `Default value is 100`.
* `--noadapt` will turn off the adapting step size.
* `--size` the height and width of the output image in pixels (only powers of 2 supported at present).
* `--cellsize` is the width and height of a pixel in arcseconds. If not specified, this is determined from the Nyquist limit.
* `--niters` max number of iterations. Default is max unsigned integer.
* `--relative_variation` the relative difference of the model between iterations needed for convergence.
* `--residual_convergence` the upper bound on the residual norm needed for convergence.


## Running the main purify executable

The main purify executable lives either in the build directory or in the in the `bin` subdirectory
of the installation directory.

## Installation

### C++ pre-requisites and dependencies

- [CMake](http://www.cmake.org/): a free software that allows cross-platform compilation
- [fftw3](www.fftw.org): Fastest Fourier Transform in the West
- [tiff](http://www.libtiff.org/): Tag Image File Format library
- [OpenMP](http://openmp.org/wp/): Optional. Speeds up some of the operations.
- [casacore](http://casacore.github.io/casacore/): Optional. Needed to interface with measurement
  sets. The main purify program requires this library (and its dependencies)
- [UCL/GreatCMakeCookOff](https://github.com/UCL/GreatCMakeCookOff): Collection of cmake recipes.
  Downloaded automatically if absent.
- [basp-group/sopt](https://github.com/basp-group/sopt): Sparse Optimization
  Compressed Sensing library. Downloaded automatically if absent.
- [Eigen 3](http://eigen.tuxfamily.org/index.php?title=Main_Page): Modern C++ linear algebra.
  Downloaded automatically if absent.
- [spdlog](https://github.com/gabime/spdlog): Optional. Logging library. Downloaded automatically if
   absent.
- [cfitsio](http://heasarc.gsfc.nasa.gov/fitsio/fitsio.html): library of C and Fortran subroutines
  for reading and writing data files in FITS (Flexible Image Transport System) data format.
  Downloaded automatically if absent. Needed by CCFits.
- [CCFits](http://heasarc.gsfc.nasa.gov/fitsio/ccfits/): c++ wrappers for cfitsio. Downloaded
  automatically if absent.
- [philsquared/Catch](https://github.com/philsquared/Catch): Optional - only for testing. A C++
  unit-testing framework. Downloaded automatically if absent.
- [google/benchmark](https://github.com/google/benchmar): Optional - only for benchmarks. A C++
  micro-benchmarking framework. Downloaded automatically if absent.

### Installing Purify

Once the dependencies are present, the program can be built with:

```
cd /path/to/code
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

To test everything went all right:

```
cd /path/to/code/build
ctest .
```

To install in directory `/X`, with libraries going to `X/lib`, python modules to
`X/lib/pythonA.B/site-packages/sopt`, etc, do:

```
cd /path/to/code/build
cmake -DCMAKE_INSTALL_PREFIX=/X ..
make install
```

### CMake Tips

It is possible to tell ``CMake`` exactly which libraries to compile and link against. The general
idea is to add ``-DVARIABLE=something`` to the command-line arguments of CMake. CMake can be called
any number of times: previous settings will not be overwritten unless specifically
requested. Some of the more common options are the following:

-  ``CMAKE_PREFIX_PATH``: CMake will look in "CMAKE\_PREFIX\_PATH/lib"
   for libraries, "CMAKE\_PREFIX\_PATH/include" for headers, etc.
-  ``PYTHON_EXECUTABLE``, ``PYTHON_LIBRARIES``, ``PYTHON_INCLUDE_DIRS``
-  ``FFTW3_LIBRARIES``, ``FFTW3_INCLUDE_DIR``
-  ``BLAS_INCLUDE_DIRS``, ``BLAS_LIBRARIES``

All these variables and more can be found and modified in the ``CMakeCache.txt`` file in the build
directory.

