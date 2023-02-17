/**
* @file Task.h
* @author 张一帆
* @brief
* @version 0.1
* @date 2023-02-07
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _TASK_H_
#define _TASK_H_
#pragma once
#include <string>
#include <vector>
#include "Result.h"

class Task
{
public:
    Task(std::string_view tasksPath, std::string_view taskId);
    ~Task();

    bool init();
    std::vector<Result> detect(unsigned char *BGR888, int width, int height);

private:
    class impl;
    impl *impl_;
};

#endif //_TASK_H_