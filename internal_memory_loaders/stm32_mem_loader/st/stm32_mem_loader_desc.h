/**
  **********************************************************************************************************************
  * @file    stm32_mem_loader_desc.h
  * @brief   Device descriptor structures for STM32 memory loader (ST).
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


#ifndef ST_MEM_LOADER_DESC_H
#define ST_MEM_LOADER_DESC_H

#include DEVICE_DESC_HEADER

#define MCU_FLASH    1
#define NAND_FLASH   2
#define NOR_FLASH    3
#define SRAM         4
#define PSRAM        5
#define PC_CARD      6
#define SPI_FLASH    7
#define I2C_FLASH    8
#define SDRAM        9
#define I2C_EEPROM   10

#define ST_SECTOR_NUM 10
#define ST_SECTOR_END 0, 0

/**
 * @brief Describes one group of identically-sized sectors.
 */
typedef struct
{
  unsigned long SectorNum;  /* Number of sectors in this group */
  unsigned long SectorSize; /* Size of each sector in bytes    */
} DeviceSectors;

/**
 * @brief Device descriptor consumed by STM32CubeProgrammer.
 */
struct StorageInfo
{
  char          DeviceName[100];            /* Device name and description     */
  unsigned short DeviceType;                /* MCU_FLASH, NAND_FLASH, etc.     */
  unsigned long DeviceStartAddress;         /* Default start address           */
  unsigned long DeviceSize;                 /* Total device size in bytes      */
  unsigned long PageSize;                   /* Programming page size in bytes  */
  unsigned char EraseValue;                 /* Byte value of erased memory     */
  DeviceSectors sectors[ST_SECTOR_NUM];
};

#endif /* ST_MEM_LOADER_DESC_H */
