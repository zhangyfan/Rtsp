/**
* @file Operators.h
* @author 张一帆
* @brief 算子调度器
* @version 0.1
* @date 2023-01-11
*
* @copyright Copyright (c) 2023
*
*/
#ifndef _OPERATORS_H_
#define _OPERATORS_H_
#pragma once
#include "Operators_global.h"

class OPERATORS_EXPORT Operators {
public:
    Operators();
    ~Operators();

    bool init();

    /**
     * @brief 调用算法
     * 
     * @param BGR888   BGR图像
     * @param width    宽度
     * @param height   高度
     * @param YUV      YUV图像，用于画线
     * @return true 
     * @return false 
     */
    bool detect(unsigned char *BGR888, int width, int height, unsigned char *YUV420);

private:
    class impl;
    impl *impl_;
};

#endif //_OPERATORS_H_