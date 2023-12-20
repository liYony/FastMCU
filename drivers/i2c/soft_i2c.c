/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2023-07-30     sp-cai        first version
 * 2023-12-20     liYony        Adaptive FastMCU
 */

#include <fmdevice.h>

#ifdef FM_USING_SOFT_I2C
#if !defined(FM_USING_SOFT_I2C1) && !defined(FM_USING_SOFT_I2C2) &&\
    !defined(FM_USING_SOFT_I2C3) && !defined(FM_USING_SOFT_I2C4) &&\
    !defined(FM_USING_SOFT_I2C5) && !defined(FM_USING_SOFT_I2C6) &&\
    !defined(FM_USING_SOFT_I2C7) && !defined(FM_USING_SOFT_I2C8)
    #error "Please define at least one FM_USING_SOFT_I2Cx"
    /*
    This driver can be disabled at:
    menuconfig -> RT-Thread Components -> Device Drivers -> Using I2C device drivers
    */
#endif

#define DBG_ENABLE
#define DBG_TAG                         "I2C_S"
#ifdef FM_I2C_BITOPS_DEBUG
    #define DBG_LEVEL                   DBG_LOG
#endif
#include <fmdbg.h>

/* i2c config class */
struct soft_i2c_config
{
    fm_base_t       scl_pin;
    fm_base_t       sda_pin;
    const char      *bus_name;
    fm_uint16_t     timing_delay;   /* scl and sda line delay */
    fm_uint16_t     timing_timeout; /* in tick */
};

/* i2c dirver class */
struct fm_soft_i2c
{
    struct fm_i2c_bus_device    i2c_bus;
    struct fm_i2c_bit_ops       ops;
};

