# brick
A growing cplusplus lib

## Configure and Compile

### Option 1: Compile and coding with QtCreator

1. Load project from QtCreator with the CMakeLists.txt

2. Enter "Project" page

3. Set Environment Variable:

    GLOG_ROOT: <glog root directory>

    FFMPEG_ROOT: <FFmpeg root directory>

### Option 2: Configure with CMake-GUI
1. Use CMake-GUI to load the project

2. Set CMake Cached Variable: 

    GLOG_ROOT_DIR: <glog root directory>

    FFMPEG_ROOT_DIR: <FFmpeg root directory>

### Option 3: Configure and Compile with CMake-CMD

```cmd
$> mkdir build
$> cd build
$> cmake ../ -DGLOG_ROOT_DIR=<glog root directory> -DFFMPEG_ROOT_DIR=<FFmpeg root directory> (-DCMAKE_BUILD_TYPE=Release, unix-like only) -DBUILD_SHARED_LIBS=TRUE
$> cmake --build ./ --config Release (windows only)
$> cmake --install ./ --prefix <install directory>
```

## Pack the lib with cpack

```cmd
$> cpack -G ZIP -C Release
```

## How to use the package

### Option 1: add_subdirectory(brick)

### Option 2: find_package(brick)
