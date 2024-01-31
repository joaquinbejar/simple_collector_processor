//
// Created by Joaquin Bejar Garcia on 28/1/24.
//


#include "collector/config.h"
#include <catch2/catch_test_macros.hpp>
#include <collector/kafkaforwarder.h>
#include <simple_polygon_io/instruction_executrion.h>

using namespace collector::config;
using namespace trading::instructions;
using forwarder::InstructionsExecutorAndForwarder;
using forwarder::query_t;
using json = nlohmann::json;
using simple_polygon_io::instructor::instructor_executor_context;
using simple_polygon_io::instructor::MetaInstruction;
typedef std::vector<std::string> queries_t;


class TestInstruction {
public:
    [[nodiscard]] json to_json() const {
        return {};
    }

    void from_json(const json &j) {}
};

TEST_CASE("ForwarderConfig Tests", "[ForwarderConfig]") {
    collector::config::ForwarderConfig config;

    SECTION("Default Configuration") {
        std::cout << config.to_json().dump(4) << std::endl;
        InstructionsExecutorAndForwarder forwarder = InstructionsExecutorAndForwarder<TestInstruction>(config);


        std::function<queries_t(Instructions<TestInstruction>)> strings_lambda = [](const Instructions<TestInstruction>& instruction) -> queries_t {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return {"SELECT 1", "SELECT 2"};
        };

        std::function<bool(query_t&)> redis_lambda = [](query_t& query) -> bool {
            query += ";";
            return true;
        };

        forwarder.start( strings_lambda, redis_lambda, nullptr);
        //sleep for 10 seconds
        std::this_thread::sleep_for(std::chrono::seconds(10));
        forwarder.stop();
    }
}

TEST_CASE("ForwarderConfig Tests with MetaInstruction", "[ForwarderConfig]") {
    collector::config::ForwarderConfig config;

    SECTION("Default Configuration") {
        std::cout << config.to_json().dump(4) << std::endl;
        InstructionsExecutorAndForwarder forwarder = InstructionsExecutorAndForwarder<MetaInstruction>(config);

        std::function<bool(query_t&)> redis_lambda = [](query_t& query) -> bool {
            query += ";";
            return true;
        };

        forwarder.start( instructor_executor_context, redis_lambda, nullptr);
        //sleep for 10 seconds
        std::this_thread::sleep_for(std::chrono::seconds(10));
        forwarder.stop();
    }
}