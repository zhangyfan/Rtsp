#ifndef __RWALG_COMMON_STRUCT_H__
#define __RWALG_COMMON_STRUCT_H__

#include <stdint.h>
#include "rwalg_macro.h"

typedef void* RWALG_HANDLE_T;  //算法处理单元句柄

typedef void* RWALG_IMAGE_T;     //图片句柄
typedef void* RWALG_FEATURE_T;  //特征句柄

typedef int RWALG_ERR_CODE_T;

#define    RWALG_SUCCESS                           0    // 成功
#define    RWALG_FAILURE                          -1    // 失败
#define    RWALG_ERR_BUSY                         -2    // 繁忙
#define    RWALG_ERR_LICENCE                      -3    // 授权失败
#define    RWALG_ERR_NOFACE                       -4    // 没有人脸
#define    RWALG_ERR_SIZE_TOOSMALL                -5    // 分辨率太小
#define    RWALG_ERR_SIZE_TOOLARGE                -6    // 分辨率太大
#define    RWALG_ERR_QUALITY_TOOLOW               -7    // 质量太差
#define    RWALG_ERR_NOTFOUND_DETECT_MODEL        -8    // 没有找到检测模型

typedef enum RWALG_IMAGE_TYPE_ENUM 
{
    RWALG_IMAGE_NV21 = 0,
    RWALG_IMAGE_GRAY,
    RWALG_IMAGE_BGR,
	RWALG_IMAGE_RGB,
} RWALG_IMAGE_TYPE_E;

/**
 * frame data source type
 */
typedef enum RWALG_FRAME_SOURCE_TYPE_ENUM
{
    RWALG_FRAME_SOURCE_ADDR = 0,
    RWALG_FRAME_SOURCE_DATA = 1,
	RWALG_FRAME_SOURCE_MAX,
} RWALG_FRAME_SOURCE_TYPE_E;

/*
* 检测算法模型
*/
typedef enum RWALG_DETECT_MODEL_ENUM
{
    DETECT_MODEL_576x320 = 0,
    DETECT_MODEL_288x512 = 1,
    DETECT_MODEL_MAX
} RWALG_DETECT_MODEL_E;

/**
 * supported image rotate degree
 */
typedef enum RWALG_IMAGE_ROTATION_ENUM 
{
    RWALG_ROTATION_DEG0   = 0,
    RWALG_ROTATION_DEG90  = 90,
    RWALG_ROTATION_DEG180 = 180,
    RWALG_ROTATION_DEG270 = 270,
	RWALG_ROTATION_MAX,
} RWALG_IMAGE_ROTATION_E;

/**
 * quality type
 */
typedef enum RWALG_QUALITY_ENUM
{
    RWALG_QUALITY_LOW = 0,
    RWALG_QUALITY_HIGH,
} RWALG_QUALITY_E;

/**
* track status
**/
typedef enum RWALG_TRACK_STATUS_ENUM
{
    RWALG_TRACK_STATUS_NEW = 0x00,  	    //TRACK 新建
    RWALG_TRACK_STATUS_UPDATE,  	        //TRACK 需要更新
    RWALG_TRACK_STATUS_DIE,			   	    //TRACK 已经消失
	RWALG_TRACK_STATUS_MAX,
}RWALG_TRACK_STATUS_E;

/**
 * 算法处理单元句柄类型
 */
typedef enum RWALG_HANDLE_TYPE_ENUM
{
    RWALG_HANDLE_TYPE_DETECT = 1,      // 检测
    RWALG_HANDLE_TYPE_RECOG,           // 特征提取、识别比对
    RWALG_HANDLE_TYPE_ATTR,            // 属性检测
    RWALG_HANDLE_TYPE_ANTI_SPOOF,      // 活体判断
    RWALG_HANDLE_TYPE_GATE,            // 门禁算法策略组合套件
    RWALG_HANDLE_TYPE_TOPVIEW_DETECT,  // 头顶检测
    RWALG_HANDLE_TYPE_OBJECT_DETECT,   // 物体检测
    RWALG_HANDLE_TYPE_DETECT_V2,       // 检测V2版本
    RWALG_HANDLE_TYPE_REID,            // REID算法
    RWALG_HANDLE_TYPE_IPC,             // 抓拍识别相机策略组合套件，依赖RWALG_HANDLE_TYPE_DETECT_V2
	RWALG_HANDLE_TYPE_GENDER_AGE,	   // 性别年龄检测
    RWALG_HANDLE_TYPE_REASONING,       // 推理模块
    RWALG_HANDLE_TYPE_MAX
}RWALG_HANDLE_TYPE_E;

