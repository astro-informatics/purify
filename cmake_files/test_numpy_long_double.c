/***********************
    DCProgs computes missed-events likelihood as described in
    Hawkes, Jalali and Colquhoun (1990, 1992)

    Copyright (C) 2013  University College London

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************/

#include <Python.h>
#include <numpy/npy_common.h>

template<class T> class type;
      
template<> struct type<npy_longdouble> 
{
  typedef npy_longdouble np_type;
  static const int value;
};
const int type<npy_longdouble>::value = 0;
template<> struct type<npy_double> 
{
  typedef npy_double np_type;
  static const int value;
};
const int type<npy_double>::value = 0;
int main() {return 0;}
