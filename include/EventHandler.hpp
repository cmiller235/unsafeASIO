#pragma once

#include "Events.hpp"

class EventHandler {
    public:
        EventHandler() {};
        ~EventHandler() = default;
        virtual void process_event(MsgEvent subId, const std::vector<char>& msg) = 0;
};
