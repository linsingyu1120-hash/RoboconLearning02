/**
 * @file    led.c
 * @brief   LED 驱动实现文件。
 */
#include "led.h"
#include "buzzer.h"

extern int get_signal(void);

static uint8_t final_initialized = 0U;
static uint8_t count = 1U;
static uint8_t led_step = 0U;
static uint8_t led_is_on = 0U;
static uint8_t buzzer_is_on = 0U;
static uint32_t now_ms;
static uint32_t duration_ms;
static uint32_t start_ms;
static mode_config current_mode = mode_idle;

static blink_config config = 
{
    1U, //led_num
    250U,  //on_ms
    250U,  //off_ms
    0U,  //buzzer_judge
};
static beep_config config_ms =
{
    0U,  //beep_sound
    0U,  //beep_shut
    0U  //beep
};
static once_timer timer_ms = 
{
    0U,  //start_ms
    0U,  //duration_ms
    0U  //running_judge
} ;

void buzzer_on(void);
void buzzer_off(void);


static once_timer led_timer = {0U};
static once_timer buzzer_timer = {0U};

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

static void timer_start(once_timer *timer_ms, uint32_t now_ms,uint32_t duration_ms)  //开始计时
{
    timer_ms->start_ms = now_ms;  //记录当前时间
    timer_ms->duration_ms = duration_ms;  //记录计时时间
    timer_ms->running_judge = 1U;  
}

static void timer_stop(once_timer *timer_ms)  //结束计时，即计数器到期了
{
    timer_ms->running_judge = 0;  
}

static uint8_t timer_is_expired(once_timer *timer_ms, uint32_t now_ms)  //计时器是否到期判断
{
    if (timer_ms->running_judge &&((now_ms - timer_ms->start_ms) >= timer_ms->duration_ms))  
    //如果运行判断置1，并且运行时间大于计时时间，说明到期了，要执行下面的程序
    {
        timer_ms->running_judge = 0;  
        return 1;
    }

    return 0;
}

static void mode_1(void)  // 逐个亮灭，蜂鸣器不工作
{
    if (led_step >= 4U)  //阶段大于3，统统置0，返回LED1
    {
        led_step = 0U;
    }

    if (led_is_on == 1U)  
    //led_is_on是一个用来判断是否该换灯操作的量，led_is_on=1时，阶段+1（LED向前亮一个）
    {
        led_on(led_step + 1U);
    }

    if (timer_is_expired(&led_timer, now_ms))  //如果LED计时器到期了(1)，执行下面程序
    {
        if (led_is_on == 1U)
        {
            led_off(led_step + 1U);  //对应灯熄灭
            led_is_on = 0U;  

            timer_start(&led_timer,now_ms,config.off_ms);  //重新开始计时
        }
        else  //如果LED计时器没到期（0），执行下面程序
        {
            led_step++;  //阶段+1

            if (led_step >= 4U)  //同上
            {
                led_step = 0U;
            }

            led_on(led_step + 1U);
            led_is_on = 1U;

            timer_start(&led_timer,now_ms,config.on_ms);
        }
    }
}

static void mode_2(void)  // 两两亮灭，蜂鸣器响200ms、停800ms
{
    config_ms.beep_sound = 200U;  //定义，让蜂鸣器响200ms、停800ms
    config_ms.beep_shut = 800U;

    if (led_step > 1U) 
    {
        led_step = 0U;
    }

    if (led_is_on == 1U)  //赋1:12闪烁，赋0:34闪烁
    {
        if (led_step == 0U) 
        {
            led_on(1U);
            led_on(2U);
        }
        else  //
        {
            led_on(3U);
            led_on(4U);
        }
    }

    if (timer_is_expired(&led_timer, now_ms))  //如果计时器到期，运行下面程序
    {
        if (led_is_on == 1U) 
        {
            if (led_step == 0U)  //阶段0，关LED1和LED2
            {
                led_off(1U);
                led_off(2U);
            }
            else  //阶段123，关LED3和LED4
            {
                led_off(3U);
                led_off(4U);
            }

            led_is_on = 0U;  //LED停止工作

            timer_start(&led_timer,now_ms,config.off_ms);  //重新计时
        }
        else  //如果LED没在工作，切换到另一组（LED3和LED4闪烁）程序
        {
            if (led_step == 0U)
            {
                led_step = 1U;
                led_on(3U);
                led_on(4U);
            }
            else
            {
                led_step = 0U;
                led_on(1U);
                led_on(2U);
            }

            led_is_on = 1U;

            timer_start(&led_timer,now_ms,config.on_ms);  //重新计时
        }
    }

    if ((buzzer_timer.running_judge == 0U) &&(buzzer_is_on == 0U))
    {
        buzzer_on();
        buzzer_is_on = 1U;
        timer_start(&buzzer_timer,now_ms,config_ms.beep_sound);
    }

    if (timer_is_expired(&buzzer_timer, now_ms))
    {
        if (buzzer_is_on == 1U)
        {
            buzzer_off();
            buzzer_is_on = 0U;
            timer_start(&buzzer_timer,now_ms,config_ms.beep_shut);
        }
        else
        {
            buzzer_on();
            buzzer_is_on = 1U;
            timer_start(&buzzer_timer,now_ms,config_ms.beep_sound);
        }
    }
}

