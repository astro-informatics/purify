====== 
PURIFY 
======

Description
===========

PURIFY is a collection of routines written in C that implements
different tools for radio interferometric imaging including file
handling (for both visibilities and fits files), implementation of the
measurement operator and set-up of the different optimization problems
used for image deconvolution. The code calls the generic Sparse
OPTimization (SOPT) package to solve the imaging optimization problems.

Creators
========

-  `R. E. Carrillo <http://people.epfl.ch/rafael.carrillo>`__
-  `J. D. McEwen <http://www.jasonmcewen.org>`__
-  `Y. Wiaux <http://basp.eps.hw.ac.uk>`__

Contributors
============

- Mayeul d'Avezac

   
Experiments -- C code
=====================

To run an example with a simulated AMI coverage and a 128X128 M31 test
image, run the script example\_ami.c.

To run an example with a simulated variable density coverage and a
256X256 version of M31 as test image, run the script example\_m31.c.

To run an example with a simulated variable density coverage and a
256X256 version of M31 as test image, run the script example\_30dor.c.

Reference
=========

When referencing this code, please cite our related paper:
[1] R. E. Carrillo, J. D. McEwen and Y. Wiaux.  "PURIFY: a new
approach to radio-interferometric
imaging". Mon. Not. Roy. Astron. Soc., 439(4):3591-3604, 2014,
`arXiv:1307.4370 <http://arxiv.org/abs/1307.4370>`__

Documentation
=============

See doc/html/index.html

Webpage
=======

http://basp-group.github.io/purify/

Installation
============

Pre-requisites:

-  `CMake <http://www.cmake.org/>`__: a free software that allows
   cross-platform compilation
-  `fftw3 <www.fftw.org>`__: Fastest Fourier Transform in the West
-  `blas <http://www.openblas.net/>`__: Basic Linear Algebras library
-  `tiff <http://www.libtiff.org/>`__: Tag Image File Format library

All of these can be
`apt-getted <https://help.ubuntu.com/12.04/serverguide/apt-get.html>`__,
`yummed <https://fedoraproject.org/wiki/Yum>`__,
`ebuilt <http://en.wikipedia.org/wiki/Ebuild>`__,
`brewed <http://brew.sh/>`__, `finked <http://www.finkproject.org/>`__,
or easily installed in some way or another.

Optional C libraries:

-  `sopt <https://github.com/astro-informatics/sopt>`__: C implementation of
   the Sparse Optimization SARA algorithm that forms the back-bone of
   purify
-  `cfitsio <http://heasarc.gsfc.nasa.gov/fitsio/fitsio.html>`__:
   library of C and Fortran subroutines for reading and writing data
   files in FITS (Flexible Image Transport System) data format. It can
   often be obtained in the same way as libraries above

Both of these libraries will downloaded and installed if they are not
found on the system.

Python Packages:

-  `numpy <http://www.numpy.org/>`__: numerical library for python
-  `scipy <http://www.scipy.org/>`__: scientific library for python
-  `pandas <http://pandas.pydata.org/>`__: data analysis library for
   python
-  `cython <http://www.cython.org/>`__: C extensions for python made
   easy
-  `py.test <http://pytest.org>`__: unit-test
   framework for python. Only needed for running unit-tests.
-  `virtualenv <https://nose.readthedocs.org/en/latest/>`__: creates an
   isolated python environment. Only needed for running tests.

These python packages will be installed automatically when going through
the ``pip`` or ``casapy`` routes below. They can be easily installed via
`easy\_install <http://pythonhosted.org/setuptools/easy_install.html>`__
or `pip <http://www.pip-installer.org/en/latest/quickstart.html>`__
otherwise.

C users
-------

To install the toolbox, please download from github and run the
following commands from the source directory (the one with the
``CMakelist.txt``, 'src' and 'python' directories...). The following
assumes a bash shell.

