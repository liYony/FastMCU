import os
import sys
import string

import xml.etree.ElementTree as etree
from xml.etree.ElementTree import SubElement
from utils import _make_path_relative
from utils import xml_indent

fs_encoding = sys.getfilesystemencoding()

def _get_filetype(fn):
    if fn.rfind('.cpp') != -1 or fn.rfind('.cxx') != -1:
        return 8

    if fn.rfind('.c') != -1 or fn.rfind('.C') != -1:
        return 1

    # assemble file type
    if fn.rfind('.s') != -1 or fn.rfind('.S') != -1:
        return 2

    # header type
    if fn.rfind('.h') != -1:
        return 5

    if fn.rfind('.lib') != -1:
        return 4

    if fn.rfind('.o') != -1:
        return 3

    # other filetype
    return 5

def MDK4AddGroup(ProjectFiles, parent, name, files, project_path, group_scons):
    # don't add an empty group
    if len(files) == 0:
        return

    group = SubElement(parent, 'Group')
    group_name = SubElement(group, 'GroupName')
    group_name.text = name

    for f in files:
        fn = f.rfile()
        name = fn.name
        path = os.path.dirname(fn.abspath)

        basename = os.path.basename(path)
        path = _make_path_relative(project_path, path)
        path = os.path.join(path, name)

        files = SubElement(group, 'Files')
        file = SubElement(files, 'File')
        file_name = SubElement(file, 'FileName')
        name = os.path.basename(path)

        if name.find('.cpp') != -1:
            obj_name = name.replace('.cpp', '.o')
        elif name.find('.c') != -1:
            obj_name = name.replace('.c', '.o')
        elif name.find('.s') != -1:
            obj_name = name.replace('.s', '.o')
        elif name.find('.S') != -1:
            obj_name = name.replace('.s', '.o')

        if ProjectFiles.count(obj_name):
            name = basename + '_' + name
        ProjectFiles.append(obj_name)
        file_name.text = name # name.decode(fs_encoding)
        file_type = SubElement(file, 'FileType')
        file_type.text = '%d' % _get_filetype(name)
        file_path = SubElement(file, 'FilePath')
        file_path.text = path # path.decode(fs_encoding)

        # for local LOCAL_CFLAGS/LOCAL_CXXFLAGS/LOCAL_CCFLAGS/LOCAL_CPPPATH/LOCAL_CPPDEFINES
        MiscControls_text = ' '
        if file_type.text == '1' and 'LOCAL_CFLAGS' in group_scons:
            MiscControls_text = MiscControls_text + group_scons['LOCAL_CFLAGS']
        elif file_type.text == '8' and 'LOCAL_CXXFLAGS' in group_scons:
            MiscControls_text = MiscControls_text + group_scons['LOCAL_CXXFLAGS']
        if 'LOCAL_CCFLAGS' in group_scons:
            MiscControls_text = MiscControls_text + group_scons['LOCAL_CCFLAGS']
        if MiscControls_text != ' ':
            FileOption     = SubElement(file,  'FileOption')
            FileArmAds     = SubElement(FileOption, 'FileArmAds')
            Cads            = SubElement(FileArmAds, 'Cads')
            VariousControls = SubElement(Cads, 'VariousControls')
            MiscControls    = SubElement(VariousControls, 'MiscControls')
            MiscControls.text = MiscControls_text
            Define          = SubElement(VariousControls, 'Define')
            if 'LOCAL_CPPDEFINES' in group_scons:
                Define.text     = ', '.join(set(group_scons['LOCAL_CPPDEFINES']))
            else:
                Define.text     = ' '
            Undefine        = SubElement(VariousControls, 'Undefine')
            Undefine.text   = ' '
            IncludePath     = SubElement(VariousControls, 'IncludePath')
            if 'LOCAL_CPPPATH' in group_scons:
                IncludePath.text = ';'.join([_make_path_relative(project_path, os.path.normpath(i)) for i in group_scons['LOCAL_CPPPATH']])
            else:
                IncludePath.text = ' '

    return group

