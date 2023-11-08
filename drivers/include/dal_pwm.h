#ifndef __DAL_PWM_H__
#define __DAL_PWM_H__

#include <dal_type.h>

typedef enum
{
    DAL_PWM_0,
    DAL_PWM_1,
    DAL_PWM_2,
    DAL_PWM_3,
    DAL_PWM_4,
    DAL_PWM_5,
    DAL_PWM_6,
    DAL_PWM_7,
    DAL_PWM_8,
    DAL_PWM_9,
    DAL_PWM_10,
    DAL_PWM_11,
    DAL_PWM_12,
    DAL_PWM_13,
    DAL_PWM_14,
    DAL_PWM_15,
    DAL_PWM_16,
    DAL_PWM_17,
    DAL_PWM_NUMBER
} dal_pwm_number_t;

typedef enum
{
    DAL_PWM_CH1,
    DAL_PWM_CH2,
    DAL_PWM_CH3,
    DAL_PWM_CH4,
    DAL_PWM_CHNUMBER
} dal_pwm_channel_t;

int mcu_pwm_init(dal_pwm_number_t pwm, uint32_t period);
int mcu_pwm_enable(dal_pwm_number_t pwm, dal_pwm_channel_t ch);
int mcu_pwm_disable(dal_pwm_number_t pwm, dal_pwm_channel_t ch);
int mcu_pwm_set_period(dal_pwm_number_t pwm, uint32_t period);
uint32_t mcu_pwm_get_period(dal_pwm_number_t pwm);
int mcu_pwm_set_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t pulse);
uint32_t mcu_pwm_get_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch);
int dal_pwm_init(dal_pwm_number_t pwm, uint32_t period);
int dal_pwm_enable(dal_pwm_number_t pwm, dal_pwm_channel_t ch);
int dal_pwm_disable(dal_pwm_number_t pwm, dal_pwm_channel_t ch);
int dal_pwm_set_period(dal_pwm_number_t pwm, uint32_t period);
uint32_t dal_pwm_get_period(dal_pwm_number_t pwm);
int dal_pwm_set_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch, uint32_t pulse);
uint32_t dal_pwm_get_pulse(dal_pwm_number_t pwm, dal_pwm_channel_t ch);

#endif /* __DAL_PWM_H__ */
