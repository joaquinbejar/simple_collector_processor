//
// Created by Joaquin Bejar Garcia on 18/11/23.
//
#include "collector/config.h"

namespace collector::config {

    /**
     * Validates the configuration.
     *
     * This method validates the PolygonIO and Redis configuration settings.
     *
     * @return True if the configuration is valid, false otherwise.
     */
    bool CollectorConfig::validate() {
        if (!simple_polygon_io::config::PolygonIOConfig::validate()) {
            logger->send<simple_logger::LogLevel::ERROR>("PolygonIO is not valid");
            return false;
        }
        if (!simple_redis::RedisConfig::validate()) {
            logger->send<simple_logger::LogLevel::ERROR>("Redis is not valid");
            return false;
        }
        if (database.empty()) {
            logger->send<simple_logger::LogLevel::ERROR>("Database is empty");
            return false;
        }
        if (table.empty()) {
            logger->send<simple_logger::LogLevel::ERROR>("Table is empty");
            return false;
        }
        return true;
    }

    /**
     * Converts the configuration to JSON format.
     *
     * This method serializes the PolygonIO and Redis configuration settings into a JSON object.
     *
     * @return A JSON object containing the configuration settings.
     */
    json CollectorConfig::to_json() const {
        json j;
        j["polygonio"] = simple_polygon_io::config::PolygonIOConfig::to_json();
        j["redis"] = simple_redis::RedisConfig::to_json();
        j["informer_interval"] = informer_interval.count();
        j["producer_interval"] = producer_interval.count();
        j["max_queue_size"] = max_queue_size;
        j["collect_interval"] = collect_interval.count();
        j["database"] = database;
        j["table"] = table;

        return j;
    }

    /**
     * Populates the configuration from a JSON object.
     *
     * This method deserializes the PolygonIO and Redis configuration settings from a given JSON object.
     *
     * @param j The JSON object containing the configuration settings.
     */
    void CollectorConfig::from_json(const json &j) {
        if (j.empty()) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing CollectorConfig: empty JSON");
            throw simple_config::ConfigException("Error parsing CollectorConfig: empty JSON");
        }
        if (!j.contains("polygonio")) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing CollectorConfig: polygonio not found");
            throw simple_config::ConfigException("Error parsing CollectorConfig: polygonio not found");
        }
        if (!j.contains("redis")) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing CollectorConfig: redis not found");
            throw simple_config::ConfigException("Error parsing CollectorConfig: redis not found");
        }
        try {
            simple_polygon_io::config::PolygonIOConfig::from_json(j["polygonio"]);
            simple_redis::RedisConfig::from_json(j["redis"]);
            if (j.contains("informer_interval")) {
                informer_interval = std::chrono::milliseconds(j["informer_interval"].get<int>());
            }
            if (j.contains("producer_interval")) {
                producer_interval = std::chrono::milliseconds(j["producer_interval"].get<int>());
            }
            if (j.contains("max_queue_size")) {
                max_queue_size = j["max_queue_size"].get<size_t>();
            }
            if (j.contains("collect_interval")) {
                collect_interval = std::chrono::seconds(j["collect_interval"].get<int>());
            }
            if (j.contains("database")) {
                database = j["database"].get<std::string>();
            }
            if (j.contains("table")) {
                table = j["table"].get<std::string>();
            }
        } catch (json::exception &e) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing CollectorConfig: " + std::string(e.what()));
            throw e;
        }
    }

    /**
     * Converts the configuration to a string representation.
     *
     * This method provides a textual representation of the PolygonIO and Redis configuration settings,
     * which includes the API URL and the API key.
     *
     * @return A string representing the configuration settings.
     */
    std::string CollectorConfig::to_string() const {
        return (std::string) "CollectorConfig {" + simple_polygon_io::config::PolygonIOConfig::to_string() + ", " +
               simple_redis::RedisConfig::to_string() + "}";
    }


    /**
 * Validates the configuration.
 *
 * This method validates the PolygonIO and Redis configuration settings.
 *
 * @return True if the configuration is valid, false otherwise.
 */
    bool ForwarderConfig::validate() {
        if (!simple_polygon_io::config::PolygonIOConfig::validate()) {
            logger->send<simple_logger::LogLevel::ERROR>("PolygonIO is not valid");
            return false;
        }
        if (!simple_redis::RedisConfig::validate()) {
            logger->send<simple_logger::LogLevel::ERROR>("Redis is not valid");
            return false;
        }
        if (!simple_kafka::config::KafkaConfig::validate()) {
            logger->send<simple_logger::LogLevel::ERROR>("Kafka is not valid");
            return false;
        }
        if (redis_key.empty()) {
            logger->send<simple_logger::LogLevel::ERROR>("Redis key is empty");
            return false;
        }
        return true;
    }

    /**
     * Converts the configuration to JSON format.
     *
     * This method serializes the PolygonIO and Redis configuration settings into a JSON object.
     *
     * @return A JSON object containing the configuration settings.
     */
    json ForwarderConfig::to_json() const {
        json j;
        j["polygonio"] = simple_polygon_io::config::PolygonIOConfig::to_json();
        j["redis"] = simple_redis::RedisConfig::to_json();
        j["kafka"] = simple_kafka::config::KafkaConfig::to_json();
        j["informer_interval"] = informer_interval.count();
        j["producer_interval"] = producer_interval.count();
        j["max_queue_size"] = max_queue_size;
        j["collect_interval"] = collect_interval.count();
        j["redis_key"] = redis_key;

        return j;
    }

    /**
     * Populates the configuration from a JSON object.
     *
     * This method deserializes the PolygonIO and Redis configuration settings from a given JSON object.
     *
     * @param j The JSON object containing the configuration settings.
     */
    void ForwarderConfig::from_json(const json &j) {
        if (j.empty()) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing ForwarderConfig: empty JSON");
            throw simple_config::ConfigException("Error parsing ForwarderConfig: empty JSON");
        }
        if (!j.contains("polygonio")) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing ForwarderConfig: polygonio not found");
            throw simple_config::ConfigException("Error parsing ForwarderConfig: polygonio not found");
        }
        if (!j.contains("redis")) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing ForwarderConfig: redis not found");
            throw simple_config::ConfigException("Error parsing ForwarderConfig: redis not found");
        }
        if (!j.contains("kafka")) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing ForwarderConfig: kafka not found");
            throw simple_config::ConfigException("Error parsing ForwarderConfig: kafka not found");
        }
        try {
            simple_polygon_io::config::PolygonIOConfig::from_json(j["polygonio"]);
            simple_redis::RedisConfig::from_json(j["redis"]);
            simple_kafka::config::KafkaConfig::from_json(j["kafka"]);

            informer_interval = std::chrono::milliseconds(j.at("informer_interval").get<int>());
            producer_interval = std::chrono::milliseconds(j.at("producer_interval").get<int>());
            max_queue_size = j.at("max_queue_size").get<size_t>();
            collect_interval = std::chrono::seconds(j.at("collect_interval").get<int>());
            redis_key = j.at("redis_key").get<std::string>();

        } catch (json::exception &e) {
            logger->send<simple_logger::LogLevel::ERROR>("Error parsing ForwarderConfig: " + std::string(e.what()));
            throw e;
        }
    }

    /**
     * Converts the configuration to a string representation.
     *
     * This method provides a textual representation of the PolygonIO and Redis configuration settings,
     * which includes the API URL and the API key.
     *
     * @return A string representing the configuration settings.
     */
    std::string ForwarderConfig::to_string() const {
        return (std::string) R"("ForwarderConfig":)" + this->to_json().dump();
    }

}
