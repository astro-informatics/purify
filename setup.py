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

def cmake_executable():
    """ Path to cmake executable """
    from os.path import join, exists
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
        prefix = abspath(self.root or self.install_base)
        pkg = abspath(self.install_lib)
        clib = abspath(join(self.install_lib, 'purify', 'lib'))
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

setup(
    name = "Purify",
    version = "0.1",

    setup_requires = ["setuptools_git >= 0.3"],
    # NOTE: python-dateutil is required by pandas,
    # but is not installed by it (pandas == 0.13)
    install_requires = ['cython', 'numpy', 'scipy',
        'pandas', 'nose', 'virtualenv'],
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
    long_description = open(join(dirname(__file__), 'README.md'), 'r').read()
)
