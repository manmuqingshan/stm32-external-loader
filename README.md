# STM32 Memory Loaders

## Overview

This repository contains STM32 memory loader source code for both **external** and **internal** memories used on STM32 hardware boards.
The **master** branch provides the memory loader projects and source files integrated with STM32CubeProgrammer.

```
stm32-memory-loaders/
├── external_memory_loaders/   # Memory loaders for external Flash/SRAM chips
│   ├── STM32F4x_boards/
│   ├── STM32F7x_boards/
│   ├── STM32G4x_boards/
│   ├── STM32H5x_boards/
│   ├── STM32H7x_boards/
│   ├── STM32L4x_boards/
│   ├── STM32L5x_boards/
│   └── STM32U5x_boards/
└── internal_memory_loaders/   # Memory loaders for on-chip Flash memories
    ├── stm32_mem_loader/      # Unified loader infrastructure (CMSIS Pack)
    └── stm32_device_loaders/  # Device-specific loader implementations
        └── stm32c5xx/
```

---

## Get software

> [!IMPORTANT]
> The `external_memory_loaders/` directory uses **submodules**. Please follow the instructions below to clone and get updates.

* To **clone** the repository (including external loader submodules), run:
```bash
git clone --recursive https://github.com/STMicroelectronics/stm32-memory-loaders.git
```
* To **pull** the latest updates, run the commands below from the root of the repository:
```bash
git pull
git submodule update --init --recursive
```

> [!NOTE]
> * The `--recursive` flag applies to the **external memory loaders** only. Internal memory loaders are self-contained and do not require submodule initialization.
> * If the GitHub **Download ZIP** option is used instead of `git clone`, the submodules must be **collected and added manually**.

---

## External memories

External memories are available on many STM32 hardware boards such as evaluation and discovery kits. They can be Flash or SRAM and provide higher storage capabilities. STM32 boards support many types of external memories from vendors such as Micron and Winbond, connected through interfaces like FMC, QSPI, and SPI.

## Internal memories

Internal memories are embedded directly inside STM32 MCUs (on-chip Flash). The loaders in this repository target the internal Flash controller of each supported STM32 series and expose a standard API consumed by STM32CubeProgrammer.

---

## External memory loaders

External memory loaders are located in [`external_memory_loaders/`](external_memory_loaders/) and are organized by STM32 series and board.

Each external memory loader project is built with EWARM or MDK-ARM IDE and comes with the corresponding source, header, and linker files:

* **Library** – source/header files providing the drivers needed to initialize the memory and perform read, write, and erase operations.
* **Loader** – source/header files containing specific information related to the supported memory (name, size, and related functions).
* **Project** – a pre-configured IDE project with the associated linker file.

### Supported boards

| STM32 Series | Board | Memory |
|---|---|---|
| STM32F4x | STM32F469I-DK | IS42S32400F, MT25QL128A |
| STM32F4x | STM32469I-EVAL | MT25QL512A |
| STM32F4x | STM32412G-DISCO | N25Q128A |
| STM32F4x | STM32F413H-DISCO | N25Q128A |
| STM32F7x | STM32F723E-DISCO | MX25L512G |
| STM32F7x | STM32F7508-DISCO | N25Q128A |
| STM32F7x | STM32F769I-EVAL | PC28F128M29 |
| STM32G4x | STM32G474E-EVAL | MT25QL512ABB |
| STM32H5x | STM32H573I-DK | MX25LM51245G |
| STM32H7x | STM32H743I-EVAL | M29W128GL, MT25TL01G |
| STM32H7x | STM32H747I-DISCO | MT25TL01G |
| STM32H7x | STM32H747I-EVAL | MT25TL01G |
| STM32H7x | STM32H735G-DK | MX25LM51245G |
| STM32H7x | STM32H7B3I-DISCO | MX25LM51245G |
| STM32H7x | STM32H7B3I-EVAL | MX25LM51245G |
| STM32L4x | STM32L476G-EVAL | M29W128GL |
| STM32L4x | STM32L4P5G-DK | MX25LM51245G |
| STM32L4x | STM32L4R9I-DISCO | MX25LM51245G |
| STM32L4x | STM32L4R9I-EVAL | MX25LM51245G |
| STM32L4x | STM32L496G-DISCO | MX25R6435F |
| STM32L4x | STM32L476G-DISCO | N25Q128A |
| STM32L5x | STM32L562E-DK | MX25LM51245G |
| STM32U5x | STM32U575I-EVAL | MX25LM51245G |
| STM32U5x | STM32U585I-IOT02A | MX25LM51245G |

### How to adapt an external memory loader project for a customized board

