#include "Algothrim.h"
#include "Block.h"
#include "json.hpp"
#include "logger.h"
#include <fstream>

class Algothrim::impl {
public:
    impl(std::string_view path, std::string_view algoId);
    ~impl();

    bool init();
    std::vector<Result> detect(unsigned char *BGR888, int width, int height);

private:
    bool loadBlocks(const std::string &path, std::string_view algoId);

private:
    std::string path_;
    std::string algoId_;
    std::vector<std::shared_ptr<Block>> blocks_;
};

Algothrim::impl::impl(std::string_view path, std::string_view algoId)
    : path_(path), algoId_(algoId) {
}

Algothrim::impl::~impl() {
}

bool Algothrim::impl::init() {
    return loadBlocks(path_, algoId_);
}

bool Algothrim::impl::loadBlocks(const std::string &path, std::string_view algoId) {
    std::vector<int> blockIds;

    //定位到taskId.json
    std::string jsonPath = fmt::format("{}/{}.json", path, algoId);

    try {
        std::ifstream ifs(jsonPath);
        auto root = nlohmann::json::parse(ifs);

        blockIds  = root["Blocks"].get<std::vector<int>>();
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("Error on load {}, {}", jsonPath, e.what());
        return false;
    }

    //转换为算法对象
    //计算block路径
    std::string blockPath = fmt::format("{}/{}", path, algoId);

    for (auto &blockId : blockIds) {
        std::shared_ptr<Block> block(new Block(blockPath, std::to_string(blockId)));

        if (block->init()) {
            blocks_.push_back(block);
        }
    }

    return !blocks_.empty();
}

std::vector<Result> Algothrim::impl::detect(unsigned char *BGR888, int width, int height) {
    std::vector<Result> res;

    for (auto block : blocks_) {
        auto subRes = block->detect(BGR888, width, height);
        std::copy(subRes.begin(), subRes.end(), std::back_inserter(res));
    }
    return res;
}

//----------------------------------------------------------
Algothrim::Algothrim(std::string_view path, std::string_view algoId) {
    impl_ = new impl(path, algoId);
}

Algothrim::~Algothrim() {
    delete impl_;
}

std::vector<Result> Algothrim::detect(unsigned char *BGR888, int width, int height) {
    return impl_->detect(BGR888, width, height);
}

bool Algothrim::init() {
    return impl_->init();
}
