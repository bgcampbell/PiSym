# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include" 
                    "${CMAKE_CURRENT_SOURCE_DIR}/littlefs" 
                    "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/include" 
                    "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source"
                    "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver"
                    "${PICO_SDK_PATH}/src/rp2_common/cmsis/stub/CMSIS/Core/Include"
                   ) 

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

string(APPEND CMAKE_EXE_LINKER_FLAGS "-Wl,--print-memory-usage")

set(runToEntryPoint platform_entry)
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
set(PICO_CXX_ENABLE_EXCEPTIONS 1)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico2 CACHE STRING "Board type")
# set(PICO_COPY_TO_RAM 1)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(PiSym C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(PiSym 
               PiSym.cpp ReadWrite.cpp IO.cpp Instructions.cpp Config.cpp DebugInfo.cpp FlashBase.cpp
               symon1_1.c BASIC1_1.c BasicTrig.c RAE_B000.c RAE_E000.c
               TraceData.cpp Disassembler.cpp CommandLine.cpp CommandLine_FileAndTape.cpp SymbolTable.cpp File.cpp Tape.cpp TapeFileBase.cpp Path.cpp
               InstructionTest.cpp
               "${CMAKE_CURRENT_SOURCE_DIR}/littlefs/pico_hal.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/littlefs/lfs.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/littlefs/hw_config.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/src/crc.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/src/f_util.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source/ff.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/src/glue.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/sd_card.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/src/my_debug.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/SPI/sd_card_spi.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/SPI/my_spi.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/SPI/sd_spi.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/sd_timeouts.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/dma_interrupts.c"
               "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/ff15/source/ffunicode.c"
            #    "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/src/crash.c"

            #    "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/SDIO/sd_card_sdio.c"
            #    "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico/src/sd_driver/SDIO/rp2040_sdio.c"
              )

pico_set_program_name(PiSym "PiSym")
pico_set_program_version(PiSym "0.1")

# Modify the below lines to enable/disable output over UART/USB
# Currently disables UART and enables USB
pico_enable_stdio_uart(PiSym 0)
pico_enable_stdio_usb(PiSym 1)

# Add the standard library to the build
target_link_libraries(PiSym
                      pico_stdlib
                      hardware_pio
                      hardware_spi
                      hardware_dma
                    #   "${CMAKE_CURRENT_SOURCE_DIR}/no-OS-FatFS-SD-SDIO-SPI-RPi-Pico"
                     )

# Add the standard include files to the build
target_include_directories(PiSym PRIVATE
                        ${CMAKE_CURRENT_LIST_DIR}
)

pico_add_extra_outputs(PiSym)

