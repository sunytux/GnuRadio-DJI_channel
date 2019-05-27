#include "VehicleManager.h"

VehicleManager::VehicleManager(Vehicle* vehicle) : vehicle(vehicle) {}
VehicleManager::VehicleManager() : vehicle(NULL) {}

void VehicleManager::forceSuccessfulTakeoff(const int timeout)
{
	while(!monitoredTakeoff(timeout)){}
}

bool VehicleManager::monitoredTakeoff(const int timeout)
{
  char errorMessage[50];

  ACK::ErrorCode error = vehicle->obtainCtrlAuthority(timeout);
  if (ACK::getError(error) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(error, errorMessage);
    std::cerr << "Failed to get authority" <<std::endl;
    return false;
  }
  // Start takeoff
  error = vehicle->control->takeoff(timeout);
  if (ACK::getError(error) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(error, errorMessage);
    std::cerr << "Failed to send take off command" <<std::endl;
    return false;
  }

  if (!isIntheAir()) {
    std::cerr << "Failed to takeoff" << std::endl;
    return false;
  }

  Telemetry::GlobalPosition deltaHeight = vehicle->broadcast->getGlobalPosition();
  do
  {
    sleep(4); //sleep for 4seconds
  } while (!isStillGoingUp(deltaHeight));

  return true;
}

bool VehicleManager::isStillGoingUp(Telemetry::GlobalPosition& deltaHeight) {
  float32_t delta;
  Telemetry::GlobalPosition currentHeight = vehicle->broadcast->getGlobalPosition();

  delta = fabs(currentHeight.altitude - deltaHeight.altitude);
  deltaHeight.altitude = currentHeight.altitude;

  return delta >= 0.009;
}

bool VehicleManager::checkStatusRange(const int timeoutCycles, 
  const Status status) {
  int attemptNumber = 0;

  while ((vehicle->broadcast->getStatus().flight < status) &&
         attemptNumber < timeoutCycles)
  {
    attemptNumber++;
    usleep(100000); //sleep in micro-second
  }

  if (attemptNumber < timeoutCycles)
      return true;
  return false;
}

bool VehicleManager::isIntheAir() {
  if (!checkStatusRange(20, TAKEOFF))
    return false;
    
  for(int i=0; i<2; ++i) {
    if (!checkStatus(110, IN_AIR_STANDBY))
      return false;
  }

  std::cout << "Successful TakeOff" << std::endl;
  return true;
}


bool VehicleManager::checkStatus(const int timeoutCycles, 
  const Status status) {
  int attemptNumber = 0;

  while ((vehicle->broadcast->getStatus().flight != status) &&
         attemptNumber < timeoutCycles)
  {
    attemptNumber++;
    usleep(100000); //sleep in micro-second
  }

  if (attemptNumber < timeoutCycles)
      return true;
  return false;
}