/**
* object moving status
*/
typedef enum RWALG_TRACK_MOVING_STATUS_ENUM
{
    RWALG_MOVING_STATUS_UNKNOWN = 0,
    RWALG_MOVING_STATUS_STATIONARY,
    RWALG_MOVING_STATUS_MOVING,
}RWALG_TRACK_MOVING_STATUS_E;

typedef enum RWALG_ANTI_SPOOF_ENUM
{
    RWALG_UNKNOW = 0,            // 未知
    RWALG_LIVING,                       // 活体
    RWALG_SPOOF                        // 非活体
} RWALG_ANTI_SPOOF_E;

typedef enum RWALG_GATE_MODE_ENUM
{
    GATE_SPEED = 0,
    GATE_NORMAL = 1,
    GATE_PRECISE = 2
} RWALG_GATE_MODE_E;

typedef enum RWALG_LICENCE_ENUM
{
    LIC_ATMEL = 0,             // atmel芯片授权
    LIC_NV906 = 1,             // nv906芯片授权
    LIC_NET = 2                  // 网络授权
} RWALG_LICENCE_E;

typedef enum RWALG_REGION_TYPE_ENUM
{
    VALID_REGION = 0,     // 有效区
    MASK_REGION  = 1,     // 屏蔽区
    MAIN_REGION  = 2,     // 主要区
    CROSS_REGION = 3,     // 穿越区
    ENTRY_REGION = 4      // 入口区
} RWALG_REGION_TYPE_E;

typedef enum RWALG_ENTRY_TYPE_ENUM
{
    ENTRY_GETIN   = 0,
    ENTRY_GETOUT  = 1,
    ENTRY_INSIDE  = 2,
    ENTRY_OUTSIDE = 3
} RWALG_ENTRY_TYPE_E;

typedef struct RWALG_POINT_ST
{
    int x;
    int y;
} RWALG_POINT_S;

/**
 * rect info
 */
typedef struct RWALG_RECT_ST
{
    int left;
    int top;
    int right;
    int bottom;
} RWALG_RECT_S;

/**
 * 3D pose
 */
typedef struct RWALG_POSE_ST
{
    float roll;
    float pitch;
    float yaw;
} RWALG_POSE_S;

typedef struct RWALG_REGION_PARAM_ST
{
    char alias[10];            // 给区域定义的别名
    int points_num;            // 区域点个数
    RWALG_POINT_S points[10];  // 顺时针或者逆时针排序的区域点
} RWALG_REGION_PARAM_S;

/**
 * frame addr
 */
typedef struct RWALG_FRAME_ADDR_ST
{
    uint32_t  stride[3];                    // Y U V stride
    void* phy_addr[3];
    void* vir_addr[3];                  // Y U V分量的指针
} RWALG_FRAME_ADDR_S;

typedef void(*RWALG_FRAME_FREE_CALLBACK)(void* frame);
/**
 *  frame structure from the camera used by SDK to detect
 */
typedef struct RWALG_FRAME_ST
{
    RWALG_FRAME_SOURCE_TYPE_E       addr_type; //帧地址类型，目前只能为RWALG_FRAME_SOURCE_DATA
    union
    {
        RWALG_FRAME_ADDR_S          addr;
        unsigned char* data;
    };
    int                             width;      //帧宽
    int                             height;     //帧高
    RWALG_IMAGE_ROTATION_E  		rotation;	//目前未使用	
    RWALG_IMAGE_TYPE_E      		type;     //帧的格式，目前只支持RWALG_IMAGE_NV21
    uint64_t                        frame_id;   // 帧的序号，对于输入表示的是输入视频帧的seq
    uint64_t						frame_pts;	// 帧的时间戳，目前未用到
    int                             delegate_flag; // 将释放权限委托给SDK，0否，1是
    RWALG_FRAME_FREE_CALLBACK      frame_free_cb; // delegate_flag为1时传入释放参数
    void* user_data;  // 用户数据，SDK不会进行处理或修改
} RWALG_FRAME_S;

