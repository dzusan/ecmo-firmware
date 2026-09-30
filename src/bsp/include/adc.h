#ifndef BSP_ADC_H
#define BSP_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

enum adc_channel
{
    ADC_CH_SEN_PRES_1,
    ADC_CH_SEN_PRES_2,

    ADC_CH_SEN_V_BAT,
    ADC_CH_SEN_V_SUP,

    ADC_CH_TOUCH_XL,
    ADC_CH_TOUCH_YD,
    ADC_CH_TOUCH_XR,
    ADC_CH_TOUCH_YU,

    ADC_CH_SEN_U_I,
    ADC_CH_SEN_V_I,
    ADC_CH_SEN_V_V,
    ADC_CH_SEN_W_V,
    ADC_CH_SEN_W_I,
    ADC_CH_SEN_U_V,

    ADC_CH_TOTAL,
};

int adc_init(void);
int adc_start(enum adc_channel channel);
int adc_get(enum adc_channel id);
void adc_irq(void); /* Should be defined externally */

#ifdef __cplusplus
}
#endif

#endif /* BSP_ADC_H */
