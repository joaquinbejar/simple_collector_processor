//
// Created by Joaquin Bejar Garcia on 20/10/23.
//

#include "collector/config.h"
#include <catch2/catch_test_macros.hpp>
#include "simple_logger/logger.h"

using namespace collector::config;

TEST_CASE("CollectorConfig Tests", "[CollectorConfig]") {
    CollectorConfig config;

    SECTION("Default Configuration") {
        REQUIRE(config.get_api_url() == "https://api.polygon.io");
        REQUIRE(config.get_api_key().empty());
    }

    SECTION("Validate with default settings") {
        REQUIRE_FALSE(config.validate());
    }


    SECTION("from_json method") {
        json j;
        j["m_polygon_api_url"] = "https://api.fromjson.com";
        j["m_polygon_api_key"] = "jsonkey123";

        REQUIRE_THROWS(config.from_json(j));
    }

    SECTION("to_string method") {
        std::string expected_str = R"(CollectorConfig {PolygonIOConfig { m_polygon_api_url='https://api.polygon.io', m_polygon_api_key=''}, {"RedisConfig":{"collect_interval":3600,"database":"Polygon","informer_interval":333,"max_queue_size":10000,"polygonio":{"m_polygon_api_key":"","m_polygon_api_url":"https://api.polygon.io"},"producer_interval":10,"redis":{"connect_timeout":30000,"connection_idle_time":0,"connection_lifetime":0,"db":0,"host":"localhost","keep_alive":true,"loglevel":"info","password":"password","port":6379,"size":1000,"socket_timeout":30000,"tag":"tag","wait_timeout":30000},"table":""}}})";
        REQUIRE(config.to_string() == expected_str);
    }

    SECTION("from_json method and key as param") {
        json j;
        j["polygonio"]["m_polygon_api_url"] = "https://api.fromjson.com";
        j["polygonio"]["m_polygon_api_key"] = "jsonkey123";
        j["table"] = "table";
        j["redis"] = R"({"connect_timeout":0,"connection_idle_time":0,"connection_lifetime":0,"db":0,"host":"localhost",
                         "password":"","port":6379,"size":1,"socket_timeout":0,"keep_alive":false,"wait_timeout":0,
                         "tag":"tag"})"_json;
        config.from_json(j);

        REQUIRE(config.get_api_url() == "https://api.fromjson.com");
        REQUIRE(config.get_api_key_as_param() == "apiKey=jsonkey123");
        REQUIRE(config.validate());
        REQUIRE(config.connection_options->port == 6379);
        REQUIRE(config.connection_options->host == "localhost");
        REQUIRE(config.connection_options->password.empty());
        REQUIRE(config.connection_options->size == 1);
        REQUIRE(config.connection_options->tag == "tag");
    }
}


