include(FetchContent)
FetchContent_Declare(simple_redis
        GIT_REPOSITORY https://github.com/joaquinbejar/simple_redis.git
        GIT_TAG v0.1.1
        )
FetchContent_MakeAvailable(simple_redis)

set(SIMPLE_POLYGON_IO_INCLUDE ${simple_redis_SOURCE_DIR}/include CACHE INTERNAL "")
if (CMAKE_DEBUG)
        message(STATUS "simple_mariadb/cmake simple_redis_SOURCE_DIR ${simple_redis_SOURCE_DIR}")
        message(STATUS "simple_mariadb/cmake SIMPLE_POLYGON_IO_INCLUDE ${SIMPLE_POLYGON_IO_INCLUDE}")
endif ()