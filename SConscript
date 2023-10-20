import os
from building import *

objs = []

objs = objs + SConscript(os.path.join('drivers', 'SConscript'))
objs = objs + SConscript(os.path.join('examples', 'SConscript'))
objs = objs + SConscript(os.path.join('utils', 'SConscript'))
objs = objs + SConscript(os.path.join('src', 'SConscript'))

Return('objs')
