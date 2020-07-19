cmake_minimum_required(VERSION 3.0)

# 设置CMAKE项目名
set(PROJECT_NAME findblue)
PROJECT(${PROJECT_NAME})

# 设置项目版本C++11
set(CMAKE_CXX_FLAGS "-std=c++11")
message(STATUS "optional:-std=c++11")
MESSAGE(STATUS "support platform: ${CMAKE_SYSTEM_NAME}")

#指定编译选项 Release / Debug
# set(CMAKE_BUILD_TYPE Release )

#如果不设置编译类型，那么默认是Release
IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()
MESSAGE("Build type: " ${CMAKE_BUILD_TYPE})

#指定可执行文件的输出目录，输出到bin下面
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#指定库文件输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#设定头文件目录
include_directories( ${PROJECT_SOURCE_DIR})
include_directories( ${PROJECT_SOURCE_DIR}/armor/include)
include_directories( ${PROJECT_SOURCE_DIR}/energy/include)
include_directories( ${PROJECT_SOURCE_DIR}/others/include)
include_directories( ${PROJECT_SOURCE_DIR}/eco/include)

# 添加Opencv Thread lib
find_package(OpenCV REQUIRED)
FIND_PACKAGE(Threads)
include_directories(${OpenCV_INCLUDE_DIRS})

#在指定目录下查找库，并保存在LIBPATH变量中
find_library(LIBPATHS  shared ${PROJECT_SOURCE_DIR}/lib)

#并且把该目录设为连接目录
link_directories( ${PROJECT_SOURCE_DIR}/lib)

#获取代码，在项目中，将所有代码都放在src文件夹中
FILE(GLOB_RECURSE DIR_SRCS "others/src/*.cpp" "energy/src/*cpp" "armor/src/*.cpp")
MESSAGE(STATUS "Src file: ${DIR_SRCS}")


############################## Begin ################################
## 在include目录下生成config.h文件
option (USE_INDUSTRY_CAMERA "Use provided math implementation" OFF)
set(CAMERA_ON 1)
set(CAMERA_OF 0)
set(VAR "VAR_NEW")
configure_file (
        "${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
        "${PROJECT_SOURCE_DIR}/others/include/config.h"
)
############################ END ##################################

#添加下级目录
#add_subdirectory( test )

#add target link

ADD_DEFINITIONS(-std=c++0x -lpthread -lMVSDK -lm -lfftw3 -lfftw3f)

#Final EXE_FILES
    #Link
    # AUX_SOURCE_DIRECTORY(test TEST_SRCS)
    # MESSAGE(STATUS "Test file: ${TEST_SRCS}")
    # add_executable(${PROJECT_NAME} main.cpp ${DIR_SRCS} ${MOD_SRCS})

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
    ADD_EXECUTABLE(${PROJECT_NAME} main_single.cpp ${DIR_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS} ${CMAKE_THREAD_LIBS_INIT} ${PROJECT_SOURCE_DIR}/lib/libMVSDK.so ${PROJECT_SOURCE_DIR}/lib/libecotracker_single.so fftw3)
ELSEIF(CMAKE_SYSTEM_NAME MATCHES "Windows")
    ADD_EXECUTABLE(${PROJECT_NAME} main_single.cpp ${DIR_SRCS})
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${OpenCV_LIBS} ${PROJECT_SOURCE_DIR}/lib/MVCAMSDK_X64.lib)
ELSE()
    MESSAGE(STATUS "Unsupport platform: ${CMAKE_SYSTEM_NAME}")
endif()