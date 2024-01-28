//
// Created by Joaquin Bejar Garcia on 28/1/24.
//

#ifndef COLLECTOR_KAFKAFORWARDER_H
#define COLLECTOR_KAFKAFORWARDER_H

#include <common/common.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <utility>
#include <trading_common/instructions.h>
#include <collector/config.h>
#include <string>

namespace forwarder {

    typedef std::string query_t;
    typedef std::vector<std::string> queries_t;

    using namespace trading::instructions;

    template<typename InstructionType>
    class InstructionsExecutorAndForwarder {

    private:
        collector::config::ForwarderConfig m_config;

        ::common::ThreadQueue<Instructions<InstructionType>> m_queue_instructions;
        ::common::ThreadQueueWithMaxSize<query_t> m_queue_queries = ::common::ThreadQueueWithMaxSize<query_t>(
                m_config.max_queue_size);


        std::thread m_instructor_consumer_thread;
        std::thread m_instructor_executor_thread;
        std::thread m_query_forwarder_thread;
        std::thread m_informer_thread;

        std::atomic<bool> m_instructor_consumer_is_running = false;
        std::atomic<bool> m_instructor_executor_is_running = false;
        std::atomic<bool> m_query_forwarder_is_running = false;
        std::atomic<bool> m_informer_is_running = false;
        std::atomic<bool> m_stop_threads = false;

        std::atomic<size_t> m_queue_instructions_enqueue_counter = 0;
        std::atomic<size_t> m_queue_instructions_enqueue_errors = 0;
        std::atomic<size_t> m_queue_instructions_dequeue_counter = 0;
        std::atomic<size_t> m_queue_instructions_dequeue_errors = 0;

        std::atomic<size_t> m_queue_queries_enqueue_counter = 0;
        std::atomic<size_t> m_queue_queries_enqueue_errors = 0;
        std::atomic<size_t> m_queue_queries_dequeue_counter = 0;
        std::atomic<size_t> m_queue_queries_dequeue_errors = 0;

        std::atomic<size_t> m_redis_counter = 0;
        std::atomic<size_t> m_redis_errors = 0;

        void m_instructor_consumer(std::function<Instructions<InstructionType>()> &instructor_consumer_context) {
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Instructor Consumer started");
            m_instructor_consumer_is_running = true;
            while (!m_stop_threads) {
                // TASK: Get instructions from kafka and put them in m_queue_instructions
                auto instruction = instructor_consumer_context(); // get instruction from kafka
                if (m_queue_instructions.enqueue(instruction)) {
                    m_queue_instructions_enqueue_counter++;
                } else {
                    m_queue_instructions_enqueue_errors++;
                }
            }
            m_instructor_consumer_is_running = false;
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Instructor Consumer stopped");
        }

        void m_instructor_executor(
                const std::function<queries_t(Instructions<InstructionType>)> &instructor_executor_context) {
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Instructor Executor started");
            m_instructor_executor_is_running = true;
            while (!m_stop_threads || m_instructor_consumer_is_running || !m_queue_instructions.empty()) {
                // TASK: Get instructions from m_queue_instructions and execute them,
                // get data from polygon and put it in m_queue_queries as a query_t
                Instructions<InstructionType> instruction;
                queries_t queries;
                if (m_queue_instructions.dequeue_blocking(instruction)) {
                    queries = instructor_executor_context(instruction); // execute instruction
                    m_queue_instructions_dequeue_counter++;
                } else {
                    m_queue_instructions_dequeue_errors++;
                    continue;
                }
                std::for_each(queries.begin(), queries.end(), [&](const query_t &query) {
                    if (m_queue_queries.enqueue(query)) {
                        m_queue_queries_enqueue_counter++;
                    } else {
                        m_queue_queries_enqueue_errors++;
                    }
                });
            }
            m_instructor_executor_is_running = false;
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Instructor Executor stopped");
        }

        void m_query_forwarder(const std::function<bool(query_t)> &query_forwarder_context) {
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Query Forwarder started");
            m_query_forwarder_is_running = true;
            while (!m_stop_threads || m_instructor_executor_is_running || !m_queue_queries.empty()) {
                // TASK: Get query_t from m_queue_queries and send it to Redis to insert in DB
                query_t query;
                if (m_queue_queries.dequeue_blocking(query)) {
                    m_queue_queries_dequeue_counter++;
                    // send query to redis
                    if (query_forwarder_context(query)) {
                        // query sent to redis
                        m_redis_counter++;

                    } else {
                        m_redis_errors++;
                    }
                } else {
                    m_queue_queries_dequeue_errors++;
                    continue;
                }
            }
            m_query_forwarder_is_running = false;
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Query Forwarder stopped");
        }

