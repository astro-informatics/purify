
                             PURIFY
  ----------------------------------------------------------------

DESCRIPTION
  PURIFY is a collection of routines written in C that implements different 
  tools for radio-interferometric imaging including file handling 
  (for both visibilities and fits files), implementation of the measurement 
  operator and set-up of the different optimization problems used for image 
  deconvolution. The code calls the generic Sparse OPTimization (SOPT) 
  package to solve the imaging optimization problems.
    

AUTHORS
  R. E. Carrillo (http://people.epfl.ch/rafael.carrillo)
  J. D. McEwen (http://www.jasonmcewen.org)
  Y. Wiaux (http://basp.eps.hw.ac.uk)

EXPERIMENTS
  To run an example with a simulated AMI coverage and a 128X128 M31 test image,
  run the script example_ami.c.

  To run an example with a simulated variable density coverage and a 256X256 
  version of M31 as test image, run the script example_m31.c.

  To run an example with a simulated variable density coverage and a 256X256 
  version of M31 as test image, run the script example_30dor.c.

REFERENCES
  When referencing this code, please cite our related paper:
    [1] R. E. Carrillo, J. D. McEwen and Y. Wiaux.  "Why CLEAN when
    you can PURIFY?", submitted to MNRAS, 2013 (arXiv:arXiv:1307.4370).  
    Preprint available at http://infoscience.epfl.ch/record/187457.

DOCUMENTATION
   See doc/html/index.html

WEBPAGE
   http://basp-group.github.io/purify/

INSTALLATION 
  To install the toolbox, the CFITSIO library 
  (heasarc.gsfc.nasa.gov/fitsio/)  and the SOPT toolbox 
  (github.com/basp-group/sopt) must be installed. 
  A makefile is provided to build the code. Before compiling you 
  might need to edit the makefile to specify your compiler and the 
  location where the dependencies are installed. Once these are 
  installed you just need to type
  
  %make all

  to install the toolbox. 

SUPPORT
  If you have any questions or comments, feel free to contact Rafael
  Carrillo at: rafael {DOT} carrillo {AT} epfl {DOT} ch.

NOTES
  The code is given for educational purpose. The code is
  in beta and still under development.

LICENSE
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
