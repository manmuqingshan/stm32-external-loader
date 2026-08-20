/**
  **********************************************************************************************************************
  * @file    mem_loader_entry.c
  * @brief   IAR EWARM entry-point definitions for the STM32 memory loader.
  *
  **********************************************************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  **********************************************************************************************************************
  */
#include "device_loader.h"

/* True for all Armv6-M / Armv7-M / Armv8-M Cortex-M cores supported by IAR. */
#define _CORTEX_ \
  ((__CORE__ == __ARM6M__)          || (__CORE__ == __ARM6SM__)         || \
   (__CORE__ == __ARM7M__)          || (__CORE__ == __ARM7EM__)         || \
   (__CORE__ == __ARM8M__)          || (__CORE__ == __ARM8M_BASELINE__) || \
   (__CORE__ == __ARM8M_MAINLINE__) || (__CORE__ == __ARM8EM_MAINLINE__))

#ifdef RELOCATABLE_FLASHLOADER
extern void FlashPreInitEntry(void);
#endif

extern Keepincompilation void FlashInitEntry(void);
extern Keepincompilation void FlashWriteEntry(void);
extern Keepincompilation void FlashEraseWriteEntry(void);
extern Keepincompilation void FlashChecksumEntry(void);
extern Keepincompilation void FlashSignoffEntry(void);

#if defined(__ICCARM__)
extern void FlashBreak(void);
#else
extern void __attribute__((noreturn)) FlashBreak(void);
#endif

extern void Fl2FlashInitEntry(void);
extern void Fl2FlashWriteEntry(void);
extern void Fl2FlashEraseWriteEntry(void);
extern void Fl2FlashChecksumEntry(void);
extern void Fl2FlashSignoffEntry(void);

#ifdef RELOCATABLE_FLASHLOADER
STACK_LESS Keepincompilation void FlashPreInitEntry(void)
{
#if ((__CORE__ == __ARM6M__) || (__CORE__ == __ARM6SM__)) /* Cortex-M0 / M0+ */
  __asm (
      "mov     r9, pc\n"
      "nop\n"
      "mov     r6, r9\n"
      "nop\n"
      "subs    r6, r6, #4\n"
      "mov     r9, r6\n"
      "nop\n"
  );
  /* Set up the stack pointer. */
  __asm volatile ("MOV R1, %0" : : "r" (CSTACK_LIMIT & 0xFFFFU) : );
  __asm (
      "mov     sp, r1\n"
      "nop\n"
      "add     sp, sp, r9\n"
  );
  /* Set up the static base register. */
  __asm volatile ("MOV R5, %0" : : "r" (IAR_STATIC_BASE & 0xFFFFU) : );
  __asm (
      "add    r9, r9, r5\n"
  );
#else /* Cortex-M3 / M4 / M7 / M33 and above */
  __asm (
      "mov r9, pc\n"
      "nop\n"
      "sub r9, r9, #4\n"
  );
  __asm volatile ("MSR msp, %0" : : "r" (CSTACK_LIMIT & 0xFFFFU) : );
  __asm (
      "add r9, r9, r9\n"
  );
  __asm volatile ("MOV R8, %0" : : "r" (IAR_STATIC_BASE & 0xFFFFU) : );
  __asm (
      "add r9, r9, r8\n"
  );
#endif
  FlashBreak();
}
#endif /* RELOCATABLE_FLASHLOADER */

STACK_LESS Keepincompilation void FlashInitEntry(void)
{
  Fl2FlashInitEntry();
  FlashBreak();
}

STACK_LESS Keepincompilation void FlashWriteEntry(void)
{
  Fl2FlashWriteEntry();
  FlashBreak();
}

STACK_LESS Keepincompilation void FlashEraseWriteEntry(void)
{
  Fl2FlashEraseWriteEntry();
  FlashBreak();
}

STACK_LESS Keepincompilation void FlashChecksumEntry(void)
{
  Fl2FlashChecksumEntry();
  FlashBreak();
}

STACK_LESS Keepincompilation void FlashSignoffEntry(void)
{
  Fl2FlashSignoffEntry();
  FlashBreak();
}

STACK_LESS Keepincompilation void FlashBufferStart(void)
{
#if defined(__ICCARM__)
  __asm ("DS8 1024\n\t");
#else
  __asm volatile (".space 1024\n");
#endif
}

STACK_LESS Keepincompilation void FlashBufferEnd(void)
{
  __asm ("movs r0, r0\n");
}

#if !defined(_CORTEX_)
extern void __vector_table(void);

__attribute__((section(".intvec")))
const uint32_t __vector_table[] = {
  (((uint32_t)&CSTACK$$Limit) & 0xFFFFU),
  (uint32_t)FlashInitEntry
};
#endif /* !_CORTEX_ */