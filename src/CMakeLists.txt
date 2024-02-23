include(include.cmake)
set(MODULE_LIB_LIST "display"
                    "console"
                    "user_input"
                    "hal_i2c"
                    "rtc"
                    "LFS"
                    "scheduler"
                    "output_controller"
                    "watchdog")


# submoudle directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/display)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/console)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/user_input)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hal_i2c)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rtc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lfs)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/scheduler)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/output_controller)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/watchdog)

# Glob all main source files
file(GLOB C_MAIN_SOURCES *.c)
# Main executable(elf file)
add_executable(${PROJECT_NAME} ${C_MAIN_SOURCES})

# coreJSON library
add_library(coreJSON ${JSON_SOURCES})

# Link using library
target_link_libraries(${PROJECT_NAME} ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})
target_link_libraries(${PROJECT_NAME} ${MODULE_LIB_LIST})
target_link_libraries(${PROJECT_NAME} coreJSON)
# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
pico_enable_stdio_usb(${PROJECT_NAME} 0)
# Display size info
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND arm-none-eabi-size  --format=GNU "${CMAKE_CURRENT_SOURCE_DIR}/../build/src/${PROJECT_NAME}.elf")
