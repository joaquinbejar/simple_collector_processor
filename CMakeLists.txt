cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(collector)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH})

include(FetchContent)
# Declared before the sub-dependencies so this pin wins over the Catch2 they
# declare themselves (FetchContent honours the first declaration).
FetchContent_Declare(Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.16.0
)
# nlohmann_json is only pulled in transitively (common_cpp v0.1.5 pins v3.11.2,
# whose cmake_minimum_required is rejected by CMake >= 4.0); pin the current
# release here so it takes precedence.
FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
)
# hiredis is only pulled in transitively (simple_redis v0.1.3 pins v1.2.0, whose
# cmake_minimum_required is rejected by CMake >= 4.0); pin the current release.
FetchContent_Declare(hiredis
        GIT_REPOSITORY https://github.com/redis/hiredis.git
        GIT_TAG v1.4.1
)
# Same for redis-plus-plus (simple_redis v0.1.3 pins 1.3.10, also rejected by
# CMake >= 4.0). 1.3.11+ generates sw/redis++/hiredis_features.h into its
# build tree; that directory is added to the simple_redis target below.
FetchContent_Declare(rediscpp
        GIT_REPOSITORY https://github.com/sewenew/redis-plus-plus.git
        GIT_TAG 1.3.15
)
# Options for libraries built from source by the sub-dependencies. CMAKE_ARGS in
# a FetchContent_Declare is ignored, so they have to be cache variables here.
# curl: only libcurl_static is consumed (simple_polygon_io links it).
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_LIBCURL_DOCS OFF CACHE BOOL "" FORCE)
set(BUILD_MISC_DOCS OFF CACHE BOOL "" FORCE)
set(ENABLE_CURL_MANUAL OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
# librdkafka: its examples and test suite do not build against the current
# macOS SDK (tinycthread's call_once macro clashes with libc++).
set(RDKAFKA_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(RDKAFKA_BUILD_TESTS OFF CACHE BOOL "" FORCE)
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
    FETCHCONTENT_GETPROPERTIES(Catch2)
    FETCHCONTENT_MAKEAVAILABLE(Catch2)
    set(CURL_DISABLE_TESTS ON)
    add_subdirectory(tests)
endif ()