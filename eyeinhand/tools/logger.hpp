#ifndef TOOLS__LOGGER_HPP
#define TOOLS__LOGGER_HPP

#include <fmt/chrono.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace tools
{
// 该函数只应该在main函数中使用！
void set_logger()
{
  using std::chrono::system_clock;
  using std::chrono::time_point;
  time_point<system_clock> t = system_clock::now();
  auto file_name = fmt::format("logs/{:%Y-%m-%d_%H:%M:%S}.log", t);
  auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_name, true);
  file_sink->set_level(spdlog::level::debug);

  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  console_sink->set_level(spdlog::level::debug);

  auto logger =
    std::make_shared<spdlog::logger>("", spdlog::sinks_init_list{file_sink, console_sink});
  logger->set_level(spdlog::level::debug);

  spdlog::set_default_logger(logger);
}

}  // namespace tools

#endif  // TOOLS__LOGGER_HPP