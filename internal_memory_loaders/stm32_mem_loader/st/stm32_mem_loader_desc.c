/**
  **********************************************************************************************************************
  * @file    stm32_mem_loader_desc.c
  * @brief   Device descriptor instance for STM32 memory loader (ST).
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
#include DEVICE_DESC_HEADER
#include "device_loader.h"
#include "stm32_mem_loader_desc.h"


#if defined(__ICCARM__)
#define STORAGE_INFO_TYPE Keepincompilation struct StorageInfo const
#else
#define STORAGE_INFO_TYPE Keepincompilation struct StorageInfo const
#endif

#define DEVICE_SECTOR_GROUP(addr, count, size) \
  (count), (size)

/* This structure contains information used by STM32CubeProgrammer to program and erase the device */
STORAGE_INFO_TYPE StorageInfo = {
    DEVICE_NAME,
    MCU_FLASH,
    DEVICE_START_ADDR,
    DEVICE_SIZE,
    DEVICE_PROG_PAGE_SIZE,
    DEVICE_EMPTY_VALUE,
    // describe sectors topology
    DEVICE_MEM_LAYOUT(), ST_SECTOR_END};

