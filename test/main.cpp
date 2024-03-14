#include <gtest/gtest.h>
#include <glog/logging.h>

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    // GLOG setup
    google::InitGoogleLogging(argv[0]);
    // Log to stderr instead of log files
    FLAGS_logtostderr = 1;
    // Set min log level to info
    FLAGS_minloglevel = 0;
    return RUN_ALL_TESTS();
}
