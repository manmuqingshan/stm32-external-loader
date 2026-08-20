/**
  **********************************************************************************************************************
  * @file    stm32c5_memory_controller.c
  * @brief   Source file for STM32C5 memory controller helpers.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32c5_memory_controller.h"

/* External declarations-----------------------------------------------------*/
extern void __iar_data_init3(void);

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Unlocks the memory control register access.
  * @retval None
  */
void MEM_Unlock(void)
{
    if ((FLASH->CR & FLASH_CR_LOCK) != RESET)
    {
        /* Authorize the memory registers access */
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

/**
  * @brief  Erases a specified memory page.
    * @param  page Page offset/address value interpreted by selected erase case.
    * @param  Case Erase region and bank selection.
  * @retval Memory status: The returned value can be: MEM_BUSY, MEM_ERROR_PROGRAM,
  *                       MEM_ERROR_WRP, MEM_ERROR_OPERATION or MEM_COMPLETE.
  */

MEM_Status MEM_ErasePage(uint32_t page, PageEraseCase Case)
{
    uint32_t Page = page & STM32C5_FLASH_PAGE_OFFSET_MASK;
    MEM_Status status = MEM_WaitForLastOperation();

    if (status == MEM_COMPLETE)
    {
        /* Configure erase region and bank selection. */
        switch (Case)
        {
            case USER_MEM_BANK0:
                FLASH->CR &= ~FLASH_CR_EDATASEL;
                FLASH->CR &= ~(PAGE_MASK << 6U);
                FLASH->CR |= FLASH_CR_PER | (Page >> 7U);
                FLASH->CR &= ~FLASH_CR_BKSEL;
                break;

            case USER_MEM_BANK1:
                FLASH->CR &= ~FLASH_CR_EDATASEL;
                FLASH->CR &= ~(PAGE_MASK << 6U);
                FLASH->CR |= FLASH_CR_PER | (Page >> 7U);
                FLASH->CR |= FLASH_CR_BKSEL;
                break;

            case EDATA_DISABLED_BANK0:
                FLASH->CR |= FLASH_CR_EDATASEL;
                FLASH->CR &= ~(PAGE_MASK << 6U);
                FLASH->CR |= FLASH_CR_PER | (Page >> 5U);
                FLASH->CR &= ~FLASH_CR_BKSEL;
                break;

            case EDATA_DISABLED_BANK1:
                FLASH->CR |= FLASH_CR_EDATASEL;
                FLASH->CR &= ~(PAGE_MASK << 6U);
                FLASH->CR |= FLASH_CR_PER | (Page >> 5U);
                FLASH->CR |= FLASH_CR_BKSEL;
                break;

            case EDATA_ENABLED_BANK0:
                FLASH->CR |= FLASH_CR_EDATASEL;
                FLASH->CR &= ~(PAGE_MASK << 6U);
                FLASH->CR |= FLASH_CR_PER | ((Page >> 3U) / 3U);
                FLASH->CR &= ~FLASH_CR_BKSEL;
                break;

            case EDATA_ENABLED_BANK1:
                FLASH->CR |= FLASH_CR_EDATASEL;
                FLASH->CR &= ~(PAGE_MASK << 6U);
                FLASH->CR |= FLASH_CR_PER | ((Page >> 3U) / 3U);
                FLASH->CR |= FLASH_CR_BKSEL;
                break;

            default:
                status = MEM_ERROR_OPERATION;
                break;
        }

        MEM_SetBKSELValue();
        FLASH->CR |= FLASH_CR_STRT;

        /* Wait for last operation to be completed */
        status = MEM_WaitForLastOperation();

        /* If the erase operation is completed, disable the PER and BKER Bits */
        FLASH->CR &= ~FLASH_CR_PER;
        FLASH->CR &= ~(PAGE_MASK << 6U);
        status = MEM_WaitForLastOperation();
    }
    /* Return the Erase Status */
    return status;
}

/**
  * @brief  Erases all memory pages.
  * @retval Memory status: The returned value can be: MEM_BUSY, MEM_ERROR_PROGRAM,
  *                       MEM_ERROR_WRP, MEM_ERROR_OPERATION or MEM_COMPLETE.
  */
MEM_Status MEM_MassErase(void)
{
    MEM_Status status = MEM_WaitForLastOperation();

    if (status == MEM_COMPLETE)
    {
        FLASH->SR = FLASH_PGERR;
        /* If the previous operation is completed, proceed to erase all pages */
        FLASH->CR &= ~FLASH_CR_EDATASEL;
        FLASH->CR |= FLASH_CR_MER;
        FLASH->CR |= FLASH_CR_STRT;

        /* Wait for last operation to be completed */
        status = MEM_WaitForLastOperation();

        /* If the erase operation is completed, disable the MER Bits */
        FLASH->CR &= ~FLASH_CR_MER;
    }
    /* Return the Erase Status */
    return status;
}

/**
  * @brief  Programs a double word (64-bit) at a specified address.
  * @param  Address: Specifies the address to be programmed.
  * @param  Data: Specifies the data to be programmed.
  * @retval Memory status: The returned value can be: MEM_BUSY, MEM_ERROR_PROGRAM,
  *                       MEM_ERROR_WRP, MEM_ERROR_OPERATION or MEM_COMPLETE.
  */
MEM_Status MEM_ProgramDoubleWord(uint32_t Address, uint64_t Data)
{
    MEM_Status status = MEM_WaitForLastOperation();

    if (status == MEM_COMPLETE)
    {
        /* If the previous operation is completed, proceed to program the new data */
        FLASH->SR = FLASH_PGERR;
        FLASH->CR |= FLASH_CR_PG;
        *(__IO uint32_t *)Address = (uint32_t)Data;
        *(__IO uint32_t *)(Address + 4U) = (uint32_t)(Data >> 32U);

        /* Wait for last operation to be completed */
        status = MEM_WaitForLastOperation();

        /* If the program operation is completed, disable the PG Bit */
        FLASH->CR &= ~FLASH_CR_PG;
    }
    /* Return the Program Status */
    return status;
}

/**
  * @brief  Unlocks the memory option control registers access.
  * @retval None
  */
void MEM_OB_Unlock(void)
{
    if ((FLASH->OPTCR & FLASH_OPTCR_OPTLOCK) != RESET)
    {
        /* Authorizes the Option Byte register programming */
        FLASH->OPTKEYR = FLASH_OPT_KEY1;
        FLASH->OPTKEYR = FLASH_OPT_KEY2;
    }
}

/**
  * @brief  Returns the memory status.
  * @retval Memory status: The returned value can be: MEM_BUSY, MEM_ERROR_PROGRAM,
  *                       MEM_ERROR_WRP, MEM_ERROR_RD, MEM_ERROR_OPERATION or MEM_COMPLETE.
  */
MEM_Status MEM_GetStatus(void)
{
    MEM_Status memstatus = MEM_COMPLETE;

    if ((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY)
    {
        memstatus = MEM_BUSY;
    }
    else
    {
        if ((FLASH->SR & FLASH_PGERR) != 0U)
        {
            memstatus = MEM_ERROR_OPERATION;
        }
    }
    /* Return the memory status */
    return memstatus;
}

/**
  * @brief  Waits for a memory operation to complete.
  * @retval Memory status: The returned value can be: MEM_BUSY, MEM_ERROR_PROGRAM,
  *                       MEM_ERROR_WRP, MEM_ERROR_OPERATION or MEM_COMPLETE.
  */
MEM_Status MEM_WaitForLastOperation(void)
{
    MEM_Status status = MEM_GetStatus();

    /* Wait for the memory operation to complete by polling on BUSY flag to be reset.
         Even if the memory operation fails, the BUSY flag will be reset and an error
         flag will be set */
    while (status == MEM_BUSY)
    {
        status = MEM_GetStatus();
    }
    /* Return the operation status */
    return status;
}

/**
    * @brief  Clear all relevant memory operation flags.
    * @retval None
    */
void MEM_ClearStatus(void) {
    FLASH->CCR = FLASH_PGERR;
}

/**
    * @brief  Check whether first user memory word is erased.
    * @retval 1 if erased, 0 otherwise.
    */
int MEM_IsEmpty(void) {
    return (*(volatile uint32_t *)STM32C5_FLASH_BASE_ADDR1) == STM32C5_FLASH_ERASED_WORD;
}

/**
    * @brief  Set empty indication bit in ACR register.
    * @retval None
    */
void MEM_SetACREmptyBit(void) {
    FLASH->ACR |= STM32C5_ACR_EMPTY_BIT;
}

/**
    * @brief  Clear empty indication bit in ACR register.
    * @retval None
    */
void MEM_ClearACREmptyBit(void) {
    FLASH->ACR &= STM32C5_ACR_EMPTY_CLEAR_MASK;
}

/**
    * @brief  Start option-byte programming sequence.
    * @retval None
    */
void MEM_StartOptionBytesProgramming(void) {
    FLASH->OPTCR |= FLASH_OPTCR_OPTSTRT;
}

/**
    * @brief  Adapt BKSEL according to bank swap option.
    * @retval None
    */
void MEM_SetBKSELValue(void) {
    /* Toggle BKSEL when bank swap is active to target the logical bank. */
    if ((FLASH->OPTSR & FLASH_OPTSR_SWAP_BANK) == FLASH_OPTSR_SWAP_BANK)
    {
        FLASH->CR ^= FLASH_CR_BKSEL;
    }
    MEM_WaitForLastOperation();
}
