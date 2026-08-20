/**
  **********************************************************************************************************************
  * @file    mem_loader_common.c
  * @brief   Common entry points for memory loader (ST/Keil) selection and verification.
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

#include <cmsis_compiler.h>
#include "device_loader.h"

extern int ST_Init(void);
extern int Keil_Init(unsigned long adr, unsigned long clk, unsigned long fnc);

extern int ST_Verify(uint32_t start_addr, uint32_t ref_buffer_addr, uint32_t size, uint32_t misalignment);
extern int Keil_Verify(unsigned long adr, unsigned long sz, unsigned char *buf);

/**
 * @brief Calls the appropriate Init function for ST or Keil loader.
 *
 * @param adr Device base address
 * @param clk Clock frequency (Hz)
 * @param fnc Function code
 * @return Loader-specific status
 */
Keepincompilation int Init(unsigned long adr, unsigned long clk, unsigned long fnc)
{
  if (clk == 0 && fnc == 0)
    return ST_Init();

  return Keil_Init(adr, clk, fnc);
}

/**
 * @brief Calls the appropriate Verify function for ST or Keil loader.
 *
 * @param adr Start address
 * @param sz Size in bytes
 * @param buf Reference buffer
 * @param misalignment Misalignment value
 * @return Loader-specific status
 */
Keepincompilation int Verify(unsigned long adr, unsigned long sz, unsigned char *buf, unsigned long misalignment)
{
  /* Route to Keil verify for small regions, ST verify for large regions. */
  if (sz < 0x2000000U)
    return Keil_Verify(adr, sz, buf);

  return ST_Verify((uint32_t)adr, (uint32_t)buf, (uint32_t)sz, (uint32_t)misalignment);
}
