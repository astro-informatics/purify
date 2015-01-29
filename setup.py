from os.path import basename, dirname, join, abspath
from os import chdir, getcwd, environ
from setuptools import setup, Extension
from distutils.command.build import build as dBuild
from setuptools.command.install import install as dInstall
from setuptools.command.build_ext import build_ext as dBuildExt
from setuptools.command.bdist_egg import bdist_egg as dBuildDistEgg
from setuptools.command.sdist import sdist as dSDist
from setuptools.command.egg_info import egg_info as dEggInfo
from distutils.dir_util import mkpath

source_dir = dirname(abspath(__file__))
package_dir = join(source_dir, 'pkg_install')
long_description = open(join(dirname(__file__), 'README.rst'), 'r').read()
mkpath(package_dir)

isCASA = 'CASAPATH' in environ
if isCASA:
    # CASA 4.2.2 has personal ideas as to where numpy includes should go
    casa_path = environ['CASAPATH'].split()[0]
    environ['CFLAGS'] = environ.get('CFLAGS', '') + ' -I%s/include' % casa_path

def cmake_cache_line(variable, value, type='STRING'):
    return "set(%s \"%s\" CACHE %s \"\")\n" % (variable, value, type)


def as_preload_file(name, info):
    """ Python information to cmake commandline """
    result = []
    if len(info.get('libraries', [])):
        libs = ('-l' + '-l'.join(info['libraries'])).rstrip().lstrip()
        if len(info.get('library_dirs', [])):
            libdirs = ('-L' + '-L'.join(info['library_dirs']))\
                .rstrip().lstrip()
        else:
            libdirs = ""
        result.append(
            cmake_cache_line("%s_LIBRARIES" % name, "%s %s" % (libdirs, libs)))
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
    if cmake is None and isCASA:
        # Tries to out-smart CASA.
        # Look places cmake might be that casa removes from path.
        directories = [
            join('/', 'usr', 'local', 'bin'),
            join(environ['HOME'], 'bin'),
            join(environ['HOME'], '.local', 'bin'),
            join(environ['HOME'], 'usr', 'bin'),
            join('/', 'sw', 'bin')  # -- default Fink location
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

            First puts variables into a cache file. This is safer that going
            through the command-line.
        """
        from sys import executable
        from os import environ
        # other args
        other_args = [
            cmake_cache_line('nobins', 'TRUE', 'BOOL'),
            cmake_cache_line('PYTHON_EXECUTABLE', executable, 'PATH'),
            cmake_cache_line('dont_install_headers', 'TRUE', 'BOOL'),
            cmake_cache_line(
                'LINK_TO_ABSOLUTE_PYTHON_PATH', isCASA, 'BOOL'),
            '\n'
        ]
        if isCASA:
            other_args.append(
                cmake_cache_line('CMAKE_PREFIX_PATH',
                    environ['CASAPATH'].split()[0], 'PATH')
            )


        with open(filename, 'w') as file:
            file.writelines(other_args)
        return ['-C%s' % filename]

    def _configure(self, build_dir):
        from distutils import log
        from distutils.spawn import spawn

        current_dir = getcwd()
        mkpath(build_dir)
        command_line = self.configure_cmdl(join(build_dir, 'Variables.cmake'))
        log.info(
            "CMake: configuring with variables in %s "
            % join(build_dir, 'Variables.cmake')
        )
        cmake = cmake_executable()

        try:
            print "build_dirt ", build_dir
            chdir(build_dir)
            spawn([cmake] + command_line + [source_dir])
        finally:
            chdir(current_dir)

    def _build(self, build_dir):
        from distutils import log
        from distutils.spawn import spawn

        log.info("CMake: building in %s" % build_dir)
        current_dir = getcwd()
        cmake = cmake_executable()

        try:
            chdir(build_dir)
            spawn([cmake, '--build', build_dir])
        finally:
            chdir(current_dir)

    def run(self):
        from os.path import abspath

        build_dir = join(dirname(abspath(__file__)), self.build_base)
        self._configure(build_dir)
        self._build(build_dir)
        self._install(build_dir, package_dir)
        try:
            prior = getattr(self.distribution, 'running_binary', False)
            self.distribution.running_binary = True
            self.distribution.have_run['egg_info'] = 0
            dBuild.run(self)
        finally:
            self.distribution.running_binary = prior

    def _install(self, build_dir, install_dir):
        from distutils import log
        from os.path import abspath

        current_cwd = getcwd()
        build_dir = abspath(build_dir)
        cmake = cmake_executable()
        pkg = abspath(install_dir)
        clib = join(pkg, 'purify', 'lib')
        log.info("CMake: Installing package to %s" % pkg)
        try:
            chdir(build_dir)
            self.spawn([
                cmake,
                '-DPYTHON_PKG_DIR=\'%s\'' % pkg,
                '-DLIBRARY_INSTALL_PATH=\'%s\'' % clib,
                source_dir
            ])
            self.spawn([cmake, '--build', '.', '--target', 'install'])
        finally:
            chdir(current_cwd)


class Install(dInstall):
    def run(self):
        from distutils import log
        from os.path import abspath
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
            self.spawn([
                cmake,
                '-DCMAKE_INSTALL_PREFIX=\'%s\'' % prefix,
                '-DPYTHON_PKG_DIR=\'%s\'' % pkg,
                '-DLIBRARY_INSTALL_PATH=\'%s\'' % clib,
                '..'
            ])
            self.spawn([cmake, '--build', build_dir, '--target', 'install'])
        finally:
            chdir(current_cwd)

        try:
            prior = getattr(self.distribution, 'running_binary', False)
            self.distribution.running_binary = prior
            self.distribution.have_run['egg_info'] = 0
            dInstall.run(self)
        finally:
            self.distribution.running_binary = prior

        self.install_casa_task(pkg, build_dir)

    def install_casa_task(self, install_dir, build_dir):
        from os import environ
        from subprocess import call
        if not isCASA:
            return

        pwd = getcwd()
        try:
            chdir(join(build_dir, 'casa_task'))
            task_dir = join(install_dir, "purify", "casa")
            r = call(("buildmytasks -i=%s -o=task.py" % task_dir).split())
            if r != 0:
                print("Could not build and install casa task.")
                print("Purify will only be available as a python package.")
        finally:
            chdir(pwd)


class BuildExt(dBuildExt):
    def __init__(self, *args, **kwargs):
        dBuildExt.__init__(self, *args, **kwargs)

    def run(self):
        pass


class BuildDistEgg(dBuildDistEgg):
    def __init__(self, *args, **kwargs):
        dBuildDistEgg.__init__(self, *args, **kwargs)

    def run(self):

        try:
            prior = getattr(self.distribution, 'running_binary', False)
            self.distribution.running_binary = True
            self.run_command('build')
            dBuildDistEgg.run(self)
        finally:
            self.distribution.running_binary = prior


class EggInfo(dEggInfo):
    def __init__(self, *args, **kwargs):
        dEggInfo.__init__(self, *args, **kwargs)

    def run(self):
        from setuptools.command.egg_info import manifest_maker
        from os import listdir
        which_template = 'MANIFEST.source.in'

        dist = self.distribution
        old_values = dist.ext_modules, dist.ext_package, \
            dist.packages, dist.package_dir
        condition = len(listdir(package_dir)) != 0 \
            and getattr(self.distribution, 'running_binary', False)
        if condition:
            which_template = 'MANIFEST.binary.in'
        else:
            dist.ext_modules, dist.ext_package = None, None
            dist.packages, dist.package_dir = None, None

        try:
            old_template = manifest_maker.template
            manifest_maker.template = which_template
            dEggInfo.run(self)
        finally:
            manifest_maker.template = old_template
            dist.ext_modules, dist.ext_package = old_values[:2]
            dist.packages, dist.package_dir = old_values[2:]


class SDist(dSDist):
    def __init__(self, *args, **kwargs):
        dSDist.__init__(self, *args, **kwargs)

    def run(self):
        dist = self.distribution
        try:
            old_values = dist.ext_modules, dist.ext_package, \
                dist.packages, dist.package_dir
            dist.ext_modules, dist.ext_package = None, None
            dist.packages, dist.package_dir = None, None
            dSDist.run(self)
        finally:
            dist.ext_modules, dist.ext_package = old_values[:2]
            dist.packages, dist.package_dir = old_values[2:]


try:
    cwd = getcwd()
    chdir(source_dir)
    packages = ['cython', 'numpy', 'scipy', 'pytest', 'pandas']
    setup(
        name="purify",
        version="0.1.1",

        # NOTE: python-dateutil is required by pandas,
        # but is not installed by it (pandas == 0.13)
        setup_requires=packages,
        install_requires=packages,
        platforms=['GNU/Linux', 'Unix', 'Mac OS-X'],

        zip_safe=False,
        cmdclass={
            'build': Build, 'install': Install,
            'build_ext': BuildExt, 'bdist_egg': BuildDistEgg,
            'egg_info': EggInfo
        },

        author="Jason McEwen",
        author_email="j.mcewen@ucl.ac.uk",
        description="Purify does what it does well",
        license="GPL-2",
        url="https://github.com/basp-group/purify",
        ext_modules=[Extension('purify.cparams', [])],
        ext_package='purify',
        packages=['purify', 'purify.tests'],
        package_dir={'': str(basename(package_dir))},
        include_package_data=True,

        keywords="radio astronomy",
        classifiers=[
            'Development Status :: 0 - Beta',
            'Intended Audience :: Developers',
            'Intended Audience :: Science/Research',
            'License :: OSI Approved :: GNU Lesser '
                'General Public License v2 (LGPLv2)',
            'Operating System :: OS Independent',
            'Programming Language :: Python :: 2.6',
            'Programming Language :: Python :: 2.7',
            'Topic :: Scientific/Engineering',
            'Topic :: Scientific/Engineering :: Astronomy',
            'Topic :: Scientific/Engineering :: Information Analysis',
            'Topic :: Software Development :: Libraries :: Python Modules',
            'Topic :: Software Development '
                ':: Libraries :: Application Frameworks',
        ],
        long_description=long_description
    )
finally:
    chdir(cwd)
