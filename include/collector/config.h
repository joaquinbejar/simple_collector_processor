//
// Created by Joaquin Bejar Garcia on 18/11/23.
//

#ifndef COLLECTOR_CONFIG_H
#define COLLECTOR_CONFIG_H


#include <simple_logger/logger.h>
#include <common/common.h>
#include <simple_polygon_io/config.h>
#include <simple_redis/config.h>

namespace collector::config {
    class CollectorConfig : public simple_polygon_io::config::PolygonIOConfig, public simple_redis::RedisConfig {
    public:

        bool validate() override;

        [[nodiscard]] json to_json() const override;

        void from_json(const json &j) override;

        [[nodiscard]] std::string to_string() const override;

        std::shared_ptr<simple_logger::Logger> logger = simple_polygon_io::config::PolygonIOConfig::logger;

        std::chrono::milliseconds informer_interval = std::chrono::milliseconds(
                common::get_env_variable_int("INFORMER_INTERVAL_MSEC", 333));
        std::chrono::milliseconds producer_interval = std::chrono::milliseconds(
                common::get_env_variable_int("PRODUCER_INTERVAL_MSEC", 10));
        size_t max_queue_size = common::get_env_variable_int("MAX_QUEUE_SIZE", 10000);
        std::chrono::seconds collect_interval = std::chrono::seconds(
                common::get_env_variable_int("COLLECT_INTERVAL_SEC", 3600));
        std::string database = common::get_env_variable_string("DATABASE", "Polygon");
        std::string table = common::get_env_variable_string("TABLE", "");
    };
}

#endif //COLLECTOR_CONFIG_H
