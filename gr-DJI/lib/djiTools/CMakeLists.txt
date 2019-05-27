set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -g -O0")

if(NOT ONBOARDSDK_SOURCE)
  set(ONBOARDSDK_SOURCE "/opt/COTS/Onboard-SDK/osdk-core")
endif()

include_directories(${ONBOARDSDK_SOURCE}/api/inc)
include_directories(${ONBOARDSDK_SOURCE}/utility/inc)
include_directories(${ONBOARDSDK_SOURCE}/hal/inc)
include_directories(${ONBOARDSDK_SOURCE}/protocol/inc)
include_directories(${ONBOARDSDK_SOURCE}/platform/linux/inc)
include_directories(${ONBOARDSDK_SOURCE}/../sample/linux/common)

FILE(GLOB DJITOOLS_SOURCE_FILES *.hpp *.cpp
	    ${ONBOARDSDK_SOURCE}/../sample/linux/common/dji_linux_environment.cpp
	    ${ONBOARDSDK_SOURCE}/../sample/linux/common/dji_linux_helpers.cpp
	    )
add_library(djiTools ${DJITOOLS_SOURCE_FILES})
