#ifndef BRICK_FFMPEGPRIV_H
#define BRICK_FFMPEGPRIV_H

#ifdef __cplusplus

#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

extern "C"{
#endif

#include <libavutil/avutil.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>

#ifdef __cplusplus
}
#endif

#endif // BRICK_FFMPEGPRIV_H
