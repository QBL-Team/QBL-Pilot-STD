/*!
 * \file HMC5883.c
 * \author no1wudi
 */

#include "stm32f4xx.h"
#include "QBL_I2C.h"
#include "HMC5883.h"

/*!
  *
  * @ingroup hmc5883_driver
  * @{
  */

/*!
 * @defgroup hmc5883_driver_private HMC5883驱动私有数据
 * @{
 */


#define HMC5883_IIC_ADDRESS     0x3C        ///< 芯片的IIC地址，参见其数据手册
#define HMC5883_DATA_SENS       440.0f      ///< 原始数据到真实数据的缩放比例，参见其数据手册
#define HMC5883_REG_DATA_ADDR   0x03        ///< 数据寄存器起始地址
#define HMC5883_REG_DATA_LENGTH 6           ///< 读取的数据量，单位为byte
#define HMC5883_REG_ADDR_WIDTH  1           ///< 寄存器地址的宽度，单位为byte

/*!
 *\brief 数据配置表，左侧为寄存器，右侧为参数值，具体参见其数据手册
 */

static const uint8_t HMC5883_CONFIG_TABLE[] =
{
    0x00, 0x18,
    0x01, 0x80,
    0x02, 0x00,
};


QBL_STATUS HMC5883_Init(void)
{

    uint8_t cnts = 0;
    QBL_STATUS status;
    for(; cnts < sizeof(HMC5883_CONFIG_TABLE); cnts += 2)
    {
        status = QBL_I2C_Mem_Read(I2C1_BASE,HMC5883_IIC_ADDRESS,HMC5883_CONFIG_TABLE[cnts], (uint8_t *)(HMC5883_CONFIG_TABLE + 1 + cnts),1,1);
        if(QBL_OK != status)
        {
            return status;
        }
    }

    return QBL_OK;
}

QBL_STATUS HMC5883_Update(float axis[3])
{
    uint8_t tmp[6];
    QBL_STATUS status;

    status = QBL_I2C_Mem_Read(I2C1_BASE, HMC5883_IIC_ADDRESS, HMC5883_REG_DATA_ADDR, tmp, HMC5883_REG_DATA_LENGTH, 5);

    if(QBL_OK != status)
    {
        return status;
    }

    axis[0] = ((int16_t)((tmp[0] << 8) | tmp[1])) / HMC5883_DATA_SENS;
    axis[1] = ((int16_t)((tmp[4] << 8) | tmp[5])) / HMC5883_DATA_SENS;
    axis[2] = ((int16_t)((tmp[2] << 8) | tmp[3])) / HMC5883_DATA_SENS;

    return QBL_OK;
}

/*!
 * @}
 */


/*!
 * @}
 */
