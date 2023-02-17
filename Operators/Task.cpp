#include "Task.h"
#include "Algothrim.h"
#include "json.hpp"
#include "logger.h"
#include <memory>
#include <vector>
#include <fstream>

class Task::impl {
public:
    impl(std::string_view tasksPath, std::string_view taskId);
    ~impl();

    bool init();
    std::vector<Result> detect(unsigned char *BGR888, int width, int height);

private:
    bool loadAlogthrims(const std::string &tasksPath, std::string_view taskId);

private:
    std::string tasksPath_;
    std::string taskId_;
    std::vector<std::shared_ptr<Algothrim>> algos_;
};

Task::impl::impl(std::string_view tasksPath, std::string_view taskId)
    : tasksPath_(tasksPath), taskId_(taskId) {
    
}

Task::impl::~impl() {

}

bool Task::impl::init() {
    return loadAlogthrims(tasksPath_, taskId_);
}

bool Task::impl::loadAlogthrims(const std::string &tasksPath, std::string_view taskId) {
    std::vector<int> algoIds;

    //定位到taskId.json
    std::string jsonPath = fmt::format("{}/{}.json", tasksPath, taskId);

    try {
        std::ifstream ifs(jsonPath);
        auto root = nlohmann::json::parse(ifs);

        algoIds   = root["Algo"].get<std::vector<int>>();
    } catch (nlohmann::json::exception &e) {
        LOG_ERROR("Error on load {}, {}", jsonPath, e.what());
        return false;
    }

    //转换为算法对象
    //计算算法目录
    std::string algPath = fmt::format("{}/{}", tasksPath, taskId);

    for (auto &algoId : algoIds) {
        std::shared_ptr<Algothrim> task(new Algothrim(algPath, std::to_string(algoId)));

        if (task->init()) {
            algos_.push_back(task);
        }
    }

    return !algos_.empty();
}

std::vector<Result> Task::impl::detect(unsigned char *BGR888, int width, int height) {
    std::vector<Result> res;

    for (auto algo : algos_) {
        auto subRes = algo->detect(BGR888, width, height);
        std::copy(subRes.begin(), subRes.end(), std::back_inserter(res));
    }
    return res;
}

//----------------------------------------------------------
Task::Task(std::string_view tasksPath, std::string_view taskId) {
    impl_ = new impl(tasksPath, taskId);
}

Task::~Task() {
    delete impl_;
}

std::vector<Result> Task::detect(unsigned char *BGR888, int width, int height) {
    return impl_->detect(BGR888, width, height);
}

bool Task::init() {
    return impl_->init();
}
