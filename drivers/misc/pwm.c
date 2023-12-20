/*
 * Copyright (c) 2006-2023, RT-Thread Development Team
 * Copyright (c) 2023, liYony
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-07     aozima       the first version
 * 2022-05-14     Stanley Lwin add pwm function
 * 2022-07-25     liYony       fix complementary outputs and add usage information in finsh
 * 2022-08-31     liYony       Add complementary output section to framework for management
 * 2022-09-24     qiyu         Add dead-time and phase configuration
 * 2023-12-19     liYony       Adaptive FastMCU
 */

#include <fmdevice.h>

static fm_err_t _pwm_control(fm_device_t dev, int cmd, void *args)
{
    fm_err_t result = FM_EOK;
    struct fm_device_pwm *pwm = (struct fm_device_pwm *)dev;
    struct fm_pwm_configuration *configuration = (struct fm_pwm_configuration *)args;

    switch (cmd)
    {
        case PWMN_CMD_ENABLE:
            configuration->complementary = FM_TRUE;
            break;
        case PWMN_CMD_DISABLE:
            configuration->complementary = FM_FALSE;
            break;
        default:
            if(pwm->ops->control)
                result = pwm->ops->control(pwm, cmd, args);
            break;
    }

    return result;
}

/*
pos: channel
void *buffer: fm_uint32_t pulse[size]
size : number of pulse, only set to sizeof(fm_uint32_t).
*/
static fm_ssize_t _pwm_read(fm_device_t dev, fm_off_t pos, void *buffer, fm_size_t size)
{
    fm_err_t result = FM_EOK;
    struct fm_device_pwm *pwm = (struct fm_device_pwm *)dev;
    fm_uint32_t *pulse = (fm_uint32_t *)buffer;
    struct fm_pwm_configuration configuration = {0};

    configuration.channel = (pos > 0) ? (pos) : (-pos);

    if (pwm->ops->control)
    {
        result = pwm->ops->control(pwm, PWM_CMD_GET,  &configuration);
        if (result != FM_EOK)
        {
            return 0;
        }

        *pulse = configuration.pulse;
    }

    return size;
}

/*
pos: channel
void *buffer: fm_uint32_t pulse[size]
size : number of pulse, only set to sizeof(fm_uint32_t).
*/
static fm_ssize_t _pwm_write(fm_device_t dev, fm_off_t pos, const void *buffer, fm_size_t size)
{
    fm_err_t result = FM_EOK;
    struct fm_device_pwm *pwm = (struct fm_device_pwm *)dev;
    fm_uint32_t *pulse = (fm_uint32_t *)buffer;
    struct fm_pwm_configuration configuration = {0};

    configuration.channel = (pos > 0) ? (pos) : (-pos);

    if (pwm->ops->control)
    {
        result = pwm->ops->control(pwm, PWM_CMD_GET, &configuration);
        if (result != FM_EOK)
        {
            return 0;
        }

        configuration.pulse = *pulse;

        result = pwm->ops->control(pwm, PWM_CMD_SET, &configuration);
        if (result != FM_EOK)
        {
            return 0;
        }
    }

    return size;
}

#ifdef FM_USING_DEVICE_OPS
static const struct fm_device_ops pwm_device_ops =
{
    FM_NULL,
    FM_NULL,
    FM_NULL,
    _pwm_read,
    _pwm_write,
    _pwm_control
};
#endif /* FM_USING_DEVICE_OPS */

fm_err_t fm_device_pwm_register(struct fm_device_pwm *device, const char *name, const struct fm_pwm_ops *ops, const void *user_data)
{
    fm_err_t result = FM_EOK;

    fm_memset(device, 0, sizeof(struct fm_device_pwm));

#ifdef FM_USING_DEVICE_OPS
    device->parent.ops = &pwm_device_ops;
#else
    device->parent.init = FM_NULL;
    device->parent.open = FM_NULL;
    device->parent.close = FM_NULL;
    device->parent.read  = _pwm_read;
    device->parent.write = _pwm_write;
    device->parent.control = _pwm_control;
#endif /* FM_USING_DEVICE_OPS */

    device->parent.type         = FM_Device_Class_PWM;
    device->ops                 = ops;
    device->parent.user_data    = (void *)user_data;

    result = fm_device_register(&device->parent, name, FM_DEVICE_FLAG_RDWR);

    return result;
}

