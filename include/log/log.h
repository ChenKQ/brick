#ifndef BRICK_LOG_H
#define BRICK_LOG_H

#include <string>

#include <glog/logging.h>

#define LOGINFO LOG(INFO)
#define LOGWARN LOG(WARNING)
#define LOGERROR LOG(ERROR)
#define LOGFATAL LOG(FATAL)

void initGLog(const char *progname, const std::string &logdirstr);

void closeGlog();

#endif // BRICK_LOG_H
