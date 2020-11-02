find_path(FFMPEG_INCLUDE_DIR libavcodec/codec.h /usr/include /usr/local/include
                                        $ENV{FFMPEG_ROOT}/include
                                        ${FFMPEG_ROOT}/include
                                        $ENV{FFMPEG_ROOT}/glog/include
                                        ${FFMPEG_ROOT}/glog/include)
get_filename_component(FFMPEG_DIR ${FFMPEG_INCLUDE_DIR} DIRECTORY)

set(FFMPEG_SEARCH_PATH /usr/lib
                       /usr/local/lib
                       ${FFMPEG_DIR}/lib)

find_library(FFMPEG_AVCODEC_LIBRARY NAMES avcodec
                                    PATHS ${FFMPEG_SEARCH_PATH})
if(FFMPEG_AVCODEC_LIBRARY)
    message(STATUS "Found FFmpeg AVCODEC: " ${FFMPEG_AVCODEC_LIBRARY})
elseif(FFMPEG_AVCODEC_LIBRARY)
    message(WARNING "Cannot find FFmpeg AVCODEC")
endif(FFMPEG_AVCODEC_LIBRARY)

find_library(FFMPEG_AVDEVICE_LIBRARY NAMES avdevice
                            PATHS ${FFMPEG_SEARCH_PATH})
if(FFMPEG_AVDEVICE_LIBRARY)
    message(STATUS "Found FFmpeg AVDEVICE: " ${FFMPEG_AVDEVICE_LIBRARY})
elseif(FFMPEG_AVDEVICE_LIBRARY)
    message(WARNING "Cannot find FFmpeg AVDEVICE")
endif(FFMPEG_AVDEVICE_LIBRARY)

find_library(FFMPEG_AVUTIL_LIBRARY NAMES avutil avformat swscale avfilter
                            PATHS ${FFMPEG_SEARCH_PATH})
if(FFMPEG_AVUTIL_LIBRARY)
    message(STATUS "Found FFmpeg AVUTIL: " ${FFMPEG_AVUTIL_LIBRARY})
elseif(FFMPEG_AVUTIL_LIBRARY)
    message(WARNING "Cannot find FFmpeg AVUTIL")
endif(FFMPEG_AVUTIL_LIBRARY)

find_library(FFMPEG_AVFORMAT_LIBRARY NAMES avformat swscale avfilter
                            PATHS ${FFMPEG_SEARCH_PATH})
if(FFMPEG_AVFORMAT_LIBRARY)
    message(STATUS "Found FFmpeg AVFORMAT: " ${FFMPEG_AVFORMAT_LIBRARY})
elseif(FFMPEG_AVFORMAT_LIBRARY)
    message(WARNING "Cannot find FFmpeg AVFORMAT")
endif(FFMPEG_AVFORMAT_LIBRARY)

find_library(FFMPEG_AVSWSCALE_LIBRARY NAMES swscale avfilter
                            PATHS ${FFMPEG_SEARCH_PATH})
if(FFMPEG_AVSWSCALE_LIBRARY)
    message(STATUS "Found FFmpeg AVSWSCALE: " ${FFMPEG_AVSWSCALE_LIBRARY})
elseif(FFMPEG_AVSWSCALE_LIBRARY)
    message(WARNING "Cannot find FFmpeg AVSWSCALE")
endif(FFMPEG_AVSWSCALE_LIBRARY)

find_library(FFMPEG_AVFILTER_LIBRARY NAMES avfilter
                            PATHS ${FFMPEG_SEARCH_PATH})
if(FFMPEG_AVFILTER_LIBRARY)
    message(STATUS "Found FFmpeg AVFILTER: " ${FFMPEG_AVFILTER_LIBRARY})
elseif(FFMPEG_AVFILTER_LIBRARY)
    message(WARNING "Cannot find FFmpeg AVFILTER")
endif(FFMPEG_AVFILTER_LIBRARY)

set(FFMPEG_FOUND_CONDITION
    FFMPEG_INCLUDE_DIR        AND
    FFMPEG_AVCODEC_LIBRARY    AND
    FFMPEG_AVDEVICE_LIBRARY   AND
    FFMPEG_AVUTIL_LIBRARY     AND
    FFMPEG_AVFORMAT_LIBRARY   AND
    FFMPEG_AVSWSCALE_LIBRARY  AND
    FFMPEG_AVFILTER_LIBRARY)

if(FFMPEG_FOUND_CONDITION)
    set(FFMPEG_FOUND TRUE)
    set(FFMPEG_LIBS ${FFMPEG_AVCODEC_LIBRARY}
                    ${FFMPEG_AVDEVICE_LIBRARY}
                    ${FFMPEG_AVUTIL_LIBRARY}
                    ${FFMPEG_AVFORMAT_LIBRARY}
                    ${FFMPEG_AVSWSCALE_LIBRARY}
                    ${FFMPEG_AVFILTER_LIBRARY})
endif(FFMPEG_FOUND_CONDITION)
