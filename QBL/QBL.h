/**
 *\file QBL.h
 *\author no1wudi
 */

#ifndef QBL_H
#define QBL_H



#ifdef __cplusplus

extern "C" {

#endif

/**
 *
 * @defgroup qbl QBL Common
 * @{
 */

#include <stdint.h>

/**
 * 状态量定义
 */

typedef enum {
    QBL_OK = 0, /**< 没有错误发生 */
    QBL_TIMEOUT, /**< 超出限定的执行时间 */
    QBL_BUSY,  /**< 模块正处于忙碌状态 */
    QBL_START_FAILED,  /**< 启动失败 */
    QBL_WRITE_ADDR_FAILED, /**< 写地址失败 */
    QBL_WRITE_FAILED,   /**< 写数据失败 */
    QBL_ADDR_NOT_MATCH, /**< 地址不匹配 */
    QBL_RECEIVE_FAILED, /**< 读取数据失败 */
} QBL_STATUS;

/**
 * @brief QBL_Init 初始化QBL库底层所必须的资源
 */

extern void QBL_Init(void);

/**
 * @brief QBL_Delay 提供延时功能
 * @param ms 延时的时间长度，单位为毫秒
 */

extern void QBL_Delay(uint32_t ms);

/**
 * @brief QBL_GetTick 获得从QBL库初始化以来经过的时间
 * @return 返回时间的单位为毫秒
 */

extern uint32_t QBL_GetTick(void);

/**
 * @}
 */


#ifdef __cplusplus

}

#endif

#endif // QBL_H
