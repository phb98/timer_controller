cmake_minimum_required(VERSION 3.12)
# set(FREERTOS_KERNEL_PATH ${CMAKE_HOME_DIRECTORY}/FreeRTOS-Kernel)
# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)

project("timer_controller" C CXX ASM)
include_directories(${CMAKE_HOME_DIRECTORY}/config/freeRTOS)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

# Initialize the SDK
pico_sdk_init()
add_compile_options(-fdata-sections -ffunction-sections -Wl,--gc-sections -O0 )
add_subdirectory(src)