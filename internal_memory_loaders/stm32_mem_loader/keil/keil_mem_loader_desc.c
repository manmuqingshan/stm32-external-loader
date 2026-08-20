/**
 * @file keil_mem_loader_desc.c
 * @brief Device descriptor definition for Keil Flash loader.
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

#include DEVICE_DESC_HEADER
#include "device_loader.h"
#include "keil_mem_loader.h"


#define FLASH_DEVICE_TYPE Keepincompilation struct FlashDevice

#define DEVICE_SECTOR_GROUP(addr, count, size) \
  (size),(0)

/**
 * @brief FlashDevice structure instance for Keil tools.
 * @details Populated using device-specific macros and constants.
 */
FLASH_DEVICE_TYPE FlashDevice = {
    FLASH_DRV_VERS,               // Driver Version, do not modify!
    DEVICE_NAME,                  // Device Name
    ONCHIP,                       // Device Type
    DEVICE_START_ADDR,            // Device Start Address
    DEVICE_SIZE,                  // Device Size in Bytes
    DEVICE_PROG_PAGE_SIZE,        // Programming Page Size
    0,                            // Reserved, must be 0
    DEVICE_EMPTY_VALUE,           // Initial Content of Erased Memory
    DEVICE_PAGE_PROG_TIMEOUT,     // Program Page Timeout
    DEVICE_SECTOR_ERASE_TIMEOUT,  // Erase Sector Timeout
    // Specify Size and Address of Sectors
    DEVICE_MEM_LAYOUT(),
    Keil_SECTOR_END
};
