/**
 * @file keil_mem_loader.c
 * @brief Flash Programming Functions adapted for Keil Device Flash.
 * @author Arm Limited
 * @date 10 January 2018
 * 
 * Copyright (c) 2010-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "keil_mem_loader.h"
#include "device_loader.h"
#include DEVICE_DESC_HEADER
/*
   Mandatory Flash Programming Functions (Called by FlashOS):
                int Init        (unsigned long adr,   // Initialize Flash
                                 unsigned long clk,
                                 unsigned long fnc);
                int UnInit      (unsigned long fnc);  // De-initialize Flash
                int EraseSector (unsigned long adr);  // Erase Sector Function
                int ProgramPage (unsigned long adr,   // Program Page Function
                                 unsigned long sz,
                                 unsigned char *buf);

   Optional  Flash Programming Functions (Called by FlashOS):
                int BlankCheck  (unsigned long adr,   // Blank Check
                                 unsigned long sz,
                                 unsigned char pat);
                int EraseChip   (void);               // Erase complete Device
      unsigned long Verify      (unsigned long adr,   // Verify Function
                                 unsigned long sz,
                                 unsigned char *buf);

       - BlanckCheck  is necessary if Flash space is not mapped into CPU memory space
       - Verify       is necessary if Flash space is not mapped into CPU memory space
       - if EraseChip is not provided than EraseSector for all sectors is called
*/

/**
 * @brief Initialize Flash Programming Functions
 * @param adr Device Base Address
 * @param clk Clock Frequency (Hz)
 * @param fnc Function Code (1 - Erase, 2 - Program, 3 - Verify)
 * @return 0 - OK, 1 - Failed
 */

int Keil_Init(unsigned long adr, unsigned long clk, unsigned long fnc)
{
  int status = mem_loader_init();
  return status;
}

/**
 * @brief De-Initialize Flash Programming Functions
 * @param fnc Function Code (1 - Erase, 2 - Program, 3 - Verify)
 * @return 0 - OK, 1 - Failed
 */

Keepincompilation int UnInit(unsigned long fnc)
{
  return 0;
}

/**
 * @brief Erase complete Flash Memory
 * @return 0 - OK, 1 - Failed
 */

Keepincompilation int EraseChip(void)
{
  int status = mem_loader_mass_erase();
  return status;
}

/**
 * @brief Erase Sector in Flash Memory
 * @param adr Sector Address
 * @return 0 - OK, 1 - Failed
 */

Keepincompilation int EraseSector(unsigned long adr)
{
  int status = mem_loader_sector_erase(adr, adr + DEVICE_SECTOR_SIZE);
  return status;
}

/**
 * @brief Program Page in Flash Memory
 * @param adr Page Start Address
 * @param sz Page Size
 * @param buf Page Data
 * @return 0 - OK, 1 - Failed
 */

Keepincompilation int ProgramPage(unsigned long adr, unsigned long sz, unsigned char *buf)
{
  int status = mem_loader_program(adr, sz, buf);
  return status;
}

/**
 * @brief Blank Check (not implemented, always returns 1)
 * @param adr Start Address
 * @param sz Size
 * @param pat Pattern
 * @return 1 (force erase)
 */

int BlankCheck(unsigned long adr, unsigned long sz, unsigned char pat)
{
  /* Force erase even if the content matches the erased pattern.
     Only an erased sector can be programmed (ECC constraint). */
  return 1;
}

/**
 * @brief Verify Flash contents against buffer
 * @param adr Start Address
 * @param sz Size
 * @param buf Reference Buffer
 * @return Address of first mismatch, or end address if all match
 */

Keepincompilation unsigned long Keil_Verify(unsigned long adr, unsigned long sz, unsigned char *buf)
{
  while (sz-- > 0)
  {
    if (*(const char *)adr++ != *(const char *)buf++)
      return adr;
  }
  return adr;
}
