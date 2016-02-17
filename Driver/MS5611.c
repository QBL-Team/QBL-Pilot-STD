/*!
 * \file MS5611.c
 * \author no1wudi
 */

#include "MS5611.h"
#include "QBL_SPI.h"
#include "stm32f4xx_gpio.h"

/*!
  *
  * @ingroup ms5611_driver MS5611 驱动
  * @{
  */

/*!
 * @defgroup ms5611_driver_internal MS5611 驱动内部
 * @{
 */

#define MS5611_CS_On() GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_RESET) ///< 开启芯片片选
#define MS5611_CS_Off() GPIO_WriteBit(GPIOE, GPIO_Pin_9, Bit_SET) ///< 关闭芯片片选
#define MS5611_CONVERSION_TIME 10 ///< 芯片的转换时间，单位为ms

const uint8_t MS5611_CMD_RST[] = { 0x1E }; ///< 重置芯片
const uint8_t MS5611_CMD_CONVERT_D1[] = { 0x48 }; ///< 设置过采样率为4096转换气压值
const uint8_t MS5611_CMD_CONVERT_D2[] = { 0x58 }; ///< 设置过采样率为4096转换温度值
const uint8_t MS5611_CMD_READ_ADC[] = { 0x00 }; ///< 读取模数转换器转换结果
const uint8_t MS5611_CMD_READ_C1[] = { 0xA2 }; ///< 读取出厂预设值C1
const uint8_t MS5611_CMD_READ_C2[] = { 0xA4 }; ///< 读取出厂预设值C2
const uint8_t MS5611_CMD_READ_C3[] = { 0xA6 }; ///< 读取出厂预设值C3
const uint8_t MS5611_CMD_READ_C4[] = { 0xA8 }; ///< 读取出厂预设值C4
const uint8_t MS5611_CMD_READ_C5[] = { 0xAA }; ///< 读取出厂预设值C5
const uint8_t MS5611_CMD_READ_C6[] = { 0xAC }; ///< 读取出厂预设值C6

static uint32_t ms_ticks = 0; ///< 读取操作的时间戳，单位为ms

static int64_t c1 = 0; ///<出厂预设值C1,压力灵敏度 | SENS T1
static int64_t c2 = 0; ///<出厂预设值C2,压力偏移值 | OFF T1
static int64_t c3 = 0; ///<出厂预设值C3,传感器灵敏度的温度系数 | TCS
static int64_t c4 = 0; ///<出厂预设值C4,压力偏移的温度系数 | TCO
static int64_t c5 = 0; ///<出厂预设值C5,参考温度 | T REF
static int64_t c6 = 0; ///<出厂预设值C6,温度的校正系数 | TEMPSENS

static int64_t d1 = 0; ///< 压力值
static int64_t d2 = 0; ///< 温度值

static int64_t dt = 0; ///< 实际温度和参考温度的差值
static int64_t temp = 0; ///< 实际温度，范围为-40~85°C，分辨率为0.01°C，例如值为2000，表示20.00°C

static int64_t off = 0; ///< 实际温度的偏移
static int64_t sens = 0; ///< 实际温度的灵敏度
static int64_t p = 0; ///< 温度矫正后的气压值，范围为10~1200mbar，分辨率为0.01mbar，例如值为10000，表示100.00mbr

static int64_t t2 = 0;
static int64_t off2 = 0;
static int64_t sens2 = 0;

/*!
 *\enum MS5611_STATE
 * 读取过程的状态标志
 */
typedef enum {
    MS5611_STATE_IDLE = 0, ///< 上电之后，默认处于本状态
    MS5611_STATE_WAIT_FOR_D1, ///<等待气压转换操作
    MS5611_STATE_WAIT_FOR_D2, ///<等待温度转换操作
} MS5611_STATE;

static MS5611_STATE ms_state; ///< 读取过程的状态变量

/*!
 * \brief MS5611_ReadPROM   读取芯片的出厂预设值
 * \param MS5611_CMD        读取预设值的命令代码
 * \return 读取出来的值
 */

static uint16_t MS5611_ReadPROM(const uint8_t* MS5611_CMD)
{
    uint8_t tmp[2];

    MS5611_CS_On();
    QBL_SPI_TransmitReceive(SPI2_BASE, MS5611_CMD, NULL, 1, 1);
    QBL_SPI_TransmitReceive(SPI2_BASE, NULL, tmp, 2, 1);
    MS5611_CS_Off();
    return (uint16_t)(tmp[0] << 8 | tmp[1]);
}

/*!
 * \brief MS5611_SendCMD 向芯片发送一条命令
 * \param MS5611_CMD    将被发送的命令，以MS5611_CMD开头
 */
