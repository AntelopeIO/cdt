#pragma once

#include <string>
#include <sstream>
#include <exception>
#include <filesystem>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/fmt/ostr.h>

namespace eosio { namespace utils {

// log to str and return it
template<typename... Args>
std::string log_to_str(const Args &... args)
{
    std::ostringstream oss;
    static auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss);
    static spdlog::logger oss_logger("ostringstream", oss_sink);
    static bool initialized = false;
    if (!initialized) {
        //this means simply use string itself
        oss_logger.set_pattern("%v");
        initialized = true;
    }

    oss_logger.info(args...);
    auto ret_val = oss.str();
    oss.str("");
    return ret_val;
}

std::shared_ptr<spdlog::logger> get_std_logger() {
    static auto logger = spdlog::stdout_color_mt("antler logger");
    bool initialized = false;
    if (!initialized) {
        //format is <color>message level</color> message
        logger->set_pattern("%^%l%$ %v");
    }
    return logger;
}

#define ANTLER_ERROR(...)                                                           \
    SPDLOG_LOGGER_ERROR(eosio::utils::get_std_logger(), __VA_ARGS__)
#define ANTLER_WARN(...)                                                            \
    SPDLOG_LOGGER_WARN(eosio::utils::get_std_logger(), __VA_ARGS__)
#define ANTLER_INFO(...)                                                            \
    SPDLOG_LOGGER_INFO(eosio::utils::get_std_logger(), __VA_ARGS__)


#define ANTLER_MULTILINE_MACRO_BEGIN do {
#define ANTLER_MULTILINE_MACRO_END  } while (0)

#define ANTLER_EXCEPTION std::runtime_error

#define ANTLER_THROW(...)                                                           \
    ANTLER_MULTILINE_MACRO_BEGIN                                                    \
    throw ANTLER_EXCEPTION( eosio::utils::log_to_str(__VA_ARGS__) );                \
    ANTLER_MULTILINE_MACRO_END

#define ANTLER_ASSERT(cond, ...)                                                    \
    ANTLER_MULTILINE_MACRO_BEGIN                                                    \
    if (!(cond))                                                                    \
        ANTLER_THROW("{}: {}", __func__, eosio::utils::log_to_str(__VA_ARGS__));    \
    ANTLER_MULTILINE_MACRO_END

#define ANTLER_QUIT(cond, ...)                                                      \
    ANTLER_MULTILINE_MACRO_BEGIN                                                    \
    if (!(cond))                                                                    \
        ANTLER_INFO("{}: {}", __func__, eosio::utils::log_to_str(__VA_ARGS__));     \
    ANTLER_MULTILINE_MACRO_END

std::ostream &operator<<(std::ostream &ost, const std::filesystem::path& p)
{
    std::string tmp = fmt::format("{}", p.native());
    ost << tmp;
    return ost;
}

}} // eosio::utils