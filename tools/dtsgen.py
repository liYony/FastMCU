import os
import subprocess
import sys

RESET = '\033[0m'
GREEN = '\033[92m'
YELLOW = '\033[93m'
RED = '\033[91m'

def print_dbg(message):
    print(message)

def print_inf(message):
    print(f"{GREEN}{message}{RESET}")

def print_wrn(message):
    print(f"{YELLOW}{message}{RESET}")

def print_err(message):
    print(f"{RED}{message}{RESET}")

bsp_root = None
sys_root = None
isystem_dirs = []
include_dts_dirs = []
bindings_dirs = []

def DtsGenDefine(env, dts):
    sys_root = env['FM_ROOT']
    bsp_root = env['BSP_ROOT']

    # sys path
    isystem_dirs.append(os.path.join(sys_root, 'include'))
    isystem_dirs.append(os.path.join(sys_root, 'dts'))
    isystem_dirs.append(os.path.join(sys_root, 'dts', 'common'))
    isystem_dirs.append(os.path.join(sys_root, 'dts', 'arm'))

    # bsp path
    isystem_dirs.append(os.path.join(os.path.dirname(bsp_root), 'dts'))
    isystem_params = " ".join(["-isystem" + dir for dir in isystem_dirs])
    
    if dts is None:
        dts = os.path.basename(bsp_root) + ".dts"
    if not os.path.exists(os.path.join(bsp_root, dts)):
        print_err("Error: %s not found" % os.path.join(bsp_root, dts))
        sys.exit(1)
    include_dts_dirs.append(os.path.join(bsp_root, dts))
    print_inf("Found BOARD.dts: %s" % os.path.join(bsp_root, dts))
    for root, dirs, files in os.walk(bsp_root):
        for file in files:
            if file.endswith(".overlay"):
                include_dts_dirs.append(os.path.join(root, file))
                print_inf("Found devicetree overlay: %s" % os.path.join(root, file))
    include_dts_params = " ".join(["-include" + dir for dir in include_dts_dirs])

    pre_d_file_path = os.path.join(bsp_root, dts + ".pre.d")
    pre_tmp_file_path = os.path.join(bsp_root, dts + ".pre.tmp")
    empty_file_path = os.path.join(sys_root, "src", "empty_file.c")
    gcc_path = env['CC']

    gcc_cmd = (
        f"{gcc_path} -x assembler-with-cpp -nostdinc {isystem_params} {include_dts_params} "
        f"-D__DTS__ -P -E -MD -MF {pre_d_file_path} -o {pre_tmp_file_path} {empty_file_path}"
    )
    print_dbg(gcc_cmd)
    result = subprocess.run(gcc_cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print_err(f"GCC command failed with exit code {result.returncode}")
        print_err(f"STDOUT: {result.stdout}")
        print_err(f"STDERR: {result.stderr}")
        exit(result.returncode)

    vendor_prefixes_path = os.path.join(sys_root, "dts", "bindings", "vendor-prefixes.txt")
    if not os.path.exists(vendor_prefixes_path):
        print_err("Error: %s not found" % vendor_prefixes_path)
        sys.exit(1)
    print_inf("Found vendor-prefixes.txt: %s" % vendor_prefixes_path)
    sys_bindings_path = os.path.join(sys_root, "dts", "bindings")
    if not os.path.exists(vendor_prefixes_path):
        print_err("Error: %s not found" % vendor_prefixes_path)
        sys.exit(1)
    bindings_dirs.append(sys_bindings_path)
    bindings_dirs_params = " ".join(["--bindings-dirs " + dir for dir in bindings_dirs])
    gen_defines_script = os.path.join(sys_root, "tools", "dts", "gen_defines.py")
    generated_header_path = os.path.join(sys_root, "include", "generated", "device_tree_unfixed.h")
    device_header_path = os.path.join(sys_root, "include", "generated", "device_extern.h")
    # Create directories if they don't exist
    if not os.path.exists(os.path.dirname(generated_header_path)):
        os.makedirs(os.path.dirname(generated_header_path))
    if not os.path.exists(os.path.dirname(device_header_path)):
        os.makedirs(os.path.dirname(device_header_path))
    dts_output_path = os.path.join(bsp_root, "fastmcu.dts")
    edt_pickle_path = os.path.join(bsp_root, "edt.pickle")
    extra_dtc_flags_raw = "'-Wno-simple_bus_reg'"
    python_path = sys.executable

    dtsgen_cmd = (
        f"{python_path} {gen_defines_script} --dts {pre_tmp_file_path} "
        f"--dtc-flags {extra_dtc_flags_raw} "
        f"{bindings_dirs_params} --header-out {generated_header_path} "
        f"--device-header-out {device_header_path} --dts-out {dts_output_path} "
        f"--edt-pickle-out {edt_pickle_path} --vendor-prefixes {vendor_prefixes_path}"
    )
    print_dbg(dtsgen_cmd)
    result = subprocess.run(dtsgen_cmd, capture_output=True, text=True)
    if result.returncode != 0:
        print_err(f"DTSGEN command failed with exit code {result.returncode}")
        print_err(f"STDOUT: {result.stdout}")
        print_err(f"STDERR: {result.stderr}")
        exit(result.returncode)
