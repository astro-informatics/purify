# yaml-cpp for purify

Dependencies
============


* Boost for thr argument parser.
* A Yaml parser for C++: https://github.com/jbeder/yaml-cpp
* Catch for testing


Compilation
===========

Link to the Yaml library:

g++ yaml-parser.cpp -lyaml-cpp -lboost_program_options -o yaml-parser.out
