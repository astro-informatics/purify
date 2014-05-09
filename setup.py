from os.path import basename, dirname, join
from os import getcwd
from setuptools import setup, Extension
from distutils.command.build import build as dBuild
from setuptools.command.install import install as dInstall
from setuptools.command.build_ext import build_ext as dBuildExt
from setuptools.command.bdist_egg import bdist_egg as dBuildDistEgg
from setuptools.command.egg_info import egg_info as dEggInfo
from distutils.dir_util import mkpath

source_dir = getcwd()
package_dir = join(source_dir, 'pkg_install')
mkpath(package_dir)

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

def cmake_executable():
    """ Path to cmake executable """
    from os.path import exists
    from os import environ
    from distutils.spawn import find_executable
    cmake = find_executable('cmake')
    if cmake is None and 'CASAPATH' in environ:
        # Tries to out-smart CASA.
        # Look places cmake might be that casa removes from path.
        directories = [
            join('/', 'usr', 'local', 'bin'),
            join(environ['HOME'], 'bin'),
            join(environ['HOME'], '.local', 'bin'),
            join(environ['HOME'], 'usr', 'bin'),
            join('/', 'sw', 'bin') # -- default Fink location
        ]
        for directory in directories:
            if exists(join(directory, 'cmake')):
                cmake = join(directory, 'cmake')
                break
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
        # other args
        other_args = [
            cmake_cache_line('nobins', 'TRUE', 'BOOL'),
            cmake_cache_line('PYTHON_EXECUTABLE', executable, 'PATH'),
            cmake_cache_line('dovirtualenv', 'FALSE', 'BOOL'),
            cmake_cache_line('dont_install_headers', 'TRUE', 'BOOL'),
            '\n',
        ]

        with open(filename, 'w') as file: file.writelines(other_args)
        return ['-C%s' % filename]

    def _configure(self, build_dir):
        from distutils import log
        from distutils.spawn import spawn
        from os import chdir

        current_dir = getcwd()
        mkpath(build_dir)
        command_line = self.configure_cmdl(join(build_dir, 'Variables.cmake'))
        log.info(
                "CMake: configuring with variables in %s "
                % join(build_dir, 'Variables.cmake')
        )
        cmake = cmake_executable()

        try:
            chdir(build_dir)
            spawn([cmake] + command_line + [source_dir])
        finally: chdir(current_dir)

    def _build(self, build_dir):
        from distutils import log
        from distutils.spawn import spawn
        from os import chdir

        log.info("CMake: building in %s" % build_dir)
        current_dir = getcwd()
        cmake = cmake_executable()

        try:
            chdir(build_dir)
            spawn([cmake, '--build', '.'])
        finally: chdir(current_dir)

    def run(self):
        from os.path import abspath

        build_dir = join(dirname(abspath(__file__)), self.build_base)
        self._configure(build_dir)
        self._build(build_dir)
        self._install(build_dir, package_dir)
        dBuild.run(self)

    def _install(self, build_dir, install_dir):
        from distutils import log
        from os.path import abspath
        from os import chdir

        current_cwd = getcwd()
        build_dir = abspath(build_dir)
        cmake = cmake_executable()
        pkg = abspath(install_dir)
        clib = join(pkg, 'purify', 'lib')
        log.info("CMake: Installing package to %s" % pkg)
        try:
            chdir(build_dir)
            self.spawn([cmake,
                '-DPYTHON_PKG_DIR=\'%s\'' % pkg,
                '-DLIBRARY_INSTALL_PATH=\'%s\'' % clib,
                source_dir
            ])
            self.spawn([cmake, '--build', '.', '--target', 'install'])
        finally: chdir(current_cwd)

class Install(dInstall):
    def run(self):
        from distutils import log
        from os.path import abspath
        from os import chdir
        self.distribution.run_command('build')
        current_cwd = getcwd()
        build_dir = join(dirname(abspath(__file__)), self.build_base)
        cmake = cmake_executable()
        prefix = abspath(self.root or self.install_base)
        pkg = abspath(self.install_lib)
        clib = abspath(join(self.install_lib, 'purify', 'lib'))
        log.info("CMake: Installing package to %s" % pkg)
        try:
            chdir(build_dir)
            self.spawn([cmake,
                '-DCMAKE_INSTALL_PREFIX=\'%s\'' % prefix,
                '-DPYTHON_PKG_DIR=\'%s\'' % pkg,
                '-DLIBRARY_INSTALL_PATH=\'%s\'' % clib,
                '..'
            ])
            self.spawn([cmake, '--build', '.', '--target', 'install'])
        finally: chdir(current_cwd)
        dInstall.run(self)

class BuildExt(dBuildExt):
    def __init__(self, *args, **kwargs):
        dBuildExt.__init__(self, *args, **kwargs)
    def run(self):pass
class BuildDistEgg(dBuildDistEgg):
    def __init__(self, *args, **kwargs):
        dBuildDistEgg.__init__(self, *args, **kwargs)
    def run(self):
        self.run_command('build')
        dBuildDistEgg.run(self)
class EggInfo(dEggInfo):
    def __init__(self, *args, **kwargs):
        dEggInfo.__init__(self, *args, **kwargs)
    def run(self):
        from distutils import log
        from os.path import exists
        from os import listdir
        if exists(package_dir) == False or len(listdir(package_dir)) == 0:
            log.info("CMake package directory does not exist: will run build.")
            self.run_command('build')
        else:
            log.info("Using existing CMake package directory %s" % package_dir)
        dEggInfo.run(self)

setup(
    name = "purify",
    version = "0.1",

    # NOTE: python-dateutil is required by pandas,
    # but is not installed by it (pandas == 0.13)
    install_requires = ['cython', 'numpy', 'scipy',
        'pandas', 'nose', 'virtualenv'],
    platforms = ['GNU/Linux','Unix','Mac OS-X'],

    zip_safe = False,
    cmdclass = {
        'build': Build, 'install': Install,
        'build_ext': BuildExt, 'bdist_egg': BuildDistEgg,
        'egg_info': EggInfo
    },

    author = "Jason McEwen",
    description = "Purify does what it does well",
    license = "GPL-2",
    url = "https://github.com/basp-group/purify",
    ext_modules = [Extension('purify.cparams', [])],
    ext_package = 'purify',
    packages = ['purify', 'purify.tests'],
    package_dir = {'': basename(package_dir)},
    include_package_data=True,

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
    long_description = open(join(dirname(__file__), 'README.md'), 'r').read()
)
