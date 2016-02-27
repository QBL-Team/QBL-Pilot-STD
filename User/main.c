#include "QBL.h"
#include "LED.h"
#include "PWMInput.h"
#include "PWMOutput.h"
#include "mavlink.h"

int main(void)
{
    QBL_Init();
    LED_Init();
    LED_Show(LED_COLOR_OFF);

    PWMInput_Init();
    PWMOutput_Init();


    for (;;) {
        LED_Show(LED_COLOR_RED);
        QBL_Delay(100);
        LED_Show(LED_COLOR_OFF);
        QBL_Delay(100);
    }
}
