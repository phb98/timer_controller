set(watchdog_src           "watchdog.c"
    )
add_library(watchdog ${watchdog_src})
include(${PROJECT_SOURCE_DIR}/src/include.cmake)
target_link_libraries(watchdog console ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})