/**
 * face detect param
 */
typedef struct RWALG_DETECT_OPTCONFIG_ST
{
	int					face_min;
    int				    face_max;
	float					pose_roll_upper_threshold;
	float					pose_yaw_upper_threshold;
	float					pose_pitch_upper_threshold;
	float					blurriness_upper_threshold;

    RWALG_DETECT_MODEL_E det_model;

	int						reserved[16]; // 保留
} RWALG_DETECT_OPTCONFIG_S;

typedef struct RWALG_ATTR_ST
{
    float hat;                          // 帽子置信度，值越大越可能是
    float mask;                       // 口罩置信度，值越大越可能是
    float normal_glasses;       // 普通眼镜置信度，值越大越可能是
    float sunglasses;              // 太阳眼镜置信度，值越大越可能是
    float blur_degree;            // 模糊度置信度，值越大越模糊
}RWALG_ATTR_S;

typedef struct RWALG_RECOG_OPTCONFIG_ST
{
    double recog_sim_thr; //识别相似度阈值
}RWALG_RECOG_OPTCONFIG_S;

typedef struct RWALG_GATE_TRACK_INFO_ST
{
    int64_t track_id;
    RWALG_TRACK_STATUS_E track_status;
    RWALG_RECT_S rect;
    float quality;
}RWALG_GATE_TRACK_INFO_S;

typedef struct RWALG_GATE_DETECT_INFO_ST
{
    RWALG_GATE_TRACK_INFO_S* vis_track_info;
    int vis_track_info_size;
    RWALG_GATE_TRACK_INFO_S* ir_track_info;
    int ir_track_info_size;
}RWALG_GATE_DETECT_INFO_S;

typedef struct RWALG_GATE_RECOG_INFO_ST
{
    int64_t track_id;                                                    // 跟踪ID
    RWALG_FRAME_S* frame;                             // 图像帧
    RWALG_RECT_S face_rect;                             //人脸在frame图片中的坐标
    RWALG_ATTR_S attr;                                     // 人脸属性
    float quality;                                                 // 人脸质量
    int recog_size;                                              // 比中的个数，最大为top_k
    double* scores;                                             // recog_size 个比中的相似度得分
    uint64_t* faceid;                                            // recog_size 个比中的faceid
}RWALG_GATE_RECOG_INFO_S;

typedef struct RWALG_GATE_SPOOF_INFO_ST
{
    int64_t track_id;
    float quality;
}RWALG_GATE_SPOOF_INFO_S;

/*
* rwalg_gate事件回调
* 注意：业务不要在回调函数中做耗时处理
*/
typedef void(*RWALG_GATE_DETECT_EVENT_CALLBACK)(RWALG_GATE_DETECT_INFO_S* detect_info);
typedef void(*RWALG_GATE_RECOG_EVENT_CALLBACK)(RWALG_IMAGE_T image_h, RWALG_GATE_RECOG_INFO_S *recog_info);
typedef void(*RWALG_GATE_SPOOF_EVENT_CALLBACK)(RWALG_GATE_SPOOF_INFO_S* spoof_info);
typedef struct RWALG_GATE_CALLBACK_ST
{
    RWALG_GATE_DETECT_EVENT_CALLBACK detect_event_cb;
    RWALG_GATE_RECOG_EVENT_CALLBACK recog_event_cb;     // image_h回调后由用户管理，使用rwalg_free释放
    RWALG_GATE_SPOOF_EVENT_CALLBACK spoof_event_cb;     // 活体攻击回调
}RWALG_GATE_CALLBACK_S;

