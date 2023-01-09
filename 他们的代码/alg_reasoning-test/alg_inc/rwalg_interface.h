#ifndef __RWALG_INTERFACE_H__
#define __RWALG_INTERFACE_H__

#include "rwalg_common_struct.h"

/**
 * @fn          rwalg_init_sdk
 * @brief       初始化SDK
 * @param[in]   param 初始化参数
 * @param[out]  null
 * @return      RET_OK success, RET_ERROR fail
 */
RWALG_API RWALG_ERR_CODE_T rwalg_init_sdk(void);

/**
 * @fn          rwalg_deinit_sdk
 * @brief       反初始化SDK
 * @param[in] null
 * @param[out]  null
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_deinit_sdk(void);

/**
 * @fn          rwalg_get_sdk_version
 * @brief       获取sdk版本号
 * @param[in] null
 * @param[out]  version
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_get_sdk_version(char* version);

/**
 * @fn          rwalg_init_handle
 * @brief       初始化算法处理单元
 * @param[in] param 算法处理单元初始化参数
 * @param[out]  handle 算法处理单元句柄
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_init_handle(RWALG_HANDLE_OPTPARAM_S param, RWALG_HANDLE_T* handle);

/**
 * @fn          rwalg_deinit_handle
 * @brief       去初始化算法处理单元
 * @param[in] null
 * @param[out]  handle 算法处理单元句柄
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_deinit_handle(RWALG_HANDLE_T handle);

/**
 * @fn          rwalg_get_version
 * @brief       获取算法处理单元版本号
 * @param[in] handle 需要查询的算法处理单元句柄
 * @param[out]  alg_version 内存应用申请，长度64字节
 *                        model_version 内存应用申请，长度64字节
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_get_version(RWALG_HANDLE_T handle, char* alg_version, char* model_version);

/**
 * @fn          rwalg_get_image_data
 * @brief       获取图像数据
 * @param[in]   image_h 图像句柄
 * @param[out]  image_info 图片数据信息
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_get_image_data(RWALG_IMAGE_T image_h, RWALG_IMAGE_INFO_S* image_info);

/**
 * @fn          rwalg_get_feature_data
 * @brief       获取图像数据
 * @param[in]   feature_h
 * @param[out]  feature_info 特征数据信息
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_get_feature_data(RWALG_FEATURE_T feature_h, RWALG_FEATURE_INFO_S* feature_info);

/**
 * @fn          rwalg_free
 * @brief      通用数据释放接口，释放rwalg中申请的数据
 * @param[in]   p 需要释放的指针
 * @param[out]  null
 * @return      RWALG_SUCCESS:成功,RWALG_FAILURE:错误
 */
RWALG_API RWALG_ERR_CODE_T rwalg_free(void *p);


#endif /*__RWALG_INTERFACE_H__*/