The required steps to build a customized external loader for STM32CubeProgrammer are available at this [link](https://www.st.com/content/ccc/resource/technical/document/user_manual/e6/10/d8/80/d6/1d/4a/f2/CD00262073.pdf/files/CD00262073.pdf/jcr:content/translations/en.CD00262073.pdf) (Section 3.9).

---

## Internal memory loaders

Internal memory loaders are located in [`internal_memory_loaders/`](internal_memory_loaders/) and are split into two parts: a **shared infrastructure** (`stm32_mem_loader`) and **device-specific loader implementations** (`stm32_device_loaders`).

### Architecture

```
internal_memory_loaders/
├── stm32_mem_loader/          # CMSIS Pack – toolchain-agnostic loader infrastructure
│   ├── common/                # Shared API and linker resources
│   │   ├── device_loader.h    # Common mem_loader_* API declarations
│   │   ├── mem_loader_common.c
│   │   └── linker/
│   │       └── iar/
│   │           └── mem_loader.icf
│   ├── iar/                   # IAR EWARM glue layer
│   │   ├── ewarm_mem_loader.c
│   │   ├── ewarm_mem_loader.h
│   │   ├── mem_loader_entry.c
│   │   ├── mem_loader_glue.c
│   │   ├── mem_loader_config.h
│   │   └── mem_loader_extra.h
│   ├── keil/                  # Keil MDK glue layer
│   │   ├── keil_mem_loader.c
│   │   ├── keil_mem_loader.h
│   │   └── keil_mem_loader_desc.c
│   ├── st/                    # ST toolchain glue layer
│   │   ├── stm32_mem_loader.c
│   │   ├── stm32_mem_loader_desc.c
│   │   └── stm32_mem_loader_desc.h
│   ├── LICENSE.md             # Pack-local license file referenced by the PDSC
│   └── STMicroelectronics.stm32_mem_loader.pdsc  # CMSIS Pack descriptor
└── stm32_device_loaders/
    └── stm32c5xx/             # STM32C5xx on-chip memory loader
        ├── memory_controller/  # Low-level memory controller driver
        │   ├── stm32c5_memory_controller.c
        │   ├── stm32c5_memory_controller.h
        │   └── stm32c5_hw.h
        ├── stm32c5_device_desc.h      # Per-variant memory geometry
        ├── stm32c5_device_loader.c    # mem_loader_* implementation
        ├── stm32c5_mem_loader.csolution.yml
        └── stm32c5_mem_loader.cproject.yml
```

### Common loader API

All internal loaders implement the following interface declared in `common/device_loader.h`:

| Function | Description |
|---|---|
| `mem_loader_init()` | Initialize the Flash controller |
| `mem_loader_deinit()` | De-initialize the Flash controller |
| `mem_loader_sector_erase(start, end)` | Erase sectors in the given address range |
| `mem_loader_program(addr, size, buf)` | Program a buffer into Flash |
| `mem_loader_mass_erase()` | Erase the entire Flash |

Return values use `mem_loader_status`: `MEM_LOADER_STATUS_SUCCESS (0)` or `MEM_LOADER_STATUS_FAIL (1)`.

### Supported toolchains

The `stm32_mem_loader` CMSIS Pack provides ready-to-use glue layers for three toolchains:

| Toolchain | Component | Linker resource |
|---|---|---|
| IAR EWARM | `STMicroelectronics::Utility:Loader:IAR` | `common/linker/iar/mem_loader.icf` |
| Keil MDK (ARMCC) | `STMicroelectronics::Utility:Loader:Keil` | Not provided in the current pack layout |
| GCC (ST toolchain) | `STMicroelectronics::Utility:Loader:ST` | Not provided in the current pack layout |

### Supported devices

| Series | Target define | Flash size | Start address |
|---|---|---|---|
| STM32C5xx | `STM32C5_0x44E_0x08000000_512K` | 512 KB | `0x08000000` |
| STM32C5xx | `STM32C5_0x44F_0x08000000_256K` | 256 KB | `0x08000000` |

### Building an internal loader (STM32C5xx example)

The internal loaders use the [Open-CMSIS-Toolbox](https://github.com/Open-CMSIS-Pack/cmsis-toolbox) `cbuild` command.

1. Install Open-CMSIS-Toolbox and make sure `cbuild` is available from your terminal `PATH`.

2. Install the required CMSIS Pack:
    ```bash
    cpackget add ARM::CMSIS@5.9.0
    ```

    The `STMicroelectronics::stm32_mem_loader` pack is referenced locally by the solution file:
    `internal_memory_loaders/stm32_mem_loader`.

3. Navigate to the loader directory and run `cbuild`:
    ```bash
    cd internal_memory_loaders/stm32_device_loaders/stm32c5xx
    cbuild stm32c5_mem_loader.csolution.yml --context stm32c5_mem_loader.IAR+STM32C5_0x44E_0x08000000_512K
    ```

4. The built `.elf` / `.out` files are placed under `build/<BuildType>+<TargetType>/`.

#### Known build warning

The build tool may report the following warning :

```text
Warning: Cannot detect the section type of 'FlashDevice'
```

This warning is expected for the current loader integration. The `FlashDevice`
symbol is required by the loader interface used by STM32CubeProgrammer and
must remain available for CubeProgrammer loader compatibility. The warning does
not indicate a failed build when the build summary reports success and the
`.elf` / `.out` output is generated.

#### Validated tool versions

The STM32C5xx IAR build has been validated with the following tool versions:

| Tool | Version |
|---|---|
| IAR ANSI C/C++ Compiler for Arm | V9.60.3.422/W64 |
| Open-CMSIS-Toolbox `cmsis-project` | 2.11.0 |
| Open-CMSIS-Toolbox `cmsis-build` | 2.11.0 |
| Open-CMSIS-Toolbox `cmsis-cbuild2cmake` | 0.9.8 |
| CMake | 4.0.1 |
| Ninja | 1.12.1 |
| CMSIS Pack `ARM::CMSIS` | 5.9.0 |


---

## Feedback and contributions

Please refer to the [CONTRIBUTING.md](CONTRIBUTING.md) guide.
