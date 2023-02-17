#include <string>
#include <memory>
#include <filesystem>
#include <fstream>
#include <gflags/gflags.h>
#include "json.hpp"
#include "logger.h"

typedef struct _st_Config {
    std::string inputURL;
    std::string outputPath;
    int websocketPort;
    int rtspPort;
} Config;

//gflags定义
DECLARE_string(id);
DECLARE_string(directory);

static std::shared_ptr<Config> g_config;

bool loadConfig(std::string_view cameraId) {
    std::filesystem::path dir(FLAGS_directory);

    dir = dir / FLAGS_id / "task.json";

    if (!std::filesystem::exists(dir)) {
        LOG_ERROR("Error on load task.json, [{}] not exists", dir.u8string());
        return false;
    }

    nlohmann::json root;
    std::fstream fs;

    fs.open(dir.u8string());
    if (!fs.is_open()) {
        LOG_ERROR("Error on load task.json, [{}] can not open", dir.u8string());
        return false;
    }

    g_config = std::make_shared<Config>();

    try {
        root = nlohmann::json::parse(fs);

        //Input rtsp
        g_config->inputURL = root["RTSPUrl"];

        //输出RTSP的路径
        auto rtspNode        = root["Rtsp"];

        g_config->outputPath = rtspNode["Path"];
        g_config->rtspPort   = rtspNode["Port"];

        //websocket
        auto wbNode = root["Websocket"];

        g_config->websocketPort = wbNode["Port"];
        return true;
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("Error on read task.json [{}]", e.what());
    }
   
    return false;
}

std::shared_ptr<Config> getConfig() {
    return g_config;
}
