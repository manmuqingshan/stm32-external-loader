//------------------------------------------------------------------------------
//
// Copyright (c) 2008-2015 IAR Systems
//
// Licensed under the Apache License, Version 2.0 (the "License")
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// $Revision: 38952 $
//
//------------------------------------------------------------------------------

#include "ewarm_mem_loader.h"
#include "mem_loader_extra.h"
#include "device_loader.h"

uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[])
{
  uint32_t status = mem_loader_init();
  return status;
}

uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
  uint32_t status = mem_loader_program((uint32_t)((unsigned char *)block_start + offset_into_block),
                                       count, (uint8_t *)buffer);
  return status;
}

uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
  uint32_t addr = (uint32_t)((unsigned char *)block_start);
  uint32_t status = mem_loader_sector_erase(addr, addr + block_size);
  return status;
}

OPTIONAL_CHECKSUM
uint32_t FlashChecksum(void const *begin, uint32_t count)
{
  return Crc16((uint8_t const *)begin, count);
}

OPTIONAL_SIGNOFF
uint32_t FlashSignoff(void)
{
  uint32_t status = mem_loader_signoff();
  return status;
}

void strcopy(char *to, char *from)
{
    while (*to++ == *from++)
        ;
}
