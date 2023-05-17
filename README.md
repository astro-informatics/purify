PURIFY
=======

[![Build Status](https://travis-ci.com/astro-informatics/purify.svg?branch=development)](https://travis-ci.com/astro-informatics/purify)
[![codecov](https://codecov.io/gh/astro-informatics/purify/branch/development/graph/badge.svg)](https://codecov.io/gh/astro-informatics/purify)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.2555252.svg)](https://doi.org/10.5281/zenodo.2555252)

Description
-------------

**PURIFY** is an open-source collection of routines written in `C++` available under the [license](#license) below. It implements different tools and high-level to perform radio interferometric imaging, _i.e._ to recover images from the Fourier measurements taken by radio interferometric telescopes. 

**PURIFY** leverages recent developments in the field of compressive sensing and convex optimization. Low-level functionality to solve the resulting convex optimisation is factored into the open-source companion code, [**SOPT**](https://github.com/astro-informatics/sopt), also written by the authors of **PURIFY**. For further background please see the [reference](#references-and-citation) section.

This documentation outlines the necessary and optional [dependencies](#dependencies-installation) upon which **PURIFY** should be built, before describing [installation](#installing-and-building-PURIFY), [testing](#testing) and [usage](#usage) details. [Contributors](#contributors), [references](#references-and-citation) and [license](#license) information then follows.

Dependencies installation
-------------------------

**PURIFY** is written in `C++11`. Pre-requisites and dependencies are listed in following and minimal versions required are meaning that they come natively with OSX and the Ubuntu Trusty release. These are also the default ones fetched by `CMake` (an internet connection is required for this).

`C++` minimal dependencies:

- [CMake](http://www.cmake.org/) v3.5.1 (Trusty as `cmake3`) A free software that allows cross-platform compilation
- [GCC](https://gcc.gnu.org) v7.3.0 GNU compiler for `C++`
- [UCL/GreatCMakeCookOff](https://github.com/UCL/GreatCMakeCookOff) Collection of `CMake` recipes.
  Downloaded automatically if absent
- [Boost](https://www.boost.org/) v1.54 (Trusty). A set of free peer-reviewed
  portable C++ libraries. V1.65 downloaded automatically if absent but needs a
  cmake >= 3.9.2.
- [fftw3](www.fftw.org): Fastest Fourier Transform in the West
- [Eigen3](http://eigen.tuxfamily.org/index.php?title=Main_Page) v3.3.7 (Trusty) Modern `C++` linear algebra.
  Downloaded automatically if absent.
- [tiff](http://www.libtiff.org/) v4.0.3 (Trusty) Tag Image File Format library
- [astro-informatics/sopt](https://github.com/astro-informatics/sopt) v* Sparse Optimization
  Compressed Sensing library. Downloaded automatically if absent.
- [cfitsio](http://heasarc.gsfc.nasa.gov/fitsio/fitsio.html) v* Library of `C` and `Fortran` subroutines
  for reading and writing data files in FITS (Flexible Image Transport System) data format.
  Downloaded automatically if absent.
- [casacore](http://casacore.github.io/casacore/) - Optional - Needed to interface with measurement
  sets. The main purify program requires this library (and its dependencies)
- [OpenMP](http://openmp.org/wp/) v4.8.4 (Trusty) - Optional - Speeds up some of the operations.
- [MPI](https://www.open-mpi.org) v3.1.1 (Trusty) - Optional - Parallelisation paradigm to speed up operations.
- [spdlog](https://github.com/gabime/spdlog) v* - Optional - Logging library. Downloaded automatically if
  absent.
- [Catch2](https://github.com/catchorg/Catch2) v2.3.0 - Optional -  A `C++`
  unit-testing framework only needed for testing. Downloaded automatically if absent.
- [google/benchmark](https://github.com/google/benchmark) `v1.3.0` - Optional - A `C++`
  micro-benchmarking framework only needed for benchmarks. Downloaded automatically if absent.

Installing and building PURIFY
-------------------------------------

To build Purify:

1. Install [TensorFlow C API](https://www.tensorflow.org/install/lang_c)
1. Clone the UCL fork of cppflow and create a conan package using

    ``` bash
    git clone git@github.com:UCL/cppflow.git
    conan create ./cppflow/ -pr:h=default -pr:b=default
    ```

1. Clone sopt and create a conan package using
  
    ``` bash
    git clone git@github.com:astro-informatics/sopt.git
    conan create ./sopt/ --build missing -s compiler.libcxx=libstdc++11 -o cppflow=on  -pr:h=default -pr:b=default
    ```

1. Once the mandatory dependencies are present, `git clone` from the [GitHub repository](https://github.com/astro-informatics/purify):

    ``` bash
    git clone https://github.com/astro-informatics/purify.git
    ```

1. Then, the program can be built using Conan:

    ``` bash
    cd /path/to/code
    mkdir build
    conan install .. --build missing -pr:h=default -pr:b=default
        conan build ..
    ```

    You can turn the various options on and off by adding flags to the `conan install` command, e.g.

    ```bash
    conan install .. --build missing -o cppflow=on -o openmp=on -o mpi=off -pr:h=default -pr:b=default
    ```

To test everything went all right:

``` bash
cd /path/to/code/build
ctest .
```

To install in directory `/X`, with libraries going to `X/' do:

```
cd /path/to/code/build
cmake -DCMAKE_INSTALL_PREFIX=/X ..
make install
```

Testing
-------

To check everything went all right, run the test suite:

```
cd /path/to/code/build
ctest .
```

Usage
------

The main purify executable lives either in the build directory or in the in the `bin` subdirectory
of the installation directory. `purify` has one required argument, it a string for the file path of the config file containing the settings.

`purify path/to/config.yaml`.

A [template with a description of the settings](https://github.com/astro-informatics/purify/blob/development/data/config/config.yaml)
is included in the `data/config` directory.`
When `purify` runs a directory will be created, and the output images will be
saved and time-stamped. Additionally, a config file with the settings used will
be saved and time-stamped, helping for reproducibility and book-keeping.

Docker
-------

If you want to use Docker instead, you can build an image using the Dockerfile
available in the repository or pulling it from
[DockerHub](https://hub.docker.com/r/uclrits/purify).

```
docker build -t purify .
```

or

```
docker pull uclrits/purify
```

Then to use it, you should mount the directory with your data and config files
to `/mydata` in the container. To run the container and mount the directory is
with:

```
docker run -it --name purify -v /full/path/to/data:/mydata uclrits/purify
```

That will start a shell inside the container in the `/mydata` directory where
you can see all the files from your `/full/path/to/data`. There you can run
`purify` as shown above.`


Contributors
------------

Check the [contributors](@ref purify_contributors) page ([github](cpp/docs/PURIFY_CONTRIBUTORS.md)).

References and citation
-----------------------

If you use **PURIFY** for work that results in publication, please reference the [webpage](#webpage) and our related academic papers:

1. L. Pratley, et al.
   "Distributed convex optimization for Radio Interferometry with PURIFY".
   Link will be here soon!
2. L. Pratley, M. Johnston-Hollitt, J. D. McEwen,
   "A fast and exact w-stacking and w-projection hybrid algorithm for wide-field interferometric imaging".
   Submitted to ApJ [arXiv:1807.09239](https://arxiv.org/abs/1807.09239)
3. L. Pratley, J. D. McEwen, M. d'Avezac, R. E. Carrillo, A. Onose, Y. Wiaux. "Robust sparse image
   reconstruction of radio interferometric observations with PURIFY".
   Accepted (2016) [arxiv:1610.02400](https://arxiv.org/abs/1610.02400v2)
4. A. Onose, R. E. Carrillo, A. Repetti, J. D. McEwen, J.-P. Thiran, J.-C. Pesquet, and Y. Wiaux.
   "Scalable splitting algorithms for big-data interferometric imaging in the SKA era" _Mon. Not.
   Roy. Astron. Soc._ **462(4):4314-4335** (2016) [arXiv:1601.04026](http://arxiv.org/abs/arXiv:1601.04026)
5. R. E. Carrillo, J. D. McEwen and Y. Wiaux.  "PURIFY: a new approach to radio-interferometric
   imaging". _Mon. Not. Roy. Astron. Soc._ **439(4):3591-3604** (2014) [arXiv:1307.4370](http://arxiv.org/abs/1307.4370)

CMake Tips
----------

It is possible to tell ``CMake`` exactly which libraries to compile and link against. The general
idea is to add ``-DVARIABLE=something`` to the command-line arguments of CMake. CMake can be called
any number of times: previous settings will not be overwritten unless specifically
requested. Some of the more common options are the following:

-  ``CMAKE_PREFIX_PATH``: CMake will look in "CMAKE_PREFIX_PATH/lib"
   for libraries, "CMAKE_PREFIX_PATH/include" for headers, etc.
-  ``FFTW3_LIBRARIES``, ``FFTW3_INCLUDE_DIR``
-  ``BLAS_INCLUDE_DIRS``, ``BLAS_LIBRARIES``

All these variables and more can be found and modified in the ``CMakeCache.txt`` file in the build
directory. There are extra CMake options sepcific to purify. ``-Ddompi=ON`` will turn MPI on in the build, ``-Dopenmp=ON`` will turn openmp on for the build. ``-Dtests=ON`` will make sure tests are built.

``ctest`` should be run to make sure the unit tests pass.

License
--------

>    PURIFY Copyright (C) 2013-2019
>
>    This program is free software; you can redistribute it and/or modify it
>    under the terms of the GNU General Public License as published by the
>    Free Software Foundation; either version 2 of the License, or (at your
>    option) any later version.
>
>    This program is distributed in the hope that it will be useful, but
>    WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
>    Public License for more details (LICENSE.txt).
>
>    You should have received a copy of the GNU General Public License along
>    with this program; if not, write to the Free Software Foundation, Inc.,
>    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

Webpage
-------

- [Documentation](http://astro-informatics.github.io/purify)
- [Repository](https://github.com/astro-informatics/purify)

Support
-------

For any questions or comments, feel free to contact [Jason McEwen](http://www.jasonmcewen.org), or add
an issue to the [issue tracker](https://github.com/astro-informatics/purify/issues).

Notes
-----

The code is given for educational purpose. The code is in beta and still under development.

