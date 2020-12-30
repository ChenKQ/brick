include(FindPackageHandleStandardArgs)

set(FFMPEG_ROOT_DIR "" CACHE PATH "Folder contains ffmpeg libraries")

set(FFMPEG_INCLUDE_SEARCH_PATHS
  /usr/include
  /usr/include/ffmpeg
  /usr/local/include
  /usr/local/include/ffmpeg
  ${FFMPEG_ROOT_DIR}/include
  ${FFMPEG_ROOT_DIR}/ffmpeg/include
)

set(FFMPEG_LIB_SEARCH_PATHS PATHS
    /usr/lib
    /usr/local/lib
    ${FFMPEG_ROOT_DIR}/lib
    ${FFMPEG_ROOT_DIR}/ffmpeg/lib
)

find_path(FFMPEG_INCLUDE_DIR libavcodec/avcodec.h
          PATHS ${FFMPEG_INCLUDE_SEARCH_PATHS})

set(_FFMPEG_SEARCH_LIBS avutil avcodec avformat avdevice swscale avfilter)
foreach(_var ${_FFMPEG_SEARCH_LIBS})
    find_library(FFMPEG_${_var}_LIBRARY NAMES ${_var}
                                        PATHS ${FFMPEG_LIB_SEARCH_PATHS})
    if(FFMPEG_${_var}_LIBRARY)
        list(APPEND FFMPEG_LIBRARY ${FFMPEG_${_var}_LIBRARY})

        message("Found FFmpeg ${_var}: " ${FFMPEG_${_var}_LIBRARY})
    else(FFMPEG_${_var}_LIBRARY)
        message(WARNING "cannot find FFmpeg ${_var}")
    endif(FFMPEG_${_var}_LIBRARY)
endforeach()

find_package_handle_standard_args(FFMPEG
                                  DEFAULT_MSG
                                  FFMPEG_INCLUDE_DIR
                                  FFMPEG_LIBRARY
)

if(FFMPEG_FOUND)
    set(FFMPEG_INCLUDE_DIRS ${FFMPEG_INCLUDE_DIR})
    set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARY})
    message("Found FFMPEG_INCLUDE_DIR: " ${FFMPEG_INCLUDE_DIR})
    message("Found FFMPEG_LIBS: " ${FFMPEG_LIBRARY})
    mark_as_advanced(FFMPEG_LIBRARIES FFMPEG_INCLUDE_DIR)
endif(FFMPEG_FOUND)