typedef struct RWALG_GATE_OPTCONFIG_ST
{
    bool open_correct;                                         // 启动自动标定
    char* correct_path;                                         // 自动标定文件存访路径，必须可读写
    int correct_ir_width;                                        // 启动自动标定，IR图像的宽
    int correct_ir_height;                                       // 启动自动标定，IR图像的高
    int top_k;                                                        // 满足识别相似度阈值的前提下，将相似度最高的top_k个id返回
    double recog_sim_thr;                                   // 识别相似度阈值
    int face_min;                                            // 最大有效人脸阈值
    int face_max;                                            // 最小有效人脸阈值
    int recog_interval;                                          // 识别通过后，间歇多少毫秒后开启下一次识别
    RWALG_GATE_MODE_E mode;                                // 模式
    RWALG_GATE_CALLBACK_S cbs;                   // 注册回调函数
    RWALG_DETECT_MODEL_E det_model;
}RWALG_GATE_OPTCONFIG_S;

typedef enum RWALG_OBJECT_MODE_ENUM
{
	RWALG_OBJECT_MODE_FIRE = 0,
	RWALG_OBJECT_MODE_MAX
}RWALG_OBJECT_MODE_E;

typedef struct RWALG_OBJECT_OPTCONFIG_ST
{
	RWALG_OBJECT_MODE_E detectMode;
}RWALG_OBJECT_OPTCONFIG_S;

struct RWALG_IPC_DETECT_INFO_ST;
struct RWALG_IPC_CAPTURE_INFO_ST;
typedef void (*RWALG_IPC_DETECT_CALLBACK)(RWALG_IPC_DETECT_INFO_ST* det_info);
typedef void (*RWALG_IPC_LEAVE_CAPTURE_CALLBACK)(RWALG_IPC_CAPTURE_INFO_ST* cap_info);

typedef struct RWALG_IPC_CALLBACK_ST
{
    RWALG_IPC_DETECT_CALLBACK det_cb;
    RWALG_IPC_LEAVE_CAPTURE_CALLBACK leave_cap_cb;
}RWALG_IPC_CALLBACK_S;

typedef struct RWALG_IPC_OPTCONFIG_ST
{
    RWALG_IPC_CALLBACK_S cbs;                   // 注册回调函数
} RWALG_IPC_OPTCONFIG_S;

typedef struct RWALG_REASONING_OPTCONFIG_ST
{
    char *config_path;      //算法参数配置⽂件
    char *model_path;       //加密的模型⽂件
} RWALG_REASONING_OPTCONFIG_S;

typedef struct RWALG_LICENCE_ATMEL_PARAM_ST
{
    int i2c_bus;
}RWALG_LICENCE_ATMEL_PARAM_S;

/**
 * 初始化算法handle时的参数
 */
typedef struct RWALG_HANDLE_OPTPARAM_ST
{
    RWALG_HANDLE_TYPE_E handle_type;                  // 算法handle类型
    union
    {
        RWALG_DETECT_OPTCONFIG_S detect_config;  // 算法检测参数 
        RWALG_GATE_OPTCONFIG_S gate_config;        // rwalg_gate参数
		RWALG_OBJECT_OPTCONFIG_S object_config;	//物体检测参数
        RWALG_IPC_OPTCONFIG_S ipc_config;
        RWALG_REASONING_OPTCONFIG_S reasoning_config;   //推理模块参数
    };
    RWALG_LICENCE_E licence_type;                               // 授权方式
    union
    {
        RWALG_LICENCE_ATMEL_PARAM_S atmel_param; // LIC_ATMEL配置参数
    };
    char* model_path;                                                    // 模型路径
}RWALG_HANDLE_OPTPARAM_S;

typedef struct RWALG_IMAGE_INFO_ST
{
    uint8_t* data;
    int width;
    int height;
    int size;
    RWALG_IMAGE_TYPE_E type;
}RWALG_IMAGE_INFO_S;

typedef struct RWALG_FEATURE_INFO_ST
{
    uint8_t* data;
    int size;
}RWALG_FEATURE_INFO_S;

#endif /*__RWALG_COMMON_STRUCT_H__*/

