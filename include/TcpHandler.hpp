#pragma once

#include "EventHandler.hpp"
#include <glog/logging.h>

class TcpHandler : public std::enable_shared_from_this<TcpHandler>, public EventHandler {
    public:
        TcpHandler(std::shared_ptr<EventHub> hub, io_context& io) : 
            _hubPtr(hub), 
            _socket(io),
            _acceptor(io)
        {};
        ~TcpHandler() = default;

        void init() {
            auto ptr = _hubPtr.lock();
            if (not ptr) { return; }
            ptr->subscribe(shared_from_this(), MsgEvent::SEND);
            await_connections();
        }

        void process_event(MsgEvent subId, const std::vector<char>& msg) override {
            switch(subId) {
                case MsgEvent::SEND:
                    send_msg(msg);
                    break;
                default:
                    break;
            }
        }
    private:
        void await_connections() {
            if (not _acceptor.is_open()) {
                ip::tcp::endpoint endpoint(ip::tcp::v4(), 55555);
                _acceptor.open(endpoint.protocol(), _ec);
                if(_ec) { LOG(INFO) << "Open error:" << _ec.message(); }
                _acceptor.set_option(ip::tcp::acceptor::reuse_address(true), _ec);
                if(_ec) { LOG(INFO) << "Reuse opt error:" << _ec.message(); }
                _acceptor.bind(endpoint, _ec);
                if(_ec) { LOG(INFO) << "Bind error:" << _ec.message(); }
                // Allowing only one client at a time, so set queue to 0, probably
                // not necessary
                _acceptor.listen(0, _ec);
                if(_ec) { LOG(INFO) << "Listen error:" << _ec.message(); }
            }
            // Capture weak pointer in lambda as to not have it increment reference
            // count of lifetime shared pointer
            LOG(INFO) << "Awaiting connection..."; 
            _acceptor.async_accept(_socket, 
                [selfWeak = std::weak_ptr<TcpHandler>(shared_from_this())]
                (const boost::system::error_code& ec) { 
                    auto ptr = selfWeak.lock();
                    if (not ptr) { return; }
                    ptr->handle_accept(ec);
                }
                );
        }

        void handle_accept(const boost::system::error_code& ec) {
            if(ec) { 
                LOG(INFO) << "Accept error:" << ec.message(); 
                _acceptor.cancel();
                _acceptor.close();
                await_connections();
            }
            // Allowing only one client at a time
            _acceptor.close();
            LOG(INFO) << "Connection accepted, IP: " 
                << _socket.remote_endpoint().address().to_string() 
                << " Port: " << _socket.remote_endpoint().port(); 
            await_msg();
        }

        void await_msg() {
            LOG(INFO) << "Awaiting message..."; 
            // We're gonna be cheeky, just make this trigger after 1 byte
            async_read(_socket, buffer(_arrayBuf.data(), 1), 
                [selfWeak = std::weak_ptr<TcpHandler>(shared_from_this())]
                (const boost::system::error_code& ec, std::size_t bytes_transferred) {
                    if(ec) { LOG(INFO) << "Received msg: " << ec.message(); }
                    auto ptr = selfWeak.lock();
                    if (not ptr) { return; }
                    ptr->forward_msg(bytes_transferred);
                });
        }

        void forward_msg(std::size_t bytesRetrieved) {
            auto hubPtr = _hubPtr.lock();
            if (not hubPtr) { return; }
            LOG(INFO) << "Server received msg, size: " << bytesRetrieved; 
            std::vector<char> copiedVec(_arrayBuf.begin(), _arrayBuf.begin() + bytesRetrieved);
            copiedVec.resize(bytesRetrieved);
            hubPtr->publish(MsgEvent::RECEIVE, copiedVec);
            await_msg();
        }

        void send_msg(const std::vector<char>& msg) { }

        boost::system::error_code _ec;
        ip::tcp::socket _socket;
        ip::tcp::acceptor _acceptor;
        std::array<char, 4096> _arrayBuf;
        std::weak_ptr<EventHub> _hubPtr;
};