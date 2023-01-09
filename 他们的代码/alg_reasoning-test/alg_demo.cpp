#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "rwalg_interface.h"
#include "rwalg_reasoning/rwalg_reasoning.h"

#define printf_err(format, ...) fprintf(stderr, "(%s:%d):" format, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define printf_debug(format, ...) fprintf(stdout, "(%s:%d):" format, __FUNCTION__, __LINE__, ##__VA_ARGS__)

int main(int argc, const char**argv)
{
#if 0
    //e.g: alg_demo xxx.bgr
    if(argc != 3)
    {
        printf_err("e.g: alg_demo xxx.bgr\n");
        return -1;
    }
    const char *pPicPath = argv[1];
#endif

    //1.init sdk
    RWALG_ERR_CODE_T ret = rwalg_init_sdk(); //全局复用,一个算法一个init,核心依赖的其实是模型与json配置
    if(ret != RWALG_SUCCESS)
    {
        printf_err("rwalg_init_sdk failed: %d\n", ret);
        return -1;
    }

    //2.init alg handle
    RWALG_HANDLE_OPTPARAM_S alg_param;
    alg_param.handle_type = RWALG_HANDLE_TYPE_REASONING;
    alg_param.reasoning_config.config_path = "./alg_res/alg_config.json";//这个就是算法配置出来的
    alg_param.reasoning_config.model_path = "./alg_res/xxx.model";//这个是固定的
    RWALG_HANDLE_T reasoning_handle;  //算法处理单元句柄
    ret = rwalg_init_handle(alg_param, &reasoning_handle);
    if(ret != RWALG_SUCCESS)    
    {
        printf_err("rwalg_init_handle failed: %d\n", ret);
        return -1;
    }

    //3.get version
    char alg_version[1024];
    char model_version[1024];
    if( RWALG_SUCCESS == rwalg_get_version(reasoning_handle, alg_version, model_version) ) //算法版本获取的意义是?我看这些值没有地方使用
    {
        printf_debug("alg_version:%s, model_version:%s\n", ret);//目的在于打印日志
    }

    //4.get model label.获取该模型能推理那些标签
    RWALG_REASONING_MODEL_LABEL_S model_label;
    if( RWALG_SUCCESS == rwalg_getModelLabel(reasoning_handle, &model_label) )
    {
        printf("label_num = %d\n", model_label.label_num);
        for(int i=0; i<model_label.label_num; ++i)
        {
            printf("label[%d]:%s", i, model_label.label[i]);
        }
    }

    //5.detect.
    constexpr int bgrWidth = 600;
    constexpr int bgrHeight = 600;
    char *bgrBuf = new char[bgrWidth*bgrHeight*3];
    RWALG_FRAME_S frame;
    frame.addr_type = RWALG_FRAME_SOURCE_DATA;
    frame.width = bgrWidth;
    frame.height = bgrHeight;
    frame.type = RWALG_IMAGE_BGR;
    frame.delegate_flag = 0;
    frame.frame_id = 0;
    frame.data = (unsigned char*)bgrBuf;
    RWALG_REASONING_RESULT_S reasoning_res;
    if( RWALG_SUCCESS == rwalg_reasoning_detect(reasoning_handle, &frame, &reasoning_res) )
    {
        if(reasoning_res.task_type == RWALG_REASONING_TASK_CLASSIFY)//分类的,比如判断是否是XXX 结果是 是或者否
        {
            printf("classify_res: class_id=%d, label=%s, conf=%.3f\n",
                   reasoning_res.classify_res.class_id, model_label.label[reasoning_res.classify_res.class_id], reasoning_res.classify_res.conf);
        }else
        {
            // 标签式的算法,某一个东西可能有好几个标签
            printf("detect_res: target_num=%d\n", reasoning_res.detect_res.target_num);
            for(int i=0; i<reasoning_res.detect_res.target_num; ++i) 
            {
                printf("target[%d]: class_id=%d, label=%s, conf=%.3f, rect(%d,%d)(%d,%d)\n",
                       i, reasoning_res.detect_res.target[i].class_id, model_label.label[reasoning_res.detect_res.target[i].class_id], reasoning_res.detect_res.target[i].conf,
                        reasoning_res.detect_res.target[i].rect.left, reasoning_res.detect_res.target[i].rect.top, reasoning_res.detect_res.target[i].rect.right, reasoning_res.detect_res.target[i].rect.bottom);
            }
        }
    }

    delete [] bgrBuf;

    //6.deinit
    rwalg_deinit_handle(reasoning_handle);
    rwalg_deinit_sdk();

    return 0;
}

