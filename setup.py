from setuptools import setup
from distutils.command.build import build as dBuild
from setuptools.command.install import install as dInstall
from os.path import dirname, join

def cmake_cache_line(variable, value, type='STRING'):
    return "set(%s \"%s\" CACHE %s \"\")\n" % (variable, value, type)
def as_preload_file(name, info):
    """ Python information to cmake commandline """
    result = []
    if len(info.get('libraries', [])):
        libs = ('-l' + '-l'.join(info['libraries'])).rstrip().lstrip()
        if len(info.get('library_dirs', [])):
            libdirs =  ('-L' + '-L'.join(info['library_dirs'])).rstrip().lstrip()
        else: libdirs = ""
        result.append(cmake_cache_line("%s_LIBRARIES" % name,
            "%s %s" %(libdirs, libs)))
    if len(info.get('include_dirs', [])):
        incs = ';'.join(info['include_dirs']).rstrip().lstrip()
        result.append(cmake_cache_line("%s_INCLUDE_DIRS" % name, incs))
    return result

def get_algebra_args(name='blas'):
    """ Gets blas/lapack arguments from numpy """
    from numpy.distutils.system_info import get_info
    for blasname in [name, '%s_mkl' % name, '%s_atlas' % name]:
        if len(get_info(blasname)) != 0: break
    else: raise RuntimeError("Could not figure out blas used in numpy")
    result = as_preload_file(name.upper(), get_info(blasname))
    if len(result) == 0:
        raise RuntimeError("Could not figure out %s used in numpy" % name)
    return result

def get_fftw3_args():
    """ Gets fftw3 arguments from numpy """
    from numpy.distutils.system_info import get_info
    result = as_preload_file('FFTW3', get_info('fftw3'))
    if len(result) == 0:
        raise RuntimeError("Could not figure out fftw3 used in numpy")
    return result

def get_casa_args():
    """ If in CASA, then sets library location by hand

        CASA removes LD_LIBRARY_PATH, PATH, and friends. So we have to install into whatever CASA
        wants.
    """
    from os import environ
    from os.path import join, exists
    if 'CASAPATH' not in environ: return []
    casapath = environ['CASAPATH'].split()[0]
    casalibs = join(casapath, 'Frameworks')
    if exists(casalibs):
        return [cmake_cache_line('LIBRARY_INSTALL_PATH', casalibs)]
    return []

def cmake_executable():
    """ Path to cmake executable """
    from distutils.spawn import find_executable
    cmake = find_executable('cmake')
    if cmake is None:
        raise RuntimeError('Could not find cmake executable in path')
    return cmake

class Build(dBuild):
    """ Build that runs cmake. """

    def configure_cmdl(self, filename):
        """ Creates cmake command-line

            First puts variables into a cache file. This is safer that going through the
            command-line.
        """
        from sys import executable
        blas_args = get_algebra_args()
        fftw3_args = get_fftw3_args()
        # CASA requires special attention to work
        casa_args = get_casa_args()
        # other args
        other_args = [
            cmake_cache_line('nobins', 'TRUE', 'BOOL'),
            cmake_cache_line('PYTHON_EXECUTABLE', executable, 'PATH'),
            cmake_cache_line('dovirtualenv', 'FALSE', 'BOOL'),
            cmake_cache_line('dont_install_headers', 'TRUE', 'BOOL'),
            '\n',
        ]

        with open(filename, 'w') as file:
            file.writelines(blas_args + fftw3_args + casa_args + other_args)
        return ['-C%s' % filename, '..']

    def run(self):
        from setuptools import find_packages
        from distutils.spawn import spawn
        from distutils.dir_util import mkpath
        from os.path import abspath
        from os import chdir, getcwd

        current_cwd = getcwd()
        build_dir = join(dirname(abspath(__file__)), self.build_base)
        mkpath(build_dir)
        command_line = self.configure_cmdl(join(build_dir, 'Variables.cmake'))
        cmake = cmake_executable()

        try:
            chdir(build_dir)
            spawn([cmake] + command_line)
            spawn([cmake, '--build', '.'])
            self.distribution.packages \
                = find_packages(join(build_dir, 'python'), exclude=['*.tests'])
        finally: chdir(current_cwd)

class Install(dInstall):
    def run(self):
        from setuptools import find_packages
        from os.path import abspath
        from os import chdir, getcwd
        self.distribution.run_command('build')
        current_cwd = getcwd()
        build_dir = join(dirname(abspath(__file__)), self.build_base)
        self.distribution.packages \
            = find_packages(join(build_dir, 'python'), exclude=['*.tests'])
        cmake = cmake_executable()
        try:
            chdir(build_dir)
            self.spawn([cmake,
                '-DCMAKE_INSTALL_PREFIX=\'%s\'' % self.install_base,
                '-DPYTHON_PKG_DIR=\'%s\'' % self.install_lib,
                '..'
            ])
            self.spawn([cmake, '--build', '.', '--target', 'install'])
        finally: chdir(current_cwd)

setup(
    name = "Purify",
    version = "0.1",

    setup_requires = ["setuptools_git >= 0.3"],
    # NOTE: python-dateutil is required by pandas,
    # but is not installed by it (pandas == 0.13)
    install_requires = ['cython', 'numpy', 'scipy',
        'python-dateutil', 'pandas', 'nose', 'virtualenv'],
    platforms = ['GNU/Linux','Unix','Mac OS-X'],

    include_package_data=True,
    exclude_package_data = {'': ['.gitignore', 'Makefile'] },

    zip_safe = False,
    cmdclass = {'build': Build, 'install': Install},

    author = "Jason McEwen",
    description = "Purify does what it does well",
    license = "GPL-2",
    url = "https://github.com/basp-group/purify",

    keywords= "radio astronomy",
    classifiers = [
         'Development Status :: 0 - Beta',
         'Intended Audience :: Developers',
         'Intended Audience :: Science/Research',
         'License :: OSI Approved :: GNU Lesser General Public License v2 (LGPLv2)',
         'Operating System :: OS Independent',
         'Programming Language :: Python :: 2.6',
         'Programming Language :: Python :: 2.7',
         'Topic :: Scientific/Engineering',
         'Topic :: Scientific/Engineering :: Astronomy',
         'Topic :: Scientific/Engineering :: Information Analysis',
         'Topic :: Software Development :: Libraries :: Python Modules',
         'Topic :: Software Development :: Libraries :: Application Frameworks',
    ],
    long_description = open(join(dirname(__file__), 'README.txt'), 'r').read()
)
