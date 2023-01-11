#include "Operators.h"
#include "rwalg_interface.h"
#include "logger.h"
#include "rwalg_reasoning/rwalg_reasoning.h"

class Operators::impl {
public:
    bool init();
    bool detect(unsigned char *BGR888, int width, int height, unsigned char *YUV420);

private: 
    void drawLine(unsigned char *YUV420, int width, int height, const RWALG_POINT_S &p1, const RWALG_POINT_S &p2);
    void drawRECT(unsigned char *YUV420, int width, int height, const RWALG_RECT_S &rect);
    void setYUVPix(unsigned char *YBuf, unsigned char *UVBuf, int width, int height, const RWALG_POINT_S &p1);

private:
    RWALG_HANDLE_T reasoningHandle_             = nullptr;
    RWALG_REASONING_MODEL_LABEL_S *modelLabels_ = nullptr;
};


bool Operators::impl::init() {
    // if (RWALG_SUCCESS == rwalg_init_sdk()) {
    //     LOG_ERROR("Erroron rwalg_init_sdk");
    //     return false;
    // }

    ////TODO: 还不知道怎么配置
    // RWALG_HANDLE_OPTPARAM_S alg_param;

    // alg_param.handle_type = RWALG_HANDLE_TYPE_REASONING;
    // alg_param.reasoning_config.config_path = "./alg_res/alg_config.json";
    // alg_param.reasoning_config.model_path = "./alg_res/xxx.model";

    // if(rwalg_init_handle(alg_param, &reasoningHandle_)!= RWALG_SUCCESS) {
    //     LOG_ERROR("Error on rwalg_init_handle");
    //     return -1;
    // }

    ////4.get model label.获取该模型能推理那些标签
    // modelLabels_ = new RWALG_REASONING_MODEL_LABEL_S();

    // if( RWALG_SUCCESS == rwalg_getModelLabel(reasoningHandle_, modelLabels_)) {
    //     LOG_DEBUG("label_num = {}", modelLabels_->label_num);

    //    for(int i=0; i < modelLabels_->label_num; ++i) {
    //        LOG_DEBUG("label[{}]:{}", i, modelLabels_->label[i]);
    //    }
    //}

    return true;
}


bool Operators::impl::detect(unsigned char *BGR888, int width, int height, unsigned char *YUV420) {
    // RWALG_FRAME_S frame;

    // frame.addr_type     = RWALG_FRAME_SOURCE_DATA;
    // frame.width         = width;
    // frame.height        = height;
    // frame.type          = RWALG_IMAGE_BGR;
    // frame.delegate_flag = 0;
    // frame.frame_id = 0;
    // frame.data = (unsigned char*)BGR888;

    // RWALG_REASONING_RESULT_S reasoning_res;

    // if(RWALG_SUCCESS == rwalg_reasoning_detect(reasoningHandle_, &frame, &reasoning_res) )
    //{
    //     if(reasoning_res.task_type == RWALG_REASONING_TASK_CLASSIFY)//分类的,比如判断是否是XXX 结果是 是或者否
    //     {
    //         LOG_DEBUG("classify_res: class_id={}}, label={}}, conf={}}",
    //                reasoning_res.classify_res.class_id,
    //                modelLabels_->label[reasoning_res.classify_res.class_id],
    //                reasoning_res.classify_res.conf);
    //     }else
    //     {
    //         // 标签式的算法,某一个东西可能有好几个标签
    //         for(int i=0; i < reasoning_res.detect_res.target_num; ++i)
    //         {
    //             int classId = reasoning_res.detect_res.target[i].class_id;
    //             char *label = modelLabels_->label[classId];
    //             float conf = reasoning_res.detect_res.target[i].conf;
    //             RWALG_RECT_S rect = reasoning_res.detect_res.target[i].rect;

    //            drawRECT(YUV420, width, height, rect);
    //        }
    //    }
    //}
    RWALG_RECT_S rect1, rect2;

    rect1.left   = 80;
    rect1.top    = 80;
    rect1.right  = 280;
    rect1.bottom = 280;

    rect2.left   = 1000;
    rect2.top    = 200;
    rect2.right  = 1200;
    rect2.bottom = 480;

    drawRECT(YUV420, width, height, rect1);
    drawRECT(YUV420, width, height, rect2);

    RWALG_POINT_S p1, p2;

    p1.x = 600;
    p1.y = 600;
    p2.x = 600;
    p2.y = 800;

    drawLine(YUV420, width, height, p1, p2);

    p1.x = 600;
    p1.y = 800;
    p2.x = 800;
    p2.y = 100;

    drawLine(YUV420, width, height, p1, p2);

    return true;
}

