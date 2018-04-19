# yaml-cpp for purify

Dependencies
============


* Boost for thr argument parser.
* A Yaml parser for C++: https://github.com/jbeder/yaml-cpp
* Catch for testing


Compilation and running
=======================

* Link to the Yaml and Boost libraries for the parser:

`g++ yaml-parser.cpp -lyaml-cpp -lboost_program_options -o yaml-parser.out`

* Nicely formatted reporting for the tests:

`g++ test.cpp -o test.out`

`./test.out --reporter compact --success`
