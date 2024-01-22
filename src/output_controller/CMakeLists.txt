
set(output_controller_src  "output_controller.c"
                           "driver/output_mcu_pwm.c"
                           "driver/output_pcf8574.c"
    )
add_library(output_controller ${output_controller_src})
include(${PROJECT_SOURCE_DIR}/src/include.cmake)
target_link_libraries(output_controller console ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})