void Operators::impl::drawRECT(unsigned char *YUV420, int width, int height, const RWALG_RECT_S &rect) {
    RWALG_POINT_S p1, p2;

    //上
    p1.x = rect.left;
    p1.y = rect.top;
    p2.x = rect.right;
    p2.y = rect.top;

    drawLine(YUV420, width, height, p1, p2);

    //左，并且上下少一个像素(线条宽度)
    p1.x = rect.left;
    p1.y = rect.top + 1;
    p2.x = rect.left;
    p2.y = rect.bottom - 1;

    drawLine(YUV420, width, height, p1, p2);

    //下
    p1.x = rect.left;
    p1.y = rect.bottom - 1;
    p2.x = rect.right;
    p2.y = rect.bottom - 1;

    drawLine(YUV420, width, height, p1, p2);

    //右
    p1.x = rect.right - 1;
    p1.y = rect.top + 1;
    p2.x = rect.right - 1;
    p2.y = rect.bottom - 1;

    drawLine(YUV420, width, height, p1, p2);
}

void Operators::impl::drawLine(unsigned char *YUV420, int width, int height, const RWALG_POINT_S &p1, const RWALG_POINT_S &p2) {
    uint8_t *YBuff = NULL, *UVBuff = NULL;

    //定位到YUV数据，这里固定是YUV420格式
    YBuff  = YUV420;
    UVBuff = YUV420 + width * height;

    // 布雷森汉姆算法画线
    int dx    = (p1.x > p2.x) ? (p1.x - p2.x) : (p2.x - p1.x);
    int dy    = (p1.y > p2.y) ? (p1.y - p2.y) : (p2.y - p1.y);
    int xstep = (p1.x < p2.x) ? 1 : -1;
    int ystep = (p1.y < p2.y) ? 1 : -1;
    int nstep = 0, eps = 0;
    int x0 = p1.x, y0 = p1.y;
    int x1 = p2.x, y1 = p2.y;

    //不能画出去，会内存越界
    x0 = (x0 >= width) ? (x0 - 1) : x0;
    x1 = (x1 >= width) ? (x1 - 1) : x1;
    y0 = (y0 >= height) ? (y0 - 1) : y0;
    y1 = (y1 >= height) ? (y1 - 1) : y1;

    RWALG_POINT_S drawPoint;

    drawPoint.x = x0;
    drawPoint.y = y0;

    if (dx > dy) {
        while (nstep <= dx) {
            setYUVPix(YBuff, UVBuff, width, height, drawPoint);
            eps += dy;
            if ((eps << 1) >= dx) {
                drawPoint.y += ystep;
                eps -= dx;
            }
            drawPoint.x += xstep;
            nstep++;
        }
    } else {
        while (nstep <= dy) {
            setYUVPix(YBuff, UVBuff, width, height, drawPoint);
            eps += dx;
            if ((eps << 1) >= dy) {
                drawPoint.x += xstep;
                eps -= dy;
            }
            drawPoint.y += ystep;
            nstep++;
        }
    }
}

void Operators::impl::setYUVPix(unsigned char *YBuf, unsigned char *UVBuf, int width, int height, const RWALG_POINT_S &p1) {
    /*
        YYYYYYYY
        UU
        VV
    */
    uint32_t y_offset = 0, u_offset = 0, v_offset = 0;
    uint32_t plane_size = width * height / 4;

    y_offset            = p1.y * width + p1.x;
    u_offset            = p1.y / 2 * width / 2 + p1.x / 2;
    v_offset            = u_offset + plane_size;

    //暂时先画红色
    YBuf[y_offset]  = 0x00;
    UVBuf[u_offset] = 0x00;
    UVBuf[v_offset] = 0xff;
}
//-----------------------------------------------------------------------------------------------
Operators::Operators() {
    impl_ = new impl();
}

Operators::~Operators() {
    delete impl_;
}

bool Operators::init() {
    return impl_->init();
}

bool Operators::detect(unsigned char *BGR888, int width, int height, unsigned char *YUV420) {
    return impl_->detect(BGR888, width, height, YUV420);
}
