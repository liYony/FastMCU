#ifndef __FASTMCU_H__
#define __FASTMCU_H__

#include <fmconfig.h>
#include <fmdef.h>
#include <fmservice.h>

/*
 * device (I/O) system interface
 */
fm_device_t fm_device_find(const char *name);

fm_err_t fm_device_register(fm_device_t dev,
                            const char *name,
                            fm_uint16_t flags);
fm_err_t fm_device_unregister(fm_device_t dev);

#ifdef FM_USING_HEAP
fm_device_t fm_device_create(int type, int attach_size);
void fm_device_destroy(fm_device_t device);
#endif

fm_err_t
fm_device_set_rx_indicate(fm_device_t dev,
                          fm_err_t (*rx_ind)(fm_device_t dev, fm_size_t size));
fm_err_t
fm_device_set_tx_complete(fm_device_t dev,
                          fm_err_t (*tx_done)(fm_device_t dev, void *buffer));

fm_err_t  fm_device_init (fm_device_t dev);
fm_err_t  fm_device_open (fm_device_t dev, fm_uint16_t oflag);
fm_err_t  fm_device_close(fm_device_t dev);
fm_ssize_t fm_device_read(fm_device_t dev,
                          fm_off_t    pos,
                          void       *buffer,
                          fm_size_t   size);
fm_ssize_t fm_device_write(fm_device_t dev,
                          fm_off_t    pos,
                          const void *buffer,
                          fm_size_t   size);
fm_err_t  fm_device_control(fm_device_t dev, int cmd, void *arg);

#endif /* __FASTMCU_H__ */
