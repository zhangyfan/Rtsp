#include "Encoder.h"
#include "Encoder_JPEG.h"
#include "rk_mpi.h"
#include "mpp_mem.h"
#include "mpp_common.h"
//#include "logger.h"
#include <cstring>

using namespace Codec;

typedef struct {
    // global flow control flag
    RK_U32 frm_eos;
    RK_U32 pkt_eos;
    RK_U32 frm_pkt_cnt;
    RK_S32 frame_count;
    RK_U64 stream_size;

    // base flow context
    MppCtx ctx;
    MppApi *mpi;
    MppEncCfg cfg;
    MppEncPrepCfg prep_cfg;
    MppEncRcCfg rc_cfg;
    MppEncCodecCfg codec_cfg;
    MppEncSliceSplit split_cfg;
    MppEncOSDPltCfg osd_plt_cfg;
    MppEncOSDPlt    osd_plt;
    MppEncOSDData   osd_data;
    MppEncROIRegion roi_region[3];
    MppEncROICfg    roi_cfg;

    // input / output
    MppBufferGroup buf_grp;
    MppBuffer frm_buf;
    MppBuffer pkt_buf;
    MppEncSeiMode sei_mode;
    MppEncHeaderMode header_mode;

    // paramter for resource malloc
    RK_U32 width;
    RK_U32 height;
    RK_U32 hor_stride;
    RK_U32 ver_stride;
    MppFrameFormat fmt;
    MppCodingType type;
    RK_S32 num_frames;
    RK_S32 loop_times;

    // resources
    size_t header_size;
    size_t frame_size;
    /* NOTE: packet buffer may overflow */
    size_t packet_size;

    
    // rate control runtime parameter
    RK_S32 fps_in_flex;
    RK_S32 fps_in_den;
    RK_S32 fps_in_num;
    RK_S32 fps_out_flex;
    RK_S32 fps_out_den;
    RK_S32 fps_out_num;
    RK_S32 bps;
    RK_S32 bps_max;
    RK_S32 bps_min;
    RK_S32 rc_mode;
    RK_S32 gop;
} MpiEncData;

static MPP_RET test_ctx_init(MpiEncData **data, int width, int height)
{
    MpiEncData *p = NULL;
    MPP_RET ret = MPP_OK;

    p = mpp_calloc(MpiEncData, 1);
    if (!p) {
        ret = MPP_ERR_MALLOC;
        goto RET;
    }

    // get paramter from cmd
    p->width        = width;
    p->height       = height;
    p->hor_stride   = MPP_ALIGN(width, 16);
    p->ver_stride   = MPP_ALIGN(height, 16);
    p->fmt          = MPP_FMT_YUV420SP;
    p->type         = MPP_VIDEO_CodingMJPEG;
    p->bps          = width * height / 8 * 30;
    p->bps_min      = p->bps * 1 / 16;
    p->bps_max      = p->bps * 17 / 16;
    p->rc_mode      = MPP_ENC_RC_MODE_CBR;
    p->num_frames   = 1;

    p->fps_in_flex  = 0;
    p->fps_in_den   = 1;
    p->fps_in_num   = 30;
    p->fps_out_flex = 0;
    p->fps_out_den  = 1;
    p->fps_out_num  = 30;
    p->gop          = 30;
    p->frame_size   = MPP_ALIGN(p->hor_stride, 64) * MPP_ALIGN(p->ver_stride, 64) * 2;
    p->header_size = 0;

RET:
    *data = p;
    return ret;
}

