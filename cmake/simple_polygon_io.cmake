include(FetchContent)
FetchContent_Declare(simple_polygon_io
        GIT_REPOSITORY https://github.com/joaquinbejar/simple-polygon-io.git
        GIT_TAG v0.1.7
        )
FetchContent_MakeAvailable(simple_polygon_io)

set(SIMPLE_POLYGON_IO_INCLUDE ${simple_polygon_io_SOURCE_DIR}/include CACHE INTERNAL "")
if (CMAKE_DEBUG)
        message(STATUS "simple_mariadb/cmake simple_polygon_io_SOURCE_DIR ${simple_polygon_io_SOURCE_DIR}")
        message(STATUS "simple_mariadb/cmake SIMPLE_POLYGON_IO_INCLUDE ${SIMPLE_POLYGON_IO_INCLUDE}")
endif ()