bool VehicleManager::moveByPositionOffset(const float xOffsetDesired,
  const float yOffsetDesired, const float zOffsetDesired, 
  const float yawDesired, const float posThresholdInM, const float yawThresholdInDeg) {

  Telemetry::GlobalPosition originBroadcastGP = vehicle->broadcast->getGlobalPosition();

  int timeoutInMilSec = 10000;
  int controlFreqInHz = 50; // Hz
  int cycleTimeInMs = 1000 / controlFreqInHz;
  int cycleTimeInUs = cycleTimeInMs * 1000;
  int outOfControlBoundsTimeLimit  = 10 * cycleTimeInMs; // 10 cycles
  int withinControlBoundsTimeReqmt = 50 * cycleTimeInMs; // 50 cycles

  double yawDesiredRad = DEG2RAD * yawDesired;
  double yawThresholdInRad = DEG2RAD * yawThresholdInDeg;
  double xOffsetRemaining, yOffsetRemaining, zOffsetRemaining;

  int   elapsedTimeInMs = 0;
  int   withinBoundsCounter = 0;
  int   outOfBounds = 0;
  int speedFactor = 2;
  float xCmd, yCmd, zCmd, currentYawInRad;

  setCommands(xOffsetDesired, yOffsetDesired, zOffsetDesired, xCmd, yCmd, zCmd);

  while (elapsedTimeInMs < timeoutInMilSec)
  {
    //set flight control
    vehicle->control->positionAndYawCtrl(xCmd, yCmd, zCmd, yawDesired);

    usleep(cycleTimeInUs);
    elapsedTimeInMs += cycleTimeInMs;

    setRemainings(currentYawInRad, xOffsetRemaining, yOffsetRemaining, zOffsetRemaining,
      xOffsetDesired, yOffsetDesired, zOffsetDesired, originBroadcastGP);

    // Adjust Command
    if (std::abs(xOffsetRemaining) < speedFactor) xCmd = xOffsetRemaining;
    if (std::abs(yOffsetRemaining) < speedFactor) yCmd = yOffsetRemaining;

    //In or Out of bounds ?
    if (std::abs(xOffsetRemaining) < posThresholdInM && 
        std::abs(yOffsetRemaining) < posThresholdInM &&
        std::abs(currentYawInRad - yawDesiredRad) < yawThresholdInRad) 
        withinBoundsCounter += cycleTimeInMs; // We are within bounds; start incrementing our in-bound counter
    else 
    {
      if (withinBoundsCounter != 0)
        outOfBounds += cycleTimeInMs; // Start incrementing an out-of-bounds counter
    }

    // Reset withinBoundsCounter if necessary
    if (outOfBounds > outOfControlBoundsTimeLimit)
    {
      withinBoundsCounter = 0;
      outOfBounds         = 0;
    }
    
    // If within bounds, set flag and break
    if (withinBoundsCounter >= withinControlBoundsTimeReqmt)
      break;
  }

  if (elapsedTimeInMs >= timeoutInMilSec)
  {
    std::cerr << "Task timeout while trying to move" << std::endl;
    return ACK::FAIL;
  }
  return ACK::SUCCESS;

}

void VehicleManager::localOffsetFromGpsOffset(Telemetry::Vector3f& deltaNed,
  const Telemetry::GlobalPosition& broadcastTarget, 
  const Telemetry::GlobalPosition& broadcastOrigin) {
  double deltaLon, deltaLat;

  deltaLon = broadcastTarget.longitude - broadcastOrigin.longitude;
  deltaLat  = broadcastTarget.latitude - broadcastOrigin.latitude;
  deltaNed.x = deltaLat * C_EARTH;
  deltaNed.y = deltaLon * C_EARTH * cos(broadcastTarget.latitude);
  deltaNed.z = broadcastTarget.altitude - broadcastOrigin.altitude;
}

void VehicleManager::toEulerAngle(const Telemetry::Quaternion& quaternion, Telemetry::Vector3f& rot) {
  //quaternion = a + xi + yj + zk où i,j,k sont des imaginaires purs et a,x,y,z sont réels

  double q2sqr = quaternion.q2 * quaternion.q2; // y^2
  double t0 = -2.0 * (q2sqr + quaternion.q3 * quaternion.q3) + 1.0; // y^2 + z^2 
  double t1 = +2.0 * (quaternion.q1 * quaternion.q2 + quaternion.q0 * quaternion.q3); // x*y + a*z
  double t2 = -2.0 * (quaternion.q1 * quaternion.q3 - quaternion.q0 * quaternion.q2); // x*z - a*y
  double t3 = +2.0 * (quaternion.q2 * quaternion.q3 + quaternion.q0 * quaternion.q1); // y*z + a*x
  double t4 = -2.0 * (quaternion.q1 * quaternion.q1 + q2sqr) + 1.0; // x^2 * y^2

  //t2 in [-1:1]
  t2 = (t2 > 1.0) ? 1.0 : t2;
  t2 = (t2 < -1.0) ? -1.0 : t2;

  rot.x = asin(t2);
  rot.y = atan2(t3, t4);
  rot.z = atan2(t1, t0);
}

