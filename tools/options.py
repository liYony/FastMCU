from SCons.Script import AddOption

def AddOptions():
    ''' ===== Add generic options to SCons ===== '''

    AddOption('--dist',
                      dest = 'dist',
                      action = 'store_true',
                      default = False,
                      help = 'make distribution')
    
    AddOption('--keil',
                      dest = 'keil',
                      action = 'store_true',
                      default = False,
                      help = 'make keil project')
    
    AddOption('--menuconfig',
                      dest = 'menuconfig',
                      action = 'store_true',
                      default = False,
                      help = 'make kconfig')

    