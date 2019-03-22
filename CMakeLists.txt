cmake_minimum_required(VERSION 2.8)
project(app)
add_definitions(-std=gnu++0x)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(UV REQUIRED)
find_package(CURL REQUIRED)

set(EXTRA_LIBS pthread rt)
set(HEADERS
    third-party/md5/md5.h
    common/interfaces/vkILogBackend.h
    common/interfaces/vkIConsoleListener.h
    common/interfaces/vkISignalListener.h
    common/interfaces/vkIFsWatcherListener.h
    common/vkConsole.h
    common/vkHandle.h
    common/vkSignals.h
    common/vkStorage.h
    common/vkUser.h
    common/vkMd5sum.h
    common/log/vkLog.h
    common/log/vkBasicLog.h
    common/log/vkSysLog.h
    io/vkFsWatcher.h
    vkServer.h
    vkBindHost.h
)
set(SRC 
    third-party/md5/md5c.cpp
    main.cpp
    app.cpp 
    common/vkConsole.cpp
    common/vkHandle.cpp
    common/vkSignals.cpp
    common/vkUser.cpp
    common/vkMd5sum.cpp
    common/log/vkLog.cpp
    common/log/vkBasicLog.cpp
    common/log/vkSysLog.cpp
    io/vkFsWatcher.cpp
    vkServer.cpp
    vkBindHost.cpp
    )
include_directories(${UV_INCLUDE_DIR})
include_directories(${CURL_INCLUDE_DIRS})
add_executable(${CMAKE_PROJECT_NAME} ${SRC} ${HEADERS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${UV_LIBRARIES} ${CURL_LIBRARIES} ${EXTRA_LIBS})
