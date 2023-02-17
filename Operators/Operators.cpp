#include "Operators.h"
#include "rwalg_interface.h"
#include "logger.h"
#include "rwalg_reasoning/rwalg_reasoning.h"
#include "json.hpp"
#include "Task.h"
#include <fstream>

class Operators::impl {
public:
    bool init(std::string_view dir, std::string_view camId);
    std::vector<Result> detect(unsigned char *BGR888, int width, int height);

private: 
    bool loadTasks(std::string_view dir, std::string_view camId);

private:
    std::vector<std::shared_ptr<Task>> tasks_;
};


bool Operators::impl::init(std::string_view dir, std::string_view camId) {
#ifndef _MSC_VER
    int ret = rwalg_init_sdk();

    if (RWALG_SUCCESS != ret) {
        LOG_ERROR("Erroron rwalg_init_sdk [{}]", ret);
        return false;
    }
#endif

    return loadTasks(dir, camId);
}


std::vector<Result> Operators::impl::detect(unsigned char *BGR888, int width, int height) {
    std::vector<Result> res;

    for (auto task: tasks_) {
        auto subRes = task->detect(BGR888, width, height);
        std::copy(subRes.begin(), subRes.end(), std::back_inserter(res));
    }
    return res;
}

bool Operators::impl::loadTasks(std::string_view dir, std::string_view camId) {
    using namespace nlohmann;
    //全部不使用filesystem好像在gcc8.3 8.4有BUG
    std::string path = fmt::format("{}/{}/task.json", dir, camId);

    if (!std::filesystem::exists(path)) {
        LOG_ERROR("Error on load task.json, {} not exists", path);
        return false;
    }

    std::vector<int> taskIds;

    try {
        std::ifstream ifs(path);
        auto root = json::parse(ifs);

        taskIds   = root["Tasks"].get<std::vector<int>>();
    } catch (json::exception &e) {
        LOG_ERROR("Error on load {}, {}", path, e.what());
        return false;
    }

    //转换为Task对象
    //计算tasks路径
    std::string tasksPath = fmt::format("{}/{}/tasks", dir, camId);

    for (auto &taskId: taskIds) {
        std::shared_ptr<Task> task(new Task(tasksPath, std::to_string(taskId)));

        if (task->init()) {
            tasks_.push_back(task);
        }
    }

    return !tasks_.empty();
}

//-----------------------------------------------------------------------------------------------
Operators::Operators() {
    impl_ = new impl();
}

Operators::~Operators() {
    delete impl_;
}

bool Operators::init(std::string_view dir, std::string_view camId) {
    return impl_->init(dir, camId);
}

std::vector<Result> Operators::detect(unsigned char *BGR888, int width, int height) {
    return impl_->detect(BGR888, width, height);
}
