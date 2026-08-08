/**
 * @file    liiushuideng.c
 * @brief   LIUSHUIDENG 驱动实现文件。
 */
#include "liushuideng.h"

static liushuideng_config liushuideng_cfg = {0U};

void liushuideng(uint8_t led_count)
{
    for(led_count=0U;led_count<4U;led_count++)
    {
        led_on(led_count);
        HAL_Delay(250U);
        led_off(led_count);
        HAL_Delay(250U);
    }

}