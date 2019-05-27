# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})

list(APPEND DJI_sources
	djiTools/dji_ack.cpp
    	djiTools/dji_aes.cpp
   	djiTools/dji_broadcast.cpp
	djiTools/dji_camera.cpp
	djiTools/dji_circular_buffer.cpp
	djiTools/dji_command.cpp
	djiTools/dji_control.cpp
	djiTools/dji_error.cpp
	djiTools/dji_gimbal.cpp
	djiTools/dji_hard_driver.cpp
	djiTools/dji_hardware_sync.cpp
	djiTools/dji_hotpoint.cpp
	djiTools/dji_linux_environment.cpp
	djiTools/dji_linux_helpers.cpp
	djiTools/dji_log.cpp
	djiTools/dji_memory.cpp
	djiTools/dji_memory_default.cpp
	djiTools/dji_mfio.cpp
	djiTools/dji_mission_manager.cpp
	djiTools/dji_mobile_communication.cpp
	djiTools/dji_open_protocol.cpp
	djiTools/dji_platform_manager.cpp
	djiTools/dji_protocol_base.cpp
	djiTools/dji_singleton.cpp
	djiTools/dji_subscription.cpp
	djiTools/dji_thread_manager.cpp
	djiTools/dji_vehicle.cpp
	djiTools/dji_version.cpp
	djiTools/dji_virtual_rc.cpp
	djiTools/dji_waypoint.cpp
	djiTools/flight_control_sample.cpp
	djiTools/linux_serial_device.cpp
	djiTools/posix_thread.cpp
	djiTools/posix_thread_manager.cpp
	djiTools/telemetry_sample.cpp
    VehicleManager.cpp
    sender_impl.cc
    aircraft_impl.cc
)

set(DJI_sources "${DJI_sources}" PARENT_SCOPE)
if(NOT DJI_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT DJI_sources)

add_library(gnuradio-DJI SHARED ${DJI_sources})
target_link_libraries(gnuradio-DJI ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-DJI PROPERTIES DEFINE_SYMBOL "gnuradio_DJI_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-DJI PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-DJI RUNTIME_COMPONENT "DJI_runtime" DEVEL_COMPONENT "DJI_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_DJI_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_DJI.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_DJI.cc
)

add_executable(test-DJI ${test_DJI_sources})

target_link_libraries(
  test-DJI
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-DJI
)

GR_ADD_TEST(test_DJI test-DJI)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

