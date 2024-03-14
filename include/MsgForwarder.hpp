#pragma once

#include <EventHub.hpp>
#include <TcpHandler.hpp>
#include <PrintHandler.hpp>

class MsgForwarder {
    public:
        MsgForwarder(io_context& io) : 
            _hub(std::make_shared<EventHub>(io)),
            _tcpHandler(std::make_shared<TcpHandler>(_hub, io)), 
            _printHandler(std::make_shared<PrintHandler>(_hub)) 
            {
                _tcpHandler->init();
                _printHandler->init();
            };
        ~MsgForwarder() = default;

    private:
        std::shared_ptr<EventHub> _hub;
        std::shared_ptr<TcpHandler> _tcpHandler;
        std::shared_ptr<PrintHandler> _printHandler;
};
