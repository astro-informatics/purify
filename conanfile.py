from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
from conan.tools.files import symlinks
import os

class PurifyConan(ConanFile):
    name = "purify"
    version = "3.0.1"
    url = "https://github.com/astro-informatics/purify"
    license = "GPL-2.0"
    description = "PURIFY is an open-source collection of routines written in C++ available under the license below. It implements different tools and high-level to perform radio interferometric imaging, i.e. to recover images from the Fourier measurements taken by radio interferometric telescopes."


    settings = "os", "compiler", "build_type", "arch"
    requires = ["fftw/3.3.9", "eigen/3.4.0","catch2/3.4.0","benchmark/1.8.2","yaml-cpp/0.7.0", "boost/1.82.0", "cfitsio/4.2.0", "sopt/4.0.0"]
    #generators = "CMakeDeps"
    exports_sources = "cpp/*", "cmake_files/*", "CMakeLists.txt"
    options = {"docs":['on','off'],
               "examples":['on','off'],
               "tests":['on','off'],
               "benchmarks":['on','off'],
               "openmp":['on','off'],
               "dompi":['on','off'],
               "coverage":['on','off'],
               "af": ['on', 'off'],
               "cimg": ['on','off'],
               "docasa": ['on','off'],
               "onnxrt": ['on', 'off']}
    default_options = {"docs": 'off',
                       "examples":'off',
                       "tests": 'on',
                       "benchmarks": 'off',
                       "openmp": 'off',
                       "dompi": 'off',
                       "coverage": 'off',
                       "af": 'off',
                       "cimg": 'off',
                       "docasa": 'on',
                       "onnxrt": 'off'}

    def configure(self):

        self.options["sopt"].onnxrt = self.options.onnxrt
        self.options["sopt"].dompi = self.options.dompi
        self.options["sopt"].openmp = self.options.openmp
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

        if self.options.docs == 'on':
            self.requires("doxygen/1.9.2")

        if self.options.cimg == 'on':
            self.requires("cimg/3.0.2")

        if self.options.onnxrt == 'on':
            self.requires("onnxruntime/1.16.3")


    def generate(self):
        tc = CMakeToolchain(self)

        tc.cache_variables['docs'] = self.options.docs
        tc.cache_variables['examples'] = self.options.examples
        tc.cache_variables['tests'] = self.options.tests
        tc.cache_variables['benchmarks'] = self.options.benchmarks
        tc.cache_variables['openmp'] = self.options.openmp
        tc.cache_variables['dompi'] = self.options.dompi
        tc.cache_variables['coverage'] = self.options.coverage
        tc.cache_variables['doaf'] = self.options.af
        tc.cache_variables['docimg'] = self.options.cimg
        tc.cache_variables['docasa'] = self.options.docasa
        tc.cache_variables['onnxrt'] = self.options.onnxrt

        # List cases where we don't use ccache
        if ('GITHUB_ACTIONS' in os.environ.keys() and self.options.docs == 'off'):
            tc.cache_variables['CMAKE_C_COMPILER_LAUNCHER'] = "ccache"
            tc.cache_variables['CMAKE_CXX_COMPILER_LAUNCHER'] = "ccache"

        tc.cache_variables['CMAKE_VERBOSE_MAKEFILE:BOOL'] = "ON"
        tc.cache_variables['MPIEXEC_MAX_NUMPROCS'] = 2
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


