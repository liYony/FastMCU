import os
import sys
import operator
from options import AddOptions
from SCons.Script import *

Projects = []
BuildOptions = {}
fmcu_root = ''
bsp_root = ''
sdk_lib = ''
bsp_library = ''
bsp_drivers = ''

def _PretreatListParameters(target_list):
    while '' in target_list: # remove null strings
        target_list.remove('')
    while ' ' in target_list: # remove ' '
        target_list.remove(' ')

    if(len(target_list) == 0):
        return False # ignore this list, don't add this list to the parameter

    return True # permit to add this list to the parameter

def MergeGroup(src_group, group):
    src_group['src'] = src_group['src'] + group['src']
    src_group['src'].sort()
    if 'CFLAGS' in group:
        if 'CFLAGS' in src_group:
            src_group['CFLAGS'] = src_group['CFLAGS'] + group['CFLAGS']
        else:
            src_group['CFLAGS'] = group['CFLAGS']
    if 'CCFLAGS' in group:
        if 'CCFLAGS' in src_group:
            src_group['CCFLAGS'] = src_group['CCFLAGS'] + group['CCFLAGS']
        else:
            src_group['CCFLAGS'] = group['CCFLAGS']
    if 'CXXFLAGS' in group:
        if 'CXXFLAGS' in src_group:
            src_group['CXXFLAGS'] = src_group['CXXFLAGS'] + group['CXXFLAGS']
        else:
            src_group['CXXFLAGS'] = group['CXXFLAGS']
    if 'CPPPATH' in group:
        if 'CPPPATH' in src_group:
            src_group['CPPPATH'] = src_group['CPPPATH'] + group['CPPPATH']
        else:
            src_group['CPPPATH'] = group['CPPPATH']
    if 'CPPDEFINES' in group:
        if 'CPPDEFINES' in src_group:
            src_group['CPPDEFINES'] = src_group['CPPDEFINES'] + group['CPPDEFINES']
        else:
            src_group['CPPDEFINES'] = group['CPPDEFINES']
    if 'ASFLAGS' in group:
        if 'ASFLAGS' in src_group:
            src_group['ASFLAGS'] = src_group['ASFLAGS'] + group['ASFLAGS']
        else:
            src_group['ASFLAGS'] = group['ASFLAGS']

    # for local CCFLAGS/CPPPATH/CPPDEFINES
    if 'LOCAL_CFLAGS' in group:
        if 'LOCAL_CFLAGS' in src_group:
            src_group['LOCAL_CFLAGS'] = src_group['LOCAL_CFLAGS'] + group['LOCAL_CFLAGS']
        else:
            src_group['LOCAL_CFLAGS'] = group['LOCAL_CFLAGS']
    if 'LOCAL_CCFLAGS' in group:
        if 'LOCAL_CCFLAGS' in src_group:
            src_group['LOCAL_CCFLAGS'] = src_group['LOCAL_CCFLAGS'] + group['LOCAL_CCFLAGS']
        else:
            src_group['LOCAL_CCFLAGS'] = group['LOCAL_CCFLAGS']
    if 'LOCAL_CXXFLAGS' in group:
        if 'LOCAL_CXXFLAGS' in src_group:
            src_group['LOCAL_CXXFLAGS'] = src_group['LOCAL_CXXFLAGS'] + group['LOCAL_CXXFLAGS']
        else:
            src_group['LOCAL_CXXFLAGS'] = group['LOCAL_CXXFLAGS']
    if 'LOCAL_CPPPATH' in group:
        if 'LOCAL_CPPPATH' in src_group:
            src_group['LOCAL_CPPPATH'] = src_group['LOCAL_CPPPATH'] + group['LOCAL_CPPPATH']
        else:
            src_group['LOCAL_CPPPATH'] = group['LOCAL_CPPPATH']
    if 'LOCAL_CPPDEFINES' in group:
        if 'LOCAL_CPPDEFINES' in src_group:
            src_group['LOCAL_CPPDEFINES'] = src_group['LOCAL_CPPDEFINES'] + group['LOCAL_CPPDEFINES']
        else:
            src_group['LOCAL_CPPDEFINES'] = group['LOCAL_CPPDEFINES']

    if 'LINKFLAGS' in group:
        if 'LINKFLAGS' in src_group:
            src_group['LINKFLAGS'] = src_group['LINKFLAGS'] + group['LINKFLAGS']
        else:
            src_group['LINKFLAGS'] = group['LINKFLAGS']
    if 'LIBS' in group:
        if 'LIBS' in src_group:
            src_group['LIBS'] = src_group['LIBS'] + group['LIBS']
        else:
            src_group['LIBS'] = group['LIBS']
    if 'LIBPATH' in group:
        if 'LIBPATH' in src_group:
            src_group['LIBPATH'] = src_group['LIBPATH'] + group['LIBPATH']
        else:
            src_group['LIBPATH'] = group['LIBPATH']
    if 'LOCAL_ASFLAGS' in group:
        if 'LOCAL_ASFLAGS' in src_group:
            src_group['LOCAL_ASFLAGS'] = src_group['LOCAL_ASFLAGS'] + group['LOCAL_ASFLAGS']
        else:
            src_group['LOCAL_ASFLAGS'] = group['LOCAL_ASFLAGS']