fm_err_t fm_pwm_enable(struct fm_device_pwm *device, int channel)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    if (!device)
    {
        return -FM_EIO;
    }

    /* Make it is positive num forever */
    configuration.channel = (channel > 0) ? (channel) : (-channel);

    /* If channel is a positive number (0 ~ n), it means using normal output pin.
     * If channel is a negative number (0 ~ -n), it means using complementary output pin. */
    if(channel > 0)
    {
        result = fm_device_control(&device->parent, PWMN_CMD_DISABLE, &configuration);
    }
    else
    {
        result = fm_device_control(&device->parent, PWMN_CMD_ENABLE, &configuration);
    }

    result = fm_device_control(&device->parent, PWM_CMD_ENABLE, &configuration);

    return result;
}

fm_err_t fm_pwm_disable(struct fm_device_pwm *device, int channel)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    if (!device)
    {
        return -FM_EIO;
    }

    /* Make it is positive num forever */
    configuration.channel = (channel > 0) ? (channel) : (-channel);

    /* If channel is a positive number (0 ~ n), it means using normal output pin.
     * If channel is a negative number (0 ~ -n), it means using complementary output pin. */
    if(channel > 0)
    {
        result = fm_device_control(&device->parent, PWMN_CMD_DISABLE, &configuration);
    }
    else
    {
        result = fm_device_control(&device->parent, PWMN_CMD_ENABLE, &configuration);
    }

    result = fm_device_control(&device->parent, PWM_CMD_DISABLE, &configuration);

    return result;
}

fm_err_t fm_pwm_set(struct fm_device_pwm *device, int channel, fm_uint32_t period, fm_uint32_t pulse)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    if (!device)
    {
        return -FM_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.period = period;
    configuration.pulse = pulse;
    result = fm_device_control(&device->parent, PWM_CMD_SET, &configuration);

    return result;
}

fm_err_t fm_pwm_set_period(struct fm_device_pwm *device, int channel, fm_uint32_t period)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    if (!device)
    {
        return -FM_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.period = period;
    result = fm_device_control(&device->parent, PWM_CMD_SET_PERIOD, &configuration);

    return result;
}

fm_err_t fm_pwm_set_pulse(struct fm_device_pwm *device, int channel, fm_uint32_t pulse)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    if (!device)
    {
        return -FM_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.pulse = pulse;
    result = fm_device_control(&device->parent, PWM_CMD_SET_PULSE, &configuration);

    return result;
}

fm_err_t fm_pwm_set_dead_time(struct fm_device_pwm *device, int channel, fm_uint32_t dead_time)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    if (!device)
    {
        return -FM_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.dead_time = dead_time;
    result = fm_device_control(&device->parent, PWM_CMD_SET_DEAD_TIME, &configuration);

    return result;
}


fm_err_t fm_pwm_set_phase(struct fm_device_pwm *device, int channel, fm_uint32_t phase)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    if (!device)
    {
        return -FM_EIO;
    }

    configuration.channel = (channel > 0) ? (channel) : (-channel);
    configuration.phase = phase;
    result = fm_device_control(&device->parent, PWM_CMD_SET_PHASE, &configuration);

    return result;
}

fm_err_t fm_pwm_get_config(struct fm_device_pwm *device, int channel, struct fm_pwm_configuration *cfg)
{
    fm_err_t result = FM_EOK;

    if (!device)
    {
        return -FM_EIO;
    }

    cfg->channel = channel;
    result = fm_device_control(&device->parent, PWM_CMD_GET, cfg);

    return result;
}

fm_err_t fm_pwm_get(struct fm_device_pwm *device, int channel, fm_uint32_t *period, fm_uint32_t *pulse)
{
    fm_err_t result = FM_EOK;
    struct fm_pwm_configuration configuration = {0};

    result = fm_pwm_get_config(device, channel, &configuration);
    *period = configuration.period;
    *pulse = configuration.pulse;

    return result;
}
