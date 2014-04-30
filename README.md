======
PURIFY
======

Description
===========

PURIFY is a collection of routines written in C that implements different
tools for radio-interferometric imaging including file handling
(for both visibilities and fits files), implementation of the measurement
operator and set-up of the different optimization problems used for image
deconvolution. The code calls the generic Sparse OPTimization (SOPT)
package to solve the imaging optimization problems.


Authors
=======
- R. E. Carrillo (http://people.epfl.ch/rafael.carrillo)
- J. D. McEwen (http://www.jasonmcewen.org)
- Y. Wiaux (http://basp.eps.hw.ac.uk)

Experiments -- C code
=====================

To run an example with a simulated AMI coverage and a 128X128 M31 test image,
run the script example_ami.c.

To run an example with a simulated variable density coverage and a 256X256
version of M31 as test image, run the script example_m31.c.

To run an example with a simulated variable density coverage and a 256X256
version of M31 as test image, run the script example_30dor.c.


Reference
=========

When referencing this code, please cite our related paper:

[1] R. E. Carrillo, J. D. McEwen and Y. Wiaux.  "Why CLEAN when
you can PURIFY?", submitted to MNRAS, 2013 (arXiv:arXiv:1307.4370).
Preprint available at http://infoscience.epfl.ch/record/187457.

Documentation
=============

See doc/html/index.html

Webpage
=======

http://basp-group.github.io/purify/

Installation
============

There are several methods for installing purify, as described below. However, all rely on the
ability to run [CMake](http://www.cmake.org/). This is a free software that allows cross-platform
compilation. Please install it first.

C users
-------

To install the toolbox, please download from github and run the following commands from the source
directory (the one with the `CMakelist.txt`, 'src' and 'python' directories...). The following
assumes a bash shell.

```bash
cd /path/to/purify
[ ! -d build ] && mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/root/
make install
```

This will put the libraries in `/path/to/root/lib`, executables `/path/to/root/bin`, and so forth.

Python users
------------

```bash
pip install git+https://github.com/UCL/purify.git@features/setup.py
```

CASA users
----------

Run the following commands from the `casapy` ipython interface:

```Python
# Import easy_install so it can be run from ipython
from setuptools.command.easy_install import main as easy_install
# Install pip, cos it is that much better
easy_install(['pip'])
```

Then exit `casapy`. Unfortunately, it does not seem to recognize that it has just installed a
module. Then install `purify` proper:

```Python
# Import pip so it can be run from ipython
from pip import main as pip
# Now install purify directly from github: that's why we got pip in the first place
pip(['install', 'git+https://github.com/UCL/purify.git@features/setup.py'])
```

This last snippet can be used to install other packages as well.

NOTE:
    CASA has the unfortunate behavior of replacing environment variables with its own. Amongst other difficulties, it means that `cmake` may have disappeared from the PATH. It is possible to extend the path from within `casapy`. It should be done prior to calling `pip` above:
    
```Python
    from os import environ
    environ['PATH'] += ":/usr/local/bin"
```

SUPPORT
=======

If you have any questions or comments, feel free to contact Rafael
Carrillo at: rafael {DOT} carrillo {AT} epfl {DOT} ch.

NOTES
=====

The code is given for educational purpose. The code is
in beta and still under development.

LICENSE
=======

PURIFY
Copyright (C) 2013 Rafael Carrillo, Jason McEwen, Yves Wiaux

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details (LICENSE.txt).

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301, USA.
