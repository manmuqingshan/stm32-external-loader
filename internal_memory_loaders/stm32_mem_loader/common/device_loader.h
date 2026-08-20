/**
  **********************************************************************************************************************
  * @file    device_loader.h
  * @brief   Common device loader interface for STM32 memory loaders.
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

#ifndef DEVICE_LOADER_H
#define DEVICE_LOADER_H

#include <stdint.h>


#if defined(__ICCARM__)
#define NO_INIT __no_init
#define Keepincompilation __root
#define STACK_LESS __stackless
extern uint32_t __iar_static_base$$SB;
#define IAR_STATIC_BASE ((unsigned int)&__iar_static_base$$SB)
#else
#define NO_INIT __attribute__((section(".noinit")))
#define STACK_LESS __attribute__((noreturn))
#define Keepincompilation __attribute__((used))
extern const unsigned int Image$$SB$$ZI$$Base;
#define IAR_STATIC_BASE ((unsigned int)&Image$$SB$$ZI$$Base)
#endif
extern const unsigned int CSTACK$$Limit;
#define CSTACK_LIMIT ((unsigned int)&CSTACK$$Limit)

/**
 * @enum mem_loader_status
 * @brief Status codes for memory loader operations.
 */
typedef enum
{
  MEM_LOADER_STATUS_SUCCESS = 0,
  MEM_LOADER_STATUS_FAIL = 1
} mem_loader_status;

/**
 * @brief Initialize the memory loader.
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_init(void);

/**
 * @brief De-initialize the memory loader.
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_deinit(void);

/**
 * @brief Erase a sector in memory.
 * @param start_addr Start address
 * @param end_addr End address
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_sector_erase(uint32_t start_addr, uint32_t end_addr);

/**
 * @brief Program memory.
 * @param start_addr Start address
 * @param size Size in bytes
 * @param buffer Data buffer
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_program(uint32_t start_addr, uint32_t size, uint8_t *buffer);

/**
 * @brief Mass erase memory.
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_mass_erase(void);

/**
 * @brief Verify memory contents against a buffer.
 * @param start_addr Start address
 * @param size Size in bytes
 * @param buffer Reference buffer
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_verify_compare(uint32_t start_addr, uint32_t size, uint8_t *buffer);

/**
 * @brief Write option bytes.
 * @param start_addr Start address
 * @param size Size in bytes
 * @param buffer Data buffer
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_write_option_bytes(uint32_t start_addr, uint32_t size, uint8_t *buffer);

/**
 * @brief Check if memory is blank.
 * @param start_addr Start address
 * @param size Size in bytes
 * @param pattern Pattern to check
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_blank_check(uint32_t start_addr, uint32_t size, uint8_t pattern);

/**
 * @brief Finalize the loader operation.
 * @return Status code ::mem_loader_status
 */
mem_loader_status mem_loader_signoff(void);


#endif /* DEVICE_LOADER_H */