struct soft_i2c_config i2c_cfg[] =
{
    #ifdef FM_USING_SOFT_I2C1
    {
        .scl_pin        = FM_SOFT_I2C1_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C1_SDA_PIN,
        .bus_name       = FM_SOFT_I2C1_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C1_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C1_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C1
    #ifdef FM_USING_SOFT_I2C2
    {
        .scl_pin        = FM_SOFT_I2C2_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C2_SDA_PIN,
        .bus_name       = FM_SOFT_I2C2_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C2_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C2_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C2
    #ifdef FM_USING_SOFT_I2C3
    {
        .scl_pin        = FM_SOFT_I2C3_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C3_SDA_PIN,
        .bus_name       = FM_SOFT_I2C3_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C3_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C3_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C3
    #ifdef FM_USING_SOFT_I2C4
    {
        .scl_pin        = FM_SOFT_I2C4_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C4_SDA_PIN,
        .bus_name       = FM_SOFT_I2C4_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C4_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C4_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C4
    #ifdef FM_USING_SOFT_I2C5
    {
        .scl_pin        = FM_SOFT_I2C5_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C5_SDA_PIN,
        .bus_name       = FM_SOFT_I2C5_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C5_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C5_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C5
    #ifdef FM_USING_SOFT_I2C6
    {
        .scl_pin        = FM_SOFT_I2C6_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C6_SDA_PIN,
        .bus_name       = FM_SOFT_I2C6_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C6_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C6_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C6
    #ifdef FM_USING_SOFT_I2C7
    {
        .scl_pin        = FM_SOFT_I2C7_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C7_SDA_PIN,
        .bus_name       = FM_SOFT_I2C7_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C7_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C7_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C7
    #ifdef FM_USING_SOFT_I2C8
    {
        .scl_pin        = FM_SOFT_I2C8_SCL_PIN,
        .sda_pin        = FM_SOFT_I2C8_SDA_PIN,
        .bus_name       = FM_SOFT_I2C8_BUS_NAME,
        .timing_delay   = FM_SOFT_I2C8_TIMING_DELAY,
        .timing_timeout = FM_SOFT_I2C8_TIMING_TIMEOUT,
    },
    #endif  //FM_USING_SOFT_I2C8
};


static struct fm_soft_i2c i2c_bus_obj[sizeof(i2c_cfg) / sizeof(i2c_cfg[0])] =
{ 0 };

/**
* This function initializes the i2c pin.
* @param i2c config class.
*/
static void pin_init(const struct soft_i2c_config *cfg)
{
    fm_pin_mode(cfg->scl_pin, PIN_MODE_OUTPUT_OD);
    fm_pin_mode(cfg->sda_pin, PIN_MODE_OUTPUT_OD);
    fm_pin_write(cfg->scl_pin, PIN_HIGH);
    fm_pin_write(cfg->sda_pin, PIN_HIGH);
}


/**
* This function sets the sda pin.
* @param i2c config class.
* @param The sda pin state.
*/
static void set_sda(void *cfg, fm_int32_t value)
{
    fm_pin_write(((const struct soft_i2c_config*)cfg)->sda_pin, value);
}

/**
* This function sets the scl pin.
* @param i2c config class.
* @param The sda pin state.
*/
static void set_scl(void *cfg, fm_int32_t value)
{
    fm_pin_write(((const struct soft_i2c_config*)cfg)->scl_pin, value);
}

/**
* This function gets the sda pin state.
* @param i2c config class.
*/
static fm_int32_t get_sda(void *cfg)
{
    return fm_pin_read(((const struct soft_i2c_config*)cfg)->sda_pin);
}

/**
* This function gets the scl pin state.
* @param i2c config class.
*/
static fm_int32_t get_scl(void *cfg)
{
    return fm_pin_read(((const struct soft_i2c_config*)cfg)->scl_pin);
}


static const struct fm_i2c_bit_ops soft_i2c_ops =
{
    .set_sda = set_sda,
    .set_scl = set_scl,
    .get_sda = get_sda,
    .get_scl = get_scl,
    .udelay = fm_delay_us,
};

/**
* if i2c is locked, this function will unlock it
*
* @param i2c config class.
*
* @return FM_EOK indicates successful unlock.
*/
static fm_err_t i2c_bus_unlock(const struct soft_i2c_config *cfg)
{
    fm_ubase_t i = 0;

    if(PIN_LOW == fm_pin_read(cfg->sda_pin))
    {
        while(i++ < 9)
        {
            fm_pin_write(cfg->scl_pin, PIN_HIGH);
            fm_delay_us(cfg->timing_delay);
            fm_pin_write(cfg->scl_pin, PIN_LOW);
            fm_delay_us(cfg->timing_delay);
        }
    }
    if(PIN_LOW == fm_pin_read(cfg->sda_pin))
    {
        return -FM_ERROR;
    }

    return FM_EOK;
}

/* I2C initialization function */
int fm_soft_i2c_init(void)
{
    int err = FM_EOK;
    struct fm_soft_i2c *obj;
    int i;

    for(i = 0; i < sizeof(i2c_bus_obj) / sizeof(i2c_bus_obj[0]); i++)
    {
        struct soft_i2c_config *cfg = &i2c_cfg[i];

        pin_init(cfg);

        obj = &i2c_bus_obj[i];
        obj->ops = soft_i2c_ops;
        obj->ops.data = cfg;
        obj->i2c_bus.priv = &obj->ops;
        obj->ops.delay_us = cfg->timing_delay;
        obj->ops.timeout = cfg->timing_timeout;
        if(fm_i2c_bit_add_bus(&obj->i2c_bus, cfg->bus_name) == FM_EOK)
        {
            i2c_bus_unlock(cfg);
            LOG_D("Software simulation %s init done"
                  ", SCL pin: 0x%02X, SDA pin: 0x%02X"
                  , cfg->bus_name
                  , cfg->scl_pin
                  , cfg->sda_pin
                 );
        }
        else
        {
            err++;
            LOG_E("Software simulation %s init fail"
                  ", SCL pin: 0x%02X, SDA pin: 0x%02X"
                  , cfg->bus_name
                  , cfg->scl_pin
                  , cfg->sda_pin
                 );
        }
    }

    return err;
}
INIT_PREV_EXPORT(fm_soft_i2c_init);

#endif // FM_USING_SOFT_I2C
