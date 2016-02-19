/**
 *\file QBL.c
 *\author no1wudi
 */

#include "QBL.h"
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
    I2C_InitTypeDef ii;
    SPI_InitTypeDef sp;
    USART_InitTypeDef ua;

    //I2C 1
    {
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

        ii.I2C_Ack = I2C_Ack_Enable;
        ii.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
        ii.I2C_ClockSpeed = 400000;
        ii.I2C_DutyCycle = I2C_DutyCycle_2;
        ii.I2C_Mode = I2C_Mode_I2C;
        ii.I2C_OwnAddress1 = 0xAA;
        I2C_Init(I2C1, &ii);
        I2C_Cmd(I2C1, ENABLE);
    }

    {
        //SPI1
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

        sp.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
        sp.SPI_CPHA = SPI_CPHA_1Edge;
        sp.SPI_CPOL = SPI_CPOL_Low;
        sp.SPI_CRCPolynomial = 7;
        sp.SPI_DataSize = SPI_DataSize_8b;
        sp.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        sp.SPI_FirstBit = SPI_FirstBit_MSB;
        sp.SPI_Mode = SPI_Mode_Master;
        sp.SPI_NSS = SPI_NSS_Soft;

        SPI_Init(SPI1, &sp);
        SPI_Cmd(SPI1, ENABLE);
    }

    {
        //USART1

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

        ua.USART_BaudRate = 115200;
        ua.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        ua.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        ua.USART_Parity = USART_Parity_No;
        ua.USART_StopBits = USART_StopBits_1;
        ua.USART_WordLength = USART_WordLength_8b;
        USART_Init(USART1, &ua);
        USART_Cmd(USART1, ENABLE);
    }
}

static void QBL_IO_Init(void)
{
    GPIO_InitTypeDef io;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    {
        //I2C1
        io.GPIO_Mode = GPIO_Mode_AF;
        io.GPIO_OType = GPIO_OType_OD;
        io.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
        io.GPIO_PuPd = GPIO_PuPd_UP;
        io.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_Init(GPIOB, &io);

        GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
    }

    {
        //SPI1
        io.GPIO_OType = GPIO_OType_PP;
        io.GPIO_PuPd = GPIO_PuPd_NOPULL;
        io.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
        GPIO_Init(GPIOA, &io);

        GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
        GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
    }

    {
        //LED和SPI片选信号
        io.GPIO_Mode = GPIO_Mode_OUT;
        io.GPIO_OType = GPIO_OType_PP; //MS5611       //W25Q16
        io.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_9 | GPIO_Pin_10;
        io.GPIO_PuPd = GPIO_PuPd_NOPULL;
        io.GPIO_Speed = GPIO_Speed_100MHz;
        GPIO_Init(GPIOE, &io);
    }

    {
        //按键和拨码开关
        io.GPIO_Mode = GPIO_Mode_IN;
        io.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2;
        GPIO_Init(GPIOE, &io);
    }

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
    SysTick_Config(SystemCoreClock / 1000);
}

uint32_t QBL_GetTick()
{
    return qbl_sys_ticks;
}
