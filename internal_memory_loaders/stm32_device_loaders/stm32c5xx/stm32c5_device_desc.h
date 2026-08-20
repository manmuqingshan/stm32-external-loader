/**
  **********************************************************************************************************************
  * @file    stm32c5_device_desc.h
  * @brief   STM32C5xx device memory geometry and variant selection.
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

#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>

/* Core memory characteristics shared across STM32C5 devices. */
#define STM32C5_MEM_BASE_ADDR 0x08000000U
#define STM32C5_MEM_PAGE_SIZE 0x2000U
#define STM32C5_MEM_EMPTY_VALUE 0xFFU
#define STM32C5_MEM_PAGE_PROG_TIMEOUT 400U
#define STM32C5_MEM_SECTOR_ERASE_TIMEOUT 400U
#define STM32C5_MEM_SECTOR_SIZE 0x2000U

/* Variant selection (one define must be provided by the build). */
#if defined(STM32C5_0x44E_0x08000000_512K)
#define STM32C5_DEVICE_NAME "STM32C5[56]xx 512K Flash"
#define STM32C5_DEVICE_START_ADDR STM32C5_MEM_BASE_ADDR
#define STM32C5_DEVICE_SIZE 0x80000U
#define STM32C5_DEVICE_MEM_SIZE_KB_MAX 0x200U
#elif defined(STM32C5_0x44F_0x08000000_256K)
#define STM32C5_DEVICE_NAME "STM32C5[34]xx 256K Flash"
#define STM32C5_DEVICE_START_ADDR STM32C5_MEM_BASE_ADDR
#define STM32C5_DEVICE_SIZE 0x40000U
#define STM32C5_DEVICE_MEM_SIZE_KB_MAX 0x100U
#else
#error "Missing Target Define"
#endif

/* Exported defines (generic loader interface mapping). */
#define DEVICE_NAME STM32C5_DEVICE_NAME
#define DEVICE_START_ADDR STM32C5_DEVICE_START_ADDR
#define DEVICE_SIZE STM32C5_DEVICE_SIZE

#define DEVICE_PROG_PAGE_SIZE STM32C5_MEM_PAGE_SIZE
#define DEVICE_EMPTY_VALUE STM32C5_MEM_EMPTY_VALUE
#define DEVICE_PAGE_PROG_TIMEOUT STM32C5_MEM_PAGE_PROG_TIMEOUT
#define DEVICE_SECTOR_ERASE_TIMEOUT STM32C5_MEM_SECTOR_ERASE_TIMEOUT

/* Internal defines: the device memory is split in sectors of 8K each. */
#define DEVICE_SECTOR_SIZE STM32C5_MEM_SECTOR_SIZE
#define DEVICE_SECTOR_COUNT ((DEVICE_SIZE) / (DEVICE_SECTOR_SIZE))

/* Exported layout description */
#define DEVICE_SECTOR_GROUPS_COUNT 1
#define DEVICE_MEM_LAYOUT() \
	DEVICE_SECTOR_GROUP(0, DEVICE_SECTOR_COUNT, DEVICE_SECTOR_SIZE)


#endif /* DEVICE_H */
