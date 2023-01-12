#include "Decoder_h264.h"
#include "logger.h"
#include <mutex>
#include "rk_mpi.h"
#include "mpp_common.h"
#include "mpp_frame.h"

// Windows下的解码实现使用ffmpeg解码器，如果要编译可以先下载一个ffmpeg
using namespace Codec;

class DecoderH264::impl {
public:
    impl();
    ~impl();

    AVFrame *decode(unsigned char *src, size_t length);

private:
    void cleanResetBuffer();

private:
    MppCtx ctx_;
    MppApi *mpi_;
    MppBufferGroup frmGroup_ = nullptr, pktGroup_ = nullptr;
    MppPacket packet_;
    MppFrame mppFrame_;
    MppBuffer frmBuffer_, pktBuffer_;
    FILE *file;
};

DecoderH264::impl::impl() {
    int ret = mpp_create(&ctx_, &mpi_);
    if (ret != MPP_OK) {
        LOG_ERROR("Failed to create MPP context (code = {})", ret);
        return;
    }

    // initialize mpp
    ret = mpp_init(ctx_, MPP_CTX_DEC, MPP_VIDEO_CodingAVC);
    if (ret != MPP_OK) {
        LOG_ERROR("Failed to initialize MPP context (code = {})", ret);
        return;
    }

    // make decode calls blocking with a timeout
    RK_S32 paramS32 = MPP_POLL_BLOCK;
    ret      = mpi_->control(ctx_, MPP_SET_OUTPUT_BLOCK, &paramS32);
    if (ret != MPP_OK) {
        LOG_ERROR("Failed to set blocking mode on MPI (code = {})", ret);
        return;
    }

    ret = mpp_buffer_group_get_internal(&frmGroup_, MPP_BUFFER_TYPE_ION);
    if (ret) {
        LOG_ERROR("Failed to retrieve frm buffer group (code = {})", ret);
        return;
    }

    ret = mpp_buffer_group_get_internal(&pktGroup_, MPP_BUFFER_TYPE_ION);
    if (ret) {
        LOG_ERROR("Failed to retrieve pkt buffer group (code = {})", ret);
        return;
    }

    ret = mpi_->control(ctx_, MPP_DEC_SET_EXT_BUF_GROUP, frmGroup_);
    if (ret) {
        LOG_ERROR("Failed to assign buffer group (code = {})", ret);
        return;
    }

    ret = mpp_buffer_group_limit_config(frmGroup_, 0, 16);
    if (ret) {
        LOG_ERROR("Failed to set buffer group limit (code = {})", ret);
        return;
    }

    ret = mpp_packet_init(&packet_, NULL, 0);
    if (ret) {
        LOG_ERROR("Failed to init packet (code = {})", ret);
        return;
    }

    ret = mpp_frame_init(&mppFrame_);
    if (ret) {
        LOG_ERROR("Failed to mpp_frame_init (code = {})", ret);
        return;
    }

    //frame
    RK_U32 hor_stride = MPP_ALIGN(1920, 16);
    RK_U32 ver_stride = MPP_ALIGN(1080, 16);

    ret = mpp_buffer_get(frmGroup_, &frmBuffer_, hor_stride * ver_stride * 3 / 2);
    if (ret) {
        LOG_ERROR("Failed to mpp_buffer_get (code = {})", ret);
        return;
    }

    mpp_frame_set_buffer(mppFrame_, frmBuffer_);

    file = fopen("out.yuv", "wb");
    LOG_INFO("Decoder inited!");
}

DecoderH264::impl::~impl() {
}

AVFrame *DecoderH264::impl::decode(unsigned char *src, size_t length) {
    //初始化packet
    mpp_packet_set_data(packet_, src);
    mpp_packet_set_size(packet_, length);
    mpp_packet_set_pos(packet_, src);
    mpp_packet_set_length(packet_, length);

    int ret = mpi_->decode_put_packet(ctx_, packet_);
    if (MPP_OK != ret) {
        LOG_ERROR("Error on decode_put_packet [{}]", ret);
        return nullptr;
    }

    ret = mpi_->decode_get_frame(ctx_, &mppFrame_);

    if (mppFrame_) {
        if (mpp_frame_get_info_change(mppFrame_)) {
            RK_U32 buf_size = mpp_frame_get_buf_size(mppFrame_);

            cleanResetBuffer();

            /* Use limit config to limit buffer count to 24 with buf_size */
            ret = mpp_buffer_group_limit_config(frmGroup_, buf_size, 24);
            if (ret) {
                LOG_ERROR("Error on mpp_buffer_group_limit_config", ret);
                return nullptr;
            }
            ret               = mpi_->control(ctx_, MPP_DEC_SET_INFO_CHANGE_READY, NULL);
            if (ret) {
                LOG_ERROR("Error on MPP_DEC_SET_INFO_CHANGE_READY", ret);
                return nullptr;
            }
        }

    }

    return nullptr;
}

void DecoderH264::impl::cleanResetBuffer() {
    int ret = 0;

    if (NULL == frmGroup_) {
        /* If buffer group is not set create one and limit it */
        ret = mpp_buffer_group_get_internal(&frmGroup_, MPP_BUFFER_TYPE_ION);
        if (ret) {
            LOG_ERROR("Error on mpp_buffer_group_get_internal [{}]", ret);
            return;
        }

        /* Set buffer to mpp decoder */
        ret = mpi_->control(ctx_, MPP_DEC_SET_EXT_BUF_GROUP, frmGroup_);
        if (ret) {
            LOG_ERROR("Error on MPP_DEC_SET_EXT_BUF_GROUP", ret);
        }
    } else {
        /* If old buffer group exist clear it */
        ret = mpp_buffer_group_clear(frmGroup_);
        if (ret) {
            LOG_ERROR("Error on mpp_buffer_group_clear", ret);
            return;
        }
    }
   
}

//----------------------------------------------------------------------------------------------------
DecoderH264::DecoderH264() {
    impl_ = new impl();
}

DecoderH264::~DecoderH264() {
    if (impl_) {
        delete impl_;
    }
}

AVFrame * DecoderH264::decode(unsigned char *src, size_t length) {
    return impl_->decode(src, length);
}