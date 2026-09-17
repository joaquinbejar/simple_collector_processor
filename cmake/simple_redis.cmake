include(FetchContent)
FetchContent_Declare(simple_redis
        GIT_REPOSITORY https://github.com/joaquinbejar/simple_redis.git
        GIT_TAG v0.1.3
        )
FetchContent_MakeAvailable(simple_redis)
# redis-plus-plus >= 1.3.11 generates a header at configure time (see the
# rediscpp declaration in the top-level CMakeLists.txt).
FetchContent_GetProperties(rediscpp)
target_include_directories(simple_redis PUBLIC ${rediscpp_BINARY_DIR}/src)

set(SIMPLE_POLYGON_IO_INCLUDE ${simple_redis_SOURCE_DIR}/include CACHE INTERNAL "")
if (CMAKE_DEBUG)
        message(STATUS "simple_mariadb/cmake simple_redis_SOURCE_DIR ${simple_redis_SOURCE_DIR}")
        message(STATUS "simple_mariadb/cmake SIMPLE_POLYGON_IO_INCLUDE ${SIMPLE_POLYGON_IO_INCLUDE}")
endif ()