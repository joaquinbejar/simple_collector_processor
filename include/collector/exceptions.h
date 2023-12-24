//
// Created by Joaquin Bejar Garcia on 18/11/23.
//

#ifndef COLLECTOR_EXCEPTIONS_H
#define COLLECTOR_EXCEPTIONS_H

#include <exception>
#include <string>
#include "simple_logger/logger.h"

namespace collector::exceptions {
    class CollectorException : public std::exception {
    public:
        explicit CollectorException(const std::shared_ptr<simple_logger::Logger> &logger, const std::string &msg);

        [[nodiscard]] const char *what() const noexcept override;

        const std::string &m_msg;
        std::shared_ptr<simple_logger::Logger> m_logger;
    };
}
#endif //COLLECTOR_EXCEPTIONS_H
