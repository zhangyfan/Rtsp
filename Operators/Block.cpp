#include "Block.h"
#include "json.hpp"
#include "logger.h"
#include "rwalg_interface.h"
#include "rwalg_reasoning/rwalg_reasoning.h"

class Block::impl {
public:
    impl(std::string_view path, std::string blockId);
    ~impl();

    bool init();
    std::vector<Result> detect(unsigned char *BGR888, int width, int height);

private:
    bool initHandle();

private:
    std::string blockId_;
    std::string config_;
    std::string model_;
    std::string license_;
    RWALG_HANDLE_T handle_;
    RWALG_REASONING_MODEL_LABEL_S label_;
};

Block::impl::impl(std::string_view path, std::string blockId) {
    config_  = fmt::format("{}/{}/alg_config.json", path, blockId);
    model_   = fmt::format("{}/{}/alg_config.gdd", path, blockId);
    license_ = fmt::format("{}/{}/license", path, blockId);
    blockId_ = blockId;
}

Block::impl::~impl() {
}

bool Block::impl::init() {
    return initHandle();
}

bool Block::impl::initHandle() {
    RWALG_HANDLE_OPTPARAM_S alg_param;

    alg_param.handle_type                   = RWALG_HANDLE_TYPE_REASONING;
    alg_param.reasoning_config.config_path  = config_.data();
    alg_param.reasoning_config.model_path   = model_.data();
    alg_param.reasoning_config.license_path = license_.data();

#ifndef _MSC_VER
    int ret = rwalg_init_handle(alg_param, &handle_);
    if (ret != RWALG_SUCCESS) {
        LOG_ERROR("rwalg_init_handle failed: {}", ret);
        return false;
    }

    //获取版本并打印日志
    char alg_version[1024];
    char model_version[1024];

    if (RWALG_SUCCESS == rwalg_get_version(handle_, alg_version, model_version)) {
        LOG_INFO("alg_version:{}, model_version:{}", alg_version, model_version);
    }

    if (RWALG_SUCCESS == rwalg_reasoning_get_model_label(handle_, &label_)) {
        for (int i = 0; i < label_.label_num; ++i) {
            LOG_INFO("label[{}]: {}", i, label_.label[i]);
        }
    }
#endif
    
    LOG_INFO("Alogthrim block {} loaded, config: {}, model: {}, license: {}", blockId_, config_, model_, license_);
    return true;
}

std::vector<Result> Block::impl::detect(unsigned char *BGR888, int width, int height) {
#ifndef _MSC_VER
    RWALG_REASONING_RESULT_S reasoning_res;
    RWALG_FRAME_S frame;

    frame.addr_type       = RWALG_FRAME_SOURCE_DATA;
    frame.width           = width;
    frame.height          = height;
    frame.type            = RWALG_IMAGE_BGR;
    frame.delegate_flag   = 0;
    frame.frame_id        = 0;
    frame.data            = BGR888;

    int ret                   = rwalg_reasoning_detect(handle_, &frame, &reasoning_res);

    if (RWALG_SUCCESS != ret) {
        return std::vector<Result>();
    }

    std::vector<Result> res;

    if (reasoning_res.task_type == RWALG_REASONING_TASK_CLASSIFY) {
    } else {
        for (int i = 0; i < reasoning_res.detect_res.target_num; ++i) {
            Result item;

            item.label = label_.label[reasoning_res.detect_res.target[i].class_id];
            item.conf  = reasoning_res.detect_res.target[i].conf;
            item.lx    = reasoning_res.detect_res.target[i].rect.left;
            item.ly    = reasoning_res.detect_res.target[i].rect.top;
            item.rx    = reasoning_res.detect_res.target[i].rect.right;
            item.ry    = reasoning_res.detect_res.target[i].rect.bottom;

            res.push_back(item);
        }
    }

    return res;
#else
    Result item;

    item.label = "Person";
    item.conf  = 0.8;
    item.lx    = 1;
    item.ly    = 1;
    item.rx    = 2;
    item.ry    = 2;

    return std::vector<Result>{item};
#endif
}

//----------------------------------------------------------
Block::Block(std::string_view path, std::string blockId) {
    impl_ = new impl(path, blockId);
}

Block::~Block() {
    delete impl_;
}

std::vector<Result> Block::detect(unsigned char *BGR888, int width, int height) {
    return impl_->detect(BGR888, width, height);
}

bool Block::init() {
    return impl_->init();
}
