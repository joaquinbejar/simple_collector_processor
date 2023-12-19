//
// Created by Joaquin Bejar Garcia on 26/11/23.
//

#ifndef COLLECTOR_PROCESSOR_H
#define COLLECTOR_PROCESSOR_H

#include <simple_polygon_io/client.h>
#include <simple_polygon_io/config.h>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <collector/config.h>
#include <simple_redis/fifo.h>
#include <simple_polygon_io/http.h>

namespace processor {
    using simple_polygon_io::tickers::Query;
    using simple_polygon_io::tickers::Queries;

    /**
     * @brief Class responsible for processing ticker data collection.
     *
     * Manages multiple threads for producing, consuming, and informing operations
     * related to ticker data. Handles thread synchronization and safe data exchange
     * between threads.
     */
    class CollectorProcessor {
    private:
        std::queue<Query> ticker_query_queue; ///< Queue for storing ticker queries.
        std::mutex mtx; ///< Mutex for thread synchronization.
        std::condition_variable cv; ///< Condition variable for thread management.
        std::thread producerThread; ///< Thread for producing ticker data.
        std::thread consumerThread; ///< Thread for consuming ticker data.
        std::thread informerThread; ///< Thread for logging information.

        bool stopThreads = false; ///< Flag to control the execution of threads.
        std::atomic<size_t> enqueue_counter = 0; ///< Counter for enqueued items.
        std::atomic<size_t> dequeue_counter = 0; ///< Counter for dequeued items.
        std::atomic<size_t> sent_counter = 0; ///< Counter for sent items.
        std::atomic<size_t> remaining_time = 0; ///< Remaining time for the next collection.
        std::atomic<bool> producer_is_running = false; ///< Flag to check if producer is running.
        std::atomic<bool> consumer_is_running = false; ///< Flag to check if consumer is running.
        std::atomic<size_t> error_counter = 0; ///< Counter for errors encountered.

        collector::config::CollectorConfig &m_config; ///< Configuration object.

        void producer(); ///< Function to produce ticker data.
        void consumer(); ///< Function to consume ticker data.
        void informer(); ///< Function to log information.

    public:
        /**
         * @brief Constructs a CollectorProcessor with given configuration.
         * @param config Configuration object for the processor.
         */
        explicit CollectorProcessor(collector::config::CollectorConfig &config);

        CollectorProcessor() = delete;

        CollectorProcessor(const CollectorProcessor &other) = delete;

        CollectorProcessor &operator=(const CollectorProcessor &other) = delete;

        CollectorProcessor(CollectorProcessor &&other) noexcept = delete;

        CollectorProcessor &operator=(CollectorProcessor &&other) noexcept = delete;

        /**
         * @brief Destructor for CollectorProcessor.
         *
         * Ensures that all threads are properly joined before destruction.
         */
        ~CollectorProcessor();

        /**
         * @brief Stops the processing threads.
         *
         * Signals the threads to stop and waits for them to finish execution.
         */
        void stop();

        /**
         * @brief Starts the processing threads.
         *
         * Initializes and starts the producer, consumer, and informer threads.
         */
        void start();

        /**
         * @brief Returns the current size of the ticker queue.
         * @return Size of the queue.
         */

        size_t size();

        /**
         * @brief Checks if the processor and consumer are running.
         * @return True if the processor is running, false otherwise.
         */
        [[nodiscard]] bool is_running() const;

    };

}
#endif //COLLECTOR_PROCESSOR_H
