
#include <gtest/gtest.h>
#include <chrono>
#include "MsgForwarder.hpp"
#include "testUtils/TcpClient.hpp"

using namespace boost::asio;
using namespace std::literals::chrono_literals;

class BasicTest : public testing::Test {
    public:
        io_context _io;
        MsgForwarder _msgForwarder {_io};
        TcpClient _client{_io};

        void SetUp() override {

        }

        void TearDown() override {
            _io.stop();
        }
};

TEST_F(BasicTest, forwardOneMsg) {
    _client.queue_msg(std::vector<char>{'H', 'I'});
    _io.run_for(10s);
}
