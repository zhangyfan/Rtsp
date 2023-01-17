#include "NetworkListener.h"
#include "logger.h"
#include <asio.hpp>
#include <memory>

using namespace Network;

class NetworkListener::impl {
public:
    impl();
    ~impl();

    void run();
    bool connect();
    bool write(const char *data, size_t size);

private:
    void connectCallback(std::error_code ec);
    void readCallback(std::error_code ec, std::size_t);

private:
    std::shared_ptr<asio::io_context> context_;
    std::shared_ptr<asio::ip::tcp::socket> socket_;
    std::shared_ptr<asio::io_context::work> worker_;
    asio::ip::tcp::endpoint endpoint_;
    std::string buffer_;
};

NetworkListener::impl::impl(): endpoint_(asio::ip::address::from_string("127.0.0.1"), 8888) {
    context_ = std::make_shared<asio::io_context>();
    worker_ = std::make_shared<asio::io_context::work>(*context_);
    socket_ = std::make_shared<asio::ip::tcp::socket>(*context_);
}

NetworkListener::impl::~impl() {
    worker_.reset();
    socket_.reset();
    context_.reset();
}

bool NetworkListener::impl::connect() {
    using namespace asio;

    socket_->async_connect(endpoint_, 
        std::bind(&NetworkListener::impl::connectCallback, this, std::placeholders::_1));
    return true;
}

void NetworkListener::impl::connectCallback(std::error_code ec) {
    if (ec) {
        //链接失败，重试
        connect();
        LOG_ERROR("Error on connected to web server!");
    } else {
        socket_->async_receive(asio::buffer(buffer_), 
            std::bind(&NetworkListener::impl::readCallback, this, std::placeholders::_1, std::placeholders::_2));
        LOG_INFO("Success connected to web server!");
    }
}

bool NetworkListener::impl::write(const char *data, size_t size) {
    return socket_->write_some(asio::buffer(data, size)) == size;
}

void NetworkListener::impl::readCallback(std::error_code ec, std::size_t) {
    if (ec) {
        LOG_ERROR("Error on read data {}", ec.message());
    } else {
        socket_->async_receive(asio::buffer(buffer_), 
            std::bind(&NetworkListener::impl::readCallback, this, std::placeholders::_1, std::placeholders::_2));
        //加业务处理是解析JSON，将操作码与回调绑定
    }
}

void NetworkListener::impl::run() {
    context_->run();
}

//-----------------------------------------------------
NetworkListener::NetworkListener() {
    impl_ = new impl();
}

NetworkListener::~NetworkListener() {
    delete impl_;
}

bool NetworkListener::connect() {
    return impl_->connect();
}

bool NetworkListener::write(const char *data, size_t size) {
    return impl_->write(data, size);
}

void NetworkListener::run() {
    return impl_->run();
}