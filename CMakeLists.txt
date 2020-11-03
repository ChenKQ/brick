cmake_minimum_required(VERSION 3.15)
project(brick LANGUAGES C CXX VERSION 0.1)

option(BUILD_SHARED_LIBS "Building shared libraries" ON)
set(GLOG_ROOT_DIR $ENV{GLOG_ROOT} CACHE PATH "Folder contains Google Glog libraries")
set(FFMPEG_ROOT_DIR $ENV{FFMPEG_ROOT} CACHE PATH "Folder contains ffmpeg libraries")

# use find_package config mode
list(APPEND CMAKE_PREFIX_PATH ${GLOG_ROOT_DIR})
# use find_package module mode
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/Modules)

find_package(GLOG REQUIRED)
if(GLOG_FOUND)
    set(LOG_SRC include/log/log.h src/log/log.cpp)
else(GLOG_FOUND)
    message(FATAL_ERROR "cannot find glog")
endif(GLOG_FOUND)

find_package(FFMPEG REQUIRED)
if(FFMPEG_FOUND)
    set(MEDIA_SRC include/media/ffmpeg.h src/media/ffmpeg.cpp
        include/media/ffmpegreader.h src/media/ffmpegreader.cpp
        include/media/ffmpegwriter.h src/media/ffmpegwriter.cpp
        include/media/imageformatconverter.h src/media/imageformatconverter.cpp
        include/media/playmanager.h src/media/playmanager.cpp)
else(FFMPEG_FOUND)
    message(FATAL_ERROR "cannot find ffmpeg")
endif(FFMPEG_FOUND)

add_library(brick ${LOG_SRC} ${MEDIA_SRC})
target_link_libraries(brick PUBLIC glog::glog ${FFMPEG_LIBRARIES})
target_include_directories(brick PUBLIC
                                  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                                  $<INSTALL_INTERFACE:include>
                                  ${FFMPEG_INCLUDE_DIRS}
)
set_property(TARGET brick PROPERTY CXX_STANDARD 11)
if(MSVC)
    set_target_properties(brick PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(MSVC)

install(TARGETS brick
        DESTINATION lib
        EXPORT brick-targets
        LIBRARY DESTINATION lib  # 动态库安装路径
        ARCHIVE DESTINATION lib  # 静态库安装路径
        RUNTIME DESTINATION bin  # 可执行文件安装路径
        PUBLIC_HEADER DESTINATION include)  # 头文件安装路径
install(FILES include/log/log.h DESTINATION include/log)
install(FILES include/media/ffmpeg.h
              include/media/ffmpegreader.h
              include/media/ffmpegwriter.h
              include/media/imageformatconverter.h
              include/media/playmanager.h
        DESTINATION include/media)

include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${brick_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${brick_VERSION_MINOR}")
include(CPack)

install(EXPORT brick-targets
        FILE brick-targets.cmake
        DESTINATION lib/cmake/brick)

include(CMakePackageConfigHelpers)
# generate the config file that is includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/brick-config.cmake"
  INSTALL_DESTINATION "lib/cmake"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
  )
# generate the version file for the config file
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/brick-config-version.cmake"
  VERSION "${brick_VERSION_MAJOR}.${brick_VERSION_MINOR}"
  COMPATIBILITY AnyNewerVersion
)

# install the configuration file
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/brick-config.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/brick-config-version.cmake
  DESTINATION lib/cmake/brick
  )

# generate the export targets for the build tree
# needs to be after the install(TARGETS ) command
export(EXPORT brick-targets
  FILE "${CMAKE_CURRENT_BINARY_DIR}/brick-targets.cmake"
)
