#ifndef __DRV_COMMON_H__
#define __DRV_COMMON_H__

#include <fmcu.h>
#ifdef FM_USING_DEVICE
#include <fmdevice.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

void _Error_Handler(char *s, int num);

#ifndef Error_Handler
#define Error_Handler() _Error_Handler(__FILE__, __LINE__)
#endif

#ifdef __cplusplus
}
#endif

#endif
