
set(hal_i2c_src  "hal_i2c.c")
add_library(hal_i2c ${hal_i2c_src})
include(${PROJECT_SOURCE_DIR}/src/include.cmake)
target_link_libraries(hal_i2c console ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})