static MPP_RET test_mpp_enc_cfg_setup(MpiEncData *p)
{
    MPP_RET ret;
    MppApi *mpi;
    MppCtx ctx;
    MppEncCfg cfg;

    if (NULL == p)
        return MPP_ERR_NULL_PTR;

    mpi = p->mpi;
    ctx = p->ctx;
    cfg = p->cfg;

    /* setup default parameter */
    if (p->fps_in_den == 0)
        p->fps_in_den = 1;
    if (p->fps_in_num == 0)
        p->fps_in_num = 30;
    if (p->fps_out_den == 0)
        p->fps_out_den = 1;
    if (p->fps_out_num == 0)
        p->fps_out_num = 30;

    if (!p->bps)
        p->bps = p->width * p->height / 8 * (p->fps_out_num / p->fps_out_den);

    mpp_enc_cfg_set_s32(cfg, "prep:width", p->width);
    mpp_enc_cfg_set_s32(cfg, "prep:height", p->height);
    mpp_enc_cfg_set_s32(cfg, "prep:hor_stride", p->hor_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:ver_stride", p->ver_stride);
    mpp_enc_cfg_set_s32(cfg, "prep:format", p->fmt);

    mpp_enc_cfg_set_s32(cfg, "rc:mode", p->rc_mode);

    /* fix input / output frame rate */
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_flex", p->fps_in_flex);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_num", p->fps_in_num);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_in_denorm", p->fps_in_den);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_flex", p->fps_out_flex);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_num", p->fps_out_num);
    mpp_enc_cfg_set_s32(cfg, "rc:fps_out_denorm", p->fps_out_den);
    mpp_enc_cfg_set_s32(cfg, "rc:gop", p->gop);

    /* drop frame or not when bitrate overflow */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_mode", MPP_ENC_RC_DROP_FRM_DISABLED);
    mpp_enc_cfg_set_u32(cfg, "rc:drop_thd", 20);        /* 20% of max bps */
    mpp_enc_cfg_set_u32(cfg, "rc:drop_gap", 1);         /* Do not continuous drop frame */

    /* setup bitrate for different rc_mode */
    mpp_enc_cfg_set_s32(cfg, "rc:bps_target", p->bps);

    /* for rc_mode MPP_ENC_RC_MODE_VBR VBR mode has wide bound */
    mpp_enc_cfg_set_s32(cfg, "rc:bps_max", p->bps_max ? p->bps_max : p->bps * 17 / 16);
    mpp_enc_cfg_set_s32(cfg, "rc:bps_min", p->bps_min ? p->bps_min : p->bps * 1 / 16);

    //for MPP_VIDEO_CodingMJPEG
    mpp_enc_cfg_set_s32(cfg, "jpeg:q_factor", 80);
    mpp_enc_cfg_set_s32(cfg, "jpeg:qf_max", 99);
    mpp_enc_cfg_set_s32(cfg, "jpeg:qf_min", 1);

    /* setup codec  */
    mpp_enc_cfg_set_s32(cfg, "codec:type", p->type);

    ret = mpi->control(ctx, MPP_ENC_SET_CFG, cfg);
    if (ret) {
        //LOG_ERROR("mpi control enc set cfg failed ret {}", ret);
        goto RET;
    }

    /* optional */
    p->sei_mode = MPP_ENC_SEI_MODE_ONE_FRAME;
    ret = mpi->control(ctx, MPP_ENC_SET_SEI_CFG, &p->sei_mode);
    if (ret) {
        //LOG_ERROR("mpi control enc set sei cfg failed ret {}", ret);
        goto RET;
    }

RET:
    return ret;
}

class EncoderJPEG::impl
{
public:
    impl();
    ~impl();
    
    void init(int width, int height, int fps);
    bool encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength);

private:
    MpiEncData *enc_;
    int width_, height_;
    MppCtx ctx_;
    MppApi *mpi_;
    MppPollType timeout = MPP_POLL_BLOCK;
};

EncoderJPEG::impl::impl() {
}

EncoderJPEG::impl::~impl() {
    mpp_destroy(ctx_);
}

