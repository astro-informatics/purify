from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.files import symlinks
import os

class PurifyConan(ConanFile):
    name = "purify"
    version = "3.0.1"
    url = "https://github.com/astro-informatics/purify"
    license = "GPL-2.0"
    descpriton = "PURIFY is an open-source collection of routines written in C++ available under the license below. It implements different tools and high-level to perform radio interferometric imaging, i.e. to recover images from the Fourier measurements taken by radio interferometric telescopes."


    settings = "os", "compiler", "build_type", "arch"
    requires = ["fftw/3.3.9", "eigen/3.4.0","catch2/3.4.0","benchmark/1.8.2","yaml-cpp/0.7.0", "boost/1.82.0", "cfitsio/4.2.0", "sopt/4.0.0"]
    #generators = "CMakeDeps"
    exports_sources = "cpp/*", "cmake_files/*", "CMakeLists.txt"
    options = {"docs":['on','off'],
               "examples":['on','off'],
               "tests":['on','off'],
               "benchmarks":['on','off'],
               "logging":['on','off'],
               "openmp":['on','off'],
               "mpi":['on','off'],
               "coverage":['on','off'],
               "af": ['on', 'off'],
               "cimg": ['on','off'],
               "casa": ['on','off'],
               "cppflow": ['on', 'off']}
    default_options = {"docs": 'off',
                       "examples":'off',
                       "tests": 'on',
                       "benchmarks": 'off',
                       "logging": 'on',
                       "openmp": 'on',
                       "mpi": 'on',
                       "coverage": 'off',
                       "af": 'off',
                       "cimg": 'off',
                       "casa": 'off',
                       "cppflow": 'off'}

    def configure(self):

        if self.options.cppflow == 'on':
            self.options["sopt"].cppflow = 'on'
        if self.options.logging == 'off':
            self.options["sopt"].logging = 'off'
        if self.options.mpi == 'off':
            self.options["sopt"].mpi = 'off'
        if self.options.openmp == 'off':
            self.options["sopt"].openmp = 'off'
        # When building the sopt package, switch off sopt tests and examples,
        # they are not going to be run.
        self.options["sopt"].examples = 'off'
        self.options["sopt"].tests = 'off'

        # Exclude boost features we don't need. without_fiber is required when
        # building from source on MacOS with gcc.
        # The rest are to speed up building from source.
        self.options["boost"].without_fiber = True
        self.options["boost"].without_python = True

    def requirements(self):
        # Override version of zlib to prevent a conflict between versions of zlib required by depedencies
        self.requires("zlib/1.2.12", override=True)

        if self.options.examples == 'on':
            self.requires("libtiff/4.5.1")

        if self.options.logging == 'on':
            self.requires("spdlog/1.12.0")

        if self.options.docs == 'on':
            self.requires("doxygen/1.9.2")

        if self.options.cimg == 'on':
            self.requires("cimg/3.0.2")

        if self.options.cppflow == 'on':
            self.requires("cppflow/2.0.0")

    def generate(self):
        tc = CMakeToolchain(self)

        tc.variables['docs'] = self.options.docs
        tc.variables['examples'] = self.options.examples
        tc.variables['tests'] = self.options.tests
        tc.variables['benchmarks'] = self.options.benchmarks
        tc.variables['logging'] = self.options.logging
        tc.variables['openmp'] = self.options.openmp
        tc.variables['dompi'] = self.options.mpi
        tc.variables['coverage'] = self.options.coverage
        tc.variables['doaf'] = self.options.af
        tc.variables['docimg'] = self.options.cimg
        tc.variables['docasa'] = self.options.casa
        tc.variables['cppflow'] = self.options.cppflow

        # List cases where we don't use ccache
        if ('GITHUB_ACTIONS' in os.environ.keys() and self.options.docs == 'off'):
            tc.variables['CMAKE_C_COMPILER_LAUNCHER'] = "ccache"
            tc.variables['CMAKE_CXX_COMPILER_LAUNCHER'] = "ccache"

        tc.variables['CMAKE_VERBOSE_MAKEFILE:BOOL'] = "ON"
        tc.variables['MPIEXEC_MAX_NUMPROCS'] = 2
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["purify"]


