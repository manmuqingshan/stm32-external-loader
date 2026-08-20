/**
  **********************************************************************************************************************
  * @file    stm32c5_memory_controller.h
  * @brief   Header file for STM32C5 memory controller helpers.
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

#ifndef __STM32C5xx_MEMORY_HAL_H
#define __STM32C5xx_MEMORY_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32c5_hw.h"

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Generic flag status values.
 */
typedef enum {
  RESET = 0,
  SET = !RESET
} FlagStatus;
/**
 * @brief Erase case selector used by page erase routine.
 */
typedef enum {
  USER_MEM_BANK0 = 1,
  USER_MEM_BANK1,
  EDATA_DISABLED_BANK0,
  EDATA_DISABLED_BANK1,
  EDATA_ENABLED_BANK0,
  EDATA_ENABLED_BANK1,
} PageEraseCase;
/**
 * @brief Memory operation status values.
 */
typedef enum {
  MEM_BUSY = 1,
  MEM_ERROR_RD,
  MEM_ERROR_PGS,
  MEM_ERROR_PGP,
  MEM_ERROR_PGA,
  MEM_ERROR_WRP,
  MEM_ERROR_PROGRAM,
  MEM_ERROR_OPERATION,
  MEM_COMPLETE
} MEM_Status;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Unlocks the memory control register access.
 */
void MEM_Unlock(void);

/**
 * @brief  Erases a specified page in memory.
 * @param  page Page offset/address value interpreted by selected erase case.
 * @param  Case Erase region and bank selection.
 * @retval MEM_Status Status of the erase operation.
 */
MEM_Status MEM_ErasePage(uint32_t page, PageEraseCase Case);

/**
 * @brief  Erases all pages in memory.
 * @retval MEM_Status Status of the mass erase operation.
 */
MEM_Status MEM_MassErase(void);

/**
 * @brief  Programs a double word at a specified address.
 * @param  Address Destination memory address.
 * @param  Data 64-bit data to be programmed.
 * @retval MEM_Status Status of the program operation.
 */
MEM_Status MEM_ProgramDoubleWord(uint32_t Address, uint64_t Data);

/**
 * @brief  Unlocks the memory option bytes register access.
 */
void MEM_OB_Unlock(void);

/**
 * @brief  Gets the current memory status.
 * @retval MEM_Status Current controller status.
 */
MEM_Status MEM_GetStatus(void);

/**
 * @brief  Waits for the last memory operation to be completed.
 * @retval MEM_Status Final status of the waited operation.
 */
MEM_Status MEM_WaitForLastOperation(void);

void MEM_ClearStatus(void);
int MEM_IsEmpty(void);
void MEM_SetACREmptyBit(void);
void MEM_ClearACREmptyBit(void);
void MEM_StartOptionBytesProgramming(void);
void MEM_SetBKSELValue(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32C5xx_MEMORY_HAL_H */
