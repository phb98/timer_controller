set(LITTLEFS_SRC "littlefs/lfs.c"
                 "littlefs/lfs_util.c")
add_compile_definitions("LFS_THREADSAFE")
# add_compile_definitions("LFS_YES_TRACE")
add_library(LFS ${LITTLEFS_SRC}
                "littlefs_port.c")
target_include_directories(LFS PUBLIC "littlefs")
include(${PROJECT_SOURCE_DIR}/src/include.cmake)
target_link_libraries(LFS ${PICO_SDK_LIB_LIST} ${FREERTOS_LIB})