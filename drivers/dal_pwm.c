#include <dal_pwm.h>

dal_weak int mcu_pwm_init(dal_pwm_number_t pwm, uint32_t period)
{
    return -1;
}

dal_weak int mcu_pwm_enable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return -1;
}

dal_weak int mcu_pwm_disable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return -1;
}

dal_weak int mcu_pwm_set_period(dal_pwm_number_t pwm, uint32_t period)
{
    return -1;
}

dal_weak uint32_t mcu_pwm_get_period(dal_pwm_number_t pwm)
{
    return (uint32_t)-1;
}

dal_weak int mcu_pwm_set_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t pulse)
{
    return -1;
}

dal_weak uint32_t mcu_pwm_get_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return (uint32_t)-1;
}

int dal_pwm_init(dal_pwm_number_t pwm, uint32_t period)
{
    return mcu_pwm_init(pwm, period);
}

int dal_pwm_enable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return mcu_pwm_enable(pwm, ch);
}

int dal_pwm_disable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return mcu_pwm_disable(pwm, ch);
}

int dal_pwm_set_period(dal_pwm_number_t pwm, uint32_t period)
{
    return mcu_pwm_set_period(pwm, period);
}

uint32_t dal_pwm_get_period(dal_pwm_number_t pwm)
{
    return mcu_pwm_get_period(pwm);
}

int dal_pwm_set_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t pulse)
{
    return mcu_pwm_set_pulse(pwm, ch, pulse);
}

uint32_t dal_pwm_get_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return mcu_pwm_get_pulse(pwm, ch);
}
