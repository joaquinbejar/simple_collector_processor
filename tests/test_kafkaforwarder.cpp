//
// Created by Joaquin Bejar Garcia on 28/1/24.
//


#include "collector/config.h"
#include <catch2/catch_test_macros.hpp>
#include <collector/kafkaforwarder.h>

using namespace collector::config;
using namespace trading::instructions;
using forwarder::InstructionsExecutorAndForwarder;
using json = nlohmann::json;

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

        std::function<Instructions<TestInstruction>()> test_instruction_lambda = []() -> Instructions<TestInstruction> {
            Instructions<TestInstruction> instructions;
            return instructions;
        };

        std::function<std::string()> string_lambda = []() -> std::string {
            return "I'm string_lambda";
        };

        forwarder.start(test_instruction_lambda, string_lambda, nullptr, nullptr);
        //sleep for 30 seconds
        std::this_thread::sleep_for(std::chrono::seconds(30));
        forwarder.stop();
    }
}