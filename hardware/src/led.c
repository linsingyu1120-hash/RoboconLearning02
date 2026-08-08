/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"

extern int get_signal(void);

static uint8_t count = 1U;
static uint8_t led_num;
static uint8_t initialized;
static uint32_t now_ms;
static uint32_t duration_ms;
static mode_config current_mode = mode_idle;
static led_phase current_phase = phase_off;
static volatile blink_config config = 
{
    1U, //led_num
    250U,  //on_ms
    250U  //off_ms
};
static led_timer phase_timer = {
    0U,  //start_ms
    0U,  //duration_ms
    0U  //running_ms
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

static void run_mode(void)
{
    switch(current_mode)
    {
      case 1U:
      mode_1();
      break;

      case 2U:
      mode_2();
      break;

      case 3U:
      mode_3();
      break;

      case 0U:

      default:
      break;  
    }
}

static uint8_t timer_now(void)
{
    return HAL_GetTick();
}

static void timer_start(led_timer timer,uint32_t now_ms,uint32_t duration_ms)
{
    phase_timer.start_ms = now_ms;
    phase_timer.duration_ms = duration_ms;
    phase_timer.running_ms = 1U;
}

static void timer_stop(led_timer timer)
{
    phase_timer.running_ms = 0U;
}

static uint8_t timer_expired(led_timer timer,uint32_t now_ms)
{
    if((phase_timer.running_ms == 0U) || ((now_ms - phase_timer.start_ms)<phase_timer.duration_ms))
    {
        return 0U;
    }
    phase_timer.running_ms = 0U;
    return 1U;
}

static void exit_mode(void)
{
    led_off(1U);
    led_off(2U);
    led_off(3U);
    led_off(4U);
    timer_stop(phase_timer);
}

static void switch_mode (uint8_t new_mode)
{
    current_mode = new_mode; 
}

static void enter_mode(void)
{
    led_off(1U);
    led_off(2U);
    led_off(3U);
    led_off(4U); 
    config.led_num = 0U;    

    if(current_mode == 0U)
    {
        current_phase = 1U;
        timer_stop(phase_timer);
        return;  
    }
    current_phase = 0U;
    run_mode();
    timer_start(phase_timer,now_ms,duration_ms);
}

void final(void)
{
    int signal_value = get_signal();
    mode_config requested_mode;
    if((signal_value < 0U)||(signal_value >= 4U))
    {
        requested_mode = 0U;
    }
    else
    {
        requested_mode = signal_value;
    }
    if((initialized == 0U)||(requested_mode != current_mode))
    {
        exit_mode();
        switch_mode(requested_mode);
        enter_mode();
        initialized = 1U;
        return;
    }
    run_mode();
}