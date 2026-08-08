/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"

extern int get_signal(void);

static uint8_t count = 1U;
static uint8_t led_num;
static mode_config current_mode = mode_single;
static volatile blink_config config = 
{
    1U, //led_num
    250U,  //on_ms
    250U  //off_ms
};

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

static void blink_led(blink_config config) //一盏LED灯的亮灭流程
{
    led_on(config.led_num);
    HAL_Delay(config.on_ms);
    led_off(config.led_num);
    HAL_Delay(config.off_ms);
}

static void led_flow(blink_config config)  //LED灯自动按顺序亮灭
{
    for(count=1U;count<5U;count++)
    {
        config.led_num=count;
        blink_led(config);
    }
}

static void mode_1(void)
{
    led_flow(config);
}

static void mode_2(void)
{
     for(count=1U;count<4U;count=count+2)
    {
        led_num=count;
        led_on(led_num);
        led_num++;
        led_on(led_num);
        HAL_Delay(config.on_ms);

        led_off(led_num);
        led_num--;
        led_off(led_num);
        HAL_Delay(config.off_ms);
    }   
}

static void mode_3(void)
{
    led_on(1U);
    led_on(2U);
    led_on(3U);
    led_on(4U);
    HAL_Delay(config.on_ms);

    led_off(1U);
    led_off(2U);
    led_off(3U);
    led_off(4U);
    HAL_Delay(config.off_ms);
}

void flow_mode(void)
{
    int signal_value;
    mode_config requested_mode;
    signal_value = get_signal();

    if ((signal_value < 0) ||(signal_value >= 3))
    {
        return;
    }

    requested_mode = signal_value;

    if (requested_mode != current_mode)
    {
        led_off(1U);
        led_off(2U);
        led_off(3U);
        led_off(4U); //exit
        current_mode = requested_mode;  //switch
        config.led_num = 1U;  //enter
    }

    switch(current_mode)  //run
    {
      case 0U:
      mode_1();
      break;

      case 1U:
      mode_2();
      break;

      case 2U:
      mode_3();
      break;

      default:
      break;  
    }
}