/*! @file telemetry_sample.cpp
 *  @version 3.3
 *  @date Jun 05 2017
 *
 *  @brief
 *  Telemetry API usage in a Linux environment.
 *  Shows example usage of the new data subscription API.
 *
 *  @Copyright (c) 2017 DJI
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "telemetry_sample.hpp"

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

bool
getBroadcastData(DJI::OSDK::Vehicle* vehicle, int responseTimeout)
{
  std::cout << "DEBUG 1" << std::endl;
  // Counters
  int elapsedTimeInMs = 0;
  int timeToPrintInMs = 2000;

  // We will listen to five broadcast data sets:
  // 1. Flight Status
  // 2. Global Position
  // 3. RC Channels
  // 4. Velocity
  // 5. Quaternion

  // Please make sure your drone is in simulation mode. You can
  // fly the drone with your RC to get different values.

  Telemetry::Status         status;
  Telemetry::GlobalPosition globalPosition;
  Telemetry::RC             rc;
  Telemetry::Vector3f       velocity;
  Telemetry::Quaternion     quaternion;

  const int TIMEOUT = 20;

  // Re-set Broadcast frequencies to their default values
  ACK::ErrorCode ack = vehicle->broadcast->setBroadcastFreqDefaults(TIMEOUT);

  std::cout << "DEBUG 2" << std::endl;

  // Print in a loop for 2 seconds
  while (elapsedTimeInMs < timeToPrintInMs)
  {
    std::cout << "DEBUG 3" << std::endl;
    // Matrice 100 broadcasts only flight status
    status         = vehicle->broadcast->getStatus();
    globalPosition = vehicle->broadcast->getGlobalPosition();
    rc             = vehicle->broadcast->getRC();
    velocity       = vehicle->broadcast->getVelocity();
    quaternion     = vehicle->broadcast->getQuaternion();
    
    std::cout << "DEBUG 4" << std::endl;
    
    std::cout << "Counter = " << elapsedTimeInMs << ":\n";
    std::cout << "-------\n";
    std::cout << "Flight Status                         = "
              << (unsigned)status.flight << "\n";
    std::cout << "Position              (LLA)           = "
              << globalPosition.latitude << ", " << globalPosition.longitude
              << ", " << globalPosition.altitude << "\n";
    std::cout << "RC Commands           (r/p/y/thr)     = " << rc.roll << ", "
              << rc.pitch << ", " << rc.yaw << ", " << rc.throttle << "\n";
    std::cout << "Velocity              (vx,vy,vz)      = " << velocity.x
              << ", " << velocity.y << ", " << velocity.z << "\n";
    std::cout << "Attitude Quaternion   (w,x,y,z)       = " << quaternion.q0
              << ", " << quaternion.q1 << ", " << quaternion.q2 << ", "
              << quaternion.q3 << "\n";
    std::cout << "-------\n\n";

    usleep(5000);
    elapsedTimeInMs += 5;
  }

  std::cout << "Done printing!\n";
  return true;
}

bool
subscribeToData(Vehicle* vehicle, int responseTimeout)
{
  // RTK can be detected as unavailable only for Flight controllers that don't support RTK
  bool rtkAvailable = false;
  // Counters
  int elapsedTimeInMs = 0;
  int timeToPrintInMs = 2000;

  // We will subscribe to six kinds of data:
  // 1. Flight Status at 1 Hz
  // 2. Fused Lat/Lon at 10Hz
  // 3. Fused Altitude at 10Hz
  // 4. RC Channels at 50 Hz
  // 5. Velocity at 50 Hz
  // 6. Quaternion at 200 Hz

  // Please make sure your drone is in simulation mode. You can fly the drone
  // with your RC to
  // get different values.

  // Telemetry: Verify the subscription
  ACK::ErrorCode subscribeStatus;
  subscribeStatus = vehicle->subscribe->verify(responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    return false;
  }

  // Package 0: Subscribe to flight status at freq 1 Hz
  int       pkgIndex        = 0;
  int       freq            = 1;
  TopicName topicList1Hz[]  = { TOPIC_STATUS_FLIGHT };
  int       numTopic        = sizeof(topicList1Hz) / sizeof(topicList1Hz[0]);
  bool      enableTimestamp = false;

  bool pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    pkgIndex, numTopic, topicList1Hz, enableTimestamp, freq);
  if (!(pkgStatus))
  {
    return pkgStatus;
  }
  subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    // Cleanup before return
    vehicle->subscribe->removePackage(pkgIndex, responseTimeout);
    return false;
  }

  // Package 1: Subscribe to Lat/Lon, and Alt at freq 10 Hz
  pkgIndex                  = 1;
  freq                      = 10;
  TopicName topicList10Hz[] = { TOPIC_GPS_FUSED, TOPIC_ALTITUDE_FUSIONED};
  numTopic                  = sizeof(topicList10Hz) / sizeof(topicList10Hz[0]);
  enableTimestamp           = false;

  pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    pkgIndex, numTopic, topicList10Hz, enableTimestamp, freq);
  if (!(pkgStatus))
  {
    return pkgStatus;
  }
  subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    // Cleanup before return
    vehicle->subscribe->removePackage(pkgIndex, responseTimeout);
    return false;
  }

  // Package 2: Subscribe to RC Channel and Velocity at freq 50 Hz
  pkgIndex                  = 2;
  freq                      = 50;
  TopicName topicList50Hz[] = { TOPIC_RC, TOPIC_VELOCITY };
  numTopic                  = sizeof(topicList50Hz) / sizeof(topicList50Hz[0]);
  enableTimestamp           = false;

  pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    pkgIndex, numTopic, topicList50Hz, enableTimestamp, freq);
  if (!(pkgStatus))
  {
    return pkgStatus;
  }
  subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    // Cleanup before return
    vehicle->subscribe->removePackage(pkgIndex, responseTimeout);
    return false;
  }

  // Package 3: Subscribe to Quaternion at freq 200 Hz.
  pkgIndex                   = 3;
  freq                       = 200;
  TopicName topicList200Hz[] = { TOPIC_QUATERNION };
  numTopic        = sizeof(topicList200Hz) / sizeof(topicList200Hz[0]);
  enableTimestamp = false;

  pkgStatus = vehicle->subscribe->initPackageFromTopicList(
    pkgIndex, numTopic, topicList200Hz, enableTimestamp, freq);
  if (!(pkgStatus))
  {
    return pkgStatus;
  }
  subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, responseTimeout);
  if (ACK::getError(subscribeStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(subscribeStatus, __func__);
    // Cleanup before return
    vehicle->subscribe->removePackage(pkgIndex, responseTimeout);
    return false;
  }

  // Package 4: Subscribe to RTK at freq 5 Hz.
  pkgIndex                   = 4;
  freq                       = 5;
  TopicName topicListRTK5Hz[] = {TOPIC_RTK_POSITION, TOPIC_RTK_YAW_INFO,
                                  TOPIC_RTK_POSITION_INFO, TOPIC_RTK_VELOCITY,
                                  TOPIC_RTK_YAW};
  numTopic        = sizeof(topicListRTK5Hz) / sizeof(topicListRTK5Hz[0]);
  enableTimestamp = false;

  pkgStatus = vehicle->subscribe->initPackageFromTopicList(
      pkgIndex, numTopic, topicListRTK5Hz, enableTimestamp, freq);
  if (!(pkgStatus))
  {
    return pkgStatus;
  }
  else {
    subscribeStatus = vehicle->subscribe->startPackage(pkgIndex, responseTimeout);
    if(subscribeStatus.data == ErrorCode::SubscribeACK::SOURCE_DEVICE_OFFLINE)
    {
      std::cout << "RTK Not Available" << "\n";
      rtkAvailable = false;
    }
    else
    {
      rtkAvailable = true;
      if (ACK::getError(subscribeStatus) != ACK::SUCCESS) {
        ACK::getErrorCodeMessage(subscribeStatus, __func__);
        // Cleanup before return
        vehicle->subscribe->removePackage(pkgIndex, responseTimeout);
        return false;
      }
    }
  }

  // Wait for the data to start coming in.
  sleep(1);

  // Get all the data once before the loop to initialize vars
  TypeMap<TOPIC_STATUS_FLIGHT>::type     flightStatus;
  TypeMap<TOPIC_GPS_FUSED>::type         latLon;
  TypeMap<TOPIC_ALTITUDE_FUSIONED>::type altitude;
  TypeMap<TOPIC_RC>::type                rc;
  TypeMap<TOPIC_VELOCITY>::type          velocity;
  TypeMap<TOPIC_QUATERNION>::type        quaternion;
  TypeMap<TOPIC_RTK_POSITION>::type      rtk;
  TypeMap<TOPIC_RTK_POSITION_INFO>::type rtk_pos_info;
  TypeMap<TOPIC_RTK_VELOCITY>::type      rtk_velocity;
  TypeMap<TOPIC_RTK_YAW>::type           rtk_yaw;
  TypeMap<TOPIC_RTK_YAW_INFO>::type      rtk_yaw_info;


  // Print in a loop for 2 sec
  while (elapsedTimeInMs < timeToPrintInMs)
  {
    flightStatus = vehicle->subscribe->getValue<TOPIC_STATUS_FLIGHT>();
    latLon       = vehicle->subscribe->getValue<TOPIC_GPS_FUSED>();
    altitude     = vehicle->subscribe->getValue<TOPIC_ALTITUDE_FUSIONED>();
    rc           = vehicle->subscribe->getValue<TOPIC_RC>();
    velocity     = vehicle->subscribe->getValue<TOPIC_VELOCITY>();
    quaternion   = vehicle->subscribe->getValue<TOPIC_QUATERNION>();
    if(rtkAvailable) {
      rtk = vehicle->subscribe->getValue<TOPIC_RTK_POSITION>();
      rtk_pos_info = vehicle->subscribe->getValue<TOPIC_RTK_POSITION_INFO>();
      rtk_velocity = vehicle->subscribe->getValue<TOPIC_RTK_VELOCITY>();
      rtk_yaw = vehicle->subscribe->getValue<TOPIC_RTK_YAW>();
      rtk_yaw_info = vehicle->subscribe->getValue<TOPIC_RTK_YAW_INFO>();
    }
    std::cout << "Counter = " << elapsedTimeInMs << ":\n";
    std::cout << "-------\n";
    std::cout << "Flight Status                         = " << (int)flightStatus
              << "\n";
    std::cout << "Position              (LLA)           = " << latLon.latitude
              << ", " << latLon.longitude << ", " << altitude << "\n";
    std::cout << "RC Commands           (r/p/y/thr)     = " << rc.roll << ", "
              << rc.pitch << ", " << rc.yaw << ", " << rc.throttle << "\n";
    std::cout << "Velocity              (vx,vy,vz)      = " << velocity.data.x
              << ", " << velocity.data.y << ", " << velocity.data.z << "\n";
    std::cout << "Attitude Quaternion   (w,x,y,z)       = " << quaternion.q0
              << ", " << quaternion.q1 << ", " << quaternion.q2 << ", "
              << quaternion.q3 << "\n";
    if(rtkAvailable) {
      std::cout << "RTK if available   (lat/long/alt/velocity_x/velocity_y/velocity_z/yaw/yaw_info/pos_info) ="
                << rtk.latitude << "," << rtk.longitude << "," << rtk.HFSL << "," << rtk_velocity.x << ","
                << rtk_velocity.y
                << "," << rtk_velocity.z << "," << rtk_yaw << "," << rtk_yaw_info << rtk_pos_info << "\n";
    }
    std::cout << "-------\n\n";
    usleep(5000);
    elapsedTimeInMs += 5;
  }

  std::cout << "Done printing!\n";
  vehicle->subscribe->removePackage(0, responseTimeout);
  vehicle->subscribe->removePackage(1, responseTimeout);
  vehicle->subscribe->removePackage(2, responseTimeout);
  vehicle->subscribe->removePackage(3, responseTimeout);
  vehicle->subscribe->removePackage(4, responseTimeout);

  return true;
}
