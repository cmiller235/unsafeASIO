#pragma once

#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <chrono>
#include <queue>
#include <glog/logging.h>

using namespace std::literals::chrono_literals;
using namespace boost::asio;

class TcpClient : public std::enable_shared_from_this<TcpClient> {
    public:
        TcpClient(io_context& io) : 
            _timer(io, 500ms),
            _socket(io),
            _resolver(io)
        { 
            try_connect();
        };
        ~TcpClient() = default;

        void try_connect() {
            _timer.async_wait(
                [&](const boost::system::error_code& ec) {
                    connect_handler(ec);
                });
        }

        void try_send() {
            _timer.async_wait(
                [&](const boost::system::error_code& ec) {
                    send_queue();
                });
        }

        void connect_handler(const boost::system::error_code& ec) {
            if(ec) { 
                LOG(INFO) << ec.message();
                return; 
            }
            connect(_socket, _resolver.resolve("127.0.0.1", "55555"), _ec);
            if(_ec) {
                LOG(INFO) << "Error while connecting, trying again.";
                try_connect();
                return;
            }
            LOG(INFO) << "Server found, IP: " 
                << _socket.remote_endpoint().address().to_string() 
                << " Port: " << _socket.remote_endpoint().port(); 
            try_send();
        }

        void send_queue() {
            if(not _msgQueue.empty()) {
                auto msg = _msgQueue.front();
                LOG(INFO) << "Sending msg...";
                auto sentData = write(_socket, buffer(msg.data(), msg.size()), _ec);
                LOG(INFO) << sentData << " bytes written.";
                if(_ec) { LOG(INFO) << "Client send error:" << _ec.message(); }
                _msgQueue.pop();
            }
            try_send();
        }

        void queue_msg(const std::vector<char>& msg) {
            _msgQueue.push(msg);
        }

    private:
        steady_timer _timer;
        ip::tcp::socket _socket;
        ip::tcp::resolver _resolver;
        boost::system::error_code _ec;
        std::array<char, 4096> _arrayBuf;
        std::queue<std::vector<char>> _msgQueue;
};
