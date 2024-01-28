include(FetchContent)
FetchContent_Declare(trading_common
        GIT_REPOSITORY https://github.com/joaquinbejar/trading_common.git
        GIT_TAG dev
        )
FetchContent_MakeAvailable(trading_common)

set(TRADING_COMMON_INCLUDE ${trading_common_SOURCE_DIR}/include CACHE INTERNAL "")
if (CMAKE_DEBUG)
        message(STATUS "ticker_collector/cmake trading_common_SOURCE_DIR ${trading_common_SOURCE_DIR}")
        message(STATUS "ticker_collector/cmake TRADING_COMMON_INCLUDE ${TRADING_COMMON_INCLUDE}")
endif ()