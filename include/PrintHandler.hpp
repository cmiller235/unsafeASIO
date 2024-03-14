#pragma once

#include "EventHandler.hpp"
#include <glog/logging.h>

class PrintHandler : public std::enable_shared_from_this<PrintHandler>, public EventHandler {
    public:
        PrintHandler(std::shared_ptr<EventHub> hub) : _hubPtr(hub) {};
        ~PrintHandler() = default;

        void init() {
            auto ptr = _hubPtr.lock();
            if(not ptr) { return; }
            ptr->subscribe(shared_from_this(), MsgEvent::RECEIVE);
        }

        void process_event(MsgEvent subId, const std::vector<char>& msg) override {
            switch(subId) {
                case MsgEvent::RECEIVE:
                    print_msg(msg);
                    break;
                default:
                    break;
            }
        }
    private:
        void print_msg(const std::vector<char>& msg) {
            auto ptr = _hubPtr.lock();
            if (not ptr) { return; }
            LOG(INFO) << "Printing msg";
            ptr->publish(MsgEvent::SEND, std::vector<char>{'O', 'I', ' ', 'M', 'A', 'T', 'E'});
        }
        std::weak_ptr<EventHub> _hubPtr;
};
