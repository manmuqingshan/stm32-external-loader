/**
  **********************************************************************************************************************
  * @file    stm32c5_device_loader.c
  * @brief   STM32C5xx memory loader implementation (mem_loader_* API).
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
#include "stm32c5_device_desc.h"
#include "stm32c5_memory_controller.h"

/* Clamp FLASHSIZE to the selected device maximum. */
static uint32_t stm32c5_get_mem_size_kb(void)
{
  uint32_t mem_size_kb = (*(volatile uint32_t *)STM32C5_FLASHSIZE_BASE) & STM32C5_FLASHSIZE_MASK;

  if ((mem_size_kb == STM32C5_FLASHSIZE_INVALID) ||
      (mem_size_kb > STM32C5_DEVICE_MEM_SIZE_KB_MAX))
  {
    mem_size_kb = STM32C5_DEVICE_MEM_SIZE_KB_MAX;
  }

  return mem_size_kb;
}

static uint64_t stm32c5_read_u64_le(const uint8_t *buffer)
{
  uint64_t value = 0U;

  for (uint32_t offset = 0U; offset < STM32C5_FLASH_DOUBLE_WORD_SIZE; offset++)
  {
    value |= ((uint64_t)buffer[offset] << (offset * 8U));
  }

  return value;
}

static uint32_t stm32c5_read_u32_le(const uint8_t *buffer)
{
  return ((uint32_t)buffer[0]) |
         ((uint32_t)buffer[1] << 8U) |
         ((uint32_t)buffer[2] << 16U) |
         ((uint32_t)buffer[3] << 24U);
}

typedef mem_loader_status (*stm32c5_program_block_cb)(uint32_t address,
                                                      const uint8_t *data,
                                                      uint32_t size,
                                                      void *context);

static mem_loader_status stm32c5_program_mem_block(uint32_t address,
                                                     const uint8_t *data,
                                                     uint32_t size,
                                                     void *context)
{
  MEM_Status status;

  (void)context;

  if (size != STM32C5_FLASH_PROGRAM_BLOCK_SIZE)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  status = MEM_ProgramDoubleWord(address, stm32c5_read_u64_le(data));
  if (status == MEM_COMPLETE)
  {
    status = MEM_ProgramDoubleWord(address + STM32C5_FLASH_DOUBLE_WORD_SIZE,
                                     stm32c5_read_u64_le(data + STM32C5_FLASH_DOUBLE_WORD_SIZE));
  }

  return (status == MEM_COMPLETE) ? MEM_LOADER_STATUS_SUCCESS : MEM_LOADER_STATUS_FAIL;
}

static mem_loader_status stm32c5_program_blocks(uint32_t start_addr,
                                                uint32_t size,
                                                uint8_t *buffer,
                                                uint32_t block_size,
                                                uint8_t fill_value,
                                                stm32c5_program_block_cb program_block,
                                                void *context)
{
  uint8_t tail[STM32C5_FLASH_PROGRAM_BLOCK_SIZE];
  uint32_t offset;

  if ((block_size == 0U) || (block_size > STM32C5_FLASH_PROGRAM_BLOCK_SIZE) ||
      (program_block == (stm32c5_program_block_cb)0))
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  while (size >= block_size)
  {
    if (program_block(start_addr, buffer, block_size, context) != MEM_LOADER_STATUS_SUCCESS)
    {
      return MEM_LOADER_STATUS_FAIL;
    }

    start_addr += block_size;
    buffer += block_size;
    size -= block_size;
  }

  if (size > 0U)
  {
    for (offset = 0U; offset < size; offset++)
    {
      tail[offset] = buffer[offset];
    }

    for (; offset < block_size; offset++)
    {
      tail[offset] = fill_value;
    }

    if (program_block(start_addr, tail, block_size, context) != MEM_LOADER_STATUS_SUCCESS)
    {
      return MEM_LOADER_STATUS_FAIL;
    }
  }

  return MEM_LOADER_STATUS_SUCCESS;
}

static mem_loader_status stm32c5_compare_memory(uint32_t start_addr, uint32_t size, const uint8_t *buffer)
{
  volatile const uint8_t *memory = (volatile const uint8_t *)start_addr;

  for (uint32_t offset = 0U; offset < size; offset++)
  {
    if (memory[offset] != buffer[offset])
    {
      return MEM_LOADER_STATUS_FAIL;
    }
  }

  return MEM_LOADER_STATUS_SUCCESS;
}

static mem_loader_status stm32c5_blank_check_memory(uint32_t start_addr, uint32_t size, uint8_t pattern)
{
  volatile const uint8_t *memory = (volatile const uint8_t *)start_addr;

  for (uint32_t offset = 0U; offset < size; offset++)
  {
    if (memory[offset] != pattern)
    {
      return MEM_LOADER_STATUS_FAIL;
    }
  }

  return MEM_LOADER_STATUS_SUCCESS;
}

