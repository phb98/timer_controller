# Glob all main source files
file(GLOB C_MAIN_SOURCES *.c)
# Main executable(elf file)
add_executable(${PROJECT_NAME} ${C_MAIN_SOURCES})
include(include.cmake)
set(MODULE_LIB_LIST "display"
                    "console")


# submoudle directory
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/display)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/console)


# Link using library
target_link_libraries(${PROJECT_NAME} ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})
target_link_libraries(${PROJECT_NAME} ${MODULE_LIB_LIST})
# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
