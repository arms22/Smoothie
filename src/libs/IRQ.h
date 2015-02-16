#ifndef IRQ_H
#define IRQ_H

#include <cmsis.h>

__STATIC_INLINE uint32_t disableIRQ(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

__STATIC_INLINE void restoreIRQ(uint32_t oldprimask)
{
    __set_PRIMASK(oldprimask);
}

#endif
