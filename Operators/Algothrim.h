/**
* @file Algothrim.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-02-07
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _ALGOTHRIM_H_
#define _ALGOTHRIM_H_
#pragma once
#include <string>
#include <vector>
#include "Result.h"

class Algothrim 
{
public:
    Algothrim(std::string_view path, std::string_view algoId);
    ~Algothrim();

    bool init();
    std::vector<Result> detect(unsigned char *BGR888, int width, int height);

private:
    class impl;
    impl *impl_;
};

#endif //_ALGOTHRIM_H_