static void MS5611_SendCMD(const uint8_t* MS5611_CMD)
{
    MS5611_CS_On();
    QBL_SPI_TransmitReceive(SPI2_BASE, MS5611_CMD, NULL, 1, 1);
    MS5611_CS_Off();
}
/*!
 * \brief MS5611_Compute    在转换和读取完成后，计算温度值和气压值
 */
static void MS5611_Compute(void)
{
    //refered to datasheet
    dt = d2 - c5 * (1 << 8);
    temp = 2000 + dt * c6 / (1 << 23);

    off = c2 * (1 << 16) + (c4 * dt) / (1 << 7);
    sens = c1 * (1 << 15) + (c3 * dt) / (1 << 8);
    p = (d1 * sens / (1 << 21) - off) / (1 << 15);

    if (temp < 2000) {
        t2 = dt * dt / ((uint64_t)1 << 31);
        off2 = 5 * (temp - 2000) * (temp - 2000) / 2;
        sens2 = 5 * (temp - 2000) * (temp - 2000) / 4;

        if (temp < -1500) {
            off2 = off2 + 7 * (temp + 1500) * (temp + 1500);
            sens2 = sens2 + 11 * (temp + 1500) * (temp + 1500) / 2;
        }
    }
    else {
        t2 = 0;
        sens2 = 0;
        off2 = 0;
    }

    temp = temp - t2;
    off = off - off2;
    sens = sens - sens2;
}

/*!
 * @}
 */

QBL_STATUS MS5611_Init(void)
{

    //reset the state machine
    ms_state = MS5611_STATE_IDLE;

    //Reset the chip

    MS5611_SendCMD(MS5611_CMD_RST);

    //Wait for chip reset
    QBL_Delay(10);

    //Read PROM

    c1 = MS5611_ReadPROM(MS5611_CMD_READ_C1); //Dummy read to activate sensor

    c1 = MS5611_ReadPROM(MS5611_CMD_READ_C1);
    c2 = MS5611_ReadPROM(MS5611_CMD_READ_C2);
    c3 = MS5611_ReadPROM(MS5611_CMD_READ_C3);
    c4 = MS5611_ReadPROM(MS5611_CMD_READ_C4);
    c5 = MS5611_ReadPROM(MS5611_CMD_READ_C5);
    c6 = MS5611_ReadPROM(MS5611_CMD_READ_C6);

    return QBL_OK;
}

bool MS5611_Update(float* Pressure, float* Temperature)
{
    uint8_t tmp[3];
    switch (ms_state) {
    case MS5611_STATE_IDLE:
        MS5611_SendCMD(MS5611_CMD_CONVERT_D1);
        ms_ticks = QBL_GetTick();
        ms_state = MS5611_STATE_WAIT_FOR_D1;
        return false;

    case MS5611_STATE_WAIT_FOR_D1:

        if (QBL_GetTick() - ms_ticks >= MS5611_CONVERSION_TIME) {
            //Read adc result
            MS5611_CS_On();
            QBL_SPI_TransmitReceive(SPI2_BASE, (void*)MS5611_CMD_READ_ADC, NULL, 1, 1);
            QBL_SPI_TransmitReceive(SPI2_BASE, NULL, tmp, 3, 1);
            MS5611_CS_Off();

            //Conver to normal order
            d1 = (uint32_t)((tmp[0] << 16) | (tmp[1] << 8) | tmp[2]);

            MS5611_SendCMD(MS5611_CMD_CONVERT_D2);
            ms_ticks = QBL_GetTick();
            ms_state = MS5611_STATE_WAIT_FOR_D2;
        }

        return false;

    case MS5611_STATE_WAIT_FOR_D2:

        if (QBL_GetTick() - ms_ticks >= MS5611_CONVERSION_TIME) {
            //Read adc result
            MS5611_CS_On();
            QBL_SPI_TransmitReceive(SPI2_BASE,MS5611_CMD_READ_ADC,NULL, 1, 1);
            QBL_SPI_TransmitReceive(SPI2_BASE,NULL, tmp, 3, 1);
            MS5611_CS_Off();

            //Conver to normal order
            d2 = (uint32_t)((tmp[0] << 16) | (tmp[1] << 8) | tmp[2]);

            MS5611_SendCMD((void*)&MS5611_CMD_CONVERT_D1);
            ms_ticks = QBL_GetTick();
            ms_state = MS5611_STATE_WAIT_FOR_D1;

            MS5611_Compute();

            *Pressure = p / 100.0f;
            *Temperature = temp / 100.0f;

            return true;
        }

        return false;

    default:
        return false;
    }
}

/*!
 * @}
 */
