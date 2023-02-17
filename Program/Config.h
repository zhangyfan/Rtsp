/**
* @file Config.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-01-31
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _CONFIG_H_
#define _CONFIG_H_
#pragma once
#include <memory>
#include <string>

typedef struct _st_Config {
    std::string inputURL;
    std::string outputPath;
    int websocketPort;
    int rtspPort;
} Config;

bool loadConfig(std::string_view cameraId);
std::shared_ptr<Config> getConfig();

#endif //_CONFIG_H_