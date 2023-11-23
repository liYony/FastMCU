#ifndef __SFUD_PORT_H__
#define __SFUD_PORT_H__

#include <fm_log.h>
#include <dal.h>

typedef struct {
    dal_spi_number_t spi;
    dal_gpio_instance_t cs;
} spi_user_data, *spi_user_data_t;


#endif /* __SFUD_PORT_H__ */