/**
 * @brief Check memory programming error status flags.
 * @return MEM_LOADER_STATUS_FAIL if a programming error is set, success otherwise.
 */
static inline mem_loader_status MEM_CheckProgramError(void)
{
  return (FLASH->CR & FLASH_PGERR) ? MEM_LOADER_STATUS_FAIL : MEM_LOADER_STATUS_SUCCESS;
}

/**
 * @brief Initialize memory loader state and unlock memory control.
 * @return Operation status.
 */
mem_loader_status mem_loader_init(void)
{
  __disable_irq();
  MEM_ClearStatus();
  MEM_Unlock();

  return MEM_CheckProgramError();
}

/**
 * @brief Deinitialize memory loader state.
 * @return Operation status.
 */
mem_loader_status mem_loader_deinit(void)
{
  return MEM_CheckProgramError();
}

/**
 * @brief Erase sectors in the selected memory address range.
 * @param start_addr Start erase address.
 * @param end_addr End erase address.
 * @return Operation status.
 */
mem_loader_status mem_loader_sector_erase(uint32_t start_addr, uint32_t end_addr)
{
  MEM_Status status;
  uint32_t gMemSize;
  uint32_t MemSize;
  uint32_t MemEndAddr;
  uint32_t MemBankSize;
  uint32_t page;
  PageEraseCase Case;

  /* Keep same IRQ policy as other memory operations during erase sequence. */
  __disable_irq();

  /* Invalid range guard: start must not be greater than end. */
  if (start_addr > end_addr)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  /* Ensure controller is idle before starting first erase. */
  status = MEM_WaitForLastOperation();
  if (status != MEM_COMPLETE)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  /* Read FLASHSIZE register value (in KBytes) with device-specific guard. */
  gMemSize = stm32c5_get_mem_size_kb();

  /* Convert KB to bytes and derive bank boundaries in the main flash window. */
  MemSize = gMemSize * STM32C5_KB_SIZE;
  MemBankSize = MemSize >> 1U;
  MemEndAddr = STM32C5_FLASH_BASE_ADDR1 + MemSize;

  while (start_addr <= end_addr)
  {
    /*
     * Condition A: Main user flash (0x0800_0000 .. FLASH_BASE_ADDR1 + MemSize).
     * - Split into bank0/bank1 around half flash size.
     * - Erase step: 0x2000 (8KB user flash page granularity).
     */
    if ((STM32C5_FLASH_BASE_ADDR1 <= start_addr) && (start_addr < MemEndAddr))
    {
      if (start_addr < (STM32C5_FLASH_BASE_ADDR1 + MemBankSize))
      {
        /* First bank of main flash. */
        Case = USER_MEM_BANK0;
        page = start_addr - STM32C5_FLASH_BASE_ADDR1;
      }
      else
      {
        /* Upper Second of main flash. */
        Case = USER_MEM_BANK1;
        page = start_addr - STM32C5_FLASH_BASE_ADDR1 - MemBankSize;
      }
      start_addr += STM32C5_USER_FLASH_PAGE_SIZE;
    }
    /*
     * Condition B: EDATA-disabled alias area (0x0840_0000 .. +64KB).
     * - Two 32KB banks in this alias region.
     * - Erase step: 0x800 (2KB page granularity).
     */
    else if ((start_addr >= STM32C5_FLASH_BASE_ADDR2) &&
             (start_addr < (STM32C5_FLASH_BASE_ADDR2 + STM32C5_EDATA_DISABLED_SIZE)))
    {
      if (start_addr < (STM32C5_FLASH_BASE_ADDR2 + STM32C5_EDATA_DISABLED_BANK_SIZE))
      {
        /* First bank of alias area. */
        Case = EDATA_DISABLED_BANK0;
        page = start_addr - STM32C5_FLASH_BASE_ADDR2;
      }
      else
      {
        /* Second bank of alias area. */
        Case = EDATA_DISABLED_BANK1;
        page = start_addr - STM32C5_FLASH_BASE_ADDR2 - STM32C5_EDATA_DISABLED_BANK_SIZE;
      }
      start_addr += STM32C5_EDATA_DISABLED_PAGE_SIZE;
    }
    /*
     * Condition C: EDATA-enabled area (0x0900_0000 ..).
     * - Bank split at +0x6000.
     * - Erase step: 0x600 (1.5KB erase granularity).
     */
    else
    {
      if (start_addr < (STM32C5_EDATA_BASE_ADDR + STM32C5_EDATA_ENABLED_BANK_SIZE))
      {
        /* First EDATA bank. */
        Case = EDATA_ENABLED_BANK0;
        page = start_addr - STM32C5_EDATA_BASE_ADDR;
      }
      else
      {
        /* Second EDATA bank. */
        Case = EDATA_ENABLED_BANK1;
        page = start_addr - STM32C5_EDATA_BASE_ADDR - STM32C5_EDATA_ENABLED_BANK_SIZE;
      }
      start_addr += STM32C5_EDATA_ENABLED_PAGE_SIZE;
    }

    /* Wait for idle then trigger page erase for selected region + page offset. */
    MEM_WaitForLastOperation();
    status = MEM_ErasePage(page, Case);
    if (status != MEM_COMPLETE)
    {
      return MEM_LOADER_STATUS_FAIL;
    }
  }

  /* Return final program/erase error flag state from controller. */
  return MEM_CheckProgramError();
}

