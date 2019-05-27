#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

#include <iostream>
#include <unistd.h>
// System Includes
#include <cmath>

// DJI OSDK includes
#include "/opt/gnuradio/default/share/include/djiosdk/dji_status.hpp"
#include "/opt/gnuradio/default/share/include/djiosdk/dji_vehicle.hpp"

#define C_EARTH (double)6378137.0 //equatorial radius in meter
#define DEG2RAD 0.01745329252

using namespace DJI::OSDK;
using namespace DJI::OSDK::Telemetry;

class VehicleManager {
	private :
		enum Status {
		  ON_GROUND_STANDBY = 1,
		  TAKEOFF = 2,
		  IN_AIR_STANDBY = 3,
		  LANDING = 4, 
		  FINISHING_LANDING = 5
		};
		
		Vehicle* vehicle; 

		/**	Check that the altitude is stable by comparing the difference in altitude
			between the current one and the one four seconds ago with a constant value.
			Returns true if stability is reached.

			@param deltaHeight, altitude recorded four seconds ago
		*/
		bool isStillGoingUp(Telemetry::GlobalPosition& deltaHeight);

		/**	Check timeoutCycles times if the vehicule is in the desired status and sleep
			for 0.1 second between each check. Return true if it is the case, false otherwise.

			@param timeoutCycles, number of maximum iterations 
			@param status, desired status
		*/
		bool checkStatus(int timeoutCycles, VehicleManager::Status status);

		/**	Same description than checkStatus except that the desired status should be reached but the actual
			one could be any further ones.

			@param timeoutCycles, number of maximum iterations 
			@param status, status to reach
		*/
		bool checkStatusRange(int timeoutCycles, VehicleManager::Status status);

		/** Look for successful takeoff by checking status. Returns true in this case, false otherwise.
		*/
		bool isIntheAir();

		/** Compute offset in meter (from offset in degree for longitude and latitude component)

			@param deltaNed, vector containing x,y and z offset in meter
			@param broadcastTarget, vector containing altitude,longitude and latitude to target
			@param broadcastOrigin, vector containing the actual altitude,longitude and latitude 
		*/
		void localOffsetFromGpsOffset(Telemetry::Vector3f& deltaNed, const Telemetry::GlobalPosition& broadcastTarget,
		 	const Telemetry::GlobalPosition& broadcastOrigin);

		/** Set rotational angles in Euler coordonate system from quaternion one given 
			by the broadcast of the aircraft.

			@param deltaNed
			@param broadcastTarget
		 	@param broadcastOrigin
		*/
		void toEulerAngle(const Telemetry::Quaternion& quaternion, Telemetry::Vector3f& rot);

		/** Set x, y and z command. 

			@param xOffsetDesired
			@param yOffsetDesired
			@param zOffsetDesired
			@param xCmd
			@param yCmd
			@param zCmd
		*/
		void setCommands(const float& xOffsetDesired, const float& yOffsetDesired, const float& zOffsetDesired,
			float& xCmd, float& yCmd, float& zCmd);

		/** Set command to the minimum of the absolute value of offsetDesired and the speedfactor.

			@param offsetdesired
			@param cmd, command to set
		*/
		void setCommand(const float& offsetDesired, float& cmd);

		/** Compute remaining distances to travel
	
			@param currentYawInRad
			@param xOffsetRemaining
			@param yOffsetRemaining
			@param zOffsetRemaining
			@param xOffsetDesired
			@param yOffsetDesired
			@param zOffsetDesired
			@param originBroadcastGP, vector containing the actual altitude, longitude and latitude		
		*/
		void setRemainings(float& currentYawInRad, double& xOffsetRemaining, double& yOffsetRemaining,
			double& zOffsetRemaining, const float xOffsetDesired, const float yOffsetDesired,
			const float zOffsetDesired, const Telemetry::GlobalPosition& originBroadcastGP);

		/** Check if the aircraft is on the ground
		*/
		bool onGround();

	public : 
		VehicleManager(Vehicle* vehicle);
		VehicleManager();

		/** Force the take off to be successful by calling repeatedly the 
			monitoredTakeoff function.

			@param timeout, maximum amount of time waited for ACK (vehicle respond).
		*/
		void forceSuccessfulTakeoff(const int timeout);

		/** Make sure that the aircraft took off and that it is in stand by in the air.
			Returns true if it is the case, false otherwise.

			@param timeout, maximum amount of time waited for ACK (vehicle respond).
		*/
		bool monitoredTakeoff(const int timeout);

		/** Move the aircraft from the desired offset with with the corresponding acceptance interval of error
			and make sure that the position is stable.

			@param xOffsetDesired
			@param yOffsetDesired
			@param zOffsetDesired
            @param yawDesired
            @param posThresholdInM
            @param yawThresholdInDeg
		*/
		bool moveByPositionOffset(float xOffsetDesired, float yOffsetDesired, float zOffsetDesired,
                     float yawDesired, float posThresholdInM, float yawThresholdInDeg);

		/** Make sure that the aircraft landed well and that it is on the ground.
			Returns true if it is the case, false otherwise.

			@param timeout, maximum amount of time waited for ACK (vehicle respond).
		*/
		bool monitoredLanding(int timeout);

		/** Make the aircraft draw a square.

			@param edgeLenght
		*/
		bool makeSquare(float edgeLenght);

};

#endif