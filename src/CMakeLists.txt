#Glob all main source files
file(GLOB C_MAIN_SOURCES *.c)

add_executable(${PROJECT_NAME}
        ${C_MAIN_SOURCES}
        )
# pull in common dependencies
target_link_libraries(${PROJECT_NAME} pico_stdlib FreeRTOS-Kernel-Heap4)

# create map/bin/hex file etc.
pico_add_extra_outputs(${PROJECT_NAME})