TEST_CASE("ForwarderConfig Tests", "[ForwarderConfig]") {
    ForwarderConfig config;

    SECTION("Default Configuration") {
        REQUIRE(config.get_api_url() == "https://api.polygon.io");
        REQUIRE(config.get_api_key().empty());
    }

    SECTION("Validate with default settings") {
        REQUIRE_FALSE(config.validate());
    }


    SECTION("from_json method") {
        std::cout << config.to_json().dump(4) << std::endl;
        json j;
        j["polygonio"]["m_polygon_api_url"] = "https://api.fromjson.com";
        j["polygonio"]["m_polygon_api_key"] = "jsonkey123";

        REQUIRE_THROWS(config.from_json(j));
    }

    SECTION("Valid from json"){
        json j = R"({
                    "collect_interval": 3600,
                    "informer_interval": 333,
                    "kafka": {
                        "kafka_brokers": "localhost:9092",
                        "kafka_group_id": "group_id_1",
                        "kafka_msg_timeout": 3000,
                        "kafka_topic": ["topic1", "topic2"],
                        "kafka_flush_timeout": 1000,
                        "kafka_warning_partition_eof": true,
                        "kafka_consumer_name": "consumer_name"
                    },
                    "max_queue_size": 10000,
                    "polygonio": {
                        "m_polygon_api_key": "mykey",
                        "m_polygon_api_url": "https://api.polygon.io"
                    },
                    "producer_interval": 10,
                    "redis": {
                        "connect_timeout": 30000,
                        "connection_idle_time": 0,
                        "connection_lifetime": 0,
                        "db": 0,
                        "host": "localhost",
                        "keep_alive": true,
                        "loglevel": "info",
                        "password": "password",
                        "port": 6379,
                        "size": 1000,
                        "socket_timeout": 30000,
                        "tag": "tag",
                        "wait_timeout": 30000
                    },
                    "redis_key": "TEST"
                })"_json;
        config.from_json(j);
        REQUIRE(config.validate());
    }

    SECTION("to_string method") {
        json j = R"({
                    "collect_interval": 3600,
                    "informer_interval": 333,
                    "kafka": {
                        "kafka_brokers": "localhost:9092",
                        "kafka_group_id": "group_id_1",
                        "kafka_msg_timeout": 3000,
                        "kafka_topic": ["topic1", "topic2"],
                        "kafka_flush_timeout": 1000,
                        "kafka_warning_partition_eof": true,
                        "kafka_consumer_name": "consumer_name"
                    },
                    "max_queue_size": 10000,
                    "polygonio": {
                        "m_polygon_api_key": "mykey",
                        "m_polygon_api_url": "https://api.polygon.io"
                    },
                    "producer_interval": 10,
                    "redis": {
                        "connect_timeout": 30000,
                        "connection_idle_time": 0,
                        "connection_lifetime": 0,
                        "db": 0,
                        "host": "localhost",
                        "keep_alive": true,
                        "loglevel": "info",
                        "password": "password",
                        "port": 6379,
                        "size": 1000,
                        "socket_timeout": 30000,
                        "tag": "tag",
                        "wait_timeout": 30000
                    },
                    "redis_key": "TEST"
                })"_json;
        config.from_json(j);
        std::string expected_str = R"("ForwarderConfig":{"collect_interval":3600,"informer_interval":333,"kafka":{"kafka_brokers":"localhost:9092","kafka_consumer_name":"consumer_name","kafka_flush_timeout":1000,"kafka_group_id":"group_id_1","kafka_msg_timeout":3000,"kafka_topic":["topic1","topic2"],"kafka_warning_partition_eof":true},"max_queue_size":10000,"polygonio":{"m_polygon_api_key":"mykey","m_polygon_api_url":"https://api.polygon.io"},"producer_interval":10,"redis":{"connect_timeout":30000,"connection_idle_time":0,"connection_lifetime":0,"db":0,"host":"localhost","keep_alive":true,"loglevel":"info","password":"password","port":6379,"size":1000,"socket_timeout":30000,"tag":"tag","wait_timeout":30000},"redis_key":"TEST"})";
        REQUIRE(config.to_string() == expected_str);
    }

    SECTION("from_json method and key as param") {
        json j = R"({
                    "collect_interval": 3600,
                    "informer_interval": 333,
                    "kafka": {
                        "kafka_brokers": "localhost:9092",
                        "kafka_group_id": "group_id_1",
                        "kafka_msg_timeout": 3000,
                        "kafka_topic": ["topic1", "topic2"],
                        "kafka_flush_timeout": 1000,
                        "kafka_warning_partition_eof": true,
                        "kafka_consumer_name": "consumer_name"
                    },
                    "max_queue_size": 10000,
                    "polygonio": {
                        "m_polygon_api_key": "mykey",
                        "m_polygon_api_url": "https://api.polygon.io"
                    },
                    "producer_interval": 10,
                    "redis": {
                        "connect_timeout": 30000,
                        "connection_idle_time": 0,
                        "connection_lifetime": 0,
                        "db": 0,
                        "host": "localhost",
                        "keep_alive": true,
                        "loglevel": "info",
                        "password": "password",
                        "port": 6379,
                        "size": 1000,
                        "socket_timeout": 30000,
                        "tag": "tag",
                        "wait_timeout": 30000
                    },
                    "redis_key": "TEST"
                })"_json;
        config.from_json(j);

        REQUIRE(config.get_api_url() == "https://api.polygon.io");
        REQUIRE(config.get_api_key_as_param() == "apiKey=mykey");
        REQUIRE(config.validate());
        REQUIRE(config.connection_options->port == 6379);
        REQUIRE(config.connection_options->host == "localhost");
        REQUIRE(config.connection_options->password == "password");
        REQUIRE(config.connection_options->size == 1000);
        REQUIRE(config.connection_options->tag == "tag");
    }
}