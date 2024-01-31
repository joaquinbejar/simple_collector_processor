include(FetchContent)
FetchContent_Declare(simple_kafka
        GIT_REPOSITORY https://github.com/joaquinbejar/simple_kafka.git
        GIT_TAG v0.1.0
        )
FetchContent_MakeAvailable(simple_kafka)

set(SIMPLE_KAFKA_INCLUDE ${simple_kafka_SOURCE_DIR}/include CACHE INTERNAL "")
if (CMAKE_DEBUG)
        message(STATUS "ticker_collector/cmake simple_kafka_SOURCE_DIR ${simple_kafka_SOURCE_DIR}")
        message(STATUS "ticker_collector/cmake SIMPLE_KAFKA_INCLUDE ${SIMPLE_KAFKA_INCLUDE}")
endif ()