.. code:: bash

    cd /path/to/purify
    [ ! -d build ] && mkdir build
    cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/root/
    make test
    make install

The libraries will end up in ``/path/to/root/lib``, the executables in
``/path/to/root/bin``, and so forth.

It is possible to tell ``CMake`` exactly which libraries to compile and
link against. The general idea is to add ``-DVARIABLE=something`` to the
command-line arguments of CMake. CMake can be called any number of
times: previous settings will not be overwritten unless specifically
requested. Some of the more common options are the following:

-  ``CMAKE_PREFIX_PATH``: CMake will look in "CMAKE\_PREFIX\_PATH/lib"
   for libraries, "CMAKE\_PREFIX\_PATH/include" for headers, etc.
-  ``PYTHON_EXECUTABLE``, ``PYTHON_LIBRARIES``, ``PYTHON_INCLUDE_DIRS``
-  ``FFTW3_LIBRARIES``, ``FFTW3_INCLUDE_DIR``
-  ``BLAS_INCLUDE_DIRS``, ``BLAS_LIBRARIES``

All these variables and more can be found and modified in the
``CMakeCache.txt`` file in the build directory.

Python users
------------

.. code:: bash

    pip install git+https://github.com/astro-informatics/purify.git

It is highly recommended to do this within a `virtual
environment <http://www.virtualenv.org/en/latest/>`__. With most python
installations, one can proceed as follows:

.. code:: bash

    # Create virtual environment in directory purify
    python -m virtualenv purify --system-site-packages
    # Install purify in this environment
    ./purify/bin/pip install git+https://github.com/astro-informatics/purify.git
    # Run tests within the environment
    ./purify/bin/python -c "import purify; purify.test()"

CASA users
----------

Run the following commands from the ``casapy`` ipython interface (by
copy/pasting them in one go and hitting return once or twice):

.. code:: Python

    # Import easy_install so it can be run from ipython
    from setuptools.command.easy_install import main as easy_install
    # Install pip, cos it is that much better
    easy_install(['pip'])

Then *exit* and *restart* ``casapy``. Unfortunately, it does not seem to
recognize that it has just installed a module. Then install ``purify`` proper:

.. code:: Python

    # The following lines are needed on Linux only.
    # They modify the compile flags to include CASA's numpy headers
    from os import environ
    environ['CFLAGS'] = "%s -I%s/include" % (
        environ.get('CFLAGS', ''),
        environ['CASAPATH'].split()[0]
    )
    # Import pip so it can be run from ipython
    from pip import main as pip
    # Now install purify and dependencies directly from github: that's why we
    # got pip in the first place
    pip(['install', 'git+https://github.com/astro-informatics/purify.git'])

This last snippet can be used to install other packages as well, by replacing
the second element in the list with the name or location of a package. It is
possible to give different compile and link flags by modifying the first two
lines appropriately. The version above makes sure CASA's numpy headers are
accessible.

Once again, please *exit* and *restart* casa to make sure it knows about the
newly installed package. An example CASA script can be found in the ``scripts``
directory. Please note that the purify task will appear only *after* importing
purify.

NOTE: CASA has the unfortunate behavior of replacing environment
variables with its own. Amongst other difficulties, it means that
``cmake`` may have disappeared from the PATH. It is possible to extend
the path from within ``casapy``. It should be done prior to calling
``pip`` above:

.. code:: Python

        from os import environ
        environ['PATH'] += ":/usr/local/bin"


Testing the install
-------------------

The python wrappers for purify come with a set of tests. They can be run via
the following code:

.. code:: Python

    import purify
    purify.test()

SUPPORT
=======

If you have any questions or comments, feel free to contact Rafael
Carrillo or Jason McEwen.

NOTES
=====

The code is given for educational purpose. The code is in beta and still
under development.

LICENSE
=======

PURIFY Copyright (C) 2013 Rafael Carrillo, Jason McEwen, Yves Wiaux

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details (LICENSE.txt).

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
