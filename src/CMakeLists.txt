# Glob all main source files
file(GLOB C_MAIN_SOURCES *.c)
# Main executable(elf file)
add_executable(${PROJECT_NAME} ${C_MAIN_SOURCES})
include(include.cmake)
set(MODULE_LIB_LIST "display"
                    "console"
                    "user_input"
                    "hal_i2c"
                    "rtc"
                    "LFS")


# submoudle directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/display)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/console)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/user_input)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/hal_i2c)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/rtc)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lfs)



# Link using library
target_link_libraries(${PROJECT_NAME} ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})
target_link_libraries(${PROJECT_NAME} ${MODULE_LIB_LIST})
# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
# Display size info
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
COMMAND arm-none-eabi-size  --format=GNU "${CMAKE_CURRENT_SOURCE_DIR}/../build/src/${PROJECT_NAME}.elf")