def GetCurrentDir():
    conscript = File('SConscript')
    fn = conscript.rfile()
    name = fn.name
    path = os.path.dirname(fn.abspath)
    return path

PatchedPreProcessor = SCons.cpp.PreProcessor

def PrepareCreate(fm_root_dir, sdk, library, drivers):
    global BuildOptions, fmcu_root, bsp_root, sdk_lib, bsp_library, bsp_drivers

    AddOptions()
    fmcu_root = os.path.abspath(fm_root_dir)
    bsp_root = os.getcwd()
    sdk_lib = sdk
    bsp_library = library
    bsp_drivers = drivers

    if GetOption('menuconfig'):
        print('menuconfig.')
        from mnconfig import ShowMenuconfig
        ShowMenuconfig(fmcu_root)
        exit(0)

    # parse rtconfig.h to get used component
    PreProcessor = PatchedPreProcessor()
    f = open('fmconfig.h', 'r')
    contents = f.read()
    f.close()
    PreProcessor.process_contents(contents)
    BuildOptions = PreProcessor.cpp_namespace

def GetDepend(depend):
    building = True
    if type(depend) == type('str'):
        if not depend in BuildOptions or BuildOptions[depend] == 0:
            building = False
        elif BuildOptions[depend] != '':
            return BuildOptions[depend]

        return building

    # for list type depend
    for item in depend:
        if item != '':
            if not item in BuildOptions or BuildOptions[item] == 0:
                building = False

    return building

def DefineGroup(name, src, depend, **parameters):
    if not GetDepend(depend):
        return []
    
    # find exist group and get path of group
    group_path = ''
    for g in Projects:
        if g['name'] == name:
            group_path = g['path']
    if group_path == '':
        group_path = GetCurrentDir()

    group = parameters
    group['name'] = name
    group['path'] = group_path
    if type(src) == type([]):
        # remove duplicate elements from list
        src = list(set(src))
        group['src'] = File(src)
    else:
        group['src'] = src

    if 'LOCAL_CPPPATH' in group:
        paths = []
        for item in group['LOCAL_CPPPATH']:
            paths.append(os.path.abspath(item))
        group['LOCAL_CPPPATH'] = paths

    # check whether to build group library
    # if 'LIBRARY' in group:
    #     objs = Env.Library(name, group['src'])
    # else:
    #     # only add source
    #     objs = group['src']
    objs = group['src']

    # merge group
    for g in Projects:
        if g['name'] == name:
            # merge to this group
            MergeGroup(g, group)
            return objs

    def PriorityInsertGroup(groups, group):
        length = len(groups)
        for i in range(0, length):
            if operator.gt(groups[i]['name'].lower(), group['name'].lower()):
                groups.insert(i, group)
                return
        groups.append(group)

    # add a new group
    PriorityInsertGroup(Projects, group)

    return objs

def StartCreate():
    if GetOption('dist'):
        print('Start create dist project.')
        from dist import MakeProjectDist
        MakeProjectDist(fmcu_root, bsp_root, sdk_lib, bsp_library, bsp_drivers)
    if GetOption('keil'):
        print('Start create keil project.')
        from keil import MDK5Project
        MDK5Project('project.uvprojx', Projects)
    if GetOption('vscode'):
        from vsc import GenerateVSCode
        GenerateVSCode(Projects)
    if GetOption('cmake'):
        from cmake import CMakeProject
        CMakeProject(Projects)
    exit(0)
