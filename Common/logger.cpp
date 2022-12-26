#include "logger.h"
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <filesystem>

namespace Common {
COMMON_EXPORT void InitLogger(const std::string &path, spdlog::level::level_enum defaultLevel) {
    using namespace spdlog;
    using namespace std;

    std::string loggerDir = path;
    filesystem::path dir(path);

    if (path.empty()) {
        //默认情况下写入到home目录
        dir        = filesystem::current_path();
    }

    dir = dir / "RtspProxy.txt";
    loggerDir = dir.u8string();

    //一个最大16MB的滚动日志
    auto console_sink  = std::make_shared<sinks::stdout_color_sink_mt>();    
    auto rotating_sink = std::make_shared<sinks::rotating_file_sink_mt>(loggerDir, 16*1024*1024, 1);  

    console_sink->set_level(defaultLevel);
    rotating_sink->set_level(defaultLevel);

    auto logger = std::make_shared<spdlog::logger>("RtspProxy", sinks_init_list({console_sink, rotating_sink}));

    logger->set_level(defaultLevel);
    logger->set_pattern("%^[%D %H:%M:%S.%e][%s:%#][%l] %v%$");
    logger->flush_on(level::warn);

    spdlog::flush_every(std::chrono::seconds(3));
    spdlog::set_default_logger(logger);
}
} // namespace VRV