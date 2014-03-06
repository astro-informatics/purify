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
#include <numpy/arrayobject.h>

int main() {
   
  npy_intp dims[1] = {1};
  PyArrayObject *dummy = (PyArrayObject*) PyArray_SimpleNew(1, dims, NPY_DOUBLE);
  PyArray_ENABLEFLAGS(dummy, 0);
    
  return 0;
}