static void mode_3(void)  // 全部一起亮灭，蜂鸣器响50ms、停100ms
{
    config_ms.beep_sound = 50U;
    config_ms.beep_shut = 100U;

    if (led_is_on == 1U)
    {
        led_on(1U);
        led_on(2U);
        led_on(3U);
        led_on(4U);
    }

    if (timer_is_expired(&led_timer, now_ms))
    {
        if (led_is_on == 1U)
        {
            led_off(1U);
            led_off(2U);
            led_off(3U);
            led_off(4U);

            led_is_on = 0U;

            timer_start(&led_timer,now_ms,config.off_ms);
        }
        else
        {
            led_on(1U);
            led_on(2U);
            led_on(3U);
            led_on(4U);

            led_is_on = 1U;

            timer_start(&led_timer,now_ms,config.on_ms);
        }
    }

    /*
     * 刚进入模式3时，立即启动第一次短鸣。
     */
    if ((buzzer_timer.running_judge == 0U) &&(buzzer_is_on == 0U))
    {
        buzzer_on();
        buzzer_is_on = 1U;

        timer_start(&buzzer_timer,now_ms,config_ms.beep_sound);
    }

    /*
     * 蜂鸣器独立进行响/停切换。
     */
    if (timer_is_expired(&buzzer_timer, now_ms))
    {
        if (buzzer_is_on == 1U)
        {
            buzzer_off();
            buzzer_is_on = 0U;

            timer_start(&buzzer_timer,now_ms,config_ms.beep_shut);
        }
        else
        {
            buzzer_on();
            buzzer_is_on = 1U;

            timer_start(&buzzer_timer,now_ms,config_ms.beep_sound);
        }
    }
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

static void exit_mode(void)  //状态机中的exit程序：熄灭LED灯,蜂鸣器停止工作
{
    led_off(1U);
    led_off(2U);
    led_off(3U);
    led_off(4U);
    buzzer_off();

    timer_stop(&led_timer);
    timer_stop(&buzzer_timer);
    led_is_on = 0;
    buzzer_is_on = 0;
}

static void switch_mode (uint8_t new_mode)  //状态机中的switch程序：读取输入的新模式，让它变成现在的模式
{
    current_mode = new_mode; 
}

static void enter_mode(void)  //状态机中的enter程序：进行初始化的动作，为接下来的run程序做准备
{
    switch(current_mode)
    {
        case 0U:
        break;

        case 1U:
        led_is_on = 1;
        timer_start(&led_timer, now_ms, config.on_ms);
        break;

        case 2U:
        led_is_on = 1;
        timer_start(&led_timer, now_ms, config.on_ms);
        break;

        case 3U:
        led_is_on = 1;
        timer_start(&led_timer, now_ms, config.on_ms);
        break;

        default:
        break;
    }

    if (config.buzzer_judge)
    {
        buzzer_on();
        buzzer_is_on = 1;
        timer_start(&buzzer_timer, now_ms, config.beep);
    }
}

void final(void)  //主程序
{
    now_ms = HAL_GetTick();
    int signal = get_signal(); //把唯一的变量signal传过来
    mode_config requested_mode;
    if((signal < 0U)||(signal >= 4U)) //如果传过来的signal不是0123，就进入统一算作0，即idle模式
    {
        requested_mode = 0U;
    }
    else  //如果传过来的signal是0123，那么把signal传给requested_mode,requested_mode会和current_mode进行比较切换
    {
        requested_mode = signal;
    }
    if((final_initialized == 0U)||(requested_mode != current_mode))  
    //如果第一次进主程序或者模式发生变化，进入状态机程序
    {
        exit_mode();
        switch_mode(requested_mode);
        enter_mode();
        final_initialized = 1U;  //之后都不再是第一次进入主程序了
        return;
    }
    run_mode();  //正常接收传过来的正确signal，就开始run程序
}