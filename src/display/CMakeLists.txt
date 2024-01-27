set(MODULE_DISPLAY_NAME display)
set(UX_SRC_FILES "ux/ux.c"
                 "ux/ux_startup.c"
                 "ux/ux_timer.c"
                 "ux/ux_mainscreen.c"
                 "ux/ux_time.c"
                 "ux/ux_menu.c")
add_library(${MODULE_DISPLAY_NAME} display.c
                                   display_u8g2_port.c
                                   ${UX_SRC_FILES}
                                   ui/ui.c)

include(${PROJECT_SOURCE_DIR}/src/include.cmake)
add_subdirectory(u8g2)
target_include_directories(${MODULE_DISPLAY_NAME} PRIVATE "./u8g2/csrc")
target_include_directories(${MODULE_DISPLAY_NAME} PRIVATE "./ui")
target_include_directories(${MODULE_DISPLAY_NAME} PRIVATE "./ux")

target_link_libraries(${MODULE_DISPLAY_NAME} u8g2 
                                             console
                                             user_input
                                             hal_i2c
                                             rtc 
                                            ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})
