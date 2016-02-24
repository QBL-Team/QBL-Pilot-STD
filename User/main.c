#include "QBL.h"
#include "LED.h"

int main(void)
{
    QBL_Init();
    LED_Show(LED_COLOR_OFF);


    for (;;) {
        LED_Show(LED_COLOR_RED);
        QBL_Delay(100);
        LED_Show(LED_COLOR_OFF);
        QBL_Delay(100);
    }
}
