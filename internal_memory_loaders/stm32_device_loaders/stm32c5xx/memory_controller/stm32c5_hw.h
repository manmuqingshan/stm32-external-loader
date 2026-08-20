/**
  **********************************************************************************************************************
  * @file    stm32c5_hw.h
  * @brief   STM32C5 memory hardware register and layout definitions.
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

#ifndef STM32C5_HW_H
#define STM32C5_HW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <intrinsics.h>

#define __STATIC_INLINE static inline
#define __IO volatile

#define STM32C5_KB_SIZE                         (0x400U)
#define STM32C5_FLASHSIZE_BASE                  (0x08FFF80CU)
#define STM32C5_FLASHSIZE_MASK                  (0x00000FFFU)
#define STM32C5_FLASHSIZE_INVALID               (0x0FFFU)

#define STM32C5_FLASH_BASE_ADDR1                (0x08000000U)
#define STM32C5_FLASH_BASE_ADDR2                (0x08400000U)
#define STM32C5_EDATA_BASE_ADDR                 (0x09000000U)
#define STM32C5_EDATA_DISABLED_SIZE             (0x10000U)
#define STM32C5_EDATA_DISABLED_BANK_SIZE        (0x8000U)
#define STM32C5_EDATA_ENABLED_BANK_SIZE         (0x6000U)
#define STM32C5_USER_FLASH_PAGE_SIZE            (0x2000U)
#define STM32C5_EDATA_DISABLED_PAGE_SIZE        (0x800U)
#define STM32C5_EDATA_ENABLED_PAGE_SIZE         (0x600U)
#define STM32C5_FLASH_PROGRAM_BLOCK_SIZE        (16U)
#define STM32C5_FLASH_DOUBLE_WORD_SIZE          (8U)
#define STM32C5_FLASH_ERASED_WORD               (0xFFFFFFFFU)
#define STM32C5_FLASH_PAGE_OFFSET_MASK          (0x00FFFFFFU)

#define STM32C5_OB_DIRECT_WRITE_START           (0x400220A0U)
#define STM32C5_OB_DIRECT_WRITE_END             (0x400220D0U)
#define STM32C5_OB_WORD_SIZE                    (4U)
#define STM32C5_OB_WRITE_STRIDE                 (8U)

#define STM32C5_ACR_EMPTY_BIT                   (0x00010000U)
#define STM32C5_ACR_EMPTY_CLEAR_MASK            ((uint32_t)~STM32C5_ACR_EMPTY_BIT)

#define FLASH_KEY1                              ((uint32_t)0x45670123U)
#define FLASH_KEY2                              ((uint32_t)0xCDEF89ABU)
#define FLASH_OPT_KEY1                          ((uint32_t)0x08192A3BU)
#define FLASH_OPT_KEY2                          ((uint32_t)0x4C5D6E7FU)

#define PAGE_MASK                               ((uint32_t)0x0000001FU)

#define FLASH_WRPERR                            ((uint32_t)0x00020000U)
#define FLASH_PGSERR                            ((uint32_t)0x00040000U)
#define FLASH_STRBERR                           ((uint32_t)0x00080000U)
#define FLASH_INCERR                            ((uint32_t)0x00200000U)
#define FLASH_OBKERR                            ((uint32_t)0x00400000U)
#define FLASH_OBKWERR                           ((uint32_t)0x02000000U)

#define FLASH_SR_BSY                            ((uint32_t)0x00000001U)
#define FLASH_PGERR                             (FLASH_WRPERR | FLASH_STRBERR | FLASH_PGSERR | \
                                                 FLASH_INCERR | FLASH_OBKERR | FLASH_OBKWERR)

#define FLASH_CR_LOCK                           ((uint32_t)0x00000001U)
#define FLASH_CR_PG                             ((uint32_t)0x00000002U)
#define FLASH_CR_PER                            ((uint32_t)0x00000004U)
#define FLASH_CR_STRT                           ((uint32_t)0x00000020U)
#define FLASH_CR_MER                            ((uint32_t)0x00008000U)
#define FLASH_CR_EDATASEL                       ((uint32_t)0x20000000U)
#define FLASH_CR_BKSEL                          ((uint32_t)0x80000000U)

#define FLASH_OPTCR_OPTLOCK                     ((uint32_t)0x00000001U)
#define FLASH_OPTCR_OPTSTRT                     ((uint32_t)0x00000002U)
#define FLASH_OPTSR_SWAP_BANK                   ((uint32_t)0x80000000U)

#define FLASH_R_BASE                            ((uint32_t)0x40022000U)

typedef struct {
  __IO uint32_t ACR;
  __IO uint32_t KEYR;
  __IO uint32_t RESERVED1;
  __IO uint32_t OPTKEYR;
  __IO uint32_t RESERVED2;
  __IO uint32_t RESERVED3;
  __IO uint32_t OPSR;
  __IO uint32_t OPTCR;
  __IO uint32_t SR;
  __IO uint32_t RESERVED4;
  __IO uint32_t CR;
  __IO uint32_t RESERVED5;
  __IO uint32_t CCR;
  __IO uint32_t RESERVED6[7];
  __IO uint32_t OPTSR;
} FLASH_TypeDef;

#define FLASH                                  ((FLASH_TypeDef *)FLASH_R_BASE)

#ifdef __ICCARM__
__STATIC_INLINE void __disable_irq(void)
{
  __disable_interrupt();
}
#elif defined(__ARMCC_VERSION)
__STATIC_INLINE void __disable_irq(void)
{
  __asm volatile ("cpsid i" : : : "memory");
}
#elif defined(__GNUC__)
__STATIC_INLINE void __disable_irq(void)
{
  __asm volatile ("cpsid i" : : : "memory");
}
#else
#error "Unsupported compiler"
#endif

#ifdef __cplusplus
}
#endif

#endif /* STM32C5_HW_H */
