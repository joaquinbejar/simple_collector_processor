cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(collector)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH})

include(cmake/simple_polygon_io.cmake)
include(cmake/simple_redis.cmake)
include(cmake/trading_common.cmake)
include(cmake/simple_kafka.cmake)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_definitions(NDEBUG)
endif()

set(COLLECTOR_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(COLLECTOR_SOURCE_FILES
        include/collector/config.h
        include/collector/exceptions.h
        include/collector/processor.h
        include/collector/kafkaforwarder.h
        src/config.cpp
        src/exceptions.cpp
)

add_library(collector STATIC ${COLLECTOR_SOURCE_FILES})
set(COLLECTOR_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

if (CMAKE_DEBUG)
    message(STATUS "collector SIMPLE_COLOR_INCLUDE: ${SIMPLE_COLOR_INCLUDE}")
    message(STATUS "collector SIMPLE_CONFIG_INCLUDE: ${SIMPLE_CONFIG_INCLUDE}")
    message(STATUS "collector SIMPLE_LOGGER_INCLUDE: ${SIMPLE_LOGGER_INCLUDE}")
    message(STATUS "collector NLOHMANN_JSON_INCLUDE: ${NLOHMANN_JSON_INCLUDE}")
    message(STATUS "collector COMMON_INCLUDE: ${COMMON_INCLUDE}")
    message(STATUS "collector SIMPLE_KAFKA_INCLUDE: ${SIMPLE_KAFKA_INCLUDE}")
    message(STATUS "collector TRADING_COMMON_INCLUDE: ${TRADING_COMMON_INCLUDE}")
endif ()

target_include_directories(collector PUBLIC
        ${COLLECTOR_INCLUDE}
        ${SIMPLE_CONFIG_INCLUDE}
        ${SIMPLE_LOGGER_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
        ${COMMON_INCLUDE}
        ${CURL_INCLUDE}
        ${SIMPLE_POLYGON_IO_INCLUDE}
        ${REDISCPP_HEADER}
        ${REDISCPP_HEADER17}
        ${REDISCPP_HEADERTLS}
        ${HIREDIS_HEADER}
        ${SIMPLE_KAFKA_INCLUDE}
        ${TRADING_COMMON_INCLUDE}
)

target_link_libraries(collector
        PUBLIC
        pthread
        simple_config
        simple_logger
        common
        libcurl_static
        simple_polygon_io
        simple_redis
        simple_kafka
        trading_common
)


set(COLLECTOR_TESTS_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
option(NO_COLLECTOR_TESTS "collector Disable collector tests" OFF)
if (NOT NO_COLLECTOR_TESTS)
    include(FetchContent)
    FetchContent_Declare(Catch2
            GIT_REPOSITORY https://github.com/catchorg/Catch2.git
            GIT_TAG v3.4.0
    )
    FETCHCONTENT_GETPROPERTIES(Catch2)
    FETCHCONTENT_MAKEAVAILABLE(Catch2)
    set(CURL_DISABLE_TESTS ON)
    add_subdirectory(tests)
endif ()