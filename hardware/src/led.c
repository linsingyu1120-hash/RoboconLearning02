/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"

static led_config led_cfg = {0U};

/* 点亮 LED1 */
void led_on(uint8_t led_num)
{
    switch(led_num)
    {
        case 0U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
        break;

        case 1U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
        break;

        case 2U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
        break;

        case 3U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
        break;

        default:
        break;
    }
}

/* 熄灭 LED1 */
void led_off(uint8_t led_num)
{
    switch(led_num)
    {
        case 0U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
        break;

        case 1U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET);
        break;

        case 2U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
        break;

        case 3U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
        break;

        default:
        break;
    }
}
