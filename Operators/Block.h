/**
* @file Block.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-02-07
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _BLOCK_H_
#define _BLOCK_H_
#pragma once
#include <filesystem>
#include <string>
#include "Result.h"

class Block 
{
public:
    Block(std::string_view path, std::string blockId);
    ~Block();

    bool init();
    std::vector<Result> detect(unsigned char *BGR888, int width, int height);

private:
    class impl;
    impl *impl_;
};

#endif //_BLOCK_H_