void VehicleManager::setCommands(const float& xOffsetDesired, const float& yOffsetDesired,
  const float& zOffsetDesired, float& xCmd, float& yCmd, float& zCmd) {

  setCommand(xOffsetDesired, xCmd);
  setCommand(yOffsetDesired, yCmd);
  zCmd = vehicle->broadcast->getGlobalPosition().height + zOffsetDesired; //Since subscription cannot give us a relative height, use broadcast.
}

void VehicleManager::setCommand(const float& offsetDesired, float& cmd) {
  int speedFactor = 2;

  if (offsetDesired > 0)
    cmd = (offsetDesired < speedFactor) ? offsetDesired : speedFactor;
  else if (offsetDesired < 0)
    cmd = (offsetDesired > -1 * speedFactor) ? offsetDesired : -1 * speedFactor;
  else
    cmd = 0;
}

void VehicleManager::setRemainings(float& currentYawInRad, double& xOffsetRemaining,
  double& yOffsetRemaining, double& zOffsetRemaining, const float xOffsetDesired,
  const float yOffsetDesired, const float zOffsetDesired, const Telemetry::GlobalPosition& originBroadcastGP) {

  Telemetry::GlobalPosition currentBroadcastGP;
  Telemetry::Vector3f localOffset, rot;
  Telemetry::Quaternion broadcastQ;

  broadcastQ = vehicle->broadcast->getQuaternion();
  toEulerAngle(broadcastQ, rot);

  currentYawInRad = rot.z;
  currentBroadcastGP = vehicle->broadcast->getGlobalPosition();
  localOffsetFromGpsOffset(localOffset, currentBroadcastGP, originBroadcastGP); //localOffSet is what we already traveled

  //! See how much farther we have to go
  xOffsetRemaining = xOffsetDesired - localOffset.x;
  yOffsetRemaining = yOffsetDesired - localOffset.y;
  zOffsetRemaining = zOffsetDesired - (-localOffset.z);
}

bool VehicleManager::monitoredLanding(int timeout)
{
  char errorMessage[50];

  // Start landing
  ACK::ErrorCode landingStatus = vehicle->control->land(timeout);
  if (ACK::getError(landingStatus) != ACK::SUCCESS)
  {
    ACK::getErrorCodeMessage(landingStatus, errorMessage);
    std::cerr << "Failed to send land command" <<std::endl;
    return false;
  }

  // First check: Landing started
  if (!checkStatusRange(20, LANDING))
      return false;

  if(!onGround())
  {
    std::cout << "Landing finished, but the aircraft is in an unexpected mode."
    "Please connect DJI GO" << std::endl;
    return false;
  }
  std::cout << "Successful landing" << std::endl;
  return true;
}

bool VehicleManager::onGround()
{
  while (vehicle->broadcast->getStatus().flight != FINISHING_LANDING)
      sleep(4);

    Telemetry::GlobalPosition gp;
    do
    {
    sleep(2);
    gp = vehicle->broadcast->getGlobalPosition();
    } while (gp.altitude != 0);

    if (gp.altitude != 0)
    return false;
  return true;
}

bool VehicleManager::makeSquare(float edgeLenght)
{
	int sleepTime = 5;
	forceSuccessfulTakeoff(10);
	sleep(sleepTime);
	moveByPositionOffset(edgeLenght, 0.0f, 0.0f, 0.0f, 1.0f, 7.0f);
	std::cout << "First edge" << std::endl;
	sleep(sleepTime);
	moveByPositionOffset( 0.0f, edgeLenght, 0.0f, 0.0f, 1.0f, 7.0f);
	std::cout << "Second edge" << std::endl;
	sleep(sleepTime);
	moveByPositionOffset(-edgeLenght, 0.0f, 0.0f, 0.0f, 1.0f, 7.0f);
	std::cout << "Third edge" << std::endl;
	sleep(sleepTime);
	moveByPositionOffset( 0.0f, -edgeLenght, 0.0f, 0.0f, 1.0f, 7.0f);
	std::cout << "Fourth edge" << std::endl;
	sleep(sleepTime);
	if(monitoredLanding(10))
	{ 
		std::cout << "Square made successfuly" << std::endl; 
		return true;
	}
	std::cout << "Failed to make square" << std::endl;
	return false;
}




