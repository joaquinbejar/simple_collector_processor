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

}
