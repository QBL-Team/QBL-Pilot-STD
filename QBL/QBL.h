#ifndef QBL_H
#define QBL_H

#include <stdint.h>

extern void QBL_Init(void);
extern void QBL_Delay(uint32_t ms);
extern uint32_t QBL_GetTick(void);

#endif // QBL_H
