/**
 *\file QBL.c
 *\author no1wudi
 */


#include "QBL.h"
#include "stm32f4xx_rcc.h"

static volatile uint32_t qbl_sys_ticks = 0;

/**
 * @brief SysTick_Handler 滴答定时器中断服务函数
 */

void SysTick_Handler(void)
{
    qbl_sys_ticks++;
}

void QBL_Delay(uint32_t ms)
{
    uint32_t tick = QBL_GetTick() + ms;
    for(;;){
        if(QBL_GetTick() >= tick ){
            break;
        }
    }
}

void QBL_Init(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

uint32_t QBL_GetTick()
{
    return qbl_sys_ticks;
}
