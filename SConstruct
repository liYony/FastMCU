import sys

sys.path.append('./tools')

try:
    from building import *
except:
    print('Cannot found RT-Thread root directory, please check RTT_ROOT')
    print()
    exit(-1)

from building import *

objs = SConscript('SConscript')

MDK5_Project('project.uvprojx')

exit(0)
