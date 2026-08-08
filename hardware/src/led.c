/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"

extern int get_signal(void);

static uint8_t count = 1U;
static uint8_t led_num;
static volatile blink_config config = 
{
    1U, //led_num
    250U,  //on_ms
    250U  //off_ms
};
//以上是题目4的全局变量
static uint8_t initialized;
static uint32_t now_ms;
static uint32_t duration_ms;
static mode_config current_mode = mode_idle;
static led_timer phase_timer = {
    0U,  //start_ms
    0U,  //duration_ms
    0U  //running_ms
};
//以上是题目5新增的全局变量

/* 点亮 LED1 */
static void led_on(uint8_t led_num) //swtich选择控制LED灯的亮
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
static void led_off(uint8_t led_num)  //switch选择控制LED灯的灭
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

static void mode_1(void) //模式1：单个单个亮灭（和下面统一一下名称）
{
    led_flow(config);
}

static void mode_2(void)  //模式2：两个两个亮灭
{
     for(count=1U;count<4U;count=count+2)  //count=1，count=3 分两种情况
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

static void mode_3(void)  //模式3：全部亮灭，比较简单粗暴的写法
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

static void run_mode(void)  //状态机中的run程序
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

      case 0U:  //idle模式，相当于什么都不用干

      default:
      break;  
    }
}

static uint8_t timer_now(void)  //以下开始创作计时器，这一步是取得当前系统时间，并把这个时间作为当前函数的返回值
{
    return HAL_GetTick();
}

static void timer_start(led_timer timer,uint32_t now_ms,uint32_t duration_ms) 
 //开始计时，记录开始时间、结束时间，开始计时运行时间赋值0ms
{
    phase_timer.start_ms = now_ms;
    phase_timer.duration_ms = duration_ms;
    phase_timer.running_ms = 0U;
}

static void timer_stop(led_timer timer) //结束计时，结束计时的运行时间赋值0ms
{
    phase_timer.running_ms = 0U;
}

static void exit_mode(void)  //状态机中的exit程序：熄灭LED灯，并且计数器停止工作
{
    led_off(1U);
    led_off(2U);
    led_off(3U);
    led_off(4U);
    timer_stop(phase_timer);
}

static void switch_mode (uint8_t new_mode)  //状态机中的switch程序：读取输入的新模式，让它变成现在的模式
{
    current_mode = new_mode; 
}

static void enter_mode(void)  //状态机中的enter程序：进行初始化的动作，为接下来的run程序做准备
{
    led_off(1U);
    led_off(2U);
    led_off(3U);
    led_off(4U);   //所有LED灯灭
    config.led_num = 0U;  //这个先归0，到时候run程序里面的mode会用到的

    if(current_mode == 0U)  //如果现在的模式是idle模式，就停止计时
    {
        timer_stop(phase_timer);
        return;  
    }
    run_mode();
    timer_start(phase_timer,now_ms,duration_ms); //开始计时
}

void final(void)  //主程序
{
    int signal = get_signal(); //把唯一的变量signal传过来
    mode_config requested_mode;
    if((signal < 0U)||(signal >= 4U)) //如果传过来的signal不是0123，就进入统一算作0，即idle模式
    {
        requested_mode = 0U;
    }
    else  //如果传过来的signal是0123，那么把signal传给需求的模式，需求的模式会传给新的模式，新的模式传给现在的模式
    {
        requested_mode = signal;
    }
    if((initialized == 0U)||(requested_mode != current_mode))  //如果第一次进主程序或者模式发生变化，进入状态机程序
    {
        exit_mode();
        switch_mode(requested_mode);
        enter_mode();
        initialized = 1U;  //进行一次这个计数器就失效了，跳出运行
        return;
    }
    run_mode();  //正常接收传过来的正确signal，就开始run程序
}