def MDK4AddLibToGroup(ProjectFiles, group, name, filename, project_path):
    name = os.path.basename(filename)
    path = os.path.dirname (filename)

    basename = os.path.basename(path)
    path = _make_path_relative(project_path, path)
    path = os.path.join(path, name)
    files = SubElement(group, 'Files')
    file = SubElement(files, 'File')
    file_name = SubElement(file, 'FileName')
    name = os.path.basename(path)

    if name.find('.cpp') != -1:
        obj_name = name.replace('.cpp', '.o')
    elif name.find('.c') != -1:
        obj_name = name.replace('.c', '.o')
    elif name.find('.s') != -1:
        obj_name = name.replace('.s', '.o')
    elif name.find('.S') != -1:
        obj_name = name.replace('.s', '.o')
    else:
        obj_name = name

    if ProjectFiles.count(obj_name):
        name = basename + '_' + name
    ProjectFiles.append(obj_name)
    try:
        file_name.text = name.decode(fs_encoding)
    except:
        file_name.text = name
    file_type = SubElement(file, 'FileType')
    file_type.text = '%d' % _get_filetype(name)
    file_path = SubElement(file, 'FilePath')

    try:
        file_path.text = path.decode(fs_encoding)
    except:
        file_path.text = path

    return group

def MDK4AddGroupForFN(ProjectFiles, parent, name, filename, project_path):
    group = SubElement(parent, 'Group')
    group_name = SubElement(group, 'GroupName')
    group_name.text = name

    name = os.path.basename(filename)
    path = os.path.dirname (filename)

    basename = os.path.basename(path)
    path = _make_path_relative(project_path, path)
    path = os.path.join(path, name)
    files = SubElement(group, 'Files')
    file = SubElement(files, 'File')
    file_name = SubElement(file, 'FileName')
    name = os.path.basename(path)

    if name.find('.cpp') != -1:
        obj_name = name.replace('.cpp', '.o')
    elif name.find('.c') != -1:
        obj_name = name.replace('.c', '.o')
    elif name.find('.s') != -1:
        obj_name = name.replace('.s', '.o')
    elif name.find('.S') != -1:
        obj_name = name.replace('.s', '.o')
    else:
        obj_name = name

    if ProjectFiles.count(obj_name):
        name = basename + '_' + name
    ProjectFiles.append(obj_name)
    file_name.text = name.decode(fs_encoding)
    file_type = SubElement(file, 'FileType')
    file_type.text = '%d' % _get_filetype(name)
    file_path = SubElement(file, 'FilePath')

    file_path.text = path.decode(fs_encoding)

    return group

