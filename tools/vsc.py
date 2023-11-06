"""
Utils for VSCode
"""

import os
import json
from utils import _make_path_relative

def delete_repeatelist(data):
    temp_dict = set([str(item) for item in data])
    data = [eval(i) for i in temp_dict]
    return data

def GetCppDefines(groups):
    defines = []
    for group in groups:
        if 'CPPDEFINES' in group.keys():
            defines += group['CPPDEFINES']

    return defines

def GetPaths(groups):
    paths = []
    for group in groups:
        if 'CPPPATH' in group.keys():
            for path in group['CPPPATH']:
                paths.append(path.replace('\\','/'))

    return paths

def GenerateCFiles(defines, paths):
    """
    Generate c_cpp_properties files
    """
    if not os.path.exists('.vscode'):
        os.mkdir('.vscode')

    vsc_file = open('.vscode/c_cpp_properties.json', 'w')
    if vsc_file:
        config_obj = {}
        config_obj['name'] = 'Win32'
        config_obj['defines'] = defines
        config_obj['intelliSenseMode'] = 'clang-x64'
        config_obj['cStandard'] = "c99"
        config_obj['cppStandard'] = "c++11"

        # format "a/b," to a/b. remove first quotation mark("),and remove end (",)
        includePath = []
        for i in paths:
            if i[0] == '\"' and i[len(i) - 2:len(i)] == '\",':
                includePath.append(_make_path_relative(os.getcwd(), i[1:len(i) - 2]))
            else:
                includePath.append(_make_path_relative(os.getcwd(), i))
        config_obj['includePath'] = includePath

        json_obj = {}
        json_obj['configurations'] = [config_obj]

        vsc_file.write(json.dumps(json_obj, ensure_ascii=False, indent=4))
        vsc_file.close()
    return

def GenerateVSCode(groups):
    print('Update setting files for VSCode...')
    GenerateCFiles(GetCppDefines(groups), GetPaths(groups))
    print('Done!')

    return
