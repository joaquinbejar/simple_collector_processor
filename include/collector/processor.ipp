//
// Created by Joaquin Bejar Garcia on 26/11/23.
//

#include <collector/processor.h>

namespace processor {

    template<typename Params, typename JsonResponse>
    void CollectorProcessor<Params,JsonResponse>::producer(const Params &params, const std::function<JsonResponse(Params)>& get_client_method) {
        producer_is_running = true;

        while (!stopThreads) {
            remaining_time = 0;
            m_config.logger->send<simple_logger::LogLevel::DEBUG>("Getting records...");
            JsonResponse json_response = get_client_method(params); // call API
            Queries queries = json_response.queries(m_config.table); // create queries m_config.connection_options->tag
            std::for_each(queries.begin(), queries.end(), [this](const Query &query) {
                size_t queue_size = size();
                if (queue_size >= m_config.max_queue_size) {
                    m_config.logger->send<simple_logger::LogLevel::DEBUG>(
                            "Queue is full: " + std::to_string(queue_size) + " items...");
                    std::this_thread::sleep_for(m_config.producer_interval);
                }
                std::unique_lock<std::mutex> lock(mtx);
                ticker_query_queue.push(query);
                enqueue_counter++;
                cv.notify_one();
                lock.unlock();
            });
            // sleep for COLLECT_INTERVAL_SEC
            for (int i = 0; i < m_config.collect_interval.count() && !stopThreads; ++i) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                remaining_time = m_config.collect_interval.count() - i;
            }
        }
        producer_is_running = false;
        m_config.logger->send<simple_logger::LogLevel::NOTICE>("Producer stopped");
    }

    template<typename Params, typename JsonResponse>
    void CollectorProcessor<Params,JsonResponse>::consumer() {
        consumer_is_running = true;
        simple_redis::FIFORedisClient redis_client(m_config);
        redis_client.connect();
        if (!redis_client.is_connected()) {
            m_config.logger->send<simple_logger::LogLevel::ERROR>("Redis client not connected");
            return;
        }
        Query query;
        while (!stopThreads || !ticker_query_queue.empty() || producer_is_running) {
            std::unique_lock<std::mutex> lock_read(mtx);
            if (!ticker_query_queue.empty()) {
                query = ticker_query_queue.front();
                ticker_query_queue.pop();
                dequeue_counter++;
            } else {
                query = "";
            }
            lock_read.unlock();
            if (!query.empty()) {
                if (redis_client.set(m_config.database, query)) {  // send query to redis with database tag
                    m_config.logger->send<simple_logger::LogLevel::DEBUG>("Query: " + query);
                    sent_counter++;
                } else {
                    error_counter++;
                    m_config.logger->send<simple_logger::LogLevel::ERROR>("Error inserting query: " + query);
                    {
                        std::unique_lock<std::mutex> lock_write(mtx);
                        ticker_query_queue.push(query);

                        cv.notify_one();
                        lock_write.unlock();
                    }
                }
            }
        }
        m_config.logger->send<simple_logger::LogLevel::NOTICE>("Consumer stopped");
        consumer_is_running = false;
    }

    template<typename Params, typename JsonResponse>
    CollectorProcessor<Params,JsonResponse>::CollectorProcessor(collector::config::CollectorConfig &config)
            : m_config(config) {
    }

    template<typename Params, typename JsonResponse>
    CollectorProcessor<Params,JsonResponse>::~CollectorProcessor() {
        if (stopThreads)
            return;
        stop();
    }

    template<typename Params, typename JsonResponse>
    void CollectorProcessor<Params,JsonResponse>::stop() {
        stopThreads = true;
        cv.notify_all();
        if (producerThread.joinable()) {
            producerThread.join();
        }
        if (consumerThread.joinable()) {
            consumerThread.join();
        }
        if (informerThread.joinable()) {
            informerThread.join();
        }
    }

    template<typename Params, typename JsonResponse>
    bool CollectorProcessor<Params,JsonResponse>::is_running() const {
        return !stopThreads || producer_is_running || consumer_is_running;
    }

    template<typename Params, typename JsonResponse>
    void CollectorProcessor<Params,JsonResponse>::start(const Params &params, const std::function<JsonResponse(Params)>& getClientMethod) {
        stopThreads = false;
        producerThread = std::thread(&CollectorProcessor<Params,JsonResponse>::producer, this, params, getClientMethod);
        consumerThread = std::thread(&CollectorProcessor<Params,JsonResponse>::consumer, this);
        informerThread = std::thread(&CollectorProcessor<Params,JsonResponse>::informer, this);
    }

    template<typename Params, typename JsonResponse>
    void CollectorProcessor<Params,JsonResponse>::informer() {
        size_t prev_enqueue_counter = 0;
        size_t prev_dequeue_counter = 0;
        size_t max_enqueue_speed = 0;
        size_t max_dequeue_speed = 0;
        std::this_thread::sleep_for(std::chrono::seconds(3));
        while (!stopThreads || producer_is_running || consumer_is_running) {
            size_t diff_enqueue = enqueue_counter - prev_enqueue_counter;
            size_t diff_dequeue = dequeue_counter - prev_dequeue_counter;
            auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                    m_config.informer_interval).count();
            if (milliseconds == 0)
                milliseconds = 1;

            size_t enqueue_speed = (diff_enqueue / milliseconds) * 1000;
            size_t dequeue_speed = (diff_dequeue / milliseconds) * 1000;
            if (enqueue_speed > max_enqueue_speed)
                max_enqueue_speed = enqueue_speed;
            if (dequeue_speed > max_dequeue_speed)
                max_dequeue_speed = dequeue_speed;

            m_config.logger->send<simple_logger::LogLevel::INFORMATIONAL>(
                    "Sent: " + std::to_string(sent_counter) + " " +
                    "Queue size: " + std::to_string(size()) + " " +
                    "Enqueue: " + std::to_string(enqueue_counter) +
                    " Speed: " + std::to_string(enqueue_speed) + "/sec, " +
                    "Max speed: " + std::to_string(max_enqueue_speed) + "/sec, " +
                    "Dequeue: " + std::to_string(dequeue_counter) +
                    " Speed: " + std::to_string(dequeue_speed) + "/sec " +
                    "Max speed: " + std::to_string(max_dequeue_speed) + "/sec, " +
                    "Errors: " + std::to_string(error_counter) + " " +
                    "Remaining time: " + std::to_string(remaining_time) + " sec"
                    , true);

            prev_enqueue_counter = enqueue_counter;
            prev_dequeue_counter = dequeue_counter;
            std::this_thread::sleep_for(m_config.informer_interval);
        }
    }

    template<typename Params, typename JsonResponse>
    size_t CollectorProcessor<Params,JsonResponse>::size() {
        std::unique_lock<std::mutex> lock(mtx);
        size_t size = ticker_query_queue.size();
        cv.notify_one();
        lock.unlock();
        return size;
    }

}


