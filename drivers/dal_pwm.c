#include <dal_pwm.h>

dal_weak int mcu_pwm_init(dal_pwm_number_t pwm, uint32_t period, uint32_t pulse)
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

int dal_pwm_init(dal_pwm_number_t pwm, uint32_t period, uint32_t pulse)
{
    return mcu_pwm_init(pwm, period, pulse);
}

int dal_pwm_enable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return mcu_pwm_enable(pwm, ch);
}

int dal_pwm_disable(dal_pwm_number_t pwm, dal_pwm_channel_t ch)
{
    return mcu_pwm_disable(pwm, ch);
}
