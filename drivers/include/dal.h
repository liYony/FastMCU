#ifndef __DAL_H__
#define __DAL_H__

#include <qkconfig.h>
#include <qk_section.h>
#include <qk_list.h>
#define LOG_LVL LVL_DEBUG
#include <qk_log.h>
#include <qk_type.h>

#define dal_weak __WEAKDEF
#define dal_printf qk_kprintf

#ifdef DAL_SERIAL_ENABLE
#include "dal_serial.h"
#endif // DAL_SERIAL_ENABLE
#ifdef DAL_IT_ENABLE
#include "dal_it.h"
#endif // DAL_IT_ENABLE

#endif // !__DAL_H__
