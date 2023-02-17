/**
* @file Result.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-02-11
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _RESULT_H_
#define _RESULT_H_
#pragma once
#include <string>

typedef struct st_Result
{
    std::string label;
    double conf;
    int lx;
    int ly;
    int rx;
    int ry;
} Result;


#endif //_RESULT_H_