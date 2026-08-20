/**
 * @file keil_mem_loader.h
 * @brief Data structures and function prototypes for Keil Flash loader.
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

#define VERS 1 // Interface Version 1.01

#define UNKNOWN 0  // Unknown
#define ONCHIP 1   // On-chip Flash Memory
#define EXT8BIT 2  // External Flash Device on 8-bit  Bus
#define EXT16BIT 3 // External Flash Device on 16-bit Bus
#define EXT32BIT 4 // External Flash Device on 32-bit Bus
#define EXTSPI 5   // External Flash Device on SPI

#define Keil_SECTOR_NUM 512 // Max Number of Sector Items
#define Keil_SECTOR_END 0xFFFFFFFF, 0xFFFFFFFF
#define PAGE_MAX 65536 // Max Page Size for Programming
#define FLASH_DRV_VERS (0x0100 + VERS) // Driver Version, do not modify!

/**
 * @struct FlashSectors
 * @brief Structure describing a flash sector.
 */
struct FlashSectors
{
    unsigned long szSector;   // Sector Size in Bytes
    unsigned long AddrSector; // Address of Sector
};

/**
 * @struct FlashDevice
 * @brief Structure describing a flash device for Keil tools.
 */
struct FlashDevice
{
    unsigned short Vers;    // Version Number and Architecture
    char DevName[128];      // Device Name and Description
    unsigned short DevType; // Device Type: ONCHIP, EXT8BIT, EXT16BIT, ...
    unsigned long DevAdr;   // Default Device Start Address
    unsigned long szDev;    // Total Size of Device
    unsigned long szPage;   // Programming Page Size
    unsigned long Res;      // Reserved for future Extension
    unsigned char valEmpty; // Content of Erased Memory

    unsigned long toProg;  // Time Out of Program Page Function
    unsigned long toErase; // Time Out of Erase Sector Function

    struct FlashSectors sectors[Keil_SECTOR_NUM];
};


// Flash Programming Functions (Called by FlashOS)
/**
 * @brief Initialize Flash
 * @param adr Device Base Address
 * @param clk Clock Frequency (Hz)
 * @param fnc Function Code
 * @return 0 - OK, 1 - Failed
 */
extern int Init(unsigned long adr, // Initialize Flash
                unsigned long clk,
                unsigned long fnc);
/**
 * @brief De-initialize Flash
 * @param fnc Function Code
 * @return 0 - OK, 1 - Failed
 */
extern int UnInit(unsigned long fnc);    // De-initialize Flash
/**
 * @brief Blank Check
 * @param adr Start Address
 * @param sz Size
 * @param pat Pattern
 * @return 0 - OK, 1 - Failed
 */
extern int BlankCheck(unsigned long adr, // Blank Check
                      unsigned long sz,
                      unsigned char pat);
/**
 * @brief Erase complete Device
 * @return 0 - OK, 1 - Failed
 */
extern int EraseChip(void);                // Erase complete Device
/**
 * @brief Erase Sector Function
 * @param adr Sector Address
 * @return 0 - OK, 1 - Failed
 */
extern int EraseSector(unsigned long adr); // Erase Sector Function
/**
 * @brief Program Page Function
 * @param adr Page Start Address
 * @param sz Page Size
 * @param buf Page Data
 * @return 0 - OK, 1 - Failed
 */
extern int ProgramPage(unsigned long adr,  // Program Page Function
                       unsigned long sz,
                       unsigned char *buf);
/**
 * @brief Verify Function
 * @param adr Start Address
 * @param sz Size
 * @param buf Reference Buffer
 * @return Address of first mismatch, or end address if all match
 */
extern unsigned long Verify(unsigned long adr, // Verify Function
                            unsigned long sz,
                            unsigned char *buf);
