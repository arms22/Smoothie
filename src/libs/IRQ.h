#ifndef IRQ_H
#define IRQ_H

#include "sLPC17xx.h"

static __INLINE uint32_t disableIRQ(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

static __INLINE void restoreIRQ(uint32_t oldprimask)
{
    __set_PRIMASK(oldprimask);
}

#endif
