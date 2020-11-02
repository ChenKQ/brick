#include "log/log.h"

#include <glog/logging.h>

void initGLog(const char *progname, const std::string &logdirstr)
{
    google::InitGoogleLogging(progname);
    google::InstallFailureSignalHandler();
    FLAGS_max_log_size = 100;

    FLAGS_log_dir = logdirstr;
    FLAGS_alsologtostderr = 1;
    FLAGS_colorlogtostderr=true;
}

void closeGlog()
{
    google::ShutdownGoogleLogging();
}
