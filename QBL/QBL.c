/**
 *\file QBL.c
 *\author no1wudi
 */

#include "QBL.h"
#include "QBL_I2C.h"
#include "QBL_SPI.h"
#include "SD.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_usart.h"
#include <stdio.h>

static volatile uint32_t qbl_sys_ticks = 0; //系统上电计时器，单位为ms

#if __GNUC__

#else

int fputc(int ch, FILE* f)
{
    USART1->DR = ch & 0x1FF;
    while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TC))
        ;
    return ch;
}

#endif

static void QBL_Clock_Init(void)
{
    RCC_DeInit();
    RCC_HSEConfig(RCC_HSE_ON);

    while (RCC_WaitForHSEStartUp() != SUCCESS)
        ;

    RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);
    RCC_PLLCmd(ENABLE);
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    RCC_HCLKConfig(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div4);
    RCC_PCLK2Config(RCC_HCLK_Div2);
}

static void QBL_Periph_Init(void)
{
    USART_InitTypeDef ua;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    USART_DeInit(USART1);
    ua.USART_BaudRate = 115200;
    ua.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    ua.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    ua.USART_Parity = USART_Parity_No;
    ua.USART_StopBits = USART_StopBits_1;
    ua.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &ua);
    USART_Cmd(USART1, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
}

static void QBL_IO_Init(void)
{
    GPIO_InitTypeDef io;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    {
        //SDIO
        io.GPIO_Mode = GPIO_Mode_AF;
        io.GPIO_OType = GPIO_OType_PP;
        io.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
        io.GPIO_PuPd = GPIO_PuPd_NOPULL;
        io.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_Init(GPIOC, &io);

        io.GPIO_Pin = GPIO_Pin_2;
        GPIO_Init(GPIOD, &io);

        GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_SDIO);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_SDIO);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_SDIO);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_SDIO);
        GPIO_PinAFConfig(GPIOC, GPIO_PinSource12, GPIO_AF_SDIO);
        GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF_SDIO);
    }

    {
        //USART
        io.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOB, &io);

        GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1);
    }
}

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
    for (;;) {
        if (QBL_GetTick() >= tick) {
            break;
        }
    }
}

void QBL_Init(void)
{
    QBL_Clock_Init();
    QBL_IO_Init();
    QBL_Periph_Init();

    QBL_I2C_Init();
    QBL_SPI_Init();
    SysTick_Config(SystemCoreClock / 1000);
}

uint32_t QBL_GetTick()
{
    return qbl_sys_ticks;
}
