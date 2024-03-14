#include <glog/logging.h>
#include "MsgForwarder.hpp"

int main(int argc, char* argv[]) {
    // GLOG setup
    google::InitGoogleLogging(argv[0]);
    // Log to stderr instead of log files
    FLAGS_logtostderr = 1;
    // Set min log level to info
    FLAGS_minloglevel = 0;
    boost::asio::io_context io;
    MsgForwarder msgForwarder(io);
    io.run();
    return 0;
}
