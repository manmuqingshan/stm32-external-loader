/**
  **********************************************************************************************************************
  * @file    stm32_mem_loader.c
  * @brief   ST-specific memory loader implementation for STM32 devices.
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

#define ST_MEM_LOADER_STATUS_SUCCESS 1
#define ST_MEM_LOADER_STATUS_FAIL 0

#define TRANSLATE_EXIT_STATUS(STATUS_CODE) \
  ((STATUS_CODE) == MEM_LOADER_STATUS_SUCCESS ? ST_MEM_LOADER_STATUS_SUCCESS : ST_MEM_LOADER_STATUS_FAIL)

/**
 * @brief Initialize the ST memory loader.
 * @return 1 on success, 0 on failure
 */
Keepincompilation int ST_Init(void)
{
  int status = mem_loader_init();
  return TRANSLATE_EXIT_STATUS(status);
}

/**
 * @brief Program memory at the specified address.
 * @param start_addr Start address
 * @param size Size in bytes
 * @param buffer Data buffer
 * @return 1 on success, 0 on failure
 */
Keepincompilation int Write(uint32_t start_addr, uint32_t size, uint8_t *buffer)
{
  int status = mem_loader_program(start_addr, size, buffer);
  return TRANSLATE_EXIT_STATUS(status);
}

/**
 * @brief Mass erase the memory.
 * @param parallelism Not used
 * @return 1 on success, 0 on failure
 */
Keepincompilation int MassErase(uint32_t parallelism)
{
  int status = mem_loader_mass_erase();
  return TRANSLATE_EXIT_STATUS(status);
}

/**
 * @brief Erase a sector in memory.
 * @param start_addr Start address
 * @param end_addr End address
 * @param orca_type Not used
 * @param dual_bank Not used
 * @return 1 on success, 0 on failure
 */
Keepincompilation int SectorErase(uint32_t start_addr, uint32_t end_addr, uint32_t orca_type, uint32_t dual_bank)
{
  int status = mem_loader_sector_erase(start_addr, end_addr);
  return TRANSLATE_EXIT_STATUS(status);
}

/**
 * @brief Write option bytes.
 * @param start_addr Start address
 * @param size Size in bytes
 * @param buffer Data buffer
 * @return 1 on success, 0 on failure
 */
Keepincompilation int WriteOB(uint32_t start_addr, uint32_t size, uint8_t *buffer)
{
  int status = mem_loader_write_option_bytes(start_addr, size, buffer);
  return TRANSLATE_EXIT_STATUS(status);
}

/**
 * @brief Calculate checksum over a memory region.
 * @param start_addr Start address
 * @param size Size in bytes
 * @param init_val Initial checksum value
 * @return Calculated checksum
 */
Keepincompilation uint32_t CheckSum(uint32_t start_addr, uint32_t size, uint32_t init_val)
{
  uint8_t misalignment_head = (uint8_t)(start_addr % 4U);
  uint8_t misalignment_tail = (uint8_t)(size % 4U);

  start_addr -= start_addr % 4U;
  size += (size % 4U == 0U) ? 0U : 4U - (size % 4U);

  for (uint32_t cnt = 0U; cnt < size; cnt += 4U)
  {
    uint32_t val = *(const uint32_t *)start_addr;

    if (misalignment_head != 0U)
    {
      switch (misalignment_head & 0xFU)
      {
        case 1U:
          init_val += (uint8_t)((val >> 8U)  & 0xFFU);
          init_val += (uint8_t)((val >> 16U) & 0xFFU);
          init_val += (uint8_t)((val >> 24U) & 0xFFU);
          misalignment_head -= 1U;
          break;
        case 2U:
          init_val += (uint8_t)((val >> 16U) & 0xFFU);
          init_val += (uint8_t)((val >> 24U) & 0xFFU);
          misalignment_head -= 2U;
          break;
        case 3U:
          init_val += (uint8_t)((val >> 24U) & 0xFFU);
          misalignment_head -= 3U;
          break;
        default:
          break;
      }
    }
    else if (((size - misalignment_tail) % 4U != 0U) && ((size - cnt) <= 4U))
    {
      switch ((size - misalignment_tail) & 0xFU)
      {
        case 1U:
          init_val += (uint8_t)(val & 0xFFU);
          init_val += (uint8_t)((val >> 8U)  & 0xFFU);
          init_val += (uint8_t)((val >> 16U) & 0xFFU);
          misalignment_tail -= 1U;
          break;
        case 2U:
          init_val += (uint8_t)(val & 0xFFU);
          init_val += (uint8_t)((val >> 8U) & 0xFFU);
          misalignment_tail -= 2U;
          break;
        case 3U:
          init_val += (uint8_t)(val & 0xFFU);
          misalignment_tail -= 3U;
          break;
        default:
          break;
      }
    }
    else
    {
      init_val += (uint8_t)(val         & 0xFFU);
      init_val += (uint8_t)((val >> 8U)  & 0xFFU);
      init_val += (uint8_t)((val >> 16U) & 0xFFU);
      init_val += (uint8_t)((val >> 24U) & 0xFFU);
    }

    start_addr += 4U;
  }

  return init_val;
}

/**
 * @brief Verify memory contents against a reference buffer.
 * @param start_addr Start address
 * @param ref_buffer_addr Reference buffer address
 * @param size Size in words
 * @param misalignment Misalignment value
 * @return 64-bit value: upper 32 bits = checksum, lower 32 bits = mismatch address (0 if all match)
 */
Keepincompilation uint64_t ST_Verify(uint32_t start_addr, uint32_t ref_buffer_addr, uint32_t size, uint32_t misalignment)
{
  uint32_t checksum_start_addr = start_addr + (misalignment & 0xFU);
  uint32_t checksum_size = size * 4U - ((misalignment >> 16U) & 0xFU) - (misalignment & 0xFU);

  uint64_t checksum = CheckSum(checksum_start_addr, checksum_size, 0);

  /* Compare memory content word by word. */
  uint32_t mismatch_addr = 0U;
  for (uint32_t cursor = 0; cursor < size * 4; cursor += 8)
  {
    if (*(uint64_t *)start_addr != *(uint64_t *)ref_buffer_addr)
    {
      mismatch_addr = ((uint32_t)start_addr);
      break;
    }

    start_addr += 8;
    ref_buffer_addr += 8;
  }

  return ((checksum << 32) + mismatch_addr);
}

/**
 * @brief De-initialize the ST memory loader.
 * @return 1 on success, 0 on failure
 */
Keepincompilation int DeInit(void)
{
  int status = mem_loader_deinit();
  return TRANSLATE_EXIT_STATUS(status);
}