        void m_informer(void *informer_context) {
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Informer started");
            m_informer_is_running = true;
            while (!m_stop_threads || m_instructor_consumer_is_running || m_instructor_executor_is_running ||
                   m_query_forwarder_is_running) {
                // TASK: Get stats from queues and threads and show them in console
                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("\033[2J\033[1;1H",true);

                std::string is_running;
                if (m_instructor_consumer_is_running)
                    is_running += "(Instructor Consumer)";
                if (m_instructor_executor_is_running)
                    is_running += "(Instructor Executor)";
                if (m_query_forwarder_is_running)
                    is_running += "(Query Forwarder)";
                if (m_informer_is_running)
                    is_running += "(Informer)";
                if (is_running.empty())
                    is_running = "None";
                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("Threads Running: " + is_running);

                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("Q_instructions size: " +
                                                                              std::to_string(
                                                                                      m_queue_instructions.size()) +
                                                                              " Q_queries size: " +
                                                                              std::to_string(m_queue_queries.size()));


                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("Q_Instructions enqueue counter: " +
                                                                              std::to_string(
                                                                                      m_queue_instructions_enqueue_counter) +
                                                                              " errors: " +
                                                                              std::to_string(
                                                                                      m_queue_instructions_enqueue_errors));
                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("Q_Instructions dequeue counter: " +
                                                                              std::to_string(
                                                                                      m_queue_instructions_dequeue_counter) +
                                                                              " errors: " +
                                                                              std::to_string(
                                                                                      m_queue_instructions_dequeue_errors));


                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("Q_Queries enqueue counter: " +
                                                                              std::to_string(
                                                                                      m_queue_queries_enqueue_counter) +
                                                                              " errors: " +
                                                                              std::to_string(
                                                                                      m_queue_queries_enqueue_errors));
                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("Q_Queries dequeue counter: " +
                                                                                std::to_string(
                                                                                        m_queue_queries_dequeue_counter) +
                                                                                " errors: " +
                                                                                std::to_string(
                                                                                        m_queue_queries_dequeue_errors));

                m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>("Redis counter: " +
                                                                              std::to_string(m_redis_counter) +
                        " Redis errors: " +
                                                                              std::to_string(m_redis_errors));

                std::this_thread::sleep_for(std::chrono::milliseconds(m_config.informer_interval));
            }
            m_informer_is_running = false;
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Informer stopped");
        }

    public:
        InstructionsExecutorAndForwarder() = delete;

        InstructionsExecutorAndForwarder(const InstructionsExecutorAndForwarder &other) = delete;

        InstructionsExecutorAndForwarder &operator=(const InstructionsExecutorAndForwarder &other) = delete;

        InstructionsExecutorAndForwarder(InstructionsExecutorAndForwarder &&other) noexcept = delete;

        InstructionsExecutorAndForwarder &operator=(InstructionsExecutorAndForwarder &&other) noexcept = delete;

        explicit InstructionsExecutorAndForwarder(collector::config::ForwarderConfig config) : m_config(
                std::move(config)) {}

        ~InstructionsExecutorAndForwarder() {
            if (m_stop_threads)
                return;
            stop();
        }

        void start(std::function<Instructions<InstructionType>()> instructor_consumer_context,
                   std::function<queries_t(Instructions<InstructionType>)> instructor_executor_context,
                   std::function<bool(query_t)> query_forwarder_context,
                   void *informer_context) {
            m_instructor_consumer_thread = std::thread(&InstructionsExecutorAndForwarder::m_instructor_consumer, this,
                                                       std::ref(instructor_consumer_context));
            m_instructor_executor_thread = std::thread(&InstructionsExecutorAndForwarder::m_instructor_executor, this,
                                                       std::ref(instructor_executor_context));
            m_query_forwarder_thread = std::thread(&InstructionsExecutorAndForwarder::m_query_forwarder, this,
                                                   std::ref(query_forwarder_context));
            m_informer_thread = std::thread(&InstructionsExecutorAndForwarder::m_informer, this,
                                            std::ref(informer_context));
        }

        void stop() {
            m_stop_threads = true;
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("Stopping InstructionsExecutorAndForwarder");
            if (m_instructor_consumer_thread.joinable())
                m_instructor_consumer_thread.join();
            if (m_instructor_executor_thread.joinable())
                m_instructor_executor_thread.join();
            if (m_query_forwarder_thread.joinable())
                m_query_forwarder_thread.join();
            if (m_informer_thread.joinable())
                m_informer_thread.join();
            m_config.logger->send<simple_logger::LogLevel::NOTICE>("InstructionsExecutorAndForwarder stopped");
        }


    };


}
#endif //COLLECTOR_KAFKAFORWARDER_H
