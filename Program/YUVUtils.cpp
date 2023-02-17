#include <utility>
#include <tuple>
#include "logger.h"
extern "C" {
    #include <libavformat/avformat.h>
    #include "libswscale/swscale.h"
    #include "libavutil/imgutils.h"
}

#ifdef _MSC_VER
std::pair<unsigned char*, size_t> toYUV420(AVFrame *frame) {
    int width  = frame->width;
    int height = frame->height;
    size_t length = width * height * 3;
    unsigned char *data = (unsigned char *)malloc(length);

    av_image_copy_to_buffer(data, (int)length, frame->data, frame->linesize, AV_PIX_FMT_YUV420P, width, height, 1);
    return std::make_pair(data, length);
}

std::pair<unsigned char *, size_t> toBGR888(AVFrame *frame) {
    int width              = frame->width;
    int height             = frame->height;
    SwsContext *swsContext = sws_getContext(
        width, height, AV_PIX_FMT_YUV420P, width, height, AV_PIX_FMT_BGR24, NULL, NULL, NULL, NULL);
    int linesize[8]        = {frame->linesize[0] * 3};
    int numBytes           = av_image_get_buffer_size(AV_PIX_FMT_BGR24, width, height, 1);
    auto buffer            = (uint8_t *)malloc(numBytes * sizeof(uint8_t));
    uint8_t *bgrBuffer[8]  = {buffer};

    sws_scale(swsContext, frame->data, frame->linesize, 0, height, bgrBuffer, linesize);

    size_t length = width * height * 3;
    unsigned char *data = (unsigned char *)malloc(length); 

    memcpy(data, bgrBuffer[0], length);
    free(buffer);

    sws_freeContext(swsContext);
    return std::make_pair(data, length);
}

bool YUV2JPG(AVFrame *frame, std::string_view path) {
    return false;
}
#else
#include "mpp_frame.h"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core_c.h"
#include "rga/im2d.hpp"
#include "rga/RockchipRga.h"
#include "rga/RgaUtils.h"
#include <fstream>

#define MPP_ALIGN(x, a) (((x) + (a)-1) & ~((a)-1))

typedef struct
{
    MppFrame frame;
    AVBufferRef *decoder_ref;
} RKMPPFrameContext;
//Warning
// Linux下输出的是MPP结构，其内部数据就是YUV420的
// 如果解码返回多个AVFrame，好像是分成了一片一片的

MppFrame AVFrame2MppFrame(AVFrame *avframe) {
    MppBuffer mppbuff;

    if (avframe->format != AV_PIX_FMT_DRM_PRIME) {
        return NULL;
    }
    AVBufferRef *framecontextref = (AVBufferRef *)av_buffer_get_opaque(
        avframe->buf[0]);
    RKMPPFrameContext *framecontext =
        (RKMPPFrameContext *)framecontextref->data;

    return framecontext->frame;
}

std::pair<unsigned char *, size_t> toYUV420(AVFrame *frame) {
    MppFrame mppFrame      = AVFrame2MppFrame(frame);
    MppBuffer buffer       = mpp_frame_get_buffer(mppFrame);
    int width              = MPP_ALIGN(frame->width, 16);
    int height             = MPP_ALIGN(frame->height, 16);

    return std::make_pair((unsigned char *)mpp_buffer_get_ptr(buffer), width * height * 3 / 2);
}

//std::pair<unsigned char *, size_t> toBGR888(AVFrame *frame) {
//    unsigned char *YUV420;
//    size_t YUVSize;
//
//    std::tie(YUV420, YUVSize) = toYUV420(frame);
//    int width                 = MPP_ALIGN(frame->width, 16);
//    int height                = MPP_ALIGN(frame->height, 16);
//
//    cv::Mat mat_src           = cv::Mat(height * 3 / 2, width, CV_8UC1, YUV420);
//    cv::Mat mat_dst           = cv::Mat(height, width, CV_8UC3);
//
//    cv::cvtColor(mat_src, mat_dst, cv::COLOR_YUV2BGR_NV12);
//
//    //拷贝数据
//    size_t BGRSize = width * height * 3;
//    unsigned char *BGR888 = (unsigned char *)malloc(BGRSize);
//
//    memcpy(BGR888, mat_dst.data, BGRSize);
//    {
//        FILE *out = fopen("out.bgr", "wb");
//        fwrite(BGR888, 1, BGRSize, out);
//        fclose(out);
//    }
//    return std::make_pair(BGR888, BGRSize);
//}

std::pair<unsigned char *, size_t> toBGR888(AVFrame *frame) {
    unsigned char *YUV420;
    size_t YUVSize;

    //SRC
    std::tie(YUV420, YUVSize) = toYUV420(frame);
    int width                 = frame->width;
    int height                = frame->height;
    int wStrike               = MPP_ALIGN(width, 16);
    int vStrike               = MPP_ALIGN(height, 16);

    //DST
    IM_STATUS STATUS;

    rga_buffer_t src;
    rga_buffer_t dst;
    char *src_buf = NULL;
    char *dst_buf = NULL;

    memset(&src, 0, sizeof(src));
    memset(&dst, 0, sizeof(dst));

    src_buf    = (char *)YUV420;
    dst_buf    = (char *)malloc(wStrike * vStrike * get_bpp_from_format(RK_FORMAT_RGB_888));

    src        = wrapbuffer_virtualaddr(src_buf, wStrike, vStrike, RK_FORMAT_YCbCr_420_SP);
    dst        = wrapbuffer_virtualaddr(dst_buf, wStrike, vStrike, RK_FORMAT_RGB_888);

    src.format = RK_FORMAT_YCbCr_420_SP;
    dst.format = RK_FORMAT_BGR_888;

    STATUS = imcvtcolor(src, dst, src.format, dst.format);

    if (STATUS != IM_STATUS_SUCCESS) {
        LOG_ERROR("Error on imcvtcolor [{}]", STATUS);
        return std::make_pair(nullptr, 0);
    }
    return std::make_pair((unsigned char *)dst_buf, wStrike * vStrike * 3);
}

bool YUV2JPG(AVFrame *frame, std::string_view path) {
    unsigned char *YUV420;
    size_t YUVSize;

    // SRC
    std::tie(YUV420, YUVSize) = toYUV420(frame);
    int width                 = frame->width;
    int height                = frame->height;
    int wStrike               = MPP_ALIGN(width, 16);
    int vStrike               = MPP_ALIGN(height, 16);

    cv::Mat yuvImg;
    cv::Mat rgbImg(vStrike, wStrike, CV_8UC3);

    yuvImg.create(vStrike * 3 / 2, wStrike, CV_8UC1);
    memcpy(yuvImg.data, YUV420, YUVSize * sizeof(unsigned char));

    //先转为rgb
    cv::cvtColor(yuvImg, rgbImg, cv::COLOR_YUV2RGB_NV21);

    //转换
    std::vector<uchar> encode;

    cv::imencode(".jpg", rgbImg, encode, {cv::IMWRITE_JPEG_QUALITY, 50});

    //写入
    std::ofstream ofs(std::string(path), std::ios::binary);

    if (!ofs.is_open()) {
        LOG_ERROR("Error on write jpg to {}", path);
        return false;
    }
    std::ostream_iterator<uchar> oit(ofs);

    std::copy(encode.begin(), encode.end(), oit);
    ofs.close();
    return true;
}
#endif // _MSC_VER