if (CMAKE_DEBUG)
    message(STATUS "simple_polygon_io/tests SIMPLE_COLOR_INCLUDE: ${SIMPLE_COLOR_INCLUDE}")
    message(STATUS "simple_polygon_io/tests SIMPLE_CONFIG_INCLUDE: ${SIMPLE_CONFIG_INCLUDE}")
    message(STATUS "simple_polygon_io/tests SIMPLE_LOGGER_INCLUDE: ${SIMPLE_LOGGER_INCLUDE}")
    message(STATUS "simple_polygon_io/tests NLOHMANN_JSON_INCLUDE: ${NLOHMANN_JSON_INCLUDE}")
    message(STATUS "simple_polygon_io/tests COMMON_INCLUDE: ${COMMON_INCLUDE}")
endif ()




add_executable(test_config_collector test_config.cpp)
target_include_directories(test_config_collector
        PRIVATE
        ${COLLECTOR_INCLUDE}
        ${SIMPLE_COLOR_INCLUDE}
        ${SIMPLE_CONFIG_INCLUDE}
        ${SIMPLE_LOGGER_INCLUDE}
        ${NLOHMANN_JSON_INCLUDE}
        ${COMMON_INCLUDE}
        ${SIMPLE_POLYGON_IO_INCLUDE}
        ${REDISCPP_HEADER}
        ${REDISCPP_HEADER17}
        ${REDISCPP_HEADERTLS}
        ${HIREDIS_HEADER}
)

target_link_libraries(test_config_collector PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_config_collector PRIVATE
        simple_logger
        simple_config
        common
        simple_polygon_io
        libcurl_static
        collector
        simple_redis
)


add_executable(test_exceptions_collector test_exceptions.cpp)
target_include_directories(test_exceptions_collector
        PRIVATE
        ${COLLECTOR_INCLUDE}
        ${SIMPLE_COLOR_INCLUDE}
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
)

target_link_libraries(test_exceptions_collector PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_exceptions_collector PRIVATE
        simple_logger
        simple_config
        common
        simple_polygon_io
        libcurl_static
        collector
        simple_redis
)

add_executable(test_processor test_processor.cpp)
target_include_directories(test_processor
        PRIVATE
        ${COLLECTOR_INCLUDE}
        ${SIMPLE_COLOR_INCLUDE}
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
)

target_link_libraries(test_processor PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_processor PRIVATE
        simple_logger
        simple_config
        common
        simple_polygon_io
        libcurl_static
        collector
        simple_redis
)

add_executable(test_kafkaforwarder test_kafkaforwarder.cpp)
target_include_directories(test_processor
        PRIVATE
        ${COLLECTOR_INCLUDE}
        ${SIMPLE_COLOR_INCLUDE}
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

target_link_libraries(test_kafkaforwarder PRIVATE Catch2::Catch2WithMain)
target_link_libraries(test_kafkaforwarder PRIVATE
        simple_logger
        simple_config
        common
        simple_polygon_io
        libcurl_static
        collector
        simple_redis
        simple_kafka
        trading_common
)