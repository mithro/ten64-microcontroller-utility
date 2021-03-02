cmake_minimum_required(VERSION 3.2)

project(lpc804cli)

#SET(CMAKE_C_COMPILER /opt/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc)

set(COMMON_SOURCE_FILES src/api.c src/twi.c)
add_executable(lpc804cli src/main.c ${COMMON_SOURCE_FILES})

INSTALL(TARGETS lpc804cli
    RUNTIME DESTINATION sbin)

#TARGET_LINK_LIBRARIES(lpc804cli ${LIBS})
