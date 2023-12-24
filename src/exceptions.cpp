//
// Created by Joaquin Bejar Garcia on 18/11/23.
//

#include "collector/exceptions.h"

namespace collector::exceptions {

    CollectorException::CollectorException(const std::shared_ptr<simple_logger::Logger> &logger,
                                                       const std::string &msg)
            : m_msg(msg), m_logger(logger) {}

    [[nodiscard]] const char *CollectorException::what() const noexcept {
        m_logger->send<simple_logger::LogLevel::ERROR>("CollectorException: " + m_msg);
        return m_msg.c_str();
    }
}