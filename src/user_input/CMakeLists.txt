set(USER_INPUT_SRC  "user_input.c"
                    "user_input_button.c")
add_library(user_input ${USER_INPUT_SRC})
include(${PROJECT_SOURCE_DIR}/src/include.cmake)
target_link_libraries(user_input console ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})