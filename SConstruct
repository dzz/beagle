#######################3
# BUILD NOTES
#
# needs sdl2, sdl_mixer, sdl_image, python-devel
#

import fnmatch
import os
import sys


build_debug_mode = True

def populate_c_matches(matches, dir):
    for root, dirnames, filenames in os.walk(dir):
        for filename in fnmatch.filter(filenames, '*.c'):
            matches.append(os.path.join(root, filename))

def populate_h_matches(matches, dir):
    for root, dirnames, filenames in os.walk(dir):
        for filename in fnmatch.filter(filenames, '*.h'):
            path = os.path.dirname(os.path.join( root, filename))
            if not path in matches:
                matches.append(path)


def configure_linux(IncludePaths, Libs):
    def configure_portaudio(includes,libs):
        includes.append('/usr/include/portaudio/')
        libs.extend(['libportaudio', 'asound'])
    
    def configure_python(includes, libs):
        includes.append('/usr/include/python3.6m/')
        libs.extend( [ 'python3.6m', 'pthread', 'dl', 'util', 'm' ] )
        
    def configure_sdl(includes,libs):
        includes.append('/usr/include/SDL2/')
        libs.extend( ['SDL2', 'SDL2_image', 'SDLmain' ] )
    
    def configure_opengl(includes,libs):
        libs.extend( ['GL'])
        includes.append('./gl_includes/')

    configure_opengl(IncludePaths, Libs)
    configure_portaudio(IncludePaths, Libs) # first because python also needs pthread
    configure_python(IncludePaths, Libs)
    configure_sdl(IncludePaths, Libs)

def configure_win32(IncludePaths, Libs):
    def configure_portaudio(includes,libs):
        includes.append('C:\\clibs\\portaudio-r1891-build\\include\\')
        libs.append('C:\\clibs\\portaudio-r1891-build\\lib\\Win32\\ReleaseMinDependency\\portaudio_x86.lib')
    
    def configure_python(includes, libs):
        includes.append('C:\\Python34\\include\\')
        libs.append( 'C:\\Python34\\libs\\python34.lib' )
        
    def configure_sdl(includes,libs):
        includes.append('C:\\clibs\\SDL2-2.0.1\\include\\')
        includes.append('C:\\clibs\\SDL2_mixer-2.0.0\\include\\')
        includes.append('C:\\clibs\\SDL2_image-2.0.0\\include\\')
        libs.append( 'C:\\clibs\\SDL2-2.0.1\\lib\\x86\\SDL2.lib' )
        libs.append( 'C:\\clibs\\SDL2-2.0.1\\lib\\x86\\SDL2main.lib' )
        libs.append( 'C:\\clibs\\SDL2_image-2.0.0\\lib\\x86\\SDL2_image.lib' )
        libs.append('C:\\clibs\\SDL2_mixer-2.0.0\\lib\\x86\\SDL2_mixer.lib' )
    
    def configure_opengl(includes,libs):
        includes.append('gl_includes\\')
        libs.append('opengl32.lib')

    configure_opengl(IncludePaths, Libs)
    configure_portaudio(IncludePaths, Libs) # first because python also needs pthread
    configure_python(IncludePaths, Libs)
    configure_sdl(IncludePaths, Libs)


Libs            = []
SourceFiles     = []
IncludePaths    = []

CCFLAGS = ""
LINKFLAGS = ""

populate_c_matches(SourceFiles, 'src')
populate_h_matches(IncludePaths, 'src')

if sys.platform == 'linux2':
    env = Environment()
if sys.platform == 'win32':
    env = Environment( MSVC_VERSION='14.0', TARGET_ARCH='x86' )

if sys.platform == 'linux2':
    configure_linux(IncludePaths, Libs)
    CCFLAGS='-pthread -Wno-incompatible-pointer-types'
if sys.platform == 'win32':
    configure_win32(IncludePaths, Libs)
    CCFLAGS='/MT /O2'
    LINKFLAGS='/SUBSYSTEM:CONSOLE /NODEFAULTLIB:msvcrt.lib'
    
if build_debug_mode:
    print SourceFiles
    print IncludePaths
    print Libs

env.Program(
        './bin/beagle_runtime', 
        SourceFiles, 
        CPPPATH= IncludePaths,
        LIBPATH=['.','./lib/'],
        LIBS=Libs,
        LINKFLAGS = LINKFLAGS,
        CCFLAGS=CCFLAGS
)

