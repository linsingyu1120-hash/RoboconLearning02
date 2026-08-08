/**
 * @file    liushuideng.h
 * @brief   LED 驱动头文件。
 *          头文件放声明和宏，具体实现放在 liushuideng.c。
 */
#ifndef LIUSHUIDENG_H
#define LIUSHUIDENG_H

#include "stm32h7xx_hal.h"
#include "led.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 种子工程先提供无参数版本，只操作 LED1；题目 1 将其扩展为带编号参数版本 */
void liushuideng(uint8_t led_count);

typedef struct 
{
    /* data */
    uint8_t led_count;
}liushuideng_config;

#ifdef __cplusplus
}
#endif

#endif /* LIUSHUIDENG_H */

