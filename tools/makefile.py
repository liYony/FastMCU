import os
import sys

from utils import *
from utils import _make_path_relative
import fmconfig

makefile = '''phony := all
all:

include config.mk

ifneq ($(MAKE_LIB),1)
TARGET := rtthread.elf
include src.mk
endif

$(if $(strip $(FM_ROOT)),,$(error FM_ROOT not defined))

include $(FM_ROOT)/tools/rtthread.mk
'''

def TargetMakefile(env):
    project = ProjectInfo(env)

    BSP_ROOT = os.path.abspath(env['BSP_ROOT'])
    FM_ROOT = os.path.abspath(env['FM_ROOT'])

    match_bsp = False
    if BSP_ROOT.startswith(FM_ROOT):
        match_bsp = True

    make = open('config.mk', 'w')

    make.write('BSP_ROOT ?= %s\n' % BSP_ROOT.replace('\\', '/'))
    make.write('FM_ROOT ?= %s\n' % FM_ROOT.replace('\\', '/'))
    make.write('\n')

    cross = os.path.abspath(fmconfig.EXEC_PATH)
    cross = os.path.join(cross, fmconfig.PREFIX)
    make.write('CROSS_COMPILE ?=%s' % cross.replace('\\', '\\\\'))
    make.write('\n')
    make.write('\n')

    make.write('CFLAGS :=%s' % (fmconfig.CFLAGS))
    make.write('\n')
    make.write('AFLAGS :=%s' % (fmconfig.AFLAGS))
    make.write('\n')
    make.write('LFLAGS :=%s' % (fmconfig.LFLAGS))
    make.write('\n')
    if 'CXXFLAGS' in dir(fmconfig):
        make.write('CXXFLAGS :=%s' % (fmconfig.CXXFLAGS))
        make.write('\n')
    if ('LIBS' in env):
        make.write('EXTERN_LIB := ')
        for tlib in env['LIBS']:
            make.write('-l%s ' % (tlib))
        if ('LIBPATH' in env):
            for tlibpath in env['LIBPATH']:
                make.write('-L%s ' % (tlibpath))
        make.write('\n')

    make.write('\n')

    Files   = project['FILES']
    Headers = project['HEADERS']
    CPPDEFINES = project['CPPDEFINES']

    paths = [os.path.normpath(i) for i in project['CPPPATH']]
    CPPPATH = []
    for path in paths:
        fn = os.path.normpath(path)
        if match_bsp:
            if fn.startswith(BSP_ROOT):
                fn = '$(BSP_ROOT)' + fn.replace(BSP_ROOT, '')
            elif fn.startswith(FM_ROOT):
                fn = '$(FM_ROOT)' + fn.replace(FM_ROOT, '')
        else:
            if fn.startswith(FM_ROOT):
                fn = '$(FM_ROOT)' + fn.replace(FM_ROOT, '')
            elif fn.startswith(BSP_ROOT):
                fn = '$(BSP_ROOT)' + fn.replace(BSP_ROOT, '')

        CPPPATH.append(fn)

    path = ''
    paths = CPPPATH
    for item in paths:
        path += '\t-I%s \\\n' % item

    make.write('CPPPATHS :=')
    if path[0] == '\t': path = path[1:]
    length = len(path)
    if path[length - 2] == '\\': path = path[:length - 2]
    make.write(path)
    make.write('\n')
    make.write('\n')

    defines = ''
    for item in project['CPPDEFINES']:
        defines += ' -D%s' % item
    make.write('DEFINES :=')
    make.write(defines)
    make.write('\n')

    files = Files
    Files = []
    for file in files:
        fn = os.path.normpath(file)
        if match_bsp:
            if fn.startswith(BSP_ROOT):
                fn = '$(BSP_ROOT)' + fn.replace(BSP_ROOT, '')
            elif fn.startswith(FM_ROOT):
                fn = '$(FM_ROOT)' + fn.replace(FM_ROOT, '')
        else:
            if fn.startswith(FM_ROOT):
                fn = '$(FM_ROOT)' + fn.replace(FM_ROOT, '')
            elif fn.startswith(BSP_ROOT):
                fn = '$(BSP_ROOT)' + fn.replace(BSP_ROOT, '')

        Files.append(fn)
        # print(fn)

    src = open('src.mk', 'w')
    files = Files
    src.write('SRC_FILES :=\n')
    for item in files:
        src.write('SRC_FILES +=%s\n' % item.replace('\\', '/'))

    make = open('Makefile', 'w')
    make.write(makefile)
    make.close()

    return
