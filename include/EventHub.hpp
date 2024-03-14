#pragma once

#include <boost/asio.hpp>
#include <boost/asio/io_context.hpp>
#include <thread>
#include <unordered_map>
#include <vector>
#include <memory>
#include <glog/logging.h>
#include "EventHandler.hpp"
#include "Events.hpp"

// Conveinence, if you are using this you are using boost asio
using namespace boost::asio;
class EventHub : public std::enable_shared_from_this<EventHub> {
    public: 
        EventHub(io_context& io):
            _io(io) 
            { };
        ~EventHub() = default;

        void subscribe(std::shared_ptr<EventHandler> handler, MsgEvent subId) { 
            if (_subMap.count(subId) == 0) {
                _subMap.emplace(subId, std::vector<std::shared_ptr<EventHandler>>{ });
            }
            _subMap[subId].push_back(handler);
        }

        void publish(MsgEvent pubId, const std::vector<char>& msg) {
            for (const auto& event : _subMap) {
                if (pubId != event.first) { continue; }
                for(const auto& handler : event.second) {
                    post(_io, [&](){ handler->process_event(pubId, msg); });
                }
            }
        }

    private:
        io_context& _io;
        std::unordered_map<MsgEvent, std::vector<std::shared_ptr<EventHandler>>> _subMap;
};
