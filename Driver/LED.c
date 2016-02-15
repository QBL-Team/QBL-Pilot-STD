/**
 *\file LED.c
 *\author no1wudi
 */

#include "LED.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"



/**
 * 红色LED连接的管脚
 */
#define RED_LED_Pin GPIO_Pin_3
/**
 * 红色LED连接的管脚
 */
#define GREEN_LED_Pin GPIO_Pin_4
/**
 * 红色LED连接的管脚
 */
#define BLUE_LED_Pin GPIO_Pin_5
/**
 * LED连接的端口
 */
#define LED_Port GPIOE

void LED_Init(void)
{
    GPIO_InitTypeDef led;
    GPIO_DeInit(GPIOE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

    led.GPIO_Mode = GPIO_Mode_OUT;
    led.GPIO_OType = GPIO_OType_PP;
    led.GPIO_Pin = BLUE_LED_Pin | GREEN_LED_Pin | RED_LED_Pin;
    led.GPIO_PuPd = GPIO_PuPd_NOPULL;
    led.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(LED_Port, &led);

    LED_Show(LED_COLOR_OFF);
}

void LED_Show(uint8_t LED_COLOR)
{
    switch (LED_COLOR) {
    case LED_COLOR_OFF:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_SET);
        break;

    case LED_COLOR_WHITE:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_RESET);
        break;

    case LED_COLOR_BLUE:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_RESET);
        break;

    case LED_COLOR_RED:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_SET);
        break;

    case LED_COLOR_GREEN:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_SET);
        break;

    case LED_COLOR_FUCHSIN:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_RESET);
        break;

    case LED_COLOR_YELLOW:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_SET);
        break;

    case LED_COLOR_CYAN:
        GPIO_WriteBit(LED_Port, RED_LED_Pin, Bit_SET);
        GPIO_WriteBit(LED_Port, GREEN_LED_Pin, Bit_RESET);
        GPIO_WriteBit(LED_Port, BLUE_LED_Pin, Bit_RESET);
        break;

    default:
        break;
    }
}
