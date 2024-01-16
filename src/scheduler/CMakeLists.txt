set(SCHEDULER_SRC  "scheduler.c"
                    )
add_library(scheduler ${SCHEDULER_SRC})
include(${PROJECT_SOURCE_DIR}/src/include.cmake)
target_link_libraries(scheduler console ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})