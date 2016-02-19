/**
 *\file QBL_I2C.c
 *\author no1wudi
 */

#include "QBL_I2C.h"
#include "stm32f4xx_i2c.h"

#define TOINSTANCE(I2C_Base) ((I2C_TypeDef*)I2C_Base) /**< 实例化模块 */

QBL_STATUS QBL_I2C_Mem_Write(const uint32_t QBL_I2C_Base, const uint8_t DevAddr, const uint8_t RegAddr, const uint8_t* Value, uint8_t Length, uint16_t TimeOut)
{
    uint32_t max_time = TimeOut + QBL_GetTick();

    //检测总线是否为忙状态
    while (SET == I2C_GetFlagStatus(TOINSTANCE(QBL_I2C_Base), I2C_FLAG_BUSY)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_BUSY;
        }
    }

    //生成起始信号
    I2C_GenerateSTART(TOINSTANCE(QBL_I2C_Base), ENABLE);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_MODE_SELECT)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_START_FAILED;
        }
    }

    //发送7位地址
    I2C_Send7bitAddress(TOINSTANCE(QBL_I2C_Base), DevAddr, I2C_Direction_Transmitter);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base),
                        I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_ADDR_NOT_MATCH;
        }
    }

    //写寄存器地址
    I2C_SendData(TOINSTANCE(QBL_I2C_Base), RegAddr);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_WRITE_ADDR_FAILED;
        }
    }

    while (Length--) {
        //向寄存器中写入数据
        I2C_SendData(TOINSTANCE(QBL_I2C_Base), *Value++);
        while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
            if (QBL_GetTick() >= max_time) {
                return QBL_WRITE_FAILED;
            }
        }
    }

    I2C_GenerateSTOP(TOINSTANCE(QBL_I2C_Base), ENABLE);

    return QBL_OK;
}

QBL_STATUS QBL_I2C_Mem_Read(const uint32_t QBL_I2C_Base, const uint8_t DevAddr, const uint8_t RegAddr, uint8_t* Value, uint8_t Length, uint8_t TimeOut)
{
    uint32_t max_time = TimeOut + QBL_GetTick();

    //检测总线是否为忙状态
    while (SET == I2C_GetFlagStatus(TOINSTANCE(QBL_I2C_Base), I2C_FLAG_BUSY)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_BUSY;
        }
    }

    //生成起始信号
    I2C_GenerateSTART(TOINSTANCE(QBL_I2C_Base), ENABLE);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_MODE_SELECT)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_START_FAILED;
        }
    }

    //发送7位地址
    I2C_Send7bitAddress(TOINSTANCE(QBL_I2C_Base), DevAddr, I2C_Direction_Transmitter);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base),
                        I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_ADDR_NOT_MATCH;
        }
    }

    //写寄存器地址
    I2C_SendData(TOINSTANCE(QBL_I2C_Base), RegAddr);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_WRITE_ADDR_FAILED;
        }
    }

    //生成起始信号
    I2C_GenerateSTART(TOINSTANCE(QBL_I2C_Base), ENABLE);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_MODE_SELECT)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_START_FAILED;
        }
    }

    //发送7位地址
    I2C_Send7bitAddress(TOINSTANCE(QBL_I2C_Base), DevAddr, I2C_Direction_Receiver);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base),
                        I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_ADDR_NOT_MATCH;
        }
    }

    while (Length) {

        if (Length == 1) {
            I2C_AcknowledgeConfig(TOINSTANCE(QBL_I2C_Base), DISABLE);
            I2C_GenerateSTOP(TOINSTANCE(QBL_I2C_Base), ENABLE);

        }

        while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            if (QBL_GetTick() >= max_time) {
                return QBL_RECEIVE_FAILED;
            }
        }

        *Value++ = I2C_ReceiveData(TOINSTANCE(QBL_I2C_Base));


        Length--;
    }
    I2C_AcknowledgeConfig(TOINSTANCE(QBL_I2C_Base), ENABLE);
    return QBL_OK;
}

QBL_STATUS QBL_I2C_Write(const uint32_t QBL_I2C_Base, const uint8_t DevAddr, const uint8_t* Data, uint8_t Length, uint8_t TimeOut)
{
    uint32_t max_time = TimeOut + QBL_GetTick();

    //检测总线是否为忙状态
    while (SET == I2C_GetFlagStatus(TOINSTANCE(QBL_I2C_Base), I2C_FLAG_BUSY)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_BUSY;
        }
    }

    //生成起始信号
    I2C_GenerateSTART(TOINSTANCE(QBL_I2C_Base), ENABLE);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_MODE_SELECT)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_START_FAILED;
        }
    }

    //发送7位地址
    I2C_Send7bitAddress(TOINSTANCE(QBL_I2C_Base), DevAddr, I2C_Direction_Transmitter);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base),
                        I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_ADDR_NOT_MATCH;
        }
    }

    while (Length--) {
        //向寄存器中写入数据
        I2C_SendData(TOINSTANCE(QBL_I2C_Base), *Data++);
        while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {
            if (QBL_GetTick() >= max_time) {
                return QBL_WRITE_FAILED;
            }
        }
    }

    I2C_GenerateSTOP(TOINSTANCE(QBL_I2C_Base), ENABLE);
    return QBL_OK;
}

QBL_STATUS QBL_I2c_Read(const uint32_t QBL_I2C_Base, const uint8_t DevAddr, uint8_t* Data, uint8_t Length, uint8_t TimeOut)
{
    uint32_t max_time = TimeOut + QBL_GetTick();

    //检测总线是否为忙状态
    while (SET == I2C_GetFlagStatus(TOINSTANCE(QBL_I2C_Base), I2C_FLAG_BUSY)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_BUSY;
        }
    }

    //生成起始信号
    I2C_GenerateSTART(TOINSTANCE(QBL_I2C_Base), ENABLE);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_MODE_SELECT)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_START_FAILED;
        }
    }

    //发送7位地址
    I2C_Send7bitAddress(TOINSTANCE(QBL_I2C_Base), DevAddr, I2C_Direction_Transmitter);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base),
                        I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_ADDR_NOT_MATCH;
        }
    }

    //生成起始信号
    I2C_GenerateSTART(TOINSTANCE(QBL_I2C_Base), ENABLE);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_MODE_SELECT)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_START_FAILED;
        }
    }

    //发送7位地址
    I2C_Send7bitAddress(TOINSTANCE(QBL_I2C_Base), DevAddr, I2C_Direction_Receiver);
    while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base),
                        I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {
        if (QBL_GetTick() >= max_time) {
            return QBL_ADDR_NOT_MATCH;
        }
    }

    while (Length) {
        while (ERROR == I2C_CheckEvent(TOINSTANCE(QBL_I2C_Base), I2C_EVENT_MASTER_BYTE_RECEIVED)) {
            if (QBL_GetTick() >= max_time) {
                return QBL_RECEIVE_FAILED;
            }
        }

        *Data++ = I2C_ReceiveData(TOINSTANCE(QBL_I2C_Base));

        if (Length == 1) {
            I2C_AcknowledgeConfig(TOINSTANCE(QBL_I2C_Base), DISABLE);
        }
        Length--;
    }

    I2C_GenerateSTOP(TOINSTANCE(QBL_I2C_Base), ENABLE);
    I2C_AcknowledgeConfig(TOINSTANCE(QBL_I2C_Base), ENABLE);

    return QBL_OK;
}