# The common part of making MDK4/5 project
def MDK45Project(tree, target, script):
    project_path = os.path.dirname(os.path.abspath(target))

    root = tree.getroot()
    out = open(target, 'w')
    out.write('<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n')

    CPPPATH = []
    CPPDEFINES = []
    LINKFLAGS = ''
    CXXFLAGS = ''
    CCFLAGS = ''
    CFLAGS = ''
    ProjectFiles = []

    # add group
    groups = tree.find('Targets/Target/Groups')
    if groups is None:
        groups = SubElement(tree.find('Targets/Target'), 'Groups')
    groups.clear() # clean old groups
    for group in script:
        group_tree = MDK4AddGroup(ProjectFiles, groups, group['name'], group['src'], project_path, group)

        # get each include path
        if 'CPPPATH' in group and group['CPPPATH']:
            if CPPPATH:
                CPPPATH += group['CPPPATH']
            else:
                CPPPATH += group['CPPPATH']

        # get each group's definitions
        if 'CPPDEFINES' in group and group['CPPDEFINES']:
            if CPPDEFINES:
                CPPDEFINES += group['CPPDEFINES']
            else:
                CPPDEFINES = group['CPPDEFINES']

        # get each group's link flags
        if 'LINKFLAGS' in group and group['LINKFLAGS']:
            if LINKFLAGS:
                LINKFLAGS += ' ' + group['LINKFLAGS']
            else:
                LINKFLAGS += group['LINKFLAGS']

        # get each group's CXXFLAGS flags
        if 'CXXFLAGS' in group and group['CXXFLAGS']:
            if CXXFLAGS:
                CXXFLAGS += ' ' + group['CXXFLAGS']
            else:
                CXXFLAGS += group['CXXFLAGS']

        # get each group's CCFLAGS flags
        if 'CCFLAGS' in group and group['CCFLAGS']:
            if CCFLAGS:
                CCFLAGS += ' ' + group['CCFLAGS']
            else:
                CCFLAGS += group['CCFLAGS']

        # get each group's CFLAGS flags
        if 'CFLAGS' in group and group['CFLAGS']:
            if CFLAGS:
                CFLAGS += ' ' + group['CFLAGS']
            else:
                CFLAGS += group['CFLAGS']

        # get each group's LIBS flags
        if 'LIBS' in group and group['LIBS']:
            for item in group['LIBS']:
                lib_path = ''
                for path_item in group['LIBPATH']:
                    full_path = os.path.join(path_item, item + '.lib')
                    if os.path.isfile(full_path): # has this library
                        lib_path = full_path
                        break

                if lib_path != '':
                    if group_tree != None:
                        MDK4AddLibToGroup(ProjectFiles, group_tree, group['name'], lib_path, project_path)
                    else:
                        group_tree = MDK4AddGroupForFN(ProjectFiles, groups, group['name'], lib_path, project_path)

    # write include path, definitions and link flags
    IncludePath = tree.find('Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls/IncludePath')
    IncludePath.text = ';'.join([_make_path_relative(project_path, os.path.normpath(i)) for i in set(CPPPATH)])

    Define = tree.find('Targets/Target/TargetOption/TargetArmAds/Cads/VariousControls/Define')
    Define.text = ', '.join(set(CPPDEFINES))

    if 'c99' in CXXFLAGS or 'c99' in CCFLAGS or 'c99' in CFLAGS:
        uC99 = tree.find('Targets/Target/TargetOption/TargetArmAds/Cads/uC99')
        uC99.text = '1'

    if 'gnu' in CXXFLAGS or 'gnu' in CCFLAGS or 'gnu' in CFLAGS:
        uGnu = tree.find('Targets/Target/TargetOption/TargetArmAds/Cads/uGnu')
        uGnu.text = '1'

    Misc = tree.find('Targets/Target/TargetOption/TargetArmAds/LDads/Misc')
    Misc.text = LINKFLAGS

    xml_indent(root)
    out.write(etree.tostring(root, encoding='utf-8').decode())
    out.close()

def MDK4Project(target, script):

    if os.path.isfile('template.uvproj') is False:
        print ('Warning: The template project file [template.uvproj] not found!')
        return

    template_tree = etree.parse('template.uvproj')

    MDK45Project(template_tree, target, script)

    # remove project.uvopt file
    project_uvopt = os.path.abspath(target).replace('uvproj', 'uvopt')
    if os.path.isfile(project_uvopt):
        os.unlink(project_uvopt)

    # copy uvopt file
    if os.path.exists('template.uvopt'):
        import shutil
        shutil.copy2('template.uvopt', '{}.uvopt'.format(os.path.splitext(target)[0]))

def MDK5Project(target, script):

    if os.path.isfile('template.uvprojx') is False:
        print ('Warning: The template project file [template.uvprojx] not found!')
        return

    template_tree = etree.parse('template.uvprojx')

    MDK45Project(template_tree, target, script)

    # remove project.uvopt file
    project_uvopt = os.path.abspath(target).replace('uvprojx', 'uvoptx')
    if os.path.isfile(project_uvopt):
        os.unlink(project_uvopt)
    # copy uvopt file
    if os.path.exists('template.uvoptx'):
        import shutil
        shutil.copy2('template.uvoptx', '{}.uvoptx'.format(os.path.splitext(target)[0]))

def MDK5_Project(target):
    from building import Projects
    MDK5Project(target, Projects)