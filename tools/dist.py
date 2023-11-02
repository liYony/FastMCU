import os
import shutil
from shutil import ignore_patterns

FASTMCU_DIR_NAME =  'fmcu'

def do_copy_file(src, dst):
    # check source file
    if not os.path.exists(src):
        return

    path = os.path.dirname(dst)
    # mkdir if path not exist
    if not os.path.exists(path):
        os.makedirs(path)

    shutil.copy2(src, dst)

def do_copy_folder(src_dir, dst_dir, ignore=None):
    import shutil
    # check source directory
    if not os.path.exists(src_dir):
        return

    try:
        if os.path.exists(dst_dir):
            shutil.rmtree(dst_dir)
    except:
        print('Deletes folder: %s failed.' % dst_dir)
        return

    shutil.copytree(src_dir, dst_dir, ignore = ignore)

def bsp_copy_files(bsp_root, dist_dir):
    # copy BSP files
    do_copy_folder(os.path.join(bsp_root), dist_dir,
        ignore_patterns('build', 'dist', '*.pyc', '*.old', '*.map', '*.bin', 
                        '.sconsign.dblite', '*.elf', '*.axf', 'cconfig.h', 
                        '*.uvguix.*', 'JLink*', '*.scvd'))

def bsp_update_sconstruct(dist_dir):
    with open(os.path.join(dist_dir, 'SConstruct'), 'r') as f:
        data = f.readlines()
    with open(os.path.join(dist_dir, 'SConstruct'), 'w') as f:
        for line in data:
            if line.find('FM_ROOT') != -1 and line.find('=') != -1:
                f.write('FM_ROOT = \'%s/\'\n\n' % FASTMCU_DIR_NAME)
            else:
                f.write(line)

def bsp_update_sconscript(dist_dir):
    with open(os.path.join(dist_dir, 'SConscript'), 'r') as f:
        data = f.readlines()
    with open(os.path.join(dist_dir, 'SConscript'), 'w') as f:
        for line in data:
            if line.find('FM_ROOT') != -1 and line.find('SConscript') != -1:
                continue
            else:
                f.write(line)

def MakeProjectDist(fm_root, bsp_root):
    print('make distribution....')
    dist_dir = os.path.join(bsp_root, 'dist', os.path.basename(bsp_root))

    fmcu_dir_path = os.path.join(dist_dir, FASTMCU_DIR_NAME)
    print('=> %s' % os.path.basename(bsp_root))
    bsp_copy_files(bsp_root, dist_dir)
    print('=> src')
    do_copy_folder(os.path.join(fm_root, 'src'), os.path.join(fmcu_dir_path, 'src'))
    print('=> include')
    do_copy_folder(os.path.join(fm_root, 'include'), os.path.join(fmcu_dir_path, 'include'))
    print('=> components')
    do_copy_folder(os.path.join(fm_root, 'components'), os.path.join(fmcu_dir_path, 'components'))
    print('=> drivers')
    do_copy_folder(os.path.join(fm_root, 'drivers'), os.path.join(fmcu_dir_path, 'drivers'))
    print('=> tools')
    do_copy_folder(os.path.join(fm_root, 'tools'), os.path.join(fmcu_dir_path, 'tools'))

    do_copy_file(os.path.join(fm_root, 'Kconfig'), os.path.join(fmcu_dir_path, 'Kconfig'))
    do_copy_file(os.path.join(fm_root, 'SConscript'), os.path.join(fmcu_dir_path, 'SConscript'))

    bsp_update_sconstruct(dist_dir)
    bsp_update_sconscript(dist_dir)
    