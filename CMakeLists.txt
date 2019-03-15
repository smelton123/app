cmake_minimum_required(VERSION 2.8)
project(app)
add_definitions(-std=gnu++0x)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(UV REQUIRED)

set(EXTRA_LIBS pthread rt)
set(HEADERS
    common/interfaces/vkILogBackend.h
    common/interfaces/vkIConsoleListener.h
    common/interfaces/vkISignalListener.h
    common/interfaces/vkIFsWatcherListener.h
    common/vkConsole.h
    common/vkHandle.h
    common/vkSignals.h
    common/log/vkLog.h
    io/vkFsWatcher.h
    vkServer.h
    vkBindHost.h
)
set(SRC 
    main.cpp
    app.cpp 
    common/vkConsole.cpp
    common/vkHandle.cpp
    common/vkSignals.cpp
    common/log/vkLog.cpp
    io/vkFsWatcher.cpp
    vkServer.cpp
    vkBindHost.cpp
    )
include_directories(${UV_INCLUDE_DIR})
add_executable(${CMAKE_PROJECT_NAME} ${SRC} ${HEADERS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${UV_LIBRARIES} ${EXTRA_LIBS})
