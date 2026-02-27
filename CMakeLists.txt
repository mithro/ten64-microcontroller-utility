cmake_minimum_required(VERSION 3.30)

project(lpc804cli)

#SET(CMAKE_C_COMPILER /opt/gcc-linaro-7.4.1-2019.02-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc)

OPTION(ENABLE_MAC_SET "Build with ability to change board MAC" OFF)

set(COMMON_SOURCE_FILES src/api.c src/twi.c)
add_executable(ten64-controller src/main.c ${COMMON_SOURCE_FILES})

IF (ENABLE_MAC_SET)
add_definitions(-DENABLE_MAC_CHANGE)
ENDIF()

set(CMAKE_EXE_LINKER_FLAGS "-static")

INSTALL(TARGETS ten64-controller
    RUNTIME DESTINATION sbin)

#TARGET_LINK_LIBRARIES(lpc804cli ${LIBS})