void EncoderJPEG::impl::init(int width, int height, int fps) {
    width_ = width;
    height_ = height;

    test_ctx_init(&enc_, width, height);

    //设置buffer
    int ret = mpp_buffer_group_get_internal(&enc_->buf_grp, MPP_BUFFER_TYPE_DRM);
    if (ret) {
        //LOG_ERROR("failed to get mpp buffer group ret {}", ret);
        return;
    }
    
    ret = mpp_buffer_get(enc_->buf_grp, &enc_->frm_buf, enc_->frame_size + enc_->header_size);
    ret = mpp_buffer_get(enc_->buf_grp, &enc_->pkt_buf, enc_->frame_size);

    //初始化
    ret = mpp_create(&ctx_, &mpi_);
    if (ret) {
        //LOG_ERROR("Error on mpp_create code [{}]", ret);
        return;
    }

    enc_->ctx = ctx_;
    enc_->mpi = mpi_;

    ret = mpi_->control(ctx_, MPP_SET_OUTPUT_TIMEOUT, &timeout);
    if (MPP_OK != ret) {
        //LOG_ERROR("mpi control set output timeout {} ret {}", timeout, ret);
        return;
    }

    ret = mpp_init(ctx_, MPP_CTX_ENC, MPP_VIDEO_CodingMJPEG);
    if (ret) {
        //LOG_ERROR("mpp_init failed ret {}", ret);
        return;
    }

    ret = mpp_enc_cfg_init(&enc_->cfg);
    if (ret) {
        //LOG_ERROR("mpp_enc_cfg_init failed ret %d\n", ret);
        return;
    }

    ret = test_mpp_enc_cfg_setup(enc_);
    if (ret) {
        //LOG_ERROR("test mpp setup failed ret %d\n", ret);
        return;
    }
}

bool EncoderJPEG::impl::encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength) {
    MppFrame frame;

    int ret = mpp_frame_init(&frame);
    if (ret) {
        //LOG_ERROR("mpp_frame_init failed\n");
        return false;
    }
    
    mpp_frame_set_width(frame, width_);
    mpp_frame_set_height(frame, height_);
    mpp_frame_set_hor_stride(frame, enc_->hor_stride);
    mpp_frame_set_ver_stride(frame, enc_->ver_stride);
    mpp_frame_set_fmt(frame, MPP_FMT_YUV420SP);
    mpp_frame_set_eos(frame, 1);

    //准备数据
    void *frameBuf = mpp_buffer_get_ptr(enc_->frm_buf);

    memcpy(frameBuf, src, length);
    mpp_frame_set_buffer(frame, enc_->frm_buf);
    
    MppMeta meta = mpp_frame_get_meta(frame);
    MppPacket packet;
    
    
    mpp_packet_init_with_buffer(&packet, enc_->pkt_buf);
    mpp_packet_set_length(packet, 0);
    mpp_meta_set_packet(meta, KEY_OUTPUT_PACKET, packet);

    ret = mpi_->encode_put_frame(ctx_, frame);
    if (ret) {
        //LOG_ERROR("mpp encode put frame failed\n");
        mpp_frame_deinit(&frame);
        return false;
    }
    
    mpp_frame_deinit(&frame);

    ret = mpi_->encode_get_packet(ctx_, &packet);
    if (ret) {
        //LOG_ERROR("mpp encode get packet failed\n");
        return false;
    }
    
    void *ptr   = mpp_packet_get_pos(packet);
    size_t len  = mpp_packet_get_length(packet);
        
    if (mpp_packet_is_partition(packet) && !mpp_packet_is_eoi(packet)) {
        //LOG_ERROR("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
    }

    dst = new unsigned char[len];

    memcpy(dst, ptr, len);
    dstLength = len;
    
    mpp_packet_deinit(&packet);
    return true;
}
//----------------------------------------------------------------------------------------------------
EncoderJPEG::EncoderJPEG() {
    impl_ = new impl();
}

EncoderJPEG::~EncoderJPEG() {
    if (impl_) {
        delete impl_;
    }
}

bool EncoderJPEG::encode(unsigned char *src, size_t length, unsigned char *&dst, size_t &dstLength)
{
    return impl_->encode(src, length, dst, dstLength);
}

void EncoderJPEG::init(int width, int height, int fps) {
    impl_->init(width, height, fps);
}