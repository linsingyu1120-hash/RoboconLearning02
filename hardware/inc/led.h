/**
 * @file    led.h
 * @brief   LED 驱动头文件。
 *          头文件放声明和宏，具体实现放在 led.c。
 */
#ifndef LED_H
#define LED_H

#include "stm32h7xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 宏定义：给 LED 使用的端口和引脚起名字 */
#define LED_GPIO_PORT GPIOB
#define LED1_PIN      GPIO_PIN_3
#define LED2_PIN      GPIO_PIN_4
#define LED3_PIN      GPIO_PIN_5
#define LED4_PIN      GPIO_PIN_6

/* 种子工程先提供无参数版本，只操作 LED1；题目 1 将其扩展为带编号参数版本 */
void final(void);

typedef struct 
{
    /* data */
    uint8_t led_num;
    uint16_t on_ms;
    uint16_t off_ms;    
} blink_config;

typedef enum
{
    mode_single=1U, // 模式1：逐个亮灭
    mode_pair=2U,  //模式2：两两亮灭
    mode_all=3U,  //模式3：一起亮灭
    mode_idle=0U,  //模式4：空闲
    requested_mode,
    signal_value
}mode_config;

typedef struct 
{
    /* data */
    uint16_t start_ms;
    uint16_t duration_ms;
    uint16_t running_ms;
}led_timer;

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
