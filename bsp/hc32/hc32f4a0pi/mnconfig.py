#!/usr/bin/env python
# -*- coding: UTF-8 -*-

import os, sys
from kconfiglib import Kconfig
from menuconfig import menuconfig
import re

def _qk_config(filename):
    try:
        config = open(filename, 'r')
    except:
        print('open config:%s failed' % filename)
        return

    rtconfig = open('qkconfig.h', 'w')
    rtconfig.write('#ifndef __QKIT_CONFIG_H__\n')
    rtconfig.write('#define __QKIT_CONFIG_H__\n\n')

    empty_line = 1

    for line in config:
        line = line.lstrip(' ').replace('\n', '').replace('\r', '')

        if len(line) == 0:
            continue

        if line[0] == '#':
            if len(line) == 1:
                if empty_line:
                    continue

                rtconfig.write('\n')
                empty_line = 1
                continue

            if line.startswith('# CONFIG_'):
                line = ' ' + line[9:]
            else:
                line = line[1:]
                if "end" in line:
                    rtconfig.write('\n/*%s */\n' % line)
                else:
                    rtconfig.write('/*%s */\n' % line)

            empty_line = 0
        else:
            empty_line = 0
            setting = line.split('=')
            if len(setting) >= 2:
                if setting[0].startswith('CONFIG_'):
                    setting[0] = setting[0][7:]

                # # remove CONFIG_PKG_XX_PATH or CONFIG_PKG_XX_VER
                # if is_pkg_special_config(setting[0]):
                #     continue

                if setting[1] == 'y':
                    rtconfig.write('#define %s\n' % setting[0])
                else:
                    rtconfig.write('#define %s %s\n' % (setting[0], re.findall(r"^.*?=(.*)$",line)[0]))

    rtconfig.write('\n')
    rtconfig.write('#endif /* __QKIT_CONFIG_H__ */\n')
    rtconfig.close()

def get_qkit_root():
    return "../../.."

def mconf_set_env():
    """
    Set Kconfig Env
    """
    os.environ["MENUCONFIG_STYLE"] = "default selection=fg:white,bg:blue"
    os.environ["KCONFIG_CONFIG"] = os.path.join(".config")
    os.environ["KCONFIG_CONFIG_HEADER"] = "# Generated by QKit Configuration\n"
    os.environ["KCONFIG_AUTOHEADER"] = os.path.join("qkconfig.h")
    os.environ["CONFIG_"] = "CONFIG_"
    os.environ['QK_ROOT_DIR'] = get_qkit_root()

def qk_config():
    mconf_set_env()
    kconf = Kconfig(filename="./Kconfig")
    menuconfig(kconf)
    kconf.write_autoconf()
    _qk_config(".config")

if __name__ == "__main__":
    qk_config()