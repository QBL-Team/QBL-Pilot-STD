#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "QBL.h"
#include "LED.h"
#include "QBL_I2C.h"
#include "stm32f4xx_i2c.h"

int main(void)
{
    QBL_Init();
    LED_Init();

    for (;;) {
        LED_Show(LED_COLOR_RED);
        QBL_Delay(100);
        LED_Show(LED_COLOR_OFF);
        QBL_Delay(100);
    }
}
