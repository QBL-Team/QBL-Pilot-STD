/**
 *\file QBL_SPI.c
 *\author no1wudi
 */

#include "QBL_SPI.h"
#include "stm32f4xx.h"
#include <stddef.h>

#define TOINSTANCE(QBL_SPI_Base) ((SPI_TypeDef*)QBL_SPI_Base) /**< 实例化模块 */

QBL_STATUS QBL_SPI_TransmitReceive(const uint32_t QBL_SPI_Base, const uint8_t* Trans, uint8_t* Rece, uint8_t Length, uint16_t TimeOut)
{
    uint32_t max_time = TimeOut + QBL_GetTick();

    while (Length) {

        if (NULL != Trans) {
            SPI_I2S_SendData(TOINSTANCE(QBL_SPI_Base), *Trans++);
        }
        else {
            SPI_I2S_SendData(TOINSTANCE(QBL_SPI_Base), 0xFF);
        }

        while (RESET == SPI_I2S_GetFlagStatus(TOINSTANCE(QBL_SPI_Base), SPI_I2S_FLAG_TXE)) {
            if (QBL_GetTick() > max_time) {
                return QBL_WRITE_FAILED;
            }
        }

        while (RESET == SPI_I2S_GetFlagStatus(TOINSTANCE(QBL_SPI_Base), SPI_I2S_FLAG_RXNE)) {
            if (QBL_GetTick() > max_time) {
                return QBL_RECEIVE_FAILED;
            }
        }

        if (NULL != Rece) {
            *Rece++ = SPI_I2S_ReceiveData(TOINSTANCE(QBL_SPI_Base));
        }
        else {
            SPI_I2S_ReceiveData(TOINSTANCE(QBL_SPI_Base));
        }
        Length--;
    }
    return QBL_OK;
}
