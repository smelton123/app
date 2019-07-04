project(app)
cmake_minimum_required(VERSION 2.8)

#if compiler does not support standard C++11, open the follow flag
#add_definitions(-std=gnu++0x)

#set C++ language Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
#flag for building c code
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections")   # for reducing binary size
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")                                    # for strip binary
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m64 -g -Wall  -O2")

#flag for building c++ code
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -O2")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -Wall -O2")

#link flag for reducing the binary's size 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-section") # for reducing binary size

#set include folder
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd-libs/include)

#set link folder
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rd-libs/lib)

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



add_executable(${CMAKE_PROJECT_NAME} ${SRC} ${HEADERS})
target_link_libraries(${CMAKE_PROJECT_NAME} "-Wl,--gc-section" curl) # link libuv.a
target_link_libraries(${CMAKE_PROJECT_NAME} "-Wl,--gc-section" uv)   # link libuv.a
target_link_libraries(${CMAKE_PROJECT_NAME} "-Wl,--gc-section" ssl)  # link libssl.a
target_link_libraries(${CMAKE_PROJECT_NAME} "-Wl,--gc-section" crypto) # link libcrypto.a
target_link_libraries(${CMAKE_PROJECT_NAME} "-Wl,-Bdynamic -pthread -ldl" "-lrt")


if (0)
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
