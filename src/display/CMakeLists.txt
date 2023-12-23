set(MODULE_DISPLAY_NAME display)
add_library(${MODULE_DISPLAY_NAME} display.c
                                   display_u8g2_port.c)

include(${PROJECT_SOURCE_DIR}/src/include.cmake)
add_subdirectory(u8g2)
target_include_directories(${MODULE_DISPLAY_NAME} PRIVATE "./u8g2/csrc")
target_link_libraries(${MODULE_DISPLAY_NAME} u8g2 console ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})
