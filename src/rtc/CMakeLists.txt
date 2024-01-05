
set(rtc_src  "rtc.c"
              "ds3231/rtc_ds3231.c")
add_library(rtc ${rtc_src})
# target_include_directories(rtc PRIVATE "./ds3231")
include(${PROJECT_SOURCE_DIR}/src/include.cmake)
target_link_libraries(rtc console ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB} hal_i2c)