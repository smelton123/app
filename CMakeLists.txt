cmake_minimum_required(VERSION 2.8)
project(app)
#add_definitions(-std=gnu++0x)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${PROJECT_SOURCE_DIR}/3rd-libs/include -m64 -Wall -O2")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -g -Wall  -O2")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
find_package(UV REQUIRED)
#SET(CMAKE_EXE_LINKER_FLAGS  "-I/home/smart/thirda/include -Wl,-Bdynamic -pthread -ldl ${CMAKE_EXE_LINKER_FLAGS}")
#find_package(CURL REQUIRED)


set(HEADERS
    third-party/md5/md5.h
    common/interfaces/vkILogBackend.h
    common/interfaces/vkIConsoleListener.h
    #common/interfaces/vkISignalListener.h
    #common/interfaces/vkIFsWatcherListener.h
    common/vkConsole.h
    common/vkHandle.h
    common/vkSignals.h
    #common/vkStorage.h
    #common/vkUser.h
    common/vkMd5sum.h
    #common/log/vkLog.h
    #common/log/vkBasicLog.h
    #common/log/vkSysLog.h
    common/vkSingleApp.h
    common/vkUpgradeWorker.h
    common/vkProcess.h
    #io/vkFsWatcher.h
    io/vkPsWatcher.h
    io/vkCpuUsage.h
    vkVersion.h
    #vkServer.h
    vkSummary.h
    #vkBindHost.h
)
set(SRC 
    third-party/md5/md5c.cpp
    main.cpp
    app.cpp 
    common/vkConsole.cpp
    common/vkHandle.cpp
    common/vkSignals.cpp
    #common/vkUser.cpp
    common/vkMd5sum.cpp
    #common/log/vkLog.cpp
    #common/log/vkBasicLog.cpp
    #common/log/vkSysLog.cpp
    common/vkSingleApp.cpp
    common/vkUpgradeWorker.cpp
    common/vkProcess.cpp
    #io/vkFsWatcher.cpp
    io/vkCpuUsage.cpp
    io/vkPsWatcher.cpp
    #vkServer.cpp
    #vkBindHost.cpp
    vkSummary.cpp
)
include_directories(${UV_INCLUDE_DIR})
include_directories(${CURL_INCLUDE_DIRS})
add_executable(${CMAKE_PROJECT_NAME} ${SRC} ${HEADERS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${UV_LIBRARIES} "${PROJECT_SOURCE_DIR}/3rd-libs/lib/libcurl.a" "${PROJECT_SOURCE_DIR}/3rd-libs/lib/libssl.a" "${PROJECT_SOURCE_DIR}/3rd-libs/lib/libcrypto.a" "-Wl,-Bdynamic -pthread -ldl" )




if (WITH_VAR_SHOW)
message(STATUS "CMAKE_C_FLAGS = " ${CMAKE_C_FLAGS})
message(STATUS "CMAKE_C_FLAGS_DEBUG = " ${CMAKE_C_FLAGS_DEBUG})
message(STATUS "CMAKE_C_FLAGS_MINSIZEREL = " ${CMAKE_C_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_C_FLAGS_RELEASE = " ${CMAKE_C_FLAGS_RELEASE})
message(STATUS "CMAKE_C_FLAGS_RELWITHDEBINFO = " ${CMAKE_C_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_CXX_FLAGS = " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_CXX_FLAGS_DEBUG = " ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "CMAKE_CXX_FLAGS_MINSIZEREL = " ${CMAKE_CXX_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_CXX_FLAGS_RELEASE = " ${CMAKE_CXX_FLAGS_RELEASE})
message(STATUS "CMAKE_CXX_FLAGS_RELWITHDEBINFO = " ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_EXE_LINKER_FLAGS = " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_DEBUG = " ${CMAKE_EXE_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELEASE = " ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_MODULE_LINKER_FLAGS = " ${CMAKE_MODULE_LINKER_FLAGS})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_DEBUG = " ${CMAKE_MODULE_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_MODULE_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_RELEASE = " ${CMAKE_MODULE_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_SHARED_LINKER_FLAGS = " ${CMAKE_SHARED_LINKER_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_DEBUG = " ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_RELEASE = " ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO})

message(STATUS "CMAKE_STATIC_LINKER_FLAGS = " ${CMAKE_STATIC_LINKER_FLAGS})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_DEBUG = " ${CMAKE_STATIC_LINKER_FLAGS_DEBUG})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL = " ${CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_RELEASE = " ${CMAKE_STATIC_LINKER_FLAGS_RELEASE})
message(STATUS "CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO = " ${CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO})
message(STATUS "PROJECT_SOURCE_DIR = " ${PROJECT_SOURCE_DIR})
endif()