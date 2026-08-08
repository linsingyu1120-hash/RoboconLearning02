/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"

/* 点亮 LED1 */
static void led_on(uint8_t led_num)
{
    switch(led_num)
    {
        case 1U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_SET);
        break;

        case 2U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_PIN, GPIO_PIN_SET);
        break;

        case 3U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_SET);
        break;

        case 4U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_PIN, GPIO_PIN_SET);
        break;

        default:
        break;
    }
}

/* 熄灭 LED1 */
static void led_off(uint8_t led_num)
{
    switch(led_num)
    {
        case 1U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED1_PIN, GPIO_PIN_RESET);
        break;

        case 2U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED2_PIN, GPIO_PIN_RESET);
        break;

        case 3U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED3_PIN, GPIO_PIN_RESET);
        break;

        case 4U:
        HAL_GPIO_WritePin(LED_GPIO_PORT, LED4_PIN, GPIO_PIN_RESET);
        break;

        default:
        break;
    }
}

static void blink_led(blink_config config ) //一盏LED灯的亮灭流程
{
    led_on(config.led_num);
    HAL_Delay(config.on_ms);
    led_off(config.led_num);
    HAL_Delay(config.off_ms);

}

void led_flow(void)
{
    uint8_t count;
    blink_config config = {1U,250U,250U};
    for(count=1U;count<5U;count++)
    {
        config.led_num=count;
        blink_led(config);
    }

}