/**
 * @brief Program memory content from a source buffer.
 * @param start_addr Target start address.
 * @param size Number of bytes to program.
 * @param buffer Source data buffer.
 * @return Operation status.
 */
mem_loader_status mem_loader_program(uint32_t start_addr, uint32_t size, uint8_t *buffer)
{
  mem_loader_status status;

  __disable_irq();

  status = stm32c5_program_blocks(start_addr,
                                  size,
                                  buffer,
                                  STM32C5_FLASH_PROGRAM_BLOCK_SIZE,
                                  DEVICE_EMPTY_VALUE,
                                  stm32c5_program_mem_block,
                                  (void *)0);
  if (status != MEM_LOADER_STATUS_SUCCESS)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  if (!MEM_IsEmpty())
  {
    MEM_ClearACREmptyBit();
  }
  return MEM_CheckProgramError();
}

/**
 * @brief Erase the full user memory area.
 * @return Operation status.
 */
mem_loader_status mem_loader_mass_erase(void)
{
  __disable_irq();
  MEM_Status Operation_Status;
  Operation_Status = MEM_MassErase();
  if (Operation_Status != MEM_COMPLETE)
  {
    return MEM_LOADER_STATUS_FAIL;
  }
  if (MEM_IsEmpty())
  {
    MEM_SetACREmptyBit();
  }
  return MEM_CheckProgramError();
}

/**
 * @brief Verify memory data against reference bytes.
 * @param start_addr Start address.
 * @param size Number of bytes.
 * @param buffer Reference buffer.
 * @return Operation status.
 */
mem_loader_status mem_loader_verify_compare(uint32_t start_addr, uint32_t size, uint8_t *buffer)
{
  if (stm32c5_compare_memory(start_addr, size, buffer) != MEM_LOADER_STATUS_SUCCESS)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  return MEM_CheckProgramError();
}

/**
 * @brief Program option bytes from caller buffer.
 * @param start_addr Start option-byte address.
 * @param size Number of bytes to write.
 * @param buffer Source buffer.
 * @return Operation status.
 */
mem_loader_status mem_loader_write_option_bytes(uint32_t start_addr, uint32_t size, uint8_t *buffer)
{
  __disable_irq();
  MEM_Status status;

  if (size == 0U)
  {
    return MEM_LOADER_STATUS_SUCCESS;
  }

  if ((start_addr < STM32C5_OB_DIRECT_WRITE_START) ||
      (start_addr >= STM32C5_OB_DIRECT_WRITE_END))
  {
    if (size <= STM32C5_OB_WORD_SIZE)
    {
      return MEM_LOADER_STATUS_SUCCESS;
    }

    start_addr += STM32C5_OB_WORD_SIZE;
    buffer += STM32C5_OB_WORD_SIZE;
    size -= STM32C5_OB_WORD_SIZE;
  }

  /* Clear OPTLOCK option lock bit with the clearing sequence. */
  MEM_OB_Unlock();

  status = MEM_WaitForLastOperation();
  if (status != MEM_COMPLETE)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  while (size >= STM32C5_OB_WORD_SIZE)
  {
    const uint32_t option_word = stm32c5_read_u32_le(buffer);

    *(__IO uint32_t *)start_addr = option_word;

    status = MEM_WaitForLastOperation();
    if (status != MEM_COMPLETE)
    {
      return MEM_LOADER_STATUS_FAIL;
    }

    if (size <= STM32C5_OB_WRITE_STRIDE)
    {
      break;
    }

    size -= STM32C5_OB_WRITE_STRIDE;
    start_addr += STM32C5_OB_WRITE_STRIDE;
    buffer += STM32C5_OB_WRITE_STRIDE;
  }

  MEM_StartOptionBytesProgramming();

  status = MEM_WaitForLastOperation();
  if (status != MEM_COMPLETE)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  return MEM_LOADER_STATUS_SUCCESS;
}

/**
 * @brief Check a range against blank pattern.
 * @param start_addr Start address.
 * @param size Number of bytes.
 * @param pattern Expected blank pattern.
 * @return Operation status.
 */
mem_loader_status mem_loader_blank_check(uint32_t start_addr, uint32_t size, uint8_t pattern)
{
  if (stm32c5_blank_check_memory(start_addr, size, pattern) != MEM_LOADER_STATUS_SUCCESS)
  {
    return MEM_LOADER_STATUS_FAIL;
  }

  return MEM_CheckProgramError();
}

/**
 * @brief Finalize loader operation.
 * @return Operation status.
 */
mem_loader_status mem_loader_signoff(void)
{
  return MEM_CheckProgramError();
}

