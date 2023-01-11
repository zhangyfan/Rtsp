/******************************************************************************
 *  @file: rwalg_reasoning.h
 *  @brief: rwalg_reasoning
 *  @author:
 *  @date:
 *  @note
     Copyright 2021, Reconova Corporation, Limited
                         ALL RIGHTS RESERVED
 ******************************************************************************/

#ifndef __RWALG_REASONING_H__
#define __RWALG_REASONING_H__
#include "../rwalg_common_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum RWALG_REASONING_TASK_TYPE_ENUM
{
    RWALG_REASONING_TASK_CLASSIFY = 0,      /* 分类任务 */
    RWALG_REASONING_TASK_DETECT             /* 探测任务 */
} RWALG_REASONING_TASK_TYPE_E;

typedef struct RWALG_REASONING_CLASSIFY_RESULT_ST
{
    int class_id;       //类别id
    float conf;         //分类置信度
} RWALG_REASONING_CLASSIFY_RESULT_S;

typedef struct RWALG_REASONING_TARGET_ST
{
    int class_id;       //类别id
    float conf;         //分类置信度
    RWALG_RECT_S rect;  //目标框
} RWALG_REASONING_TARGET_S;

typedef struct RWALG_REASONING_DETECT_RESULT_ST
{
    int target_num;
    RWALG_REASONING_TARGET_S target[100];
} RWALG_REASONING_DETECT_RESULT_S;

typedef struct RWALG_REASONING_RESULT_ST
{
    RWALG_REASONING_TASK_TYPE_E task_type;
    union
    {
        RWALG_REASONING_CLASSIFY_RESULT_S classify_res;
        RWALG_REASONING_DETECT_RESULT_S detect_res;
    };
} RWALG_REASONING_RESULT_S;

typedef struct RWALG_REASONING_MODEL_LABEL_ST
{
    int label_num;
    char label[50][100];
} RWALG_REASONING_MODEL_LABEL_S;


/**
 * @fn          rwalg_reasoning_detect
 * @brief       推理探测一帧图像
 * @param[in]   handle 算法处理单元句柄
                frame  一张静态图 *仅支持RWALG_IMAGE_BGR
 * @param[out]  result 推理结果
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_reasoning_detect(RWALG_HANDLE_T handle, RWALG_FRAME_S* frame, RWALG_REASONING_RESULT_S* result);

/**
 * @fn          rwalg_getModelLabel
 * @brief       获取模型的标签，顺序与探测得到的class_id一致
 * @param[in]   handle 算法处理单元句柄
 * @param[out]  model_label 推理结果
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_getModelLabel(RWALG_HANDLE_T handle, RWALG_REASONING_MODEL_LABEL_S* model_label);

#ifdef __cplusplus
}
#endif

#endif /*__RWALG_REASONING_H__*/
