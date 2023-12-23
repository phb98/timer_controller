set(PICO_SDK_LIB_LIST "pico_stdlib" "hardware_i2c")
set(FREERTOS_LIB "FreeRTOS-Kernel-Heap4")

include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/src/console)
include_directories(${PROJECT_SOURCE_DIR}/src/display)
