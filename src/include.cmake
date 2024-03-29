set(PICO_SDK_LIB_LIST "pico_stdlib" "hardware_i2c" "hardware_flash" "hardware_sync" "hardware_pwm")
set(FREERTOS_LIB "FreeRTOS-Kernel-Heap4")
include(${PROJECT_SOURCE_DIR}/src/coreJSON/jsonFilePaths.cmake)
include_directories(${PROJECT_SOURCE_DIR}/config)
include_directories(${PROJECT_SOURCE_DIR}/src/console)
include_directories(${PROJECT_SOURCE_DIR}/src/display)
include_directories(${PROJECT_SOURCE_DIR}/src/display/ui)
include_directories(${PROJECT_SOURCE_DIR}/src/display/ux)
include_directories(${PROJECT_SOURCE_DIR}/src/user_input)
include_directories(${PROJECT_SOURCE_DIR}/src/hal_i2c)
include_directories(${PROJECT_SOURCE_DIR}/src/rtc)
include_directories(${PROJECT_SOURCE_DIR}/src/lfs)
include_directories(${PROJECT_SOURCE_DIR}/src/scheduler)
include_directories(${PROJECT_SOURCE_DIR}/src/output_controller)
include_directories(${PROJECT_SOURCE_DIR}/src/coreJSON/source/include)
include_directories(${PROJECT_SOURCE_DIR}/src/watchdog)








