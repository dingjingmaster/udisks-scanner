file(GLOB COMMON_SRC
        ${CMAKE_SOURCE_DIR}/app/common/log.c
        ${CMAKE_SOURCE_DIR}/app/common/log.h

        ${CMAKE_SOURCE_DIR}/app/common/global.h
        )
include_directories(${CMAKE_SOURCE_DIR}/app/common)