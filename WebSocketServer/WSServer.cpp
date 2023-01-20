#include "WSServer.h"
#include "logger.h"
#include <hv/WebSocketServer.h>
#include <hv/WebSocketChannel.h>
#include <hv/HttpService.h>
#include <map>

using namespace Websocket;

class WSServer::impl {
public:
    impl();
    ~impl();

    bool init(int port);
    void setMessageCallback(const std::string &path, std::function<void(const std::string &)> cb);
    void asyncSend(const std::string &path, unsigned char *data, size_t size);

private:
    void onOpen(const WebSocketChannelPtr &, const HttpRequestPtr &);
    void onMessage(const WebSocketChannelPtr &, const std::string &);
    void onClose(const WebSocketChannelPtr &);
    
private:
    hv::WebSocketService *ws_;
    hv::WebSocketServer *server_ = nullptr;
    std::multimap<std::string, WebSocketChannelPtr> path2Channel_;
    std::multimap<WebSocketChannelPtr, std::string> channel2Path_;
    std::multimap<std::string, std::function<void(const std::string &)>> msgCallbacks_;
};

WSServer::impl::impl() {
    ws_ = new hv::WebSocketService();

    ws_->onopen = std::bind(&WSServer::impl::onOpen, this, std::placeholders::_1, std::placeholders::_2);
    ws_->onmessage = std::bind(&WSServer::impl::onMessage, this, std::placeholders::_1, std::placeholders::_2);
    ws_->onclose = std::bind(&WSServer::impl::onClose, this, std::placeholders::_1);
}

WSServer::impl::~impl() {
    delete ws_;
    if (server_) {
        delete server_;
    }
}

bool WSServer::impl::init(int port) {
    server_ = new hv::WebSocketServer;

    server_->registerWebSocketService(ws_);
    server_->setPort(port);
    server_->setThreadNum(2);
    server_->run(false);

    return true;
}

void WSServer::impl::onOpen(const WebSocketChannelPtr &channel, const HttpRequestPtr &request) {
    std::string path = request->Path();

    path2Channel_.insert(std::make_pair(path, channel));
    channel2Path_.insert(std::make_pair(channel, path));

    LOG_INFO("New websocket connection to [{}]", path);
}

void WSServer::impl::onMessage(const WebSocketChannelPtr &channel, const std::string &message) {
    auto range = channel2Path_.equal_range(channel);

    for (auto it = range.first; it != range.second; it++) {
        auto rangeCB = msgCallbacks_.equal_range(it->second);

        for (auto itCB = rangeCB.first; itCB != rangeCB.second; itCB++) {
            itCB->second(message);
        }
    }
}

void WSServer::impl::onClose(const WebSocketChannelPtr &channel) {
    auto range = channel2Path_.equal_range(channel);

    for (auto it = range.first; it != range.second; it++) {
        path2Channel_.erase(it->second);
        LOG_INFO("Websocket to [{}] disconnected!", it->second);
    }
    channel2Path_.erase(range.first, range.second);
}

void WSServer::impl::setMessageCallback(const std::string &path, std::function<void(const std::string &)> cb) {
    msgCallbacks_.insert(std::make_pair(path, cb));
}

void WSServer::impl::asyncSend(const std::string &path, unsigned char *data, size_t size) {
    auto range = path2Channel_.equal_range(path);

    for (auto it = range.first; it != range.second; it++) {
        it->second->send((const char*)data, size, WS_OPCODE_BINARY);
    }
}
//-----------------------------------------------------------------------
WSServer::WSServer() {
    impl_ = new impl();
}

WSServer::~WSServer() {
    delete impl_;
}

bool WSServer::init(int port) {
    return impl_->init(port);
}

void WSServer::setMessageCallback(const std::string &path, std::function<void(const std::string &)> cb) {
    impl_->setMessageCallback(path, cb);
}

void WSServer::asyncSend(const std::string &path, unsigned char *data, size_t size) {
    impl_->asyncSend(path, data, size);
}